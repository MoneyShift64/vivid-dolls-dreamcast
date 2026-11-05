/* Vivid Dolls decompilation - system code (0x8000A5A0-0x8000D000).
 *
 * TLB setup, the cooperative task pool used by the game's "processes",
 * libultra guMtxF2L/guMtxL2F, the Aleck64 8251 UART at 0x02000040, the boot
 * code and thread entry points, ROM/overlay DMA, the per-frame input thread,
 * and the RCP scheduler (a lightly modified copy of the N64 SDK's sched.c).
 * Verified against the original by tools/difftest/test_system.py.
 */
#include <stdarg.h>
#include "system.h"
#include "heap.h"
#include "scene.h"
#include "object.h"
#include "collide.h"

/* ------------------------------------------------------------------ externs */

/* libultra (names from symbol_addrs.txt, or func_ names for unnamed ones) */
extern void  osCreateThread(OSThread *t, s32 id, void (*entry)(void *), void *arg, void *sp, s32 pri);
extern void  osStartThread(OSThread *t);
extern void  osSetThreadPri(OSThread *t, s32 pri);
extern void  osCreateMesgQueue(OSMesgQueue *mq, OSMesg *msg, s32 count);
extern s32   osRecvMesg(OSMesgQueue *mq, OSMesg *msg, s32 flag);
extern s32   osSendMesg(OSMesgQueue *mq, OSMesg msg, s32 flag);
extern void  osSetEventMesg(u32 event, OSMesgQueue *mq, OSMesg msg);
extern void  osCreatePiManager(s32 pri, OSMesgQueue *cmdQ, OSMesg *cmdBuf, s32 cmdMsgCnt);
extern void  osInvalDCache(void *vaddr, s32 nbytes);
extern void  osInvalICache(void *vaddr, s32 nbytes);
extern void  osWritebackDCache(void *vaddr, s32 nbytes);
extern u32   osSetIntMask(u32 mask);
extern void  osCreateViManager(s32 pri);
extern void  osViSetMode(void *mode);
extern void  osViBlack(u8 active);
extern void  osViSetEvent(OSMesgQueue *mq, OSMesg msg, u32 retraceCount);
extern void  osSpTaskYield(void);
extern u32   osSpTaskYielded(OSTask *t);
extern void *osViGetCurrentFramebuffer(void);
extern void *osViGetNextFramebuffer(void);
extern void  __osContRamRead(void *a0, s32 a1, s32 a2, void *a3);
extern u8    osViModeTable[][0x50];

extern void  func_8001CAC0(void);                                     /* osInitialize */
extern void  func_8001C990(void);                                     /* osWritebackDCacheAll */
extern s32   func_80025950(void *mb, s32 pri, s32 dir, u32 devAddr, void *vAddr, u32 nbytes,
                           OSMesgQueue *mq);                          /* osPiStartDma */
extern void  func_80021280(OSTask *t);                                /* osSpTaskLoad */
extern void  func_8002148C(OSTask *t);                                /* osSpTaskStartGo */
extern void  func_800211E0(u64 *buf, u64 size);                       /* osDpSetNextBuffer */
extern void  func_80021B00(void *fb);                                 /* osViSwapBuffer */
extern void *func_8001D340(void);                                     /* __osGetCurrFaultedThread */
extern void  func_80021FC0(OSMesgQueue *mq);
extern void  func_80025DC0(s32 a0, s32 a1);
extern void  func_8001D350(s32 a0, s32 a1);
extern void  func_80021EA0(s32 a0, s32 a1);
extern void  func_80024750(s32 a0, s32 a1, s32 a2);
extern void  func_80023F90(s32 a0, s32 a1, s32 a2);
extern void  func_800221A0(s32 a0, s32 a1);
extern void  func_800236C0(s32 a0, s32 a1, s32 a2, s32 a3, s32 a4);

#define osInitialize          func_8001CAC0
#define osWritebackDCacheAll  func_8001C990
#define osPiStartDma          func_80025950
#define osSpTaskLoad          func_80021280
#define osSpTaskStartGo       func_8002148C
#define osDpSetNextBuffer     func_800211E0
#define osViSwapBuffer        func_80021B00

/* the hand-written assembly parts (see system.h) */
extern TlbEntry *func_8000ADA4(s32 index, TlbEntry *e);               /* Tlb_Write */
extern void      func_8000B150(u32 *ctx);                             /* Task_SaveContext */
extern void      func_8000AE90(void);                                 /* Task_Exit */
#define Tlb_Write         func_8000ADA4
#define Task_SaveContext  func_8000B150
#define Task_Exit         func_8000AE90

/* game code in other areas */
extern void  func_8000F3E0(void);
extern void  func_8000F6EC(void);
extern u16   func_8000F750(void);                                     /* returns D_800A6840 */
extern void  func_8000F8F4(void);
extern void  func_8000FA34(void);
extern void  func_8000FA4C(void);
extern void  func_8000F890(void);
extern void  func_8000F350(void);
extern void  func_8000FDB0(void);
extern void  func_8000FF64(u8 *gfxBuf);
extern void  func_8000D150(void *thread);
extern void  func_80014E90(void);
extern void  func_800154D0(void);
extern void  func_8001BB18(s32 a0);
extern void  func_8001BE80(void);
extern void  func_8001BE88(void);                                     /* empty (profiler hooks) */
extern void  func_8001BE90(void);
extern void  func_8001BE98(s32 marker);
extern void  func_8001BEA0(void);
extern void  func_8001BF08(void);
extern void  Audio_Init(void);

/* data */
typedef struct { s32 index; u32 size; u32 mask; } TlbPageSize;
typedef struct { u32 vram; u32 unk4; u32 romStart; u32 romEnd; } OverlayEntry;
typedef struct { u8 data[0x18]; } OSIoMesg;

extern TlbPageSize  D_80038410[];         /* page sizes 4K..16M: {index, size, pagemask} */
extern OverlayEntry gOverlayTable[];
extern TaskPool *D_800C51DC;              /* the task pool */
extern Task *D_800B05A4;                  /* &pool->head */
extern Task *D_800C53FC;                  /* &pool->tail */
extern s32   gArgDebug;
extern s32   gScreenWidth;
extern Heap  gGameHeap;
extern RingPool gSceneNodePool;
extern SceneNode *gSceneRoot0, *gSceneRoot1, *gSceneRoot2, *gSceneRoot3;

extern OSThread    D_8003E138, D_8003E2E8, D_8003E498, D_8003E648, D_80041958;
extern u8          D_8003F000[], D_8003F800[], D_80041000[], D_80041800[];
extern OSMesgQueue D_80041900, D_80041918, D_80041938, D_800B04E0, D_800B0520, D_800C30A8, D_800C30E0;
extern OSMesg      D_80041930[], D_80041950[], D_800B0620[], D_800C1798[];
extern void       *D_8003E130;            /* faulted thread */
extern s32         D_8003E134;
extern s32         D_80043310;            /* set when a frame was started, cleared by the input thread */
extern u32         D_80043314;            /* retraces since the last frame was started */
extern u32         D_80043320;            /* RCP busy counter (2 = gfx task started) */
extern s32         D_80038470, D_80038474, D_80038478;
extern s32         D_800AF254, D_800AF260, D_800B0430, D_800B04D0, D_800B4AA4, D_800B0720;
extern s32         D_800C0AB8;            /* gfx buffer index (0/1) */
extern u32         D_800C51CC;            /* retraces per frame */
extern s32         D_800C51D0;            /* controller thread state */
extern u32         D_800C53F8;            /* frames in flight */
extern s32         D_800C1674;
extern u32         D_800C30C8;            /* credit counter, max 99 */
extern u16         D_800C1678, D_800C16E0, D_800C1898, D_800C53D8, D_800C53DA, D_800B0574, D_800B0576;
extern u8          D_800B0730[];          /* 2 gfx buffers of 0x2180 bytes */
extern AllocNode   D_800B0558, D_800B0578;
extern SceneLight  D_800C1730;
extern OSSched     D_800C0AC0;
extern u8          D_800C51C0[];          /* scheduler thread stack top */
extern OSMesgQueue *D_800B0568;           /* scheduler command queue */
extern OSScClient  D_800B054C, D_800C165C;
extern u8          D_800C16C0[];
extern u8          D_8069E600[];          /* task pool memory (0x114000 bytes) */
extern u16         D_807B2600[], D_807D7E00[];   /* the two frame buffers */

/* input words, two players each: */
extern u16 D_800B058C[2], D_800C5334[2], D_800B0596[2], D_800B043A[2], D_800AF222[2], D_800AF238[2];
extern u16 D_800B0498[2], D_800C532C[2], D_800B4A80[2];
extern u16 D_800B060C[2], D_800C0D48[2], D_800AF258[2];

/* ---------------------------------------------------------------------- TLB */

/* @ 0x8000A5A0 */
/* Fill all 32 TLB entries with harmless mappings (pairs of KSEG0 pages, invalid). */
void Tlb_ClearAll(void) {
    TlbEntry e;
    s32 i;

    e.entryLo0 = 0;
    e.entryLo1 = 0;
    e.pageMask = 0x2000;
    for (i = 0; i < 32; i++) {
        e.entryHi = ((0x80000 + i) / 2) << 13;
        Tlb_Write(i, &e);
    }
}

/* @ 0x8000A628 */
/* Map vaddr -> paddr (an even/odd page pair) in TLB entry 'index'. sizeIdx selects
 * the page size in D_80038410 (0 = 4K .. 6 = 16M); returns 0 for a bad index.
 * cached != 0 uses cache mode 3, else 2 (uncached). paddr is shifted signed. */
s32 Tlb_Map(s32 index, u32 vaddr, u32 paddr, s32 sizeIdx, s32 cached) {
    TlbPageSize *ps = &D_80038410[sizeIdx];
    u32 attr = (cached != 0 ? 3 : 2) << 3;
    TlbEntry e;

    if (ps->index != sizeIdx) {
        return 0;
    }
    e.entryHi = vaddr;
    e.entryLo0 = ((s32)paddr >> 6) | attr | 7;
    e.entryLo1 = ((s32)(paddr + ps->size) >> 6) | attr | 7;
    e.pageMask = ps->mask;
    Tlb_Write(index, &e);
    return 1;
}

/* ---------------------------------------------------------------- task pool */

/* @ 0x8000A6D0 */
/* Set up the task pool: header, a stack of free block pointers (NULL-terminated)
 * and empty head/tail sentinels. Blocks start 16-byte aligned after the pointers. */
void Task_InitPool(u8 *pool, u32 poolSize, u32 blockSize) {
    TaskPool *tp = (TaskPool *)pool;
    u32 n = (poolSize - 0x30) / (blockSize + 4);
    Task **slot = tp->freeList;
    uintptr_t blk;

    D_800C51DC = tp;
    tp->blockSize = blockSize;
    blk = ((uintptr_t)slot + (n + 1) * 4 + 0x10) & ~(uintptr_t)0xF;
    tp->freeTop = slot;
    for (; n != 0; n--) {
        *slot++ = (Task *)blk;
        blk += blockSize;
    }
    *slot = NULL;
    D_800B05A4 = &tp->head;
    D_800C53FC = &tp->tail;
    tp->head.prev = NULL;
    tp->tail.next = NULL;
    tp->head.next = &tp->tail;
    tp->tail.prev = &tp->head;
}

/* @ 0x8000A76C */
/* Create a task running func(args...) with the given priority: take a free block,
 * insert it before the first task with a higher prio (FIFO among equals), copy the
 * argBytes of arguments to the top of its stack, and build an initial context frame
 * below them (registers from Task_SaveContext, ra = Task_Exit, pc = func, a0.. = the
 * arguments again). Returns NULL when the pool is full. */
Task *Task_Create(void (*func)(), s32 prio, s32 argBytes, ...) {
    TaskPool *tp = D_800C51DC;
    Task **slot = tp->freeTop;
    Task *t, *n;
    u8 *args;
    u32 *dst, *ctx;
    s32 i;
    va_list ap;

    tp->freeTop = slot + 1;
    t = *slot;
    if (t == NULL) {
        tp->freeTop = slot;
        return NULL;
    }
    t->prio = prio;
    n = D_800B05A4->next;
    while (n->next != NULL && prio >= n->prio) {
        n = n->next;
    }
    n->prev->next = t;
    t->next = n;
    t->prev = n->prev;
    n->prev = t;

    args = (u8 *)t + tp->blockSize - argBytes;
    dst = (u32 *)args;
    va_start(ap, argBytes);
    for (i = argBytes / 4; i != 0; i--) {
        *dst++ = va_arg(ap, u32);
    }
    va_end(ap);

    ctx = (u32 *)(args - 0xC0);
    Task_SaveContext(ctx);
    ctx[0x38 / 4] = (u32)(uintptr_t)Task_Exit;
    ctx[0x3C / 4] = (u32)(uintptr_t)func;
    dst = ctx;
    va_start(ap, argBytes);
    for (i = argBytes / 4; i != 0; i--) {
        *dst++ = va_arg(ap, u32);
    }
    va_end(ap);
    t->sp = ctx;
    return t;
}

/* @ 0x8000A8D0 */
/* Give a task's block back to the pool and unlink it (its next pointer stays valid,
 * Task_Exit uses it to switch to the following task). Returns t. */
Task *Task_Free(Task *t) {
    TaskPool *tp = D_800C51DC;

    tp->freeTop--;
    *tp->freeTop = t;
    t->prev->next = t->next;
    t->next->prev = t->prev;
    return t;
}

/* ------------------------------------------------------- fixed-point matrix */

/* @ 0x8000A924 */
/* guMtxF2L with the arguments swapped (destination first): float 4x4 -> N64 s15.16
 * Mtx (integer halves in m[0..7], fractions in m[8..15]). */
void Mtx_ToFixed(s32 *m, f32 mf[4][4]) {
    s32 *ai = m;
    s32 *af = m + 8;
    s32 i, j, e1, e2;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            e1 = (s32)(mf[i][j * 2] * 65536.0f);
            e2 = (s32)(mf[i][j * 2 + 1] * 65536.0f);
            *ai++ = (e1 & 0xFFFF0000) | ((u32)e2 >> 16);
            *af++ = ((u32)e1 << 16) | (e2 & 0xFFFF);
        }
    }
}

/* @ 0x8000A9E8 */
/* guMtxL2F (destination first): N64 s15.16 Mtx -> float 4x4. Unused. */
void Mtx_FromFixed(f32 mf[4][4], s32 *m) {
    u32 *ai = (u32 *)m;
    u32 *af = (u32 *)(m + 8);   /* endian: read as words (was two u16 reads per word) */
    s32 i, j;
    u32 e1, e2;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 2; j++) {
            e1 = (*ai & 0xFFFF0000) | (*af >> 16);
            e2 = (*ai << 16) | (*af & 0xFFFF);
            mf[i][j * 2] = (f32)(s32)e1 * (1.0f / 65536.0f);
            mf[i][j * 2 + 1] = (f32)(s32)e2 * (1.0f / 65536.0f);
            ai++;
            af++;
        }
    }
}

/* @ 0x8000AAE0 */
/* |x| (abs.s). Unused. */
f32 Math_AbsF(f32 x) {
    return __builtin_fabsf(x);
}

/* -------------------------------------------------- Aleck64 UART (8251-like) */
/* Data register at 0x02000040, status/command at 0x02000042, control at
 * 0x02000044 (mapped by Main_Boot's TLB entry 2). The original is hand-written
 * assembly with busy-wait loops; the loop lengths are kept as spin counts. */

#define UART_DATA   (*(volatile u16 *)0x02000040)
#define UART_STATUS (*(volatile u16 *)0x02000042)
#define UART_CTRL   (*(volatile u16 *)0x02000044)

static void Uart_Spin(s32 n) {
    volatile s32 i = n;

    do {
        i = i - 1;
    } while (i != 0);
}

/* @ 0x8000AAF0 */
/* Reset and configure the UART (three 0 writes, internal reset 0x40, mode 0x4E, command 0x25). */
void Uart_Init(void) {
    UART_CTRL = 0x0B0B;
    Uart_Spin(0x780);
    UART_STATUS = 0;
    Uart_Spin(0x780);
    UART_STATUS = 0;
    Uart_Spin(0x780);
    UART_STATUS = 0;
    Uart_Spin(0x780);
    UART_STATUS = 0x4040;
    Uart_Spin(0x780);
    UART_STATUS = 0x4E;
    Uart_Spin(0x780);
    UART_STATUS = 0x25;
    UART_CTRL = 0x0F04;
    Uart_Spin(0x780);
}

/* @ 0x8000ABA0 */
/* Send a byte: wait (1001 polls max) for TX ready (status bit 0). 0 = sent, -1 = timeout. */
s32 Uart_PutByte(u32 c) {
    s32 tries = 1000;
    s32 ret;

    for (;;) {
        if (UART_STATUS & 1) {
            Uart_Spin(0xF0);
            UART_DATA = (u16)c;
            ret = 0;
            break;
        }
        Uart_Spin(0x3C);
        if (tries-- == 0) {
            ret = -1;
            break;
        }
    }
    Uart_Spin(0x3C);
    return ret;
}

/* @ 0x8000AC24 */
/* Receive a byte: wait (1001 polls max) for RX ready (status bit 1); byte or -1. */
s32 Uart_GetByte(void) {
    s32 tries = 1000;

    for (;;) {
        if (UART_STATUS & 2) {
            Uart_Spin(0x1E);
            return UART_DATA & 0xFF;
        }
        Uart_Spin(0x1E);
        if (tries-- == 0) {
            return -1;
        }
    }
}

/* @ 0x8000AC88 */
u32 Uart_ReadData(void) {
    return UART_DATA & 0xFF;
}

/* @ 0x8000AC98 */
u16 Uart_ReadStatus(void) {
    return UART_STATUS;
}

/* @ 0x8000ACA4 */
/* Busy-wait n+1 short delays. */
void Uart_Delay(s32 n) {
    do {
        Uart_Spin(0x1E);
    } while (n-- != 0);
}

/* ------------------------------------------------------------ boot, threads */

/* @ 0x8000B210 */
/* Entry from the boot code: init libultra, set up the TLB (0x01000000 -> 0xC0000000 1M
 * pages, 0x02000000 -> 0xC0800000 and 0x02010000 -> 0xC0C00000 4K, all uncached),
 * clear the arg flags and start the idle thread. */
void Main_Boot(u8 *cmdline) {
    osInitialize();
    Tlb_ClearAll();
    Tlb_Map(1, 0x01000000, 0xC0000000, 5, 0);
    Tlb_Map(2, 0x02000000, 0xC0800000, 0, 0);
    Tlb_Map(3, 0x02010000, 0xC0C00000, 0, 0);
    Main_ParseArgs(NULL);
    osCreateThread(&D_8003E648, 1, Main_IdleThread, cmdline, D_80041800, 10);
    osStartThread(&D_8003E648);
}

/* @ 0x8000B2D0 */
/* Idle thread (id 1): PI manager, fault thread (id 5) and the main gfx thread (id 6,
 * not started in debug mode), then drop to priority 0 and spin. */
void Main_IdleThread(void *arg) {
    osCreatePiManager(150, &D_80041900, (OSMesg *)D_80041800, 0x40);
    osCreateMesgQueue(&D_80041938, D_80041950, 1);
    osCreateThread(&D_80041958, 5, Fault_Thread, NULL, &D_80043310, 250);
    osStartThread(&D_80041958);
    osCreateThread(&D_8003E498, 6, Main_GfxThread, arg, D_80041000, 11);
    if (gArgDebug == 0) {
        osStartThread(&D_8003E498);
    }
    osSetThreadPri(NULL, 0);
    for (;;) {
    }
}

/* Map N64 pad bits to the cabinet layout: U 1, D 2, L 4, R 8, A 0x10, B 0x20, Start 0x100. */
static u16 Input_MapButtons(u16 b) {
    u16 r = (b >> 11) & 1;

    if (b & 0x400) r |= 2;
    if (b & 0x200) r |= 4;
    if (b & 0x100) r |= 8;
    if (b & 0x8000) r |= 0x10;
    if (b & 0x4000) r |= 0x20;
    if (b & 0x1000) r |= 0x100;
    return r;
}

/* @ 0x8000B3C8 */
/* Input thread (id 8), a scheduler client. On every retrace message (type 1): finish
 * controller reads, update the input state and build the per-player button words
 * (mode D_800B0720: 0 copy, 1 clear, 2 remap pad bits, using the alternate inputs
 * when opposite directions are held), handle coin/credit inputs (max 99), and count
 * retraces. A pre-NMI message (type 4) adds 2 to the frames-in-flight count. */
void Input_Thread(void *arg) {
    OSMesg msg = NULL;
    s16 type;
    s32 i;

    for (;;) {
        osRecvMesg(&D_800B04E0, &msg, 1);
        type = *(s16 *)msg;
        if (type == 1) {
            if (func_8000F750() != 0 && D_80043310 != 0) {
                func_8000F6EC();
                D_80043310 = 0;
                if (D_800B04D0 != 0) {
                    func_8000F3E0();
                }
                if (D_800B0430 != 0) {
                    D_800B04D0 = 0;
                    D_800B4AA4 = 1;
                    func_80021FC0(&D_800C30A8);
                }
            }
            func_8000FA4C();
            func_8000FA34();
            func_8000F8F4();
            switch (D_800B0720) {
            case 0:
                D_800B060C[0] = D_800B0498[0];
                D_800B060C[1] = D_800B0498[1];
                D_800C0D48[0] = D_800C532C[0];
                D_800C0D48[1] = D_800C532C[1];
                D_800AF258[0] = D_800B4A80[0];
                D_800AF258[1] = D_800B4A80[1];
                break;
            case 1:
                D_800B060C[0] = 0;
                D_800B060C[1] = 0;
                D_800C0D48[0] = 0;
                D_800C0D48[1] = 0;
                D_800AF258[0] = 0;
                D_800AF258[1] = 0;
                break;
            case 2:
                for (i = 0; i < 2; i++) {
                    u16 sel = D_800B058C[i];

                    if ((sel & 0xC00) == 0xC00 || (sel & 0x300) == 0x300) {
                        D_800B060C[i] = Input_MapButtons(D_800C5334[i]);
                        D_800C0D48[i] = Input_MapButtons(D_800B0596[i]);
                        D_800AF258[i] = Input_MapButtons(D_800B043A[i]);
                    } else {
                        D_800B060C[i] = Input_MapButtons(sel);
                        D_800C0D48[i] = Input_MapButtons(D_800AF222[i]);
                        D_800AF258[i] = Input_MapButtons(D_800AF238[i]);
                    }
                }
                break;
            }
            if (D_800C1674 == 0) {
                if (D_800C53D8 != 0 || D_800C53DA != 0 || D_800C1898 != 0) {
                    func_8001BB18(0);
                    if (D_800C1898 != 0) {
                        D_800C30C8++;
                    }
                }
                if (D_800C30C8 < 99) {
                    D_800C1678 = 0xFFFF;
                } else {
                    D_800C30C8 = 99;
                    D_800C1678 = 0xFFF3;
                }
            }
            if (D_800B0574 != 0 || D_800B0576 != 0) {
                D_800AF260 = 1;
            }
            D_80043314++;
        } else if (type == 4) {
            D_800C53F8 += 2;
        }
    }
}

/* End-of-frame housekeeping shared by both paths of Main_GfxThread. */
static void Main_EndFrame(void) {
    func_8001BE98(0x82);
    Ring_SetStop(&gSceneNodePool);
    AllocList_FreeAll(&gGameHeap, &D_800B0578);
    Scene_ConvertMatrices(gSceneRoot0);
    Scene_ConvertMatrices(gSceneRoot1);
    Scene_ConvertMatrices(gSceneRoot2);
    Scene_ConvertMatrices(gSceneRoot3);
    func_8001BE98(0xC2);
    func_8001BEA0();
}

/* @ 0x8000BD28 */
/* Main gfx thread (id 6): initialise all systems, start the controller (id 7) and
 * input (id 8) threads, then loop on the scheduler client queue: on a retrace (type 1),
 * if fewer than 2 frames are in flight and enough retraces have passed, build the next
 * frame into the alternate gfx buffer; on a task-done message (type 2) retire a frame. */
void Main_GfxThread(void *arg) {
    OSMesg msg;
    s16 type;

    D_80043314 = 2;
    msg = NULL;
    D_800C51CC = 1;
    D_800C0AB8 = 0;
    Main_InitSystems();
    ObjList_Init();
    D_80043310 = 0;
    osCreateThread(&D_8003E138, 7, Cont_Thread, arg, D_8003F000, 12);
    osStartThread(&D_8003E138);
    osCreateThread(&D_8003E2E8, 8, Input_Thread, arg, D_8003F800, 13);
    osStartThread(&D_8003E2E8);
    for (;;) {
        osRecvMesg(&D_800C30E0, &msg, 1);
        type = *(s16 *)msg;
        if (type == 1) {
            if (D_800C53F8 < 2 && D_80043314 >= D_800C51CC) {
                D_80043310 = 1;
                D_80043314 = 0;
                func_8000FF64(&D_800B0730[D_800C0AB8 * 0x2180]);
                func_8001BE98(0xC1);
                if (D_800C53F8 == 0) {
                    Main_EndFrame();
                }
                D_800C53F8++;
                D_800C0AB8 ^= 1;
            }
        } else if (type == 2) {
            if (D_800C53F8 >= 2) {
                Main_EndFrame();
            }
            if (D_800C53F8 != 0) {
                D_800C53F8--;
            }
        }
    }
}

/* ---------------------------------------------------------- ROM / overlays */

/* @ 0x8000BFE8 */
/* Blocking PI DMA from ROM to RAM in 1 KB pieces. */
void Rom_Read(u32 romAddr, void *dst, s32 size) {
    OSIoMesg mb;
    OSMesg msg;
    u8 *d = dst;

    osInvalDCache(dst, size);
    while (size >= 0x400) {
        osPiStartDma(&mb, 0, 0, romAddr, d, 0x400, &D_80041918);
        osRecvMesg(&D_80041918, &msg, 1);
        romAddr += 0x400;
        size -= 0x400;
        d += 0x400;
    }
    if (size != 0) {
        osPiStartDma(&mb, 0, 0, romAddr, d, size, &D_80041918);
        osRecvMesg(&D_80041918, &msg, 1);
    }
}

#define OVERLAY_RAM  ((u32 *)0x800E0000)
#define OVERLAY_SIZE 0x4D000

/* @ 0x8000C0D4 */
/* Load overlay 'index' (gOverlayTable entry {vram, ?, romStart, romEnd}): clear the
 * whole 0x4D000-byte overlay area, flush caches, then DMA it like Rom_Read. */
void Overlay_Load(s32 index) {
    OverlayEntry *ov = &gOverlayTable[index];
    u32 rom = ov->romStart;
    u8 *d = (u8 *)(uintptr_t)ov->vram;
    s32 size = ov->romEnd - rom;
    u32 *p = OVERLAY_RAM;
    s32 i;
    OSIoMesg mb;
    OSMesg msg;

    for (i = OVERLAY_SIZE / 4; i != 0; i--) {
        *p++ = 0;
    }
    osWritebackDCacheAll();
    osInvalDCache(OVERLAY_RAM, OVERLAY_SIZE);
    osInvalICache(OVERLAY_RAM, OVERLAY_SIZE);
    while (size >= 0x400) {
        osPiStartDma(&mb, 0, 0, rom, d, 0x400, &D_80041918);
        osRecvMesg(&D_80041918, &msg, 1);
        rom += 0x400;
        size -= 0x400;
        d += 0x400;
    }
    if (size != 0) {
        osPiStartDma(&mb, 0, 0, rom, d, size, &D_80041918);
        osRecvMesg(&D_80041918, &msg, 1);
    }
}

/* @ 0x8000C218 */
/* Never called: references some libultra functions (with zero arguments) so they get linked. */
void Main_ForceLink(void) {
    func_80025DC0(0, 0);
    __osContRamRead(NULL, 0, 0, NULL);
    func_8001D350(0, 0);
    func_80021EA0(0, 0);
    func_80024750(0, 0, 0);
    func_80023F90(0, 0, 0);
    func_800221A0(0, 0);
    func_800236C0(0, 0, 0, 0, 0);
}

/* @ 0x8000C2A8 */
/* Initialise message queues, camera/light, the 0x52B000-byte game heap, scene graph,
 * scheduler (pri 15, VI mode 2, 1 field) and its two clients, audio and the task pool. */
void Main_InitSystems(void) {
    osCreateMesgQueue(&D_80041918, D_80041930, 1);
    osCreateMesgQueue(&D_800B04E0, D_800C1798, 0x40);
    osCreateMesgQueue(&D_800C30E0, D_800B0620, 0x40);
    Camera_Init();
    Light_Init(&D_800C1730);
    Heap_Init(&gGameHeap, 0x52B000);
    AllocList_Init(&D_800B0578, &D_800B0558);
    Scene_Init();
    Sched_Create(&D_800C0AC0, D_800C51C0, 15, 2, 1);
    D_800B0568 = Sched_GetCmdQ(&D_800C0AC0);
    func_8000F890();
    D_800C16E0 = 1;
    func_8000F350();
    func_80014E90();
    func_800154D0();
    func_8001BE80();
    func_8000FDB0();
    Audio_Init();
    Sched_AddClient(&D_800C0AC0, &D_800B054C, &D_800C30E0);
    Sched_AddClient(&D_800C0AC0, &D_800C165C, &D_800B04E0);
    Task_InitPool(D_8069E600, 0x114000, 0x4000);
    func_8001BF08();
}

/* @ 0x8000C424 */
/* Controller thread (id 7): waits for SI messages of type 5; after a controller read
 * either starts the next read (if enabled) or waits on D_800B0520. D_800C51D0 shows
 * where it is blocked (1 = SI queue, 2 = D_800B0520). */
void Cont_Thread(void *arg) {
    OSMesg msg = NULL;

    for (;;) {
        D_800C51D0 = 1;
        osRecvMesg(&D_800C30A8, &msg, 1);
        D_800C51D0 = 0;
        if (*(s16 *)msg != 5) {
            continue;
        }
        D_800B4AA4 = 0;
        if (func_8000F750() != 0) {
            D_800B04D0 = 1;
            if (D_800AF254 != 0) {
                __osContRamRead(&D_800C30A8, 0, 0, D_800C16C0);
            }
        } else {
            D_800C51D0 = 2;
            osRecvMesg(&D_800B0520, &msg, 1);
            D_800C51D0 = 0;
        }
    }
}

/* @ 0x8000C510 */
/* Fault thread (id 5): wait for OS_EVENT_FAULT, then show the faulted thread. */
void Fault_Thread(void *arg) {
    OSMesg msg = NULL;
    void *t;

    osSetEventMesg(12, &D_80041938, (OSMesg)0x10);
    D_8003E134 = 0;
    osRecvMesg(&D_80041938, &msg, 1);
    D_8003E130 = t = func_8001D340();
    if (t != NULL) {
        func_8000D150(t);
    }
    for (;;) {
    }
}

/* ------------------------------------------------------- RCP scheduler (sched.c) */

#define SC_RETRACE_MSG  0x29A
#define SC_RSP_DONE_MSG 0x29B
#define SC_RDP_DONE_MSG 0x29C
#define SC_PRE_NMI_MSG  0x29D
#define SC_NEEDS_RDP    1
#define SC_NEEDS_RSP    2
#define SC_YIELD        0x10
#define SC_YIELDED      0x20
#define M_GFXTASK       1
#define M_AUDTASK       2

/* @ 0x8000C590 */
/* osScCreateScheduler (doAudio is not cleared, as in the original). Also blanks the screen. */
void Sched_Create(OSSched *sc, void *stack, s32 pri, u8 mode, u8 numFields) {
    sc->retraceMsg.type = 1;
    sc->curRSPTask = NULL;
    sc->curRDPTask = NULL;
    sc->clientList = NULL;
    sc->frameCount = 0;
    sc->audioListHead = NULL;
    sc->gfxListHead = NULL;
    sc->audioListTail = NULL;
    sc->gfxListTail = NULL;
    sc->prenmiMsg.type = 4;
    osCreateMesgQueue(&sc->interruptQ, sc->intBuf, 8);
    osCreateMesgQueue(&sc->cmdQ, sc->cmdMsgBuf, 8);
    osCreateViManager(0xFE);
    osViSetMode(osViModeTable[mode]);
    osViBlack(1);
    osSetEventMesg(4, &sc->interruptQ, (OSMesg)SC_RSP_DONE_MSG);
    osSetEventMesg(9, &sc->interruptQ, (OSMesg)SC_RDP_DONE_MSG);
    osSetEventMesg(14, &sc->interruptQ, (OSMesg)SC_PRE_NMI_MSG);
    osViSetEvent(&sc->interruptQ, (OSMesg)SC_RETRACE_MSG, numFields);
    osCreateThread(&sc->thread, 4, Sched_Main, sc, stack, pri);
    osStartThread(&sc->thread);
}

/* @ 0x8000C6D8 */
void Sched_AddClient(OSSched *sc, OSScClient *c, OSMesgQueue *msgQ) {
    u32 mask = osSetIntMask(1);

    c->msgQ = msgQ;
    c->next = sc->clientList;
    sc->clientList = c;
    osSetIntMask(mask);
}

/* @ 0x8000C730 */
void Sched_RemoveClient(OSSched *sc, OSScClient *c) {
    OSScClient *client = sc->clientList;
    OSScClient *prev = NULL;
    u32 mask = osSetIntMask(1);

    while (client != NULL) {
        if (client == c) {
            if (prev != NULL) {
                prev->next = c->next;
            } else {
                sc->clientList = c->next;
            }
            break;
        }
        prev = client;
        client = client->next;
    }
    osSetIntMask(mask);
}

/* @ 0x8000C7C0 */
OSMesgQueue *Sched_GetCmdQ(OSSched *sc) {
    return &sc->cmdQ;
}

/* @ 0x8000C7C8 */
/* Scheduler thread. On pre-NMI: wait 8 retraces, then over the next 8 messages wipe
 * every 8th column of both frame buffers (one column phase per retrace), then idle. */
void Sched_Main(void *arg) {
    OSSched *sc = arg;
    OSMesg msg = NULL;
    s32 n, x, y;

    for (;;) {
        osRecvMesg(&sc->interruptQ, &msg, 1);
        switch ((s32)(intptr_t)msg) {
        case SC_RETRACE_MSG:
            func_8001BE90();
            func_8001BE88();
            func_8001BE98(0x81);
            Sched_HandleRetrace(sc);
            func_8001BE98(0xC1);
            continue;
        case SC_RSP_DONE_MSG:
            Sched_HandleRSP(sc);
            continue;
        case SC_RDP_DONE_MSG:
            Sched_HandleRDP(sc);
            continue;
        case SC_PRE_NMI_MSG:
            break;
        default:
            continue;
        }
        break;
    }

    n = 8;
    do {
        osRecvMesg(&sc->interruptQ, &msg, 1);
        n -= ((s32)(intptr_t)msg == SC_RETRACE_MSG);
    } while (n != 0);

    do {
        osRecvMesg(&sc->interruptQ, &msg, 1);
        if ((s32)(intptr_t)msg == SC_RETRACE_MSG) {
            for (y = 239; y >= 0; y--) {
                for (x = n; x < gScreenWidth; x += 8) {
                    D_807B2600[y * gScreenWidth + x] = 0;
                    D_807D7E00[y * gScreenWidth + x] = 0;
                    osWritebackDCache(&D_807B2600[y * gScreenWidth + x], 2);
                    osWritebackDCache(&D_807D7E00[y * gScreenWidth + x], 2);
                }
            }
        }
        n++;
    } while (n < 8);

    for (;;) {
        osRecvMesg(&sc->interruptQ, &msg, 1);
    }
}

/* @ 0x8000C9D8 */
/* __scHandleRetrace: queue new tasks, yield or schedule, notify all clients. */
void Sched_HandleRetrace(OSSched *sc) {
    OSScTask *rspTask = NULL;
    OSScTask *sp = NULL;
    OSScTask *dp = NULL;
    OSScClient *client;
    s32 state;

    sc->frameCount++;
    while (osRecvMesg(&sc->cmdQ, (OSMesg *)&rspTask, 0) != -1) {
        Sched_AppendList(sc, rspTask);
    }
    if (sc->doAudio && sc->curRSPTask != NULL) {
        Sched_Yield(sc);
    } else {
        state = ((sc->curRSPTask == NULL) << 1) | (sc->curRDPTask == NULL);
        if (Sched_Schedule(sc, &sp, &dp, state) != state) {
            Sched_Exec(sc, sp, dp);
        }
    }
    for (client = sc->clientList; client != NULL; client = client->next) {
        osSendMesg(client->msgQ, (OSMesg)sc, 0);
    }
}

/* @ 0x8000CADC */
/* __scHandleRSP: RSP done (or yielded: re-queue XBUS gfx tasks at the front). */
void Sched_HandleRSP(OSSched *sc) {
    OSScTask *t, *sp = NULL, *dp = NULL;
    s32 state;

    t = sc->curRSPTask;
    sc->curRSPTask = NULL;
    if ((t->state & SC_YIELD) && osSpTaskYielded(&t->list)) {
        t->state |= SC_YIELDED;
        if ((t->flags & 7) == 3) {
            t->next = sc->gfxListHead;
            sc->gfxListHead = t;
            if (sc->gfxListTail == NULL) {
                sc->gfxListTail = t;
            }
        }
    } else {
        t->state &= ~SC_NEEDS_RSP;
        Sched_TaskComplete(sc, t);
    }
    state = ((sc->curRSPTask == NULL) << 1) | (sc->curRDPTask == NULL);
    if (Sched_Schedule(sc, &sp, &dp, state) != state) {
        Sched_Exec(sc, sp, dp);
    }
}

/* @ 0x8000CBCC */
/* __scHandleRDP */
void Sched_HandleRDP(OSSched *sc) {
    OSScTask *t, *sp = NULL, *dp = NULL;
    s32 state;

    t = sc->curRDPTask;
    sc->curRDPTask = NULL;
    t->state &= ~SC_NEEDS_RDP;
    Sched_TaskComplete(sc, t);
    state = ((sc->curRSPTask == NULL) << 1) | (sc->curRDPTask == NULL);
    if (Sched_Schedule(sc, &sp, &dp, state) != state) {
        Sched_Exec(sc, sp, dp);
    }
}

/* @ 0x8000CC5C */
/* __scTaskReady: t if no frame buffer swap is pending, else NULL. */
OSScTask *Sched_TaskReady(OSScTask *t) {
    if (t != NULL) {
        return osViGetCurrentFramebuffer() == osViGetNextFramebuffer() ? t : NULL;
    }
    return NULL;
}

/* @ 0x8000CCB0 */
/* __scTaskComplete: when both RSP and RDP are done, notify and (last gfx task) swap
 * buffers, un-blanking the screen the first time. Keeps the RCP busy counter. */
s32 Sched_TaskComplete(OSSched *sc, OSScTask *t) {
    if ((t->state & (SC_NEEDS_RSP | SC_NEEDS_RDP)) == 0) {
        osSendMesg(t->msgQ, t->msg, 1);
        if (t->list.type == M_GFXTASK) {
            if ((t->flags & 0x60) == 0x60) {
                if (D_80038478 != 0) {
                    osViBlack(0);
                    D_80038478 = 0;
                }
                osViSwapBuffer(t->framebuffer);
            }
            if (D_80043320 != 0) {
                D_80043320--;
            }
            func_8001BE98(0x4A);
        } else {
            func_8001BE98(0x48);
        }
        return 1;
    }
    if (D_80043320 != 0) {
        D_80043320--;
    }
    func_8001BE98(0x49);
    return 0;
}

/* @ 0x8000CD8C */
/* __scAppendList */
void Sched_AppendList(OSSched *sc, OSScTask *t) {
    if (t->list.type == M_AUDTASK) {
        if (sc->audioListTail != NULL) {
            sc->audioListTail->next = t;
        } else {
            sc->audioListHead = t;
        }
        sc->audioListTail = t;
        sc->doAudio = 1;
    } else {
        if (sc->gfxListTail != NULL) {
            sc->gfxListTail->next = t;
        } else {
            sc->gfxListHead = t;
        }
        sc->gfxListTail = t;
    }
    t->next = NULL;
    t->state = t->flags & 3;
}

/* @ 0x8000CDE4 */
/* __scExec: start sp on the RSP and/or dp on the RDP (always flushes the D-cache). */
void Sched_Exec(OSSched *sc, OSScTask *sp, OSScTask *dp) {
    if (sp != NULL) {
        osWritebackDCacheAll();
        if (sp->list.type == M_AUDTASK) {
            if (D_80043320 == 1) {
                func_8001BE98(0x4A);
            } else if (D_80043320 == 2) {
                func_8001BE98(0x49);
            }
            func_8001BE98(0x8);
        } else {
            func_8001BE98(0x9);
            D_80043320 = 2;
        }
        sp->state &= ~(SC_YIELD | SC_YIELDED);
        osSpTaskLoad(&sp->list);
        osSpTaskStartGo(&sp->list);
        sc->curRSPTask = sp;
        if (sp == dp) {
            sc->curRDPTask = dp;
        }
    }
    if (dp != NULL && dp != sp) {
        osDpSetNextBuffer(dp->list.output_buff, *dp->list.output_buff_size);
        D_80038470 = 1;
        D_80038474 = 0;
        sc->curRDPTask = dp;
    }
}

/* @ 0x8000CEF8 */
/* __scYield: ask a running gfx task to yield. */
void Sched_Yield(OSSched *sc) {
    if (sc->curRSPTask->list.type == M_GFXTASK) {
        sc->curRSPTask->state |= SC_YIELD;
        osSpTaskYield();
    }
}

/* @ 0x8000CF30 */
/* __scSchedule: pick tasks for the free units (avail bit 2 = RSP, bit 1 = RDP), recursing
 * while anything changes. Task type (flags & 7): 3 = XBUS (RSP+RDP together, may be
 * yielded), 2/6/7 = RSP then RDP separately, others never start here. */
s32 Sched_Schedule(OSSched *sc, OSScTask **sp, OSScTask **dp, s32 availRCP) {
    s32 avail = availRCP;
    OSScTask *gfx = sc->gfxListHead;
    OSScTask *audio = sc->audioListHead;

    if (sc->doAudio && (avail & SC_NEEDS_RSP)) {
        if (gfx != NULL && (gfx->flags & 0x10)) {
            *sp = gfx;
            avail &= ~SC_NEEDS_RSP;
        } else {
            *sp = audio;
            avail &= ~SC_NEEDS_RSP;
            sc->doAudio = 0;
            sc->audioListHead = sc->audioListHead->next;
            if (sc->audioListHead == NULL) {
                sc->audioListTail = NULL;
            }
        }
    } else if (Sched_TaskReady(gfx) != NULL) {
        switch (gfx->flags & 7) {
        case 3:
            if (gfx->state & SC_YIELDED) {
                if (avail & SC_NEEDS_RSP) {
                    *sp = gfx;
                    avail &= ~SC_NEEDS_RSP;
                    if (gfx->state & SC_NEEDS_RDP) {
                        *dp = gfx;
                        avail &= ~SC_NEEDS_RDP;
                    }
                    sc->gfxListHead = sc->gfxListHead->next;
                    if (sc->gfxListHead == NULL) {
                        sc->gfxListTail = NULL;
                    }
                }
            } else if (avail == (SC_NEEDS_RSP | SC_NEEDS_RDP)) {
                *dp = gfx;
                *sp = gfx;
                sc->gfxListHead = sc->gfxListHead->next;
                avail = 0;
                if (sc->gfxListHead == NULL) {
                    sc->gfxListTail = NULL;
                }
            }
            break;
        case 2:
        case 6:
        case 7:
            if (gfx->state & SC_NEEDS_RSP) {
                if (avail & SC_NEEDS_RSP) {
                    *sp = gfx;
                    avail &= ~SC_NEEDS_RSP;
                }
            } else if (gfx->state & SC_NEEDS_RDP) {
                if (avail & SC_NEEDS_RDP) {
                    *dp = gfx;
                    avail &= ~SC_NEEDS_RDP;
                    sc->gfxListHead = sc->gfxListHead->next;
                    if (sc->gfxListHead == NULL) {
                        sc->gfxListTail = NULL;
                    }
                }
            }
            break;
        default:
            break;
        }
    }
    if (avail != availRCP) {
        avail = Sched_Schedule(sc, sp, dp, avail);
    }
    return avail;
}
