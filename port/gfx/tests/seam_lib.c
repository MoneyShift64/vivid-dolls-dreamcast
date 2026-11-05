/* Vivid Dolls port - software model of the PVR output vs the RDP for texture
 * rectangles (port/gfx/tools/seam_check.py). Host only.
 *
 * The harness script copies the emulator RAM into this library (in memory only:
 * it holds decoded photo pixels) and runs the frame's display list through
 * f3d.c. For every textured TEXRECT the backend would draw:
 *  - the texture is laid out exactly as pvr_backend.c puts it in VRAM
 *    (pvr_texfill.h), on top of "stale" VRAM (texels never written = black);
 *  - "PVR" models: the quad is rasterised at 640x480 with bilinear sampling at
 *    pixel centres (texel centres at +0.5), the neighbour texels addressed
 *      hw: modulo the power-of-two size (the PVR wraps the filter neighbour
 *          even with UV clamp), or
 *      gl: clamped per texel to the power-of-two size (what GL-based
 *          emulators such as the harness's Flycast do);
 *    each with the current layout ("new") and without the wrap-side padding
 *    ("old", the layout before the fix);
 *  - "RDP" reference: every N64 pixel the RDP writes (1-cycle TEXRECT rules,
 *    see texrect() in f3d.c) with the RDP's 3-point bilinear filter at the RDP's
 *    s,t, then doubled to 640x480.
 * Buffers are 640x480 RGB float sums per pixel (R+G+B), -1 = not drawn.
 */
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "f3d.h"
#include "pvr_texfill.h"

#define EXPORT __attribute__((visibility("default")))
#define W 640
#define H 480

enum { M_HW_NEW, M_HW_OLD, M_GL_NEW, M_RDP, M_COUNT };

static uint8_t *g_ram;
static uint32_t g_ram_size;
static uint8_t g_zero[1 << 20];
static float g_out[M_COUNT][W * H];
static int g_min_h = 1, g_max_h = 4096;    /* texture height filter for the rects modelled */

static void *rp_addr(uint32_t a)
{
    uint32_t off = a & 0x00FFFFFFu;
    if (off + 4096 <= g_ram_size)
        return g_ram + off;
    return g_zero;
}

typedef struct {
    uint64_t key;
    int w, h;                 /* image size */
    PvrTexLayout L;
    uint16_t *vram_new, *vram_old;    /* w2*h2 PVR texels */
} MTex;

#define MAXTEX 2048
static MTex g_tex[MAXTEX];
static int g_ntex;
static uint16_t g_row[1024];

static void *m_find(void *ctx, uint64_t key)
{
    int i;
    (void)ctx;
    for (i = g_ntex - 1; i >= 0; i--)
        if (g_tex[i].key == key) return &g_tex[i];
    return NULL;
}

static void *m_upload(void *ctx, const F3DTexImage *img)
{
    MTex *t;
    PvrTexLayout Lo;
    size_t n;
    (void)ctx;
    if (g_ntex >= MAXTEX) {
        int i;
        for (i = 0; i < g_ntex; i++) { free(g_tex[i].vram_new); free(g_tex[i].vram_old); }
        g_ntex = 0;
    }
    t = &g_tex[g_ntex++];
    t->key = img->key; t->w = img->width; t->h = img->height;
    pvrtf_layout(img, &t->L);
    n = (size_t)t->L.w2 * t->L.h2;
    t->vram_new = calloc(n, 2);        /* stale VRAM = 0 (black) */
    t->vram_old = calloc(n, 2);
    pvrtf_fill(img, &t->L, t->vram_new, g_row);
    /* the old layout: no wrap-side copies; re-clear what the fill wrote there */
    Lo = t->L; Lo.wrap_row = 0; Lo.wrap_col = 0;
    pvrtf_fill(img, &Lo, t->vram_old, g_row);
    {
        int y;
        if (t->L.wrap_col)
            for (y = 0; y < t->L.fh; y++) {
                t->vram_old[(size_t)y * t->L.w2 + t->L.w2 - 1] = 0;
                t->vram_old[(size_t)y * t->L.w2 + t->L.w2 - 2] = 0;
            }
    }
    return t;
}

static void texel_rgb(const MTex *t, uint16_t c, float *rgb, float *a)
{
    int r, g, b;
    switch (t->L.fmt) {
    case PVRTF_565:
        r = (c >> 11) & 31; g = (c >> 5) & 63; b = c & 31;
        rgb[0] = (float)((r << 3) | (r >> 2)); rgb[1] = (float)((g << 2) | (g >> 4)); rgb[2] = (float)((b << 3) | (b >> 2));
        *a = 1; break;
    case PVRTF_1555:
        r = (c >> 10) & 31; g = (c >> 5) & 31; b = c & 31;
        rgb[0] = (float)((r << 3) | (r >> 2)); rgb[1] = (float)((g << 3) | (g >> 2)); rgb[2] = (float)((b << 3) | (b >> 2));
        *a = (c & 0x8000) ? 1.0f : 0.0f; break;
    default:
        rgb[0] = (float)(((c >> 8) & 15) * 17); rgb[1] = (float)(((c >> 4) & 15) * 17); rgb[2] = (float)((c & 15) * 17);
        *a = (float)((c >> 12) & 15) / 15.0f; break;
    }
}

/* PVR bilinear at texel position (u, v) (texel units, centres at +0.5) */
static void pvr_sample(const MTex *t, const uint16_t *vram, int hw, float u, float v, float *rgb, float *a)
{
    const int w2 = t->L.w2, h2 = t->L.h2;
    float fu = u - 0.5f, fv = v - 0.5f;
    int x0 = (int)floorf(fu), y0 = (int)floorf(fv), i;
    float ax = fu - (float)x0, ay = fv - (float)y0;
    float acc[3] = { 0, 0, 0 }, aa = 0;
    for (i = 0; i < 4; i++) {
        int x = x0 + (i & 1), y = y0 + (i >> 1);
        float wgt = ((i & 1) ? ax : 1 - ax) * ((i >> 1) ? ay : 1 - ay), c[3], ca;
        if (hw) { x &= w2 - 1; y &= h2 - 1; }
        else { x = x < 0 ? 0 : (x >= w2 ? w2 - 1 : x); y = y < 0 ? 0 : (y >= h2 ? h2 - 1 : y); }
        texel_rgb(t, vram[(size_t)y * w2 + x], c, &ca);
        acc[0] += wgt * c[0]; acc[1] += wgt * c[1]; acc[2] += wgt * c[2]; aa += wgt * ca;
    }
    rgb[0] = acc[0]; rgb[1] = acc[1]; rgb[2] = acc[2]; *a = aa;
}

/* RDP 3-point filter at (s, t) (texel units, centres on integers), image texels
   only (the window holds every texel the RDP fetches) */
static void rdp_sample(const MTex *tx, float s, float t, float *rgb, float *a)
{
    int s0 = (int)floorf(s), t0 = (int)floorf(t), i;
    float fs = s - (float)s0, ft = t - (float)t0, c[4][3], ca[4];
    for (i = 0; i < 4; i++) {
        int x = s0 + (i & 1), y = t0 + (i >> 1);
        x = x < 0 ? 0 : (x >= tx->w ? tx->w - 1 : x);
        y = y < 0 ? 0 : (y >= tx->h ? tx->h - 1 : y);
        texel_rgb(tx, tx->vram_new[(size_t)y * tx->L.w2 + x], c[i], &ca[i]);
    }
    for (i = 0; i < 3; i++) {
        if (fs + ft < 1.0f) rgb[i] = c[0][i] + fs * (c[1][i] - c[0][i]) + ft * (c[2][i] - c[0][i]);
        else rgb[i] = c[3][i] + (1 - fs) * (c[2][i] - c[3][i]) + (1 - ft) * (c[1][i] - c[3][i]);
    }
    *a = fs + ft < 1.0f ? ca[0] + fs * (ca[1] - ca[0]) + ft * (ca[2] - ca[0])
                        : ca[3] + (1 - fs) * (ca[2] - ca[3]) + (1 - ft) * (ca[1] - ca[3]);
}

static void m_draw(void *ctx, const F3DState *st, const F3DVert *v, int count)
{
    const MTex *t = st->tex;
    float minx = 1e9f, miny = 1e9f, maxx = -1e9f, maxy = -1e9f, u0, v0, dudx, dvdy;
    int i, k, px, py, blend_cut;
    (void)ctx;
    if (!st->rect || !t || !st->tex_rgb || count < 3 || st->filter != F3D_FILTER_BILINEAR)
        return;
    if (t->h < g_min_h || t->h > g_max_h)
        return;
    /* the quad's affine mapping (rects: axis-aligned, w = 1) */
    for (i = 0; i < count; i++) {
        if (v[i].x < minx) minx = v[i].x;
        if (v[i].x > maxx) maxx = v[i].x;
        if (v[i].y < miny) miny = v[i].y;
        if (v[i].y > maxy) maxy = v[i].y;
    }
    if (maxx <= minx || maxy <= miny) return;
    {
        /* solve u = u0 + dudx*(x - minx) from two vertices with different x */
        const F3DVert *a = &v[0], *b = NULL, *c = NULL;
        for (i = 1; i < count; i++) if (v[i].x != a->x && v[i].y == a->y) { b = &v[i]; break; }
        for (i = 1; i < count; i++) if (v[i].y != a->y && v[i].x == a->x) { c = &v[i]; break; }
        if (!b || !c) return;
        dudx = (b->u - a->u) * st->tex_w / (b->x - a->x);
        dvdy = (c->v - a->v) * st->tex_h / (c->y - a->y);
        u0 = a->u * st->tex_w + (minx - a->x) * dudx;
        v0 = a->v * st->tex_h + (miny - a->y) * dvdy;
    }
    blend_cut = st->blend != F3D_BLEND_OPAQUE || st->alpha_ref;
    /* only the first triangle pair of a rect is needed: the quad is the bbox */
    if (count > 6) return;
    for (py = (int)ceilf(miny * 2 - 0.5f); py < H && (py + 0.5f) < maxy * 2; py++) {
        if (py < 0) continue;
        for (px = (int)ceilf(minx * 2 - 0.5f); px < W && (px + 0.5f) < maxx * 2; px++) {
            float x = (px + 0.5f) * 0.5f, y = (py + 0.5f) * 0.5f, rgb[3], a;
            float u = u0 + (x - minx) * dudx, vv = v0 + (y - miny) * dvdy;
            if (px < 0) continue;
            for (k = 0; k < 3; k++) {
                const uint16_t *vr = k == M_HW_OLD ? t->vram_old : t->vram_new;
                pvr_sample(t, vr, k != M_GL_NEW, u, vv, rgb, &a);
                if (blend_cut && a < 0.5f) continue;
                g_out[k][py * W + px] = rgb[0] * v[0].r / 255.0f + rgb[1] * v[0].g / 255.0f + rgb[2] * v[0].b / 255.0f;
            }
        }
    }
    /* RDP: N64 pixels (X, Y) with X in [minx, maxx), Y in [miny, maxy) (the
       interpreter emits the covered pixel rectangle); s,t at the pixel = the
       PVR coordinate at the pixel centre - 0.5 (see texrect() in f3d.c) */
    for (py = (int)ceilf(miny); py < (int)ceilf(maxy) && py < 240; py++)
        for (px = (int)ceilf(minx); px < (int)ceilf(maxx) && px < 320; px++) {
            float rgb[3], a, sum;
            float s = u0 + (px + 0.5f - minx) * dudx - 0.5f, tt = v0 + (py + 0.5f - miny) * dvdy - 0.5f;
            if (px < 0 || py < 0) continue;
            rdp_sample(t, s, tt, rgb, &a);
            if (blend_cut && a < 0.5f) continue;
            sum = rgb[0] * v[0].r / 255.0f + rgb[1] * v[0].g / 255.0f + rgb[2] * v[0].b / 255.0f;
            g_out[M_RDP][(2 * py) * W + 2 * px] = sum;
            g_out[M_RDP][(2 * py) * W + 2 * px + 1] = sum;
            g_out[M_RDP][(2 * py + 1) * W + 2 * px] = sum;
            g_out[M_RDP][(2 * py + 1) * W + 2 * px + 1] = sum;
        }
}

static void m_nop(void *ctx) { (void)ctx; }
static void m_cimg(void *ctx, uint32_t a, int f, int s, int w) { (void)ctx; (void)a; (void)f; (void)s; (void)w; }
static void m_rect(void *ctx, float a, float b, float c, float d) { (void)ctx; (void)a; (void)b; (void)c; (void)d; }
static void m_fill(void *ctx, float a, float b, float c, float d, uint32_t e) { (void)ctx; (void)a; (void)b; (void)c; (void)d; (void)e; }

static F3DBackend g_be;
static int g_inited;

EXPORT void sm_set_height_range(int lo, int hi) { g_min_h = lo; g_max_h = hi; }

/* runs the frame; returns a pointer to M_COUNT buffers of W*H floats */
EXPORT const float *sm_frame(const uint8_t *ram, uint32_t ram_size, uint32_t dl)
{
    int m, i;
    if (!g_ram || g_ram_size != ram_size) {
        free(g_ram);
        g_ram = malloc(ram_size);
        g_ram_size = ram_size;
    }
    memcpy(g_ram, ram, ram_size);
    if (!g_inited) {
        memset(&g_be, 0, sizeof(g_be));
        g_be.begin_frame = m_nop;
        g_be.end_frame = m_nop;
        g_be.color_image = m_cimg;
        g_be.clear_depth = m_rect;
        g_be.fill_rect = m_fill;
        g_be.tex_find = m_find;
        g_be.tex_upload = m_upload;
        g_be.draw_tris = m_draw;
        g_be.caps = F3D_CAP_RGBA16;
        f3d_init(&g_be, rp_addr);
        f3d_set_ucode(0);
        g_inited = 1;
    }
    for (m = 0; m < M_COUNT; m++)
        for (i = 0; i < W * H; i++) g_out[m][i] = -1.0f;
    f3d_run_task(dl);
    return &g_out[0][0];
}
