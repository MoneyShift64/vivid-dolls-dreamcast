/* Vivid Dolls port - recording F3D backend for host tests. */
#ifndef NULL_BACKEND_H
#define NULL_BACKEND_H

#include "f3d_backend.h"

#define NB_MAX_TRIS   4096
#define NB_MAX_TEX    64
#define NB_MAX_RECTS  64
#define NB_MAX_TEXELS (64 * 1024)

typedef struct {
    F3DVert v[3];
    F3DState st;
    int tex_index;          /* index into textures[], -1 = none */
} NBTri;

typedef struct {
    uint64_t key;
    int w, h, fmt, siz, alpha;
    uint32_t *texels;       /* into the pool */
} NBTex;

typedef struct {
    float x0, y0, x1, y1;
    uint32_t argb;
    int depth;              /* 1 = clear_depth */
} NBRect;

typedef struct NullBackend {
    int frames_begun, frames_ended;
    int ntris, ntex, nrects;
    int uploads, finds_hit;
    uint32_t cimg_addr; int cimg_width, cimg_siz;
    NBTri tris[NB_MAX_TRIS];
    NBTex tex[NB_MAX_TEX];
    NBRect rects[NB_MAX_RECTS];
    uint32_t pool[NB_MAX_TEXELS];
    int pool_used;
    int last_stride;        /* F3DTexImage.stride of the last upload (texels, 0 = width) */
} NullBackend;

/* accept F3DTexImage.rgba16 (F3D_CAP_RGBA16): expanded to ARGB8888 on upload */
void null_backend_set_rgba16(F3DBackend *be, int on);

void null_backend_init(F3DBackend *be, NullBackend *nb);
void null_backend_clear(NullBackend *nb);   /* forget primitives (keeps textures) */

#endif
