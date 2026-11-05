/* Vivid Dolls port - RSP/RDP task start (osSpTaskLoad/StartGo) and their
 * completion events.
 *
 * The RCP is a separate processor on the N64: osSpTaskStartGo only starts a task,
 * and the SP/DP "done" interrupts arrive later. The port does the same with an
 * "RCP" KOS thread: osSpTaskStartGo queues a job and returns at once; the RCP
 * thread runs the F3D interpreter on the display list (one PVR scene per task;
 * the PVR calls may block for a few vblanks in pvr_wait_ready), then posts the
 * SP-done event and, for XBUS tasks (OSScTask flags & 7 == 3, the only kind the
 * game sends: flags 0x63), the DP-done event too.
 *
 * Why not run F3D in the scheduler thread (first version): while F3D waited for
 * the PVR, the vblank kept queueing retrace messages into the scheduler's
 * 8-entry interrupt queue; once it was full, the SP/DP-done messages (sent
 * NOBLOCK, as libultra's event dispatch does) were dropped, the frame never
 * retired, and the game froze on the boot logo. The RCP thread therefore never
 * drops a completion: if the queue is full it waits until the scheduler has
 * taken a message (on the N64 the interrupt would stay pending as well).
 *
 * Audio tasks have nothing to execute (the synthesizer mixed in alAudioFrame):
 * their SP done is posted at once, from osSpTaskStartGo itself (NOBLOCK into
 * the scheduler's own queue; only if that queue is full it goes through the RCP
 * thread, which waits for room). For tasks whose RDP part is started separately
 * (types 2/6/7) DP done is posted by osDpSetNextBuffer via the RCP thread.
 * Tasks never yield (osSpTaskYielded = 0).
 *
 * XBUS gfx tasks: SP done is posted when the RCP thread STARTS the task and DP
 * done when F3D is over. To the scheduler this is an RSP that finished and an
 * RDP still drawing: the task completes (message to the game, buffer swap) only
 * with DP done, exactly as before, and the next gfx task still waits for the
 * RDP. But the RSP is free for the audio task of the next retrace. Before, the
 * audio task had to wait until F3D was over (the scheduler "yields" a running
 * gfx task, a no-op here, and waits for its SP done), so every slow graphics
 * frame (wide-photo pan, ending) also delayed the audio frame, and runs of them
 * emptied the AICA stream (music stutter on a real Dreamcast).
 */
#include <kos.h>
#include <string.h>
#include "ultra_internal.h"
#include "f3d.h"
#include "vd_debug.h"
#include "pvr_backend.h"

typedef struct {                 /* libultra OSTask_t (as in src/game/system.h) */
    u32  type;
    u32  flags;
    u32  ucode_boot, ucode_boot_size, ucode, ucode_size, ucode_data, ucode_data_size;
    u32  dram_stack, dram_stack_size;
    u32  output_buff, output_buff_size;
    u32  data_ptr, data_size, yield_data_ptr, yield_data_size;
} VdOSTask;

/* the OSTask is OSScTask.list, at offset 0x10 of the scheduler task */
#define SCTASK_FLAGS(t) (*(u32 *)((u8 *)(t) - 0x10 + 0x08))

extern s32 D_800B0504;           /* gGfxMode: microcode 0..3 */

volatile int vd_gfx_busy;        /* 1 from osSpTaskStartGo (gfx) until the F3D run is over */
volatile int vd_reset_pending;
volatile u32 vd_gfx_tasks_started, vd_gfx_tasks_done, vd_events_retried;
static VdOSTask *vd_sp_loaded;

/* graphics timing, always on (read by port/gfx/tools/perf_run.py):
   F3D time per task (us), its histogram in 2 ms buckets (last = 14 ms and more),
   time spent waiting in pvr_wait_ready inside it */
volatile u32 vd_perf_tasks, vd_perf_f3d_us_sum, vd_perf_f3d_us_max, vd_perf_wait_us_sum;
volatile u32 vd_perf_f3d_hist[8];
/* last 256 gfx tasks: retrace count when queued, F3D us, textures uploaded */
volatile u32 vd_perf_ring[256][3];
/* copy of the interpreter's cumulative counters (F3DStats) after every gfx task */
volatile F3DStats vd_perf_f3d_stats;
extern volatile u32 vd_vi_retrace_count;
static u32 vd_perf_qvi;
static int vd_ucode = -1;

/* RCP jobs: a gfx task to run (then SP [+DP] done), or just an event to post */
enum { VD_JOB_GFX, VD_JOB_EVENT };
typedef struct { int kind; int event; VdOSTask *task; u32 gen; } VdRcpJob;
#define VD_RCP_JOBS 16
static VdRcpJob vd_jobs[VD_RCP_JOBS];
static volatile u32 vd_job_head, vd_job_tail;
static volatile u32 vd_rcp_gen;          /* bumped by a soft reset: stale completions are dropped */
static semaphore_t vd_rcp_sem;
static kthread_t *vd_rcp_thd;

/* Post an RCP event; if the scheduler's queue is full, wait until there is room
 * (never drop a completion). Gives up only when a soft reset started meanwhile. */
static void vd_post_event_reliable(int event, u32 gen)
{
    int tries = 0;

    while (vd_try_post_event(event) < 0) {
        if (gen != vd_rcp_gen)
            return;
        if (tries++ == 0) {
            vd_events_retried++;
            VD_LOG(VD_DBG_EVT, "rcp: event %d: queue full, waiting\n", event);
        }
        thd_sleep(1);
    }
}

static void vd_job_push(int kind, int event, VdOSTask *t)
{
    irq_mask_t s = irq_disable();
    VdRcpJob *j = &vd_jobs[vd_job_head % VD_RCP_JOBS];

    if (vd_job_head - vd_job_tail >= VD_RCP_JOBS) {
        irq_restore(s);
        VD_LOG(VD_DBG_EVT, "rcp: job queue full, job dropped\n");
        return;
    }
    j->kind = kind;
    j->event = event;
    j->task = t;
    j->gen = vd_rcp_gen;
    vd_job_head++;
    irq_restore(s);
    sem_signal(&vd_rcp_sem);
}

static void *vd_rcp_main(void *arg)
{
    VdRcpJob j;
    irq_mask_t s;
    int early;

    (void)arg;
    for (;;) {
        sem_wait(&vd_rcp_sem);
        s = irq_disable();
        if (vd_job_tail == vd_job_head) {
            irq_restore(s);
            continue;
        }
        j = vd_jobs[vd_job_tail % VD_RCP_JOBS];
        vd_job_tail++;
        irq_restore(s);
        early = 0;
        if (j.kind == VD_JOB_EVENT) {
            if (j.gen == vd_rcp_gen)
                vd_post_event_reliable(j.event, j.gen);
            continue;
        }
        if (!vd_reset_pending && j.gen == vd_rcp_gen) {
            if (D_800B0504 != vd_ucode) {
                vd_ucode = D_800B0504;
                f3d_set_ucode(vd_ucode);
            }
            uint64_t t0 = timer_us_gettime64();
            if ((SCTASK_FLAGS(j.task) & 7) == 3) {
                vd_post_event_reliable(OS_EVENT_SP, j.gen);   /* RSP part "done": audio may run */
                early = 1;
            }
            f3d_run_task(j.task->data_ptr);
#ifdef VD_TEST_SLOW_US
            /* test builds only: make every frame slower (coin test at low fps) */
#ifdef VD_TEST_SLOW_ALT
            if (vd_perf_tasks & 1)          /* every other task only: about 40-45 fps */
#endif
            while ((u32)(timer_us_gettime64() - t0) < VD_TEST_SLOW_US) {}
#endif
            {
                u32 us = (u32)(timer_us_gettime64() - t0);
                vd_perf_tasks++;
                vd_perf_f3d_us_sum += us;
                if (us > vd_perf_f3d_us_max) vd_perf_f3d_us_max = us;
                vd_perf_f3d_hist[us >= 14000 ? 7 : us / 2000]++;
                vd_perf_wait_us_sum += pvr_backend_stats()->wait_us_last;
                memcpy((void *)&vd_perf_f3d_stats, f3d_stats(), sizeof(F3DStats));
                {
                    static u32 up_last;
                    u32 up = pvr_backend_stats()->tex_uploads;
                    volatile u32 *r = vd_perf_ring[vd_perf_tasks & 255];
                    r[0] = vd_perf_qvi;
                    r[1] = us;
                    r[2] = up - up_last;
                    up_last = up;
                }
            }
#if VD_DEBUG
            {
                const PvrBackendStats *st = pvr_backend_stats();
                VD_LOG(VD_DBG_GFX, "rcp: gfx task %u dl=%08x f3d %u us (wait %u, tris %d up %d hit %d live %d vfail %d)\n",
                       (unsigned)vd_gfx_tasks_done, (unsigned)j.task->data_ptr,
                       (unsigned)(timer_us_gettime64() - t0), st->wait_us_last, st->tris, st->tex_uploads, st->tex_hits,
                       st->tex_live, st->vram_fail);
            }
#endif
        }
        vd_gfx_tasks_done++;
        vd_gfx_busy = 0;
        VD_LOG(VD_DBG_GFX, "rcp: gfx task %u done\n", (unsigned)vd_gfx_tasks_done - 1);
        if (j.gen == vd_rcp_gen) {
            if (!early)
                vd_post_event_reliable(OS_EVENT_SP, j.gen);
            if ((SCTASK_FLAGS(j.task) & 7) == 3)
                vd_post_event_reliable(OS_EVENT_DP, j.gen);
        }
    }
    return NULL;
}

void vd_ultra_sp_init(void)
{
    kthread_attr_t attr;
    irq_mask_t s = irq_disable();

    vd_sp_loaded = 0;
    vd_rcp_gen++;                       /* jobs / completions of an old boot are dropped */
    vd_job_tail = vd_job_head;
    irq_restore(s);
    if (vd_rcp_thd == NULL) {
        sem_init(&vd_rcp_sem, 0);
        memset(&attr, 0, sizeof(attr));
        attr.create_detached = true;
        attr.stack_size = 128 * 1024;   /* runs the F3D interpreter */
        /* Below the scheduler (libultra 15) and the audio manager (14), above every
           other libultra thread: the RCP works "in parallel" with the game threads
           (it blocks in the PVR calls, which lets them run meanwhile), but a retrace
           preempts F3D at once, so the scheduler hands out the audio task and the
           audio frame is mixed on time even while a slow graphics task runs (with
           the RCP thread above everything, audio waited for F3D: music stutter
           during long frames). Same total CPU work. */
        attr.prio = vd_kos_prio(14) + 1;
        attr.label = "vd_rcp";
        vd_rcp_thd = thd_create_ex(&attr, vd_rcp_main, NULL);
        vd_ucode = -1;
    }
}

void osSpTaskLoad(void *t)
{
    vd_sp_loaded = t;
}

void osSpTaskStartGo(void *tp)
{
    VdOSTask *t = tp;

    if (t == 0)
        t = vd_sp_loaded;
    if (t == 0)
        return;
    if (t->type == M_GFXTASK) {
        vd_gfx_busy = 1;
        vd_gfx_tasks_started++;
        vd_perf_qvi = vd_vi_retrace_count;
        VD_LOG(VD_DBG_GFX, "sp: gfx task %u queued (fb %08x)\n", (unsigned)vd_gfx_tasks_started - 1,
               (unsigned)*(u32 *)((u8 *)t - 0x10 + 0x0C));
        vd_job_push(VD_JOB_GFX, 0, t);
    } else {
        /* M_AUDTASK (and anything else): nothing to execute; done at once,
           not queued behind a gfx task the RCP thread may be running */
        VD_LOG(VD_DBG_GFX, "sp: task type %u done at once\n", (unsigned)t->type);
        if (vd_try_post_event(OS_EVENT_SP) < 0)
            vd_job_push(VD_JOB_EVENT, OS_EVENT_SP, t);
    }
}

void osSpTaskYield(void)
{
}

u32 osSpTaskYielded(void *t)
{
    (void)t;
    return 0;
}

void osDpSetNextBuffer(void *buf, u64 size)
{
    (void)buf;
    (void)size;
    vd_job_push(VD_JOB_EVENT, OS_EVENT_DP, NULL);
}

VD_ALIAS(func_80021280, osSpTaskLoad);
VD_ALIAS(func_8002148C, osSpTaskStartGo);
VD_ALIAS(func_800211E0, osDpSetNextBuffer);
