/* Vivid Dolls port - recording F3D backend for host tests. */
#include "null_backend.h"

#include <string.h>

static void nb_begin(void *ctx) { ((NullBackend *)ctx)->frames_begun++; }
static void nb_end(void *ctx) { ((NullBackend *)ctx)->frames_ended++; }

static void nb_cimg(void *ctx, uint32_t addr, int fmt, int siz, int width)
{
    NullBackend *nb = ctx;
    (void)fmt;
    nb->cimg_addr = addr;
    nb->cimg_siz = siz;
    nb->cimg_width = width;
}

static void add_rect(NullBackend *nb, float x0, float y0, float x1, float y1, uint32_t argb, int depth)
{
    NBRect *r;
    if (nb->nrects >= NB_MAX_RECTS) return;
    r = &nb->rects[nb->nrects++];
    r->x0 = x0; r->y0 = y0; r->x1 = x1; r->y1 = y1; r->argb = argb; r->depth = depth;
}

static void nb_clear_depth(void *ctx, float x0, float y0, float x1, float y1)
{
    add_rect(ctx, x0, y0, x1, y1, 0, 1);
}

static void nb_fill(void *ctx, float x0, float y0, float x1, float y1, uint32_t argb)
{
    add_rect(ctx, x0, y0, x1, y1, argb, 0);
}

/* handles are 1-based indices cast to pointers */
static void *nb_find(void *ctx, uint64_t key)
{
    NullBackend *nb = ctx;
    int i;
    for (i = 0; i < nb->ntex; i++)
        if (nb->tex[i].key == key) {
            nb->finds_hit++;
            return (void *)(intptr_t)(i + 1);
        }
    return NULL;
}

static void *nb_upload(void *ctx, const F3DTexImage *img)
{
    NullBackend *nb = ctx;
    NBTex *t;
    int n = img->width * img->height;
    nb->uploads++;
    if (nb->ntex >= NB_MAX_TEX || nb->pool_used + n > NB_MAX_TEXELS)
        return NULL;
    t = &nb->tex[nb->ntex];
    t->key = img->key; t->w = img->width; t->h = img->height;
    t->fmt = img->fmt; t->siz = img->siz; t->alpha = img->alpha;
    t->texels = &nb->pool[nb->pool_used];
    nb->last_stride = (int)img->stride;
    if (img->rgba16) {
        int x, y, pitch = img->stride ? (int)img->stride : img->width;
        for (y = 0; y < img->height; y++)
            for (x = 0; x < img->width; x++) {
                uint16_t c = img->rgba16[y * pitch + x];
                uint32_t r = (c >> 11) & 31, g = (c >> 6) & 31, b = (c >> 1) & 31;
                r = (r << 3) | (r >> 2); g = (g << 3) | (g >> 2); b = (b << 3) | (b >> 2);
                t->texels[y * img->width + x] = ((c & 1) ? 0xFF000000u : 0) | (r << 16) | (g << 8) | b;
            }
    } else {
        memcpy(t->texels, img->argb, (size_t)n * 4);
    }
    nb->pool_used += n;
    nb->ntex++;
    return (void *)(intptr_t)nb->ntex;
}

static void nb_draw(void *ctx, const F3DState *st, const F3DVert *v, int count)
{
    NullBackend *nb = ctx;
    int i;
    for (i = 0; i + 2 < count && nb->ntris < NB_MAX_TRIS; i += 3) {
        NBTri *t = &nb->tris[nb->ntris++];
        memcpy(t->v, &v[i], sizeof(t->v));
        t->st = *st;
        t->tex_index = st->tex ? (int)(intptr_t)st->tex - 1 : -1;
    }
}

void null_backend_clear(NullBackend *nb)
{
    nb->ntris = 0;
    nb->nrects = 0;
}

void null_backend_set_rgba16(F3DBackend *be, int on)
{
    be->caps = on ? (be->caps | F3D_CAP_RGBA16) : (be->caps & ~(unsigned)F3D_CAP_RGBA16);
}

void null_backend_init(F3DBackend *be, NullBackend *nb)
{
    memset(nb, 0, sizeof(*nb));
    memset(be, 0, sizeof(*be));
    be->ctx = nb;
    be->begin_frame = nb_begin;
    be->end_frame = nb_end;
    be->color_image = nb_cimg;
    be->clear_depth = nb_clear_depth;
    be->fill_rect = nb_fill;
    be->tex_find = nb_find;
    be->tex_upload = nb_upload;
    be->draw_tris = nb_draw;
}
