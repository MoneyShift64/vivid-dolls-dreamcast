/* Vivid Dolls port - F3D (libultra 2.0H Fast3D) display-list interpreter.
 *
 * Replaces the RSP graphics microcode and the RDP.  Portable C99; the
 * Dreamcast build defines F3D_USE_SH4ZAM to run the matrix maths on the SH4's
 * XMTRX/FTRV (see f3d_mtx_mul / F3D_XFORM below).
 *
 * Usage:
 *     f3d_init(&backend, addr_hook);
 *     f3d_set_ucode(gGfxMode);           // 0..3, see f3d.c for the Y-axis note
 *     f3d_run_task(task->data_ptr);      // one graphics task = one frame
 *
 * Memory: every address found in a display list goes through
 *   1. segment resolution (only for addresses < 0x10000000, i.e. segmented or
 *      physical N64 addresses: seg[(a >> 24) & 15] + (a & 0xFFFFFF)), then
 *   2. the addr hook, which returns a host pointer (identity on the Dreamcast).
 * Structures (Gfx, Vtx, Mtx words, Vp, lights) are read in host byte order;
 * 16-bit texels/TLUT entries are read as host-order u16 (see ARCHITECTURE.md 3).
 */
#ifndef F3D_H
#define F3D_H

#include <stdint.h>
#include "f3d_backend.h"

typedef void *(*F3DAddrFn)(uint32_t addr);

typedef struct F3DStats {
    uint32_t cmds;          /* commands executed */
    uint32_t vtx;           /* vertices loaded */
    uint32_t tris_in;       /* G_TRI1 executed */
    uint32_t tris_culled;   /* rejected by culling / clipping */
    uint32_t tris_out;      /* triangles sent to the backend (after fan split) */
    uint32_t rects;         /* TEXRECT / FILLRECT */
    uint32_t tex_decodes;   /* texture cache misses */
    uint32_t unknown;       /* unknown / unsupported commands */
    uint32_t last_unknown;  /* w0 of the last one */
    uint32_t loadblocks, loadtiles, loadtluts;
    uint32_t load_bytes;    /* bytes copied into TMEM */
    uint32_t hash_bytes;    /* bytes hashed for texture keys */
    uint32_t tex_lookups;   /* texture key computations (cache-state misses) */
    uint32_t tex_direct;    /* misses converted straight from the load source (no TMEM copy) */
} F3DStats;

void  f3d_init(const F3DBackend *be, F3DAddrFn addr);
void  f3d_set_ucode(int gfx_mode);      /* gGfxMode: 0..3 */
void  f3d_reset(void);                  /* power-on RSP/RDP state */
void  f3d_run_task(uint32_t dl);        /* begin_frame, run, end_frame */
void  f3d_run_dl(uint32_t dl);          /* run without frame callbacks */
const F3DStats *f3d_stats(void);
void  f3d_clear_stats(void);
/* Optional diagnostics sink (NULL = silent). */
void  f3d_set_log(void (*log)(const char *msg));

/* ---- maths hooks ------------------------------------------------------
 * Matrices are float[16] in N64 order: row-vector convention, m[row*4+col],
 * translation in m[12..14].  Read as column-major (SH4ZAM's layout) the same
 * memory is the transpose, so it can be loaded into XMTRX unchanged.
 *   f3d_mtx_mul(out, a, b): out = a * b  (apply a first, then b)
 *   F3D_XFORM_LOAD(m) + f3d_xform(out, x, y, z): out = (x, y, z, 1) * m
 * f3d_mtx_selftest() compares the hooked versions with the C ones (returns 0
 * when they agree) - call it once at boot on the Dreamcast.
 */
void f3d_mtx_mul_c(float out[16], const float a[16], const float b[16]);
int  f3d_mtx_selftest(void);

#if defined(F3D_USE_SH4ZAM)
#include <sh4zam/shz_xmtrx.h>
static inline void f3d_mtx_mul(float out[16], const float a[16], const float b[16])
{
    /* column-major: out' = b' * a'  ->  load b, apply a */
    float tmp[16] __attribute__((aligned(8)));
    shz_xmtrx_load_apply_store_unaligned_4x4(tmp, b, a);
    for (int i = 0; i < 16; i++) out[i] = tmp[i];
}
#define F3D_XFORM_LOAD(m) shz_xmtrx_load_unaligned_4x4(m)
static inline void f3d_xform(float out[4], const float m[16], float x, float y, float z)
{
    shz_vec4_t r;
    (void)m;
    r = shz_xmtrx_transform_vec4(shz_vec4_init(x, y, z, 1.0f));
    out[0] = r.x; out[1] = r.y; out[2] = r.z; out[3] = r.w;
}
#else
#define f3d_mtx_mul f3d_mtx_mul_c
#define F3D_XFORM_LOAD(m) ((void)0)
static inline void f3d_xform(float out[4], const float m[16], float x, float y, float z)
{
    out[0] = x * m[0] + y * m[4] + z * m[8]  + m[12];
    out[1] = x * m[1] + y * m[5] + z * m[9]  + m[13];
    out[2] = x * m[2] + y * m[6] + z * m[10] + m[14];
    out[3] = x * m[3] + y * m[7] + z * m[11] + m[15];
}
#endif

/* ---- access to internal state (tests, debugging) ---------------------- */
typedef struct F3DTile {
    uint8_t fmt, siz, pal, cms, cmt, masks, maskt, shifts, shiftt;
    uint16_t line, tmem;    /* in 64-bit words */
    uint16_t uls, ult, lrs, lrt;   /* 10.2 */
} F3DTile;
const uint8_t  *f3d_tmem(void);       /* 4096 bytes */
const uint16_t *f3d_tlut(void);       /* 256 entries */
const F3DTile  *f3d_tile(int i);
uint32_t        f3d_geometry_mode(void);
uint32_t        f3d_othermode_h(void);
uint32_t        f3d_othermode_l(void);
/* Decode tile `t` (size from its tile descriptor) into out (ARGB8888); returns
 * texels written, 0 if it doesn't fit max_texels. */
int f3d_decode_tile(int t, uint32_t *out, int max_texels, int *w, int *h);

#endif
