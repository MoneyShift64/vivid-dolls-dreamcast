/* Vivid Dolls port - F3D interpreter -> renderer backend interface.
 *
 * The interpreter (f3d.c) does everything the RSP and the RDP's set-up stage do:
 * transform, lighting, texgen, clipping (near/far + scissor), culling, viewport,
 * texture loads into an emulated TMEM, texture decode, colour-combiner reduction.
 * A backend only receives screen-space triangles with a small state block.
 *
 * Coordinates are N64 screen pixels of the colour image (320x240 for this game),
 * y down, sub-pixel floats.  The backend scales to its own resolution.
 *
 * Plain C99, no KOS dependency.
 */
#ifndef F3D_BACKEND_H
#define F3D_BACKEND_H

#include <stdint.h>

/* One output vertex. */
typedef struct F3DVert {
    float x, y;          /* screen pixels, y down */
    float z;             /* N64 depth mapped to 0 (near) .. 1 (far) */
    float w;             /* clip-space w (> 0); 1 for rectangles and orthographic views */
    float u, v;          /* normalised texture coordinates: 0..1 spans tex_w/tex_h texels */
    uint8_t r, g, b, a;  /* modulate colour (see F3DState.tex_rgb / tex_alpha) */
    uint8_t or_, og, ob, oa; /* offset ("specular") colour, added after modulation */
} F3DVert;

enum { F3D_FILTER_POINT = 0, F3D_FILTER_BILINEAR = 1 };
enum { F3D_WRAP_REPEAT = 0, F3D_WRAP_MIRROR = 1, F3D_WRAP_CLAMP = 2 };
enum {
    F3D_BLEND_OPAQUE = 0,  /* dst = src */
    F3D_BLEND_ALPHA  = 1,  /* dst = src*a + dst*(1-a) */
    F3D_BLEND_ADD    = 2,  /* dst = src*a + dst */
    F3D_BLEND_NOWRITE = 3  /* dst = dst (colour writes off, e.g. z-only) */
};
enum { F3D_CYCLE_1 = 0, F3D_CYCLE_2 = 1, F3D_CYCLE_COPY = 2, F3D_CYCLE_FILL = 3 };

/* Draw state for a batch of triangles.  `serial` changes whenever any other
 * field changes, so a backend can cache its polygon header by serial. */
typedef struct F3DState {
    void    *tex;        /* backend texture handle from tex_find/tex_upload, NULL = none */
    uint16_t tex_w, tex_h; /* decoded texture size (u,v are normalised to this) */
    uint8_t  filter;     /* F3D_FILTER_* */
    uint8_t  wrap_s, wrap_t; /* F3D_WRAP_* */
    uint8_t  blend;      /* F3D_BLEND_* */
    uint8_t  alpha_ref;  /* 0 = no alpha test, else discard pixels with alpha < alpha_ref */
    uint8_t  z_cmp;      /* depth test (less-or-equal, N64 z: smaller = nearer) */
    uint8_t  z_upd;      /* depth write */
    uint8_t  z_decal;    /* ZMODE_DEC (coplanar decal) */
    uint8_t  tex_rgb;    /* rgb = texel.rgb * mod.rgb + off.rgb  (else rgb = mod.rgb) */
    uint8_t  tex_alpha;  /* a   = texel.a * mod.a + off.a        (else a = mod.a)    */
    uint8_t  offset;     /* some vertex of this batch has a non-zero offset colour */
    uint8_t  cycle;      /* F3D_CYCLE_* */
    uint8_t  rect;       /* 1 = from TEXRECT/FILLRECT (screen space, w = 1) */
    uint32_t othermode_h, othermode_l, combine_h, combine_l;   /* raw RDP state */
    uint32_t serial;
} F3DState;

/* A decoded texture, handed to tex_upload when tex_find misses. */
typedef struct F3DTexImage {
    uint64_t key;        /* content hash (TMEM bytes used + TLUT used + format/size) */
    uint16_t width, height;
    uint8_t  fmt, siz;   /* N64 source format (G_IM_FMT_*, G_IM_SIZ_*) */
    uint8_t  alpha;      /* 0 = all opaque, 1 = only 0/255 alpha, 2 = partial alpha */
    uint8_t  pad;        /* bit 0: the tile clamps in s, bit 1: in t (pad by edge replication) */
    const uint32_t *argb; /* width*height ARGB8888 (0xAARRGGBB), row-major, top row first */
    /* Only with F3D_CAP_RGBA16 and an RGBA16 texture without TLUT: argb is NULL and
       rgba16 holds the width*height N64 RGBA5551 texels (host u16, row-major); the
       colour each stands for is the ARGB8888 the interpreter would have given
       (5-bit channels expanded as (c << 3) | (c >> 2), alpha bit -> 0 / 255). */
    const uint16_t *rgba16;
    /* Row pitch of rgba16 in texels (>= width). The interpreter may point rgba16
       straight at the source image of a LOADTILE (photo strips) instead of copying. */
    uint32_t stride;
} F3DTexImage;
#define F3D_TEXIMAGE_STRIDE 1

/* F3DBackend.caps */
#define F3D_CAP_RGBA16 1      /* tex_upload accepts F3DTexImage.rgba16 */

typedef struct F3DBackend {
    void *ctx;
    void  (*begin_frame)(void *ctx);
    void  (*end_frame)(void *ctx);
    /* G_SETCIMG: addr is the resolved N64/port address; siz G_IM_SIZ_*. */
    void  (*color_image)(void *ctx, uint32_t addr, int fmt, int siz, int width);
    /* G_FILLRECT with the colour image == Z image: clear depth in that rectangle. */
    void  (*clear_depth)(void *ctx, float x0, float y0, float x1, float y1);
    /* G_FILLRECT in fill mode: solid colour, no blending, pixels [x0,x1) x [y0,y1). */
    void  (*fill_rect)(void *ctx, float x0, float y0, float x1, float y1, uint32_t argb);
    /* Texture cache: return the handle for key or NULL; tex_upload creates one. */
    void *(*tex_find)(void *ctx, uint64_t key);
    void *(*tex_upload)(void *ctx, const F3DTexImage *img);
    /* Triangles: count vertices, 3 per triangle, already clipped and culled. */
    void  (*draw_tris)(void *ctx, const F3DState *st, const F3DVert *v, int count);
    unsigned caps;      /* F3D_CAP_* */
} F3DBackend;

#endif
