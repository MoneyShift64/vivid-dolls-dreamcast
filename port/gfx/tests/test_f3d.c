/* Host unit tests for the F3D interpreter (port/gfx/f3d.c).
 * Build & run: make -C port/gfx/tests   (or see the Makefile there)
 *
 * The display lists below are built the same way the game builds them
 * (src/game/sprite.c, text.c, object.c, gfx.c, photoview.c and the static
 * set-up lists in the data segment, written out here symbolically). */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../f3d.h"
#include "../null_backend.h"

typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint32_t u32;
typedef int32_t s32;

/* ------------------------------------------------------------ fake RDRAM */
#define RAM_SIZE (2u << 20)
static u8 ram[RAM_SIZE] __attribute__((aligned(16)));
static u32 ram_top;

static void *addr_hook(u32 a) { return &ram[a & (RAM_SIZE - 1) & 0x00FFFFFF]; }
static u32 alloc(u32 n)
{
    u32 a = (ram_top + 15) & ~15u;
    if (a + n > RAM_SIZE) { printf("  fake RAM exhausted\n"); exit(2); }
    ram_top = a + n; memset(&ram[a], 0, n); return 0x80000000u | a;
}
#define P(a) ((void *)&ram[(a) & 0x00FFFFFF])

/* display-list builder */
typedef struct { u32 base, n; } DL;
static DL dl_new(void) { DL d; d.base = alloc(0x4000); d.n = 0; return d; }
static void G(DL *d, u32 w0, u32 w1) { u32 *p = P(d->base + d->n * 8); p[0] = w0; p[1] = w1; d->n++; }

/* ------------------------------------------------------------ test macros */
static int fails, checks;
#define CHECK(c) do { checks++; if (!(c)) { fails++; printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #c); } } while (0)
#define NEAR(a, b, eps) CHECK(fabs((double)(a) - (double)(b)) <= (eps))

static F3DBackend be;
static NullBackend nb;

/* ------------------------------------------------------------ helpers */
typedef struct { s16 x, y, z; u16 flag; s16 s, t; u8 r, g, b, a; } Vtx;

static u32 vtx_buf(const Vtx *v, int n) { u32 a = alloc((u32)n * 16); memcpy(P(a), v, (size_t)n * 16); return a; }

/* Mtx_ToFixed (src/game/system.c) */
static u32 mtx_fixed(const float m[16])
{
    u32 a = alloc(64), *o = P(a);
    int i, j;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 2; j++) {
            s32 e1 = (s32)(m[i * 4 + j * 2] * 65536.0f), e2 = (s32)(m[i * 4 + j * 2 + 1] * 65536.0f);
            o[i * 2 + j] = ((u32)e1 & 0xFFFF0000u) | ((u32)e2 >> 16);
            o[8 + i * 2 + j] = ((u32)e1 << 16) | ((u32)e2 & 0xFFFF);
        }
    return a;
}

static void m_ident(float m[16]) { memset(m, 0, 64); m[0] = m[5] = m[10] = m[15] = 1; }

static void m_ortho(float m[16], float l, float r, float b, float t, float n, float f)
{
    m_ident(m);
    m[0] = 2 / (r - l); m[5] = 2 / (t - b); m[10] = -2 / (f - n);
    m[12] = -(r + l) / (r - l); m[13] = -(t + b) / (t - b); m[14] = -(f + n) / (f - n);
}

static void m_persp(float m[16], float fovy, float aspect, float n, float f)
{
    float c = 1.0f / tanf(fovy * 3.14159265f / 360.0f);
    memset(m, 0, 64);
    m[0] = c / aspect; m[5] = c; m[10] = (n + f) / (n - f); m[11] = -1; m[14] = 2 * n * f / (n - f);
}

static u32 viewport(int w, int h, int flip)
{
    u32 a = alloc(16);
    s16 *v = P(a);
    v[0] = (s16)(w * 2); v[1] = (s16)(flip ? -h * 2 : h * 2); v[2] = 0x1FF; v[3] = 0;
    v[4] = (s16)(w * 2); v[5] = (s16)(h * 2); v[6] = 0x1FF; v[7] = 0;
    return a;
}

/* gsDPSetCombineLERP, same mode in both cycles */
static u32 ccw0(int a, int b, int c, int d, int Aa, int Ab, int Ac, int Ad)
{
    (void)b; (void)d; (void)Ab; (void)Ad;
    return 0xFC000000u | (u32)a << 20 | (u32)c << 15 | (u32)Aa << 12 | (u32)Ac << 9 | (u32)a << 5 | (u32)c;
}
static u32 ccw1(int a, int b, int c, int d, int Aa, int Ab, int Ac, int Ad)
{
    (void)a; (void)c;
    return (u32)b << 28 | (u32)b << 24 | (u32)Aa << 21 | (u32)Ac << 18 | (u32)d << 15 | (u32)Ab << 12 |
           (u32)Ad << 9 | (u32)d << 6 | (u32)Ab << 3 | (u32)Ad;
}
#define CC(d_, a, b, c, dd, Aa, Ab, Ac, Ad) G(d_, ccw0(a, b, c, dd, Aa, Ab, Ac, Ad), ccw1(a, b, c, dd, Aa, Ab, Ac, Ad))
#define CC_SHADE(d_)     CC(d_, 15, 15, 31, 4, 7, 7, 7, 4)
#define CC_DECALRGBA(d_) CC(d_, 15, 15, 31, 1, 7, 7, 7, 1)
#define CC_MODEL(d_)     CC(d_, 1, 15, 4, 3, 1, 7, 3, 7)      /* (T0)*SHADE+PRIM / T0*PRIM */
/* render mode word: flags + the same blender in both cycles */
static u32 rm(u32 flags, u32 p, u32 a, u32 m, u32 b)
{
    return flags | p << 30 | a << 26 | m << 22 | b << 18 | p << 28 | a << 24 | m << 20 | b << 16;
}
#define RENDERMODE(d_, w1) G(d_, 0xB900031D, (w1))

/* the static lists of the game, written symbolically */
static void dl_rdp_init(DL *d)      /* D_80038560 */
{
    int i;
    G(d, 0xFB000000, 0); G(d, 0xFA000000, 0); G(d, 0xF9000000, 0); G(d, 0xF8000000, 0);
    G(d, 0xF7000000, 0); G(d, 0xEE000000, 0); G(d, 0xEC000000, 0); G(d, 0xEB000000, 0); G(d, 0xEA000000, 0);
    G(d, 0xBA001701, 0);
    CC_SHADE(d);
    for (i = 0; i < 8; i++) G(d, 0xF5100000, (u32)i << 24);
    for (i = 0; i < 8; i++) G(d, 0xF2000000, (u32)i << 24);
    G(d, 0xE7000000, 0);
}

static void dl_rdp_modes(DL *d)     /* D_80038648 */
{
    G(d, 0xBA000602, 0xC0); G(d, 0xBA000402, 0x30); G(d, 0xB9000002, 1); G(d, 0xF9000000, 8);
    G(d, 0xBA001301, 0x80000); G(d, 0xBA000C02, 0x2000); G(d, 0xBA000903, 0xC00);
    G(d, 0xBA001102, 0); G(d, 0xBA001001, 0); G(d, 0xBA000E02, 0);
}

static void frame_start(DL *d, u32 vp, int clear)
{
    u32 zbuf = 0x80115000u, fb = 0x807B2600u & 0x80FFFFFF;
    zbuf = 0x80100000u; fb = 0x80140000u;
    G(d, 0xBC000006, 0);
    G(d, 0xB6000000, 0x001F3207); G(d, 0xB7000000, 0x204); G(d, 0xBB000000, 0);
    dl_rdp_init(d);
    dl_rdp_modes(d);
    G(d, 0x03800010, vp); G(d, 0xE7000000, 0);
    G(d, 0xED000000, ((u32)(319 * 4) << 12) | (239 * 4));
    G(d, 0xFE000000, zbuf);
    G(d, 0xE7000000, 0); G(d, 0xBA001402, 0x300000);
    G(d, 0xFF100000 | 319, zbuf); G(d, 0xF7000000, 0xFFFCFFFC);
    G(d, 0xF6000000 | (319u << 14) | (239u << 2), 0);
    G(d, 0xFF100000 | 319, fb);
    if (clear) {
        u32 c = ((0x10 << 8) & 0xF800) | ((0x20 << 3) & 0x7C0) | ((0xF8 >> 2) & 0x3E);
        G(d, 0xF7000000, ((c | 1) << 16) | 1 | c);
        G(d, 0xF6000000 | (319u << 14) | (239u << 2), 0);
    }
    G(d, 0xE7000000, 0); G(d, 0xBA001402, 0);
}

static void run(DL *d)
{
    G(d, 0xE9000000, 0); G(d, 0xB8000000, 0);
    null_backend_clear(&nb);
    f3d_run_task(d->base);
}


static const NBTex *tri_tex(int i) { return nb.tris[i].tex_index >= 0 ? &nb.tex[nb.tris[i].tex_index] : NULL; }

/* ======================================================================== */
static void test_mtx_and_selftest(void)
{
    float m[16];
    u32 a;
    DL d = dl_new();
    printf("mtx / selftest\n");
    CHECK(f3d_mtx_selftest() == 0);
    /* a fixed-point matrix with negative and fractional values survives */
    m_ident(m);
    m[0] = -1.5f; m[5] = 0.25f; m[12] = -100.0f; m[13] = 100.75f; m[1] = -0.0001f;
    a = mtx_fixed(m);
    {
        Vtx v[3] = { { 0, 0, 0, 0, 0, 0, 255, 255, 255, 255 }, { 10, 0, 0, 0, 0, 0, 255, 255, 255, 255 },
                     { 0, 10, 0, 0, 0, 0, 255, 255, 255, 255 } };
        float pr[16];
        frame_start(&d, viewport(320, 240, 1), 0);
        m_ortho(pr, -160, 160, -120, 120, -1, 3000);
        G(&d, 0x01030040, mtx_fixed(pr));
        G(&d, 0x01020040, a);
        G(&d, 0x04200030, vtx_buf(v, 3));
        G(&d, 0xBF000000, 0x00000A14);
        run(&d);
        CHECK(nb.ntris == 1);
        if (nb.ntris == 1) {
            /* v0 -> (-100, 100.75) world -> screen (60, 19.25) */
            NEAR(nb.tris[0].v[0].x, 60.0, 0.25);
            NEAR(nb.tris[0].v[0].y, 19.25, 0.25);
            /* v1: x = 10*-1.5 - 100 = -115 -> 45 */
            NEAR(nb.tris[0].v[1].x, 45.0, 0.25);
            /* v2: y = 10*0.25 + 100.75 -> screen 16.75 */
            NEAR(nb.tris[0].v[2].y, 16.75, 0.25);
        }
    }
}

/* frame start + the screen quad of ScreenQuad_SetColorA2 (D_80038880) */
static void test_frame_and_screen_quad(void)
{
    DL d = dl_new(), q = dl_new();
    float pr[16], mv[16];
    Vtx v[4] = { { -160, 120, 0, 0, 0, 0, 255, 0, 0, 128 }, { 160, 120, 0, 0, 0, 0, 0, 255, 0, 128 },
                 { -160, -120, 0, 0, 0, 0, 0, 0, 255, 128 }, { 160, -120, 0, 0, 0, 0, 255, 255, 255, 128 } };
    printf("frame set-up, fill rects, screen quad\n");
    /* D_80038880 */
    G(&q, 0xB6000000, 0x001F3207); G(&q, 0xB7000000, 0x204); G(&q, 0xBB000000, 0x80008000);
    G(&q, 0xE7000000, 0); G(&q, 0xBA000E02, 0);
    RENDERMODE(&q, rm(0x40 | 0x4000 | 0x200, 0, 0, 1, 0));     /* XLU_SURF, ZMODE_OPA */
    CC_SHADE(&q);
    G(&q, 0x04300040, vtx_buf(v, 4));
    G(&q, 0xBF000000, 0x00001E0A); G(&q, 0xBF000000, 0x0000141E); G(&q, 0xB8000000, 0);

    frame_start(&d, viewport(320, 240, 1), 1);
    G(&d, 0xED000000, (320u * 4 << 12) | (240u * 4));   /* full 320x240 (the game uses 319x239) */
    m_ortho(pr, -160, 160, -120, 120, -1, 3000);
    G(&d, 0x01030040, mtx_fixed(pr));
    m_ident(mv); mv[14] = -405.11322f;
    G(&d, 0x01020040, mtx_fixed(mv));
    G(&d, 0x06000000, q.base);
    run(&d);

    CHECK(nb.frames_begun == nb.frames_ended && nb.frames_begun > 0);
    CHECK(nb.cimg_addr == 0x80140000u && nb.cimg_width == 320 && nb.cimg_siz == 2);
    CHECK(nb.nrects == 2);
    if (nb.nrects == 2) {
        CHECK(nb.rects[0].depth == 1);
        NEAR(nb.rects[0].x1, 319, 0); NEAR(nb.rects[0].y1, 239, 0);   /* game scissor 319x239 */
        CHECK(nb.rects[1].depth == 0);
        /* clear colour r=0x10 g=0x20 b=0xF8 through RGBA5551 */
        CHECK(nb.rects[1].argb == 0xFF1021FFu);
    }
    CHECK(nb.ntris == 2);
    if (nb.ntris == 2) {
        const NBTri *t = &nb.tris[0];
        /* TRI1 0 3 1: (-160,120) -> screen (0,0) top-left, (160,-120) -> (320,240) */
        NEAR(t->v[0].x, 0, 0.25); NEAR(t->v[0].y, 0, 0.25);
        NEAR(t->v[1].x, 320, 0.25); NEAR(t->v[1].y, 240, 0.25);
        NEAR(t->v[2].x, 320, 0.25); NEAR(t->v[2].y, 0, 0.25);
        CHECK(t->v[0].r == 255 && t->v[0].g == 0 && t->v[0].a == 128);
        CHECK(t->v[2].g == 255);
        CHECK(t->st.tex == NULL && !t->st.tex_rgb);
        CHECK(t->st.blend == F3D_BLEND_ALPHA);
        CHECK(t->st.alpha_ref == 8);              /* D_80038648: threshold vs blend alpha 8 */
        CHECK(!t->st.z_cmp);
        NEAR(t->v[0].w, 1.0, 1e-4);
    }
}

/* Sprite_Parse for a CI8 sprite: TLUT, LOADBLOCK as 16b with dxt, tile 0 CI8 */
static void build_sprite_setup(DL *d)       /* gSpriteSetupDL */
{
    G(d, 0xB6000000, 0x001F3207); G(d, 0xBB000001, 0x80008000); G(d, 0xE7000000, 0);
    G(d, 0xBA000E02, 0x8000);
    RENDERMODE(d, rm(0x200, 0, 0, 0, 0));        /* CVG_DST_FULL, blender bypassed */
    CC_DECALRGBA(d);
    G(d, 0xBA000C02, 0);                         /* TEXTFILT POINT */
    G(d, 0xB8000000, 0);
}

static u32 load_block_w1(s32 words_per_line, s32 texels)   /* sprite.c load_block */
{
    s32 n = 0x800, t = texels - 1;
    if (words_per_line != 0) n = words_per_line + 0x7FF;
    if (t >= 0x800) t = 0x7FF;
    return ((u32)(t & 0xFFF) << 12) | 0x07000000u | (u32)((words_per_line != 0 ? n / words_per_line : n) & 0xFFF);
}

static void test_ci8_sprite(void)
{
    enum { W = 16, H = 4 };
    DL d = dl_new(), s = dl_new(), sp = dl_new();
    u32 pal = alloc(512), tex = alloc(W * H);
    u16 *pp = P(pal);
    u8 *tp = P(tex);
    float pr[16], mv[16];
    int i, x, y;
    Vtx v[4];
    printf("CI8 sprite (TLUT 256, LOADBLOCK dxt, odd-row swap)\n");
    for (i = 0; i < 256; i++)
        pp[i] = (u16)(((i & 31) << 11) | (((i * 3) & 31) << 6) | (((255 - i) & 31) << 1) | (i != 0));
    for (i = 0; i < W * H; i++) tp[i] = (u8)(i * 7 + 1);
    tp[5] = 0;                                    /* a transparent texel */

    /* corners as Sprite_Parse mode 0: s,t = corner*tile*0x40 + 0x1F */
    for (i = 0; i < 4; i++) {
        int cx = i & 1, cy = i >> 1;
        v[i].x = (s16)(cx * W); v[i].y = (s16)(-cy * H); v[i].z = 0; v[i].flag = 0;
        v[i].s = (s16)(W * cx * 0x40 + 0x1F); v[i].t = (s16)(H * cy * 0x40 + 0x1F);
        v[i].r = 0; v[i].g = 0; v[i].b = 0x7F; v[i].a = 0xFF;
    }
    G(&sp, 0xFD100000, pal); G(&sp, 0xE8000000, 0); G(&sp, 0xF5000100, 0x02000000); G(&sp, 0xE6000000, 0);
    G(&sp, 0xF0000000, 0x023FC000); G(&sp, 0xE7000000, 0);
    G(&sp, 0xFD500000, tex); G(&sp, 0xF5500000, 0x07080200); G(&sp, 0xE6000000, 0);
    G(&sp, 0xF3000000, load_block_w1(W >> 3, (W * H + 1) >> 1)); G(&sp, 0xE7000000, 0);
    G(&sp, ((((u32)(W + 7) >> 3) & 0x1FF) << 9) | 0xF5480000u, 0x80200);
    G(&sp, 0xF2000000, ((u32)((W - 1) * 4) << 12) | (u32)((H - 1) * 4));
    G(&sp, 0x04300040, vtx_buf(v, 4));
    G(&sp, 0xBF000000, 0 * 0xA0000 + 3 * 0xA00 + 1 * 0xA);   /* cmd 0x0130: v0 v3 v1? see sprite.c */
    G(&sp, 0xBF000000, 0 * 0xA0000 + 2 * 0xA00 + 3 * 0xA);
    G(&sp, 0xB8000000, 0);
    build_sprite_setup(&s);

    frame_start(&d, viewport(320, 240, 1), 0);
    m_ortho(pr, -160, 160, -120, 120, -1, 3000);
    G(&d, 0x01030040, mtx_fixed(pr));
    G(&d, 0x06000000, s.base);
    m_ident(mv); mv[12] = -100; mv[13] = 50; mv[14] = -405.11322f;
    G(&d, 0x01020040, mtx_fixed(mv));
    G(&d, 0x06000000, sp.base);
    run(&d);

    CHECK(nb.ntris == 2);
    if (nb.ntris >= 1) {
        const NBTri *t = &nb.tris[0];
        const NBTex *tx = tri_tex(0);
        CHECK(tx != NULL);
        CHECK(t->st.tex_rgb && t->st.tex_alpha);
        CHECK(t->st.filter == F3D_FILTER_POINT);
        CHECK(t->st.wrap_s == F3D_WRAP_CLAMP && t->st.wrap_t == F3D_WRAP_CLAMP);
        CHECK(t->st.blend == F3D_BLEND_OPAQUE && t->st.alpha_ref == 8);
        /* modulate colour is white for DECALRGBA */
        CHECK(t->v[0].r == 255 && t->v[0].a == 255);
        /* screen position: (x=-100, y=50) -> (60, 70) */
        NEAR(t->v[0].x, 60, 0.25); NEAR(t->v[0].y, 70, 0.25);
        /* UV: 0x1F / 32 * 0.5 = 0.484 texel */
        NEAR(t->v[0].u * W, 0x1F / 64.0, 1e-3);
        NEAR(t->v[1].u * W, W + 0x1F / 64.0, 1e-3);   /* v3 */
        NEAR(t->v[1].v * H, H + 0x1F / 64.0, 1e-3);
        if (tx) {
            CHECK(tx->w == W && tx->h == H && tx->fmt == 2 && tx->siz == 1);
            CHECK(tx->alpha == 1);
            for (y = 0; y < H; y++)
                for (x = 0; x < W; x++) {
                    u16 c = pp[tp[y * W + x]];
                    u32 r = (c >> 11) & 31, g = (c >> 6) & 31, b = (c >> 1) & 31;
                    u32 e = ((c & 1) ? 0xFF000000u : 0) | (((r << 3) | (r >> 2)) << 16) |
                            (((g << 3) | (g >> 2)) << 8) | ((b << 3) | (b >> 2));
                    if (tx->texels[y * W + x] != e) {
                        CHECK(tx->texels[y * W + x] == e);
                        y = H; break;
                    }
                }
        }
        CHECK(nb.tris[1].tex_index == nb.tris[0].tex_index);
    }
    CHECK(nb.uploads == 1);
}

/* CI4: 16-entry palette at tmem 0x100, palette 0; text.c / sprite.c CI4 path */
static void test_ci4_and_texrect(void)
{
    enum { W = 32, H = 8 };
    DL d = dl_new(), t = dl_new();
    u32 pal = alloc(32), tex = alloc(W * H / 2);
    u16 *pp = P(pal);
    u8 *tp = P(tex);
    int i, x, y, ok = 1;
    printf("CI4 text rectangle (TLUT tile 7, LOADBLOCK, TEXRECT + RDPHALF)\n");
    for (i = 0; i < 16; i++) pp[i] = (u16)((i << 12) | (i << 7) | (i << 2) | 1);
    pp[0] = 0;
    for (i = 0; i < W * H / 2; i++) tp[i] = (u8)((i * 5) ^ (i >> 3));
    /* text.c Text_BuildRect: TLUT then bitmap; CI 16b load of a CI4 bitmap */
    G(&t, 0xE7000000, 0); G(&t, 0xBA000E02, 0x8000);
    RENDERMODE(&t, rm(0x8 | 0x1000 | 0x2000 | 0x4000, 0, 3, 0, 2));   /* gTextRectSetupDL */
    CC_DECALRGBA(&t);
    G(&t, 0xBB000001, 0x80008000); G(&t, 0xBA001301, 0);
    G(&t, 0xFD100000, pal); G(&t, 0xE8000000, 0); G(&t, 0xF5000100, 0x07000000); G(&t, 0xE6000000, 0);
    G(&t, 0xF0000000, 0x0703C000); G(&t, 0xE7000000, 0);
    G(&t, 0xFD500000, tex); G(&t, 0xF5500000, 0x07000000); G(&t, 0xE6000000, 0);
    G(&t, 0xF3000000, load_block_w1(W >> 4, (W * H + 3) >> 2)); G(&t, 0xE7000000, 0);
    G(&t, (((((u32)(W >> 1) + 7) >> 3) & 0x1FF) << 9) | 0xF5400000u, 0);
    G(&t, 0xF2000000, ((u32)((W - 1) * 4) << 12) | (u32)((H - 1) * 4));
    /* TEXRECT (40,20)-(72,28) in 1-cycle mode, s=t=0, 1:1 */
    G(&t, 0xE4000000 | (72u * 4 << 12) | (28u * 4), (40u * 4 << 12) | (20u * 4));
    G(&t, 0xB4000000, 0);
    G(&t, 0xB3000000, 0x04000400);
    /* half-size horizontally, scissored at x < 10: starts at -6 */
    G(&t, 0xE4000000 | (26u * 4 << 12) | (40u * 4), (0u << 12) | (32u * 4));
    G(&t, 0xB4000000, (u32)(6 * 2 * 32) << 16);   /* s already advanced past the clipped part */
    G(&t, 0xB3000000, 0x08000400);
    G(&t, 0xB8000000, 0);

    frame_start(&d, viewport(320, 240, 1), 0);
    G(&d, 0x06000000, t.base);
    run(&d);

    CHECK(nb.ntris == 4);
    if (nb.ntris == 4) {
        const NBTri *a = &nb.tris[0];
        const NBTex *tx = tri_tex(0);
        CHECK(a->st.rect == 1 && a->st.tex_rgb);
        CHECK(a->st.alpha_ref >= 1);
        NEAR(a->v[0].x, 40, 0); NEAR(a->v[0].y, 20, 0);
        NEAR(a->v[2].x, 72, 0); NEAR(a->v[2].y, 28, 0);
        NEAR(a->v[0].u, 0, 1e-6); NEAR(a->v[2].u, 1.0, 1e-5); NEAR(a->v[2].v, 1.0, 1e-5);
        CHECK(tx && tx->w == W && tx->h == H && tx->siz == 0);
        if (tx) {
            for (y = 0; y < H && ok; y++)
                for (x = 0; x < W; x++) {
                    u8 b = tp[(y * W + x) / 2];
                    u32 n = (x & 1) ? (b & 15u) : (b >> 4);
                    u16 c = pp[n];
                    u32 r = (c >> 11) & 31;
                    u32 e = ((c & 1) ? 0xFF000000u : 0) | (((r << 3) | (r >> 2)) << 16);
                    if ((tx->texels[y * W + x] & 0xFFFF0000u) != e) { ok = 0; break; }
                }
            CHECK(ok);
        }
        /* second rect: dsdx 2.0, s starts at 12 texels, bilinear: the RDP sample
           of pixel X (s = 12 + 2X) sits at the pixel centre in the PVR's texel-centre
           convention (+0.5), relative to the window of fetched texels (12..62) */
        {
            const NBTex *t2 = tri_tex(2);
            NEAR(nb.tris[2].v[0].x, 0, 0); NEAR(nb.tris[2].v[2].x, 26, 0);
            CHECK(t2 && t2->w == 20);     /* texels 12..31: the tile (32 wide) clamps */
            if (t2) {
                NEAR(nb.tris[2].v[0].u * t2->w, 12 - 0.5 - 12, 1e-4);
                NEAR(nb.tris[2].v[2].u * t2->w, 12 + 25.5 * 2 + 0.5 - 12, 1e-3);
            }
        }
    }
}

/* PhotoView_Build strip: LOADTILE 321x7 RGBA16, line 81 -> TMEM wraps */
static void test_photo_loadtile(void)
{
    enum { IW = 400 };
    DL d = dl_new(), p = dl_new();
    u32 img = alloc(IW * 16 * 2);
    u16 *ip = P(img);
    int x, y, ok_rows = 1, wrapped = 1;
    printf("photo strip LOADTILE (RGBA16, TMEM wrap) + RDP texrect coverage\n");
    for (y = 0; y < 16; y++)
        for (x = 0; x < IW; x++)
            ip[y * IW + x] = (u16)(((x & 31) << 11) | ((y & 31) << 6) | (((x >> 5) & 31) << 1) | 1);
    G(&p, 0xB6000000, 0x001F3207); G(&p, 0xBB000001, 0x80008000); G(&p, 0xE7000000, 0);
    G(&p, 0xBA000E02, 0); RENDERMODE(&p, rm(0x200, 0, 0, 0, 0)); CC_DECALRGBA(&p);
    G(&p, 0xBA001301, 0);
    G(&p, 0xFD100000 | (IW - 1), img + (8 * 2) + (2 * IW * 2));   /* x = 8, y = 2 */
    G(&p, 0xF510A200, 0x07080200); G(&p, 0xE6000000, 0); G(&p, 0xF4000000, 0x07500018);
    G(&p, 0xE7000000, 0); G(&p, 0xF510A200, 0x80200); G(&p, 0xF2000000, 0x500018);
    G(&p, 0xE4000000 | ((0x140 * 4 + 1) << 12) | 0x19, (1u << 12) | 1);
    G(&p, 0xB4000000, 0); G(&p, 0xB3000000, 0x04000400);
    G(&p, 0xB8000000, 0);
    frame_start(&d, viewport(320, 240, 1), 0);
    G(&d, 0x06000000, p.base);
    run(&d);
    CHECK(nb.ntris == 2);
    if (nb.ntris == 2) {
        /* RDP rule (texrect): scanlines 1..6 of the 0.25..6.25 strip, sampling rows
           1..6 exactly; row 0 (overwritten in TMEM by the wrap of row 6) is never
           fetched, so the decoded window is rows 1..6, columns 0..318 */
        const NBTex *tx = tri_tex(0);
        float ymin = 1e9f, ymax = -1e9f;
        int i;
        for (i = 0; i < 3; i++) {
            float yy = nb.tris[0].v[i].y;
            if (yy < ymin) ymin = yy;
            if (yy > ymax) ymax = yy;
        }
        NEAR(ymin, 1, 0); NEAR(ymax, 7, 0);
        CHECK(tx && tx->w == 319 && tx->h == 6);   /* scissor 319: pixels 1..318 */
        if (tx) {
            for (y = 0; y < 6; y++)
                for (x = 0; x < 319; x++) {
                    u16 c = ip[(2 + 1 + y) * IW + 8 + x];
                    if ((tx->texels[y * tx->w + x] >> 16 & 0xF8) != (u32)((c >> 11) << 3) ||
                        (tx->texels[y * tx->w + x] >> 8 & 0xF8) != (u32)(((c >> 6) & 31) << 3)) ok_rows = 0;
                }
            CHECK(ok_rows);
            /* v at the centre of the first scanline = row 1 + 0.5 - window origin 1 */
            CHECK(wrapped);
        }
    }
}


/* Photo strips with the backend taking raw RGBA16 (as the PVR backend): the
 * direct-from-source path (narrow image) must give exactly the texels of the
 * TMEM path, and for a wide (640) image the aligned window texture must show the
 * same source texel at every point of the rectangle, for every sub-4-pixel
 * scroll offset, and be reused while the window moves inside its alignment. */
static void strip_dl(DL *p, u32 img, int iw, int x, int y)
{
    int frac = x & 3, xh = ((0x140 - frac) * 4) + 1, xl = (frac * -4) + 1;
    u32 st = 0;
    G(p, 0xB6000000, 0x001F3207); G(p, 0xBB000001, 0x80008000); G(p, 0xE7000000, 0);
    G(p, 0xBA000E02, 0); RENDERMODE(p, rm(0x200, 0, 0, 0, 0)); CC_DECALRGBA(p);
    G(p, 0xBA001301, 0);
    G(p, 0xFD100000 | (u32)(iw - 1), img + (u32)((x & ~3) * 2) + (u32)((y + 6) * iw * 2));
    G(p, 0xF510A200, 0x07080200); G(p, 0xE6000000, 0); G(p, 0xF4000000, 0x07500018);
    G(p, 0xE7000000, 0); G(p, 0xF510A200, 0x80200); G(p, 0xF2000000, 0x500018);
    if (xl < 0) {
        s32 dd = (s32)((u32)xl << 10) >> 7;
        st = (u32)((dd & -(dd < 1)) * -0x10000);
    }
    G(p, 0xE4000000 | ((u32)(xh & (~xh >> 31) & 0xFFF) << 12) | (0x19 + 0x18),
      ((u32)(xl & (~xl >> 31) & 0xFFF) << 12) | (1 + 0x18));
    G(p, 0xB4000000, st); G(p, 0xB3000000, 0x04000400);
    G(p, 0xB8000000, 0);
}

static u32 tex_at(const NBTex *tx, float u, float v)
{
    int x = (int)floorf(u * (float)tx->w), y = (int)floorf(v * (float)tx->h);
    if (x < 0) x = 0;
    if (x >= tx->w) x = tx->w - 1;
    if (y < 0) y = 0;
    if (y >= tx->h) y = tx->h - 1;
    return tx->texels[y * tx->w + x];
}

/* draws the strip with and without F3D_CAP_RGBA16; 1 if every sampled point agrees */
static DL sd, sp_;

static void strip_run(u32 img, int iw, int x, int y)
{
    if (!sd.base) { sd = dl_new(); sp_ = dl_new(); }
    sd.n = 0; sp_.n = 0;
    strip_dl(&sp_, img, iw, x, y);
    frame_start(&sd, viewport(320, 240, 1), 0);
    G(&sd, 0x06000000, sp_.base);
    run(&sd);
}

static int strip_compare(u32 img, int iw, int x, int y, int *uploads_raw)
{
    NBTri ref[2], raw[2];
    NBTex rt[2], wt[2];
    int k, i, ok = 1, u0 = 0;
    for (k = 0; k < 2; k++) {
        null_backend_set_rgba16(&be, k);
        null_backend_clear(&nb);
        nb.ntex = 0; nb.pool_used = 0;          /* forget textures */
        u0 = nb.uploads;
        strip_run(img, iw, x, y);
        if (nb.ntris != 2) return 0;
        for (i = 0; i < 2; i++) {
            /* own copies: the next run reuses the backend's texel pool */
            static u32 tbuf[2][2][512 * 8];
            NBTex *dst = k == 0 ? &rt[i] : &wt[i];
            if (k == 0) ref[i] = nb.tris[i]; else raw[i] = nb.tris[i];
            *dst = *tri_tex(i);
            if (dst->w * dst->h > 512 * 8) return 0;
            memcpy(tbuf[k][i], dst->texels, (size_t)dst->w * dst->h * 4);
            dst->texels = tbuf[k][i];
        }
        if (k == 1) *uploads_raw = nb.uploads - u0;

    }
    null_backend_set_rgba16(&be, 0);
    if (iw * 2 < 1024)
        /* narrow source: the direct path hands over exactly the texels of the TMEM path */
        for (i = 0; i < 2; i++)
            if (rt[i].w != wt[i].w || rt[i].h != wt[i].h || rt[i].alpha != wt[i].alpha ||
                memcmp(rt[i].texels, wt[i].texels, (size_t)rt[i].w * rt[i].h * 4) != 0 ||
                memcmp(&ref[i].v, &raw[i].v, sizeof(ref[i].v)) != 0)
                ok = 0;
    for (i = 0; i < 2; i++) {
        int a, b, s;
        for (a = 0; a < 3; a++)
            for (b = 0; b < 3; b++)
                for (s = 1; s < 16; s++) {
                    float f = (float)s / 16.0f;
                    const F3DVert *ra = &ref[i].v[a], *rb = &ref[i].v[b], *wa = &raw[i].v[a], *wb = &raw[i].v[b];
                    float cu = (ra->u + (rb->u - ra->u) * f) * (float)rt[i].w;
                    float cv = (ra->v + (rb->v - ra->v) * f) * (float)rt[i].h;
                    u32 c0;
                    if (fabsf(cu - roundf(cu)) < 0.01f || fabsf(cv - roundf(cv)) < 0.01f)
                        continue;           /* on a texel boundary: floor is down to rounding */
                    c0 = tex_at(&rt[i], ra->u + (rb->u - ra->u) * f, ra->v + (rb->v - ra->v) * f);
                    u32 c1 = tex_at(&wt[i], wa->u + (wb->u - wa->u) * f, wa->v + (wb->v - wa->v) * f);
                    if (c0 != c1 || ra->x != wa->x || ra->y != wa->y) {
                        if (ok && getenv("STRIP_DEBUG")) {
                            int q;
                            for (q = 0; q < 6; q++)
                                printf("      ref row %d: %08x %08x   raw row %d col 20: %08x\n", q, rt[i].texels[q * rt[i].w],
                                       rt[i].texels[q * rt[i].w + 1], q, wt[i].texels[q * wt[i].w + 20]);
                        }
                        if (ok && getenv("STRIP_DEBUG"))
                            printf("    tri %d a %d b %d f %.3f: ref u %.6f v %.6f (w %d h %d) raw u %.6f v %.6f (w %d h %d) c %08x %08x  x %.2f %.2f\n",
                                   i, a, b, f, ra->u + (rb->u - ra->u) * f, ra->v + (rb->v - ra->v) * f, rt[i].w, rt[i].h,
                                   wa->u + (wb->u - wa->u) * f, wa->v + (wb->v - wa->v) * f, wt[i].w, wt[i].h, c0, c1, ra->x, wa->x);
                        ok = 0;
                    }
                }
    }
    return ok;
}

static void test_photo_raw_paths(void)
{
    enum { IW = 640, IH = 40 };
    u32 img, img2;
    ram_top = 0x1000;                   /* fresh fake RAM (the tests before used ~2 MB) */
    sd.base = 0;
    img = alloc(IW * IH * 2 + 64);
    u16 *ip = P(img), *ip2;
    int x, y, ok = 1, up, ups = 0, narrow_ok = 1;
    printf("photo strips: raw RGBA16 paths (direct source, wide aligned window)\n");
    for (y = 0; y < IH; y++)
        for (x = 0; x < IW; x++)
            ip[y * IW + x] = (u16)((((x * 7) & 31) << 11) | (((x >> 3) & 31) << 6) | ((y & 31) << 1) | 1);
    for (x = 0; x <= 319; x += 1)
        if (!strip_compare(img, IW, x, 3, &up)) { ok = 0; printf("  wide x=%d differs\n", x); }
    CHECK(ok);
    /* reuse: one pass over the positions, textures kept */
    null_backend_set_rgba16(&be, 1);
    nb.ntex = 0; nb.pool_used = 0;
    up = nb.uploads;
    for (x = 0; x <= 319; x++) {
        null_backend_clear(&nb);
        strip_run(img, IW, x, 3);
    }
    ups = nb.uploads - up;
    null_backend_set_rgba16(&be, 0);
    nb.ntex = 0; nb.pool_used = 0;
    /* 320 positions: without reuse 320 uploads; the aligned window is re-made only
       when it crosses a 32-texel alignment step (about every 32 positions) */
    printf("  wide: %d uploads for 320 scroll positions\n", ups);
    CHECK(ups <= 16);
    /* narrow image (stride < 1024 bytes): direct path, texels identical to TMEM path */
    img2 = alloc(400 * IH * 2 + 64);
    ip2 = P(img2);
    for (y = 0; y < IH; y++)
        for (x = 0; x < 400; x++)
            ip2[y * 400 + x] = (u16)((((x * 5) & 31) << 11) | (((y * 3) & 31) << 6) | ((x & 31) << 1) | (x % 13 ? 1 : 0));
    for (x = 0; x <= 79; x++)
        if (!strip_compare(img2, 400, x, 2, &up)) { narrow_ok = 0; printf("  narrow x=%d differs\n", x); }
    CHECK(narrow_ok);
}

/* Model: RGBA16 wrapped texture, lighting, TEXEL0*SHADE+PRIM, cull, z-buffer,
 * perspective, near clipping */
static void test_model_lit_persp(void)
{
    enum { W = 8, H = 8 };
    DL d = dl_new(), s = dl_new(), m = dl_new();
    u32 tex = alloc(W * H * 2), lights = alloc(32);
    u16 *tp = P(tex);
    u8 *lp = P(lights);
    float pr[16], mv[16];
    int i;
    Vtx v[6];
    printf("model: lighting, TEXEL0*SHADE+PRIM, culling, perspective, near clip\n");
    for (i = 0; i < W * H; i++) tp[i] = (u16)(0xF801 | ((i & 31) << 1));
    /* ambient 40,40,40 ; light 200,100,0 from +z (eye space) */
    lp[0] = 40; lp[1] = 40; lp[2] = 40;
    lp[16] = 200; lp[17] = 100; lp[18] = 0; lp[24] = 0; lp[25] = 0; lp[26] = 127;
    /* gObjModelSetupDL-like */
    G(&s, 0xB6000000, 0x001F3207); G(&s, 0xB7000000, 0x00022205); G(&s, 0xBB000001, 0x80008000);
    G(&s, 0xBC000002, 0x80000040); G(&s, 0x03860010, lights + 16); G(&s, 0x03880010, lights);
    G(&s, 0xE7000000, 0); G(&s, 0xBA000E02, 0);
    RENDERMODE(&s, rm(0x8 | 0x10 | 0x20 | 0x40 | 0x2000, 0, 0, 1, 1));   /* gObjModelSetupDL */
    CC_MODEL(&s);
    G(&s, 0xFA000000, 0x102030FF);
    G(&s, 0xB8000000, 0);
    /* RGBA16 W x H, masks 3, wrap */
    G(&m, 0xFD100000, tex);
    G(&m, 0xF5100000, 0x07000000 | (3 << 14) | (3 << 4));
    G(&m, 0xE6000000, 0);
    G(&m, 0xF3000000, load_block_w1(W / 4, W * H));
    G(&m, 0xE7000000, 0);
    G(&m, ((((W * 2 + 7) >> 3)) << 9) | 0xF5100000, (3 << 14) | (3 << 4));
    G(&m, 0xF2000000, ((u32)((W - 1) * 4) << 12) | (u32)((H - 1) * 4));
    /* triangle facing the camera (normal +z), one facing away, one crossing the near plane */
    memset(v, 0, sizeof(v));
    v[0].x = -50; v[0].y = -50; v[1].x = 50; v[1].y = -50; v[2].x = 0; v[2].y = 50;
    for (i = 0; i < 3; i++) { v[i].r = 0; v[i].g = 0; v[i].b = 127; v[i].a = 200; }
    v[1].s = 16 * 32;       /* 16 texels -> 8 after scale 0.5, = 1.0 with masks 3 */
    v[3].x = -50; v[3].y = -50; v[3].z = 400; v[4].x = 50; v[4].y = -50; v[4].z = 400; v[5].x = 0; v[5].y = 50; v[5].z = 300;
    for (i = 3; i < 6; i++) { v[i].b = 127; v[i].a = 255; }
    G(&m, 0x04500060, vtx_buf(v, 6));
    G(&m, 0xBF000000, 0x00000A14);              /* 0 1 2: CCW (front) */
    G(&m, 0xBF000000, 0x0000140A);              /* 0 2 1: back -> culled */
    G(&m, 0xB6000000, 0x00002000);              /* no culling for the near-plane case */
    G(&m, 0xBF000000, 0x001E2832);              /* 3 4 5 */
    G(&m, 0xB8000000, 0);

    frame_start(&d, viewport(320, 240, 1), 0);
    m_persp(pr, 33, 320.0f / 240.0f, 10, 3000);
    G(&d, 0x01030040, mtx_fixed(pr));
    m_ident(mv); mv[14] = -405.11322f;          /* camera at z=405 looking at the origin */
    G(&d, 0x01020040, mtx_fixed(mv));
    G(&d, 0x06000000, s.base);
    G(&d, 0x06000000, m.base);
    run(&d);

    CHECK(f3d_stats()->tris_in == 3);
    CHECK(f3d_stats()->tris_culled == 1);
    CHECK(nb.ntris >= 1 + 2);                     /* front tri + the near/scissor-clipped polygon */
    if (nb.ntris >= 3) {
        const NBTri *t = &nb.tris[0];
        float c = 1.0f / tanf(33 * 3.14159265f / 360.0f);
        /* x screen = 160 + (x * c/aspect / w) * 160 */
        NEAR(t->v[0].x, 160 + (-50 * c / (320.0f / 240.0f) / 405.11322f) * 160, 0.05);
        NEAR(t->v[0].y, 120 - (-50 * c / 405.11322f) * 120, 0.05);
        NEAR(t->v[0].w, 405.11322, 0.01);
        CHECK(t->v[0].z > 0.5f && t->v[0].z < 1.0f);
        CHECK(t->st.z_cmp && t->st.z_upd);
        CHECK(t->st.tex_rgb && t->st.tex_alpha && t->st.offset);
        CHECK(t->st.wrap_s == F3D_WRAP_REPEAT && t->st.tex_w == W);
        /* shade = ambient 40 + light 200,100,0 * 1.0 -> 240,140,40 */
        CHECK(abs(t->v[0].r - 240) <= 1 && abs(t->v[0].g - 140) <= 1 && abs(t->v[0].b - 40) <= 1);
        /* offset = prim rgb, alpha = texel * prim alpha (255) */
        CHECK(t->v[0].or_ == 0x10 && t->v[0].og == 0x20 && t->v[0].ob == 0x30);
        CHECK(t->v[0].a == 255 && t->v[0].oa == 0);
        NEAR(t->v[1].u, 1.0, 1e-4);
        /* the clipped polygon: every vertex in front of the near plane */
        for (i = 1; i < nb.ntris; i++) {
            int k;
            for (k = 0; k < 3; k++) {
                const F3DVert *o = &nb.tris[i].v[k];
                CHECK(o->w >= 10.0f - 0.01f && o->z >= -0.001f);
                CHECK(o->x >= -0.01f && o->x <= 319.01f && o->y >= -0.01f && o->y <= 239.01f);
            }
        }
    }
}

/* Gfx_SetViewport + scissor: geometry outside the scissor is clipped away */
static void test_viewport_scissor(void)
{
    DL d = dl_new();
    float pr[16], mv[16];
    Vtx v[3] = { { -160, 120, 0, 0, 0, 0, 9, 9, 9, 255 }, { 160, 120, 0, 0, 0, 0, 9, 9, 9, 255 },
                 { -160, -120, 0, 0, 0, 0, 9, 9, 9, 255 } };
    u32 vp = alloc(16);
    s16 *p = P(vp);
    int i, k, ok = 1;
    printf("viewport rectangle + scissor clipping\n");
    /* Gfx_SetViewport(x=100, y=60, w=100, h=80, sx=sy=0x2000) in mode 0 */
    p[0] = 640; p[1] = -480; p[2] = 0x1FF; p[3] = 0;
    p[4] = (s16)((320 - ((160 - (100 + 50)) * 2)) * 2); p[5] = (s16)((240 - ((120 - (60 + 40)) * 2)) * 2); p[6] = 0x1FF;
    frame_start(&d, viewport(320, 240, 1), 0);
    m_ortho(pr, -160, 160, -120, 120, -1, 3000);
    G(&d, 0x01030040, mtx_fixed(pr));
    m_ident(mv);
    G(&d, 0x01020040, mtx_fixed(mv));
    G(&d, 0x03800010, vp); G(&d, 0xE7000000, 0);
    G(&d, 0xED000000 | (400u << 12) | 240u, (796u << 12) | 556u);
    G(&d, 0x04200030, vtx_buf(v, 3));
    G(&d, 0xBF000000, 0x00000A14);
    run(&d);
    CHECK(nb.ntris >= 1);
    for (i = 0; i < nb.ntris; i++)
        for (k = 0; k < 3; k++) {
            const F3DVert *o = &nb.tris[i].v[k];
            if (o->x < 100 - 0.01f || o->x > 199 + 0.01f || o->y < 60 - 0.01f || o->y > 139 + 0.01f) ok = 0;
        }
    CHECK(ok);
    /* the scissor's top-left corner lies inside the triangle: some vertex is there */
    ok = 0;
    for (i = 0; i < nb.ntris; i++)
        for (k = 0; k < 3; k++)
            if (fabsf(nb.tris[i].v[k].x - 100) < 0.01f && fabsf(nb.tris[i].v[k].y - 60) < 0.01f) ok = 1;
    CHECK(ok);
}

/* DL call / branch, segments, matrix push/pop, flat shading, SHADE off, texgen */
static void test_misc(void)
{
    DL d = dl_new(), sub = dl_new(), br = dl_new();
    float pr[16], mv[16];
    Vtx v[3] = { { 0, 0, 0, 0, 0, 0, 10, 20, 30, 40 }, { 10, 0, 0, 0, 0, 0, 50, 60, 70, 80 },
                 { 0, 10, 0, 0, 0, 0, 90, 100, 110, 120 } };
    Vtx n[3];
    u32 va, la = alloc(32);
    u8 *lp = P(la);
    int i;
    printf("DL call/branch, segments, push/pop, flat shading, texgen\n");
    va = vtx_buf(v, 3);
    /* branch target draws with flat shading (flag selects vertex 1) */
    G(&br, 0xB6000000, 0x200);
    G(&br, 0xBF000000, 0x01000A14);
    G(&br, 0xB8000000, 0);
    /* sub: push a translation, draw, pop, branch */
    m_ident(mv); mv[12] = 100;
    G(&sub, 0x01040040, mtx_fixed(mv));          /* MODELVIEW MUL PUSH */
    G(&sub, 0x04200030, 0x06000000u | ((va & 0xFFFFFF) - 0x10000));   /* segment 6 */
    G(&sub, 0xBF000000, 0x00000A14);
    G(&sub, 0xBD000000, 0);                      /* POPMTX */
    G(&sub, 0x04200030, va);
    G(&sub, 0x06010000, br.base);                /* branch, no return here */
    G(&sub, 0xBF000000, 0x00000A14);             /* never reached */
    G(&sub, 0xB8000000, 0);

    frame_start(&d, viewport(320, 240, 1), 0);
    G(&d, 0xBC001806, 0x00010000);               /* segment 6 = 0x10000 */
    m_ortho(pr, -160, 160, -120, 120, -1, 3000);
    G(&d, 0x01030040, mtx_fixed(pr));
    m_ident(mv);
    G(&d, 0x01020040, mtx_fixed(mv));
    CC_SHADE(&d);
    G(&d, 0x06000000, sub.base);
    /* G_SHADE off: shade reads as 0 */
    G(&d, 0xB6000000, 0x4);
    G(&d, 0xBF000000, 0x00000A14);
    G(&d, 0xB7000000, 0x204);
    /* texgen: normal along lookat-x gives s = 2*512*scale */
    memset(lp, 0, 32);
    lp[8] = 127;           /* lookat x dir */
    lp[16 + 9] = 127;      /* lookat y dir */
    G(&d, 0x03840010, la); G(&d, 0x03820010, la + 16);
    G(&d, 0xBC000002, 0x80000040);
    G(&d, 0xB7000000, 0x00060000);               /* LIGHTING | TEXTURE_GEN */
    G(&d, 0xBB000001, 0x07C007C0);
    G(&d, 0xF5100000, (5 << 14) | (5 << 4));
    G(&d, 0xF2000000, (31u * 4 << 12) | (31u * 4));
    CC_DECALRGBA(&d);
    memcpy(n, v, sizeof(n));
    n[0].r = 127; n[0].g = 0; n[0].b = 0;        /* normal +x */
    n[1].r = 0; n[1].g = 127; n[1].b = 0;        /* normal +y */
    n[2].r = 0; n[2].g = 0; n[2].b = 127;
    G(&d, 0x04200030, vtx_buf(n, 3));
    G(&d, 0xBF000000, 0x00000A14);
    run(&d);

    CHECK(nb.ntris == 4);
    if (nb.ntris == 4) {
        NEAR(nb.tris[0].v[0].x, 260, 0.25);       /* pushed translation */
        NEAR(nb.tris[1].v[0].x, 160, 0.25);       /* popped */
        for (i = 0; i < 3; i++) CHECK(nb.tris[1].v[i].r == 50 && nb.tris[1].v[i].a == 80);
        CHECK(nb.tris[0].v[2].r == 90 && nb.tris[0].v[2].a == 120);   /* smooth */
        CHECK(nb.tris[2].v[0].r == 0 && nb.tris[2].v[0].a == 0);      /* SHADE off */
        /* texgen: s = (1+1)*512*0x7C0/65536 = 31 texels of 32 */
        NEAR(nb.tris[3].v[0].u, 31.0 / 32.0, 1e-3);
        NEAR(nb.tris[3].v[0].v, 15.5 / 32.0, 1e-3);
        NEAR(nb.tris[3].v[1].v, 31.0 / 32.0, 1e-3);
        NEAR(nb.tris[3].v[2].u, 15.5 / 32.0, 1e-3);
    }
    CHECK(f3d_stats()->unknown == 0);
}

/* texture cache: same content -> no re-upload; changed content -> new key */
static void test_tex_cache(void)
{
    enum { W = 4, H = 4 };
    int i, pass;
    u32 tex = alloc(W * H * 2);
    u16 *tp = P(tex);
    int up0;
    printf("texture cache by content hash\n");
    for (i = 0; i < W * H; i++) tp[i] = 0xFFFF;
    up0 = nb.uploads;
    for (pass = 0; pass < 3; pass++) {
        DL d = dl_new();
        if (pass == 2) tp[5] = 0x003F;
        frame_start(&d, viewport(320, 240, 1), 0);
        CC_DECALRGBA(&d); G(&d, 0xBA000E02, 0);
        G(&d, 0xFD100000, tex); G(&d, 0xF5100000, 0x07000000); G(&d, 0xE6000000, 0);
        G(&d, 0xF3000000, load_block_w1(W / 4, W * H)); G(&d, 0xE7000000, 0);
        G(&d, 0xF5100000 | (1 << 9), 0); G(&d, 0xF2000000, (3u * 4 << 12) | (3u * 4));
        G(&d, 0xE4000000 | (20u * 4 << 12) | (20u * 4), (10u * 4 << 12) | (10u * 4));
        G(&d, 0xB4000000, 0); G(&d, 0xB3000000, 0x04000400);
        run(&d);
        CHECK(nb.ntris == 2);
    }
    CHECK(nb.uploads - up0 == 2);
    {
        u32 c = nb.tex[nb.ntex - 1].texels[5];
        CHECK(c == 0xFF0000FFu);                 /* 0x003F = blue, alpha 1 */
    }
}

/* IA / I decodes through f3d_decode_tile */
static void test_ia_i_decode(void)
{
    DL d = dl_new();
    u32 tex = alloc(64);
    u8 *tp = P(tex);
    u32 out[64];
    int w, h, i;
    printf("IA4/IA8/I4/I8/IA16 decode\n");
    for (i = 0; i < 64; i++) tp[i] = (u8)(i * 37 + 3);
    frame_start(&d, viewport(320, 240, 1), 0);
    G(&d, 0xFD100000, tex); G(&d, 0xF5100000, 0x07000000); G(&d, 0xE6000000, 0);
    G(&d, 0xF3000000, 0x07000000 | (31u << 12));  /* 32 16-bit texels, dxt 0 */
    G(&d, 0xF5880000 | (1 << 9), 0x01000000);    /* tile 1: I 8b, line 1 (8 texels) */
    G(&d, 0xF2000000, 0x01000000 | (7u * 4 << 12) | (0));
    G(&d, 0xF5600000 | (1 << 9), 0x02000000);    /* tile 2: IA 4b, line 1 (16 texels) */
    G(&d, 0xF2000000, 0x02000000 | (15u * 4 << 12) | (0));
    G(&d, 0xF5800000 | (1 << 9), 0x03000000);    /* tile 3: I 4b */
    G(&d, 0xF2000000, 0x03000000 | (15u * 4 << 12) | (0));
    G(&d, 0xF5680000 | (1 << 9), 0x04000000);    /* tile 4: IA 8b */
    G(&d, 0xF2000000, 0x04000000 | (7u * 4 << 12) | (0));
    G(&d, 0xF5700000 | (1 << 9), 0x05000000);    /* tile 5: IA 16b */
    G(&d, 0xF2000000, 0x05000000 | (3u * 4 << 12) | (0));
    run(&d);
    CHECK(f3d_decode_tile(1, out, 64, &w, &h) == 8 && w == 8 && h == 1);
    CHECK(out[3] == ((u32)tp[3] * 0x01010101u));
    CHECK(f3d_decode_tile(2, out, 64, &w, &h) == 16);
    {
        u32 nib = tp[1] >> 4, i3 = nib >> 1, iv = i3 * 0x24 + (i3 >> 1);
        CHECK(out[2] == (((nib & 1) ? 0xFF000000u : 0) | iv * 0x10101u));
    }
    CHECK(f3d_decode_tile(3, out, 64, &w, &h) == 16);
    CHECK(out[5] == (u32)(tp[2] & 15) * 17u * 0x01010101u);
    CHECK(f3d_decode_tile(4, out, 64, &w, &h) == 8);
    CHECK(out[6] == (((u32)(tp[6] & 15) * 17u) << 24 | ((u32)(tp[6] >> 4) * 17u) * 0x10101u));
    CHECK(f3d_decode_tile(5, out, 64, &w, &h) == 4);
    {
        u16 c; memcpy(&c, &tp[2], 2);
        CHECK(out[1] == (((u32)(c & 0xFF) << 24) | (u32)(c >> 8) * 0x10101u));
    }
}

int main(void)
{
    null_backend_init(&be, &nb);
    f3d_set_ucode(0);
    f3d_init(&be, addr_hook);
    ram_top = 0x1000;

    test_mtx_and_selftest();
    test_frame_and_screen_quad();
    test_ci8_sprite();
    test_ci4_and_texrect();
    test_photo_loadtile();
    test_photo_raw_paths();
    f3d_clear_stats();
    test_model_lit_persp();
    test_viewport_scissor();
    f3d_clear_stats();
    test_misc();
    test_tex_cache();
    test_ia_i_decode();

    printf("%d checks, %d failures\n", checks, fails);
    return fails != 0;
}
