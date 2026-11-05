/* Vivid Dolls decompilation - system code (0x8000A5A0-0x8000D000):
 * TLB setup, the cooperative task pool, fixed-point matrix conversion, the
 * Aleck64 8251 UART, boot and thread entry points, ROM/overlay DMA, the
 * input thread, and the libultra-style RCP scheduler (sched.c).
 *
 * Not decompiled (hand-written assembly, see docs/notes/system.md):
 *   0x8000A910 (spills a0-a3 to the caller's arg area), 0x8000ACD0 Tlb_Read,
 *   0x8000ADA4 Tlb_Write, 0x8000AE90 Task_Exit, 0x8000AEAC Task_Yield,
 *   0x8000AF94 Task_SwitchNext, 0x8000B06C Task_RunAll, 0x8000B150 Task_SaveContext.
 */
#ifndef SYSTEM_H
#define SYSTEM_H

#include "vd_common.h"

/* ---- libultra-compatible types (only the parts the code touches) ---- */
typedef void *OSMesg;
typedef struct { u8 data[0x18]; } OSMesgQueue;
typedef struct { u8 data[0x1B0]; } OSThread;

typedef struct {                 /* OSTask_t */
    u32  type;                   /* 0x00: 1 = M_GFXTASK, 2 = M_AUDTASK */
    u32  flags;
    u32  ucode_boot, ucode_boot_size, ucode, ucode_size, ucode_data, ucode_data_size;
    u32  dram_stack, dram_stack_size;
    u64 *output_buff;            /* 0x28 */
    u64 *output_buff_size;       /* 0x2C */
    u32  data_ptr, data_size, yield_data_ptr, yield_data_size;
} OSTask;                        /* 0x40 */

typedef struct OSScTask {
    struct OSScTask *next;       /* 0x00 */
    u32  state;                  /* 0x04: OS_SC_NEEDS_RDP 1, NEEDS_RSP 2, YIELD 0x10, YIELDED 0x20 */
    u32  flags;                  /* 0x08: type in bits 0-2, PARALLEL 0x10, LAST_TASK 0x20, SWAPBUFFER 0x40 */
    void *framebuffer;           /* 0x0C */
    OSTask list;                 /* 0x10 */
    OSMesgQueue *msgQ;           /* 0x50 */
    OSMesg msg;                  /* 0x54 */
} OSScTask;

typedef struct OSScClient {
    struct OSScClient *next;
    OSMesgQueue *msgQ;
} OSScClient;

typedef struct {
    s16  type;
    u8   misc[30];
} OSScMsg;

typedef struct {
    OSScMsg     retraceMsg;      /* 0x000: type 1 */
    OSScMsg     prenmiMsg;       /* 0x020: type 4 */
    OSMesgQueue interruptQ;      /* 0x040 */
    OSMesg      intBuf[8];       /* 0x058 */
    OSMesgQueue cmdQ;            /* 0x078 */
    OSMesg      cmdMsgBuf[8];    /* 0x090 */
    OSThread    thread;          /* 0x0B0 */
    OSScClient *clientList;      /* 0x260 */
    OSScTask   *audioListHead;   /* 0x264 */
    OSScTask   *gfxListHead;     /* 0x268 */
    OSScTask   *audioListTail;   /* 0x26C */
    OSScTask   *gfxListTail;     /* 0x270 */
    OSScTask   *curRSPTask;      /* 0x274 */
    OSScTask   *curRDPTask;      /* 0x278 */
    u32         frameCount;      /* 0x27C */
    s32         doAudio;         /* 0x280 */
} OSSched;

/* ---- cooperative task pool (D_800C51DC points at the manager) ---- */
typedef struct Task {
    struct Task *next;           /* 0x0 */
    struct Task *prev;           /* 0x4 */
    u32 *sp;                     /* 0x8: saved context frame (0xC0 bytes) */
    s32  prio;                   /* 0xC: list is sorted by ascending prio */
} Task;

typedef struct {
    u32    blockSize;            /* 0x00: bytes per task block (task header + stack) */
    u32    unk4;
    Task **freeTop;              /* 0x08: stack of free blocks, NULL-terminated */
    Task   head;                 /* 0x0C: sentinel before the first task */
    Task   tail;                 /* 0x1C: sentinel after the last task (scheduler context) */
    Task  *freeList[1];          /* 0x2C: n block pointers + NULL */
} TaskPool;

typedef struct {
    u32 entryHi, entryLo0, entryLo1, pageMask;
} TlbEntry;

void  Tlb_ClearAll(void);                                                      /* 0x8000A5A0 */
s32   Tlb_Map(s32 index, u32 vaddr, u32 paddr, s32 sizeIdx, s32 cached);       /* 0x8000A628 */
void  Task_InitPool(u8 *pool, u32 poolSize, u32 blockSize);                    /* 0x8000A6D0 */
Task *Task_Create(void (*func)(), s32 prio, s32 argBytes, ...);                /* 0x8000A76C */
Task *Task_Free(Task *t);                                                      /* 0x8000A8D0 */
void  Mtx_ToFixed(s32 *m, f32 mf[4][4]);                                       /* 0x8000A924 */
void  Mtx_FromFixed(f32 mf[4][4], s32 *m);                                     /* 0x8000A9E8 */
f32   Math_AbsF(f32 x);                                                        /* 0x8000AAE0 */
void  Uart_Init(void);                                                         /* 0x8000AAF0 */
s32   Uart_PutByte(u32 c);                                                     /* 0x8000ABA0 */
s32   Uart_GetByte(void);                                                      /* 0x8000AC24 */
u32   Uart_ReadData(void);                                                     /* 0x8000AC88 */
u16   Uart_ReadStatus(void);                                                   /* 0x8000AC98 */
void  Uart_Delay(s32 n);                                                       /* 0x8000ACA4 */
void  Main_Boot(u8 *cmdline);                                                  /* 0x8000B210 */
void  Main_IdleThread(void *arg);                                              /* 0x8000B2D0 */
void  Input_Thread(void *arg);                                                 /* 0x8000B3C8 */
void  Main_GfxThread(void *arg);                                               /* 0x8000BD28 */
void  Rom_Read(u32 romAddr, void *dst, s32 size);                              /* 0x8000BFE8 */
void  Overlay_Load(s32 index);                                                 /* 0x8000C0D4 */
void  Main_ForceLink(void);                                                    /* 0x8000C218 */
void  Main_InitSystems(void);                                                  /* 0x8000C2A8 */
void  Cont_Thread(void *arg);                                                  /* 0x8000C424 */
void  Fault_Thread(void *arg);                                                 /* 0x8000C510 */
void  Sched_Create(OSSched *sc, void *stack, s32 pri, u8 mode, u8 numFields);  /* 0x8000C590 */
void  Sched_AddClient(OSSched *sc, OSScClient *c, OSMesgQueue *msgQ);          /* 0x8000C6D8 */
void  Sched_RemoveClient(OSSched *sc, OSScClient *c);                          /* 0x8000C730 */
OSMesgQueue *Sched_GetCmdQ(OSSched *sc);                                       /* 0x8000C7C0 */
void  Sched_Main(void *arg);                                                   /* 0x8000C7C8 */
void  Sched_HandleRetrace(OSSched *sc);                                        /* 0x8000C9D8 */
void  Sched_HandleRSP(OSSched *sc);                                            /* 0x8000CADC */
void  Sched_HandleRDP(OSSched *sc);                                            /* 0x8000CBCC */
OSScTask *Sched_TaskReady(OSScTask *t);                                        /* 0x8000CC5C */
s32   Sched_TaskComplete(OSSched *sc, OSScTask *t);                            /* 0x8000CCB0 */
void  Sched_AppendList(OSSched *sc, OSScTask *t);                              /* 0x8000CD8C */
void  Sched_Exec(OSSched *sc, OSScTask *sp, OSScTask *dp);                     /* 0x8000CDE4 */
void  Sched_Yield(OSSched *sc);                                                /* 0x8000CEF8 */
s32   Sched_Schedule(OSSched *sc, OSScTask **sp, OSScTask **dp, s32 availRCP); /* 0x8000CF30 */

#endif
