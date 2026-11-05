/* Vivid Dolls - photo view: scrolling 16-bit images and the mosaic effect
 * (RAM 0x800145F0-0x80014E90).
 *
 * Decoded photos (320x240, 320x480, 640x240) are bigger than the 320x240 screen,
 * so a PhotoView shows a 320x240 window into one, drawn as 40 strips of 320x6
 * texture rectangles. PhotoView_Mosaic makes the "pixelated picture" effect.
 */
#ifndef VD_PHOTOVIEW_H
#define VD_PHOTOVIEW_H

#include "vd_common.h"
#include "sprite.h"

typedef struct PhotoView {
    SpriteGfx *dl;          /* 0x00 0xD00-byte display list */
    SpriteGfx *setup_dl;    /* 0x04 D_80038A20 */
    SpriteGfx *finish_dl;   /* 0x08 D_80038A60 */
    s32        owned;       /* 0x0C */
    s32        width;       /* 0x10 image size in pixels */
    s32        height;      /* 0x14 */
    u16       *image;       /* 0x18 RGBA5551 pixels */
} PhotoView;                /* 0x1C bytes */

PhotoView *PhotoView_New(PhotoView *v, u16 *image, s32 width, s32 height, s32 x, s32 y);  /* 0x800145F0 */
s32  PhotoView_Delete(PhotoView *v);                                                       /* 0x800146B8 */
void PhotoView_SetScroll(PhotoView *v, s32 x, s32 y);                                      /* 0x80014714 */
void PhotoView_Build(PhotoView *v, s32 x, s32 y);                                          /* 0x80014774 */
void Image_Mosaic(u16 *img, s32 stride, s32 unused, s32 x, s32 y, s32 w, s32 h,
                  s32 level, f32 brightness);                                              /* 0x80014A00 */
void Image_CopyRect(const u16 *src, s32 src_stride, s32 unused1, s32 sx, s32 sy, s32 w, s32 h,
                    u16 *dst, s32 dst_stride, s32 unused2, s32 dx, s32 dy);                /* 0x80014DD8 */

#endif
