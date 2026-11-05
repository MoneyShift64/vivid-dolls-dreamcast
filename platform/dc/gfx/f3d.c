/* Vivid Dolls port - F3D (libultra 2.0H Fast3D) display-list interpreter.
 * See f3d.h and port/notes/gfx.md.
 *
 * Y axis and the microcode variants: Gfx_SetFullViewport / Gfx_SetViewport
 * store a NEGATIVE vscale[1] when gGfxMode < 2 (the mode the game always uses
 * is 0) and a positive one for modes 2 and 3 (the SGI F3D 2.0H xbus/fifo
 * microcodes, which negate Y as usual: screen_y = vtrans - y*vscale).  The
 * picture is upright in every mode, so the mode-0/1 microcode must map
 * screen_y = vtrans + y*vscale.  f3d_set_ucode() selects the sign.
 */
#include "f3d.h"

#include <math.h>
#include <string.h>

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;

/* ------------------------------------------------------------------ opcodes */
enum {
    OP_SPNOOP = 0x00, OP_MTX = 0x01, OP_MOVEMEM = 0x03, OP_VTX = 0x04, OP_DL = 0x06,
    OP_TRI1 = 0xBF, OP_CULLDL = 0xBE, OP_POPMTX = 0xBD, OP_MOVEWORD = 0xBC, OP_TEXTURE = 0xBB,
    OP_SETOTHERMODE_H = 0xBA, OP_SETOTHERMODE_L = 0xB9, OP_ENDDL = 0xB8,
    OP_SETGEOMETRYMODE = 0xB7, OP_CLEARGEOMETRYMODE = 0xB6, OP_LINE3D = 0xB5,
    OP_RDPHALF_1 = 0xB4, OP_RDPHALF_2 = 0xB3, OP_RDPHALF_CONT = 0xB2,
    OP_TEXRECT = 0xE4, OP_TEXRECTFLIP = 0xE5, OP_RDPLOADSYNC = 0xE6, OP_RDPPIPESYNC = 0xE7,
    OP_RDPTILESYNC = 0xE8, OP_RDPFULLSYNC = 0xE9, OP_SETKEYGB = 0xEA, OP_SETKEYR = 0xEB,
    OP_SETCONVERT = 0xEC, OP_SETSCISSOR = 0xED, OP_SETPRIMDEPTH = 0xEE, OP_RDPSETOTHERMODE = 0xEF,
    OP_LOADTLUT = 0xF0, OP_SETTILESIZE = 0xF2, OP_LOADBLOCK = 0xF3, OP_LOADTILE = 0xF4,
    OP_SETTILE = 0xF5, OP_FILLRECT = 0xF6, OP_SETFILLCOLOR = 0xF7, OP_SETFOGCOLOR = 0xF8,
    OP_SETBLENDCOLOR = 0xF9, OP_SETPRIMCOLOR = 0xFA, OP_SETENVCOLOR = 0xFB, OP_SETCOMBINE = 0xFC,
    OP_SETTIMG = 0xFD, OP_SETZIMG = 0xFE, OP_SETCIMG = 0xFF
};

/* geometry mode (F3D) */
#define G_ZBUFFER            0x00000001
#define G_TEXTURE_ENABLE     0x00000002
#define G_SHADE              0x00000004
#define G_SHADING_SMOOTH     0x00000200
#define G_CULL_FRONT         0x00001000
#define G_CULL_BACK          0x00002000
#define G_FOG                0x00010000
#define G_LIGHTING           0x00020000
#define G_TEXTURE_GEN        0x00040000
#define G_TEXTURE_GEN_LINEAR 0x00080000

/* other mode L */
#define AA_EN          0x0008
#define Z_CMP          0x0010
#define Z_UPD          0x0020
#define CVG_X_ALPHA    0x1000
#define FORCE_BL       0x4000
#define ZMODE_SHIFT    10
#define ZSRC_PRIM      0x0004

#define MV_STACK  10
#define DL_STACK  10
#define NUM_VTX   16

/* ------------------------------------------------------------------ state */
typedef struct {
    float x, y, z, w;       /* clip space */
    float s, t;             /* texels (after G_TEXTURE scale) */
    float r, g, b, a;       /* shade 0..255 */
    float fog;              /* 0..1 */
    u8 clip;                /* outcode */
} Vtx;

typedef struct { float col[3]; float dir[3]; } Light;

/* combiner input after reduction: value = t*TEXEL + c */
typedef struct { float t, c; } Lin;

static struct {
    const F3DBackend *be;
    F3DAddrFn addr;
    void (*log)(const char *);
    float ysign;                 /* -1 standard F3D, +1 for gGfxMode 0/1 */

    /* RSP */
    u32 seg[16];
    float mv[MV_STACK][16];
    int mv_top;
    float proj[16];
    float mvp[16];
    int mvp_dirty;
    float vscale[3], vtrans[3];
    Light light[8];
    float amb[3];
    int numlights;
    float lookat[2][3];
    float ldir_model[8][3];      /* light dirs in model space (F3D transforms lights, not normals) */
    float lookat_model[2][3];
    int lights_dirty;
    u32 geom;
    float tex_ss, tex_st;        /* G_TEXTURE scale */
    int tex_on, tex_tile, tex_level;
    s16 fog_mul, fog_off;
    Vtx vtx[NUM_VTX];
    u32 rdphalf1, rdphalf2;

    /* RDP */
    F3DTile tile[8];
    u8 tmem[4096] __attribute__((aligned(8)));
    u16 tlut[256];
    u32 timg_addr; int timg_fmt, timg_siz, timg_width;
    u32 cimg_addr; int cimg_fmt, cimg_siz, cimg_width;
    u32 zimg_addr;
    u32 omh, oml;
    u32 cc_h, cc_l;
    u8 prim[4], env[4], fog[4], blend[4];
    u8 prim_min_level, prim_lod_frac;
    u32 fill_color;
    u16 prim_z, prim_dz;
    float sc_x0, sc_y0, sc_x1, sc_y1;

    /* lazy TMEM loads (see "RDP: TMEM") */
    struct PLoad {
        u8 kind;            /* PL_BLOCK / PL_TILE */
        u8 applied;         /* already copied into tmem[] */
        u8 hashed;
        u16 lo;             /* first TMEM byte written */
        u16 len;            /* conservative span [lo, lo + len) mod 4096 (4096 = all) */
        const u8 *src;      /* block: source; tile: first row */
        u32 stride;         /* tile: source row stride in bytes */
        u32 rows, rowbytes, line8;   /* tile */
        u32 words, dxt;     /* block */
        u64 hash;           /* source bytes hash (valid when hashed) */
        u32 hash_r0;        /* first row covered by hash (tile loads) */
    } pl[4];
    int npl;
    u64 tlut_hash_all;
    u64 tlut_hash_bank[16];
    u32 tlut_valid;         /* bit 16: all, bits 0-15: banks */

    /* derived */
    int state_dirty;
    F3DState st;
    F3DState st_last;
    u32 serial;
    int tex_dirty;      /* TMEM/tile/tlut changed since the cached key */
    u64 tex_key;
    void *tex_handle;
    int tex_key_tile;
    u16 tex_w, tex_h;
    /* texel window of the next texture lookup (texrect, see there): on, x0, y0, w, h */
    int crop_on, crop_x0, crop_y0, crop_w, crop_h;
    u32 tex_crop_sig;   /* window of the cached handle */
    int tex_ox, tex_oy; /* window origin of the texture last returned */

    F3DStats stats;
} S;

static u32 tex_buf[256 * 256];

/* ------------------------------------------------------------------ helpers */
#if defined(__GNUC__)
#define F3D_PREFETCH(p) __builtin_prefetch(p)
#else
#define F3D_PREFETCH(p) ((void)0)
#endif

/* equality of n bytes (n multiple of 4, both 4-aligned): the library memcmp
   compares byte by byte on the SH4 and showed in the profile */
static inline int eq32(const void *a, const void *b, u32 n)
{
    const u32 *x = (const u32 *)a, *y = (const u32 *)b;
    for (n >>= 2; n; n--)
        if (*x++ != *y++) return 0;
    return 1;
}

static void logmsg(const char *m) { if (S.log) S.log(m); }

static void *ptr(u32 a)
{
    if (a < 0x10000000u)
        a = S.seg[(a >> 24) & 0xF] + (a & 0x00FFFFFFu);
    return S.addr(a);
}

static u32 resolve(u32 a)
{
    if (a < 0x10000000u)
        a = S.seg[(a >> 24) & 0xF] + (a & 0x00FFFFFFu);
    return a;
}

static float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }

void f3d_mtx_mul_c(float out[16], const float a[16], const float b[16])
{
    float r[16];
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            r[i * 4 + j] = a[i * 4 + 0] * b[0 * 4 + j] + a[i * 4 + 1] * b[1 * 4 + j] +
                           a[i * 4 + 2] * b[2 * 4 + j] + a[i * 4 + 3] * b[3 * 4 + j];
    memcpy(out, r, sizeof(r));
}

int f3d_mtx_selftest(void)
{
    float a[16], b[16], r1[16], r2[16], v1[4], v2[4];
    int i, bad = 0;
    for (i = 0; i < 16; i++) {
        a[i] = (float)((i * 7) % 5) - 1.5f;
        b[i] = (float)((i * 3) % 7) * 0.25f - 0.5f;
    }
    f3d_mtx_mul(r1, a, b);
    f3d_mtx_mul_c(r2, a, b);
    for (i = 0; i < 16; i++)
        if (fabsf(r1[i] - r2[i]) > 1e-3f) bad |= 1;
    F3D_XFORM_LOAD(a);
    f3d_xform(v1, a, 1.0f, 2.0f, 3.0f);
    v2[0] = 1 * a[0] + 2 * a[4] + 3 * a[8] + a[12];
    v2[1] = 1 * a[1] + 2 * a[5] + 3 * a[9] + a[13];
    v2[2] = 1 * a[2] + 2 * a[6] + 3 * a[10] + a[14];
    v2[3] = 1 * a[3] + 2 * a[7] + 3 * a[11] + a[15];
    for (i = 0; i < 4; i++)
        if (fabsf(v1[i] - v2[i]) > 1e-3f) bad |= 2;
    return bad;
}

/* libultra Mtx: 16 s32 words; word k of the first 8 = integer halves of
 * elements 2k (high half) and 2k+1 (low half); the last 8 = fractions.  Read
 * as u32 words, which keeps it independent of byte order (Mtx_ToFixed writes
 * whole words too). */
static void read_mtx(float out[16], u32 a)
{
    const u32 *w = (const u32 *)ptr(a);
    int i;
    for (i = 0; i < 16; i++) {
        u32 iw = w[i >> 1], fw = w[8 + (i >> 1)];
        u32 ip = (i & 1) ? (iw & 0xFFFF) : (iw >> 16);
        u32 fp = (i & 1) ? (fw & 0xFFFF) : (fw >> 16);
        out[i] = (float)(s32)((ip << 16) | fp) * (1.0f / 65536.0f);
    }
}

static void identity(float m[16])
{
    memset(m, 0, 16 * sizeof(float));
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

static void normalize3(float v[3])
{
    float l = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
    if (l > 1e-12f) {
        l = 1.0f / sqrtf(l);
        v[0] *= l; v[1] *= l; v[2] *= l;
    }
}

/* ------------------------------------------------------------------ init */
void f3d_set_log(void (*log)(const char *)) { S.log = log; }
const F3DStats *f3d_stats(void) { return &S.stats; }
void f3d_clear_stats(void) { memset(&S.stats, 0, sizeof(S.stats)); }
static void pl_flush(void);
const uint8_t *f3d_tmem(void) { pl_flush(); return S.tmem; }
const uint16_t *f3d_tlut(void) { return S.tlut; }
const F3DTile *f3d_tile(int i) { return &S.tile[i & 7]; }
uint32_t f3d_geometry_mode(void) { return S.geom; }
uint32_t f3d_othermode_h(void) { return S.omh; }
uint32_t f3d_othermode_l(void) { return S.oml; }

void f3d_set_ucode(int mode)
{
    S.ysign = ((unsigned)mode < 2) ? 1.0f : -1.0f;
}

void f3d_reset(void)
{
    const F3DBackend *be = S.be;
    F3DAddrFn addr = S.addr;
    void (*log)(const char *) = S.log;
    float ys = S.ysign;
    memset(&S, 0, sizeof(S));
    S.be = be; S.addr = addr; S.log = log; S.ysign = ys != 0.0f ? ys : 1.0f;
    identity(S.mv[0]);
    identity(S.proj);
    S.mvp_dirty = 1;
    S.vscale[0] = 640.0f / 4; S.vscale[1] = 480.0f / 4; S.vscale[2] = 511.0f;
    S.vtrans[0] = 640.0f / 4; S.vtrans[1] = 480.0f / 4; S.vtrans[2] = 511.0f;
    S.numlights = 1;
    S.lights_dirty = 1;
    S.sc_x1 = 320; S.sc_y1 = 240;
    S.cimg_width = 320;
    S.state_dirty = 1;
    S.tex_dirty = 1;
}

void f3d_init(const F3DBackend *be, F3DAddrFn addr)
{
    S.be = be;
    S.addr = addr;
    if (S.ysign == 0.0f)
        S.ysign = 1.0f;
    f3d_reset();
}

/* ------------------------------------------------------------------ RSP: matrices, lights */
static const float *cur_mvp(void)
{
    if (S.mvp_dirty) {
        f3d_mtx_mul(S.mvp, S.mv[S.mv_top], S.proj);
        S.mvp_dirty = 0;
    }
    return S.mvp;
}

static void cmd_mtx(u32 w0, u32 w1)
{
    float m[16];
    u32 p = (w0 >> 16) & 0xFF;
    read_mtx(m, w1);
    if (p & 1) {                       /* projection: no stack in F3D */
        if (p & 2) memcpy(S.proj, m, sizeof(m));
        else f3d_mtx_mul(S.proj, m, S.proj);
    } else {
        if ((p & 4) && S.mv_top < MV_STACK - 1) {
            memcpy(S.mv[S.mv_top + 1], S.mv[S.mv_top], sizeof(m));
            S.mv_top++;
        }
        if (p & 2) memcpy(S.mv[S.mv_top], m, sizeof(m));
        else f3d_mtx_mul(S.mv[S.mv_top], m, S.mv[S.mv_top]);
        S.lights_dirty = 1;
    }
    S.mvp_dirty = 1;
}

static void read_light(Light *l, u32 a)
{
    const u8 *p = (const u8 *)ptr(a);
    l->col[0] = p[0]; l->col[1] = p[1]; l->col[2] = p[2];
    l->dir[0] = (float)(s8)p[8]; l->dir[1] = (float)(s8)p[9]; l->dir[2] = (float)(s8)p[10];
}

static void cmd_movemem(u32 w0, u32 w1)
{
    u32 idx = (w0 >> 16) & 0xFF;
    if (idx == 0x80) {                                     /* G_MV_VIEWPORT */
        const s16 *v = (const s16 *)ptr(w1);
        S.vscale[0] = v[0] / 4.0f; S.vscale[1] = v[1] / 4.0f; S.vscale[2] = v[2];
        S.vtrans[0] = v[4] / 4.0f; S.vtrans[1] = v[5] / 4.0f; S.vtrans[2] = v[6];
    } else if (idx == 0x82 || idx == 0x84) {               /* LOOKATY / LOOKATX */
        Light l;
        read_light(&l, w1);
        memcpy(S.lookat[idx == 0x84 ? 0 : 1], l.dir, sizeof(l.dir));
        S.lights_dirty = 1;
    } else if (idx >= 0x86 && idx <= 0x94 && !(idx & 1)) { /* L0..L7 */
        int n = (int)(idx - 0x86) >> 1;
        read_light(&S.light[n], w1);
        S.lights_dirty = 1;
    } else if (idx >= 0x98 && idx <= 0x9E) {
        /* G_MV_MATRIX_n: direct MVP load, unused by the game */
        S.stats.unknown++; S.stats.last_unknown = w0;
    } else {
        S.stats.unknown++; S.stats.last_unknown = w0;
    }
}

static void cmd_moveword(u32 w0, u32 w1)
{
    u32 index = w0 & 0xFF, off = (w0 >> 8) & 0xFFFF;
    switch (index) {
    case 0x02:                                    /* NUMLIGHT: 0x80000000 + (n+1)*32 */
        S.numlights = (int)(((w1 & 0x7FFFFFFF) >> 5)) - 1;
        if (S.numlights < 0) S.numlights = 0;
        if (S.numlights > 7) S.numlights = 7;
        S.lights_dirty = 1;
        break;
    case 0x06:                                    /* SEGMENT */
        S.seg[(off >> 2) & 0xF] = w1;
        break;
    case 0x08:                                    /* FOG: mul << 16 | offset */
        S.fog_mul = (s16)(w1 >> 16);
        S.fog_off = (s16)(w1 & 0xFFFF);
        break;
    case 0x0A: {                                  /* LIGHTCOL: n*0x20 (+4 = copy) */
        int n = (int)(off >> 5) & 7;
        if (!(off & 4)) {
            S.light[n].col[0] = (float)(w1 >> 24);
            S.light[n].col[1] = (float)((w1 >> 16) & 0xFF);
            S.light[n].col[2] = (float)((w1 >> 8) & 0xFF);
        }
        break;
    }
    case 0x0E:                                    /* PERSPNORM: precision only */
    case 0x04:                                    /* CLIP ratio */
        break;
    default:                                      /* MATRIX, POINTS: unused */
        S.stats.unknown++; S.stats.last_unknown = w0;
        break;
    }
}

/* F3D transforms light (and lookat) directions into model space with the
 * modelview matrix and dots them with the untransformed normal. */
static void xform_dir(float o[3], const float *m, const float d[3])
{
    o[0] = m[0] * d[0] + m[1] * d[1] + m[2] * d[2];
    o[1] = m[4] * d[0] + m[5] * d[1] + m[6] * d[2];
    o[2] = m[8] * d[0] + m[9] * d[1] + m[10] * d[2];
    normalize3(o);
}

static void update_lights(void)
{
    const float *m = S.mv[S.mv_top];
    int i;
    for (i = 0; i < S.numlights; i++)
        xform_dir(S.ldir_model[i], m, S.light[i].dir);
    xform_dir(S.lookat_model[0], m, S.lookat[0]);
    xform_dir(S.lookat_model[1], m, S.lookat[1]);
    /* the ambient colour is light[numlights] */
    S.amb[0] = S.light[S.numlights].col[0];
    S.amb[1] = S.light[S.numlights].col[1];
    S.amb[2] = S.light[S.numlights].col[2];
    S.lights_dirty = 0;
}

/* ------------------------------------------------------------------ RSP: vertices */
typedef struct { s16 x, y, z; u16 flag; s16 s, t; u8 c[4]; } N64Vtx;

static u8 outcode(const Vtx *v)
{
    u8 c = 0;
    if (v->x < -v->w) c |= 1;
    if (v->x > v->w) c |= 2;
    if (v->y < -v->w) c |= 4;
    if (v->y > v->w) c |= 8;
    if (v->z < -v->w) c |= 16;
    if (v->z > v->w) c |= 32;
    return c;
}

static void cmd_vtx(u32 w0, u32 w1)
{
    int n = (int)((w0 >> 20) & 0xF) + 1, v0 = (int)((w0 >> 16) & 0xF), i;
    const N64Vtx *src = (const N64Vtx *)ptr(w1);
    const float *m = cur_mvp();
    const int lighting = (S.geom & G_LIGHTING) != 0;
    const int texgen = (S.geom & G_TEXTURE_GEN) != 0;

    if (lighting && S.lights_dirty)
        update_lights();
    F3D_XFORM_LOAD(m);
    for (i = 0; i < n; i++) {
        Vtx *d = &S.vtx[(v0 + i) & 15];
        const N64Vtx *s = &src[i];
        float c[4];
        f3d_xform(c, m, (float)s->x, (float)s->y, (float)s->z);
        d->x = c[0]; d->y = c[1]; d->z = c[2]; d->w = c[3];
        d->s = (float)s->s * (1.0f / 32.0f) * S.tex_ss;
        d->t = (float)s->t * (1.0f / 32.0f) * S.tex_st;
        if (lighting) {
            float nx = (float)(s8)s->c[0], ny = (float)(s8)s->c[1], nz = (float)(s8)s->c[2];
            float r = S.amb[0], g = S.amb[1], b = S.amb[2];
            int l;
            for (l = 0; l < S.numlights; l++) {
                const float *ld = S.ldir_model[l];
                float k = (nx * ld[0] + ny * ld[1] + nz * ld[2]) * (1.0f / 127.0f);
                if (k > 0.0f) {
                    r += S.light[l].col[0] * k;
                    g += S.light[l].col[1] * k;
                    b += S.light[l].col[2] * k;
                }
            }
            d->r = clampf(r, 0, 255); d->g = clampf(g, 0, 255); d->b = clampf(b, 0, 255);
            if (texgen) {
                float nl = sqrtf(nx * nx + ny * ny + nz * nz);
                float ds = 0, dt = 0;
                if (nl > 0) {
                    ds = (nx * S.lookat_model[0][0] + ny * S.lookat_model[0][1] + nz * S.lookat_model[0][2]) / nl;
                    dt = (nx * S.lookat_model[1][0] + ny * S.lookat_model[1][1] + nz * S.lookat_model[1][2]) / nl;
                }
                if (S.geom & G_TEXTURE_GEN_LINEAR) {
                    ds = 1.0f - acosf(clampf(ds, -1, 1)) * (2.0f / 3.14159265f);
                    dt = 1.0f - acosf(clampf(dt, -1, 1)) * (2.0f / 3.14159265f);
                }
                d->s = (ds + 1.0f) * 512.0f * S.tex_ss;
                d->t = (dt + 1.0f) * 512.0f * S.tex_st;
            }
        } else {
            d->r = s->c[0]; d->g = s->c[1]; d->b = s->c[2];
        }
        d->a = s->c[3];
        if (S.geom & G_FOG) {
            float zw = d->w != 0.0f ? d->z / d->w : 0.0f;
            float f = (zw * (float)S.fog_mul + (float)S.fog_off) * (1.0f / 255.0f);
            d->fog = clampf(f, 0, 1);
            d->a = d->fog * 255.0f;        /* the RSP writes fog into shade alpha */
        } else {
            d->fog = 0;
        }
        d->clip = outcode(d);
    }
    S.stats.vtx += (u32)n;
}

/* ------------------------------------------------------------------ RDP: TMEM */
static void tmem_write(u32 addr, u8 v) { S.tmem[addr & 0xFFF] = v; }

/* 4 bytes to a 4-aligned TMEM address (wraps at 4 KB like tmem_write) */
static inline void tmem_write4(u32 addr, const u8 *src)
{
    u8 *d = &S.tmem[addr & 0xFFC];
    if (((uintptr_t)src & 3) == 0) {
        *(u32 *)(void *)d = *(const u32 *)(const void *)src;
    } else if (((uintptr_t)src & 1) == 0) {
        ((u16 *)(void *)d)[0] = ((const u16 *)(const void *)src)[0];
        ((u16 *)(void *)d)[1] = ((const u16 *)(const void *)src)[1];
    } else {
        d[0] = src[0]; d[1] = src[1]; d[2] = src[2]; d[3] = src[3];
    }
}

/* ---- lazy loads ------------------------------------------------------------
 * LOADBLOCK / LOADTILE do not copy into tmem[] at once: the load is recorded
 * (source pointer + layout) as a pending load. A texture whose TMEM bytes all
 * come from one recorded load gets its cache key from a hash of the load's
 * SOURCE bytes and the layout (one pass over the source, no copy); the copy into
 * tmem[] is made only when the texture has to be decoded (cache miss) or when
 * something else needs the real TMEM bytes. The decode itself always runs on
 * tmem[] exactly as before, so the decoded texels are unchanged.
 * Invariant: recorded loads never overlap each other (an overlapping new load
 * first copies the older one, unless it rewrites all of its bytes).
 * The source is read when the key is made / the copy is done, i.e. a little
 * later than the RDP would (within the same task); at the end of a task every
 * pending load is copied and forgotten. */
enum { PL_BLOCK = 1, PL_TILE = 2 };
typedef struct PLoad PLoad;

static void apply_block(const PLoad *p)
{
    u32 dst = p->lo, k, cnt = 0, b;
    const u8 *src = p->src;
    for (k = 0; k < p->words; k++) {
        u32 sw = ((cnt >> 11) & 1) ? 4 : 0;
        if ((dst & 3) == 0) {
            tmem_write4((dst + k * 8) ^ sw, src + k * 8);
            tmem_write4((dst + k * 8 + 4) ^ sw, src + k * 8 + 4);
        } else {
            for (b = 0; b < 8; b++)
                tmem_write((dst + k * 8 + b) ^ sw, src[k * 8 + b]);
        }
        cnt += p->dxt;
    }
}

static void apply_tile(const PLoad *p)
{
    u32 r, i, rowbytes = p->rowbytes;
    for (r = 0; r < p->rows; r++) {
        const u8 *src = p->src + r * p->stride;
        u32 dst = (u32)p->lo + r * p->line8;
        u32 sw = (r & 1) ? 4 : 0;
        i = 0;
        if ((dst & 7) == 0 && ((uintptr_t)src & 3) == 0 && (dst & 0xFFF) + rowbytes <= 4096) {
            /* fast path (photo strips): no wrap inside the row, aligned 8-byte units;
               odd rows have their two 32-bit halves swapped */
            u32 *d = (u32 *)(void *)&S.tmem[dst & 0xFFF];
            const u32 *q = (const u32 *)(const void *)src;
            u32 units = rowbytes >> 3, k;
            if (sw) {
                for (k = 0; k < units; k++, d += 2, q += 2) {
                    u32 a = q[0], b = q[1];
                    d[0] = b; d[1] = a;
                }
            } else {
                for (k = 0; k < units; k++, d += 2, q += 2) {
                    u32 a = q[0], b = q[1];
                    d[0] = a; d[1] = b;
                }
            }
            i = units << 3;
        } else if ((dst & 3) == 0) {
            for (; i + 4 <= rowbytes; i += 4)
                tmem_write4((dst + i) ^ sw, src + i);
        }
        for (; i < rowbytes; i++)
            tmem_write((dst + i) ^ sw, src[i]);
    }
}

static void pl_apply(PLoad *p)
{
    if (p->applied)
        return;
    if (p->kind == PL_BLOCK) apply_block(p);
    else apply_tile(p);
    p->applied = 1;
    S.stats.load_bytes += p->kind == PL_BLOCK ? p->words * 8 : p->rows * p->rowbytes;
}

static void pl_remove(int i)
{
    S.npl--;
    if (i < S.npl)
        memmove(&S.pl[i], &S.pl[i + 1], (size_t)(S.npl - i) * sizeof(PLoad));
}

/* copy every recorded load into tmem[] and forget them */
static void pl_flush(void)
{
    int i;
    for (i = 0; i < S.npl; i++)
        pl_apply(&S.pl[i]);
    S.npl = 0;
}

static int span_overlap(u32 alo, u32 alen, u32 blo, u32 blen)
{
    if (!alen || !blen) return 0;
    if (alen >= 4096 || blen >= 4096) return 1;
    return ((blo - alo) & 0xFFF) < alen || ((alo - blo) & 0xFFF) < blen;
}

/* a contains b */
static int span_contains(u32 alo, u32 alen, u32 blo, u32 blen)
{
    if (alen >= 4096) return 1;
    if (blen > alen) return 0;
    return ((blo - alo) & 0xFFF) + blen <= alen;
}

/* every byte of the span is written (LOADBLOCK; a LOADTILE of whole 8-byte rows) */
static int pl_solid(const PLoad *p)
{
    if (p->kind == PL_BLOCK) return 1;
    return (p->rowbytes & 7) == 0 && (p->rows == 1 || p->line8 == p->rowbytes);
}

static int pl_same_layout(const PLoad *a, const PLoad *b)
{
    return a->kind == b->kind && a->lo == b->lo && a->len == b->len &&
           (a->kind == PL_BLOCK ? a->words == b->words
                                : (a->rows == b->rows && a->rowbytes == b->rowbytes && a->line8 == b->line8));
}

static void pl_add(const PLoad *n)
{
    int i;
    for (i = 0; i < S.npl; ) {
        PLoad *p = &S.pl[i];
        if (span_overlap(p->lo, p->len, n->lo, n->len)) {
            if (!(pl_same_layout(p, n) || (pl_solid(n) && span_contains(n->lo, n->len, p->lo, p->len))))
                pl_apply(p);      /* partly overwritten: its other bytes stay in TMEM */
            pl_remove(i);
            continue;
        }
        i++;
    }
    if (S.npl == (int)(sizeof(S.pl) / sizeof(S.pl[0]))) {
        pl_apply(&S.pl[0]);
        pl_remove(0);
    }
    S.pl[S.npl++] = *n;
}

static void cmd_loadblock(u32 w0, u32 w1)
{
    F3DTile *t = &S.tile[(w1 >> 24) & 7];
    u32 uls = (w0 >> 12) & 0xFFF, ult = w0 & 0xFFF, lrs = (w1 >> 12) & 0xFFF, dxt = w1 & 0xFFF;
    u32 texels = lrs >= uls ? lrs - uls + 1 : 0;
    u32 bytes = (texels << S.timg_siz) >> 1;
    u32 words = (bytes + 7) >> 3;
    PLoad p;

    if (words > 512) words = 512;
    S.stats.loadblocks++;
    if (words) {
        memset(&p, 0, sizeof(p));
        p.kind = PL_BLOCK;
        p.src = (const u8 *)ptr(S.timg_addr + ((((ult * (u32)S.timg_width) + uls) << S.timg_siz) >> 1));
        p.lo = (u16)(t->tmem * 8);
        p.len = (u16)(words * 8);
        p.words = words;
        p.dxt = dxt;
        pl_add(&p);
    }
    t->uls = (u16)uls; t->ult = (u16)ult; t->lrs = (u16)lrs; t->lrt = (u16)dxt;
    S.tex_dirty = 1;
}

static void cmd_loadtile(u32 w0, u32 w1)
{
    F3DTile *t = &S.tile[(w1 >> 24) & 7];
    u32 uls = (w0 >> 12) & 0xFFF, ult = w0 & 0xFFF, lrs = (w1 >> 12) & 0xFFF, lrt = w1 & 0xFFF;
    u32 s0 = uls >> 2, t0 = ult >> 2, s1 = lrs >> 2, t1 = lrt >> 2;
    u32 texels = s1 >= s0 ? s1 - s0 + 1 : 0;
    u32 rowbytes = (texels << S.timg_siz) >> 1;
    u32 stride = ((u32)S.timg_width << S.timg_siz) >> 1;
    u32 rows = t1 >= t0 ? t1 - t0 + 1 : 0, span;
    PLoad p;

    if (rows > 1026) rows = 1026;       /* as the original loop: stops after row 1025 */
    S.stats.loadtiles++;
    if (rows && rowbytes) {
        memset(&p, 0, sizeof(p));
        p.kind = PL_TILE;
        p.src = (const u8 *)ptr(S.timg_addr) + t0 * stride + ((s0 << S.timg_siz) >> 1);
        p.stride = stride;
        p.rows = rows;
        p.rowbytes = rowbytes;
        p.line8 = (u32)t->line * 8;
        p.lo = (u16)(t->tmem * 8);
        span = (rows - 1) * p.line8 + ((rowbytes + 7) & ~7u);
        p.len = (u16)(span > 4096 ? 4096 : span);
        pl_add(&p);
    }
    t->uls = (u16)uls; t->ult = (u16)ult; t->lrs = (u16)lrs; t->lrt = (u16)lrt;
    S.tex_dirty = 1;
}

static void cmd_loadtlut(u32 w0, u32 w1)
{
    F3DTile *t = &S.tile[(w1 >> 24) & 7];
    u32 sl = (w0 >> 14) & 0x3FF, tl = (w0 >> 2) & 0x3FF, sh = (w1 >> 14) & 0x3FF, i;
    const u16 *src = (const u16 *)ptr(S.timg_addr + (tl * (u32)S.timg_width + sl) * 2);
    u32 base = (u32)t->tmem - 0x100;
    S.stats.loadtluts++;
    {
        u32 n = sh >= sl ? sh - sl + 1 : 0;
        if (n > 256) n = 256;
        base &= 0xFF;
        if (base + n <= 256 && ((uintptr_t)src & 1) == 0) {
            /* the same palette loaded again (every sprite of a kind loads its
               TLUT): the cached TLUT hashes stay valid */
            int same;
            if ((((uintptr_t)src | (uintptr_t)&S.tlut[base]) & 3) == 0 && (n & 1) == 0) {
                same = eq32(&S.tlut[base], src, n * 2);
            } else {
                same = 1;
                for (i = 0; i < n && same; i++)
                    same = S.tlut[base + i] == src[i];
            }
            if (same) {
                S.tex_dirty = 1;
                return;
            }
            memcpy(&S.tlut[base], src, n * 2);
        } else {
            for (i = 0; i < n; i++)
                S.tlut[(base + i) & 0xFF] = src[i];
        }
        S.tlut_valid = 0;
    }
    S.tex_dirty = 1;
}

/* fetch one texel of tile t at tile coordinates (x, y) as ARGB8888 */
static u32 expand5551(u16 c)
{
    u32 r = (c >> 11) & 31, g = (c >> 6) & 31, b = (c >> 1) & 31;
    r = (r << 3) | (r >> 2); g = (g << 3) | (g >> 2); b = (b << 3) | (b >> 2);
    return ((c & 1) ? 0xFF000000u : 0) | (r << 16) | (g << 8) | b;
}

static u32 ia16(u16 c) { u32 i = c >> 8; return ((u32)(c & 0xFF) << 24) | (i << 16) | (i << 8) | i; }
static u32 grey(u32 i, u32 a) { return (a << 24) | (i << 16) | (i << 8) | i; }

static u32 tlut_color(u32 idx)
{
    u16 c = S.tlut[idx & 0xFF];
    if (((S.omh >> 14) & 3) == 3)           /* IA16 TLUT */
        return ia16(c);
    return expand5551(c);
}

static u32 fetch(const F3DTile *t, int en_tlut, int x, int y)
{
    u32 base = (u32)t->tmem * 8 + (u32)y * t->line * 8;
    u32 sw = (y & 1) ? 4 : 0;
    u32 a;
    switch (t->siz) {
    case 0: {
        u8 b = S.tmem[((base + ((u32)x >> 1)) ^ sw) & 0xFFF];
        u32 n = (x & 1) ? (b & 0xF) : (b >> 4);
        if (en_tlut)
            return tlut_color(((u32)t->pal << 4) | n);
        switch (t->fmt) {
        case 3: { u32 i = (n >> 1) * 0x24 + ((n >> 1) >> 1); return grey(i > 255 ? 255 : i, (n & 1) ? 255 : 0); } /* IA4 */
        case 2: return grey((((u32)t->pal << 4) | n), 255);        /* CI4 without TLUT */
        default: return grey(n * 17, n * 17);                       /* I4 */
        }
    }
    case 1: {
        u8 b = S.tmem[((base + (u32)x) ^ sw) & 0xFFF];
        if (en_tlut)
            return tlut_color(b);
        if (t->fmt == 3) return grey((b >> 4) * 17, (b & 15) * 17);  /* IA8 */
        return grey(b, b);                                           /* I8 / CI8 without TLUT */
    }
    case 2: {
        u16 c;
        a = ((base + (u32)x * 2) ^ sw) & 0xFFE;
        memcpy(&c, &S.tmem[a], 2);
        if (en_tlut)
            return tlut_color(c >> 8);
        if (t->fmt == 3) return ia16(c);
        if (t->fmt == 4) return grey(c >> 8, c & 0xFF);
        return expand5551(c);
    }
    default: {                                                       /* 32b: RG low half, BA high half */
        u16 rg, ba;
        a = ((base + (u32)x * 2) ^ sw) & 0x7FE;
        memcpy(&rg, &S.tmem[a], 2);
        memcpy(&ba, &S.tmem[a + 0x800], 2);
        return ((u32)(ba & 0xFF) << 24) | ((u32)(rg >> 8) << 16) | ((u32)(rg & 0xFF) << 8) | (ba >> 8);
    }
    }
}

static void tile_dims(const F3DTile *t, int *w, int *h)
{
    int tw = (int)(((t->lrs - t->uls) & 0xFFF) >> 2) + 1;
    int th = (int)(((t->lrt - t->ult) & 0xFFF) >> 2) + 1;
    if (t->masks && !(t->cms & 2)) tw = 1 << (t->masks > 10 ? 10 : t->masks);
    else if (t->masks && (1 << t->masks) < tw) tw = 1 << t->masks;   /* clamp + mask: mask wins past it */
    if (t->maskt && !(t->cmt & 2)) th = 1 << (t->maskt > 10 ? 10 : t->maskt);
    else if (t->maskt && (1 << t->maskt) < th) th = 1 << t->maskt;
    if (tw < 1) tw = 1;
    if (th < 1) th = 1;
    if (tw > 1024) tw = 1024;
    if (th > 1024) th = 1024;
    *w = tw; *h = th;
}

int f3d_decode_tile(int ti, uint32_t *out, int max_texels, int *w, int *h)
{
    const F3DTile *t = &S.tile[ti & 7];
    int en_tlut = ((S.omh >> 14) & 3) >= 2, x, y;
    pl_flush();
    tile_dims(t, w, h);
    if (*w * *h > max_texels)
        return 0;
    for (y = 0; y < *h; y++)
        for (x = 0; x < *w; x++)
            out[y * *w + x] = fetch(t, en_tlut, x, y);
    return *w * *h;
}

/* Texture key hash over 32-bit words: two running sums (Fletcher style: h1 = sum,
 * h2 = sum of sums, so any change of a word or of the order changes the key) and a
 * multiply in the final mix. The 64-bit byte-wise FNV used first cost ~0.1 ms per
 * 4 KB on the SH4 (64-bit multiplies are software); the title screen hashes
 * ~500 KB of photo strips per frame. */
static u64 fnv(u64 h, const void *p, u32 n)
{
    const u8 *b = (const u8 *)p;
    u32 h1 = (u32)(h >> 32), h2 = (u32)h, w;

    S.stats.hash_bytes += n;

    if (((uintptr_t)b & 3) == 0) {
        const u32 *q = (const u32 *)(const void *)b;
        /* 32 bytes (one SH4 cache line) per iteration, the line 64 bytes ahead
           prefetched: the photo strips are hashed from RAM that is not in the
           cache every frame (prefetching past the end is harmless) */
        for (; n >= 32; n -= 32, q += 8) {
            F3D_PREFETCH(q + 16);
            h1 += q[0]; h2 += h1;
            h1 += q[1]; h2 += h1;
            h1 += q[2]; h2 += h1;
            h1 += q[3]; h2 += h1;
            h1 += q[4]; h2 += h1;
            h1 += q[5]; h2 += h1;
            h1 += q[6]; h2 += h1;
            h1 += q[7]; h2 += h1;
        }
        for (; n >= 16; n -= 16, q += 4) {
            h1 += q[0]; h2 += h1;
            h1 += q[1]; h2 += h1;
            h1 += q[2]; h2 += h1;
            h1 += q[3]; h2 += h1;
        }
        for (; n >= 4; n -= 4) {
            h1 += *q++; h2 += h1;
        }
        b = (const u8 *)q;
    } else {
        for (; n >= 4; n -= 4, b += 4) {
            memcpy(&w, b, 4);
            h1 += w; h2 += h1;
        }
    }
    while (n--) {
        h1 += *b++; h2 += h1;
    }
    h1 = (h1 ^ (h2 >> 13)) * 0x01000193u;
    h2 = (h2 ^ (h1 >> 11)) * 0x9E3779B1u;
    return ((u64)h1 << 32) | h2;
}

/* key over everything the decoded image depends on */
static u64 tex_key(const F3DTile *t, int w, int h, int en_tlut)
{
    u64 k = 0xCBF29CE484222325ull;
    u32 hdr[6], y;
    u32 rowbytes = (((u32)w << t->siz) >> 1) + 1;
    hdr[0] = t->fmt | (t->siz << 4) | (t->pal << 8) | ((u32)en_tlut << 12) | ((S.omh >> 14 & 3) << 16);
    hdr[1] = t->line; hdr[2] = t->tmem; hdr[3] = (u32)w; hdr[4] = (u32)h; hdr[5] = 0x46334421u;
    k = fnv(k, hdr, sizeof(hdr));
    if (t->siz == 3) rowbytes = (u32)w * 2;
    for (y = 0; y < (u32)h; y++) {
        u32 a = ((u32)t->tmem * 8 + y * t->line * 8) & 0xFFF;
        u32 n = rowbytes + 8;      /* + the swapped neighbours of odd rows */
        if (a + n <= 4096) {
            k = fnv(k, &S.tmem[a], n);
        } else {
            k = fnv(k, &S.tmem[a], 4096 - a);
            k = fnv(k, &S.tmem[0], n - (4096 - a));
        }
        if (t->line == 0) break;
    }
    if (t->siz == 3)
        k = fnv(k, &S.tmem[0x800], 0x800);
    if (en_tlut) {
        if (t->siz == 0) k = fnv(k, &S.tlut[(t->pal << 4) & 0xFF], 32);
        else k = fnv(k, S.tlut, sizeof(S.tlut));
    }
    return k;
}

/* ---- keys from recorded loads ---- */
static u64 tlut_hash(int siz, int pal)
{
    if (siz == 0) {
        int b = pal & 15;
        if (!(S.tlut_valid & (1u << b))) {
            S.tlut_hash_bank[b] = fnv(0x9E3779B97F4A7C15ull + (u64)b, &S.tlut[b << 4], 32);
            S.tlut_valid |= 1u << b;
        }
        return S.tlut_hash_bank[b];
    }
    if (!(S.tlut_valid & 0x10000u)) {
        S.tlut_hash_all = fnv(0xC2B2AE3D27D4EB4Full, S.tlut, sizeof(S.tlut));
        S.tlut_valid |= 0x10000u;
    }
    return S.tlut_hash_all;
}

/* source hashes of the loads of this task, by (source, size): a texture loaded
 * again (same sprite drawn several times) is hashed once per task */
#define SRC_MEMO 64
static struct { const u8 *src; u32 a, b, c; u64 h; } src_memo[SRC_MEMO];
static u32 src_memo_gen[SRC_MEMO], src_memo_cur = 1;

/* Hash of the load's source bytes. For a tile load, rows before r0 can be left
 * out when the decode reads only rows r0 and later: a row's TMEM bytes are
 * written by that row and can only be overwritten by later rows (wrap), never by
 * earlier ones. */
static u64 pl_source_hash(PLoad *p, u32 r0)
{
    u32 a, b, c, slot;
    u64 h;
    if (p->kind == PL_BLOCK || r0 >= p->rows) r0 = 0;
    if (p->hashed && p->hash_r0 == r0)
        return p->hash;
    if (p->kind == PL_BLOCK) { a = p->words * 8; b = 0; c = 1; }
    else { a = p->rowbytes; b = p->stride; c = p->rows; }
    slot = (u32)(((uintptr_t)p->src >> 3) ^ a ^ (c << 7) ^ (r0 << 3)) & (SRC_MEMO - 1);
    if (src_memo_gen[slot] == src_memo_cur && src_memo[slot].src == p->src && src_memo[slot].a == a &&
        src_memo[slot].b == b && src_memo[slot].c == (c | (r0 << 16))) {
        h = src_memo[slot].h;
    } else {
        u32 r;
        h = 0x6A09E667F3BCC908ull + r0;
        if (p->kind == PL_BLOCK) {
            h = fnv(h, p->src, a);
        } else {
            for (r = r0; r < c; r++) {
                F3D_PREFETCH(p->src + (r + 1) * b);          /* next row's first line */
                F3D_PREFETCH(p->src + (r + 1) * b + 32);
                h = fnv(h, p->src + r * b, a);
            }
        }
        src_memo[slot].src = p->src; src_memo[slot].a = a; src_memo[slot].b = b; src_memo[slot].c = c | (r0 << 16);
        src_memo[slot].h = h;
        src_memo_gen[slot] = src_memo_cur;
    }
    p->hash = h;
    p->hash_r0 = r0;
    p->hashed = 1;
    return h;
}

/* The recorded load that wrote every TMEM byte the decode of tile t (w x h)
 * reads, or NULL. Decode reads, for row y, bytes [0, rowread) of the row at
 * tmem*8 + y*line*8 (odd rows: address ^ 4, same 8-byte unit). */
static PLoad *pl_source_of(const F3DTile *t, int w, int h)
{
    u32 rowread = ((u32)w << t->siz) >> 1, lo = (u32)t->tmem * 8, len;
    int i;
    if (t->siz == 3 || S.npl == 0)
        return NULL;
    if (t->siz == 0) rowread = ((u32)w + 1) >> 1;
    if (rowread == 0) rowread = 1;
    len = (t->line ? (u32)(h - 1) * t->line * 8 : 0) + ((rowread + 7) & ~7u);
    for (i = 0; i < S.npl; i++) {
        PLoad *p = &S.pl[i];
        if (p->kind == PL_TILE) {
            if (p->lo == lo && p->line8 == (u32)t->line * 8 && (u32)h <= p->rows && rowread <= p->rowbytes &&
                (t->line != 0 || h == 1))
                return p;
        } else if (len <= 4096 && span_contains(p->lo, p->len, lo, len)) {
            return p;
        }
    }
    return NULL;
}

static u64 pl_key(PLoad *p, const F3DTile *t, int w, int h, int en_tlut, int r0)
{
    u32 hdr[12];
    u64 sh = pl_source_hash(p, (u32)r0), k;
    hdr[0] = 0x4C4F4144u;                       /* key family: recorded load */
    hdr[1] = t->fmt | (t->siz << 4) | (t->pal << 8) | ((u32)en_tlut << 12) | ((S.omh >> 14 & 3) << 16) |
             ((u32)p->kind << 20);
    hdr[2] = t->line; hdr[3] = t->tmem; hdr[4] = (u32)w; hdr[5] = (u32)h;
    hdr[6] = p->lo; hdr[7] = p->kind == PL_BLOCK ? p->words : p->rows;
    hdr[8] = p->kind == PL_BLOCK ? p->dxt : p->rowbytes;
    hdr[9] = p->line8;
    hdr[10] = (u32)sh; hdr[11] = (u32)(sh >> 32);
    k = fnv(0x243F6A8885A308D3ull, hdr, sizeof(hdr));
    if (en_tlut) {
        u64 th = tlut_hash(t->siz, t->pal);
        k ^= th * 0x9E3779B97F4A7C15ull + (k << 6) + (k >> 2);
    }
    return k;
}

/* Can the texels (ox.., oy..) w x h of tile t be read straight from the source of
 * tile load p? True when the tile addresses the load's rows 1:1 (same TMEM start
 * and line, 8-aligned rows) and no later row of the load wrapped round the 4 KB
 * TMEM onto the bytes read from rows oy..oy+h-1. */
static int pl_direct_ok(const PLoad *p, const F3DTile *t, int ox, int oy, int w, int h)
{
    u32 i, j, need, r1;
    if (p->kind != PL_TILE || p->lo != (u32)t->tmem * 8 || p->line8 != (u32)t->line * 8 || (p->line8 & 7) ||
        (p->lo & 7) || t->line == 0)
        return 0;
    need = (u32)(ox + w) * 2;                /* bytes of each row the window reads */
    r1 = (u32)(oy + h);
    if (need > p->rowbytes || r1 > p->rows || ((uintptr_t)p->src & 1) || (p->stride & 1))
        return 0;
    if (p->line8 >= ((p->rowbytes + 7) & ~7u) &&
        (p->lo & 0xFFF) + (p->rows - 1) * p->line8 + ((p->rowbytes + 7) & ~7u) <= 4096)
        return 1;                            /* nothing wraps */
    if (p->rows > 32)
        return 0;
    for (i = (u32)oy; i < r1; i++)
        for (j = i + 1; j < p->rows; j++)
            if (span_overlap((p->lo + j * p->line8) & 0xFFF, (p->rowbytes + 7) & ~7u,
                             (p->lo + i * p->line8) & 0xFFF, (need + 7) & ~7u))
                return 0;
    return 1;
}

/* ---- strips of a wide source image (the 640x240 photo) ----
 * A photo strip's texel window moves with the scroll position, so as a texture
 * of its own (crop) every strip changes whenever the photo scrolls sideways: all
 * 40 strips were converted and uploaded every frame of a horizontal pan (vertical
 * pans reuse the strips of 6 rows earlier). For a source image wider than
 * WIDE_MIN_STRIDE bytes the window is instead taken from a texture whose left
 * edge sits at a fixed, 64-byte aligned position of the SOURCE memory (O below):
 * it contains the window plus up to 32 texels of the real image on either side.
 * While the window slides by less than that, the texture content (and so its
 * key) does not change and the cache hits; the u offset follows the window.
 * The alignment phase depends on the row address (row index mod 8), so the 40
 * strips cross their alignment boundary in 8 different frames, not all at once.
 * Texels drawn are the same as with the crop texture (rows: the same window and
 * edge padding; columns: the same texels, the PVR's bilinear neighbour at the
 * window's left / right end is the real neighbouring texel of the image instead
 * of a replicated edge texel). Narrow images (320-wide photos, sprites) never take
 * this path, so their output is unchanged. */
#define WIDE_MIN_STRIDE 1024     /* bytes: 512+ RGBA16 texels per source row */
#define WIDE_ALIGN 64            /* bytes */
#define WIDE_ALIGN_TX (WIDE_ALIGN / 2)

static void *wide_texture(PLoad *p, const F3DTile *t, int ox, int oy, int w, int h, u32 clampf)
{
    uintptr_t a = (uintptr_t)p->src + (uintptr_t)ox * 2;          /* window start, load row 0 */
    u32 ph = ((u32)((uintptr_t)p->src / p->stride) & 7) * (WIDE_ALIGN / 8);
    uintptr_t o = ((a - 2 + ph) & ~(uintptr_t)(WIDE_ALIGN - 1)) - ph;
    int tx0 = (int)((a - o) >> 1);                               /* 1 .. WIDE_ALIGN_TX */
    int tw = (w + WIDE_ALIGN_TX + 1 + WIDE_ALIGN_TX * 2 - 1) & ~(WIDE_ALIGN_TX * 2 - 1);
    const u8 *base = (const u8 *)o + (u32)oy * p->stride;
    u32 hdr[8], and1 = 1, y;
    u64 key;
    void *hdl;
    if (tw > 512 || (o & 3))
        return NULL;
    {
        u64 rows = 0x3C6EF372FE94F82Bull;
        for (y = 0; y < (u32)h; y++) {
            F3D_PREFETCH(base + (y + 1) * p->stride);
            F3D_PREFETCH(base + (y + 1) * p->stride + 32);
            rows = fnv(rows, base + y * p->stride, (u32)tw * 2);
        }
        /* opaque or 1-bit alpha: decided by the window texels only (as the crop
           texture), and part of the key */
        for (y = 0; y < (u32)h; y++) {
            const u16 *r = (const u16 *)(const void *)(base + y * p->stride) + tx0;

            int x = 0;
            if (((uintptr_t)r & 2) != 0 && w > 0) { and1 &= r[0]; x = 1; }
            {
                const u32 *q = (const u32 *)(const void *)(r + x);
                u32 acc = 0xFFFFFFFFu;
                for (; x + 8 <= w; x += 8, q += 4)
                    acc &= q[0] & q[1] & q[2] & q[3];
                for (; x + 2 <= w; x += 2, q++)
                    acc &= *q;
                and1 &= acc & (acc >> 16);
            }
            for (; x < w; x++)
                and1 &= r[x];
        }
        hdr[0] = 0x57494445u;                                   /* key family: wide strip */
        hdr[1] = (u32)t->fmt | ((u32)t->siz << 4) | ((S.omh >> 14 & 3) << 16) | ((and1 & 1) << 20);
        hdr[2] = (u32)tw; hdr[3] = (u32)h; hdr[4] = clampf; hdr[5] = p->stride;
        hdr[6] = (u32)rows; hdr[7] = (u32)(rows >> 32);
        key = fnv(0x3C6EF372FE94F82Bull, hdr, sizeof(hdr));
    }
    hdl = S.be->tex_find(S.be->ctx, key);
    if (!hdl && S.be->tex_upload) {
        F3DTexImage img;
        memset(&img, 0, sizeof(img));
        img.key = key; img.width = (u16)tw; img.height = (u16)h;
        img.fmt = t->fmt; img.siz = t->siz;
        img.alpha = (and1 & 1) ? 0 : 1;
        img.pad = (u8)clampf;
        img.rgba16 = (const u16 *)(const void *)base;
        img.stride = p->stride >> 1;
        hdl = S.be->tex_upload(S.be->ctx, &img);
        S.stats.tex_decodes++;
        S.stats.tex_direct++;
    }
    if (!hdl)
        return NULL;
    S.tex_dirty = 0;
    S.tex_key_tile = (int)(t - S.tile);
    S.tex_handle = hdl;
    S.tex_key = key;
    S.tex_w = (u16)tw; S.tex_h = (u16)h;
    S.tex_crop_sig = 0xFFFFFFFFu;            /* never matches: recomputed per rectangle */
    S.tex_ox = ox - tx0; S.tex_oy = oy;
    return hdl;
}

static void *get_texture(int ti, u16 *w_out, u16 *h_out)
{
    const F3DTile *t = &S.tile[ti & 7];
    int en_tlut = ((S.omh >> 14) & 3) >= 2, w, h;
    u64 key;
    void *hdl;
    PLoad *src;

    int ox = 0, oy = 0, fw, fh;
    u32 sig = 0, clampf;

    if (S.crop_on)
        sig = 0x80000000u | ((u32)S.crop_x0 << 20) | ((u32)S.crop_y0 << 10) | 0;
    if (!S.tex_dirty && S.tex_key_tile == ti && S.tex_handle && S.tex_crop_sig == sig &&
        (!S.crop_on || ((u32)S.tex_w == (u32)S.crop_w && (u32)S.tex_h == (u32)S.crop_h))) {
        *w_out = S.tex_w; *h_out = S.tex_h;
        return S.tex_handle;
    }
    tile_dims(t, &w, &h);
    if (w * h > (int)(sizeof(tex_buf) / 4)) {
        if (w > 256) w = 256;
        if (h > 256) h = 256;
    }
    fw = w; fh = h;                       /* extent of the tile that is read */
    if (S.crop_on) {
        /* only the texels the RDP fetches for this rectangle (see texrect) */
        ox = S.crop_x0 < w ? S.crop_x0 : w - 1;
        oy = S.crop_y0 < h ? S.crop_y0 : h - 1;
        w = S.crop_w < w - ox ? S.crop_w : w - ox;
        h = S.crop_h < h - oy ? S.crop_h : h - oy;
        if (w < 1) w = 1;
        if (h < 1) h = 1;
        fw = ox + w; fh = oy + h;
    }
    clampf = (((t->cms & 2) || !t->masks) ? 1u : 0u) | (((t->cmt & 2) || !t->maskt) ? 2u : 0u);
    S.stats.tex_lookups++;
    src = pl_source_of(t, fw, fh);
    if (src && S.crop_on && S.be && (S.be->caps & F3D_CAP_RGBA16) && t->siz == 2 && !en_tlut && t->fmt != 3 &&
        t->fmt != 4 && src->stride >= WIDE_MIN_STRIDE && w <= 512 - 2 * WIDE_ALIGN_TX - 2 && pl_direct_ok(src, t, ox, oy, w, h)) {
        hdl = wide_texture(src, t, ox, oy, w, h, clampf);
        if (hdl) {
            *w_out = S.tex_w; *h_out = S.tex_h;
            return hdl;
        }
    }
    if (src) {
        key = pl_key(src, t, fw, fh, en_tlut, oy);
    } else {
        pl_flush();
        key = tex_key(t, fw, fh, en_tlut);
    }
    if (ox | oy | (fw != w) | (fh != h) | (clampf << 1)) {
        u32 extra[4] = { (u32)ox, (u32)oy, ((u32)w << 16) | (u32)h, clampf | 0x43520000u };
        key = fnv(key, extra, sizeof(extra));
    }
    hdl = S.be && S.be->tex_find ? S.be->tex_find(S.be->ctx, key) : NULL;
    if (!hdl && S.be && S.be->tex_upload) {
        F3DTexImage img;
        int x, y, any0 = 0, part = 0;
        img.rgba16 = NULL;
        img.stride = (u32)w;
        if (src && t->siz == 2 && !en_tlut && t->fmt != 3 && t->fmt != 4 && (S.be->caps & F3D_CAP_RGBA16) &&
            pl_direct_ok(src, t, ox, oy, w, h)) {
            /* RGBA16 window of a tile load whose rows are intact in TMEM (photo
               strips): the texels are the source bytes themselves (TMEM odd-row
               swaps cancel out in the fetch), so the backend reads the source
               directly: no TMEM copy, no extraction. The load stays pending. */
            const u16 *s0 = (const u16 *)(const void *)(src->src + (u32)oy * src->stride + (u32)ox * 2);
            u32 pitch = src->stride >> 1, and1 = 1;
            for (y = 0; y < h; y++) {
                const u16 *r = s0 + (u32)y * pitch;
                x = 0;
                if (((uintptr_t)r & 3) == 0) {
                    const u32 *q = (const u32 *)(const void *)r;
                    u32 acc = 0xFFFFFFFFu;
                    for (; x + 8 <= w; x += 8, q += 4)
                        acc &= q[0] & q[1] & q[2] & q[3];
                    and1 &= acc & (acc >> 16);
                }
                for (; x < w; x++)
                    and1 &= r[x];
                if (!(and1 & 1)) break;
            }
            any0 = !(and1 & 1);
            img.rgba16 = s0;
            img.stride = pitch;
            S.stats.tex_direct++;
        } else if (src) {
            /* the decode reads only bytes of this load; the others stay pending */
            pl_apply(src);
        }
        if (img.rgba16) {
            /* done above */
        } else if (t->siz == 2 && !en_tlut && t->fmt != 3 && t->fmt != 4 && (S.be->caps & F3D_CAP_RGBA16)) {
            /* RGBA16 for a backend that converts it itself: the raw texels as
               fetch() reads them (odd rows: 32-bit halves swapped back) */
            u16 *o16 = (u16 *)(void *)tex_buf;
            u32 and1 = 1;
            for (y = 0; y < h; y++) {
                u32 ty = (u32)(y + oy);
                u32 base = ((u32)t->tmem * 8 + ty * t->line * 8) & 0xFFF, sw = (ty & 1) ? 4 : 0;
                u16 *o = o16 + y * w;
                x = 0;
                base += (u32)ox * 2;             /* window start; units only when 8-aligned */
                if ((ox & 3) == 0 && base + (u32)w * 2 <= 4096) {
                    /* whole 8-byte units without wrap */
                    const u32 *q = (const u32 *)(const void *)&S.tmem[base];
                    u32 acc = 0xFFFFFFFFu;
                    for (; x + 4 <= w; x += 4, q += 2) {
                        u32 a = sw ? q[1] : q[0], b = sw ? q[0] : q[1];
                        acc &= a & b;
                        memcpy(&o[x], &a, 4);
                        memcpy(&o[x + 2], &b, 4);
                    }
                    and1 &= acc & (acc >> 16);
                }
                for (; x < w; x++) {
                    u16 c = *(const u16 *)(const void *)&S.tmem[((base + (u32)x * 2) ^ sw) & 0xFFE];
                    and1 &= c;
                    o[x] = c;
                }
                (void)0;
            }
            any0 = !(and1 & 1);
            img.rgba16 = o16;
        } else if (t->siz == 2 && !en_tlut && t->fmt != 3 && t->fmt != 4) {
            /* RGBA16 fast path (photo strips, re-decoded whenever the photo scrolls):
               same texels as fetch(), without the per-texel format switch */
            u32 and1 = 1;
            for (y = 0; y < h; y++) {
                u32 ty = (u32)(y + oy);
                u32 base = (u32)t->tmem * 8 + ty * t->line * 8 + (u32)ox * 2, sw = (ty & 1) ? 4 : 0;
                u32 *o = tex_buf + y * w;
                for (x = 0; x < w; x++) {
                    u16 c = *(const u16 *)(const void *)&S.tmem[((base + (u32)x * 2) ^ sw) & 0xFFE];
                    and1 &= c;
                    o[x] = expand5551(c);
                }
            }
            any0 = !(and1 & 1);
        } else {
            for (y = 0; y < h; y++)
                for (x = 0; x < w; x++) {
                    u32 c = fetch(t, en_tlut, x + ox, y + oy), a = c >> 24;
                    tex_buf[y * w + x] = c;
                    if (a == 0) any0 = 1;
                    else if (a != 255) part = 1;
                }
        }
        img.key = key; img.width = (u16)w; img.height = (u16)h;
        img.fmt = t->fmt; img.siz = t->siz;
        img.alpha = part ? 2 : (any0 ? 1 : 0);
        img.pad = (u8)clampf;             /* bit 0: s clamps, bit 1: t clamps */
        img.argb = img.rgba16 ? NULL : tex_buf;
        hdl = S.be->tex_upload(S.be->ctx, &img);
        S.stats.tex_decodes++;
    }
    S.tex_dirty = 0;
    S.tex_key_tile = ti;
    S.tex_handle = hdl;
    S.tex_key = key;
    S.tex_w = (u16)w; S.tex_h = (u16)h;
    S.tex_crop_sig = sig;
    S.tex_ox = ox; S.tex_oy = oy;
    *w_out = (u16)w; *h_out = (u16)h;
    return hdl;
}

/* ------------------------------------------------------------------ combiner */
/* 1-cycle mode uses the second cycle's combiner inputs (games set both the same). */
typedef struct { u8 a, b, c, d, Aa, Ab, Ac, Ad; } CC;

static CC decode_cc(int cycle)
{
    CC r;
    u32 h = S.cc_h, l = S.cc_l;
    if (cycle == 0) {
        r.a = (h >> 20) & 0xF; r.c = (h >> 15) & 0x1F; r.Aa = (h >> 12) & 7; r.Ac = (h >> 9) & 7;
        r.b = (l >> 28) & 0xF; r.d = (l >> 15) & 7; r.Ab = (l >> 12) & 7; r.Ad = (l >> 9) & 7;
    } else {
        r.a = (h >> 5) & 0xF; r.c = h & 0x1F; r.b = (l >> 24) & 0xF; r.Aa = (l >> 21) & 7; r.Ac = (l >> 18) & 7;
        r.d = (l >> 6) & 7; r.Ab = (l >> 3) & 7; r.Ad = l & 7;
    }
    return r;
}

/* colour inputs (channel ch 0..2), alpha ch 3 */
static Lin in_rgb(int sel, int slot, int ch, const float *shade)
{
    Lin v = { 0, 0 };
    const float k = 1.0f / 255.0f;
    switch (sel) {
    case 1: case 2: v.t = 1; break;                               /* TEXEL0/1 */
    case 3: v.c = S.prim[ch] * k; break;
    case 4: v.c = shade[ch] * k; break;
    case 5: v.c = S.env[ch] * k; break;
    case 6:
        if (slot == 0 || slot == 3) v.c = 1;                      /* 1 */
        else if (slot == 1) v.c = 0;                              /* CENTER */
        else v.c = 0;                                             /* SCALE */
        break;
    case 7:
        if (slot == 0) v.c = 0;                                   /* NOISE */
        else if (slot == 2) v.c = 0;                              /* COMBINED_ALPHA */
        break;
    default:
        if (slot == 2) {
            switch (sel) {
            case 8: case 9: v.t = 1; break;                       /* TEXEL0/1_ALPHA (approx.) */
            case 10: v.c = S.prim[3] * k; break;
            case 11: v.c = shade[3] * k; break;
            case 12: v.c = S.env[3] * k; break;
            case 14: v.c = S.prim_lod_frac * k; break;
            default: break;                                       /* LOD_FRACTION, K5, 0 */
            }
        }
        break;
    }
    if (slot == 3 && sel == 7) v.c = 0;                           /* d: 7 = 0 */
    return v;
}

static Lin in_a(int sel, int slot, const float *shade)
{
    Lin v = { 0, 0 };
    const float k = 1.0f / 255.0f;
    switch (sel) {
    case 1: case 2: v.t = 1; break;
    case 3: v.c = S.prim[3] * k; break;
    case 4: v.c = shade[3] * k; break;
    case 5: v.c = S.env[3] * k; break;
    case 6: v.c = (slot == 2) ? S.prim_lod_frac * k : 1; break;  /* c: PRIM_LOD_FRAC, else 1 */
    case 0: v.c = 0; break;                                       /* COMBINED / LOD_FRACTION */
    default: break;
    }
    return v;
}

static Lin combine(Lin a, Lin b, Lin c, Lin d)
{
    Lin r, df;
    df.t = a.t - b.t; df.c = a.c - b.c;
    /* (dt T + dc)(ct T + cc) ; T*T approximated by T */
    r.t = df.t * c.t + df.t * c.c + df.c * c.t + d.t;
    r.c = df.c * c.c + d.c;
    return r;
}

static int cc_uses_texel(const CC *cc)
{
    return cc->a == 1 || cc->a == 2 || cc->b == 1 || cc->b == 2 || cc->c == 1 || cc->c == 2 ||
           cc->c == 8 || cc->c == 9 || cc->d == 1 || cc->d == 2 || cc->Aa == 1 || cc->Aa == 2 ||
           cc->Ab == 1 || cc->Ab == 2 || cc->Ac == 1 || cc->Ac == 2 || cc->Ad == 1 || cc->Ad == 2;
}

/* Combiner reduction cache: per (combiner, prim, env, cycle) the inputs that do
 * not depend on the vertex shade are evaluated once (with the same in_rgb/in_a
 * code), channels without any shade input are combined once; per vertex only
 * the shade inputs are filled in and combine() runs on the same values as the
 * per-vertex reduction did before, so the results are bit-identical. */
static struct {
    int valid;
    u32 gen;                /* bumped whenever the reduction changes */
    u32 cc_h, cc_l, prim, env;
    u8 lod, sel;
    int uses_texel;
    Lin k[4][4];            /* [channel 0-2 rgb, 3 alpha][slot a b c d] */
    s8 sh[4][4];            /* shade component of that input, -1 = constant */
    u8 is_const[4];
    float cmod[4], coff[4];
    u8 ctex[4];
} CCC;

static void cc_prepare(int sel)
{
    u32 prim = ((u32)S.prim[0] << 24) | ((u32)S.prim[1] << 16) | ((u32)S.prim[2] << 8) | S.prim[3];
    u32 env = ((u32)S.env[0] << 24) | ((u32)S.env[1] << 16) | ((u32)S.env[2] << 8) | S.env[3];
    static const float zero4[4] = { 0, 0, 0, 0 };
    CC cc;
    int ch, sl;
    if (CCC.valid && CCC.cc_h == S.cc_h && CCC.cc_l == S.cc_l && CCC.prim == prim && CCC.env == env &&
        CCC.lod == S.prim_lod_frac && CCC.sel == sel)
        return;
    CCC.valid = 1;
    CCC.gen++;
    CCC.cc_h = S.cc_h; CCC.cc_l = S.cc_l; CCC.prim = prim; CCC.env = env;
    CCC.lod = S.prim_lod_frac; CCC.sel = (u8)sel;
    cc = decode_cc(sel);
    CCC.uses_texel = cc_uses_texel(&cc);
    for (ch = 0; ch < 4; ch++) {
        const u8 rs[4] = { cc.a, cc.b, cc.c, cc.d }, as[4] = { cc.Aa, cc.Ab, cc.Ac, cc.Ad };
        int c = 1;
        for (sl = 0; sl < 4; sl++) {
            int v = ch < 3 ? rs[sl] : as[sl];
            s8 sh = -1;
            if (ch < 3) {
                if (v == 4) sh = (s8)ch;
                else if (v == 11 && sl == 2) sh = 3;
                else CCC.k[ch][sl] = in_rgb(v, sl, ch, zero4);
            } else {
                if (v == 4) sh = 3;
                else CCC.k[ch][sl] = in_a(v, sl, zero4);
            }
            CCC.sh[ch][sl] = sh;
            if (sh >= 0) c = 0;
        }
        CCC.is_const[ch] = (u8)c;
        if (c) {
            Lin r = combine(CCC.k[ch][0], CCC.k[ch][1], CCC.k[ch][2], CCC.k[ch][3]);
            CCC.cmod[ch] = r.t; CCC.coff[ch] = r.c;
            CCC.ctex[ch] = r.t != 0.0f;
        }
    }
}

static inline void cc_eval(const float *shade, float mod[4], float off[4], int *tex_rgb, int *tex_a)
{
    int ch, sl;
    for (ch = 0; ch < 4; ch++) {
        int tx;
        if (CCC.is_const[ch]) {
            mod[ch] = CCC.cmod[ch]; off[ch] = CCC.coff[ch];
            tx = CCC.ctex[ch];
        } else {
            Lin v[4], r;
            for (sl = 0; sl < 4; sl++) {
                int sh = CCC.sh[ch][sl];
                if (sh < 0) v[sl] = CCC.k[ch][sl];
                else { v[sl].t = 0; v[sl].c = shade[sh] * (1.0f / 255.0f); }
            }
            r = combine(v[0], v[1], v[2], v[3]);
            mod[ch] = r.t; off[ch] = r.c;
            tx = r.t != 0.0f;
        }
        if (tx) {
            if (ch < 3) *tex_rgb = 1;
            else *tex_a = 1;
        }
    }
}

/* last vertex colour: inputs and the 8 colour bytes (r g b a or og ob oa) */
static struct { int valid; u32 gen; u32 key[6]; u8 col[8]; u8 tx_rgb, tx_a; } VM;

/* ------------------------------------------------------------------ draw state */
static int blender_is_fog(void)
{
    /* cycle-0 blender P = FOG, A = SHADE_A (G_RM_FOG_SHADE_A) */
    return ((S.oml >> 30) & 3) == 3 && ((S.oml >> 26) & 3) == 2;
}

static void build_state(F3DState *st, int rect, int textured)
{
    u32 l = S.oml;
    u32 cyc = (S.omh >> 20) & 3;
    u32 p, a, m, b;
    memset(st, 0, sizeof(*st));
    st->cycle = (u8)cyc;
    st->rect = (u8)rect;
    st->othermode_h = S.omh; st->othermode_l = S.oml;
    st->combine_h = S.cc_h; st->combine_l = S.cc_l;
    st->filter = ((S.omh >> 12) & 3) == 0 ? F3D_FILTER_POINT : F3D_FILTER_BILINEAR;
    if (cyc == F3D_CYCLE_COPY) st->filter = F3D_FILTER_POINT;

    /* blender, cycle 0 (the one used in 1-cycle mode); a fog first cycle in
     * 1-cycle mode is folded into the vertex colours, the rest uses cycle 1 */
    if (blender_is_fog() && cyc == F3D_CYCLE_2) {
        p = (l >> 28) & 3; a = (l >> 24) & 3; m = (l >> 20) & 3; b = (l >> 16) & 3;
    } else if (blender_is_fog()) {
        p = 0; a = 0; m = 0; b = 0;     /* IN*IN_A + IN*(1-A) after the fog fold */
    } else {
        p = (l >> 30) & 3; a = (l >> 26) & 3; m = (l >> 22) & 3; b = (l >> 18) & 3;
    }
    (void)p; (void)a;
    st->blend = F3D_BLEND_OPAQUE;
    if (cyc < 2 && (l & FORCE_BL) && m == 1) {
        if (b == 0) st->blend = F3D_BLEND_ALPHA;
        else if (b == 2) st->blend = F3D_BLEND_ADD;
    }
    if (cyc < 2 && (l & FORCE_BL) && m == 1 && b == 2 && a == 3)
        st->blend = F3D_BLEND_NOWRITE;

    /* alpha test: threshold compare against the blend colour alpha; coverage x
     * alpha drops pixels whose alpha is 0 */
    if (cyc < 2) {
        if ((l & 3) == 1) st->alpha_ref = S.blend[3] ? S.blend[3] : 1;
        else if ((l & 3) == 3) st->alpha_ref = 1;
        if ((l & CVG_X_ALPHA) && st->blend == F3D_BLEND_OPAQUE && st->alpha_ref == 0)
            st->alpha_ref = 1;
    } else if (cyc == F3D_CYCLE_COPY && (l & 3)) {
        st->alpha_ref = 1;
    }

    if (cyc < 2) {
        int zbuf = rect ? (l & ZSRC_PRIM) != 0 : (S.geom & G_ZBUFFER) != 0;
        st->z_cmp = (u8)(zbuf && (l & Z_CMP));
        st->z_upd = (u8)(zbuf && (l & Z_UPD));
        st->z_decal = (u8)(((l >> ZMODE_SHIFT) & 3) == 3);
    }
    if (textured >= 0) {
        const F3DTile *t = &S.tile[textured & 7];
        st->tex = get_texture(textured & 7, &st->tex_w, &st->tex_h);
        st->wrap_s = (t->cms & 2) || !t->masks ? F3D_WRAP_CLAMP : ((t->cms & 1) ? F3D_WRAP_MIRROR : F3D_WRAP_REPEAT);
        st->wrap_t = (t->cmt & 2) || !t->maskt ? F3D_WRAP_CLAMP : ((t->cmt & 1) ? F3D_WRAP_MIRROR : F3D_WRAP_REPEAT);
        /* a tile without mask and without clamp still clamps at its size on
         * hardware only if CLAMP is set; without either it wraps at 1024 -
         * treat as clamp (no game texture relies on it) */
    }
}

static const F3DState *commit_state(F3DState *st)
{
    st->serial = 0;
    if (!eq32(st, &S.st_last, sizeof(*st))) {
        S.st_last = *st;
        S.serial++;
    }
    S.st = S.st_last;
    S.st.serial = S.serial;
    return &S.st;
}

/* ------------------------------------------------------------------ triangles */
typedef struct {
    float x, y, z, w, s, t, c[4], fog;
} CV;   /* clip-space vertex used while clipping */

#define MAX_POLY 16

static inline float plane_dist(const CV *v, int p)
{
    /* clip-space planes: near, far, and the 4 scissor edges expressed in
     * homogeneous coordinates (screen = vtrans + ndc*vscale) */
    switch (p) {
    case 0: return v->z + v->w;                                          /* near */
    case 1: return v->w - v->z;                                          /* far */
    case 2: return v->x * S.vscale[0] + v->w * (S.vtrans[0] - S.sc_x0);  /* sx >= x0 */
    case 3: return -v->x * S.vscale[0] + v->w * (S.sc_x1 - S.vtrans[0]); /* sx <= x1 */
    case 4: return v->y * S.ysign * S.vscale[1] + v->w * (S.vtrans[1] - S.sc_y0);
    default: return -v->y * S.ysign * S.vscale[1] + v->w * (S.sc_y1 - S.vtrans[1]);
    }
}

/* no vertex outside any plane: clip_poly() would return the polygon unchanged */
static int all_inside(const CV *v, int n)
{
    int i;
    for (i = 0; i < n; i++)
        if (plane_dist(&v[i], 0) < 0 || plane_dist(&v[i], 1) < 0 || plane_dist(&v[i], 2) < 0 ||
            plane_dist(&v[i], 3) < 0 || plane_dist(&v[i], 4) < 0 || plane_dist(&v[i], 5) < 0)
            return 0;
    return 1;
}

static int clip_poly(CV *in, int n, CV *tmp)
{
    int p, i;
    CV *src = in, *dst = tmp;
    for (p = 0; p < 6; p++) {
        int m = 0, all_in = 1;
        for (i = 0; i < n; i++)
            if (plane_dist(&src[i], p) < 0) { all_in = 0; break; }
        if (all_in)
            continue;
        for (i = 0; i < n; i++) {
            const CV *a = &src[i], *b = &src[(i + 1) % n];
            float da = plane_dist(a, p), db = plane_dist(b, p);
            if (da >= 0 && m < MAX_POLY)
                dst[m++] = *a;
            if ((da >= 0) != (db >= 0) && m < MAX_POLY) {
                float t = da / (da - db);
                CV *o = &dst[m++];
                int k;
                o->x = a->x + (b->x - a->x) * t;
                o->y = a->y + (b->y - a->y) * t;
                o->z = a->z + (b->z - a->z) * t;
                o->w = a->w + (b->w - a->w) * t;
                o->s = a->s + (b->s - a->s) * t;
                o->t = a->t + (b->t - a->t) * t;
                o->fog = a->fog + (b->fog - a->fog) * t;
                for (k = 0; k < 4; k++)
                    o->c[k] = a->c[k] + (b->c[k] - a->c[k]) * t;
            }
        }
        n = m;
        if (n < 3)
            return 0;
        { CV *sw = src; src = dst; dst = sw; }
    }
    if (src != in)
        memcpy(in, src, (size_t)n * sizeof(CV));
    return n;
}

static u8 to8(float v)
{
    v = v * 255.0f + 0.5f;
    return (u8)(v < 0 ? 0 : (v > 255 ? 255 : v));
}

static void emit_poly(CV *p, int n, int rect, int tex_tile)
{
    F3DVert out[3 * (MAX_POLY - 2)];
    F3DVert fv[MAX_POLY];
    F3DState st;
    int textured, tile = tex_tile & 7;
    int fold_fog = blender_is_fog() && ((S.omh >> 20) & 3) != F3D_CYCLE_2;
    int i, tex_rgb = 0, tex_a = 0, any_off = 0, k, do_uv;
    const F3DTile *t = &S.tile[tile & 7];
    float ms = 1.0f, mt = 1.0f, uls = 0, ult = 0, tw = 1, th = 1;
    int shs = 0, sht = 0;

    cc_prepare(((S.omh >> 20) & 3) == F3D_CYCLE_2 ? 0 : 1);
    textured = CCC.uses_texel && tex_tile >= 0;
    build_state(&st, rect, textured ? tile : -1);
    do_uv = textured && st.tex_w && st.tex_h;
    if (do_uv) {
        /* tile_coord() per vertex, factored: the shift is a power of two (exact) */
        shs = t->shifts; sht = t->shiftt;
        if (shs > 10) ms = (float)(1 << (16 - shs)); else if (shs > 0) ms = 1.0f / (float)(1 << shs);
        if (sht > 10) mt = (float)(1 << (16 - sht)); else if (sht > 0) mt = 1.0f / (float)(1 << sht);
        uls = (float)t->uls * 0.25f; ult = (float)t->ult * 0.25f;
        if (rect && (S.tex_ox | S.tex_oy)) { uls += (float)S.tex_ox; ult += (float)S.tex_oy; }
        tw = (float)st.tex_w; th = (float)st.tex_h;
    }
    for (i = 0; i < n; i++) {
        float *shade = p[i].c;
        F3DVert *o = &fv[i];
        /* the colour bytes depend only on the shade (+ fog) inputs and the
           combiner state: reuse them when they are the same as for the last
           vertex (sprites and text quads have one colour for all corners) */
        u32 key[6];
        memcpy(key, shade, 16);
        if (fold_fog) memcpy(&key[4], &p[i].fog, 4); else key[4] = 0;
        key[5] = fold_fog ? (0x80000000u | ((u32)S.fog[0] << 16) | ((u32)S.fog[1] << 8) | S.fog[2]) : 0;
        if (VM.gen == CCC.gen && VM.valid && eq32(VM.key, key, sizeof(key))) {
            memcpy(&o->r, VM.col, 8);
            tex_rgb |= VM.tx_rgb; tex_a |= VM.tx_a;
        } else {
            float mod[4], off[4];
            int trgb = 0, ta = 0;
            cc_eval(shade, mod, off, &trgb, &ta);
            if (fold_fog) {
                float f = p[i].fog, fc;
                for (k = 0; k < 3; k++) {
                    fc = S.fog[k] * (1.0f / 255.0f);
                    mod[k] *= 1.0f - f;
                    off[k] = off[k] * (1.0f - f) + fc * f;
                }
            }
            o->r = to8(mod[0]); o->g = to8(mod[1]); o->b = to8(mod[2]); o->a = to8(mod[3]);
            o->or_ = to8(off[0]); o->og = to8(off[1]); o->ob = to8(off[2]); o->oa = to8(off[3]);
            memcpy(VM.key, key, sizeof(key));
            memcpy(VM.col, &o->r, 8);
            VM.tx_rgb = (u8)trgb; VM.tx_a = (u8)ta;
            VM.gen = CCC.gen; VM.valid = 1;
            tex_rgb |= trgb; tex_a |= ta;
        }
        if (rect) {
            o->x = p[i].x; o->y = p[i].y; o->z = p[i].z; o->w = 1.0f;
        } else {
            float iw = 1.0f / p[i].w;
            o->x = S.vtrans[0] + p[i].x * iw * S.vscale[0];
            o->y = S.vtrans[1] + S.ysign * p[i].y * iw * S.vscale[1];
            o->z = clampf((S.vtrans[2] + p[i].z * iw * S.vscale[2]) * (1.0f / 1023.0f), 0, 1);
            o->w = p[i].w;
        }
        if (do_uv) {
            float su = p[i].s, sv = p[i].t;
            if (shs) su *= ms;
            if (sht) sv *= mt;
            o->u = (su - uls) / tw;
            o->v = (sv - ult) / th;
        } else {
            o->u = o->v = 0;
        }
    }
    /* untextured channels: colour is the constant part */
    for (i = 0; i < n; i++) {
        F3DVert *o = &fv[i];
        if (!tex_rgb || !textured) { o->r = o->or_; o->g = o->og; o->b = o->ob; o->or_ = o->og = o->ob = 0; }
        if (!tex_a || !textured) { o->a = o->oa; o->oa = 0; }
        if (o->or_ | o->og | o->ob | o->oa) any_off = 1;
    }
    st.tex_rgb = (u8)(textured && tex_rgb);
    st.tex_alpha = (u8)(textured && tex_a);
    st.offset = (u8)any_off;
    if (!textured) { st.tex = NULL; st.tex_w = st.tex_h = 0; }
    for (i = 0; i + 2 < n; i++) {
        out[i * 3 + 0] = fv[0];
        out[i * 3 + 1] = fv[i + 1];
        out[i * 3 + 2] = fv[i + 2];
    }
    S.stats.tris_out += (u32)(n - 2);
    if (S.be && S.be->draw_tris)
        S.be->draw_tris(S.be->ctx, commit_state(&st), out, (n - 2) * 3);
}

static void to_cv(CV *o, const Vtx *v)
{
    o->x = v->x; o->y = v->y; o->z = v->z; o->w = v->w;
    o->s = v->s; o->t = v->t;
    o->c[0] = v->r; o->c[1] = v->g; o->c[2] = v->b; o->c[3] = v->a;
    o->fog = v->fog;
}

static void draw_tri(int i0, int i1, int i2, int flag)
{
    const Vtx *v[3];
    CV poly[MAX_POLY], tmp[MAX_POLY];
    int n, i;
    float area = 0;

    S.stats.tris_in++;
    v[0] = &S.vtx[i0 & 15]; v[1] = &S.vtx[i1 & 15]; v[2] = &S.vtx[i2 & 15];
    if (v[0]->clip & v[1]->clip & v[2]->clip) {
        S.stats.tris_culled++;
        return;
    }
    for (i = 0; i < 3; i++)
        to_cv(&poly[i], v[i]);
    if (!(S.geom & G_SHADE)) {
        for (i = 0; i < 3; i++)
            poly[i].c[0] = poly[i].c[1] = poly[i].c[2] = poly[i].c[3] = 0;
    } else if (!(S.geom & G_SHADING_SMOOTH)) {
        const Vtx *f = v[flag > 2 ? 0 : flag];
        for (i = 0; i < 3; i++) {
            poly[i].c[0] = f->r; poly[i].c[1] = f->g; poly[i].c[2] = f->b; poly[i].c[3] = f->a;
        }
    }
    n = all_inside(poly, 3) ? 3 : clip_poly(poly, 3, tmp);
    if (n < 3) {
        S.stats.tris_culled++;
        return;
    }
    /* culling on the projected polygon (y-down screen): front faces are
     * counter-clockwise with y up, i.e. negative area here */
    if (S.geom & (G_CULL_FRONT | G_CULL_BACK)) {
        float sx[MAX_POLY], sy[MAX_POLY];
        for (i = 0; i < n; i++) {
            float iw = 1.0f / poly[i].w;
            sx[i] = S.vtrans[0] + poly[i].x * iw * S.vscale[0];
            sy[i] = S.vtrans[1] + S.ysign * poly[i].y * iw * S.vscale[1];
        }
        for (i = 0; i < n; i++) {
            int j = (i + 1) % n;
            area += sx[i] * sy[j] - sx[j] * sy[i];
        }
        if (((S.geom & G_CULL_BACK) && area >= 0) || ((S.geom & G_CULL_FRONT) && area <= 0)) {
            S.stats.tris_culled++;
            return;
        }
    }
    emit_poly(poly, n, 0, S.tex_on ? S.tex_tile : -1);
}

/* ------------------------------------------------------------------ rectangles */
static void scissor_rect(float *x0, float *y0, float *x1, float *y1)
{
    if (*x0 < S.sc_x0) *x0 = S.sc_x0;
    if (*y0 < S.sc_y0) *y0 = S.sc_y0;
    if (*x1 > S.sc_x1) *x1 = S.sc_x1;
    if (*y1 > S.sc_y1) *y1 = S.sc_y1;
}

static void cmd_fillrect(u32 w0, u32 w1)
{
    float x1 = (float)(((w0 >> 12) & 0xFFF) >> 2), y1 = (float)((w0 & 0xFFF) >> 2);
    float x0 = (float)(((w1 >> 12) & 0xFFF) >> 2), y0 = (float)((w1 & 0xFFF) >> 2);
    u32 cyc = (S.omh >> 20) & 3;
    S.stats.rects++;
    if (cyc >= 2) {             /* fill / copy: inclusive lower-right */
        x1 += 1; y1 += 1;
    }
    scissor_rect(&x0, &y0, &x1, &y1);
    if (x1 <= x0 || y1 <= y0)
        return;
    if (cyc == F3D_CYCLE_FILL) {
        u32 argb;
        if (S.cimg_addr == S.zimg_addr) {
            if (S.be && S.be->clear_depth) S.be->clear_depth(S.be->ctx, x0, y0, x1, y1);
            return;
        }
        if (S.cimg_siz == 3) {
            u32 c = S.fill_color;
            argb = ((c & 0xFF) << 24) | (c >> 8);
        } else {
            /* the 5551 alpha bit is coverage in the frame buffer, not transparency */
            argb = expand5551((u16)(S.fill_color >> 16)) | 0xFF000000u;
        }
        if (S.be && S.be->fill_rect) S.be->fill_rect(S.be->ctx, x0, y0, x1, y1, argb);
        return;
    }
    {
        /* 1/2-cycle fill rectangle: combiner with shade = 0, no texture */
        CV p[4];
        int i;
        memset(p, 0, sizeof(p));
        p[0].x = x0; p[0].y = y0; p[1].x = x1; p[1].y = y0;
        p[2].x = x1; p[2].y = y1; p[3].x = x0; p[3].y = y1;
        for (i = 0; i < 4; i++) { p[i].w = 1; p[i].z = S.prim_z / 32767.0f; }
        emit_poly(p, 4, 1, -1);
    }
}

/* TEXRECT as the RDP draws it (angrylion's edge walker / coverage code):
 *  - 1/2-cycle mode, no AA: pixel (X, Y) is written iff its integer corner lies
 *    inside the rectangle: xl <= X < xh, yl <= Y < yh (10.2 edges, scissor alike).
 *  - texture coordinates at that pixel: s = s0 + (X - xl) * dsdx (exact edge),
 *    t = t0 + (Y - floor(yl)) * dtdy (t starts on the scanline holding the top
 *    edge, without a sub-scanline adjustment).
 *  - point sampling takes texel floor(s); bilinear blends floor(s) and floor(s)+1
 *    by frac(s) (texel centres on integer coordinates, no half-texel shift).
 * The PVR samples at pixel centres (two per N64 pixel at 640x480) and its
 * bilinear filter has texel centres at +0.5, so for bilinear the coordinates are
 * shifted to put the RDP's sample of each N64 pixel at its centre, and only the
 * texels the RDP fetches are decoded (texel window, clamped at its edges), so
 * nothing outside them bleeds in. Example: the photo strips (TEXRECT 0.25..6.25,
 * 7 rows loaded, the 7th overwriting the start of row 0 in TMEM) draw scanlines
 * 1..6 with rows 1..6; the clobbered row 0 is never seen on hardware.
 * Copy / fill mode keep the inclusive rectangle of before. */
static void texrect(u32 w0, u32 w1, u32 st, u32 dsdt, int flip)
{
    float xh = ((w0 >> 12) & 0xFFF) * 0.25f, yh = (w0 & 0xFFF) * 0.25f;
    float xl = ((w1 >> 12) & 0xFFF) * 0.25f, yl = (w1 & 0xFFF) * 0.25f;
    int tile = (int)((w1 >> 24) & 7);
    float s = (float)(s16)(st >> 16) / 32.0f, t = (float)(s16)(st & 0xFFFF) / 32.0f;
    float dsdx = (float)(s16)(dsdt >> 16) / 1024.0f, dtdy = (float)(s16)(dsdt & 0xFFFF) / 1024.0f;
    u32 cyc = (S.omh >> 20) & 3;
    float x0, y0, x1, y1;
    CV p[4];
    int i;

    S.stats.rects++;
    if (cyc >= 2) {             /* copy mode: inclusive, 4 texels per step */
        xh += 1; yh += 1;
        if (cyc == F3D_CYCLE_COPY) dsdx /= 4.0f;
    }
    x0 = xl; y0 = yl; x1 = xh; y1 = yh;
    scissor_rect(&x0, &y0, &x1, &y1);
    if (cyc < 2) {
        /* covered pixels: integer corner inside */
        x0 = ceilf(x0); y0 = ceilf(y0); x1 = ceilf(x1); y1 = ceilf(y1);
    }
    if (x1 <= x0 || y1 <= y0)
        return;
    memset(p, 0, sizeof(p));
    p[0].x = x0; p[0].y = y0; p[1].x = x1; p[1].y = y0;
    p[2].x = x1; p[2].y = y1; p[3].x = x0; p[3].y = y1;
    if (cyc < 2) {
        const F3DTile *tl = &S.tile[tile & 7];
        int bilerp = ((S.omh >> 12) & 3) != 0;
        float yt = floorf(yl);
        /* RDP values at the first / last covered pixel */
        float sx0 = s + (x0 - xl) * dsdx, sx1 = s + (x1 - 1 - xl) * dsdx;   /* along x */
        float ty0 = t + (y0 - yt) * dtdy, ty1 = t + (y1 - 1 - yt) * dtdy;   /* along y */
        float ha = bilerp ? 0.5f : 0.0f;
        for (i = 0; i < 4; i++) {
            /* texel coordinate at the PVR position: corner convention for point
               sampling (as before); for bilinear the RDP sample of pixel X sits at
               X + 0.5 and the PVR's texel centres are at +0.5 */
            float dx = p[i].x - x0, dy = p[i].y - y0;
            float a = sx0 + (dx - ha) * dsdx + ha, b = ty0 + (dy - ha) * dtdy + ha;
            if (!flip) { p[i].s = a; p[i].t = b; }
            else {
                /* flipped: s runs down, t runs across */
                p[i].s = s + (p[i].y - ha - yt) * dsdx + ha;
                p[i].t = t + (p[i].x - ha - xl) * dtdy + ha;
            }
            p[i].w = 1;
            p[i].z = (S.oml & ZSRC_PRIM) ? S.prim_z / 32767.0f : 0;
        }
        if (bilerp && !flip && !tl->shifts && !tl->shiftt && ((tl->cms & 2) || !tl->masks) &&
            ((tl->cmt & 2) || !tl->maskt)) {
            /* texel window: tile-relative texels fetched for these pixels */
            float ul = (float)tl->uls * 0.25f, ut = (float)tl->ult * 0.25f;
            float smin = (sx0 < sx1 ? sx0 : sx1) - ul, smax = (sx0 < sx1 ? sx1 : sx0) - ul;
            float tmin = (ty0 < ty1 ? ty0 : ty1) - ut, tmax = (ty0 < ty1 ? ty1 : ty0) - ut;
            int sfrac = smin != floorf(smin) || dsdx != floorf(dsdx);
            int tfrac = tmin != floorf(tmin) || dtdy != floorf(dtdy);
            int c0 = (int)floorf(smin), c1 = (int)floorf(smax) + sfrac;
            int r0 = (int)floorf(tmin), r1 = (int)floorf(tmax) + tfrac;
            if (c0 < 0) c0 = 0;
            if (r0 < 0) r0 = 0;
            if (c1 >= c0 && r1 >= r0) {
                S.crop_on = 1;
                S.crop_x0 = c0; S.crop_y0 = r0;
                S.crop_w = c1 - c0 + 1; S.crop_h = r1 - r0 + 1;
            }
        }
    } else {
        for (i = 0; i < 4; i++) {
            float dx = p[i].x - xl, dy = p[i].y - yl;
            if (!flip) { p[i].s = s + dx * dsdx; p[i].t = t + dy * dtdy; }
            else { p[i].s = s + dy * dsdx; p[i].t = t + dx * dtdy; }
            p[i].w = 1;
            p[i].z = (S.oml & ZSRC_PRIM) ? S.prim_z / 32767.0f : 0;
        }
    }
    emit_poly(p, 4, 1, tile);
    S.crop_on = 0;
}

/* ------------------------------------------------------------------ main loop */
static int cull_dl(u32 w0, u32 w1)
{
    int a = (int)((w0 & 0xFFFF) / 40), b = (int)((w1 & 0xFFFF) / 40), i;
    u8 c = 0x3F;
    if (a > 15 || b > 15 || a > b) return 0;
    for (i = a; i <= b; i++) c &= S.vtx[i].clip;
    return c != 0;
}

void f3d_run_dl(uint32_t dl)
{
    const u32 *stack[DL_STACK];
    int sp = 0;
    const u32 *pc = (const u32 *)ptr(dl);

    for (;;) {
        u32 w0 = pc[0], w1 = pc[1];
        u32 op = w0 >> 24;
        pc += 2;
        S.stats.cmds++;
        switch (op) {
        case OP_SPNOOP: break;
        case OP_MTX: cmd_mtx(w0, w1); break;
        case OP_MOVEMEM: cmd_movemem(w0, w1); break;
        case OP_VTX: cmd_vtx(w0, w1); break;
        case OP_DL:
            if (((w0 >> 16) & 0xFF) == 0) {
                if (sp < DL_STACK) stack[sp++] = pc;
                else { logmsg("f3d: DL stack overflow"); break; }
            }
            pc = (const u32 *)ptr(w1);
            break;
        case OP_TRI1:
            draw_tri((int)((w1 >> 16) & 0xFF) / 10, (int)((w1 >> 8) & 0xFF) / 10, (int)(w1 & 0xFF) / 10,
                     (int)(w1 >> 24));
            break;
        case OP_CULLDL:
        case OP_ENDDL:
            /* G_CULLDL: everything off-screen ends this list like G_ENDDL */
            if (op == OP_CULLDL && !cull_dl(w0, w1)) break;
            if (sp == 0) {
                pl_flush();         /* the sources may change before the next task */
                return;
            }
            pc = stack[--sp];
            break;
        case OP_POPMTX:
            if (S.mv_top > 0) { S.mv_top--; S.mvp_dirty = 1; S.lights_dirty = 1; }
            break;
        case OP_MOVEWORD: cmd_moveword(w0, w1); break;
        case OP_TEXTURE:
            S.tex_ss = (float)(w1 >> 16) / 65536.0f;
            S.tex_st = (float)(w1 & 0xFFFF) / 65536.0f;
            S.tex_tile = (int)((w0 >> 8) & 7);
            S.tex_level = (int)((w0 >> 11) & 7);
            S.tex_on = (w0 & 0xFF) != 0;
            break;
        case OP_SETOTHERMODE_H: case OP_SETOTHERMODE_L: {
            u32 sh = (w0 >> 8) & 0xFF, len = w0 & 0xFF;
            u32 mask = (len >= 32 ? 0xFFFFFFFFu : ((1u << len) - 1)) << sh;
            u32 *m = op == OP_SETOTHERMODE_H ? &S.omh : &S.oml;
            u32 old = *m;
            *m = (*m & ~mask) | (w1 & mask);
            if (op == OP_SETOTHERMODE_H && ((old ^ *m) & (3u << 14))) S.tex_dirty = 1;
            break;
        }
        case OP_SETGEOMETRYMODE: S.geom |= w1; break;
        case OP_CLEARGEOMETRYMODE: S.geom &= ~w1; break;
        case OP_RDPHALF_1: S.rdphalf1 = w1; break;
        case OP_RDPHALF_2: S.rdphalf2 = w1; break;
        case OP_RDPHALF_CONT: break;
        case OP_TEXRECT: case OP_TEXRECTFLIP: {
            /* F3D: the rectangle is followed by RDPHALF_1 (s, t) and RDPHALF_2 (dsdx, dtdy) */
            u32 st = S.rdphalf1, dd = S.rdphalf2;
            if ((pc[0] >> 24) == OP_RDPHALF_1) { st = pc[1]; pc += 2; S.stats.cmds++; }
            if ((pc[0] >> 24) == OP_RDPHALF_2) { dd = pc[1]; pc += 2; S.stats.cmds++; }
            texrect(w0, w1, st, dd, op == OP_TEXRECTFLIP);
            break;
        }
        case OP_RDPLOADSYNC: case OP_RDPPIPESYNC: case OP_RDPTILESYNC: case OP_RDPFULLSYNC:
        case OP_SETKEYGB: case OP_SETKEYR: case OP_SETCONVERT:
            break;
        case OP_SETSCISSOR:
            S.sc_x0 = ((w0 >> 12) & 0xFFF) * 0.25f; S.sc_y0 = (w0 & 0xFFF) * 0.25f;
            S.sc_x1 = ((w1 >> 12) & 0xFFF) * 0.25f; S.sc_y1 = (w1 & 0xFFF) * 0.25f;
            break;
        case OP_SETPRIMDEPTH: S.prim_z = (u16)(w1 >> 16); S.prim_dz = (u16)w1; break;
        case OP_RDPSETOTHERMODE:
            if (((S.omh ^ w0) & (3u << 14))) S.tex_dirty = 1;
            S.omh = w0 & 0x00FFFFFF; S.oml = w1;
            break;
        case OP_LOADTLUT: cmd_loadtlut(w0, w1); break;
        case OP_SETTILESIZE: {
            F3DTile *t = &S.tile[(w1 >> 24) & 7];
            t->uls = (u16)((w0 >> 12) & 0xFFF); t->ult = (u16)(w0 & 0xFFF);
            t->lrs = (u16)((w1 >> 12) & 0xFFF); t->lrt = (u16)(w1 & 0xFFF);
            S.tex_dirty = 1;
            break;
        }
        case OP_LOADBLOCK: cmd_loadblock(w0, w1); break;
        case OP_LOADTILE: cmd_loadtile(w0, w1); break;
        case OP_SETTILE: {
            F3DTile *t = &S.tile[(w1 >> 24) & 7];
            t->fmt = (u8)((w0 >> 21) & 7); t->siz = (u8)((w0 >> 19) & 3);
            t->line = (u16)((w0 >> 9) & 0x1FF); t->tmem = (u16)(w0 & 0x1FF);
            t->pal = (u8)((w1 >> 20) & 0xF);
            t->cmt = (u8)((w1 >> 18) & 3); t->maskt = (u8)((w1 >> 14) & 0xF); t->shiftt = (u8)((w1 >> 10) & 0xF);
            t->cms = (u8)((w1 >> 8) & 3); t->masks = (u8)((w1 >> 4) & 0xF); t->shifts = (u8)(w1 & 0xF);
            S.tex_dirty = 1;
            break;
        }
        case OP_FILLRECT: cmd_fillrect(w0, w1); break;
        case OP_SETFILLCOLOR: S.fill_color = w1; break;
        case OP_SETFOGCOLOR:
            S.fog[0] = (u8)(w1 >> 24); S.fog[1] = (u8)(w1 >> 16); S.fog[2] = (u8)(w1 >> 8); S.fog[3] = (u8)w1;
            break;
        case OP_SETBLENDCOLOR:
            S.blend[0] = (u8)(w1 >> 24); S.blend[1] = (u8)(w1 >> 16); S.blend[2] = (u8)(w1 >> 8); S.blend[3] = (u8)w1;
            break;
        case OP_SETPRIMCOLOR:
            S.prim[0] = (u8)(w1 >> 24); S.prim[1] = (u8)(w1 >> 16); S.prim[2] = (u8)(w1 >> 8); S.prim[3] = (u8)w1;
            S.prim_min_level = (u8)(w0 >> 8); S.prim_lod_frac = (u8)w0;
            break;
        case OP_SETENVCOLOR:
            S.env[0] = (u8)(w1 >> 24); S.env[1] = (u8)(w1 >> 16); S.env[2] = (u8)(w1 >> 8); S.env[3] = (u8)w1;
            break;
        case OP_SETCOMBINE: S.cc_h = w0 & 0x00FFFFFF; S.cc_l = w1; break;
        case OP_SETTIMG:
            S.timg_fmt = (int)((w0 >> 21) & 7); S.timg_siz = (int)((w0 >> 19) & 3);
            S.timg_width = (int)(w0 & 0xFFF) + 1; S.timg_addr = resolve(w1);
            break;
        case OP_SETZIMG: S.zimg_addr = resolve(w1); break;
        case OP_SETCIMG:
            S.cimg_fmt = (int)((w0 >> 21) & 7); S.cimg_siz = (int)((w0 >> 19) & 3);
            S.cimg_width = (int)(w0 & 0xFFF) + 1; S.cimg_addr = resolve(w1);
            if (S.be && S.be->color_image)
                S.be->color_image(S.be->ctx, S.cimg_addr, S.cimg_fmt, S.cimg_siz, S.cimg_width);
            break;
        default:
            /* G_LINE3D, G_SPRITE2D and anything else: not used by the game */
            S.stats.unknown++;
            S.stats.last_unknown = w0;
            logmsg("f3d: unknown command");
            break;
        }
    }
}

void f3d_run_task(uint32_t dl)
{
    if (S.be && S.be->begin_frame) S.be->begin_frame(S.be->ctx);
    S.tex_handle = NULL;          /* backend caches may have been flushed between frames */
    S.tex_dirty = 1;
    src_memo_cur++;               /* source hashes are remembered within one task only */
    f3d_run_dl(dl);
    if (S.be && S.be->end_frame) S.be->end_frame(S.be->ctx);
}
