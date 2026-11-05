/* Vivid Dolls - object / entity system (RAM 0x80016D20-0x80019D70). See object.h. */
#include "object.h"
#include "siff.h"

extern Heap gGameHeap;                  /* 0x80173600 */
extern u8 D_611F0[];                    /* start of the asset files in ROM (an address constant) */
extern FontDef *D_80038C00[32];         /* font definition tables (filled at run time) */
extern SpriteGfx D_80038C10[];          /* model setup display list */
extern SpriteGfx D_80038C80[], D_80038CC0[], D_80038D00[], D_80038D50[];   /* sprite setup DLs */
extern SpriteGfx D_80038DA0[];          /* screen sprite setup DL */
extern SpriteGfx D_80038DE0[], D_80038E60[], D_80038EA0[], D_80038EE0[];   /* model setup DLs */
/* default DL for Obj_NewDL: a G_ENDDL followed by 4 vertices (loaded by D_80038F68 via
 * gObjDefaultDL + 8). endian: typed as {Gfx, Vtx[4]} so the boot loader swaps the
 * vertices per field, not as Gfx words. */
extern struct { SpriteGfx dl[1]; SpriteVtx vtx[4]; } D_80038F20;
extern SpriteGfx D_80038F68[], D_80038FA8[];
extern char D_8003D8D0[];               /* " " */
extern u8 D_800C1770[];                 /* default lights (ambient at +0, light 0 at +8) */
extern Obj D_800A6B30;                  /* object list head sentinel */
extern Obj D_800A6BE0;                  /* object list tail sentinel */

extern void bzero(void *p, u32 n);
extern void func_8000A924(ObjMtx *out, MtxF m);                       /* MtxF -> fixed-point Mtx */
extern void func_80008540(MtxF out, MtxF in, f32 a, f32 b, f32 c);    /* rotate */
extern void func_800085B4(MtxF out, MtxF in, f32 x, f32 y, f32 z);    /* translate by -pivot */
extern void func_80008750(MtxF out, MtxF in, f32 x, f32 y, f32 z);    /* scale */
extern ObjSceneNode *func_800116B8(s32 kind);                         /* scene node alloc */
extern void func_800117BC(ObjSceneNode *node);                        /* scene node free */

/* The constructors store the ORIGINAL addresses of the transform callbacks so the
 * objects match the game byte for byte; a port points these at the C versions. */
extern void func_800193F0(Obj *obj);    /* = Obj_Transform3D */
extern void func_80019520(Obj *obj);    /* = Obj_Transform2D */
extern void func_800195F4(Obj *obj);    /* = Obj_TransformView */
extern void func_80019688(Obj *obj);    /* = Obj_TransformScreen */
#define XFORM_3D     func_800193F0
#define XFORM_2D     func_80019520
#define XFORM_VIEW   func_800195F4
#define XFORM_SCREEN func_80019688

#define ASSET_ROM(a) (((u32)(a) & 0xFFFFFF) + (u32)D_611F0)

/* F3D commands */
#define G_DL_CALL       0x06000000
#define G_ENDDL         0xB8000000
#define G_PIPESYNC      0xE7000000
#define G_MTX_LOAD      0x01000040

static void set_gfx(SpriteGfx *g, u32 w0, u32 w1) { g->w0 = w0; g->w1 = w1; }

/* ---------------------------------------------------------------- fonts */

/* Load font `id` (< 32) from D_80038C00: one sprite per glyph, until a zero entry
 * (at most 255). Returns NULL for bad ids. */
static Font *font_new(s32 id, Sprite *(*make)(Sprite *, u32))
{
    FontDef *defs, *d;
    Font *font;
    s32 n, i;

    if (id >= 32)
        return NULL;
    defs = D_80038C00[id];
    n = 0;
    for (d = defs; d->rom != 0; d++) {
        n++;
        if (n >= 255)
            break;
    }
    font = Heap_Alloc(&gGameHeap, sizeof(Font));
    font->sprites = Heap_Alloc(&gGameHeap, (u32)n * 4);
    font->glyphs = Heap_Alloc(&gGameHeap, (u32)n * 4);
    for (i = 0, d = defs; i < n; i++, d++) {
        if (d->rom == 0)
            break;
        font->sprites[i] = make(NULL, ASSET_ROM(d->rom));
        font->glyphs[i] = &d->glyph;
    }
    font->count = n;
    return font;
}

/* @ 0x80016D20 */
/* Font with one sprite per glyph (see font_new). */
Font *Font_New(s32 id) { return font_new(id, Sprite_New); }

/* @ 0x80016E58 */
/* Same as Font_New, glyph sprites share one palette. */
Font *Font_NewSharedPalette(s32 id) { return font_new(id, Sprite_NewSharedPalette); }

/* @ 0x80016F90 */
/* Free a font and all its glyph sprites. */
s32 Font_Delete(Font *font)
{
    s32 i;

    for (i = 0; i < font->count; i++)
        font->sprites[i] = (Sprite *)Sprite_Delete(font->sprites[i]);
    Heap_Free(&gGameHeap, font->sprites);
    Heap_Free(&gGameHeap, font->glyphs);
    Heap_Free(&gGameHeap, font);
    return 0;
}

/* @ 0x80017030 */
/* Allocate a text display-list buffer (contents left uninitialised except `cur`). */
TextBuf *TextBuf_New(void)
{
    TextBuf *tb = Heap_Alloc(&gGameHeap, sizeof(TextBuf));
    if (tb != NULL)
        tb->cur = 0;
    return tb;
}

/* @ 0x8001705C */
/* Free a TextBuf. */
void TextBuf_Delete(TextBuf *tb)
{
    Heap_Free(&gGameHeap, tb);
}

/* Shared glyph loop of Text_Build / Text_BuildTlut: up to 32 characters, one
 * matrix (relative to the previous glyph) + one sprite DL call per glyph. A space
 * (glyph 0) only advances; Text_Build also writes two empty commands for it
 * (`space_nops`), Text_BuildTlut writes nothing. */
static SpriteGfx *text_glyphs(SpriteGfx *g, ObjMtx *mtx, Font *font, const char *str, f32 x, f32 y,
                              s32 space_nops)
{
    const u8 *s = (const u8 *)str;
    const u8 *end = s + 32;
    f32 prev_x = 0.0f, prev_y = 0.0f;
    MtxF tmp;

    for (; *s != 0; ) {
        u32 idx = (u8)(*s - 0x20);
        FontGlyph *gl;

        if (idx != 0) {
            gl = font->glyphs[idx];
            Mtx_Translate(tmp, (x - (f32)gl->x0) - prev_x, (y + (f32)gl->y) - prev_y, 0.0f);
            gl = font->glyphs[idx];
            prev_x = x - (f32)gl->x0;
            prev_y = y + (f32)gl->y;
            func_8000A924(mtx, tmp);
            set_gfx(&g[0], G_MTX_LOAD, (u32)mtx);
            set_gfx(&g[1], G_DL_CALL, (u32)font->sprites[idx]->dl);
            mtx++;
            g += 2;
        } else if (space_nops) {
            set_gfx(&g[0], 0, 0);
            set_gfx(&g[1], 0, 0);
            g += 2;
        }
        gl = font->glyphs[idx];
        s++;
        x += (f32)(gl->x1 - gl->x0);
        if (!(s < end))
            break;
    }
    return g;
}

/* @ 0x80017080 */
/* Build the display list for `str` at (x, y) into the TextBuf's other buffer and
 * return it (double buffering: tb->cur flips each call). */
SpriteGfx *Text_Build(TextBuf *tb, Font *font, const char *str, f32 x, f32 y)
{
    s32 b = tb->cur ^ 1;
    SpriteGfx *g = tb->dl[b];

    tb->cur = b;
    set_gfx(g++, 0xBA001701, 0x00800000);
    g = text_glyphs(g, tb->mtx[b], font, str, x, y, 1);
    set_gfx(g++, G_PIPESYNC, 0);
    set_gfx(g++, 0xBA001701, 0);
    set_gfx(g, G_ENDDL, 0);
    return tb->dl[b];
}

/* @ 0x80017304 */
/* Text_Build that first loads a palette (TLUT) from `tlut`: 256 colours if the
 * font's first sprite is 8-bit (format 1), else 16. */
SpriteGfx *Text_BuildTlut(TextBuf *tb, Font *font, const char *str, f32 x, f32 y, void *tlut)
{
    s32 b = tb->cur ^ 1;
    SpriteGfx *g = tb->dl[b];

    tb->cur = b;
    set_gfx(g++, 0xBA001701, 0x00800000);
    set_gfx(g++, 0xFD100000, (u32)tlut);
    set_gfx(g++, 0xE8000000, 0);
    set_gfx(g++, 0xF5000100, 0x02000000);
    set_gfx(g++, 0xE6000000, 0);
    set_gfx(g++, 0xF0000000, font->sprites[0]->format == 1 ? 0x023FC000 : 0x0203C000);
    set_gfx(g++, G_PIPESYNC, 0);
    g = text_glyphs(g, tb->mtx[b], font, str, x, y, 0);
    set_gfx(g++, G_PIPESYNC, 0);
    set_gfx(g++, 0xBA001701, 0);
    set_gfx(g, G_ENDDL, 0);
    return tb->dl[b];
}

/* @ 0x8001763C */
/* Build text and, if `node` is given, hook the result (plus the font's sprite
 * set-up/finish lists) into it. Returns tb. */
TextBuf *Text_Attach(ObjSceneNode *node, TextBuf *tb, Font *font, const char *str, f32 x, f32 y)
{
    SpriteGfx *dl = Text_Build(tb, font, str, x, y);
    if (node != NULL) {
        node->u.draw.dl = dl;
        node->u.draw.setup_dl = font->sprites[0]->setup_dl;
        node->u.draw.finish_dl = font->sprites[0]->finish_dl;
    }
    return tb;
}

/* @ 0x800176C4 */
/* Text_Attach with Text_BuildTlut. */
TextBuf *Text_AttachTlut(ObjSceneNode *node, TextBuf *tb, Font *font, const char *str,
                         f32 x, f32 y, void *tlut)
{
    SpriteGfx *dl = Text_BuildTlut(tb, font, str, x, y, tlut);
    if (node != NULL) {
        node->u.draw.dl = dl;
        node->u.draw.setup_dl = font->sprites[0]->setup_dl;
        node->u.draw.finish_dl = font->sprites[0]->finish_dl;
    }
    return tb;
}

/* @ 0x80017754 */
/* Width in pixels of (up to 32 characters of) `str`. */
s32 Font_TextWidth(Font *font, const char *str)
{
    const u8 *s = (const u8 *)str;
    const u8 *end = s + 32;
    s32 w = 0;

    while (*s != 0) {
        FontGlyph *gl = font->glyphs[(u8)(*s - 0x20)];
        s++;
        w += gl->x1 - gl->x0;
        if (!(s < end))
            break;
    }
    return w;
}

/* ---------------------------------------------------------------- allocation list */

/* @ 0x800177B0 */
/* Empty list between two sentinel nodes. */
void AllocList_Init(AllocNode *head, AllocNode *tail)
{
    head->next = NULL; head->prev = NULL; head->heap = NULL; head->ptr = NULL;
    tail->next = NULL; tail->prev = NULL; tail->heap = NULL; tail->ptr = NULL;
    head->prev = NULL;
    tail->next = NULL;
    head->next = tail;
    tail->prev = head;
}

/* @ 0x800177F4 */
/* Record (heap, ptr) in a new node (from the game heap) before `tail`. */
void AllocList_Add(Heap *heap, void *ptr, AllocNode *tail)
{
    AllocNode *n = Heap_Alloc(&gGameHeap, sizeof(AllocNode));

    n->next = NULL; n->prev = NULL; n->heap = NULL; n->ptr = NULL;
    tail->prev->next = n;
    n->next = tail;
    n->prev = tail->prev;
    tail->prev = n;
    n->heap = heap;
    n->ptr = ptr;
}

/* @ 0x80017878 */
/* Free every recorded block and its node (nodes go back to `heap`). */
void AllocList_FreeAll(Heap *heap, AllocNode *head)
{
    AllocNode *n = head->next;
    AllocNode *next;

    while (n->next != NULL) {
        next = n->next;
        Heap_Free(n->heap, n->ptr);
        n->prev->next = n->next;
        n->next->prev = n->prev;
        Heap_Free(heap, n);
        n = next;
    }
}

/* ---------------------------------------------------------------- models */

static Model *model_new(Model *m, u32 rom, s32 lighting)
{
    if (m == NULL) {
        m = Heap_Alloc(&gGameHeap, sizeof(Model));
        bzero(m, sizeof(Model));
        m->owned = 1;
    } else {
        bzero(m, sizeof(Model));
        m->owned = 0;
    }
    if (m != NULL)
        Model_Load(m, rom, lighting);
    return m;
}

/* @ 0x80017900 */
/* Load a model (allocated if m is NULL), with lighting set-up commands. */
Model *Model_New(Model *m, u32 rom) { return model_new(m, rom, 1); }

/* @ 0x80017984 */
/* Model_New without the lighting/texture-gen commands. */
Model *Model_NewUnlit(Model *m, u32 rom) { return model_new(m, rom, 0); }

/* @ 0x80017A08 */
/* Free a model's buffer, and the Model itself if Model_New allocated it. */
s32 Model_Delete(Model *m)
{
    u16 owned = m->owned;
    Heap_Free(&gGameHeap, m->buffer);
    if (owned)
        Heap_Free(&gGameHeap, m);
    return 0;
}

#if !VD_BIG_ENDIAN
/* endian (little-endian builds only): convert the copied model sections to
 * native order once, before the texture records are relocated and the command
 * stream is translated:
 *  - vertices ([off1,off2), F3D Vtx handed straight to the RSP): the six 16-bit
 *    fields of each 16-byte vertex (x y z flag s t); the colour bytes stay;
 *  - texture records: unk0, width, height, aux, image;
 *  - RGBA16 images (format 0 in a 10xx command): the 16-bit texels, once per
 *    image, found by walking the (big-endian) command stream;
 *  - [off2,off3) (section2), CI4/CI8 images and the aux data are left as they
 *    are (byte data / unknown layout; nothing in the game reads them as words).
 * No model asset exists in the ROM, so this is untested on real data. */
static void model_swap_native(Model *m, u32 vtx_bytes, u32 ntex, u32 img_bytes, const u8 *cmd)
{
    u32 done_img[64];
    u32 ndone = 0, i;
    u8 *v;

    for (v = (u8 *)m->vertices; v + 16 <= (u8 *)m->vertices + vtx_bytes; v += 16)
        SwapArray16(v, 6);
    for (i = 0; i < ntex; i++) {
        ModelTex *t = &m->textures[i];
        t->unk0 = SWAP32(t->unk0);
        t->width = SWAP16(t->width);
        t->height = SWAP16(t->height);
        t->aux = (u8 *)(uintptr_t)SWAP32((u32)(uintptr_t)t->aux);
        t->image = (u8 *)(uintptr_t)SWAP32((u32)(uintptr_t)t->image);
    }
    for (;;) {
        u32 c = BE16(cmd);
        cmd += 2;
        if ((c & 0xC000) == 0x4000) {
            cmd += 2;
        } else if ((c & 0xC000) == 0x8000) {
            u32 k, off, size;
            ModelTex *t;
            if ((c & 0x300) != 0 || (c & 0x3F) >= ntex)
                continue;
            t = &m->textures[c & 0x3F];
            off = (u32)(uintptr_t)t->image;            /* still relative to the images */
            size = (u32)(t->width & 0x7F) * (u32)(t->height & 0x7F) * 2;
            for (k = 0; k < ndone && done_img[k] != off; k++) {
            }
            if (k < ndone || off >= img_bytes || size > img_bytes - off)
                continue;
            if (ndone < 64)
                done_img[ndone++] = off;
            SwapArray16(m->images + off, size / 2);
        } else if ((c & 0xC000) == 0xC000) {
            break;
        }
    }
}
#define MODEL_SWAP_NATIVE(m, vb, nt, ib, cmd) model_swap_native(m, vb, nt, ib, cmd)
#else
#define MODEL_SWAP_NATIVE(m, vb, nt, ib, cmd) ((void)0)
#endif

/* @ 0x80017A64 */
/* Load a model asset (LZSS). Header: u32[8] = { header0, off1..off6, gfx count },
 * sections [off1,off3) vertices etc., [off3,off4) 16-byte texture records,
 * [off4,off5) images, [off5,off6) aux data; then a u16 command stream that is
 * translated into an F3D display list after the copied data:
 *   00xx  triangle (three vertex nibbles, x10)
 *   01xx  load vertices (next word = first vertex); with `lighting`, a change in
 *         bits 8-9 also switches texture generation on (0x200) or off
 *   10xx  load texture record (bits 0-5): bits 8-9 = 0 RGBA16, 1 CI4, 2 CI8, 3 none
 *   11xx  end */
s32 Model_Load(Model *m, u32 rom, s32 lighting)
{
    u32 *hdr;
    u8 *src, *dst;
    u32 o1, o2, o3, o4, o5, o6, n;
    SpriteGfx *g;
    u16 *cmd;
    u16 mode = 0;
    s32 i;

    m->setup_dl = D_80038C10;
    hdr = (u32 *)Asset_LoadLzss(rom);
    src = (u8 *)hdr + 0x20;
    /* endian: the header and the command stream are big-endian ROM data */
    o1 = BE32(&hdr[1]); o2 = BE32(&hdr[2]); o3 = BE32(&hdr[3]);
    o4 = BE32(&hdr[4]); o5 = BE32(&hdr[5]); o6 = BE32(&hdr[6]);
    dst = Heap_Alloc(&gGameHeap, (BE32(&hdr[7]) + 0x18) * 8 + (o6 - o1));
    m->header0 = (s16)BE32(&hdr[0]);
    m->buffer = dst;
    m->vertices = (SpriteVtx *)dst;
    m->section2 = dst + (o3 - o1) - (o3 - o2);

    n = o3 - o1; Mem_Copy(dst, src, n); dst += n; src += n;
    m->textures = (ModelTex *)dst;
    n = o4 - o3; Mem_Copy(dst, src, n); dst += n; src += n;
    m->images = dst;
    n = o5 - o4; Mem_Copy(dst, src, n); dst += n; src += n;
    m->aux = dst;
    n = o6 - o5; Mem_Copy(dst, src, n); dst += n; src += n;
    MODEL_SWAP_NATIVE(m, o2 - o1, (o4 - o3) / 16, o5 - o4, src);

    for (i = 0; i < (s32)(o4 - o3) / 16; i++) {
        ModelTex *t = &m->textures[i];
        t->aux += (u32)m->aux;
        t->image += (u32)m->images;
    }

    g = (SpriteGfx *)dst;
    m->dl = g;
    cmd = (u16 *)src;
    for (;;) {
        u32 c = BE16(cmd++);
        u32 op = c & 0xC000;

        if (op == 0x4000) {
            u32 v;
            if ((mode != (c & 0x300)) & (lighting != 0)) {
                if ((c & 0x300) == 0x200) {
                    set_gfx(g++, 0xB7000000, 0x40000);
                    set_gfx(g++, 0xBB000001, 0x07C007C0);
                } else {
                    set_gfx(g++, 0xB6000000, 0x40000);
                    set_gfx(g++, 0xBB000001, 0x80008000);
                }
                mode = c & 0x300;
            }
            v = BE16(cmd++);
            set_gfx(g++, ((((c & 0xF) << 4) | ((c & 0xF0) >> 4)) << 16) | (((c & 0xF) + 1) << 4) | 0x04000000,
                    (u32)m->vertices + v * 16);
        } else if (op == 0) {
            set_gfx(g++, 0xBF000000, ((c & 0xF) * 10 << 16) | (((c & 0xF0) >> 4) * 10 << 8) |
                                     (((c & 0xF00) >> 8) * 10));
        } else if (op == 0x8000) {
            ModelTex *t = &m->textures[c & 0x3F];
            u32 w = t->width, h = t->height;
            u32 sw = Math_Ctz16(w);         /* log2 width/height (mask) */
            u32 sh = Math_Ctz16(h);
            u32 cms = (w >> 6) & 2;
            u32 cmt = (h >> 6) & 2;
            u32 tile = (cmt << 18) | ((sh & 0xF) << 14) | (cms << 8) | ((sw & 0xF) << 4);
            s32 lrs, dxt, words, lines;
            u32 mode2 = c & 0x300;

            if (mode2 == 0) {
                /* 16-bit RGBA */
                set_gfx(g++, 0xFD100000, (u32)t->image);
                set_gfx(g++, 0xF5100000, tile | 0x07000000);
                set_gfx(g++, 0xE6000000, 0);
                words = (s32)(w & 0x7F) / 4;
                dxt = words > 0 ? words + 0x7FF : 0x800;
                lrs = (s32)((w & 0x7F) * (h & 0x7F)) - 1;
                if (lrs >= 0x800)
                    lrs = 0x7FF;
                set_gfx(g++, 0xF3000000, ((lrs & 0xFFF) << 12) | 0x07000000 |
                                         (words > 0 ? (u32)(dxt / words) & 0xFFF : (u32)dxt & 0xFFF));
                set_gfx(g++, G_PIPESYNC, 0);
                set_gfx(g++, ((((w & 0x7F) * 2 + 7) >> 3) << 9) | 0xF5100000, tile);
                set_gfx(g++, 0xF2000000, ((((w & 0x7F) - 1) * 4 & 0xFFF) << 12) |
                                         (((h & 0x7F) - 1) * 4 & 0xFFF));
            } else if (mode2 == 0x100 || mode2 == 0x200) {
                /* 4-bit (0x100) or 8-bit (0x200) colour index, loaded as 16-bit words */
                u32 lw0;
                set_gfx(g++, 0xFD500000, (u32)t->image);
                set_gfx(g++, 0xF5500000, tile | 0x07000000);
                set_gfx(g++, 0xE6000000, 0);
                if (mode2 == 0x100) {
                    lines = (s32)(w >> 3);
                    lrs = ((s32)(w * h + 1) >> 1) - 1;
                } else {
                    lines = (s32)(w >> 4);
                    lrs = ((s32)(w * h + 3) >> 2) - 1;
                }
                dxt = lines != 0 ? lines + 0x7FF : 0x800;
                if (lrs >= 0x800)
                    lrs = 0x7FF;
                set_gfx(g++, 0xF3000000, ((lrs & 0xFFF) << 12) | 0x07000000 |
                                         (lines != 0 ? (u32)(dxt / lines) & 0xFFF : (u32)dxt & 0xFFF));
                set_gfx(g++, G_PIPESYNC, 0);
                if (mode2 == 0x100)
                    lw0 = (((s32)(w + 7) >> 3) & 0x1FF) << 9 | 0xF5480000;
                else
                    lw0 = (((s32)((w >> 1) + 7) >> 3) & 0x1FF) << 9 | 0xF5400000;
                set_gfx(g++, lw0, tile);
                set_gfx(g++, 0xF2000000, (((w - 1) * 4 & 0xFFF) << 12) | ((h - 1) * 4 & 0xFFF));
            }
        } else {
            set_gfx(g, G_ENDDL, 0);
            break;
        }
    }
    Heap_Free(&gGameHeap, hdr);
    return 0;
}

/* @ 0x800182E4 */
/* Byte copy; returns dst. */
void *Mem_Copy(void *dst, const void *src, u32 n)
{
    u8 *d = dst;
    const u8 *s = src;
    while (n-- != 0)
        *d++ = *s++;
    return dst;
}

/* @ 0x8001831C */
/* Number of trailing zero bits of a 16-bit value (log2 of a power of two).
 * Quirk: loops forever if the low 16 bits are 0. */
u16 Math_Ctz16(u32 x)
{
    u32 n = 0;
    if (!(x & 1)) {
        do {
            x = (x & 0xFFFF) >> 1;
            n++;
        } while (!(x & 1));
    }
    return (u16)n;
}

/* ---------------------------------------------------------------- object constructors */

/* @ 0x80018350 */
/* Raw display-list object; NULL setup/finish lists default to D_80038F20. */
Obj *Obj_NewDL(ObjSceneNode *parent, SpriteGfx *setup, void *dl, SpriteGfx *finish)
{
    Obj *obj = Obj_Create(parent, OBJ_DL);
    obj->transform = XFORM_3D;
    obj->data = dl;
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)(setup == NULL ? D_80038F20.dl : setup));
    set_gfx(&obj->finish[0], G_DL_CALL, (u32)(finish == NULL ? D_80038F20.dl : finish));
    return obj;
}

/* @ 0x800183F4 */
/* Raw display-list object using the built-in default lists. */
Obj *Obj_NewDefaultDL(ObjSceneNode *parent)
{
    Obj *obj = Obj_Create(parent, OBJ_DL);
    obj->transform = XFORM_3D;
    obj->data = D_80038FA8;
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038F68);
    return obj;
}

/* @ 0x8001843C */
/* Viewport object (no display lists; its node is a kind-4 viewport node). */
Obj *Obj_NewView(ObjSceneNode *parent)
{
    Obj *obj = Obj_Create(parent, OBJ_VIEW);
    obj->transform = XFORM_VIEW;
    return obj;
}

/* @ 0x80018464 */
/* Model object that loads and owns its model. */
Obj *Obj_NewModel(ObjSceneNode *parent, u32 rom)
{
    Obj *obj = Obj_Create(parent, OBJ_MODEL);
    obj->transform = XFORM_3D;
    obj->flags |= OBJF_OWNS_DATA;
    obj->data = Model_New(NULL, rom);
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038DE0);
    set_gfx(&obj->finish[0], G_ENDDL, 0);
    return obj;
}

static void sprite_finish2(Obj *obj)
{
    set_gfx(&obj->finish[0], G_PIPESYNC, 0);
    set_gfx(&obj->finish[1], 0xBA000C02, 0x2000);
}

static void sprite_finish3(Obj *obj)
{
    sprite_finish2(obj);
    set_gfx(&obj->finish[2], 0xBA001701, 0);
}

static Obj *new_sprite(ObjSceneNode *parent, Sprite *spr, SpriteGfx *setup, s32 owned, s32 three)
{
    Obj *obj = Obj_Create(parent, OBJ_SPRITE);
    obj->transform = XFORM_2D;
    if (owned)
        obj->flags |= OBJF_OWNS_DATA;
    obj->data = spr;
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)setup);
    if (three)
        sprite_finish3(obj);
    else
        sprite_finish2(obj);
    return obj;
}

/* Owned sprite: the sprite is loaded after the object is created. */
static Obj *new_owned_sprite(ObjSceneNode *parent, u32 rom, Sprite *(*make)(Sprite *, u32),
                             SpriteGfx *setup, s32 three)
{
    Obj *obj = Obj_Create(parent, OBJ_SPRITE);
    obj->transform = XFORM_2D;
    obj->flags |= OBJF_OWNS_DATA;
    obj->data = make(NULL, rom);
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)setup);
    if (three)
        sprite_finish3(obj);
    else
        sprite_finish2(obj);
    return obj;
}

/* @ 0x800184E4 */
/* Sprite object that loads and owns a plain sprite (setup D_80038C80). */
Obj *Obj_NewSpritePlain(ObjSceneNode *parent, u32 rom)
{
    return new_owned_sprite(parent, rom, Sprite_NewPlain, D_80038C80, 0);
}

/* @ 0x80018578 */
/* Same as Obj_NewSpritePlain with set-up list D_80038CC0. */
Obj *Obj_NewSpritePlain2(ObjSceneNode *parent, u32 rom)
{
    return new_owned_sprite(parent, rom, Sprite_NewPlain, D_80038CC0, 0);
}

/* @ 0x8001860C */
/* Switch a sprite object to set-up list D_80038CC0. */
Obj *Obj_UseSetupCC0(Obj *obj)
{
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038CC0);
    return obj;
}

/* @ 0x80018628 */
/* Switch a sprite object to set-up list D_80038C80. */
Obj *Obj_UseSetupC80(Obj *obj)
{
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038C80);
    return obj;
}

/* @ 0x80018644 */
/* Sprite object for an existing plain sprite (not owned). */
Obj *Obj_NewSpritePlainRef(ObjSceneNode *parent, Sprite *spr)
{
    return new_sprite(parent, spr, D_80038C80, 0, 0);
}

/* @ 0x800186B0 */
/* Sprite object that loads and owns a sprite (setup D_80038D00). */
Obj *Obj_NewSprite(ObjSceneNode *parent, u32 rom)
{
    return new_owned_sprite(parent, rom, Sprite_New, D_80038D00, 1);
}

/* @ 0x80018754 */
/* Same as Obj_NewSprite with set-up list D_80038D50. */
Obj *Obj_NewSprite2(ObjSceneNode *parent, u32 rom)
{
    return new_owned_sprite(parent, rom, Sprite_New, D_80038D50, 1);
}

/* @ 0x800187F8 */
/* Sprite object for an existing sprite (not owned). */
Obj *Obj_NewSpriteRef(ObjSceneNode *parent, Sprite *spr)
{
    return new_sprite(parent, spr, D_80038D00, 0, 1);
}

static void screen_finish(Obj *obj)
{
    set_gfx(&obj->finish[0], G_PIPESYNC, 0);
    set_gfx(&obj->finish[1], 0xBA001301, 0x80000);
}

/* @ 0x80018874 */
/* Screen-sprite object that loads and owns its sprite. */
Obj *Obj_NewScreen(ObjSceneNode *parent, u32 rom)
{
    Obj *obj = Obj_Create(parent, OBJ_SCREEN);
    obj->transform = XFORM_SCREEN;
    obj->flags |= OBJF_OWNS_DATA;
    obj->data = Sprite_NewScreen(NULL, rom);
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038DA0);
    screen_finish(obj);
    return obj;
}

/* @ 0x80018908 */
/* Text object with its own TextBuf; the text is built at (0, 0). */
Obj *Obj_NewText(ObjSceneNode *parent, Font *font, const char *str)
{
    Obj *obj = Obj_Create(parent, OBJ_TEXT);
    obj->transform = XFORM_3D;
    obj->flags |= OBJF_OWNS_DATA;
    obj->data = TextBuf_New();
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038D00);
    obj->data2 = font;
    Text_Build(obj->data, obj->data2, str, 0.0f, 0.0f);
    sprite_finish3(obj);
    return obj;
}

/* @ 0x800189D4 */
/* Obj_NewText that loads a palette first (Text_BuildTlut). */
Obj *Obj_NewTextTlut(ObjSceneNode *parent, Font *font, const char *str, void *tlut)
{
    Obj *obj = Obj_Create(parent, OBJ_TEXT);
    obj->transform = XFORM_3D;
    obj->flags |= OBJF_OWNS_DATA;
    obj->data = TextBuf_New();
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038D00);
    obj->data2 = font;
    Text_BuildTlut(obj->data, obj->data2, str, 0.0f, 0.0f, tlut);
    sprite_finish3(obj);
    return obj;
}

/* @ 0x80018AB0 */
/* Model object for an existing model (not owned). */
Obj *Obj_NewModelRef(ObjSceneNode *parent, Model *m)
{
    Obj *obj = Obj_Create(parent, OBJ_MODEL);
    obj->transform = XFORM_3D;
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038DE0);
    obj->data = m;
    set_gfx(&obj->finish[0], G_ENDDL, 0);
    return obj;
}

/* @ 0x80018B08 */
/* Screen-sprite object for an existing screen sprite (not owned). */
Obj *Obj_NewScreenRef(ObjSceneNode *parent, Sprite *spr)
{
    Obj *obj = Obj_Create(parent, OBJ_SCREEN);
    obj->transform = XFORM_SCREEN;
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038DA0);
    obj->data = spr;
    screen_finish(obj);
    return obj;
}

/* @ 0x80018B74 */
/* Text object using a caller-supplied TextBuf. */
Obj *Obj_NewTextRef(ObjSceneNode *parent, Font *font, TextBuf *tb, const char *str)
{
    Obj *obj = Obj_Create(parent, OBJ_TEXT);
    obj->transform = XFORM_3D;
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038D00);
    obj->data = tb;
    obj->data2 = font;
    Text_Build(obj->data, obj->data2, str, 0.0f, 0.0f);
    sprite_finish3(obj);
    return obj;
}

/* @ 0x80018C3C */
/* Obj_NewTextRef with Text_BuildTlut. */
Obj *Obj_NewTextRefTlut(ObjSceneNode *parent, Font *font, TextBuf *tb, const char *str, void *tlut)
{
    Obj *obj = Obj_Create(parent, OBJ_TEXT);
    obj->transform = XFORM_3D;
    set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038D00);
    sprite_finish3(obj);
    obj->data = tb;
    obj->data2 = font;
    Text_BuildTlut(obj->data, obj->data2, str, 0.0f, 0.0f, tlut);
    return obj;
}

/* @ 0x80018D14 */
/* Rebuild a text object's string and re-attach it. */
Obj *Obj_SetText(Obj *obj, const char *str)
{
    Text_Build(obj->data, obj->data2, str, 0.0f, 0.0f);
    obj->flags |= OBJF_ATTACH;
    return obj;
}

/* @ 0x80018D64 */
/* Obj_SetText with Text_BuildTlut. */
Obj *Obj_SetTextTlut(Obj *obj, const char *str, void *tlut)
{
    Text_BuildTlut(obj->data, obj->data2, str, 0.0f, 0.0f, tlut);
    obj->flags |= OBJF_ATTACH;
    return obj;
}

/* ---------------------------------------------------------------- object list and setters */

/* @ 0x80018DBC */
/* Clear both sentinels and link them into an empty list. */
void ObjList_Init(void)
{
    u32 *p;
    s32 i;

    p = (u32 *)&D_800A6B30;
    for (i = 0; i < 0x2C; i++)
        p[i] = 0;
    p = (u32 *)&D_800A6BE0;
    for (i = 0; i < 0x2C; i++)
        p[i] = 0;
    D_800A6B30.prev = NULL;
    D_800A6BE0.next = NULL;
    D_800A6B30.next = &D_800A6BE0;
    D_800A6BE0.prev = &D_800A6B30;
}

/* @ 0x80018E20 */
/* Request deletion (done over the next two Obj_UpdateAll calls). */
s32 Obj_Kill(Obj *obj)
{
    obj->flags |= OBJF_KILL;
    return 0;
}

/* @ 0x80018E34 */
/* Replace the object's data (ignored if the object owns its data). */
Obj *Obj_SetData(Obj *obj, void *data)
{
    if (obj->flags & OBJF_OWNS_DATA)
        return obj;
    switch (obj->type) {
    case OBJ_SCREEN:
        obj->flags |= OBJF_DIRTY;
        /* fall through */
    case OBJ_TEXT:
    case OBJ_SPRITE:
    case OBJ_MODEL:
        obj->data = data;
        break;
    }
    obj->flags |= OBJF_ATTACH;
    return obj;
}

/* @ 0x80018E90 */
/* Use model set-up list D_80038EE0 (and re-attach). */
Obj *Obj_SetModelAlt2(Obj *obj) { obj->flags |= OBJF_MODEL_ALT2 | OBJF_ATTACH; return obj; }

/* @ 0x80018EA4 */
/* Use model set-up list D_80038E60 / D_80038EA0 (and re-attach). */
Obj *Obj_SetModelAlt(Obj *obj) { obj->flags |= OBJF_MODEL_ALT | OBJF_ATTACH; return obj; }

/* @ 0x80018EB8 */
/* Primitive colour (gDPSetPrimColor) in the object's set-up list. */
Obj *Obj_SetColor(Obj *obj, u32 r, u32 g, u32 b, u32 a)
{
    obj->setup[4].w0 = 0xFA000000;
    obj->setup[4].w1 = (r << 24) | ((g & 0xFF) << 16) | ((b & 0xFF) << 8) | (a & 0xFF);
    return obj;
}

/* @ 0x80018EF4 */
/* Set the position from integers and mark the transform dirty. */
Obj *Obj_SetPos(Obj *obj, s32 x, s32 y, s32 z)
{
    obj->pos[0] = (f32)x;
    obj->pos[1] = (f32)y;
    obj->pos[2] = (f32)z;
    obj->flags |= OBJF_DIRTY;
    return obj;
}

/* @ 0x80018F38 */
/* Mirror in X (3D objects) and re-transform. */
Obj *Obj_FlipX(Obj *obj) { obj->flags |= OBJF_FLIP_X | OBJF_DIRTY | OBJF_ATTACH; return obj; }

/* @ 0x80018F4C */
/* Mirror in Y (3D objects) and re-transform. */
Obj *Obj_FlipY(Obj *obj) { obj->flags |= OBJF_FLIP_Y | OBJF_DIRTY | OBJF_ATTACH; return obj; }

/* @ 0x80018F60 */
/* Mirror in Z (3D objects) and re-transform. */
Obj *Obj_FlipZ(Obj *obj) { obj->flags |= OBJF_FLIP_Z | OBJF_DIRTY | OBJF_ATTACH; return obj; }

/* @ 0x80018F74 */
/* Mark the transform dirty. */
Obj *Obj_Invalidate(Obj *obj) { obj->flags |= OBJF_DIRTY; return obj; }

/* @ 0x80018F88 */
/* Make visible (attach next update). */
Obj *Obj_Show(Obj *obj)
{
    obj->flags = (obj->flags & ~OBJF_DETACH) | OBJF_ATTACH;
    return obj;
}

/* @ 0x80018FA0 */
/* Hide (detach next update). */
Obj *Obj_Hide(Obj *obj)
{
    obj->flags = (obj->flags & ~OBJF_ATTACH) | OBJF_DETACH;
    return obj;
}

/* @ 0x80018FB8 */
/* Set the per-frame update callback and its argument. */
Obj *Obj_SetUpdate(Obj *obj, ObjFunc fn, void *arg)
{
    obj->update = fn;
    obj->update_arg = arg;
    return obj;
}

/* @ 0x80018FC8 */
/* Remove the update callback. */
Obj *Obj_ClearUpdate(Obj *obj) { obj->update = NULL; return obj; }

/* @ 0x80018FD4 */
/* Remove the transform callback (e.g. after Obj_SetMatrix). */
Obj *Obj_ClearTransform(Obj *obj) { obj->transform = NULL; return obj; }

/* @ 0x80018FE0 */
/* Set the node matrix directly. */
Obj *Obj_SetMatrix(Obj *obj, MtxF m)
{
    Mtx_Copy(obj->node->mtx, m);
    return obj;
}

/* @ 0x80019010 */
/* Use other lights: light 0 at lights+0x48, ambient at lights+0x40. */
Obj *Obj_SetLights(Obj *obj, u8 *lights)
{
    set_gfx(&obj->setup[2], 0x03860010, (u32)(lights + 0x48));
    set_gfx(&obj->setup[3], 0x03880010, (u32)(lights + 0x40));
    return obj;
}

/* ---------------------------------------------------------------- per-frame update */

/* @ 0x80019040 */
/* Run every object once: deferred deletion (kill -> free node -> free object),
 * update callback, transform callback when dirty, then attach/detach. */
void Obj_UpdateAll(void)
{
    Obj *obj = D_800A6B30.next;
    Obj *next = obj->next;

    while (next != NULL) {
        u16 f = obj->flags;
        if (f & OBJF_KILL) {
            Obj_FreeNode(obj);
            obj->flags = (obj->flags & ~OBJF_KILL) | OBJF_DEAD;
        } else if (f & OBJF_DEAD) {
            Obj_FreeData(obj);
            obj->prev->next = obj->next;
            obj->next->prev = obj->prev;
            Heap_Free(&gGameHeap, obj);
        } else {
            if (obj->update != NULL)
                obj->update(obj);
            if (obj->flags & OBJF_DIRTY) {
                if (obj->transform != NULL)
                    obj->transform(obj);
                obj->flags &= ~OBJF_DIRTY;
            }
            f = obj->flags;
            if (f & OBJF_DETACH) {
                Obj_Detach(obj);
                obj->flags &= ~(OBJF_DETACH | OBJF_ATTACH);
            } else if (f & OBJF_ATTACH) {
                Obj_Attach(obj);
                obj->flags &= ~OBJF_ATTACH;
            }
        }
        obj = next;
        next = next->next;
    }
}

/* @ 0x80019174 */
/* Allocate a zeroed object, append it to the object list and give it a scene node
 * (a viewport node for OBJ_VIEW, else a drawable node with default lights,
 * white primitive colour and scale 1) linked last under `parent`. */
Obj *Obj_Create(ObjSceneNode *parent, s32 type)
{
    Obj *obj = Heap_Alloc(&gGameHeap, sizeof(Obj));
    u32 *p = (u32 *)obj;
    ObjSceneNode *tail;
    s32 i;

    for (i = 0; i < 0x2C; i++)
        p[i] = 0;
    D_800A6BE0.prev->next = obj;
    obj->next = &D_800A6BE0;
    obj->prev = D_800A6BE0.prev;
    D_800A6BE0.prev = obj;
    obj->flags = OBJF_DIRTY;
    obj->type = (s16)type;

    if (type != OBJ_VIEW) {
        obj->scale[2] = 1.0f;
        obj->scale[1] = 1.0f;
        obj->scale[0] = 1.0f;
        obj->node = func_800116B8(2);
    } else {
        obj->node = func_800116B8(4);
    }
    tail = parent->u.group.tail;
    tail->prev->next = obj->node;
    obj->node->prev = parent->u.group.tail->prev;
    obj->node->next = parent->u.group.tail;
    parent->u.group.tail->prev = obj->node;

    if (type != OBJ_VIEW) {
        set_gfx(&obj->setup[1], 0xBC000002, 0x80000040);
        set_gfx(&obj->setup[2], 0x03860010, (u32)(D_800C1770 + 8));
        set_gfx(&obj->setup[3], 0x03880010, (u32)D_800C1770);
        set_gfx(&obj->setup[4], 0xFA000000, 0xFF);
        set_gfx(&obj->setup[5], G_ENDDL, 0);
    } else {
        set_gfx(&obj->setup[0], G_ENDDL, 0);
    }
    for (i = 0; i < 4; i++)
        set_gfx(&obj->finish[i], G_ENDDL, 0);
    return obj;
}

/* @ 0x8001933C */
/* Free owned data (OBJF_OWNS_DATA) according to the object type. */
void Obj_FreeData(Obj *obj)
{
    if (!(obj->flags & OBJF_OWNS_DATA))
        return;
    switch ((u32)obj->type) {
    case OBJ_TEXT:
        TextBuf_Delete(obj->data);
        obj->data = NULL;
        break;
    case OBJ_SPRITE:
    case OBJ_SCREEN:
        Sprite_Delete(obj->data);
        obj->data = NULL;
        break;
    case OBJ_MODEL:
        Model_Delete(obj->data);
        obj->data = NULL;
        break;
    }
}

/* @ 0x800193C4 */
/* Free the object's scene node. */
void Obj_FreeNode(Obj *obj)
{
    func_800117BC(obj->node);
    obj->node = NULL;
}

/* @ 0x800193F0 */
/* Node matrix = translate(pos) * scale * rotate [* flips] * translate(-pivot). */
void Obj_Transform3D(Obj *obj)
{
    MtxF a, b;

    Mtx_Translate(a, obj->pos[0], obj->pos[1], obj->pos[2]);
    func_80008750(b, a, obj->scale[0], obj->scale[1], obj->scale[2]);
    func_80008540(a, b, obj->rot[1], obj->rot[0], obj->rot[2]);
    if (obj->flags & (OBJF_FLIP_X | OBJF_FLIP_Y | OBJF_FLIP_Z)) {
        Mtx_Identity(b);
        if (obj->flags & OBJF_FLIP_X)
            b[0][0] = -1.0f;
        if (obj->flags & OBJF_FLIP_Y)
            b[1][1] = -1.0f;
        if (obj->flags & OBJF_FLIP_Z)
            b[2][2] = -1.0f;
        Mtx_Mul(a, b, a);
    }
    func_800085B4(obj->node->mtx, a, -obj->pivot[0], -obj->pivot[1], -obj->pivot[2]);
}

/* @ 0x80019520 */
/* Same for 2D sprites, in screen coordinates (origin top-left, y down, 320x240). */
void Obj_Transform2D(Obj *obj)
{
    MtxF a, b;

    Mtx_Translate(a, obj->pos[0] - 160.0f, 120.0f - obj->pos[1], obj->pos[2]);
    func_80008750(b, a, obj->scale[0], obj->scale[1], obj->scale[2]);
    func_80008540(a, b, obj->rot[1], obj->rot[0], obj->rot[2]);
    func_800085B4(obj->node->mtx, a, -obj->pivot[0], -obj->pivot[1], -obj->pivot[2]);
}

/* @ 0x800195F4 */
/* Viewport: pos[0..2], rot[0..2] truncated into the node's six s16 fields. */
void Obj_TransformView(Obj *obj)
{
    obj->node->u.view.v[0] = (s16)(s32)obj->pos[0];
    obj->node->u.view.v[1] = (s16)(s32)obj->pos[1];
    obj->node->u.view.v[2] = (s16)(s32)obj->pos[2];
    obj->node->u.view.v[3] = (s16)(s32)obj->rot[0];
    obj->node->u.view.v[4] = (s16)(s32)obj->rot[1];
    obj->node->u.view.v[5] = (s16)(s32)obj->rot[2];
}

/* @ 0x80019688 */
/* Screen sprites rebuild their rectangles from pos and scale. */
void Obj_TransformScreen(Obj *obj)
{
    Sprite_DrawScreen(obj->data, obj->pos[0], obj->pos[1], obj->scale[0], obj->scale[1]);
}

/* @ 0x800196BC */
/* Point the scene node at the object's display lists. */
void Obj_Attach(Obj *obj)
{
    ObjSceneNode *node;

    if (obj->type != OBJ_VIEW) {
        obj->node->u.draw.setup_dl = obj->setup;
        obj->node->u.draw.finish_dl = obj->finish;
    }
    switch ((u32)obj->type) {
    case OBJ_TEXT: {
        TextBuf *tb = obj->data;
        obj->node->u.draw.dl = tb->dl[tb->cur];
        break;
    }
    case OBJ_SPRITE:
    case OBJ_SCREEN:
        obj->node->u.draw.dl = ((Sprite *)obj->data)->dl;
        break;
    case OBJ_MODEL:
        if (obj->flags & OBJF_MODEL_ALT2) {
            set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038EE0);
        } else if (obj->flags & OBJF_MODEL_ALT) {
            if (obj->flags & (OBJF_FLIP_X | OBJF_FLIP_Y | OBJF_FLIP_Z))
                set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038EA0);
            else
                set_gfx(&obj->setup[0], G_DL_CALL, (u32)D_80038E60);
        }
        node = obj->node;
        node->u.draw.dl = ((Model *)obj->data)->dl;
        break;
    case OBJ_DL:
        obj->node->u.draw.dl = obj->data;
        break;
    }
}

/* @ 0x800197C8 */
/* Unhook the display lists from the scene node (hide). */
void Obj_Detach(Obj *obj)
{
    if (obj->type != OBJ_VIEW) {
        obj->node->u.draw.setup_dl = NULL;
        obj->node->u.draw.dl = NULL;
        obj->node->u.draw.finish_dl = NULL;
    }
}

/* ---------------------------------------------------------------- scene tables */

/* @ 0x80019800 */
/* Load every asset in `assets`, then create every object in `objs` and apply its
 * colour, position, scale and visibility. Kind 13 (viewport) maps the fields to
 * pos/rot instead. Unknown kinds (9, 0, >13) create nothing but still apply the
 * settings to whatever object is already in the slot. */
void Scene_Build(SceneObjDesc *objs, SceneAssetDesc *assets)
{
    SceneAssetDesc *a;
    SceneObjDesc *d;

    for (a = assets; a->slot != NULL; a++) {
        void *p;
        switch (a->type) {
        case 0: p = Model_New(NULL, ASSET_ROM(a->rom)); break;
        case 2: p = Sprite_NewPlain(NULL, ASSET_ROM(a->rom)); break;
        case 4: p = Sprite_New(NULL, ASSET_ROM(a->rom)); break;
        case 9: p = Sprite_NewScreen(NULL, ASSET_ROM(a->rom)); break;
        default: continue;
        }
        *a->slot = p;
    }

    for (d = objs; d->slot != NULL; d++) {
        Obj *obj;
        s32 made = 1;

        switch (d->kind) {
        case 1:  obj = Obj_NewModelRef(*d->parent, *(Model **)d->arg); break;
        case 2:  obj = Obj_NewSpritePlain(*d->parent, ASSET_ROM(d->arg)); break;
        case 3:  obj = Obj_NewSpritePlain2(*d->parent, ASSET_ROM(d->arg)); break;
        case 4:  obj = Obj_NewSprite(*d->parent, ASSET_ROM(d->arg)); break;
        case 5:  obj = Obj_NewSprite2(*d->parent, ASSET_ROM(d->arg)); break;
        case 6:  obj = Obj_NewSpriteRef(*d->parent, *(Sprite **)d->arg); break;
        case 7:  obj = Obj_NewSpritePlainRef(*d->parent, *(Sprite **)d->arg); break;
        case 8:  obj = Obj_NewText(*d->parent, *(Font **)d->arg, D_8003D8D0); break;
        case 10: obj = Obj_NewScreenRef(*d->parent, *(Sprite **)d->arg); break;
        case 11: obj = Obj_NewDefaultDL(*d->parent); break;
        case 12: {
            u32 *w = d->arg;
            obj = Obj_NewDL(*d->parent, (SpriteGfx *)w[0], (void *)w[1], (SpriteGfx *)w[2]);
            break;
        }
        case 13: obj = Obj_NewView(*d->parent); break;
        default: made = 0; obj = NULL; break;
        }
        if (made)
            *d->slot = obj;

        if (d->kind != 13) {
            Obj_SetColor(*d->slot, 0, 0, 0, d->alpha);
            Obj_SetPos(*d->slot, d->x, d->y, d->z);
            if (!(d->sx == 1.0f && d->sy == 1.0f && d->sz == 1.0f)) {
                (*d->slot)->scale[0] = d->sx;
                (*d->slot)->scale[1] = d->sy;
                (*d->slot)->scale[2] = d->sz;
            }
            if (d->visible)
                Obj_Show(*d->slot);
        } else {
            (*d->slot)->pos[0] = (f32)d->x;
            (*d->slot)->pos[1] = (f32)d->y;
            (*d->slot)->pos[2] = (f32)(d->z - 1);
            (*d->slot)->rot[0] = d->sx - 1.0f;
            (*d->slot)->rot[1] = (f32)(s16)(s32)(d->sy * 8192.0f);
            (*d->slot)->rot[2] = (f32)(s16)(s32)(d->sz * 8192.0f);
        }
    }
}

/* @ 0x80019C9C */
/* Kill every object of the table and free every asset (models with Model_Delete,
 * all other types as sprites), clearing the slots. */
void Scene_Destroy(SceneObjDesc *objs, SceneAssetDesc *assets)
{
    SceneObjDesc *d;
    SceneAssetDesc *a;

    for (d = objs; d->slot != NULL; d++)
        Obj_Kill(*d->slot);
    for (a = assets; a->rom != 0; a++) {
        if (a->type == 0)
            Model_Delete(*a->slot);
        else
            Sprite_Delete(*a->slot);
        *a->slot = NULL;
    }
}
