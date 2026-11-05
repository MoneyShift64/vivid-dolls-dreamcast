/* Vivid Dolls - photo view and mosaic effect. See photoview.h. */
#include "photoview.h"
#include "heap.h"

extern Heap gGameHeap;              /* 0x80173600 */
extern SpriteGfx gPhotoSetupDL[];   /* 0x80038A20 */
extern SpriteGfx gPhotoFinishDL[];  /* 0x80038A60 */
void bzero(void *p, u32 n);

#define PTR32(p) ((u32)(uintptr_t)(p))

static const s16 sMosaicSize[] = { 1, 2, 4, 8, 16, 32 };             /* 0x80038A78 */
static const f32 sMosaicArea[] = { 1.0f, 4.0f, 16.0f, 64.0f, 256.0f, 1024.0f };   /* 0x80038A88 */

/* @ 0x800145F0 */
PhotoView *PhotoView_New(PhotoView *v, u16 *image, s32 width, s32 height, s32 x, s32 y)
{
    if (v == NULL) {
        v = Heap_Alloc(&gGameHeap, sizeof(PhotoView));
        bzero(v, sizeof(PhotoView));
        v->dl = Heap_Alloc(&gGameHeap, 0xD00);
        v->owned = 1;
    } else {
        bzero(v, sizeof(PhotoView));
        v->owned = 0;
    }
    if (v != NULL) {
        v->image = image;
        v->width = width;
        v->height = height;
        PhotoView_Build(v, x, y);
    }
    return v;
}

/* @ 0x800146B8 */
s32 PhotoView_Delete(PhotoView *v)
{
    s32 owned = v->owned;
    Heap_Free(&gGameHeap, v->dl);
    if (owned != 0)
        Heap_Free(&gGameHeap, v);
    return 0;
}

/* @ 0x80014714  scroll position is clamped so the window stays on the image */
void PhotoView_SetScroll(PhotoView *v, s32 x, s32 y)
{
    if (x < 0) {
        x = 0;
    } else if (v->width - 320 < x) {
        x = v->width - 320;
    }
    if (y < 0) {
        y = 0;
    } else if (v->height - 240 < y) {
        y = v->height - 240;
    }
    PhotoView_Build(v, x, y);
}

/* @ 0x80014774  40 strips of 320x6 pixels; x is split into a 4-pixel-aligned
 * load position and a sub-offset applied to the rectangle */
void PhotoView_Build(PhotoView *v, s32 x, s32 y)
{
    SpriteGfx *g = v->dl;
    const s32 frac = x & 3;
    const s32 xh = ((0x140 - frac) * 4) + 1;
    const s32 xl = (frac * -4) + 1;
    const s32 width = v->width;
    s32 row, yl = 1, yh = 0x19;

    v->setup_dl = gPhotoSetupDL;
    v->finish_dl = gPhotoFinishDL;
    for (row = 0; row < 0xF0; row += 6) {
        u32 st;
        g[0].w0 = ((u32)(width - 1) & 0xFFF) | 0xFD100000u;          /* G_SETTIMG RGBA16 */
        g[0].w1 = PTR32((u8 *)v->image + ((x & ~3) * 2) + ((y + row) * width * 2));
        g[1].w0 = 0xF510A200; g[1].w1 = 0x07080200;                     /* G_SETTILE */
        g[2].w0 = 0xE6000000; g[2].w1 = 0;                              /* G_RDPLOADSYNC */
        g[3].w0 = 0xF4000000; g[3].w1 = 0x07500018;                     /* G_LOADTILE 320x6 */
        g[4].w0 = 0xE7000000; g[4].w1 = 0;                              /* G_RDPPIPESYNC */
        g[5].w0 = 0xF510A200; g[5].w1 = 0x80200;                        /* G_SETTILE */
        g[6].w0 = 0xF2000000; g[6].w1 = 0x500018;                       /* G_SETTILESIZE */
        g[7].w0 = ((u32)(xh & (~xh >> 31) & 0xFFF) << 12) |
                  ((u32)(yh & (~((s32)((u32)yh << 16) >> 16) >> 31) & 0xFFF) | 0xE4000000u);   /* G_TEXRECT */
        g[7].w1 = ((u32)(xl & (~xl >> 31) & 0xFFF) << 12) |
                  (u32)(yl & (~((s32)((u32)yl << 16) >> 16) >> 31) & 0xFFF);
        g[8].w0 = 0xB4000000;
        st = 0;
        if (xl < 0) {
            s32 d = (s32)((u32)xl << 10) >> 7;
            st = (u32)((d & -(d < 1)) * -0x10000);
        }
        if (yl < 0) {
            s32 d = (s32)((u32)yl << 16) >> 13;
            st |= (u32)(-(d & -(d < 1))) & 0xFFFF;
        }
        g[8].w1 = st;
        g[9].w0 = 0xB3000000; g[9].w1 = 0x04000400;                     /* dsdx = dtdy = 1 */
        g += 10;
        yl += 0x18;
        yh += 0x18;
    }
    g->w0 = 0xB8000000;                                                 /* G_ENDDL */
    g->w1 = 0;
}

/* @ 0x80014A00  replace each n x n block (n = 1 << level, level 0-5) by its
 * average colour times brightness. Blocks are not clipped to w/h. */
void Image_Mosaic(u16 *img, s32 stride, s32 unused, s32 x, s32 y, s32 w, s32 h,
                  s32 level, f32 brightness)
{
    s32 by, bx;
    (void)unused;
    if (level >= 6)
        level = 5;

    for (by = y; by < y + h; by += sMosaicSize[level]) {
        for (bx = x; bx < x + w; bx += sMosaicSize[level]) {
            u32 r = 0, gr = 0, b = 0;
            s32 i, j, ri, gi, bi;
            f32 f, area;

            for (j = 0; j < sMosaicSize[level]; j++) {
                for (i = 0; i < sMosaicSize[level]; i++) {
                    u32 p = (u32)img[(by + j) * stride + bx + i] << 16;
                    r += p >> 27;
                    gr += ((s32)p >> 22) & 0x1F;
                    b += ((s32)p >> 17) & 0x1F;
                }
            }
            area = sMosaicArea[level];
            f = ((f32)(f64)r / area) * brightness;
            ri = (s32)(f < 0.0f ? f - 0.5f : f + 0.5f);
            area = sMosaicArea[level];
            f = ((f32)(f64)gr / area) * brightness;
            gi = (s32)(f < 0.0f ? f - 0.5f : f + 0.5f);
            area = sMosaicArea[level];
            f = ((f32)(f64)b / area) * brightness;
            bi = (s32)(f < 0.0f ? f - 0.5f : f + 0.5f);

            for (j = 0; j < sMosaicSize[level]; j++)
                for (i = 0; i < sMosaicSize[level]; i++)
                    img[(by + j) * stride + bx + i] = (u16)((ri << 11) + (gi << 6) + (bi * 2) + 1);
        }
    }
}

/* @ 0x80014DD8 */
void Image_CopyRect(const u16 *src, s32 src_stride, s32 unused1, s32 sx, s32 sy, s32 w, s32 h,
                    u16 *dst, s32 dst_stride, s32 unused2, s32 dx, s32 dy)
{
    s32 row, col;
    (void)unused1;
    (void)unused2;
    for (row = 0; row < h; row++)
        for (col = 0; col < w; col++)
            dst[(row + dy) * dst_stride + dx + col] = src[(row + sy) * src_stride + sx + col];
}
