/* Vivid Dolls - sprites. See sprite.h. */
#include "sprite.h"
#include "siff.h"
#include "heap.h"

extern Heap gGameHeap;                  /* 0x80173600 */
extern SpriteGfx gSpriteSetupPlainDL[];  /* 0x80038930: st_offset == 0 */
extern SpriteGfx gSpriteSetupDL[];       /* 0x80038968 */
extern SpriteGfx gSpriteFinishDL[];      /* 0x800389A8 */

void bzero(void *p, u32 n);

/* corner offsets used to expand one vrtx point into a quad (0x8003D780 / 0x8003D788) */
static const u8 sCornerST[4][2] = { { 0x00, 0x00 }, { 0x40, 0x00 }, { 0x00, 0x40 }, { 0x40, 0x40 } };
static const u8 sCornerXY[4][2] = { { 0, 0 }, { 1, 0 }, { 0, 1 }, { 1, 1 } };

#define PTR32(p) ((u32)(uintptr_t)(p))

/* endian: sprite asset fields are big-endian (ROM order) */
static inline u16 rd16(const u8 *p) { return BE16(p); }

#if !VD_BIG_ENDIAN
/* endian (little-endian builds only): the F3D interpreter reads TLUTs and
 * RGBA16 texels as native u16, so the copies made from the asset are swapped
 * once here, after parsing. pal_words / tile_words = u32 words copied. Nothing
 * reads these arrays as big-endian afterwards. 8- and 4-bit texels are left as
 * they are. Vertices are built field by field, so they are native already. */
static void sprite_swap_native(Sprite *spr, u32 pal_words, u32 tile_words)
{
    if (spr->palette != NULL && pal_words != 0)
        SwapArray16(spr->palette, pal_words * 2);
    if (spr->format == SPRITE_FMT_RGBA16 && spr->tiles != NULL && tile_words != 0)
        SwapArray16(spr->tiles, tile_words * 2);
}
#define SPRITE_SWAP_NATIVE(spr, pw, tw) sprite_swap_native(spr, pw, tw)
#else
#define SPRITE_SWAP_NATIVE(spr, pw, tw) ((void)(pw), (void)(tw))
#endif

/* G_LOADBLOCK dxt and texel count, as the game computes them */
static u32 load_block(s32 words_per_line, s32 texels)
{
    s32 n = 0x800, t = texels - 1;
    if (words_per_line != 0)
        n = words_per_line + 0x7FF;
    if (t >= 0x800)
        t = 0x7FF;
    return ((u32)(t & 0xFFF) << 12) | 0x07000000u |
           (u32)((words_per_line != 0 ? n / words_per_line : n) & 0xFFF);
}

/* @ 0x80012D0C
 * st_offset: added to every texture coordinate (0x1F normally); 0 also selects
 *            the plain render-mode set-up.
 * shared_palette: non-zero = don't copy or load the palette (one is already loaded). */
s32 Sprite_Parse(Sprite *spr, u32 rom_addr, s32 st_offset, s32 shared_palette)
{
    u16 vtx_map[16];
    SiffIter it;
    u8 *data, *p, *cursor = NULL;
    u32 tag;
    s32 pal_count = 0, mode = 0;
    u16 tile_w = 0, tile_h = 0, tw2 = 0, th2 = 0;
    s16 tile_count = 0, vtx_count = 0, dl_count, off_x = 0;
    u16 off_y = 0;
    u32 pal_words = 0, tile_words = 0;    /* endian: for SPRITE_SWAP_NATIVE */

    SiffIter_Init(&it);
    data = Asset_LoadLzss(rom_addr);
    p = data;
    if (st_offset == 0) {
        spr->setup_dl = gSpriteSetupPlainDL;
        spr->finish_dl = NULL;
    } else {
        spr->setup_dl = gSpriteSetupDL;
        spr->finish_dl = gSpriteFinishDL;
    }
    spr->format = SPRITE_FMT_RGBA16;

    /* ---- pass 1: find "info" and allocate one block for everything ---- */
    for (;;) {
        p = SiffIter_Enter(&it, (u32 *)p);
        tag = it.level[it.depth].tag;
        if (tag == TAG_SP3D || tag == TAG_SIFF)
            continue;
        if (tag == TAG_INFO) {
            const u8 *q = p;
            pal_count = (s16)rd16(q + 0);
            tile_w = rd16(q + 2);
            tile_h = rd16(q + 4);
            tw2 = rd16(q + 6);
            th2 = rd16(q + 8);
            tile_count = (s16)rd16(q + 10);
            vtx_count = (s16)rd16(q + 12);
            dl_count = (s16)rd16(q + 14);
            off_x = (s16)rd16(q + 16);
            off_y = rd16(q + 18);
            mode = (s16)rd16(q + 20);
            if ((u32)mode >= 2) {
                Heap_Free(&gGameHeap, data);
                return -1;
            }
            cursor = Heap_Alloc(&gGameHeap, (u32)(pal_count * 2) + (u32)(th2 * (tw2 * tile_count)) +
                                (u32)(vtx_count * 16) + (u32)((dl_count + 0x18) * 8));
            spr->buffer = cursor;
            break;
        }
        p = SiffIter_Leave(&it);
        if (it.depth == -1)
            break;
    }

    /* ---- pass 2: copy the data and build the display list ---- */
    p = data;
    for (;;) {
        u8 *body = SiffIter_Enter(&it, (u32 *)p);
        p = body;
        tag = it.level[it.depth].tag;

        if (tag == TAG_SIFF || tag == TAG_SP3D)
            continue;

        if (tag == TAG_PAL4 || tag == TAG_PAL8) {
            spr->format = (tag == TAG_PAL4) ? SPRITE_FMT_CI4 : SPRITE_FMT_CI8;
            if (shared_palette == 0) {
                s32 n = (pal_count >> 1) - 1;
                u32 *src = (u32 *)p, *dst = (u32 *)cursor;
                spr->palette = (u16 *)cursor;
                pal_words = (u32)(n + 1);
                while (n != -1) {
                    *dst++ = *src++;
                    n--;
                }
                p = (u8 *)src;
                cursor = (u8 *)dst;
            }
        } else if (tag == TAG_TILE) {
            s32 n = (s32)((u32)(tw2 * th2 * tile_count) >> 2) - 1;
            u32 *src = (u32 *)p, *dst = (u32 *)cursor;
            spr->tiles = cursor;
            tile_words = (u32)(n + 1);
            while (n != -1) {
                *dst++ = *src++;
                n--;
            }
            p = (u8 *)src;
            cursor = (u8 *)dst;
        } else if (tag == TAG_VRTX) {
            SpriteVtx *v = (SpriteVtx *)cursor;
            spr->vertices = v;
            if (mode == 1) {
                /* one vertex per point; texture coordinates follow the position */
                u32 i;
                for (i = 0; i < (u32)vtx_count; i++, v++) {
                    v->x = (s16)rd16(p);
                    v->z = 0;
                    v->flag = 0;
                    v->y = (s16)rd16(p + 2);
                    v->r = 0; v->g = 0; v->b = 0x7F; v->a = 0xFF;
                    v->s = (s16)(((off_x + v->x) << 6) + st_offset);
                    p += 4;
                    v->t = (s16)(st_offset - ((off_y + (u16)v->y) << 6));
                }
            } else {
                /* each point is the top-left corner of a tile: make 4 vertices */
                u32 i, j;
                for (i = 0; i < ((u32)vtx_count >> 2); i++) {
                    s16 x = (s16)rd16(p), y = (s16)rd16(p + 2);
                    p += 4;
                    for (j = 0; j < 4; j++, v++) {
                        v->x = (s16)(x + sCornerXY[j][0] * tile_w);
                        v->z = 0;
                        v->flag = 0;
                        v->y = (s16)(y - sCornerXY[j][1] * tile_h);
                        v->s = (s16)((tile_w * sCornerST[j][0]) + st_offset);
                        v->r = 0; v->g = 0; v->b = 0x7F; v->a = 0xFF;
                        v->t = (s16)((tile_h * sCornerST[j][1]) + st_offset);
                    }
                }
            }
            cursor = (u8 *)v;
        } else if (tag == TAG_DLST) {
            SpriteGfx *g = (SpriteGfx *)cursor;
            const u32 w = tile_w, h = tile_h, texels = w * h;
            const s32 lrs = (s32)(w - 1) * 4, lrt = (s32)(h - 1) * 4;
            const u32 tilesize = ((u32)(lrs & 0xFFF) << 12) | (u32)(lrt & 0xFFF);
            const u32 tile_bytes = (u32)tw2 * th2;

            spr->dl = g;
            if (spr->format < 2 && shared_palette == 0) {
                /* load the palette into TMEM */
                g[0].w0 = 0xFD100000; g[0].w1 = PTR32(spr->palette);      /* G_SETTIMG */
                g[1].w0 = 0xE8000000; g[1].w1 = 0;                         /* G_RDPTILESYNC */
                g[2].w0 = 0xF5000100; g[2].w1 = 0x02000000;                /* G_SETTILE (TLUT) */
                g[3].w0 = 0xE6000000; g[3].w1 = 0;                         /* G_RDPLOADSYNC */
                g[4].w0 = 0xF0000000;                                      /* G_LOADTLUT */
                g[4].w1 = (spr->format == SPRITE_FMT_CI8 ? 0x023F0000u : 0x02030000u) | 0xC000;
                g[5].w0 = 0xE7000000; g[5].w1 = 0;                         /* G_RDPPIPESYNC */
                g += 6;
            }

            for (;;) {
                u16 cmd = rd16(p);
                p += 2;
                switch (cmd & 0xC000) {
                case 0x4000: {                                             /* load vertices */
                    u16 idx = rd16(p);
                    u32 n = cmd & 0xF, first = (cmd & 0xF0) >> 4, count = n + 1;
                    p += 2;
                    g->w0 = (((n << 4) | first) << 16) | ((count << 4) | 0x04000000u);   /* G_VTX */
                    g->w1 = PTR32(spr->vertices + idx);
                    g++;
                    if (mode != 0) {
                        u32 k;
                        for (k = 0; k < count; k++)
                            vtx_map[first + k] = idx++;
                    }
                    break;
                }
                case 0x0000:                                               /* triangle */
                    if (mode != 0) {
                        SpriteVtx *v = spr->vertices + vtx_map[cmd & 0xF];
                        s32 s = (v->s + 5) >> 4, t = (v->t + 5) >> 4;
                        g->w0 = ((u32)(s & 0xFFF) << 12) | ((u32)(t & 0xFFF) | 0xF2000000u);  /* G_SETTILESIZE */
                        g->w1 = ((u32)((s + lrs) & 0xFFF) << 12) | (u32)((t + lrt) & 0xFFF);
                        g++;
                    }
                    g->w0 = 0xBF000000;                                    /* G_TRI1 */
                    g->w1 = (u32)(cmd & 0xF) * 0xA0000 + ((u32)(cmd & 0xF0) >> 4) * 0xA00 +
                            ((u32)(cmd & 0xF00) >> 8) * 0xA;
                    g++;
                    break;
                case 0x8000: {                                             /* load a texture tile */
                    u32 tile = cmd & 0x3FFF;
                    if (spr->format == SPRITE_FMT_CI8) {
                        g[0].w0 = 0xFD500000; g[0].w1 = PTR32(spr->tiles + tile * tile_bytes);
                        g[1].w0 = 0xF5500000; g[1].w1 = 0x07080200;
                        g[2].w0 = 0xE6000000; g[2].w1 = 0;
                        g[3].w0 = 0xF3000000; g[3].w1 = load_block((s32)(w >> 3), (s32)(texels + 1) >> 1);
                        g[4].w0 = 0xE7000000; g[4].w1 = 0;
                        g[5].w0 = ((((u32)(w + 7) >> 3) & 0x1FF) << 9) | 0xF5480000u; g[5].w1 = 0x80200;
                        g[6].w0 = 0xF2000000; g[6].w1 = tilesize;
                    } else if (spr->format == SPRITE_FMT_CI4) {
                        g[0].w0 = 0xFD500000; g[0].w1 = PTR32(spr->tiles + tile * tile_bytes);
                        g[1].w0 = 0xF5500000; g[1].w1 = 0x07080200;
                        g[2].w0 = 0xE6000000; g[2].w1 = 0;
                        g[3].w0 = 0xF3000000; g[3].w1 = load_block((s32)(w >> 4), (s32)(texels + 3) >> 2);
                        g[4].w0 = 0xE7000000; g[4].w1 = 0;
                        g[5].w0 = ((((u32)((w >> 1) + 7) >> 3) & 0x1FF) << 9) | 0xF5400000u; g[5].w1 = 0x80200;
                        g[6].w0 = 0xF2000000; g[6].w1 = tilesize;
                    } else {
                        s32 wq = (s32)w / 4;
                        s32 n = wq > 0 ? wq + 0x7FF : 0x800;
                        s32 t = (s32)texels - 1;
                        if (t >= 0x800) t = 0x7FF;
                        g[0].w0 = 0xFD100000; g[0].w1 = PTR32(spr->tiles + tile * tile_bytes * 2);
                        g[1].w0 = 0xF5100000; g[1].w1 = 0x07080200;
                        g[2].w0 = 0xE6000000; g[2].w1 = 0;
                        g[3].w0 = 0xF3000000;
                        g[3].w1 = ((u32)(t & 0xFFF) << 12) | 0x07000000u | (u32)((wq <= 0 ? n : n / wq) & 0xFFF);
                        g[4].w0 = 0xE7000000; g[4].w1 = 0;
                        g[5].w0 = ((((u32)(w * 2 + 7) >> 3) & 0x1FF) << 9) | 0xF5100000u; g[5].w1 = 0x80200;
                        g[6].w0 = 0xF2000000; g[6].w1 = tilesize;
                    }
                    g += 7;
                    break;
                }
                default:                                                   /* 0xC000: end */
                    g->w0 = 0xB8000000;                                    /* G_ENDDL */
                    g->w1 = 0;
                    Heap_Free(&gGameHeap, data);
                    SPRITE_SWAP_NATIVE(spr, pal_words, tile_words);
                    return 0;
                }
            }
        }
        /* info, unknown chunks, and everything after a handled chunk */
        p = SiffIter_Leave(&it);
        if (it.depth == -1)
            break;
    }
    Heap_Free(&gGameHeap, data);
    SPRITE_SWAP_NATIVE(spr, pal_words, tile_words);
    return 0;
}

/* @ 0x80012820 */
Sprite *Sprite_NewPlain(Sprite *spr, u32 rom_addr)
{
    if (spr == NULL) {
        spr = Heap_Alloc(&gGameHeap, sizeof(Sprite));
        bzero(spr, sizeof(Sprite));
        spr->owned = 1;
    } else {
        bzero(spr, sizeof(Sprite));
        spr->owned = 0;
    }
    if (spr != NULL)
        Sprite_Parse(spr, rom_addr, 0, 0);
    return spr;
}

/* @ 0x800128A8 */
Sprite *Sprite_New(Sprite *spr, u32 rom_addr)
{
    if (spr == NULL) {
        spr = Heap_Alloc(&gGameHeap, sizeof(Sprite));
        bzero(spr, sizeof(Sprite));
        spr->owned = 1;
    } else {
        bzero(spr, sizeof(Sprite));
        spr->owned = 0;
    }
    if (spr != NULL)
        Sprite_Parse(spr, rom_addr, 0x1F, 0);
    return spr;
}

/* @ 0x80012930 */
Sprite *Sprite_NewSharedPalette(Sprite *spr, u32 rom_addr)
{
    if (spr == NULL) {
        spr = Heap_Alloc(&gGameHeap, sizeof(Sprite));
        bzero(spr, sizeof(Sprite));
        spr->owned = 1;
    } else {
        bzero(spr, sizeof(Sprite));
        spr->owned = 0;
    }
    if (spr != NULL)
        Sprite_Parse(spr, rom_addr, 0x1F, 1);
    return spr;
}

/* @ 0x80012A38 */
s32 Sprite_Delete(Sprite *spr)
{
    u16 owned = spr->owned;
    Heap_Free(&gGameHeap, spr->buffer);
    if (owned != 0)
        Heap_Free(&gGameHeap, spr);
    return 0;
}

/* ------------------------------------------------------------------------
 * Screen sprites
 * ------------------------------------------------------------------------ */

extern SpriteGfx gSpriteRectSetupDL[];   /* 0x800389C0 */
extern SpriteGfx gSpriteRectFinishDL[];  /* 0x80038A00 */
extern s32 gScreenWidth;                 /* 0x800C30D0: 320 or 640 */

/* @ 0x800129B8 */
Sprite *Sprite_NewScreen(Sprite *spr, u32 rom_addr)
{
    if (spr == NULL) {
        spr = Heap_Alloc(&gGameHeap, sizeof(Sprite));
        bzero(spr, sizeof(Sprite));
        spr->owned = 1;
    } else {
        bzero(spr, sizeof(Sprite));
        spr->owned = 0;
    }
    if (spr != NULL)
        Sprite_ParseScreen(spr, rom_addr);
    return spr;
}

/* @ 0x8001397C  loads a mode-4 sprite; returns -1 if it isn't one */
s32 Sprite_ParseScreen(Sprite *spr, u32 rom_addr)
{
    SiffIter it;
    u8 *data, *p, *cursor = NULL;
    u32 tag;
    s16 pal_count = 0, tile_count = 0, cmd_count = 0;
    u16 tw2 = 0, th2 = 0;
    u32 pal_words = 0;                    /* endian: for SPRITE_SWAP_NATIVE */

    SiffIter_Init(&it);
    data = Asset_LoadLzss(rom_addr);
    p = data;

    for (;;) {
        p = SiffIter_Enter(&it, (u32 *)p);
        tag = it.level[it.depth].tag;
        if (tag == TAG_SP3D || tag == TAG_SIFF)
            continue;
        if (tag == TAG_INFO) {
            const u8 *q = p;
            s16 dl_count, flags;
            pal_count = (s16)rd16(q + 0);
            tw2 = rd16(q + 6);
            th2 = rd16(q + 8);
            spr->tile_bytes = (u16)(tw2 * th2);
            tile_count = (s16)rd16(q + 10);
            cmd_count = (s16)rd16(q + 12);
            dl_count = (s16)rd16(q + 14);
            spr->origin_x = (s16)rd16(q + 16);
            spr->origin_y = (s16)rd16(q + 18);
            flags = (s16)rd16(q + 20);
            spr->flags = (u8)flags;
            if (flags & 2) {
                spr->tile_w = 0x20;
                spr->tile_h = 0x20;
            } else {
                spr->tile_w = rd16(q + 2);
                spr->tile_h = rd16(q + 4);
            }
            if (!(flags & 4)) {
                Heap_Free(&gGameHeap, data);
                return -1;
            }
            cursor = Heap_Alloc(&gGameHeap, (u32)(pal_count * 2) + (u32)(th2 * (tw2 * tile_count)) +
                                (u32)(cmd_count * 2) + (u32)((dl_count + 0x18) * 8));
            spr->buffer = cursor;
            break;
        }
        p = SiffIter_Leave(&it);
        if (it.depth == -1)
            break;
    }

    p = data;
    for (;;) {
        p = SiffIter_Enter(&it, (u32 *)p);
        tag = it.level[it.depth].tag;
        if (tag == TAG_SIFF || tag == TAG_SP3D)
            continue;
        if (tag == TAG_PAL4 || tag == TAG_PAL8) {
            s32 n = (s32)((u32)pal_count >> 1) - 1;
            u32 *src = (u32 *)p, *dst = (u32 *)cursor;
            spr->format = (tag == TAG_PAL4) ? SPRITE_FMT_CI4 : SPRITE_FMT_CI8;
            spr->palette = (u16 *)cursor;
            pal_words = (u32)(n + 1);
            while (n != -1) { *dst++ = *src++; n--; }
            cursor = (u8 *)dst;
        } else if (tag == TAG_TILE) {
            s32 n = (s32)((u32)(th2 * tw2 * tile_count) >> 2) - 1;
            u32 *src = (u32 *)p, *dst = (u32 *)cursor;
            spr->tiles = cursor;
            while (n != -1) { *dst++ = *src++; n--; }
            cursor = (u8 *)dst;
        } else if (tag == TAG_DLST) {
            s32 n = cmd_count - 1;
            u16 *src = (u16 *)p, *dst = (u16 *)cursor;
            spr->rect_cmds = dst;
            /* endian: the command words are converted to native while copying */
            while (n != -1) { *dst++ = BE16(src); src++; n--; }
            cursor = (u8 *)dst;
            spr->dl = (SpriteGfx *)(((uintptr_t)cursor + 7) & ~(uintptr_t)7);
            SPRITE_SWAP_NATIVE(spr, pal_words, 0);   /* screen sprites are CI4/CI8 */
            Sprite_DrawScreen(spr, 0.0f, 0.0f, 1.0f, 1.0f);
            Heap_Free(&gGameHeap, data);
            return 0;
        }
        p = SiffIter_Leave(&it);
        if (it.depth == -1)
            break;
    }
    Heap_Free(&gGameHeap, data);
    SPRITE_SWAP_NATIVE(spr, pal_words, 0);
    return 0;
}

/* texture-rectangle coordinate clamped like gSPScisTextureRectangle: negative -> 0 */
static inline u32 rect_coord(s32 v)
{
    return (u32)(v & ((s32)~(s16)v >> 31) & 0xFFF);
}

/* the texture-coordinate correction gSPScisTextureRectangle applies when the
 * rectangle starts off-screen */
static inline s32 scis_shift(s32 v, s32 step)
{
    s32 d = ((s16)v * (s16)step) >> 7;
    if ((s16)step < 0)
        return d & ((s32)~d >> 31);         /* max(d, 0) */
    return d & -(d < 1);                    /* min(d, 0) */
}

/* float -> unsigned, as GCC does it */
static inline u32 f2u(f32 f)
{
    if (!(f >= 2.1474836e9f))
        return (u32)(s32)f;
    return (u32)(s32)(f - 2.1474836e9f) | 0x80000000u;
}

/* @ 0x80013D78  rebuilds the screen sprite's display list at (x, y) and scale */
void Sprite_DrawScreen(Sprite *spr, f32 x, f32 y, f32 scale_x, f32 scale_y)
{
    SpriteGfx *g = spr->dl;
    const u16 *cmd = spr->rect_cmds;
    f32 dtdy, dsdx;
    s32 dtdy_i, dsdx_i;

    spr->setup_dl = gSpriteRectSetupDL;
    spr->finish_dl = gSpriteRectFinishDL;
    g[0].w0 = 0xFD100000; g[0].w1 = PTR32(spr->palette);
    g[1].w0 = 0xE8000000; g[1].w1 = 0;
    g[2].w0 = 0xF5000100; g[2].w1 = 0x02000000;
    if (spr->format == SPRITE_FMT_CI8) {
        /* original quirk: the G_RDPLOADSYNC is overwritten by the G_LOADTLUT,
         * so the CI8 palette load is 5 commands, not 6 */
        g[3].w0 = 0xF0000000; g[3].w1 = 0x023FC000;
        g[4].w0 = 0xE7000000; g[4].w1 = 0;
        g += 5;
    } else {
        g[3].w0 = 0xE6000000; g[3].w1 = 0;
        g[4].w0 = 0xF0000000; g[4].w1 = 0x0203C000;
        g[5].w0 = 0xE7000000; g[5].w1 = 0;
        g += 6;
    }

    dtdy = (f32)(1024.0 / (f64)scale_y);
    dtdy_i = (s32)dtdy;
    dsdx = (f32)(1024.0 / (f64)(f32)((f64)scale_x * ((f64)(f32)gScreenWidth / 320.0)));
    dsdx_i = (s32)dsdx;

    for (;;) {
        u16 c = *cmd++;
        switch (c & 0xC000) {
        case 0x8000: {
            u32 w = spr->tile_w, h = spr->tile_h;
            g[0].w0 = 0xFD500000; g[0].w1 = PTR32(spr->tiles + (c & 0x3FFF) * spr->tile_bytes);
            g[1].w0 = 0xF5500000; g[1].w1 = 0x07080200;
            g[2].w0 = 0xE6000000; g[2].w1 = 0;
            g[3].w0 = 0xF3000000;
            if (spr->format == SPRITE_FMT_CI8) {
                g[3].w1 = load_block((s32)(w >> 3), (s32)(w * h + 1) >> 1);
                g[5].w0 = ((((u32)(w + 7) >> 3) & 0x1FF) << 9) | 0xF5480000u;
            } else {
                g[3].w1 = load_block((s32)(w >> 4), (s32)(w * h + 3) >> 2);
                g[5].w0 = ((((u32)((u16)w >> 1) + 7) >> 3 & 0x1FF) << 9) | 0xF5400000u;
            }
            g[4].w0 = 0xE7000000; g[4].w1 = 0;
            g[5].w1 = 0x80200;
            g[6].w0 = 0xF2000000;
            g[6].w1 = ((((u32)(spr->tile_w - 1) * 4) & 0xFFF) << 12) | (((u32)(spr->tile_h - 1) * 4) & 0xFFF);
            g += 7;
            break;
        }
        case 0x0000: {
            u16 c2 = *cmd++;
            f32 fx = (f32)((c & 0x1FF) - spr->origin_x);
            f64 xscale = (f64)(f32)gScreenWidth / 320.0;
            f32 fy = (f32)((c2 & 0x1FF) + spr->origin_y);
            f64 ylo = ((f64)(y + ((fy + (f32)((((c2 >> 9) & 0x1F) + 1) * 2)) * scale_y)) * 4.0) + 0.0;
            s32 xh = (s32)(((f64)(x + ((fx + (f32)(((((s32)((u32)c << 16) >> 25) & 0x1F) + 1) * 2)) * scale_x)) * xscale * 4.0) + 0.0);
            s32 yh = (s32)ylo;
            s32 xl, yl;
            f64 yl_d;
            u32 s_part, st;

            g[0].w0 = (rect_coord(xh) << 12) | (rect_coord(yh) | 0xE4000000u);     /* G_TEXRECT */
            xl = (s32)(((f64)(x + (fx * scale_x)) * xscale * 4.0) + 0.0);
            yl = (s32)(((f64)(y + (fy * scale_y)) * 4.0) + 0.0);
            g[0].w1 = (rect_coord(xl) << 12) | rect_coord(yl);
            g[1].w0 = 0xB4000000;                                                  /* G_RDPHALF_1 */
            if ((s16)xl < 0)
                s_part = (u32)(0x20 - scis_shift(xl, dsdx_i)) << 16;
            else
                s_part = 0x20 << 16;
            yl_d = ((f64)(y + (fy * scale_y)) * 4.0) + 0.0;
            st = s_part | 0x20;
            if (yl_d < 0.0)
                st = s_part | ((u32)(0x20 - scis_shift((s32)yl_d, dtdy_i)) & 0xFFFF);
            g[1].w1 = st;
            g[2].w0 = 0xB3000000;                                                  /* G_RDPHALF_2 */
            g[2].w1 = (f2u(dsdx) << 16) | (f2u(dtdy) & 0xFFFF);
            g += 3;
            break;
        }
        case 0xC000:
            g->w0 = 0xB8000000;                                                    /* G_ENDDL */
            g->w1 = 0;
            return;
        default:
            return;
        }
    }
}

/* ------------------------------------------------------------------------
 * Attaching sprites to scene nodes
 * ------------------------------------------------------------------------ */
#include "scene.h"

/* @ 0x80012A94  node matrix = scale(sx, sy, 1) * translate(x, y, 0) */
void Sprite_Place(Sprite *spr, SceneNode *node, f32 x, f32 y, f32 sx, f32 sy)
{
    MtxF scale;
    Mtx_Translate(node->mtx, x, y, 0.0f);
    Mtx_Scale(scale, sx, sy, 1.0f);
    Mtx_Mul(node->mtx, scale, node->mtx);
    node->dl = spr->dl;
    node->setup_dl = spr->setup_dl;
    node->finish_dl = spr->finish_dl;
}

/* @ 0x80012B38  (same code as Sprite_Place) */
void Sprite_Place2(Sprite *spr, SceneNode *node, f32 x, f32 y, f32 sx, f32 sy)
{
    MtxF scale;
    Mtx_Translate(node->mtx, x, y, 0.0f);
    Mtx_Scale(scale, sx, sy, 1.0f);
    Mtx_Mul(node->mtx, scale, node->mtx);
    node->dl = spr->dl;
    node->setup_dl = spr->setup_dl;
    node->finish_dl = spr->finish_dl;
}

/* @ 0x80012BDC  fixed depth, caller-supplied render set-up */
void Sprite_PlaceCustom(SpriteGfx *setup, Sprite *spr, SceneNode *node, f32 x, f32 y, f32 sx, f32 sy)
{
    MtxF scale;
    union { u32 u; f32 f; } z = { 0xC3CA8E7E };      /* -405.11322 */
    Mtx_Translate(node->mtx, x, y, z.f);
    Mtx_Scale(scale, sx, sy, 1.0f);
    Mtx_Mul(node->mtx, scale, node->mtx);
    node->setup_dl = setup;
    node->dl = spr->dl;
}

/* @ 0x80012C84  screen sprites: centred coordinates, y up */
void Sprite_PlaceScreen(Sprite *spr, SceneNode *node, f32 x, f32 y, f32 sx, f32 sy)
{
    Sprite_DrawScreen(spr, x + 160.0f, 120.0f - y, sx, sy);
    node->dl = spr->dl;
    node->setup_dl = spr->setup_dl;
    node->finish_dl = spr->finish_dl;
}
