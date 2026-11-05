/* Vivid Dolls port - PVR texture layout and conversion (pure C, no KOS).
 *
 * Shared by pvr_backend.c (Dreamcast) and the host seam model
 * (tests/seam_lib.c, tools/seam_check.py), so the model samples exactly the
 * texels the backend puts into VRAM.
 *
 * Layout: non-twiddled, row pitch w2 (power of two, 8..512), h2 rows.
 *  - rows [0, height) / columns [0, width): the image;
 *  - clamped (or >= 8 texel) sizes: one replicated column / row after the image
 *    (the bilinear neighbour at the right / bottom edge);
 *  - small wrapped sizes (< 8): tiled up to the power of two;
 *  - **wrap-side padding**: the PVR's bilinear filter takes the neighbour of a
 *    sample half a texel before the first row / column from the *last* row /
 *    column of the power-of-two texture (its address wraps even with UV clamp
 *    on; GL-based emulators clamp per texel and hide this). So when the
 *    replicated edge leaves spare rows / columns, the last row (h2-1) is a copy
 *    of row 0 and the last column (w2-1) a copy of column 0. Without it the top
 *    line of every photo strip blended 25 % of stale VRAM (thin dark lines
 *    every 12 pixels at 640x480 on a real Dreamcast).
 * Everything else in the power-of-two area is never sampled and not written.
 */
#ifndef PVR_TEXFILL_H
#define PVR_TEXFILL_H

#include <stddef.h>
#include <stdint.h>
#include "f3d_backend.h"

enum { PVRTF_565 = 0, PVRTF_1555 = 1, PVRTF_4444 = 2 };

typedef struct {
    int w2, h2;      /* power-of-two size */
    int fw, fh;      /* columns (even) / rows [0, fh) written from the image + edge pad */
    int wrap_row;    /* 1: row h2-1 holds a copy of row 0 (written separately) */
    int wrap_col;    /* 1: columns w2-2, w2-1 hold copies of column 0 */
    int fmt;         /* PVRTF_* */
} PvrTexLayout;

static inline int pvrtf_pow2(int v)
{
    int p = 8;
    while (p < v && p < 1024) p <<= 1;
    return p;
}

static inline uint16_t pvrtf_565(uint32_t c) { return (uint16_t)(((c >> 8) & 0xF800) | ((c >> 5) & 0x07E0) | ((c >> 3) & 0x001F)); }
static inline uint16_t pvrtf_1555(uint32_t c)
{
    return (uint16_t)(((c >> 24) >= 0x80 ? 0x8000 : 0) | ((c >> 9) & 0x7C00) | ((c >> 6) & 0x03E0) | ((c >> 3) & 0x001F));
}
static inline uint16_t pvrtf_4444(uint32_t c)
{
    return (uint16_t)(((c >> 16) & 0xF000) | ((c >> 12) & 0x0F00) | ((c >> 8) & 0x00F0) | ((c >> 4) & 0x000F));
}

static inline void pvrtf_layout(const F3DTexImage *img, PvrTexLayout *L)
{
    /* padding beyond the image: edge replication for clamped textures (and any
       of 8 texels or more), tiling for small wrapped ones */
    const int rep_x = img->width >= 8 || (img->pad & 1);
    const int rep_y = img->height >= 8 || (img->pad & 2);
    int w2 = pvrtf_pow2(img->width), h2 = pvrtf_pow2(img->height), fw, fh;
    if (w2 > 512) w2 = 512;
    if (h2 > 512) h2 = 512;
    fw = rep_x ? (img->width + 1 < w2 ? img->width + 1 : w2) : w2;
    fh = rep_y ? (img->height + 1 < h2 ? img->height + 1 : h2) : h2;
    fw = (fw + 1) & ~1;                         /* whole 32-bit words */
    if (fw > w2) fw = w2;
    L->w2 = w2; L->h2 = h2; L->fw = fw; L->fh = fh;
    L->wrap_row = rep_y && fh < h2;             /* row h2-1 is spare */
    L->wrap_col = rep_x && fw + 2 <= w2;        /* the last word (w2-2, w2-1) is spare */
    L->fmt = img->alpha == 0 ? PVRTF_565 : (img->alpha == 1 ? PVRTF_1555 : PVRTF_4444);
}

/* Converts the image into buf (pitch w2): rows [0, fh) with fw columns (+ the
 * wrap column if L->wrap_col), and row h2-1 if L->wrap_row. row_buf: scratch
 * of >= w2 u16. */
static inline void pvrtf_fill(const F3DTexImage *img, const PvrTexLayout *L, uint16_t *buf, uint16_t *row_buf)
{
    const int rep_x = img->width >= 8 || (img->pad & 1);
    const int rep_y = img->height >= 8 || (img->pad & 2);
    const int w2 = L->w2, fw = L->fw, fh = L->fh, fmt = L->fmt;
    int x, y;
    if (img->rgba16) {
        /* N64 RGBA5551 -> RGB565 / ARGB1555, two texels per 32-bit word; the same
           values as pvrtf_565()/pvrtf_1555() of the expanded ARGB8888 colour.
           Rows are img->stride texels apart (0 = width): the interpreter may hand
           over the source image itself (photo strips). */
        const int pitch = img->stride ? (int)img->stride : img->width;
        const int wd = img->width;
        for (y = 0; y < fh; y++) {
            int sy = y < img->height ? y : (rep_y ? img->height - 1 : y % img->height);
            const uint16_t *row = img->rgba16 + (size_t)sy * pitch;
            uint32_t *dst = (uint32_t *)(void *)(buf + (size_t)y * w2);
            int n2 = fw / 2, direct = 0;
            if (!rep_x && wd < 8) {
                /* small tiled row: build it with wrap first */
                uint16_t *o = row_buf;
                for (x = 0; x < fw; x++)
                    o[x] = row[x % wd];
                row = o;
                direct = n2;
            } else {
                direct = (wd < fw ? wd : fw) / 2;    /* pairs fully inside the image */
            }
            if (fmt == PVRTF_565) {
                if (((uintptr_t)row & 3) == 0) {
                    const uint32_t *q = (const uint32_t *)(const void *)row;
                    for (x = 0; x < direct; x++) {
                        uint32_t c = q[x];
                        dst[x] = (c & 0xFFC0FFC0u) | ((c >> 5) & 0x00200020u) | ((c >> 1) & 0x001F001Fu);
                    }
                } else {
                    for (x = 0; x < direct; x++) {
                        uint32_t c = (uint32_t)row[2 * x] | ((uint32_t)row[2 * x + 1] << 16);
                        dst[x] = (c & 0xFFC0FFC0u) | ((c >> 5) & 0x00200020u) | ((c >> 1) & 0x001F001Fu);
                    }
                }
                for (x = direct; x < n2; x++) {      /* edge replication */
                    int a = 2 * x < wd ? 2 * x : wd - 1, b = 2 * x + 1 < wd ? 2 * x + 1 : wd - 1;
                    uint32_t c = (uint32_t)row[a] | ((uint32_t)row[b] << 16);
                    dst[x] = (c & 0xFFC0FFC0u) | ((c >> 5) & 0x00200020u) | ((c >> 1) & 0x001F001Fu);
                }
            } else {
                if (((uintptr_t)row & 3) == 0) {
                    const uint32_t *q = (const uint32_t *)(const void *)row;
                    for (x = 0; x < direct; x++) {
                        uint32_t c = q[x];
                        dst[x] = ((c & 0x00010001u) << 15) | ((c >> 1) & 0x7FFF7FFFu);
                    }
                } else {
                    for (x = 0; x < direct; x++) {
                        uint32_t c = (uint32_t)row[2 * x] | ((uint32_t)row[2 * x + 1] << 16);
                        dst[x] = ((c & 0x00010001u) << 15) | ((c >> 1) & 0x7FFF7FFFu);
                    }
                }
                for (x = direct; x < n2; x++) {
                    int a = 2 * x < wd ? 2 * x : wd - 1, b = 2 * x + 1 < wd ? 2 * x + 1 : wd - 1;
                    uint32_t c = (uint32_t)row[a] | ((uint32_t)row[b] << 16);
                    dst[x] = ((c & 0x00010001u) << 15) | ((c >> 1) & 0x7FFF7FFFu);
                }
            }
        }
    } else {
        for (y = 0; y < fh; y++) {
            int sy = y < img->height ? y : (rep_y ? img->height - 1 : y % img->height);
            const uint32_t *row = img->argb + sy * img->width;
            uint16_t *o = buf + (size_t)y * w2;
            if (fmt == PVRTF_565) {
                for (x = 0; x < fw; x++)
                    o[x] = pvrtf_565(row[x < img->width ? x : (rep_x ? img->width - 1 : x % img->width)]);
            } else if (fmt == PVRTF_1555) {
                for (x = 0; x < fw; x++)
                    o[x] = pvrtf_1555(row[x < img->width ? x : (rep_x ? img->width - 1 : x % img->width)]);
            } else {
                for (x = 0; x < fw; x++)
                    o[x] = pvrtf_4444(row[x < img->width ? x : (rep_x ? img->width - 1 : x % img->width)]);
            }
        }
    }
    if (L->wrap_col)
        for (y = 0; y < fh; y++) {
            uint16_t *o = buf + (size_t)y * w2;
            o[w2 - 2] = o[0];                   /* w2-2 is never sampled; keeps the word defined */
            o[w2 - 1] = o[0];
        }
    if (L->wrap_row) {
        const uint32_t *s = (const uint32_t *)(const void *)buf;
        uint32_t *d = (uint32_t *)(void *)(buf + (size_t)(L->h2 - 1) * w2);
        for (x = 0; x < w2 / 2; x++)
            d[x] = s[x];
    }
}

#endif
