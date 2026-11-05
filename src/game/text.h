/* Vivid Dolls - 8x8 bitmap-font text objects (RAM 0x80015AA0-0x80016D20).
 *
 * A TextObj turns a short string into a CI4 texture (one 8x8 glyph per
 * character, rendered from the 1-bit font in ROM at 0x196710) plus the F3D
 * display list that draws it, either as a textured quad attached to a scene
 * node (Text_Place / Text_PlaceRotated) or as a screen-space texture rectangle
 * (Text_PlaceScreen).
 *
 * Font (ROM 0x196710-0x196CC0, 0x5B0 bytes): 182 glyphs for character codes
 * 0x20..0xD5, 8 bytes each, one byte per row (top row first), MSB = leftmost
 * pixel. The game indexes it with (byte - 0x20) * 8 without range checks, so
 * bytes below 0x20 or above 0xD5 (e.g. EUC/SJIS lead bytes) read whatever ROM
 * data lies before/after the font.
 *
 * Bitmap: CI4, width = length*8 pixels, height 8, row stride = length*4 bytes.
 * Palette indices used (see Text_ExpandGlyph):
 *   1     empty pixel (palette[1] = "bg" colour; 0 = transparent)
 *   2     drop shadow: pixel set one row down and one pixel right of an ink pixel
 *   3..10 ink, index = 3 + row (vertical colour gradient)
 *   palette[0] is always 0.
 */
#ifndef VD_TEXT_H
#define VD_TEXT_H

#include "vd_common.h"
#include "sprite.h"

#define TEXT_FONT_ROM   0x196710u   /* 8x8 1bpp font, first glyph = ' ' */

typedef struct TextObj {
    SpriteGfx *setup_dl;    /* 0x000 D_80038B10 (quad) / D_80038B88 (rect) */
    SpriteGfx *finish_dl;   /* 0x004 D_80038B58 (quad: draws the 2 triangles) / D_80038BC0 (rect) */
    SpriteGfx  dl[32];      /* 0x008 display list built by Text_BuildQuadDL / Text_BuildRectDL */
    SpriteVtx  vtx[4];      /* 0x108 quad, from template D_80038AD0 */
    u16        palette[16]; /* 0x148 RGBA5551 TLUT */
    u8        *bitmap;      /* 0x168 heap block: CI4 texture + 16 bytes glyph scratch */
    u32        capacity;    /* 0x16C length given to Text_Create (bitmap is sized for it) */
    u32        length;      /* 0x170 current length in characters (always even) */
    s32        owned;       /* 0x174 1 = TextObj itself was heap-allocated by Text_Create */
    u8         unk178[8];
} TextObj;                  /* 0x180 bytes */

struct SceneNode;

TextObj *Text_Create(TextObj *t, const u8 *str, u16 top, u16 bottom, u16 bg, u16 shadow); /* 0x80015AA0 */
s32      Text_Delete(TextObj *t);                                                       /* 0x80015BE4 */
void     Text_Place(struct SceneNode *node, TextObj *t, f32 x, f32 y, f32 z, f32 scale); /* 0x80015C40 */
void     Text_PlaceRotated(struct SceneNode *node, TextObj *t, f32 x, f32 y, f32 z,
                           f32 angle, f32 scale);                                       /* 0x80015D04 */
void     Text_PlaceScreen(struct SceneNode *node, TextObj *t, f32 x, f32 y, f32 sx, f32 sy); /* 0x80015E14 */
void     Text_SetColors(TextObj *t, u16 top, u16 bottom, u16 bg, u16 shadow);          /* 0x80015EA4 */
void     Text_SetGradient(TextObj *t, u16 from, u16 to, u16 bg, u16 shadow);           /* 0x80016094 */
TextObj *Text_SetString(TextObj *t, const u8 *str);                                     /* 0x800161E8 */
void     Text_BuildQuadDL(TextObj *t);                                                  /* 0x80016298 */
void     Text_BuildRectDL(TextObj *t, s16 x, s16 y, f32 sx, f32 sy);                    /* 0x8001650C */
u8      *Text_RenderString(const u8 *str, u32 font_rom, u8 ink, u8 empty, u8 shadow);   /* 0x800169B4 */
u8      *Text_RenderInto(u8 *bitmap, const u8 *str, u32 font_rom, u8 ink, u8 empty, u8 shadow); /* 0x80016B00 */
void     Text_ExpandGlyph(u32 *dst, const u8 *glyph, s16 stride, u32 ink, u32 empty, u32 shadow); /* 0x80016C3C */

#endif
