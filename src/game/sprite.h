/* Vivid Dolls - sprites ("Sp3d" assets) (RAM 0x80012820-0x80013D78).
 *
 * Sprite_Parse loads a sprite asset from ROM and turns it into ready-to-draw
 * N64 data: Vtx array, palette, texture tiles and an F3D display list. One heap
 * block holds all four. The Dreamcast port replaces the display list with PVR
 * quads (see port/vd_assets.c), but this is the reference for what gets drawn.
 */
#ifndef VD_SPRITE_H
#define VD_SPRITE_H

#include "vd_common.h"

/* F3D vertex, 16 bytes */
typedef struct {
    s16 x, y, z;
    u16 flag;
    s16 s, t;
    u8 r, g, b, a;
} SpriteVtx;

typedef struct { u32 w0, w1; } SpriteGfx;

enum { SPRITE_FMT_CI4 = 0, SPRITE_FMT_CI8 = 1, SPRITE_FMT_RGBA16 = 2 };

typedef struct Sprite {
    void       *buffer;     /* 0x00 heap block holding everything below */
    SpriteVtx  *vertices;   /* 0x04 */
    u16        *palette;    /* 0x08 RGBA5551 */
    u8         *tiles;      /* 0x0C */
    SpriteGfx  *setup_dl;   /* 0x10 render-mode set-up (D_80038930 / D_80038968 / D_800389C0) */
    SpriteGfx  *dl;         /* 0x14 the sprite's own display list */
    SpriteGfx  *finish_dl;  /* 0x18 NULL, D_800389A8 or D_80038A00 */
    u16        *rect_cmds;  /* 0x1C screen sprites: copy of the "dlst" byte code */
    u16         tile_bytes; /* 0x20 screen sprites: bytes per tile */
    u16         tile_w;     /* 0x22 screen sprites */
    u16         tile_h;     /* 0x24 screen sprites */
    u8          format;     /* 0x26 SPRITE_FMT_* */
    u8          flags;      /* 0x27 screen sprites: info[10] (bit 1 = 32x32 tiles, bit 2 = screen sprite) */
    u16         owned;      /* 0x28 1 = allocated by Sprite_New, freed with it */
    s16         origin_x;   /* 0x2A screen sprites */
    s16         origin_y;   /* 0x2C screen sprites */
    u8          unk2E[2];
} Sprite;                   /* 0x30 bytes */

/* "Screen sprites" (info mode 4, e.g. full-screen pictures) have no vertices:
 * they are drawn as texture rectangles, rebuilt by Sprite_DrawScreen each time. */

s32 Sprite_Parse(Sprite *spr, u32 rom_addr, s32 st_offset, s32 shared_palette);   /* 0x80012D0C */
Sprite *Sprite_New(Sprite *spr, u32 rom_addr);                                    /* 0x800128A8 */
Sprite *Sprite_NewSharedPalette(Sprite *spr, u32 rom_addr);                       /* 0x80012930 */
Sprite *Sprite_NewPlain(Sprite *spr, u32 rom_addr);                               /* 0x80012820 */
Sprite *Sprite_NewScreen(Sprite *spr, u32 rom_addr);                              /* 0x800129B8 */
s32     Sprite_Delete(Sprite *spr);                                               /* 0x80012A38 */
s32     Sprite_ParseScreen(Sprite *spr, u32 rom_addr);                            /* 0x8001397C */
void    Sprite_DrawScreen(Sprite *spr, f32 x, f32 y, f32 scale_x, f32 scale_y);   /* 0x80013D78 */


struct SceneNode;
void Sprite_Place(Sprite *spr, struct SceneNode *node, f32 x, f32 y, f32 sx, f32 sy);         /* 0x80012A94 */
void Sprite_Place2(Sprite *spr, struct SceneNode *node, f32 x, f32 y, f32 sx, f32 sy);        /* 0x80012B38 */
void Sprite_PlaceCustom(SpriteGfx *setup, Sprite *spr, struct SceneNode *node,
                        f32 x, f32 y, f32 sx, f32 sy);                                      /* 0x80012BDC */
void Sprite_PlaceScreen(Sprite *spr, struct SceneNode *node, f32 x, f32 y, f32 sx, f32 sy);   /* 0x80012C84 */

#endif
