/* Vivid Dolls - frame rendering, video mode, viewports, cabinet I/O, controller
 * pak / EEPROM wrappers and geometry helpers (RAM 0x8000F3E0-0x80015AA0).
 * Every function is checked against the original by tools/difftest/test_gfx.py.
 * See docs/notes/gfx.md.
 */
#include "gfx.h"
#include "rotate.h"
#include "collide.h"
#include "libm.h"

/* ------------------------------------------------------------------ externs */
/* libultra */
extern void func_80022048(PadData *pads);                      /* osContGetReadData */
extern s32  osSendMesg(void *mq, void *msg, s32 flag);
extern void osCreateMesgQueue(void *mq, void *msgs, s32 count);
extern u32  osVirtualToPhysical(void *p);
extern u32  osSetIntMask(u32 mask);
extern void osViSetMode(void *mode);
extern void osViBlack(u32 on);
extern void bcopy(const void *src, void *dst, s32 n);
extern void func_80021990(u32 features);                      /* osViSetSpecialFeatures */
extern void func_8001C990(void);                              /* osWritebackDCacheAll */
extern s32  func_80024360(void *mq, u8 *pattern);             /* osPfsIsPlug */
extern s32  func_80024A30(void *mq, void *pfs, s32 channel);  /* osPfsInitPak */
extern s32  func_800230D0(void *pfs, u16 company, u32 game, u8 *name, u8 *ext,
                          s32 length, s32 *file_no);          /* osPfsAllocateFile */
extern s32  osPfsFindFile(void *pfs, u16 company, u32 game, u8 *name, u8 *ext, s32 *file_no);
extern s32  func_80023B60(void *pfs, s32 file_no, u8 flag, s32 offset, s32 size, u8 *buf); /* osPfsReadWriteFile */
extern s32  func_800236C0(void *pfs, u16 company, u32 game, u8 *name, u8 *ext);      /* osPfsDeleteFile */
extern s32  osPfsChecker(void *pfs);
extern s32  func_80024850(void *mq);                          /* osEepromProbe */
extern s32  func_800249C0(void *mq, u8 addr, u8 *buf, s32 n); /* osEepromLongRead */
extern s32  func_800248D0(void *mq, u8 addr, u8 *buf, s32 n); /* osEepromLongWrite */

/* game code not decompiled here */
extern void func_8001BE98(s32 id);                            /* debug timing marker */
extern void func_8000B06C(void);
extern void Obj_UpdateAll(void);
extern void func_8000A924(s32 *out, MtxF m);                  /* MtxF -> fixed-point Mtx */

/* screen */
extern s32 gScreenWidth;                /* 0x800C30D0 */
extern s32 D_800C5330;                  /* screen height */
extern s32 D_800B0504;                  /* microcode / frame mode 0..3 */

/* Aleck64 cabinet I/O registers */
#define IO_REG16(a) (*(volatile u16 *)(a))
#define IO_PLAYERS  0x02000002          /* P1 low byte, P2 high byte (active low) */
#define IO_EXTRA    0x02000004          /* extra buttons, coin, test, service (active low) */
#define IO_DIPS     0x02000000          /* DIP switches (active low) */
#define IO_OUTPUT   0x0200000A          /* coin counters / lockouts */

/* ================================================================ controllers */
extern PadData D_800A67F8[4];
extern s16 D_800C30A0[4];               /* pad present */
extern s32 D_800A67E0, D_800A67E4;      /* last stick x / y (dead zone applied) */
extern s16 D_800B04AC[4];               /* stick x, -60..60 */
extern s16 D_800C5298[4];               /* stick y, -60..60 */
extern s32 D_800A6844;                  /* work: buttons + stick directions */
extern u16 D_800A6838[4];               /* previous buttons + stick directions */
extern u16 D_800C5334[4];               /* held (with stick directions) */
extern u16 D_800B0596[4];               /* pressed (with stick directions) */
extern u16 D_800B043A[4];               /* auto-repeat (with stick directions) */
extern u16 D_800B04A4[4];               /* auto-repeat timer */
extern u16 D_800A6830[4];               /* previous raw buttons */
extern u16 D_800B058C[4];               /* held raw buttons */
extern u16 D_800AF222[4];               /* pressed raw buttons */
extern u16 D_800AF238[4];               /* auto-repeat raw buttons */
extern u16 D_800C167A[4];               /* auto-repeat timer (raw) */

/* Stick value -> dead zone of 7, clamped to +-60. */
static s32 Pad_StickValue(s32 v)
{
    s32 a = v;
    if (a < 0) {
        a = -a;
    }
    a -= 7;
    a &= ~a >> 31;      /* max(a, 0) */
    if (a >= 61) {
        a = 60;
    }
    if (v < 0) {
        a = -a;
    }
    return a;
}

/* @ 0x8000F3E0 */
/* Read the 4 controllers: stick dead zone, stick as d-pad bits (0x800 up,
 * 0x400 down, 0x100 right, 0x200 left; the real d-pad bits are masked out),
 * pressed edges and 8-frame auto-repeat, for both the stick-mapped and the raw
 * buttons. A pad reporting an error reads as all zero. */
void Pad_Read(void)
{
    u16 i;

    func_80022048(D_800A67F8);
    for (i = 0; i < 4; i++) {
        PadData *p = &D_800A67F8[i];
        s32 v, cur;
        u16 trig;
        u32 raw;

        if (p->errnum != 0) {
            p->stick_x = 0;
            p->stick_y = 0;
            p->button = 0;
            D_800C30A0[i] = 0;
        } else {
            D_800C30A0[i] = 1;
        }

        v = Pad_StickValue(p->stick_x);
        D_800A67E0 = v;
        D_800B04AC[i] = v;
        v = Pad_StickValue(p->stick_y);
        D_800C5298[i] = v;
        D_800A67E4 = v;

        D_800A6844 = p->button & 0xF0FF;
        if (D_800C5298[i] >= 31) {
            D_800A6844 |= 0x800;
        } else if (D_800C5298[i] < -30) {
            D_800A6844 |= 0x400;
        }
        if (D_800B04AC[i] >= 31) {
            D_800A6844 |= 0x100;
        } else if (D_800B04AC[i] < -30) {
            D_800A6844 |= 0x200;
        }

        cur = D_800A6844;
        trig = (D_800A6838[i] ^ cur) & cur;
        D_800C5334[i] = cur;
        D_800B0596[i] = trig;
        D_800B043A[i] = trig;
        if (trig != 0) {
            D_800B04A4[i] = 8;
        }
        if (D_800B04A4[i] != 0) {
            D_800B04A4[i]--;
        } else {
            D_800B04A4[i] = 1;
            D_800B043A[i] = D_800C5334[i];
        }
        D_800A6838[i] = D_800A6844;

        raw = p->button;
        trig = raw & (raw ^ D_800A6830[i]);
        D_800B058C[i] = raw;
        D_800AF222[i] = trig;
        D_800AF238[i] = trig;
        if (trig != 0) {
            D_800C167A[i] = 8;
        }
        if (D_800C167A[i] != 0) {
            D_800C167A[i]--;
        } else {
            D_800C167A[i] = 1;
            D_800AF238[i] = D_800B058C[i];
        }
        D_800A6830[i] = p->button;
    }
}

extern u16 D_800C5304[4], D_800B0610[4], D_800B4A38[4], D_800C30D4[4];

/* @ 0x8000F6EC */
/* Clear four per-pad u16 tables. */
void Pad_ClearState(void)
{
    u16 i;
    for (i = 0; i < 4; i++) {
        D_800C5304[i] = 0;
        D_800B0610[i] = 0;
        D_800B4A38[i] = 0;
        D_800C30D4[i] = 0;
    }
}

extern u16 D_800A6840;                  /* 1 = serial interface free for the pad reader */
extern s32 D_800C51D0;                  /* SI ownership mode */
extern u8  D_800B0520[];                /* OSMesgQueue */
extern u8  D_800C5328[];                /* its OSMesg */
extern u8  D_800C30A8[];                /* SI OSMesgQueue */
extern u8  D_800A6810[];
extern volatile s32 D_800B4AA4;         /* pad read in progress */
extern s32 D_800B04D0;                  /* pad read pending */

/* @ 0x8000F750 */
u16 Si_IsReleased(void)
{
    return D_800A6840;
}

/* @ 0x8000F75C */
/* Give the serial interface back to the pad reader. */
void Si_Release(void)
{
    D_800A6840 = 1;
    if (D_800C51D0 == 2) {
        osSendMesg(D_800B0520, D_800C5328, 1);
    }
}

/* @ 0x8000F7A4 */
/* Take the serial interface (for pak / EEPROM access): wait for a running pad
 * read, clear pad state, finish a pending read. */
void Si_Acquire(void)
{
    u16 i;

    D_800A6840 = 0;
    while (D_800B4AA4 != 0) {
    }
    for (i = 0; i < 4; i++) {
        D_800C5304[i] = 0;
        D_800B0610[i] = 0;
        D_800B4A38[i] = 0;
        D_800C30D4[i] = 0;
    }
    if (D_800B04D0 != 0) {
        Pad_Read();
        D_800B04D0 = 0;
    }
    if (D_800C51D0 == 1) {
        osSendMesg(D_800C30A8, D_800A6810, 0);
    }
}

/* ================================================================ cabinet I/O */
extern s32 D_800B0428[2], D_800AF264[2];    /* credits per coin / coins per credit (mode 1) */
extern s32 D_800C51C4[2], D_800B056C[2];    /* credits per coin / coins per credit (mode 0) */
extern u16 D_800C1678;                      /* output mask */

/* @ 0x8000F890 */
/* 1 coin = 1 credit on both slots, all outputs enabled. */
void Coin_InitRates(void)
{
    s32 i;
    for (i = 0; i < 2; i++) {
        D_800B0428[i] = 1;
        D_800AF264[i] = 1;
        D_800C51C4[i] = 1;
        D_800B056C[i] = 1;
    }
    D_800C1678 = 0xFFFF;
}

extern u16 D_800B0498, D_800B049A;          /* held P1 / P2 */
extern u16 D_800C52A8, D_800C52AA;          /* previous held P1 / P2 */
extern u16 D_800C532C, D_800C532E;          /* pressed P1 / P2 */
extern u16 D_800B4A80, D_800B4A82;          /* auto-repeat P1 / P2 */
extern u16 D_800C52F0[2];                   /* auto-repeat timers */

/* @ 0x8000F8F4 */
/* Read the two cabinet players (8 bits each + one extra button each from the
 * extra port as bit 8), pressed edges and 8-frame auto-repeat. */
void Io_ReadButtons(void)
{
    u32 prev0 = D_800B0498;
    u32 prev1 = D_800B049A;
    u32 raw = ~IO_REG16(IO_PLAYERS);
    u32 p0 = raw & 0xFF;
    u32 p1 = (raw & 0xFFFF) >> 8;
    u32 ext = ~IO_REG16(IO_EXTRA);
    u32 t0, t1;

    D_800C52A8 = prev0;
    D_800C52AA = prev1;
    D_800B0498 = p0;
    p0 |= (ext & 1) << 8;
    D_800B049A = p1;
    p1 |= (ext & 2) << 7;
    t0 = p0 & (p0 ^ prev0);
    t1 = p1 & (p1 ^ prev1);
    D_800B0498 = p0;
    D_800B049A = p1;
    D_800C532C = t0;
    D_800C532E = t1;

    D_800B4A80 = t0;
    if (t0 != 0) {
        D_800C52F0[0] = 8;
    } else if (D_800C52F0[0] != 0) {
        D_800C52F0[0]--;
    } else {
        D_800C52F0[0] = 1;
        D_800B4A80 = p0;
    }

    D_800B4A82 = D_800C532E;
    if (D_800C532E != 0) {
        D_800C52F0[1] = 8;
    } else if (D_800C52F0[1] != 0) {
        D_800C52F0[1]--;
    } else {
        D_800C52F0[1] = 1;
        D_800B4A82 = D_800B049A;
    }
}

extern u16 D_800C52A0;

/* @ 0x8000FA34 */
void Io_ReadDipSwitches(void)
{
    D_800C52A0 = ~IO_REG16(IO_DIPS);
}

extern u16 D_800C0AB0, D_800B0438, D_800C1898;     /* SERVICE switch (input bit 4): now, previous, pressed */
extern u16 D_800C30C4, D_800C31B0, D_800AF220;     /* TEST switch (input bit 5): now, previous, pressed */
/* (names per the test mode's own labels; the locals below were named the other way round) */
extern u16 D_800C16E0;                  /* 1 = shared coin counter (all coins to slot 0) */
extern u16 D_800C0D4C[2];               /* coin switch now */
extern u16 D_800C16E2[2];               /* coin switch previous */
extern u16 D_800C53D8[2];               /* coin switch pressed */
extern u32 D_800B04F8[2];               /* coin switch hold time */
extern u16 D_800B0574[2];               /* coin jam (held 30+ frames) */
extern u16 D_800C1654[2];               /* coin counter pulses pending */
extern u32 D_800C16B8[2];               /* total coins (bookkeeping) */
extern u32 D_800C52F4[2];               /* coins not yet turned into credits */
extern s32 D_800B4A30[2];               /* coinage mode per slot */
extern s32 D_800C30C8[2];               /* credits */
extern u16 D_800B0274[2];               /* coin counter pulse timer */
extern u16 D_800C178C;                  /* fixed output bits */

/* @ 0x8000FA4C */
/* Coin slots, test and service switches; coins -> credits; drive the coin
 * counters (one pulse = 64 frames, bit 5 of the timer) and coin lockouts. */
void Coin_Update(void)
{
    u16 shared = D_800C16E0;
    u32 in = (u16)~IO_REG16(IO_EXTRA);
    u16 coin1 = (in >> 3) & 1;
    u16 coin0 = (in >> 2) & 1;
    u16 svc = (in >> 5) & 1;    /* actually the TEST switch */
    u16 test = (in >> 4) & 1;   /* actually the SERVICE switch */
    u16 prev;
    s32 i;
    u32 out;

    prev = D_800C0AB0;
    D_800B0438 = prev;
    D_800C0AB0 = test;
    prev = test & (test ^ prev);
    D_800C1898 = prev;
    prev = D_800C30C4;
    D_800C30C4 = svc;
    D_800C31B0 = prev;
    D_800AF220 = svc & (svc ^ prev);

    for (i = 0; i < 2; i++) {
        u16 edge;
        u32 c;

        D_800C16E2[i] = D_800C0D4C[i];
        if (i != 0) {
            D_800C0D4C[i] = coin1;
        } else {
            D_800C0D4C[0] = coin0;
        }
        edge = D_800C0D4C[i] & (D_800C0D4C[i] ^ D_800C16E2[i]);
        D_800C53D8[i] = edge;
        if (edge != 0) {
            D_800B04F8[i] = 1;
        }
        if (D_800C16E2[i] != 0) {
            if (D_800C0D4C[i] != 0) {
                if (++D_800B04F8[i] >= 30) {
                    D_800B0574[i] = 1;
                }
            } else if (D_800B04F8[i] >= 2) {
                if (D_800B04F8[i] >= 30) {
                    D_800B0574[i] = 1;
                } else {
                    /* switch released after 2..29 frames: one coin */
                    D_800C1654[i]++;
                    D_800C16B8[i]++;
                    if (shared == 0) {
                        D_800C52F4[i]++;
                    } else {
                        D_800C52F4[0]++;
                    }
                }
            }
        }

        c = D_800C52F4[i];
        if (c != 0) {
            if (D_800B4A30[i] != 0) {
                if (c >= (u32)D_800AF264[i]) {
                    D_800C30C8[i] += D_800B0428[i];
                    D_800C52F4[i] -= D_800AF264[i];
                }
            } else {
                if (c >= (u32)D_800B056C[i]) {
                    D_800C30C8[i] += D_800C51C4[i];
                    D_800C52F4[i] -= D_800B056C[i];
                }
            }
        }

        if (D_800B0274[i] != 0) {
            D_800B0274[i]--;
        } else if (D_800C1654[i] != 0) {
            D_800C1654[i]--;
            D_800B0274[i] = 0x3F;
        }
    }

    out = (D_800B0274[0] >> 5) & 1;
    if (D_800B0274[1] & 0x20) {
        out |= 2;
    }
    if (D_800B0574[0] == 0) {
        out |= 4;
    }
    if (D_800B0574[1] == 0) {
        out |= 8;
    }
    IO_REG16(IO_OUTPUT) = D_800C178C | (out & D_800C1678);
}

/* ================================================================ video */
extern u16 D_807B2600[0x12C00];         /* frame buffer 0 (640x240 or 320x480 x 16 bit) */
extern u16 D_807D7E00[0x12C00];         /* frame buffer 1 */
extern u32 D_8013B510[0x7000];          /* RDP output buffer */
extern s32 D_800B05A0;                  /* clear the frame buffer each frame */
extern u16 *D_800B28A8, *D_800B4A28;    /* frame buffer pointers */
extern u16 D_800B2888, D_800B4A08;
extern u32 D_8013A800, D_8013A804;
extern s32 D_800C1788, D_800C53D0;      /* near / far clip */
extern s32 D_800C53F8, D_800C1650;      /* D_800C1650: frame counter */
extern u8  D_800B0594, D_800B0618, D_800B0724;  /* clear colour b, g, r */
extern u32 osViModeTable[];
extern u32 D_800A6850[20];              /* OSViMode in use */
extern u32 D_800A68A8[20];              /* saved OSViMode */
extern u32 *D_800A68A0;                 /* current OSViMode */
extern u8  D_8003B110[];                /* initial OSViMode (ROM data) */
extern s32 D_800386C0, D_800386C4;      /* horizontal / vertical screen position adjust */

/* @ 0x8000FDB0 */
/* Clear both frame buffers and the RDP output buffer, set default screen
 * size (320x240), clip planes and video mode, then blank the screen. */
void Video_Init(void)
{
    s32 i;

    for (i = 0; i <= 0x12BFF; i++) {
        D_807B2600[i] = 0;
    }
    for (i = 0; i <= 0x12BFF; i++) {
        D_807D7E00[i] = 0;
    }
    D_800B05A0 = 1;
    D_800B28A8 = D_807B2600;
    D_800B0504 = 0;
    D_800B2888 = 2;
    D_800B4A08 = 2;
    D_800B4A28 = D_807D7E00;
    for (i = 0; i < 0x7000; i++) {
        D_8013B510[i] = 0;
    }
    D_8013A800 = 0;
    D_8013A804 = 0x1C000;
    gScreenWidth = 320;
    D_800C5330 = 240;
    D_800C1788 = 10;
    D_800C53F8 = 0;
    D_800C1650 = 0;
    D_800B0594 = 0;
    D_800B0618 = 0;
    D_800B0724 = 0;
    D_800C53D0 = 3000;
    for (i = 0; i < 20; i++) {
        D_800A68A8[i] = osViModeTable[i];
    }
    D_800A68A0 = osViModeTable;
    bcopy(D_8003B110, D_800A6850, 0x50);
    Video_ApplyMode();
    osViBlack(1);
}

/* N64 Vp */
typedef struct { s16 vscale[4]; s16 vtrans[4]; } Vp;
extern Vp D_800A68F8;                   /* full-screen viewport */
extern s32 D_800A6908;                  /* viewports used this frame */
extern Vp D_800A6910[8];

extern SpriteGfx *D_800B053C;           /* display list head */
extern s32 D_800B0540;
extern s32 D_800386C8;                  /* run D_80038560 once */
extern SpriteGfx D_800386A0[], D_80038560[], D_80038648[];
extern u8  D_80115000[];                /* Z buffer */
extern MtxF D_800B05B8;                 /* perspective MtxF */
extern f32 gCameraFov;
extern Vec3f gCameraEye, gCameraTarget, gCameraUp;
extern MtxF D_800C16E8;                 /* view MtxF */
extern s32 D_800B0548;                  /* debug: identity view */
extern SceneNode *gSceneGroup1;
extern SceneNode *gSceneRoot0, *gSceneRoot1, *gSceneRoot2, *gSceneRoot3;
extern SceneLight D_800C1730;
extern u8 D_80033FA0[], D_80034070[];   /* rspboot */
extern u8 D_80037210[], D_8003CA50[];   /* microcode 0 text / data */
extern u8 D_80034E90[], D_8003BA50[];   /* microcode 2 */
extern u8 D_80035E10[], D_8003C250[];   /* microcode 3 */
extern u8 D_80173510[], D_8013B110[], D_8013A810[];
extern u8 D_800C30E0[];                 /* task done queue */
extern void *D_800B0568;                /* scheduler queue */

#define GFX(w0_, w1_) do { SpriteGfx *g_ = D_800B053C++; g_->w0 = (w0_); g_->w1 = (w1_); } while (0)

/* @ 0x8000FF64 */
/* Build and send the frame's graphics task: segment 0, RSP/RDP init lists,
 * viewport, clear Z buffer (and optionally the frame buffer), set up
 * ortho / perspective projection, camera LookAt and light, update the four
 * scene trees and emit them (roots 0, 2, 3 with ortho, root 1 with perspective),
 * fill in the OSTask for the selected microcode and post it. */
void Gfx_RenderFrame(GfxTask *task)
{
    SpriteGfx *start;
    SpriteGfx *g;
    MtxF m;
    f32 fw, fh;
    u32 norm;
    u32 c;

    if (D_800C532E & 0x100) {
        D_800B0540 ^= 1;
    }
    start = task->dl;
    D_800B053C = start;
    func_8001BE98(0xC1);
    func_8001BE98(0x83);
    func_8000B06C();
    func_8001BE98(0xC3);
    func_8001BE98(0x81);
    Obj_UpdateAll();

    GFX(0xBC000006, 0);                                 /* segment 0 = 0 */
    GFX(0x06000000, (u32)D_800386A0);
    if (D_800386C8 != 0) {
        GFX(0x06000000, (u32)D_80038560);
        D_800386C8 = 0;
    }
    GFX(0x06000000, (u32)D_80038648);
    D_800B053C = Gfx_SetFullViewport(D_800B053C);

    g = D_800B053C++;                                   /* Z image */
    g->w0 = 0xFE000000;
    g->w1 = osVirtualToPhysical(D_80115000);
    GFX(0xE7000000, 0);                                 /* clear the Z buffer */
    GFX(0xBA001402, 0x300000);
    g = D_800B053C++;
    g->w0 = 0xFF100000 | ((gScreenWidth - 1) & 0xFFF);
    g->w1 = osVirtualToPhysical(D_80115000);
    GFX(0xF7000000, 0xFFFCFFFC);
    GFX(0xF6000000 | (((gScreenWidth - 1) & 0x3FF) << 14) | (((D_800C5330 - 1) & 0x3FF) << 2), 0);
    g = D_800B053C++;                                   /* colour image */
    g->w0 = 0xFF100000 | ((gScreenWidth - 1) & 0xFFF);
    g->w1 = osVirtualToPhysical(task->fb);
    if (D_800B05A0 != 0) {                              /* clear the frame buffer */
        c = ((D_800B0724 << 8) & 0xF800) | ((D_800B0618 << 3) & 0x7C0) | ((D_800B0594 >> 2) & 0x3E);
        GFX(0xF7000000, ((c | 1) << 16) | 1 | c);
        GFX(0xF6000000 | (((gScreenWidth - 1) & 0x3FF) << 14) | (((D_800C5330 - 1) & 0x3FF) << 2), 0);
    }
    fw = (f32)gScreenWidth;
    fh = (f32)D_800C5330;
    GFX(0xE7000000, 0);
    GFX(0xBA001402, 0);                                 /* 1-cycle mode */

    Mtx_Ortho(m, (f32)((f64)-fw / 2.0), (f32)((f64)fw / 2.0), (f32)((f64)-fh / 2.0),
              (f32)((f64)fh / 2.0), -1.0f, 3000.0f, 1.0f);
    func_8000A924(task->ortho, m);
    norm = Mtx_PerspectiveG(D_800B05B8, gCameraFov, (f32)gScreenWidth / (f32)D_800C5330,
                            (f32)D_800C1788, (f32)D_800C53D0, 1.0f);
    func_8000A924(task->persp, D_800B05B8);
    Mtx_LookAtReflect(D_800C16E8, (VdLookAt *)task->lookat, &gCameraEye, &gCameraTarget,
                      gCameraUp.x, gCameraUp.y, gCameraUp.z);
    if (D_800B0548 != 0) {
        Mtx_Identity(D_800C16E8);
    }
    Mtx_Copy(gSceneGroup1->mtx, D_800C16E8);
    Light_Update(&D_800C1730, D_800C16E8);
    Mtx_Identity(m);
    Scene_UpdateWorld(gSceneRoot0, m);
    Scene_UpdateWorld(gSceneRoot1, m);
    Scene_UpdateWorld(gSceneRoot2, m);
    Scene_UpdateWorld(gSceneRoot3, m);

    GFX(0x03840010, (u32)&task->lookat[0x00]);          /* LookAt x */
    GFX(0x03820010, (u32)&task->lookat[0x10]);          /* LookAt y */
    GFX(0xBC00000E, norm & 0xFFFF);                     /* perspective normalisation */
    GFX(0xBC000002, 0x80000040);                        /* 1 light */
    GFX(0x03860010, (u32)D_800C1730.col);               /* light 1 */
    GFX(0x03880010, (u32)D_800C1730.amb_col);           /* ambient */
    GFX(0x01030040, (u32)task->ortho);                  /* projection, load */
    D_800B053C = Scene_BuildGfx(D_800B053C, gSceneRoot0);
    GFX(0x01030040, (u32)task->persp);
    D_800B053C = Scene_BuildGfx(D_800B053C, gSceneRoot1);
    GFX(0x01030040, (u32)task->ortho);
    D_800B053C = Scene_BuildGfx(D_800B053C, gSceneRoot2);
    GFX(0x01030040, (u32)task->ortho);
    D_800B053C = Scene_BuildGfx(D_800B053C, gSceneRoot3);
    GFX(0xE9000000, 0);                                 /* full sync */
    GFX(0xB8000000, 0);                                 /* end */

    task->data_size = ((s32)((u32)D_800B053C - (u32)start) >> 3) << 3;
    task->data_ptr = start;
    task->type = 1;
    if (D_800B0504 != 1) {
        task->ucode_boot = D_80033FA0;
        task->ucode_boot_size = D_80034070 - D_80033FA0;
    }
    task->yield_data_size = 0x900;
    task->flags = 0;
    switch (D_800B0504) {
    case 0:
        task->yield_data_size = 0xFC0;
        task->ucode = D_80037210;
        task->ucode_data = D_8003CA50;
        task->output_buff = D_8013B510;
        task->output_buff_end = D_80173510;
        task->kind = 0x63;
        break;
    case 2:
        task->ucode = D_80034E90;
        task->ucode_data = D_8003BA50;
        task->kind = 0x63;
        break;
    case 3:
        task->ucode = D_80035E10;
        task->ucode_data = D_8003C250;
        task->output_buff = D_8013B510;
        task->output_buff_end = D_80173510;
        task->kind = 0x63;
        break;
    }
    task->ucode_size = 0x1000;
    task->ucode_data_size = 0x800;
    task->dram_stack = D_8013B110;
    task->dram_stack_size = 0x400;
    task->yield_data = D_8013A810;
    task->mq = D_800C30E0;
    task->unk00 = 0;
    task->msg = task->msgbuf;
    task->fb_out = task->fb;
    func_8001C990();
    osSendMesg(D_800B0568, task, 1);
    D_800C1650++;
}

/* @ 0x800107D0 */
/* Full-screen viewport (Y flipped in modes 0 and 1), pipe sync and scissor;
 * resets the per-frame viewport counter. */
SpriteGfx *Gfx_SetFullViewport(SpriteGfx *gfx)
{
    s32 w = gScreenWidth;
    s32 h = D_800C5330;

    D_800A68F8.vscale[0] = w * 2;
    D_800A68F8.vscale[1] = ((u32)D_800B0504 < 2) ? -h * 2 : h * 2;
    D_800A68F8.vscale[2] = 0x1FF;
    D_800A68F8.vtrans[0] = w * 2;
    D_800A68F8.vtrans[1] = h * 2;
    D_800A68F8.vscale[3] = 0;
    D_800A68F8.vtrans[2] = 0x1FF;
    D_800A68F8.vtrans[3] = 0;
    gfx[0].w0 = 0x03800010;
    gfx[0].w1 = (u32)&D_800A68F8;
    gfx[1].w0 = 0xE7000000;
    gfx[1].w1 = 0;
    gfx[2].w0 = 0xED000000;
    D_800A6908 = 0;
    gfx[2].w1 = (((s32)((f32)(w - 1) * 4.0f) & 0xFFF) << 12) | ((s32)((f32)(h - 1) * 4.0f) & 0xFFF);
    return gfx + 3;
}

/* @ 0x80010900 */
/* Viewport for a scene node: rectangle (x, y, w, h) in screen pixels, scale
 * sx/sy in 1/8192 units (0x2000 = full size). Uses one of 8 Vp slots per
 * frame; when they run out nothing is emitted. Arguments are truncated to
 * 16 bits like the original. */
SpriteGfx *Gfx_SetViewport(SpriteGfx *gfx, s32 x_, s32 y_, s32 w_, s32 h_, s32 sx_, s32 sy_)
{
    s16 x = x_, y = y_, w = w_, h = h_, sx = sx_, sy = sy_;
    s32 i = D_800A6908;
    s32 sw, sh, hh;
    f64 s;

    if (i < 8) {
        s = (f64)(f32)sx / 8192.0;
        sw = gScreenWidth;
        D_800A6910[i].vscale[0] = (s32)((f64)(sw * 2) * s);
        sh = D_800C5330;
        s = (f64)(f32)sy / 8192.0;
        hh = ((u32)D_800B0504 < 2) ? -sh * 2 : sh * 2;
        D_800A6910[i].vscale[1] = (s32)((f64)hh * s);

        i = D_800A6908;
        D_800A6910[i].vtrans[0] = (sw - ((sw / 2 - (x + w / 2)) * 2)) * 2;
        D_800A6910[i].vscale[2] = 0x1FF;
        D_800A6910[i].vscale[3] = 0;
        D_800A6910[i].vtrans[1] = (sh - ((sh / 2 - (y + h / 2)) * 2)) * 2;
        D_800A6910[i].vtrans[2] = 0x1FF;
        D_800A6910[i].vtrans[3] = 0;
        D_800A6908 = i + 1;
        gfx[0].w0 = 0x03800010;
        gfx[0].w1 = (u32)&D_800A6910[i];
        gfx[1].w0 = 0xE7000000;
        gfx[1].w1 = 0;
        gfx[2].w0 = 0xED000000 | (((s32)((f32)x * 4.0f) & 0xFFF) << 12) | ((s32)((f32)y * 4.0f) & 0xFFF);
        gfx[2].w1 = (((s32)((f32)(x + w - 1) * 4.0f) & 0xFFF) << 12) |
                    ((s32)((f32)(y + h - 1) * 4.0f) & 0xFFF);
        gfx += 3;
    }
    return gfx;
}

/* @ 0x80010BF4 */
/* Apply gScreenWidth to the video mode (x scale, width, field origins) with
 * interrupts masked, restore the saved mode into the current table entry,
 * set the new mode, save it, then adjust the screen position. */
void Video_ApplyMode(void)
{
    s32 xscale = (gScreenWidth << 10) / 640;
    u32 mask = osSetIntMask(0x80401);
    s32 i;

    D_800A6850[8] = xscale;                 /* comRegs.xScale */
    D_800A6850[2] = gScreenWidth;           /* comRegs.width */
    D_800A6850[10] = gScreenWidth * 2;      /* fldRegs[0].origin */
    D_800A6850[15] = gScreenWidth * 2;      /* fldRegs[1].origin */
    osSetIntMask(mask);
    for (i = 0; i < 20; i++) {
        D_800A68A0[i] = D_800A68A8[i];
    }
    osViSetMode(D_800A6850);
    for (i = 0; i < 20; i++) {
        D_800A68A8[i] = D_800A6850[i];
    }
    D_800A68A0 = D_800A6850;
    Video_ApplyHStart();
    Video_ApplyVStart();
    func_80021990(0xA2);
}

/* Add an offset to both 16-bit halves of a VI start register, each mod 65535
 * (signed C remainder, as in the original). */
static u32 Video_OffsetStart(u32 v, s32 off)
{
    return ((((s32)(v >> 16) + off) % 65535) << 16) | (((s32)(v & 0xFFFF) + off) % 65535);
}

/* @ 0x80010D28 */
void Video_ApplyHStart(void)
{
    D_800A68A0[7] = Video_OffsetStart(D_800A68A8[7], D_800386C0);       /* comRegs.hStart */
}

/* @ 0x80010DB0 */
void Video_ApplyVStart(void)
{
    s32 off = D_800386C4;
    D_800A68A0[12] = Video_OffsetStart(D_800A68A8[12], off);            /* fldRegs[0].vStart */
    D_800A68A0[17] = Video_OffsetStart(D_800A68A8[17], off);            /* fldRegs[1].vStart */
}

/* ================================================================ screen quads */
/* Four 4-vertex quads (N64 Vtx, 16 bytes; colour at 12..15) and their DLs. */
/* endian: typed as Vtx (not u8[16]) so the boot loader swaps x/y/z/flag/s/t and keeps the
 * colour bytes; the colour/alpha writes below are byte stores at offsets 12..15 as before. */
extern SpriteVtx D_800386D0[4], D_80038710[4], D_80038750[4], D_80038790[4];
extern SpriteGfx D_800387D0[], D_80038828[], D_80038880[], D_800388D8[];

#define QUAD_Z (-405.11322021484375f)   /* camera distance: z = 0 plane at screen scale */

static void ScreenQuad_Place(SceneNode *n, f32 x, f32 y, f32 sx, f32 sy)
{
    MtxF s;
    Mtx_Translate(n->mtx, x, y, QUAD_Z);
    Mtx_Scale(s, sx, sy, 1.0f);
    Mtx_Mul(n->mtx, s, n->mtx);
}

/* @ 0x80010EB0 */
void ScreenQuad_SetAlpha0(SceneNode *n, u8 a, f32 x, f32 y, f32 sx, f32 sy)
{
    s32 i;
    for (i = 3; i >= 0; i--) {
        D_800386D0[i].a = a;
    }
    ScreenQuad_Place(n, x, y, sx, sy);
    n->dl = D_800387D0;
}

/* @ 0x80010F48 */
void ScreenQuad_SetAlpha1(SceneNode *n, u8 a, f32 x, f32 y, f32 sx, f32 sy)
{
    s32 i;
    for (i = 3; i >= 0; i--) {
        D_80038710[i].a = a;
    }
    ScreenQuad_Place(n, x, y, sx, sy);
    n->dl = D_80038828;
}

/* @ 0x80010FE0 */
void ScreenQuad_SetColor2(SceneNode *n, u8 r, u8 g, u8 b, f32 x, f32 y, f32 sx, f32 sy)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        D_80038750[i].r = r;
        D_80038750[i].g = g;
        D_80038750[i].b = b;
    }
    ScreenQuad_Place(n, x, y, sx, sy);
    n->dl = D_80038880;
}

/* @ 0x80011088 */
void ScreenQuad_SetColorA2(SceneNode *n, u8 r, u8 g, u8 b, u8 a, f32 x, f32 y, f32 sx, f32 sy)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        D_80038750[i].r = r;
        D_80038750[i].g = g;
        D_80038750[i].b = b;
        D_80038750[i].a = a;
    }
    ScreenQuad_Place(n, x, y, sx, sy);
    n->dl = D_80038880;
}

/* @ 0x80011140 */
void ScreenQuad_SetColor3(SceneNode *n, u8 r, u8 g, u8 b, f32 x, f32 y, f32 sx, f32 sy)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        D_80038790[i].r = r;
        D_80038790[i].g = g;
        D_80038790[i].b = b;
    }
    ScreenQuad_Place(n, x, y, sx, sy);
    n->dl = D_800388D8;
}

/* @ 0x800111E8 */
void ScreenQuad_SetColorA3(SceneNode *n, u8 r, u8 g, u8 b, u8 a, f32 x, f32 y, f32 sx, f32 sy)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        D_80038790[i].r = r;
        D_80038790[i].g = g;
        D_80038790[i].b = b;
        D_80038790[i].a = a;
    }
    ScreenQuad_Place(n, x, y, sx, sy);
    n->dl = D_800388D8;
}

/* @ 0x800112A0 */
/* Place a display-list node: translate (x, y, QUAD_Z), scale (sx, sy). */
void ScreenQuad_SetDL(SceneNode *n, SpriteGfx *dl, f32 x, f32 y, f32 sx, f32 sy)
{
    ScreenQuad_Place(n, x, y, sx, sy);
    n->dl = dl;
}

/* @ 0x80011334 */
/* Place a display-list node with a full transform: translate, scale, then
 * rotate Y, X, Z (degrees). */
void ScreenQuad_SetDLXform(SceneNode *n, SpriteGfx *dl, const NodeXform *x)
{
    MtxF s, t;
    Mtx_Translate(n->mtx, x->tx, x->ty, x->tz);
    Mtx_Scale(s, x->sx, x->sy, x->sz);
    Mtx_Mul(t, s, n->mtx);
    Mtx_PreRotateYXZ(n->mtx, t, x->ry, x->rx, x->rz);
    n->dl = dl;
}

/* ================================================================ controller pak */
extern u8 D_800A6990[4][0x68];          /* OSPfs per channel */
extern u8 D_80038AB0[], D_80038AC0[];   /* game name, extension */
extern char D_8003D7C0[];               /* "pfsCreate" */
extern char D_8003D7CC[];               /* "pfsRead@1" */
extern char D_8003D7D8[];               /* "pfsRead@2" */
extern char D_8003D7E4[];               /* "pfsWrite#1" */
extern char D_8003D7F0[];               /* "pfsWrite#2" */
extern char D_8003D7FC[];               /* "pfsChkFile#1" */
extern s32 D_800B0544;                  /* EEPROM present: 0 yes, -1 no */

#define PFS_COMPANY 1
#define PFS_GAME    1

/* @ 0x80014E90 */
s32 Pfs_Init(void)
{
    osCreateMesgQueue(D_800B0520, D_800C5328, 1);
    return 0;
}

/* Common start of the pak functions: is a pak plugged into `ch` (when the
 * plug test itself works) and can it be initialised? Returns the OSPfs or NULL. */
static void *Pfs_Open(s32 ch)
{
    u8 plugged;
    void *pfs;

    if (func_80024360(D_800C30A8, &plugged) != 0 && !(plugged & (1 << ch))) {
        return NULL;
    }
    pfs = D_800A6990[ch];
    if (func_80024A30(D_800C30A8, pfs, ch) != 0) {
        return NULL;
    }
    return pfs;
}

/* @ 0x80014EC0 */
/* Create the save file. -1 if no pak; allocation errors are only handled. */
s32 Pfs_CreateFile(s32 ch, s32 length)
{
    s32 ret = 0;
    s32 file_no;
    s32 err;
    void *pfs;

    Si_Acquire();
    pfs = Pfs_Open(ch);
    if (pfs == NULL) {
        ret = -1;
    } else {
        err = func_800230D0(pfs, PFS_COMPANY, PFS_GAME, D_80038AB0, D_80038AC0, length, &file_no);
        if (err != 0) {
            Pfs_HandleError(err, pfs, D_8003D7C0);
        }
    }
    Si_Release();
    return ret;
}

/* Read (flag 0) or write (flag 1) the save file. */
static s32 Pfs_ReadWrite(s32 ch, u8 *buf, s32 offset, s32 size, u8 flag,
                         char *where1, char *where2)
{
    s32 ret = 0;
    s32 file_no;
    s32 err;
    void *pfs;

    Si_Acquire();
    pfs = Pfs_Open(ch);
    if (pfs == NULL) {
        ret = -1;
    } else {
        err = osPfsFindFile(pfs, PFS_COMPANY, PFS_GAME, D_80038AB0, D_80038AC0, &file_no);
        if (err != 0 && Pfs_HandleError(err, pfs, where1) != 0) {
            ret = -1;
        } else {
            err = func_80023B60(pfs, file_no, flag, offset, size, buf);
            if (err != 0) {
                Pfs_HandleError(err, pfs, where2);
            }
        }
    }
    Si_Release();
    return ret;
}

/* @ 0x80014FC0 */
s32 Pfs_ReadFile(s32 ch, u8 *buf, s32 offset, s32 size)
{
    return Pfs_ReadWrite(ch, buf, offset, size, 0, D_8003D7CC, D_8003D7D8);
}

/* @ 0x80015110 */
s32 Pfs_WriteFile(s32 ch, u8 *buf, s32 offset, s32 size)
{
    return Pfs_ReadWrite(ch, buf, offset, size, 1, D_8003D7E4, D_8003D7F0);
}

/* @ 0x80015260 */
/* Delete the save file (the error tag reuses "pfsWrite#1"). */
s32 Pfs_DeleteFile(s32 ch)
{
    s32 ret = 0;
    s32 err;
    void *pfs;

    Si_Acquire();
    pfs = Pfs_Open(ch);
    if (pfs == NULL) {
        ret = -1;
    } else {
        err = func_800236C0(pfs, PFS_COMPANY, PFS_GAME, D_80038AB0, D_80038AC0);
        if (err != 0) {
            Pfs_HandleError(err, pfs, D_8003D7E4);
            ret = -1;
        }
    }
    Si_Release();
    return ret;
}

/* @ 0x8001534C */
/* Bit mask of channels with a pak (undefined if osPfsIsPlug doesn't write it). */
u8 Pfs_GetPluggedMask(void)
{
    u8 mask;
    Si_Acquire();
    func_80024360(D_800C30A8, &mask);
    Si_Release();
    return mask;
}

/* @ 0x80015384 */
/* 1 if the save file exists (or a find error could be fixed), else -1. */
s32 Pfs_CheckFile(s32 ch)
{
    s32 ret;
    s32 file_no;
    s32 err;
    void *pfs;

    Si_Acquire();
    pfs = Pfs_Open(ch);
    if (pfs == NULL) {
        ret = -1;
    } else {
        err = osPfsFindFile(pfs, PFS_COMPANY, PFS_GAME, D_80038AB0, D_80038AC0, &file_no);
        if (err != 0) {
            err = Pfs_HandleError(err, pfs, D_8003D7FC);
        }
        ret = (err != 0) ? -1 : 1;
    }
    Si_Release();
    return ret;
}

/* @ 0x80015474 */
void Pfs_Nop(void)
{
}

/* @ 0x8001547C */
/* Recover from a pak error: new pak -> re-init, inconsistent -> run the
 * checker, anything else -> -1. `where` is a debug tag, unused. */
s32 Pfs_HandleError(s32 err, void *pfs, const char *where)
{
    (void)where;
    switch (err) {
    case 2:     /* PFS_ERR_NEW_PACK */
        return func_80024A30(((void **)pfs)[1], pfs, ((s32 *)pfs)[2]);
    case 3:     /* PFS_ERR_INCONSISTENT */
        return osPfsChecker(pfs);
    default:
        return -1;
    }
}

/* @ 0x800154D0 */
s32 Eeprom_Probe(void)
{
    Si_Acquire();
    if (func_80024850(D_800C30A8) != 0) {
        D_800B0544 = 0;
    } else {
        D_800B0544 = -1;
    }
    Si_Release();
    return D_800B0544;
}

/* @ 0x80015524 */
/* Read the whole 512-byte EEPROM. */
s32 Eeprom_Read(u8 *buf)
{
    s32 r;
    Si_Acquire();
    r = func_800249C0(D_800C30A8, 0, buf, 0x200);
    Si_Release();
    return r;
}

/* @ 0x8001556C */
s32 Eeprom_Write(u8 *buf)
{
    s32 r;
    Si_Acquire();
    r = func_800248D0(D_800C30A8, 0, buf, 0x200);
    Si_Release();
    return r;
}

/* ================================================================ geometry */
#define GEOM_EPS 1.0000000168623835e-16     /* (f64)1e-16f */

/* @ 0x800155C0 */
/* Parameter t of the point on the line closest to p (0 for a zero direction). */
f32 Line3_ProjectParam(const Vec3f *p, const Line3f *line)
{
    f32 dx = line->dir.x;
    f32 dy = line->dir.y;
    f32 dz = line->dir.z;
    f64 len2 = (f64)((dx * dx) + (dy * dy) + (dz * dz));

    if (len2 < GEOM_EPS) {
        return 0.0f;
    }
    return (f32)((((f64)dx * (f64)(p->x - line->point.x)) + ((f64)dy * (f64)(p->y - line->point.y)) +
                  ((f64)dz * (f64)(p->z - line->point.z))) / len2);
}

/* @ 0x80015664 */
/* Distance from p to the segment a-b. */
f32 Segment3_DistanceToPoint(const Vec3f *p, const Vec3f *a, const Vec3f *b)
{
    Line3f line;
    f32 t, dx, dy, dz;

    line.point.x = a->x;
    line.dir.x = b->x - a->x;
    line.point.y = a->y;
    line.dir.y = b->y - a->y;
    line.point.z = a->z;
    line.dir.z = b->z - a->z;
    t = Line3_ProjectParam(p, &line);
    if ((f64)t < 0.0) {
        dx = p->x - a->x;
        dy = p->y - a->y;
        dz = p->z - a->z;
    } else if ((f64)t >= 1.0) {
        dx = p->x - b->x;
        dy = p->y - b->y;
        dz = p->z - b->z;
    } else {
        return Line3_DistanceToPoint(p, &line);
    }
    return sqrtf((dx * dx) + (dy * dy) + (dz * dz));
}

/* @ 0x8001581C */
/* Orthogonal projection of p onto the plane; 0 (out untouched) if the normal
 * is zero. The scale factor is computed in double. */
s32 Plane_ProjectPoint(Vec3f *out, const Vec3f *p, const Planef *plane)
{
    const f32 *n = &plane->a;
    const f32 *q = &p->x;
    f32 *o = &out->x;
    f32 a = plane->a, b = plane->b, c = plane->c;
    f64 len2 = (f64)((a * a) + (b * b) + (c * c));
    f64 t;
    s32 i;

    if (len2 < GEOM_EPS) {
        return 0;
    }
    t = (f64)((a * p->x) + (b * p->y) + (c * p->z) + plane->d) / len2;
    for (i = 0; i < 3; i++) {
        o[i] = (f32)((f64)q[i] - ((f64)n[i] * t));
    }
    return 1;
}

/* @ 0x800158D4 */
/* Same as Plane_ProjectPoint with a double plane and double output. */
s32 Plane_ProjectPointD(f64 out[3], const Vec3f *p, const f64 plane[4])
{
    const f32 *q = &p->x;
    f64 a = plane[0], b = plane[1], c = plane[2];
    f64 len2 = (a * a) + (b * b) + (c * c);
    f64 t;
    s32 i;

    if (len2 < GEOM_EPS) {
        return 0;
    }
    t = ((a * (f64)p->x) + (b * (f64)p->y) + (c * (f64)p->z) + plane[3]) / len2;
    for (i = 0; i < 3; i++) {
        out[i] = (f64)q[i] - (plane[i] * t);
    }
    return 1;
}

/* @ 0x80015988 */
/* out = a x b; safe when out is a or b. */
void Vec3_Cross(Vec3f *out, const Vec3f *a, const Vec3f *b)
{
    if (out != a && out != b) {
        out->x = (a->y * b->z) - (b->y * a->z);
        out->y = (a->z * b->x) - (b->z * a->x);
        out->z = (a->x * b->y) - (b->x * a->y);
    } else {
        f32 t[3];
        f32 *o = &out->x;
        s32 i;
        t[0] = (a->y * b->z) - (b->y * a->z);
        t[1] = (a->z * b->x) - (b->z * a->x);
        t[2] = (a->x * b->y) - (b->x * a->y);
        for (i = 0; i < 3; i++) {
            o[i] = t[i];
        }
    }
}
