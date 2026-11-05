/* Vivid Dolls port - asset containers, LZSS and sprites. See vd_assets.h. */
#include <string.h>
#include "vd_assets.h"

/* ------------------------------------------------------------------ LZSS -- */

u32 vd_lzss_size(const u8 *src) { return vd_le32(src); }

u32 vd_lzss_decompress(const u8 *src, u8 *dst)
{
    const u32 size = vd_le32(src);
    const u8 *p = src + 4;
    u32 out = 0;
    u32 flags = 0;
    int bits = 0;

    while (out < size) {
        if (bits == 0) {
            flags = *p++;
            bits = 8;
        }
        bits--;
        if (flags & 0x80) {
            /* back-reference: 4-bit length, 12-bit distance */
            u32 b1 = p[0], b2 = p[1];
            u32 len = (b1 >> 4) + 3;
            u32 dist = (((b1 & 0xF) << 8) | b2) + 1;
            p += 2;
            while (len-- && out < size) {
                dst[out] = dst[out - dist];
                out++;
            }
        } else {
            dst[out++] = *p++;
        }
        flags = (flags << 1) & 0xFF;
    }
    return (u32)(p - src);
}

int vd_asset_is_raw(const u8 *a) { return memcmp(a, "Siff", 4) == 0; }

/* ----------------------------------------------------------- Siff chunks -- */

const u8 *vd_siff_find(const u8 *data, u32 size, const char tag[4], u32 *out_size)
{
    u32 o = 0;
    while (o + 8 <= size) {
        const u8 *c = data + o;
        u32 n = vd_be32(c + 4);
        if (memcmp(c, tag, 4) == 0) {
            if (out_size) *out_size = n;
            return c + 8;
        }
        /* descend into containers */
        if (memcmp(c, "Siff", 4) == 0 || memcmp(c, "Sp3d", 4) == 0) {
            const u8 *r = vd_siff_find(c + 8, n, tag, out_size);
            if (r) return r;
        }
        o += 8 + n;
    }
    return NULL;
}

/* --------------------------------------------------------------- Sprites -- */
/*
 * dlst byte code (matches the game's RSP display list builder):
 *   40 nn hhhh  load vertices: nn+1 vertices starting at vertex hhhh. Each vrtx
 *               (x, y) entry makes one quad = 4 vertices, so quad base = hhhh / 4.
 *   80 tt       select tile tt as the current texture.
 *   C0 ..       end of list.
 *   aaaa bbbb   draw two triangles; their vertex nibbles (e.g. 0130 0320) name
 *               a quad in the loaded batch: quad = min(nibbles) / 4.
 * Mode 4 sprites (info[10] == 4, full-screen backgrounds) have no vrtx chunk:
 *   80 tt xxxx yyyy  draw tile tt at (xxxx - 0x3E00, yyyy - 0x0E00).
 */
int vd_sprite_parse(const u8 *siff, u32 size, VdSprite *s)
{
    u32 n_info, n_pal, n_tile, n_dl, n_vtx = 0;
    const u8 *info = vd_siff_find(siff, size, "info", &n_info);
    const u8 *pal  = vd_siff_find(siff, size, "pal8", &n_pal);
    const u8 *tile = vd_siff_find(siff, size, "tile", &n_tile);
    const u8 *dl   = vd_siff_find(siff, size, "dlst", &n_dl);
    const u8 *vtx  = vd_siff_find(siff, size, "vrtx", &n_vtx);
    u32 i = 0, base = 0;
    u8 tex = 0;

    if (!info || !pal || !tile || !dl || n_info < 24 || n_pal < 512)
        return -1;

    memset(s, 0, sizeof(*s));
    s->palette = pal;
    s->tiles = tile;
    s->tile_w = vd_be16(info + 2);
    s->tile_h = vd_be16(info + 4);
    s->tile_count = vd_be16(info + 10);

    while (i + 2 <= n_dl && s->num_quads < VD_SPRITE_MAX_QUADS) {
        u8 op = dl[i];
        if (vtx == NULL) {                      /* mode 4: direct coordinates */
            if (op == 0x80 && i + 6 <= n_dl) {
                VdSpriteQuad *q = &s->quads[s->num_quads++];
                q->tile = dl[i + 1];
                q->x = (s16)(vd_be16(dl + i + 2) - 0x3E00);
                q->y = (s16)-(s16)(vd_be16(dl + i + 4) - 0x0E00);
                i += 6;
                continue;
            }
            if (op == 0xC0) break;
            i += 2;
            continue;
        }
        if (op == 0x40) {
            base = vd_be16(dl + i + 2) / 4;
            i += 4;
        } else if (op == 0x80) {
            tex = dl[i + 1];
            i += 2;
        } else if (op == 0xC0) {
            break;
        } else {
            u16 a = vd_be16(dl + i), b = vd_be16(dl + i + 2);
            u8 nib[6] = { (u8)((a >> 8) & 15), (u8)((a >> 4) & 15), (u8)(a & 15),
                          (u8)((b >> 8) & 15), (u8)((b >> 4) & 15), (u8)(b & 15) };
            u8 m = 15;
            u32 qi;
            for (int k = 0; k < 6; k++) if (nib[k] < m) m = nib[k];
            qi = base + m / 4;
            if ((qi + 1) * 4 <= n_vtx) {
                VdSpriteQuad *q = &s->quads[s->num_quads++];
                q->x = (s16)vd_be16(vtx + qi * 4);
                q->y = (s16)vd_be16(vtx + qi * 4 + 2);
                q->tile = tex;
            }
            i += 4;
        }
    }
    if (s->num_quads == 0) return -1;

    /* bounding box (screen space: y down) */
    {
        s16 minx = 32767, maxx = -32768, miny = 32767, maxy = -32768;
        for (u32 k = 0; k < s->num_quads; k++) {
            s16 x = s->quads[k].x, y = (s16)-s->quads[k].y;
            if (x < minx) minx = x;
            if (x > maxx) maxx = x;
            if (y < miny) miny = y;
            if (y > maxy) maxy = y;
        }
        s->min_x = minx;
        s->max_y = (s16)-miny;
        s->width = (u16)(maxx - minx + s->tile_w);
        s->height = (u16)(maxy - miny + s->tile_h);
    }
    return 0;
}

void vd_sprite_render_argb1555(const VdSprite *s, u16 *out)
{
    const u32 tsz = (u32)s->tile_w * s->tile_h;
    memset(out, 0, (size_t)s->width * s->height * 2);
    for (u32 k = 0; k < s->num_quads; k++) {
        const VdSpriteQuad *q = &s->quads[k];
        const u8 *t = s->tiles + q->tile * tsz;
        int ox = q->x - s->min_x;
        int oy = s->max_y - q->y;           /* flip y: game y grows upwards */
        for (int y = 0; y < s->tile_h; y++) {
            for (int x = 0; x < s->tile_w; x++) {
                u16 c = vd_be16(s->palette + 2 * t[y * s->tile_w + x]);
                if (c & 1)                  /* alpha bit set = opaque */
                    out[(oy + y) * s->width + ox + x] = vd_rgba5551_to_argb1555(c);
            }
        }
    }
}
