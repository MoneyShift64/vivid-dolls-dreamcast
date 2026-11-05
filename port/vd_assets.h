/* Vivid Dolls port - asset containers ("Siff"), LZSS, and sprites ("Sp3d").
 *
 * Matches the game code:
 *   Asset_LoadLzss   (func_8001BC30)  - LZSS decompression
 *   Sprite_Parse     (func_80012D0C)  - Siff/Sp3d chunk walking
 * Everything here is endian-safe and has no N64 dependencies.
 */
#ifndef VD_ASSETS_H
#define VD_ASSETS_H

#include "vd_types.h"

/* ROM layout */
#define VD_ASSET_ROM_START 0x611F0u  /* game asset pointers are (ptr & 0xFFFFFF) + this */
#define VD_ASSET_ROM_END   0x23AFE0u

/* ---- LZSS ----
 * Stream = 4-byte little-endian output size, then groups of 8 items led by a flag
 * byte (MSB first). Flag bit 1 = back-reference of 2 bytes "LD DD":
 * length = L + 3, distance = 0xDDD + 1. Flag bit 0 = one literal byte. */
u32  vd_lzss_size(const u8 *src);
/* Decompresses into dst (must hold vd_lzss_size bytes). Returns bytes of src consumed. */
u32  vd_lzss_decompress(const u8 *src, u8 *dst);

/* An asset in ROM is either a raw "Siff" container or LZSS data that unpacks to one. */
int  vd_asset_is_raw(const u8 *rom_asset);

/* ---- Siff chunks ----
 * Big-endian IFF style: 4-char tag, u32 size, data. "Siff" and "Sp3d" contain
 * sub-chunks. Returns pointer to the chunk data (and its size) or NULL. */
const u8 *vd_siff_find(const u8 *data, u32 size, const char tag[4], u32 *out_size);

/* ---- Sprites (Sp3d) ----
 * pal8: 256 x RGBA5551 palette          tile: 8-bit tiles, tile_w x tile_h each
 * vrtx: one (x, y) s16 per placed tile   dlst: byte code placing tiles (see .c)
 * info: s16[12] - [1] tile_w, [2] tile_h, [5] tile count, [10] mode            */
#define VD_SPRITE_MAX_QUADS 512

typedef struct {
    s16 x, y;     /* top-left corner, game coordinates (y grows upwards) */
    u8  tile;     /* index into the tile data */
} VdSpriteQuad;

typedef struct {
    const u8 *palette;       /* 256 big-endian RGBA5551 entries */
    const u8 *tiles;         /* tile_count * tile_w * tile_h bytes */
    u16 tile_w, tile_h, tile_count;
    u16 num_quads;
    VdSpriteQuad quads[VD_SPRITE_MAX_QUADS];
    s16 min_x, max_y;        /* bounding box (for rendering to a flat image) */
    u16 width, height;
} VdSprite;

/* siff = decompressed Siff container. Returns 0 on success. */
int  vd_sprite_parse(const u8 *siff, u32 size, VdSprite *out);

/* Draws the whole sprite into a flat ARGB1555 image (width*height). Handy for
 * tests and tools; the Dreamcast build can instead upload each tile as a
 * PAL8 texture (tiles are 8..64 px power-of-two sizes, as the PVR wants). */
void vd_sprite_render_argb1555(const VdSprite *spr, u16 *out_pixels);

#endif
