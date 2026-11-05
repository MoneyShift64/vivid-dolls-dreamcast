/* Vivid Dolls port - F3D backend for the Dreamcast PVR (KallistiOS).
 *
 * N64 rendering is strictly in submission order (the RDP is an immediate-mode
 * rasteriser), so everything goes into the PVR translucent list with
 * autosort disabled ("presort"): polygons are drawn in the order sent, opaque
 * ones simply use ONE/ZERO blending.  Depth test/write follow the N64 render
 * mode (depth = 1/w, compare GEQUAL).
 *
 * Textures: ARGB8888 (or raw RGBA16) from the interpreter -> RGB565 /
 * ARGB1555 / ARGB4444, padded to a power of two (layout in pvr_texfill.h),
 * non-twiddled in VRAM, cached by the interpreter's content key.
 * Screen: the N64 colour image (320 wide) is scaled to 640x480.
 */
#include "pvr_backend.h"

#include <dc/pvr.h>
#include <arch/timer.h>
#include <arch/cache.h>
#include <dc/pvr/pvr_dma.h>
#include <string.h>
#include "pvr_texfill.h"

#define TEX_SLOTS   512
#define TEX_KEEP    2           /* frames a texture survives without use (PVR may still render it) */

typedef struct TexEntry {
    uint64_t key;
    pvr_ptr_t ptr;
    uint16_t w2, h2;            /* PVR (power-of-two) size */
    uint16_t w, h;              /* N64 size */
    uint32_t fmt;               /* PVR_TXRFMT_* */
    int last_frame;
    int used;
} TexEntry;

static struct {
    TexEntry tex[TEX_SLOTS];
    int frame;
    float sx, sy;               /* N64 pixels -> 640x480 */
    uint32_t hdr_serial;
    int hdr_valid;
    int skip;                   /* current state draws nothing */
    int force_alpha255;
    int cur_blend;              /* current header blends */
    int in_frame;
    PvrBackendStats stats;
} B;

static uint16_t conv_buf[1024 * 1024 / 4] __attribute__((aligned(32)));   /* staging, up to 512x512 16-bit */
static uint16_t row_buf[1024] __attribute__((aligned(32)));                /* one padded source row */

const PvrBackendStats *pvr_backend_stats(void) { return &B.stats; }

/* Per-frame PVR load, accumulated for the harness (tools/port/dc_harness/perf_cases.py):
   [0] frames, [1] triangles, [2] polygon headers, [3] opaque pixels, [4] blended
   pixels (640x480 screen pixels drawn with alpha blending: the PVR's translucent
   fill cost), [5] max blended pixels in one frame, [6] max triangles in one frame,
   [7] texture bytes uploaded, [8] max blended polygons in one frame */
volatile uint32_t vd_pvr_acc[9];
static uint32_t fr_tris, fr_hdrs, fr_opq, fr_blend, fr_blend_polys;

/* ------------------------------------------------------------------ textures */
/* stable pool (the interpreter keeps entry pointers for the current frame)
 * + chained hash buckets */
#define BUCKETS 1024
static int16_t bucket[BUCKETS];
static int16_t chain[TEX_SLOTS];
static int buckets_ready;
/* free slots (stack): no scan of the 512-entry pool per upload */
static int16_t free_stack[TEX_SLOTS];
static int free_top;

static void buckets_init(void)
{
    int i;
    for (i = 0; i < BUCKETS; i++) bucket[i] = -1;
    for (i = 0; i < TEX_SLOTS; i++) chain[i] = -1;
    free_top = 0;
    for (i = TEX_SLOTS - 1; i >= 0; i--) free_stack[free_top++] = (int16_t)i;
    buckets_ready = 1;
}

static int bucket_of(uint64_t key) { return (int)((key ^ (key >> 29)) & (BUCKETS - 1)); }

static void tex_free(int i)
{
    TexEntry *e = &B.tex[i];
    int16_t *pp;
    if (!e->used) return;
    for (pp = &bucket[bucket_of(e->key)]; *pp >= 0; pp = &chain[*pp])
        if (*pp == i) { *pp = chain[i]; break; }
    chain[i] = -1;
    if (e->ptr) pvr_mem_free(e->ptr);
    memset(e, 0, sizeof(*e));
    free_stack[free_top++] = (int16_t)i;
}

void pvr_backend_flush_textures(void)
{
    int i;
    for (i = 0; i < TEX_SLOTS; i++)
        tex_free(i);
}

/* free textures not used during the last `keep` frames (never the current one) */
static int evict_old(int keep)
{
    int i, n = 0;
    for (i = 0; i < TEX_SLOTS; i++)
        if (B.tex[i].used && B.frame - B.tex[i].last_frame > keep) {
            tex_free(i);
            B.stats.tex_evictions++;
            n++;
        }
    return n;
}

static void *pb_tex_find(void *ctx, uint64_t key)
{
    int i;
    (void)ctx;
    for (i = bucket[bucket_of(key)]; i >= 0; i = chain[i])
        if (B.tex[i].key == key) {
            B.tex[i].last_frame = B.frame;
            B.stats.tex_hits++;
            return &B.tex[i];
        }
    return NULL;
}

static int free_slot(void)
{
    return free_top > 0 ? free_stack[--free_top] : -1;     /* popped; pushed back by tex_free */
}

static void *pb_tex_upload(void *ctx, const F3DTexImage *img)
{
    TexEntry *e;
    int x, y, slot, w2, h2, fw, fh;
    PvrTexLayout L;
    uint32_t fmt;
    (void)ctx;

    slot = free_slot();
    if (slot < 0 && evict_old(1) > 0)
        slot = free_slot();
    if (slot < 0)
        return NULL;
    e = &B.tex[slot];
    /* layout and padding: see pvr_texfill.h (edge replication for clamp, tiling
       for small wrapped sizes, wrap-side copies of row 0 / column 0 for the
       bilinear neighbour before the first row / column) */
    pvrtf_layout(img, &L);
    w2 = L.w2; h2 = L.h2; fw = L.fw; fh = L.fh;
    fmt = L.fmt == PVRTF_565 ? PVR_TXRFMT_RGB565 : (L.fmt == PVRTF_1555 ? PVR_TXRFMT_ARGB1555 : PVR_TXRFMT_ARGB4444);
    e->ptr = pvr_mem_malloc((size_t)w2 * h2 * 2);
    if (!e->ptr && evict_old(1) > 0)
        e->ptr = pvr_mem_malloc((size_t)w2 * h2 * 2);
    if (!e->ptr) { B.stats.vram_fail++; free_stack[free_top++] = (int16_t)slot; return NULL; }
    /* non-twiddled (row-major) texture, converted into the staging buffer with the
       VRAM row pitch (w2), then copied to VRAM by the PVR DMA (large textures:
       the photo strips, re-uploaded when the photo scrolls or changes) or with
       32-bit stores (small ones). Uncached stores to VRAM were most of the
       upload time. (The KOS twiddler, pvr_txr_load_ex, divides per texel.) */
    pvrtf_fill(img, &L, conv_buf, row_buf);
    {
        size_t bytes = (size_t)fh * w2 * 2, alloc = (size_t)w2 * h2 * 2;
        if (bytes >= 2048) {
            /* whole rows (pitch w2) in one DMA (includes the wrap column); the
               unwritten columns are never sampled */
            bytes = (bytes + 31) & ~(size_t)31;
            if (bytes > alloc) bytes = alloc;
            dcache_wback_range((uintptr_t)conv_buf, bytes);
            if (pvr_txr_load_dma(conv_buf, e->ptr, bytes, true, NULL, NULL) != 0)
                goto stores;
            B.stats.tex_dma++;
        } else {
        stores:
            for (y = 0; y < fh; y++) {
                volatile uint32_t *dst = (volatile uint32_t *)((uint8_t *)e->ptr + (size_t)y * w2 * 2);
                const uint32_t *src = (const uint32_t *)(const void *)(conv_buf + (size_t)y * w2);
                for (x = 0; x < fw / 2; x++)
                    dst[x] = src[x];
                if (L.wrap_col)
                    dst[w2 / 2 - 1] = src[w2 / 2 - 1];
            }
        }
        if (L.wrap_row) {
            /* row h2-1 = row 0 (+ its wrap column) */
            volatile uint32_t *dst = (volatile uint32_t *)((uint8_t *)e->ptr + (size_t)(h2 - 1) * w2 * 2);
            const uint32_t *src = (const uint32_t *)(const void *)(conv_buf + (size_t)(h2 - 1) * w2);
            for (x = 0; x < fw / 2; x++)
                dst[x] = src[x];
            if (L.wrap_col)
                dst[w2 / 2 - 1] = src[w2 / 2 - 1];
        }
    }
    e->key = img->key;
    e->w2 = (uint16_t)w2; e->h2 = (uint16_t)h2;
    e->w = img->width; e->h = img->height;
    e->fmt = fmt | PVR_TXRFMT_NONTWIDDLED;
    e->last_frame = B.frame;
    e->used = 1;
    chain[slot] = bucket[bucket_of(e->key)];
    bucket[bucket_of(e->key)] = (int16_t)slot;
    B.stats.tex_uploads++;
    vd_pvr_acc[7] += (uint32_t)(fh * w2 * 2);
    return e;
}

/* ------------------------------------------------------------------ frame */
static void pb_begin(void *ctx)
{
    uint64_t t0 = timer_us_gettime64();
    (void)ctx;
    pvr_wait_ready();
    B.stats.wait_us_last = (unsigned)(timer_us_gettime64() - t0);
    pvr_scene_begin();
    pvr_list_begin(PVR_LIST_TR_POLY);
    B.hdr_valid = 0;
    B.in_frame = 1;
    B.frame++;
    B.stats.frames++;
    if ((B.frame & 15) == 0) evict_old(TEX_KEEP + 30);
}

static void pb_end(void *ctx)
{
    (void)ctx;
    if (!B.in_frame) return;
    pvr_list_finish();
    pvr_scene_finish();
    B.in_frame = 0;
    vd_pvr_acc[0]++;
    vd_pvr_acc[1] += fr_tris; vd_pvr_acc[2] += fr_hdrs;
    vd_pvr_acc[3] += fr_opq; vd_pvr_acc[4] += fr_blend;
    if (fr_blend > vd_pvr_acc[5]) vd_pvr_acc[5] = fr_blend;
    if (fr_tris > vd_pvr_acc[6]) vd_pvr_acc[6] = fr_tris;
    if (fr_blend_polys > vd_pvr_acc[8]) vd_pvr_acc[8] = fr_blend_polys;
    fr_tris = fr_hdrs = fr_opq = fr_blend = fr_blend_polys = 0;
}

static void pb_cimg(void *ctx, uint32_t addr, int fmt, int siz, int width)
{
    (void)ctx; (void)addr; (void)fmt; (void)siz;
    if (width <= 0) width = 320;
    B.sx = 640.0f / (float)width;
    B.sy = B.sx;                 /* 320x240 -> 640x480, 640x480 -> 1:1 */
}

static void pb_clear_depth(void *ctx, float x0, float y0, float x1, float y1)
{
    /* the PVR clears depth at the start of every scene; the game only clears
     * the Z buffer once per frame, before drawing */
    (void)ctx; (void)x0; (void)y0; (void)x1; (void)y1;
}

/* Direct rendering: headers and vertices are written straight into the store
   queue (pvr_list_begin has pointed it at the TA) and flushed, 32 bytes each. */
static inline void dr_send32(const void *p)
{
    uint32_t *d = (uint32_t *)pvr_dr_target();
    const uint32_t *s = (const uint32_t *)p;
    d[0] = s[0]; d[1] = s[1]; d[2] = s[2]; d[3] = s[3];
    d[4] = s[4]; d[5] = s[5]; d[6] = s[6]; d[7] = s[7];
    pvr_dr_commit(d);
}

static void send_hdr(const pvr_poly_hdr_t *h) { dr_send32(h); }

static void pb_fill(void *ctx, float x0, float y0, float x1, float y1, uint32_t argb)
{
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    pvr_vertex_t v;
    (void)ctx;
    if (!B.in_frame) return;
    if (x0 <= 0 && y0 <= 0 && x1 * B.sx >= 639 && y1 * B.sy >= 479)
        pvr_set_bg_color(((argb >> 16) & 0xFF) / 255.0f, ((argb >> 8) & 0xFF) / 255.0f, (argb & 0xFF) / 255.0f);
    pvr_poly_cxt_col(&cxt, PVR_LIST_TR_POLY);
    cxt.blend.src = PVR_BLEND_ONE;
    cxt.blend.dst = PVR_BLEND_ZERO;
    cxt.depth.comparison = PVR_DEPTHCMP_ALWAYS;
    cxt.depth.write = false;
    cxt.gen.culling = PVR_CULLING_NONE;
    pvr_poly_compile(&hdr, &cxt);
    send_hdr(&hdr);
    B.hdr_valid = 0;
    v.argb = argb | 0xFF000000u; v.oargb = 0; v.z = 1.0f; v.u = v.v = 0;
    v.flags = PVR_CMD_VERTEX; v.x = x0 * B.sx; v.y = y1 * B.sy; dr_send32(&v);
    v.flags = PVR_CMD_VERTEX; v.x = x0 * B.sx; v.y = y0 * B.sy; dr_send32(&v);
    v.flags = PVR_CMD_VERTEX; v.x = x1 * B.sx; v.y = y1 * B.sy; dr_send32(&v);
    v.flags = PVR_CMD_VERTEX_EOL; v.x = x1 * B.sx; v.y = y0 * B.sy; dr_send32(&v);
}

/* ------------------------------------------------------------------ triangles */
static void compile_state(const F3DState *st, const F3DVert *v, int count)
{
    pvr_poly_cxt_t cxt;
    pvr_poly_hdr_t hdr;
    const TexEntry *e = (const TexEntry *)st->tex;
    int i, below = 0, above = 0;
    int blend = st->blend;

    B.skip = 0;
    B.force_alpha255 = 0;
    if (blend == F3D_BLEND_NOWRITE) { B.skip = 1; return; }

    /* N64 alpha test with an opaque blender: emulate with alpha blending on
     * (mostly 1-bit) alpha; untextured polygons are decided per vertex */
    if (blend == F3D_BLEND_OPAQUE && st->alpha_ref) {
        if (!st->tex_alpha) {
            for (i = 0; i < count; i++) {
                if (v[i].a < st->alpha_ref) below = 1; else above = 1;
            }
            if (!above) { B.skip = 1; return; }
            if (!below) B.force_alpha255 = 1;
            else blend = F3D_BLEND_ALPHA;
        } else {
            B.force_alpha255 = 1;           /* only the texel alpha decides */
            blend = F3D_BLEND_ALPHA;
        }
    } else if (blend == F3D_BLEND_OPAQUE) {
        B.force_alpha255 = 1;
    }

    if (e && st->tex_rgb) {
        pvr_poly_cxt_txr(&cxt, PVR_LIST_TR_POLY, (int)e->fmt, e->w2, e->h2, e->ptr,
                         st->filter == F3D_FILTER_BILINEAR ? PVR_FILTER_BILINEAR : PVR_FILTER_NEAREST);
        cxt.txr.env = PVR_TXRENV_MODULATEALPHA;
        cxt.txr.alpha = !st->tex_alpha;     /* true = ignore the texel alpha */
        cxt.txr.uv_clamp = (st->wrap_s == F3D_WRAP_CLAMP ? (st->wrap_t == F3D_WRAP_CLAMP ? PVR_UVCLAMP_UV : PVR_UVCLAMP_U)
                                                          : (st->wrap_t == F3D_WRAP_CLAMP ? PVR_UVCLAMP_V : PVR_UVCLAMP_NONE));
        cxt.txr.uv_flip = (st->wrap_s == F3D_WRAP_MIRROR ? (st->wrap_t == F3D_WRAP_MIRROR ? PVR_UVFLIP_UV : PVR_UVFLIP_U)
                                                          : (st->wrap_t == F3D_WRAP_MIRROR ? PVR_UVFLIP_V : PVR_UVFLIP_NONE));
        cxt.gen.specular = st->offset ? true : false;
    } else {
        pvr_poly_cxt_col(&cxt, PVR_LIST_TR_POLY);
    }
    cxt.gen.culling = PVR_CULLING_NONE;     /* the interpreter culls */
    cxt.gen.alpha = blend != F3D_BLEND_OPAQUE;
    switch (blend) {
    case F3D_BLEND_ALPHA: cxt.blend.src = PVR_BLEND_SRCALPHA; cxt.blend.dst = PVR_BLEND_INVSRCALPHA; break;
    case F3D_BLEND_ADD:   cxt.blend.src = PVR_BLEND_SRCALPHA; cxt.blend.dst = PVR_BLEND_ONE; break;
    default:              cxt.blend.src = PVR_BLEND_ONE; cxt.blend.dst = PVR_BLEND_ZERO; break;
    }
    cxt.depth.comparison = st->z_cmp ? PVR_DEPTHCMP_GEQUAL : PVR_DEPTHCMP_ALWAYS;
    cxt.depth.write = st->z_upd ? true : false;
    pvr_poly_compile(&hdr, &cxt);
    send_hdr(&hdr);
    B.stats.polys_hdr++;
    fr_hdrs++;
    B.cur_blend = blend != F3D_BLEND_OPAQUE;
}

static uint32_t argb_of(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
    return ((uint32_t)a << 24) | ((uint32_t)r << 16) | ((uint32_t)g << 8) | b;
}

static void pb_draw(void *ctx, const F3DState *st, const F3DVert *v, int count)
{
    const TexEntry *e = (const TexEntry *)st->tex;
    float us = 1.0f, vs = 1.0f;
    int i;
    (void)ctx;
    if (!B.in_frame) return;
    /* alpha decisions for untextured alpha-tested polygons depend on the
     * vertices, so those headers are rebuilt per batch */
    if (!B.hdr_valid || st->serial != B.hdr_serial || (st->alpha_ref && !st->tex_alpha)) {
        compile_state(st, v, count);
        B.hdr_serial = st->serial;
        B.hdr_valid = 1;
    }
    if (B.skip)
        return;
    if (e && st->tex_rgb) {
        us = (float)st->tex_w / (float)e->w2;
        vs = (float)st->tex_h / (float)e->h2;
    }
    for (i = 0; i < count; i++) {
        const F3DVert *s = &v[i];
        pvr_vertex_t *pv = (pvr_vertex_t *)pvr_dr_target();
        float z;
        if (s->w != 1.0f || st->rect) z = st->rect ? 1.0f + (1.0f - s->z) : 1.0f / s->w;
        else z = 1.0f + (1.0f - s->z);     /* orthographic: N64 z, nearer = larger */
        pv->flags = (i % 3 == 2) ? PVR_CMD_VERTEX_EOL : PVR_CMD_VERTEX;
        pv->x = s->x * B.sx;
        pv->y = s->y * B.sy;
        pv->z = z;
        pv->u = s->u * us;
        pv->v = s->v * vs;
        pv->argb = argb_of(s->r, s->g, s->b, B.force_alpha255 ? 255 : s->a);
        pv->oargb = argb_of(s->or_, s->og, s->ob, 0);
        pvr_dr_commit(pv);
    }
    B.stats.tris += count / 3;
    fr_tris += (uint32_t)(count / 3);
    for (i = 0; i + 2 < count; i += 3) {
        float a = ((v[i + 1].x - v[i].x) * (v[i + 2].y - v[i].y) - (v[i + 2].x - v[i].x) * (v[i + 1].y - v[i].y)) *
                  (0.5f * B.sx * B.sy);
        uint32_t px = (uint32_t)(a < 0 ? -a : a);
        if (B.cur_blend) { fr_blend += px; fr_blend_polys++; } else fr_opq += px;
    }
}

/* ------------------------------------------------------------------ setup */
void pvr_backend_fill_params(void *out)
{
    pvr_init_params_t *p = out;
    memset(p, 0, sizeof(*p));
    p->opb_sizes[PVR_LIST_OP_POLY] = PVR_BINSIZE_0;
    p->opb_sizes[PVR_LIST_OP_MOD] = PVR_BINSIZE_0;
    p->opb_sizes[PVR_LIST_TR_POLY] = PVR_BINSIZE_32;
    p->opb_sizes[PVR_LIST_TR_MOD] = PVR_BINSIZE_0;
    p->opb_sizes[PVR_LIST_PT_POLY] = PVR_BINSIZE_0;
    p->vertex_buf_size = 1024 * 1024;
    p->dma_enabled = 0;
    p->fsaa_enabled = 0;
    p->autosort_disabled = 1;          /* keep the N64 draw order */
    p->opb_overflow_count = 3;
}

void pvr_backend_init(F3DBackend *be)
{
    if (buckets_ready) pvr_backend_flush_textures();
    memset(&B, 0, sizeof(B));
    buckets_init();
    B.sx = B.sy = 2.0f;
    memset(be, 0, sizeof(*be));
    be->ctx = &B;
    be->begin_frame = pb_begin;
    be->end_frame = pb_end;
    be->color_image = pb_cimg;
    be->clear_depth = pb_clear_depth;
    be->fill_rect = pb_fill;
    be->tex_find = pb_tex_find;
    be->tex_upload = pb_tex_upload;
    be->draw_tris = pb_draw;
    be->caps = F3D_CAP_RGBA16;
}
