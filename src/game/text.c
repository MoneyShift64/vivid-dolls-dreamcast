/* Vivid Dolls - 8x8 bitmap-font text objects. See text.h. */
#include "text.h"
#include "heap.h"
#include "siff.h"
#include "matrix.h"
#include "scene.h"

extern Heap gGameHeap;              /* 0x80173600 */
extern s32 gScreenWidth;            /* 0x800C30D0: 320 or 640 */
extern SpriteGfx D_80038B10[];      /* quad set-up DL (CI4, RGBA16 TLUT) */
extern SpriteGfx D_80038B58[];      /* quad finish DL: 2x G_TRI1 + mode reset */
extern SpriteGfx D_80038B88[];      /* texrect set-up DL */
extern SpriteGfx D_80038BC0[];      /* texrect finish DL */
extern const SpriteVtx D_80038AD0[4];   /* quad template: (0,0) (8,0) (8,-8) (0,-8), s/t 0..0x200 */
extern const f32 D_80038BD8[4];     /* top colour ramp to white:    0.0 0.3 0.5 1.0 */
extern const f32 D_80038BE8[4];     /* bottom colour ramp to white: 0.5 0.3 0.1 0.0 */

void bzero(void *p, u32 n);
void func_800071CC(MtxF m, f32 angle, f32 x, f32 y, f32 z);   /* rotation about axis (x,y,z) */

#define PTR32(p) ((u32)(uintptr_t)(p))

/* @ 0x80015AA0
 * Creates (t == NULL: heap-allocates) a text object for str. Odd lengths are
 * padded with a space. Original quirk not kept: str is copied into a 32-byte
 * stack buffer without a length check (more than 30 characters overflow it). */
TextObj *Text_Create(TextObj *t, const u8 *str, u16 top, u16 bottom, u16 bg, u16 shadow)
{
    u8 buf[32];
    u32 n;

    if (t == NULL) {
        t = Heap_Alloc(&gGameHeap, sizeof(TextObj));
        bzero(t, sizeof(TextObj));
        t->owned = 1;
    } else {
        bzero(t, sizeof(TextObj));
        t->owned = 0;
    }
    if (t != NULL) {
        n = 0;
        while (str[n] != 0) {
            buf[n] = str[n];
            n++;
        }
        if (n & 1)
            buf[n++] = ' ';
        buf[n] = 0;
        t->length = n;
        t->capacity = n;
        Text_SetColors(t, top, bottom, bg, shadow);
        t->bitmap = Text_RenderString(buf, TEXT_FONT_ROM, 3, 1, 2);
    }
    return t;
}

/* @ 0x80015BE4  frees the bitmap, and the object if Text_Create allocated it */
s32 Text_Delete(TextObj *t)
{
    s32 owned = t->owned;
    Heap_Free(&gGameHeap, t->bitmap);
    if (owned)
        Heap_Free(&gGameHeap, t);
    return 0;
}

/* @ 0x80015C40
 * node matrix = scale(scale, scale, 1) * translate(x + 0.5, y + 0.5, z) (the
 * +0.5 is done in double), then rebuilds the quad DL and attaches it. */
void Text_Place(SceneNode *node, TextObj *t, f32 x, f32 y, f32 z, f32 scale)
{
    MtxF m;
    Mtx_Translate(node->mtx, (f32)((f64)x + 0.5), (f32)((f64)y + 0.5), z);
    Mtx_Scale(m, scale, scale, 1.0f);
    Mtx_Mul(node->mtx, m, node->mtx);
    Text_BuildQuadDL(t);
    node->dl = t->dl;
    node->setup_dl = t->setup_dl;
    node->finish_dl = t->finish_dl;
}

/* @ 0x80015D04  as Text_Place, with a rotation by angle about Z between the
 * translation and the scale */
void Text_PlaceRotated(SceneNode *node, TextObj *t, f32 x, f32 y, f32 z, f32 angle, f32 scale)
{
    MtxF m;
    Mtx_Translate(node->mtx, (f32)((f64)x + 0.5), (f32)((f64)y + 0.5), z);
    func_800071CC(m, angle, 0.0f, 0.0f, 1.0f);
    Mtx_Mul(node->mtx, m, node->mtx);
    Mtx_Scale(m, scale, scale, 1.0f);
    Mtx_Mul(node->mtx, m, node->mtx);
    Text_BuildQuadDL(t);
    node->dl = t->dl;
    node->setup_dl = t->setup_dl;
    node->finish_dl = t->finish_dl;
}

/* @ 0x80015E14  screen-space texture rectangle at (x, y) (truncated to s16),
 * scaled by sx, sy; attaches the DL to node */
void Text_PlaceScreen(SceneNode *node, TextObj *t, f32 x, f32 y, f32 sx, f32 sy)
{
    Text_BuildRectDL(t, (s16)(s32)x, (s16)(s32)y, sx, sy);
    node->dl = t->dl;
    node->setup_dl = t->setup_dl;
    node->finish_dl = t->finish_dl;
}

/* RGBA5551 -> 8-bit channels (low 3 bits zero) */
#define C5_R(c) ((f32)(((u32)(c) >> 8) & 0xF8))
#define C5_G(c) ((f32)(((u32)(c) >> 3) & 0xF8))
#define C5_B(c) ((f32)(((u32)(c) << 2) & 0xF8))

/* 8-bit channels (truncated) -> opaque RGBA5551 */
static u16 pack5551(f32 r, f32 g, f32 b)
{
    return (u16)((((s32)r << 8) & ~0x7FF) | (((s32)g << 3) & 0x7C0) | (((s32)b >> 2) & 0x3E) | 1);
}

/* @ 0x80015EA4
 * palette[0] = 0, [1] = bg (empty pixels), [2] = shadow; ink rows 0..3
 * (entries 3..6) = top blended towards white by 0, .3, .5, 1; rows 4..7
 * (entries 7..10) = bottom blended towards white by .5, .3, .1, 0. */
void Text_SetColors(TextObj *t, u16 top, u16 bottom, u16 bg, u16 shadow)
{
    f32 r, g, b;
    s32 i;

    t->palette[0] = 0;
    t->palette[1] = bg;
    t->palette[2] = shadow;
    r = C5_R(top); g = C5_G(top); b = C5_B(top);
    for (i = 0; i < 4; i++) {
        f32 k = D_80038BD8[i];
        t->palette[3 + i] = pack5551(r + (255.0f - r) * k, g + (255.0f - g) * k, b + (255.0f - b) * k);
    }
    r = C5_R(bottom); g = C5_G(bottom); b = C5_B(bottom);
    for (i = 4; i < 8; i++) {
        f32 k = D_80038BE8[i - 4];
        t->palette[3 + i] = pack5551(r + (255.0f - r) * k, g + (255.0f - g) * k, b + (255.0f - b) * k);
    }
}

/* @ 0x80016094
 * palette[3] = from, entries 4..11 = linear blend from -> to at 1/8 .. 8/8
 * (the step is accumulated in double precision). */
void Text_SetGradient(TextObj *t, u16 from, u16 to, u16 bg, u16 shadow)
{
    f32 r0 = C5_R(from), g0 = C5_G(from), b0 = C5_B(from);
    f32 dr = C5_R(to) - r0, dg = C5_G(to) - g0, db = C5_B(to) - b0;
    f32 k = 0.125f;
    s32 i;

    t->palette[0] = 0;
    t->palette[1] = bg;
    t->palette[3] = from;
    t->palette[2] = shadow;
    for (i = 0; i < 8; i++) {
        f32 r = r0 + dr * k, g = g0 + dg * k, b = b0 + db * k;
        k = (f32)((f64)k + 0.125);
        t->palette[4 + i] = pack5551(r, g, b);
    }
}

/* @ 0x800161E8
 * Re-renders the text with a new string, cut to the capacity given at
 * creation (odd lengths padded with a space). The bitmap is re-laid out for
 * the new length. */
TextObj *Text_SetString(TextObj *t, const u8 *str)
{
    u8 buf[32];
    u32 n = 0;

    while (*str != 0 && n < t->capacity)
        buf[n++] = *str++;
    if (n & 1)
        buf[n++] = ' ';
    buf[n] = 0;
    t->length = n;
    Text_RenderInto(t->bitmap, buf, TEXT_FONT_ROM, 3, 1, 2);
    return t;
}

/* G_LOADBLOCK w1 for the length*8 x 8 CI4 bitmap, loaded as 16-bit texels */
static u32 text_load_block(u32 len)
{
    u32 wpl = len >> 1;                   /* 64-bit words per line */
    u32 n = (wpl != 0) ? wpl + 0x7FF : 0x800;
    u32 texels = ((len << 6) >> 2) - 1;
    if (texels >= 0x800)
        texels = 0x7FF;
    return ((texels & 0xFFF) << 12) | 0x07000000u | (((wpl != 0) ? n / wpl : n) & 0xFFF);
}

static u32 text_tile_line(u32 len)  /* G_SETTILE w0 for the CI4 render tile */
{
    return (((((len << 3) >> 1) + 7) >> 3 & 0x1FF) << 9) | 0xF5400000u;
}

static u32 text_tile_size(u32 len)  /* G_SETTILESIZE w1: (len*8-1, 7) in 10.2 */
{
    return ((((len << 5) - 4) & 0xFFF) << 12) | 0x1C;
}

/* @ 0x80016298
 * Display list for drawing the text as a textured quad (8*length x 8 units,
 * origin top-left, y up); the triangles themselves are in the finish DL. */
void Text_BuildQuadDL(TextObj *t)
{
    SpriteGfx *g = t->dl;
    u32 len;
    s32 i;

    for (i = 0; i < 4; i++)
        t->vtx[i] = D_80038AD0[i];
    len = t->length;
    t->vtx[0].s = -5;
    t->vtx[3].s = -5;
    t->setup_dl = D_80038B10;
    t->finish_dl = D_80038B58;
    t->vtx[1].s = (s16)((len << 9) - 5);
    t->vtx[2].x = (s16)(len << 3);
    t->vtx[1].x = (s16)(len << 3);
    t->vtx[2].s = (s16)((len << 9) - 5);

    g[0].w0 = 0xFD500000; g[0].w1 = PTR32(t->bitmap);       /* G_SETTIMG CI 16b */
    g[1].w0 = 0xF5500000; g[1].w1 = 0x07000000;             /* G_SETTILE load tile */
    g[2].w0 = 0xE6000000; g[2].w1 = 0;                      /* G_RDPLOADSYNC */
    g[3].w0 = 0xF3000000; g[3].w1 = text_load_block(len);   /* G_LOADBLOCK */
    g[4].w0 = 0xE7000000; g[4].w1 = 0;                      /* G_RDPPIPESYNC */
    g[5].w0 = text_tile_line(len); g[5].w1 = 0;             /* G_SETTILE CI4 */
    g[6].w0 = 0xF2000000; g[6].w1 = text_tile_size(len);    /* G_SETTILESIZE */
    g[7].w0 = 0xFD100000; g[7].w1 = PTR32(t->palette);      /* G_SETTIMG TLUT */
    g[8].w0 = 0xE8000000; g[8].w1 = 0;                      /* G_RDPTILESYNC */
    g[9].w0 = 0xF5000100; g[9].w1 = 0x07000000;             /* G_SETTILE tmem 0x100 */
    g[10].w0 = 0xE6000000; g[10].w1 = 0;                    /* G_RDPLOADSYNC */
    g[11].w0 = 0xF0000000; g[11].w1 = 0x0703C000;           /* G_LOADTLUT 16 colours */
    g[12].w0 = 0xE7000000; g[12].w1 = 0;                    /* G_RDPPIPESYNC */
    g[13].w0 = 0x04300040; g[13].w1 = PTR32(t->vtx);        /* G_VTX 4 */
    g[14].w0 = 0xB8000000; g[14].w1 = 0;                    /* G_ENDDL */
}

/* coordinate clamped at 0 when negative as an s16 (gScissoredTexRect style) */
static u32 rect_clamp(s32 v)
{
    return ((s16)v < 0) ? 0 : (u32)v;
}

/* texture-coordinate shift for a rectangle starting off-screen */
static s32 scis_shift(s16 coord, s16 step)
{
    s32 v = (coord * step) >> 7;
    if (step < 0)
        return v >= 0 ? v : 0;
    return v <= 0 ? v : 0;
}

/* float -> u32 the way GCC does it (values >= 2^31 handled separately) */
static u32 f2u(f32 f)
{
    if (f >= 2147483648.0f)
        return (u32)(s32)(f - 2147483648.0f) | 0x80000000u;
    return (u32)(s32)f;
}

/* @ 0x8001650C
 * Display list for drawing the text as a G_TEXRECT at screen position
 * (x, y) in 320x240 units (x from the centre, y up from the centre), scaled
 * by sx (also by gScreenWidth/320) and sy. */
void Text_BuildRectDL(TextObj *t, s16 x, s16 y, f32 sx, f32 sy)
{
    SpriteGfx *g = t->dl;
    f64 xscale;
    f32 sxs, wpix, dsdx, dtdy;
    s32 ulx, uly, lrx, lry;
    u32 st;

    t->setup_dl = D_80038B88;
    t->finish_dl = D_80038BC0;
    g[0].w0 = 0xFD100000; g[0].w1 = PTR32(t->palette);      /* G_SETTIMG TLUT */
    g[1].w0 = 0xE8000000; g[1].w1 = 0;                      /* G_RDPTILESYNC */
    g[2].w0 = 0xF5000100; g[2].w1 = 0x07000000;             /* G_SETTILE tmem 0x100 */
    g[3].w0 = 0xE6000000; g[3].w1 = 0;                      /* G_RDPLOADSYNC */
    g[4].w0 = 0xF0000000; g[4].w1 = 0x0703C000;             /* G_LOADTLUT */
    g[5].w0 = 0xE7000000; g[5].w1 = 0;                      /* G_RDPPIPESYNC */
    g[6].w0 = 0xFD500000; g[6].w1 = PTR32(t->bitmap);       /* G_SETTIMG CI 16b */
    g[7].w0 = 0xF5500000; g[7].w1 = 0x07000000;             /* G_SETTILE load tile */
    g[8].w0 = 0xE6000000; g[8].w1 = 0;                      /* G_RDPLOADSYNC */
    g[9].w0 = 0xF3000000; g[9].w1 = text_load_block(t->length);   /* G_LOADBLOCK */
    g[10].w0 = 0xE7000000; g[10].w1 = 0;                    /* G_RDPPIPESYNC */
    g[11].w0 = text_tile_line(t->length); g[11].w1 = 0;     /* G_SETTILE CI4 */
    g[12].w0 = 0xF2000000; g[12].w1 = text_tile_size(t->length);  /* G_SETTILESIZE */

    xscale = (f64)(f32)gScreenWidth / 320.0;
    sxs = (f32)((f64)sx * xscale);
    ulx = (s32)((f64)(x + 160) * xscale * 4.0);
    uly = (120 - y) * 4;
    wpix = (f32)(f64)(u32)(t->length << 3);
    dsdx = (f32)(1024.0 / (f64)sxs);
    dtdy = (f32)(1024.0 / (f64)sy);
    lrx = ulx + (s16)(s32)(wpix * sxs) * 4;
    lry = uly + (s16)(s32)((f64)sy * 8.0) * 4;

    g[13].w0 = ((rect_clamp(lrx) & 0xFFF) << 12) | (rect_clamp(lry) & 0xFFC) | 0xE4000000u;  /* G_TEXRECT */
    g[13].w1 = ((rect_clamp(ulx) & 0xFFF) << 12) | (rect_clamp(uly) & 0xFFC);
    g[14].w0 = 0xB4000000;                                  /* G_RDPHALF_1: s, t */
    st = ((s16)ulx < 0) ? (u32)(-scis_shift((s16)ulx, (s16)(s32)dsdx)) << 16 : 0;
    if ((s16)uly < 0)
        st |= (u32)(-scis_shift((s16)uly, (s16)(s32)dtdy)) & 0xFFFF;
    g[14].w1 = st;
    g[15].w0 = 0xB3000000;                                  /* G_RDPHALF_2: dsdx, dtdy */
    g[15].w1 = (f2u(dsdx) << 16) | (f2u(dtdy) & 0xFFFF);
    g[16].w0 = 0xB8000000; g[16].w1 = 0;                    /* G_ENDDL */
}

/* @ 0x800169B4
 * Allocates a bitmap for str (strlen*32 bytes + 16 scratch) and renders it.
 * No check for allocation failure (as in the original). */
u8 *Text_RenderString(const u8 *str, u32 font_rom, u8 ink, u8 empty, u8 shadow)
{
    s16 len = 0, i;
    s32 size;
    u8 *bitmap, *glyph;

    while (str[len] != 0)
        len++;
    size = len * 32;
    bitmap = Heap_Alloc(&gGameHeap, (u32)size | 0x10);
    glyph = bitmap + size;
    for (i = 0; i < len; i++) {
        Rom_Read(font_rom + (u32)(str[i] - 0x20) * 8, glyph, 8);
        Text_ExpandGlyph((u32 *)(bitmap + i * 4), glyph, (s16)(len * 4), ink, empty, shadow);
    }
    return bitmap;
}

/* @ 0x80016B00  renders str into an existing bitmap (stride = strlen*4) */
u8 *Text_RenderInto(u8 *bitmap, const u8 *str, u32 font_rom, u8 ink, u8 empty, u8 shadow)
{
    s16 len = 0, i;
    u8 *glyph;

    while (str[len] != 0)
        len++;
    glyph = bitmap + len * 32;
    for (i = 0; i < len; i++) {
        Rom_Read(font_rom + (u32)(str[i] - 0x20) * 8, glyph, 8);
        Text_ExpandGlyph((u32 *)(bitmap + i * 4), glyph, (s16)(len * 4), ink, empty, shadow);
    }
    return bitmap;
}

/* @ 0x80016C3C
 * Expands one 8x8 1-bit glyph to CI4 (8 pixels = one u32 per row; stride in
 * bytes). Ink pixels use index ink+row, empty pixels `empty`, and an empty
 * pixel whose upper-left neighbour is ink gets `shadow`. Quirk kept: the ink
 * index is an 8-bit counter, so ink+row > 15 spills into the next pixel. */
void Text_ExpandGlyph(u32 *dst, const u8 *glyph, s16 stride, u32 ink, u32 empty, u32 shadow)
{
    u8 lut[4];
    u32 prev = 0;
    s16 row, px;

    lut[1] = ink & 0xF;
    lut[3] = ink & 0xF;
    lut[0] = empty & 0xF;
    lut[2] = shadow & 0xF;
    for (row = 0; row < 8; row++) {
        u32 bits = *glyph++, cur = bits, out = 0;
        for (px = 0; px < 8; px++) {
            out = (out << 4) | lut[((cur & 0x80) >> 7) | ((prev & 0x80) >> 6)];
            cur <<= 1;
            prev <<= 1;
        }
        prev = bits >> 1;
        WBE32(dst, out);    /* endian: 8 CI4 texels, leftmost in the high nibble of byte 0 */
        dst = (u32 *)((u8 *)dst + stride);
        lut[1]++;
        lut[3]++;
    }
}
