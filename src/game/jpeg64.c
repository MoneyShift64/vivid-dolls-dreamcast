/* Vivid Dolls - "jpeg64 Ver0.00" photo codec. See jpeg64.h.
 * Every function keeps the original's exact float operation order. */
#include "jpeg64.h"
#include "heap.h"
#include "siff.h"
#include "libm.h"

extern Heap gGameHeap;

/* decoder state / work buffers (game BSS) */
extern s32 D_800A6C90;              /* bit position in the stream */
extern u8 *D_800A6C94;              /* bitstream */
extern f32 D_800A6C98[8][8];        /* cos table [u][x] */
extern f32 D_800A6D98[8][8];        /* cos table transposed [x][u] */
extern u16 D_800A6E98[256];         /* RGBA5551 macroblock */
extern s16 D_800A70A0[256 * 3];     /* Y,Cb,Cr per macroblock pixel */
extern s16 D_800A76A0[6 * 64];      /* spatial blocks */
extern s16 D_800A79A0[6 * 64];      /* dequantised blocks */
extern s16 D_800A7CA0[6 * 64];      /* coefficients */
extern s16 D_800A7FA0[6];           /* DC predictors */
#define sBitPos   D_800A6C90
#define sBitBuf   D_800A6C94
#define sCos      D_800A6C98
#define sCosT     D_800A6D98
#define sRgbaMb   D_800A6E98
#define sPixMb    D_800A70A0
#define sSpatial  D_800A76A0
#define sDequant  D_800A79A0
#define sCoef     D_800A7CA0
#define sDcPred   D_800A7FA0

/* ROM tables */
extern s32 gJpeg64Zigzag[64];
extern f32 gJpeg64QuantLuma[64];
extern f32 gJpeg64QuantChroma[64];
extern Jpeg64Huff gJpeg64HuffDcLuma, gJpeg64HuffDcChroma, gJpeg64HuffAcLuma, gJpeg64HuffAcChroma;
extern u8  D_8003A014[256];         /* 8-bit -> 5-bit colour table */
extern u32 D_80039FD0[17];          /* (1 << n) - 1 bit masks */
extern u16 D_800392D0[16], D_800392F0[16];   /* encoder DC luma codes / lengths */
extern u16 D_80039310[16], D_80039330[16];   /* encoder DC chroma codes / lengths */
extern u16 D_80039350[256], D_80039550[256]; /* encoder AC luma codes / lengths */
extern u16 D_80039750[256], D_80039950[256]; /* encoder AC chroma codes / lengths */

/* round half away from zero, as the game does it everywhere */
static inline s32 Jpeg64_Round(f32 f)
{
    return (s32)(f < 0.0f ? f - 0.5f : f + 0.5f);
}

/* @ 0x80019D70 */
/* Decodes the jpeg64 Siff file at rom_addr into out (a 16-bit image whose
 * row stride is the photo width), placing it at (x, y). Returns the photo
 * size through width/height. The whole file is loaded into a heap buffer;
 * the bitstream starts at file offset 0x58. */
void Jpeg64_Decode(u32 rom_addr, u16 *out, s32 x, s32 y, s32 *width, s32 *height)
{
    u32 *hdr;
    u8 *file;
    s32 w, h, mx, my, row, col;
    u16 scale;

    hdr = Heap_AllocFront(&gGameHeap, 0x50);
    Rom_Read(rom_addr, hdr, 0x50);
    /* endian: the header is ROM data (big-endian) */
    file = Heap_AllocFront(&gGameHeap, BE32((u8 *)hdr + 4) + 8);
    w = BE32((u8 *)hdr + 0x34);            /* "xw  " chunk value */
    h = BE32((u8 *)hdr + 0x40);            /* "yw  " chunk value */
    scale = BE16((u8 *)hdr + 0x4E);        /* low half of "sc  " */
    Rom_Read(rom_addr, file, BE32((u8 *)hdr + 4) + 8);
    Heap_Free(&gGameHeap, hdr);
    *width = w;
    *height = h;
    Jpeg64_Init(file + 0x58);
    for (row = 5; row >= 0; row--) {
        sDcPred[row] = 0;
    }
    for (my = 0; my < h; my += 16) {
        for (mx = 0; mx < w; mx += 16) {
            u16 *src;
            Jpeg64_DecodeMacroblock(sCoef, sDcPred);
            Jpeg64_Dequant(sDequant, sCoef, 1.0f / (f32)(s16)scale);
            Jpeg64_Idct(sSpatial, sDequant);
            Jpeg64_Upsample(sPixMb, sSpatial);
            Jpeg64_YCbCrToRgba5551(sRgbaMb, sPixMb);
            src = sRgbaMb;
            for (row = 0; row < 16; row++) {
                for (col = 0; col < 16; col++) {
                    out[(my + row + y) * w + mx + col + x] = src[col];
                }
                src += 16;
            }
        }
    }
    Heap_Free(&gGameHeap, file);
}

/* @ 0x80019FAC */
/* Starts reading the bitstream at data and builds the IDCT cosine tables:
 * C[0][x] = 1/(2*sqrt(2)), C[u][x] = cos((2x+1)*u*pi/16) / 2, plus the
 * transposed copy. */
void Jpeg64_Init(u8 *data)
{
    s32 u, x;

    sBitPos = 0;
    sBitBuf = data;
    for (x = 0; x < 8; x++) {
        sCos[0][x] = 0.35355338f;
        sCosT[x][0] = 0.35355338f;
    }
    for (u = 1; u < 8; u++) {
        f32 fu = (f32)u;
        for (x = 0; x < 8; x++) {
            f32 fx = (f32)x;
            f32 c = cosf((((fx + fx) + 1.0f) * fu * 3.1415927f) / 16.0f) * 0.5f;
            sCos[u][x] = c;
            sCosT[x][u] = c;
        }
    }
}

/* @ 0x8001A118 */
/* Current bit position in the stream (encoder: number of bits written). */
s32 Jpeg64_GetBitPos(void)
{
    return sBitPos;
}

/* @ 0x8001A124 */
/* Encoder: 256 RGB888 pixels -> Y,Cb,Cr triples (Cb/Cr centred on 0). */
void Jpeg64_RgbToYCbCr(s16 *pix, u8 *rgb)
{
    s32 i;

    for (i = 255; i != -1; i--) {
        f32 r = (f32)rgb[0], g = (f32)rgb[1], b = (f32)rgb[2];
        pix[0] = (s16)Jpeg64_Round((r * 0.299f + g * 0.587f) + b * 0.114f);
        pix[1] = (s16)Jpeg64_Round((r * -0.1684f - g * 0.3316f) + b * 0.5f);
        pix[2] = (s16)Jpeg64_Round((r * 0.5f - g * 0.4187f) - b * 0.0813f);
        rgb += 3;
        pix += 3;
    }
}

/* @ 0x8001A2D4 */
/* 256 Y,Cb,Cr triples -> RGBA5551 (alpha bit always set). */
void Jpeg64_YCbCrToRgba5551(u16 *out, s16 *pix)
{
    s32 i;

    for (i = 255; i != -1; i--) {
        f32 cr = (f32)pix[2];
        f32 y = (f32)pix[0];
        f32 cb = (f32)pix[1];
        s32 r = Jpeg64_Round(y + cr * 1.402f);
        s32 g = Jpeg64_Round((y - cb * 0.3441f) - cr * 0.7139f);
        s32 b = Jpeg64_Round((y + cb * 1.7718f) - cr * 0.0012f);

        if (r < 0) r = 0; else if (r >= 256) r = 255;
        if (g < 0) g = 0; else if (g >= 256) g = 255;
        if (b < 0) b = 0; else if (b >= 256) b = 255;
        pix += 3;
        *out++ = (D_8003A014[r] << 11) + (D_8003A014[g] << 6) + (D_8003A014[b] << 1) + 1;
    }
}

/* @ 0x8001A4A0 */
/* Encoder: splits a 16x16 Y,Cb,Cr macroblock into 6 blocks. Y is shifted by
 * -128; chroma just takes the top-left pixel of every 2x2 square. */
void Jpeg64_Downsample(s16 *blocks, s16 *pix)
{
    s32 r, c;

    for (r = 0; r < 8; r++) {
        for (c = 0; c < 8; c++) {
            s16 *o = blocks + r * 8 + c;
            s16 *p = pix + (r * 16 + c) * 3;
            s16 *q = pix + (2 * r * 16 + 2 * c) * 3;
            o[0]   = p[0] - 0x80;
            o[64]  = p[8 * 3] - 0x80;
            o[128] = p[128 * 3] - 0x80;
            o[192] = p[(128 + 8) * 3] - 0x80;
            o[256] = q[1];
            o[320] = q[2];
        }
    }
}

/* @ 0x8001A530 */
/* Builds a 16x16 Y,Cb,Cr macroblock from 6 decoded blocks: Y + 128, each
 * chroma sample doubled 2x2. */
void Jpeg64_Upsample(s16 *pix, s16 *blocks)
{
    s32 r, c;

    for (r = 0; r < 8; r++) {
        for (c = 0; c < 8; c++) {
            s16 *b = blocks + r * 8 + c;
            s16 *p = pix + (r * 16 + c) * 3;
            s16 *q = pix + (2 * r * 16 + 2 * c) * 3;
            s16 v;
            p[0]              = b[0] + 0x80;
            p[8 * 3]          = b[64] + 0x80;
            p[128 * 3]        = b[128] + 0x80;
            p[(128 + 8) * 3]  = b[192] + 0x80;
            v = b[256];                    /* Cb */
            q[(16 + 1) * 3 + 1] = v;
            q[16 * 3 + 1]       = v;
            q[1 * 3 + 1]        = v;
            q[1]                = v;
            v = b[320];                    /* Cr */
            q[(16 + 1) * 3 + 2] = v;
            q[16 * 3 + 2]       = v;
            q[1 * 3 + 2]        = v;
            q[2]                = v;
        }
    }
}

/* Separable 8x8 matrix transform of 6 blocks, shared by the FDCT and the IDCT
 * (the original has two copies that differ only in which table is used where):
 *   tmp[r][x] = sum_u in[r][u] * rowTab[u][x]
 *   out[y][x] = round(sum_v colTab[y][v] * tmp[v][x])
 * Sums run from index 0 upwards, exactly as in the original. */
static void Jpeg64_Transform(s16 *out, s16 *in, f32 (*rowTab)[8], f32 (*colTab)[8])
{
    s32 blk, r, u, x, y;
    f32 tmp[8][8];

    for (blk = 0; blk < 6; blk++) {
        for (r = 0; r < 8; r++) {
            f32 c = (f32)in[r * 8];
            for (x = 0; x < 8; x++) {
                tmp[r][x] = c * rowTab[0][x];
            }
            for (u = 1; u < 8; u++) {
                c = (f32)in[r * 8 + u];
                for (x = 0; x < 8; x++) {
                    tmp[r][x] = tmp[r][x] + c * rowTab[u][x];
                }
            }
        }
        for (y = 0; y < 8; y++) {
            f32 acc[8];
            f32 k = colTab[y][0];
            for (x = 0; x < 8; x++) {
                acc[x] = k * tmp[0][x];
            }
            for (u = 1; u < 8; u++) {
                k = colTab[y][u];
                for (x = 0; x < 8; x++) {
                    acc[x] = acc[x] + k * tmp[u][x];
                }
            }
            for (x = 0; x < 8; x++) {
                out[y * 8 + x] = (s16)Jpeg64_Round(acc[x]);
            }
        }
        out += 64;
        in += 64;
    }
}

/* @ 0x8001A5D8 */
/* Encoder: forward DCT of 6 blocks (out = C * in * C^T). */
void Jpeg64_Fdct(s16 *out, s16 *in)
{
    Jpeg64_Transform(out, in, sCosT, sCos);
}

/* @ 0x8001AA40 */
/* Inverse DCT of 6 blocks (out = C^T * in * C), rounded half away from zero. */
void Jpeg64_Idct(s16 *out, s16 *in)
{
    Jpeg64_Transform(out, in, sCos, sCosT);
}

/* @ 0x8001AEA8 */
/* Encoder: quantise 6 blocks (blocks 0-3 luma table, 4-5 chroma), dividing by
 * table * scale and rounding half away from zero; exact zeros stay zero. */
void Jpeg64_Quantize(s16 *out, s16 *in, f32 scale)
{
    s32 blk, i;

    for (blk = 0; blk < 6; blk++) {
        f32 *q = blk < 4 ? gJpeg64QuantLuma : gJpeg64QuantChroma;
        for (i = 0; i < 64; i++) {
            f32 v = (f32)in[i];
            if (v == 0.0f) {
                out[i] = 0;
            } else if (v < 0.0f) {
                out[i] = (s16)(s32)(v / (q[i] * scale) - 0.5f);
            } else {
                out[i] = (s16)(s32)(v / (q[i] * scale) + 0.5f);
            }
        }
        out += 64;
        in += 64;
    }
}

/* @ 0x8001AFB0 */
/* Dequantise 6 blocks: out = trunc(in * (table * scale)), blocks 0-3 with the
 * luma table, 4-5 with the chroma table. */
void Jpeg64_Dequant(s16 *out, s16 *in, f32 scale)
{
    s32 blk, i;

    for (blk = 0; blk < 6; blk++) {
        f32 *q = blk < 4 ? gJpeg64QuantLuma : gJpeg64QuantChroma;
        for (i = 0; i < 64; i++) {
            out[i] = (s16)(s32)((f32)in[i] * (q[i] * scale));
        }
        out += 64;
        in += 64;
    }
}

/* @ 0x8001B064 */
/* Encoder: Huffman-codes one macroblock (6 quantised blocks, natural order).
 * Runs of 16+ zeros emit ZRL (0/15) symbols; trailing zeros emit EOB, but no
 * EOB is written when coefficient 63 is non-zero. */
void Jpeg64_EncodeMacroblock(s16 *coef, s16 *pred)
{
    s16 b, k, run;

    for (b = 0; b < 6; b++) {
        const u16 *dcCodes, *dcLens, *acCodes, *acLens;
        s16 *blk;
        u16 dc, old;

        run = 0;
        if (b < 4) {
            dcCodes = D_800392D0; dcLens = D_800392F0;
            acCodes = D_80039350; acLens = D_80039550;
        } else {
            dcCodes = D_80039310; dcLens = D_80039330;
            acCodes = D_80039750; acLens = D_80039950;
        }
        blk = coef + b * 64;
        dc = blk[0];
        old = pred[b];
        pred[b] = dc;
        Jpeg64_EncodeDC((s16)(dc - old), dcCodes, dcLens);

        k = 1;
        for (;;) {
            s16 v = blk[gJpeg64Zigzag[k]];
            k++;
            if (v == 0) {
                run++;
                if (k >= 64) {
                    Jpeg64_EncodeAC(0, 0, acCodes, acLens);       /* EOB */
                    break;
                }
                continue;
            }
            while (run >= 16) {
                Jpeg64_EncodeAC(0, 15, acCodes, acLens);          /* ZRL */
                run -= 16;
            }
            Jpeg64_EncodeAC(v, (u16)run, acCodes, acLens);
            run = 0;
            if (k >= 64) {
                break;
            }
        }
    }
}

/* @ 0x8001B250 */
/* Decodes one macroblock: 6 blocks of DC + run/level AC symbols, written to
 * coef in natural order through the zigzag table. pred holds one DC predictor
 * per block slot. Like the original there is no check that k stays < 64. */
void Jpeg64_DecodeMacroblock(s16 *coef, s16 *pred)
{
    s16 b, k;
    s16 level, run;

    for (b = 0; b < 6; b++) {
        const Jpeg64Huff *dc, *ac;
        s16 *blk;
        s32 diff;

        if (b < 4) {
            dc = &gJpeg64HuffDcLuma;
            ac = &gJpeg64HuffAcLuma;
        } else {
            dc = &gJpeg64HuffDcChroma;
            ac = &gJpeg64HuffAcChroma;
        }
        diff = Jpeg64_DecodeDC(dc->valptr, dc->mincode, dc->maxcode, dc->huffval);
        blk = coef + b * 64;
        blk[0] = pred[b] + diff;
        pred[b] = pred[b] + diff;

        k = 1;
        for (;;) {
            Jpeg64_DecodeAC(&level, &run, ac->valptr, ac->mincode, ac->maxcode, ac->huffval);
            if (run == 0 && level == 0) {                     /* EOB */
                for (; k < 64; k++) {
                    blk[gJpeg64Zigzag[k]] = 0;
                }
                break;
            }
            for (run--; run != -1; run--) {
                blk[gJpeg64Zigzag[k]] = 0;
                k++;
            }
            blk[gJpeg64Zigzag[k]] = level;
            k++;
            if (k >= 64) {
                break;
            }
        }
    }
}

/* number of bits needed for |x| (x taken as s16) */
static inline u32 Jpeg64_BitSize(s32 x)
{
    u32 a = (s16)x < 0 ? (u32)-x : (u32)x;
    u32 size = 0;

    if ((a & 0xFFFF) != 0) {
        do {
            a = (a & 0xFFFF) >> 1;
            size++;
        } while (a != 0);
    }
    return size;
}

/* @ 0x8001B4D0 */
/* Encoder: writes a DC difference (size category code, then the bits;
 * negative values as x - 1, JPEG style). */
void Jpeg64_EncodeDC(s32 diff, const u16 *codes, const u16 *lens)
{
    u32 size = Jpeg64_BitSize(diff);

    Jpeg64_PutBits(codes[size], lens[size]);
    if (size != 0) {
        if ((s16)diff < 0) {
            diff--;
        }
        Jpeg64_PutBits(diff & 0xFFFF, size);
    }
}

/* @ 0x8001B55C */
/* Encoder: writes an AC run/level pair (symbol run*16 + size, then the bits). */
void Jpeg64_EncodeAC(s32 level, u32 run, const u16 *codes, const u16 *lens)
{
    u32 size = Jpeg64_BitSize(level);
    u32 sym = size + (run & 0xFFFF) * 16;

    Jpeg64_PutBits(codes[sym], lens[sym]);
    if (size != 0) {
        if ((s16)level < 0) {
            level--;
        }
        Jpeg64_PutBits(level & 0xFFFF, size);
    }
}

/* Decodes one Huffman symbol (shared by Jpeg64_DecodeDC/AC, inlined twice in
 * the original). The first bit read is the code's MSB. No length limit, like
 * the original: a bad code keeps walking past the 16 lengths. */
static inline u32 Jpeg64_HuffDecode(const u16 *valptr, const u16 *mincode,
                                    const u16 *maxcode, const u16 *huffval)
{
    s32 len = 1;
    u32 code = Jpeg64_GetBit();

    while ((code & 0xFFFF) > maxcode[len] || maxcode[len] == 0xFFFF) {
        len++;
        code = Jpeg64_GetBit() | (code << 1);
    }
    return huffval[(s32)(valptr[len] + (code & 0xFFFF) - mincode[len])];
}

/* @ 0x8001B5F4 */
/* Decodes a DC difference: size category, then that many bits (JPEG EXTEND). */
s32 Jpeg64_DecodeDC(const u16 *valptr, const u16 *mincode, const u16 *maxcode, const u16 *huffval)
{
    u32 size = Jpeg64_HuffDecode(valptr, mincode, maxcode, huffval);
    u32 v;

    if (size == 0) {
        return 0;
    }
    v = Jpeg64_GetBits(size) & 0xFFFF;
    if ((v >> ((size - 1) & 31)) & 1) {
        return v;
    }
    return v + 1 - (1u << (size & 31));
}

/* @ 0x8001B708 */
/* Decodes an AC symbol: *run = zero run (high nibble), *level = value. */
void Jpeg64_DecodeAC(s16 *level, s16 *run, const u16 *valptr, const u16 *mincode,
                     const u16 *maxcode, const u16 *huffval)
{
    u32 sym = Jpeg64_HuffDecode(valptr, mincode, maxcode, huffval);
    u32 size = sym & 0xF;
    u32 v;

    *run = sym >> 4;
    if (size == 0) {
        *level = 0;
        return;
    }
    v = Jpeg64_GetBits(size);
    *level = v;
    if (((s32)(s16)v >> (size - 1)) & 1) {
        return;
    }
    *level = v + 1 - (1 << size);
}

/* @ 0x8001B84C */
/* Reverses the low n bits of v (n taken as u16; n = 0 gives 0). */
u32 Jpeg64_ReverseBits(u32 v, u32 n)
{
    u32 r = 0;
    u32 i;

    for (i = n - 1; (i & 0xFFFF) != 0xFFFF; i--) {
        r = (r << 1) | (v & 1);
        v = (v & 0xFFFF) >> 1;
    }
    return r & 0xFFFF;
}

/* @ 0x8001B898 */
/* Encoder: writes the low n bits of v MSB-first into the LSB-first stream
 * (by reversing them, then merging into the 24-bit window at the bit position). */
void Jpeg64_PutBits(u32 v, s32 n)
{
    s32 pos, byte, bit;
    u32 mask, w;
    u8 *p;

    if (n >= 2) {
        v = Jpeg64_ReverseBits(v, n);
    }
    pos = sBitPos;
    byte = pos / 8;
    bit = (pos - byte * 8) & 31;
    mask = D_80039FD0[n];
    p = sBitBuf + byte;
    w = p[0] + (p[1] << 8) + (p[2] << 16);
    w = (((v & 0xFFFF) & mask) << bit) | (w & ~(mask << bit));
    p[0] = w;
    sBitBuf[byte + 1] = w >> 8;
    sBitBuf[byte + 2] = w >> 16;
    sBitPos = sBitPos + n;
}

/* @ 0x8001B9A8 */
/* Reads one bit (bits are taken least-significant first). */
u32 Jpeg64_GetBit(void)
{
    s32 pos = sBitPos;
    u8 *p = sBitBuf + (pos >> 3);
    u32 w = p[0] + (p[1] << 8) + (p[2] << 16);

    sBitPos = pos + 1;
    return (w >> (pos & 7)) & 1;
}

/* @ 0x8001B9F8 */
/* Reads n bits (n taken as u16, at most 16 useful); the first bit read
 * becomes the most significant bit of the result. */
u32 Jpeg64_GetBits(u32 n)
{
    s32 pos = sBitPos;
    u8 *p = sBitBuf + (pos >> 3);
    u32 w = p[0] + (p[1] << 8) + (p[2] << 16);
    u32 r = 0;
    u32 i;

    sBitPos = pos + n;
    w >>= pos & 7;
    for (i = n - 1; (i & 0xFFFF) != 0xFFFF; i--) {
        r = (r << 1) | (w & 1);
        w = (w & 0xFFFF) >> 1;
    }
    return r & 0xFFFF;
}
