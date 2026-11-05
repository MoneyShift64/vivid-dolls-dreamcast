/* Vivid Dolls port - in-process display-list replay for differential checks
 * (port/gfx/tools/replay_check.py).
 *
 * Built as a shared library around one version of f3d.c (old or new, see
 * the Makefile target "replay"). The harness script copies the emulator's RAM
 * into this library (never to a file: it holds decoded photo pixels) and runs
 * the display list of the last frame through the interpreter with a hashing
 * backend. The digest covers everything a backend receives: frame calls, colour
 * image, fills, depth clears, every triangle (all F3DState fields except the
 * handle/serial, all vertex fields bit for bit) and the decoded content of the
 * texture each triangle uses (texture keys/handles are internal and ignored).
 * Two builds fed the same RAM must give the same digests.
 *
 * The backend also implements the image-window texture interface
 * (F3DBackend.img_*), materialising the window from its own RAM mirror the way
 * the PVR samples it, so the photo path is checked against the TMEM path.
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "f3d.h"          /* -I selects the version (ref or new) */

#define EXPORT __attribute__((visibility("default")))

static uint8_t *g_ram;
static uint32_t g_ram_size;
static uint8_t g_zero[1 << 20];

static void *rp_addr(uint32_t a)
{
    uint32_t off = a & 0x00FFFFFFu;
    if (off + 4096 <= g_ram_size)
        return g_ram + off;
    return g_zero;
}

/* ---- digest ---- */
static uint64_t H;
static void hmix(const void *p, size_t n)
{
    const uint8_t *b = p;
    while (n--) {
        H ^= *b++;
        H *= 0x100000001B3ull;
    }
}
static void hu32(uint32_t v) { hmix(&v, 4); }

/* ---- textures ---- */
#define MAXTEX 4096
typedef struct { uint64_t key; uint64_t digest; int w, h, alpha; } RTex;
static RTex g_tex[MAXTEX];
static int g_ntex;
static uint32_t g_stats[16];   /* 0 uploads 1 finds_hit 2 tris 3 draws 4 frames 5 img_updates 6 img_texels */

static uint64_t digest_argb(const uint32_t *p, int w, int h)
{
    uint64_t s = H;
    uint64_t d;
    H = 0xCBF29CE484222325ull;
    hmix(p, (size_t)w * h * 4);
    d = H;
    H = s;
    return d;
}

static void *rp_find(void *ctx, uint64_t key)
{
    int i;
    (void)ctx;
    for (i = g_ntex - 1; i >= 0; i--)
        if (g_tex[i].key == key) { g_stats[1]++; return &g_tex[i]; }
    return NULL;
}

#ifdef F3D_CAP_RGBA16
static uint32_t g_conv[1024 * 1024];
#endif

static void *rp_upload(void *ctx, const F3DTexImage *img)
{
    RTex *t;
    const uint32_t *argb = img->argb;
    (void)ctx;
#ifdef F3D_CAP_RGBA16
    if (img->rgba16) {
        int i, n = img->width * img->height;
#ifdef F3D_TEXIMAGE_STRIDE
        int pitch = img->stride ? (int)img->stride : img->width;
#else
        int pitch = img->width;
#endif
        for (i = 0; i < n; i++) {
            uint16_t c = img->rgba16[(i / img->width) * pitch + i % img->width];
            uint32_t r = (c >> 11) & 31, g = (c >> 6) & 31, b = (c >> 1) & 31;
            r = (r << 3) | (r >> 2); g = (g << 3) | (g >> 2); b = (b << 3) | (b >> 2);
            g_conv[i] = ((c & 1) ? 0xFF000000u : 0) | (r << 16) | (g << 8) | b;
        }
        argb = g_conv;
        g_stats[6] += (uint32_t)n;
    }
#endif
    g_stats[0]++;
    if (g_ntex >= MAXTEX) {             /* simple reset; keys stay correct */
        g_ntex = 0;
    }
    t = &g_tex[g_ntex++];
    t->key = img->key;
    t->w = img->width; t->h = img->height; t->alpha = img->alpha;
    t->digest = digest_argb(argb, img->width, img->height);
    return t;
}

static void rp_begin(void *ctx) { (void)ctx; hu32(0xB0B0B0B0u); }
static void rp_end(void *ctx) { (void)ctx; hu32(0xE0E0E0E0u); }
static void rp_cimg(void *ctx, uint32_t addr, int fmt, int siz, int width)
{
    (void)ctx; (void)addr;
    hu32(0xC1C1C1C1u); hu32((uint32_t)fmt); hu32((uint32_t)siz); hu32((uint32_t)width);
}
static void rp_clear_depth(void *ctx, float x0, float y0, float x1, float y1)
{
    (void)ctx;
    hu32(0xD0D0D0D0u); hmix(&x0, 4); hmix(&y0, 4); hmix(&x1, 4); hmix(&y1, 4);
}
static void rp_fill(void *ctx, float x0, float y0, float x1, float y1, uint32_t argb)
{
    (void)ctx;
    hu32(0xF1F1F1F1u); hmix(&x0, 4); hmix(&y0, 4); hmix(&x1, 4); hmix(&y1, 4); hu32(argb);
}

static int g_verbose;
#include <stdio.h>

static void rp_draw(void *ctx, const F3DState *st, const F3DVert *v, int count)
{
    const RTex *t = st->tex;
    uint8_t b[16];
    (void)ctx;
    hu32(0x7A7A7A7Au);
    b[0] = st->filter; b[1] = st->wrap_s; b[2] = st->wrap_t; b[3] = st->blend; b[4] = st->alpha_ref;
    b[5] = st->z_cmp; b[6] = st->z_upd; b[7] = st->z_decal; b[8] = st->tex_rgb; b[9] = st->tex_alpha;
    b[10] = st->offset; b[11] = st->cycle; b[12] = st->rect; b[13] = 0; b[14] = 0; b[15] = 0;
    hmix(b, 16);
    hu32(st->tex_w); hu32(st->tex_h);
    hu32(st->othermode_h); hu32(st->othermode_l); hu32(st->combine_h); hu32(st->combine_l);
    if (t) { hmix(&t->digest, 8); hu32((uint32_t)t->w); hu32((uint32_t)t->h); hu32((uint32_t)t->alpha); }
    else hu32(0);
    {
        int i;
        for (i = 0; i < count; i++) {
            hmix(&v[i].x, 4); hmix(&v[i].y, 4); hmix(&v[i].z, 4); hmix(&v[i].w, 4);
            hmix(&v[i].u, 4); hmix(&v[i].v, 4);
            hmix(&v[i].r, 1); hmix(&v[i].g, 1); hmix(&v[i].b, 1); hmix(&v[i].a, 1);
            hmix(&v[i].or_, 1); hmix(&v[i].og, 1); hmix(&v[i].ob, 1); hmix(&v[i].oa, 1);
        }
    }
    g_stats[2] += (uint32_t)count / 3;
    g_stats[3]++;
    if (g_verbose)
        fprintf(stderr, "draw n=%d tex=%016llx w=%d h=%d a=%d blend=%d filt=%d H=%016llx\n", count,
                t ? (unsigned long long)t->digest : 0ull, t ? t->w : 0, t ? t->h : 0, t ? t->alpha : 0,
                st->blend, st->filter, (unsigned long long)H);
}

static F3DBackend g_be;
static int g_inited;

EXPORT void rp_set_verbose(int v) { g_verbose = v; }

EXPORT int rp_frame(const uint8_t *ram, uint32_t ram_size, uint32_t dl, uint64_t *digest, uint32_t *stats)
{
    if (!g_ram || g_ram_size != ram_size) {
        free(g_ram);
        g_ram = malloc(ram_size);
        g_ram_size = ram_size;
    }
    memcpy(g_ram, ram, ram_size);
    if (!g_inited) {
        memset(&g_be, 0, sizeof(g_be));
        g_be.begin_frame = rp_begin;
        g_be.end_frame = rp_end;
        g_be.color_image = rp_cimg;
        g_be.clear_depth = rp_clear_depth;
        g_be.fill_rect = rp_fill;
        g_be.tex_find = rp_find;
        g_be.tex_upload = rp_upload;
        g_be.draw_tris = rp_draw;
#ifdef F3D_CAP_RGBA16
        g_be.caps = F3D_CAP_RGBA16;
#endif
        f3d_init(&g_be, rp_addr);
        f3d_set_ucode(0);
        g_inited = 1;
    }
    H = 0xCBF29CE484222325ull;
    memset(g_stats, 0, sizeof(g_stats));
    f3d_clear_stats();
    f3d_run_task(dl);
    *digest = H;
    g_stats[4] = f3d_stats()->cmds;
#ifdef RP_STATS
    {
        const F3DStats *fs = f3d_stats();
        g_stats[7] = fs->loadblocks; g_stats[8] = fs->loadtiles; g_stats[9] = fs->loadtluts;
        g_stats[10] = fs->load_bytes; g_stats[11] = fs->hash_bytes; g_stats[12] = fs->tex_lookups;
        g_stats[13] = fs->tris_in; g_stats[14] = fs->rects; g_stats[15] = fs->vtx;
    }
#endif
    if (stats) memcpy(stats, g_stats, sizeof(g_stats));
    return 0;
}
