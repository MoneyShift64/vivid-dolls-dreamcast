/* Vivid Dolls port - libultra shim on KallistiOS: internal declarations.
 *
 * The game C sees libultra structures only as opaque blobs (system.h:
 * OSMesgQueue = 0x18 bytes, OSThread = 0x1B0 bytes), so the shim keeps its
 * own state inside them (VdMesgQueue / VdThread below, checked to fit).
 *
 * Scheduling model (port/notes/sys.md): every libultra thread is a KOS thread
 * whose KOS priority follows the libultra one (higher libultra number = lower
 * KOS number = runs first). Blocking in osRecvMesg/osSendMesg sleeps on the
 * thread's own genwait object; a send/receive that wakes a thread with a
 * higher priority than the caller hands the CPU over at once (thd_pass in
 * thread context, thd_schedule_next from an interrupt), as libultra does.
 */
#ifndef VD_ULTRA_INTERNAL_H
#define VD_ULTRA_INTERNAL_H

#include <stdint.h>
#include <stddef.h>

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;
typedef float    f32;
typedef double   f64;

typedef void *OSMesg;

/* ---- libultra constants ---- */
#define OS_MESG_NOBLOCK 0
#define OS_MESG_BLOCK   1

#define OS_EVENT_SW1      0
#define OS_EVENT_SW2      1
#define OS_EVENT_CART     2
#define OS_EVENT_COUNTER  3
#define OS_EVENT_SP       4
#define OS_EVENT_SI       5
#define OS_EVENT_AI       6
#define OS_EVENT_VI       7
#define OS_EVENT_PI       8
#define OS_EVENT_DP       9
#define OS_EVENT_CPU_BREAK 10
#define OS_EVENT_SP_BREAK 11
#define OS_EVENT_FAULT    12
#define OS_EVENT_THREADSTATUS 13
#define OS_EVENT_PRENMI   14
#define OS_NUM_EVENTS     15

#define OS_IM_NONE 0x00000001u
#define OS_IM_ALL  0x003FFF01u

#define M_GFXTASK 1
#define M_AUDTASK 2

/* ---- shim structures stored inside the game's opaque blobs ---- */
struct VdThread;

typedef struct VdMesgQueue {          /* same field order as libultra's OSMesgQueue */
    struct VdThread *mtqueue;         /* threads blocked on an empty queue */
    struct VdThread *fullqueue;       /* threads blocked on a full queue */
    volatile s32 validCount;
    s32 first;
    s32 msgCount;
    OSMesg *msg;
} VdMesgQueue;                        /* 0x18 */

#define VD_THREAD_MAGIC 0x56445448u   /* 'VDTH' */

typedef struct VdThread {
    struct VdThread *next;            /* link in a wait list (priority order) */
    s32 priority;                     /* libultra priority */
    struct VdThread **queue;          /* wait list this thread is on, or NULL */
    void *kt;                         /* kthread_t * (NULL until started) */
    void (*entry)(void *);
    void *arg;
    s32 id;
    u32 magic;
    volatile s32 woken;               /* set by the waker, checked by the sleeper */
    volatile s32 go;                  /* start gate (osStartThread) */
    volatile s32 started;
    struct VdThread *all_next;        /* registry of all created threads */
    void *stack_mem;                  /* stack allocated by the shim */
} VdThread;

/* ---- threads / queues (os_thread.c) ---- */
void       vd_ultra_threads_init(void);
VdThread  *vd_thread_self(void);              /* never NULL (non-libultra callers share one record) */
int        vd_kos_prio(s32 ultra_pri);        /* libultra priority -> KOS priority */
void       vd_kill_all_threads(void);         /* soft reset: destroy every libultra thread */
void       vd_post_event(int event);          /* send the osSetEventMesg message, non-blocking */
int        vd_try_post_event(int event);      /* same, returns -1 if the queue is full */
/* Lowest address any thread's sp may reach (the game's task stacks in the task
 * pool): KOS asserts on sp < stack base, so thread stacks are declared from here. */
extern void *vd_stack_floor;

/* libultra API implemented by the shim (names as in libultra) */
void osCreateThread(void *t, s32 id, void (*entry)(void *), void *arg, void *sp, s32 pri);
void osStartThread(void *t);
void osSetThreadPri(void *t, s32 pri);
s32  osGetThreadPri(void *t);
void osCreateMesgQueue(void *mq, OSMesg *msg, s32 count);
s32  osSendMesg(void *mq, OSMesg msg, s32 flag);
s32  osJamMesg(void *mq, OSMesg msg, s32 flag);
s32  osRecvMesg(void *mq, OSMesg *msg, s32 flag);
void osSetEventMesg(u32 event, void *mq, OSMesg msg);
u32  osSetIntMask(u32 mask);

/* ---- VI (os_vi.c) ---- */
void  vd_ultra_vi_init(void);                 /* installs the vblank handler (once) */
void  vd_ultra_vi_reset(void);
extern volatile u32 vd_vi_retrace_count;      /* vblanks since boot */
extern volatile int vd_vi_black;              /* osViBlack state */
void *vd_vi_current_fb(void);

/* ---- PI / ROM (os_pi.c) ---- */
extern const u8 *vd_rom_image;                /* .z64 byte order, vd_rom_size bytes */
extern u32 vd_rom_size;

/* ---- SP / RDP (os_sp.c) ---- */
void vd_ultra_sp_init(void);
extern volatile int vd_gfx_busy;              /* 1 while the F3D interpreter runs a task */
extern volatile int vd_reset_pending;         /* set by the soft reset: tasks complete without drawing */

/* ---- controllers / cabinet I/O (os_cont.c) ---- */
void vd_input_vblank(void);                   /* called from the vblank interrupt */
void vd_ultra_cont_reset(void);
extern volatile u16 vd_dip_switches;          /* DIP word as the game sees it (1 = ON), default 0 */
volatile u16 *vd_io_reg(u32 addr);            /* Aleck64 I/O register at 0x02000000+ */
extern void (*vd_soft_reset_request)(void);   /* called (from the interrupt) on A+B+X+Y+Start */

/* ---- EEPROM, RAM only (os_eeprom.c) ---- */
void vd_eeprom_init(void);                    /* nothing to do: the game never saves */

/* ---- audio library stubs (al_stub.c) ---- */
void vd_audio_stub_reset(void);

/* ---- misc ---- */
#define VD_ALIAS(name, target) \
    __typeof__(target) name __attribute__((alias(#target)))

#define VD_STATIC_ASSERT(cond, name) typedef char vd_static_assert_##name[(cond) ? 1 : -1]

#endif
