/* Vivid Dolls port - jpeg64 photo decoder. See vd_jpeg64.h. */
#include <math.h>
#include <string.h>
#include "vd_jpeg64.h"
#include "vd_assets.h"
#include "vd_jpeg64_tables.h"

/* float constants exactly as stored in the game code */
static float fbits(u32 b) { float f; memcpy(&f, &b, 4); return f; }

/* ------------------------------------------------------------ bit reader -- */
/* Bits come least-significant first out of a 24-bit little-endian window. */
typedef struct {
    const u8 *buf;
    u32 size;
    s32 pos;              /* bit position (game: D_800A6C90) */
} BitReader;

static u32 window24(const BitReader *br)
{
    u32 i = (u32)(br->pos >> 3);
    u32 b0 = i < br->size ? br->buf[i] : 0;
    u32 b1 = i + 1 < br->size ? br->buf[i + 1] : 0;
    u32 b2 = i + 2 < br->size ? br->buf[i + 2] : 0;
    return b0 | (b1 << 8) | (b2 << 16);
}

static u32 get_bit(BitReader *br)                 /* Jpeg64_GetBit */
{
    u32 v = window24(br) >> (br->pos & 7);
    br->pos++;
    return v & 1;
}

static u32 get_bits(BitReader *br, u32 n)         /* Jpeg64_GetBits */
{
    u32 v = window24(br) >> (br->pos & 7);
    u32 r = 0;
    br->pos += (s32)n;
    while (n--) {                                  /* first bit read = MSB */
        r = (r << 1) | (v & 1);
        v = (v & 0xFFFF) >> 1;
    }
    return r & 0xFFFF;
}

/* ---------------------------------------------------------------- Huffman -- */
typedef struct {
    const u16 *valptr, *mincode, *maxcode;
    const u8 *huffval;
} Huff;

static const Huff huff_dc[2] = {
    { vd_j64_dc_luma_valptr, vd_j64_dc_luma_mincode, vd_j64_dc_luma_maxcode, vd_j64_dc_luma_huffval },
    { vd_j64_dc_chroma_valptr, vd_j64_dc_chroma_mincode, vd_j64_dc_chroma_maxcode, vd_j64_dc_chroma_huffval },
};
static const Huff huff_ac[2] = {
    { vd_j64_ac_luma_valptr, vd_j64_ac_luma_mincode, vd_j64_ac_luma_maxcode, vd_j64_ac_luma_huffval },
    { vd_j64_ac_chroma_valptr, vd_j64_ac_chroma_mincode, vd_j64_ac_chroma_maxcode, vd_j64_ac_chroma_huffval },
};

static u32 huff_decode(BitReader *br, const Huff *h)
{
    u32 code = get_bit(br);
    u32 len = 1;
    /* maxcode 0xFFFF = no codes of this length */
    while (len < 17 && ((code & 0xFFFF) > h->maxcode[len] || h->maxcode[len] == 0xFFFF)) {
        len++;
        code = (code << 1) | get_bit(br);
    }
    if (len >= 17) return 0;                      /* corrupt stream */
    return h->huffval[h->valptr[len] + (code & 0xFFFF) - h->mincode[len]];
}

static s32 extend(u32 v, u32 n)
{
    if (n == 0) return 0;
    if ((v >> (n - 1)) & 1) return (s32)v;
    return (s32)v + 1 - (1 << n);
}

/* ------------------------------------------------------------------- info -- */
int vd_jpeg64_info(const u8 *siff, u32 size, VdJpeg64Info *o)
{
    u32 n;
    const u8 *t = vd_siff_find(siff, size, "type", &n);
    const u8 *xw = vd_siff_find(siff, size, "xw  ", NULL);
    const u8 *yw = vd_siff_find(siff, size, "yw  ", NULL);
    const u8 *sc = vd_siff_find(siff, size, "sc  ", NULL);
    const u8 *d  = vd_siff_find(siff, size, "data", &o->data_size);
    if (!t || memcmp(t, "jgx", 3) != 0 || !xw || !yw || !sc || !d) return -1;
    o->width = vd_be32(xw);
    o->height = vd_be32(yw);
    o->scale = vd_be32(sc);
    o->data = d;
    return 0;
}

/* ----------------------------------------------------------------- decode -- */
static float idct_tab[8][8];                       /* game: D_800A6C98 [u][x] */
static int idct_ready;

static void idct_init(void)                        /* part of Jpeg64_Init */
{
    /* NOTE: the game calls its own libultra cosf (func_8001C450); the host cosf
     * may differ by 1 ulp in rare cases. Swap in the decompiled cosf for
     * guaranteed bit-exact output. */
    for (int x = 0; x < 8; x++) idct_tab[0][x] = fbits(0x3EB504F3);
    for (int u = 1; u < 8; u++)
        for (int x = 0; x < 8; x++)
            idct_tab[u][x] = cosf((((float)x + (float)x) + 1.0f) * (float)u * fbits(0x40490FDB) / 16.0f) * 0.5f;
    idct_ready = 1;
}

static s16 round_half_away(float f)
{
    return (s16)(s32)(f < 0.0f ? f - 0.5f : f + 0.5f);
}

/* coef: 8x8 dequantised coefficients (row = vertical frequency) */
static void idct8x8(const s16 coef[64], s16 out[64])          /* Jpeg64_Idct */
{
    float tmp[8][8];
    for (int r = 0; r < 8; r++) {                  /* rows: tmp = coef * A */
        for (int x = 0; x < 8; x++)
            tmp[r][x] = (float)coef[r * 8] * idct_tab[0][x];
        for (int u = 1; u < 8; u++)
            for (int x = 0; x < 8; x++)
                tmp[r][x] = tmp[r][x] + (float)coef[r * 8 + u] * idct_tab[u][x];
    }
    for (int y = 0; y < 8; y++) {                  /* columns: out = A^T * tmp */
        float acc[8];
        for (int x = 0; x < 8; x++) acc[x] = idct_tab[0][y] * tmp[0][x];
        for (int v = 1; v < 8; v++)
            for (int x = 0; x < 8; x++) acc[x] = acc[x] + idct_tab[v][y] * tmp[v][x];
        for (int x = 0; x < 8; x++) out[y * 8 + x] = round_half_away(acc[x]);
    }
}

static int clamp255(s32 v) { return v < 0 ? 0 : v > 255 ? 255 : v; }

u32 vd_jpeg64_decode(const VdJpeg64Info *info, u16 *out, u32 stride)
{
    BitReader br = { info->data, info->data_size, 0 };
    s16 pred[6] = { 0 };                           /* one DC predictor per block slot */
    const float inv_sc = 1.0f / (float)(s16)info->scale;

    const float k_rv = fbits(0x3FB374BC), k_gu = fbits(0x3EB02DE0), k_gv = fbits(0x3F36C227);
    const float k_bu = fbits(0x3FE2CA58), k_bv = fbits(0x3A9D4952);

    if (!idct_ready) idct_init();
    if ((info->width | info->height) & 15) return 0;

    for (u32 my = 0; my < info->height; my += 16) {
        for (u32 mx = 0; mx < info->width; mx += 16) {
            s16 pix[16 * 16][3];                   /* Y, Cb, Cr per pixel */

            for (int b = 0; b < 6; b++) {
                const int ch = b < 4 ? 0 : 1;
                s16 zz[64] = { 0 };
                s16 deq[64], spat[64];
                u32 k = 1;

                /* DC */
                u32 n = huff_decode(&br, &huff_dc[ch]);
                s32 diff = extend(n ? get_bits(&br, n) : 0, n);
                zz[0] = (s16)(pred[b] + diff);
                pred[b] = (s16)(pred[b] + diff);

                /* AC: (run, size) symbols, 0x00 = end of block */
                while (k < 64) {
                    u32 s = huff_decode(&br, &huff_ac[ch]);
                    u32 run = s >> 4, size = s & 15;
                    s32 level = size ? extend(get_bits(&br, size), size) : 0;
                    if (run == 0 && level == 0) break;
                    k += run;
                    if (k < 64) zz[vd_j64_zigzag[k]] = (s16)level;
                    k++;
                }

                /* dequantise (natural order, table / scale, truncate) */
                for (int i = 0; i < 64; i++) {
                    float q = (ch ? vd_j64_quant_chroma[i] : vd_j64_quant_luma[i]) * inv_sc;
                    deq[i] = (s16)(s32)((float)zz[i] * q);
                }
                idct8x8(deq, spat);

                /* upsample into the 16x16 macroblock */
                if (b < 4) {
                    int ox = (b & 1) * 8, oy = (b >> 1) * 8;
                    for (int y = 0; y < 8; y++)
                        for (int x = 0; x < 8; x++)
                            pix[(oy + y) * 16 + ox + x][0] = (s16)(spat[y * 8 + x] + 128);
                } else {
                    int c = b - 3;                 /* 1 = Cb, 2 = Cr */
                    for (int y = 0; y < 8; y++)
                        for (int x = 0; x < 8; x++) {
                            s16 v = spat[y * 8 + x];
                            pix[(2 * y) * 16 + 2 * x][c] = v;
                            pix[(2 * y) * 16 + 2 * x + 1][c] = v;
                            pix[(2 * y + 1) * 16 + 2 * x][c] = v;
                            pix[(2 * y + 1) * 16 + 2 * x + 1][c] = v;
                        }
                }
            }

            /* YCbCr -> RGBA5551 (Jpeg64_YCbCrToRgba5551) */
            for (int i = 0; i < 256; i++) {
                float Y = (float)pix[i][0], cb = (float)pix[i][1], cr = (float)pix[i][2];
                float fr = Y + cr * k_rv;
                float fg = (Y - cb * k_gu) - cr * k_gv;
                float fb = (Y + cb * k_bu) - cr * k_bv;
                int r = clamp255(round_half_away(fr));
                int g = clamp255(round_half_away(fg));
                int bl = clamp255(round_half_away(fb));
                out[(my + i / 16) * stride + mx + i % 16] =
                    (u16)((vd_j64_to5bit[r] << 11) | (vd_j64_to5bit[g] << 6) | (vd_j64_to5bit[bl] << 1) | 1);
            }
        }
    }
    return (u32)((br.pos + 7) >> 3);
}
