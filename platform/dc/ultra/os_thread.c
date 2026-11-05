/* Vivid Dolls port - libultra threads, message queues and events on KOS.
 * See ultra_internal.h for the model and port/notes/sys.md for the details.
 */
#include <kos.h>
#include <stdlib.h>
#include <string.h>
#include "ultra_internal.h"
#include "vd_debug.h"

VD_STATIC_ASSERT(sizeof(VdMesgQueue) <= 0x18, mq_fits);
VD_STATIC_ASSERT(sizeof(VdThread) <= 0x1B0, thread_fits);

void *vd_stack_floor;
static VdThread *vd_all_threads;           /* every thread created with osCreateThread */
static VdThread  vd_foreign;               /* record shared by non-libultra callers (boot, KOS threads) */

static struct { VdMesgQueue *mq; OSMesg msg; } vd_events[OS_NUM_EVENTS];

void vd_ultra_threads_init(void)
{
    memset(vd_events, 0, sizeof(vd_events));
    memset(&vd_foreign, 0, sizeof(vd_foreign));
    vd_foreign.priority = 0;
    vd_foreign.magic = VD_THREAD_MAGIC;
    vd_all_threads = NULL;
}

/* libultra 0..255 (higher runs first) -> KOS (lower runs first). Priority 0 (the
 * idle thread's spin loop) goes to PRIO_MAX, which KOS never ages, so it only ever
 * runs when nothing else can. KOS's own threads (PRIO_DEFAULT = 10) stay above. */
int vd_kos_prio(s32 pri)
{
    if (pri <= 0)
        return PRIO_MAX;
    if (pri > 255)
        pri = 255;
    return 20 + (255 - pri);
}

VdThread *vd_thread_self(void)
{
    kthread_t *me = thd_get_current();
    VdThread *t;

    for (t = vd_all_threads; t != NULL; t = t->all_next)
        if (t->kt == me)
            return t;
    return &vd_foreign;
}

/* ---- wait lists: priority order, FIFO among equals (like __osEnqueueThread) ---- */
static void vd_enqueue(VdThread **q, VdThread *t)
{
    while (*q != NULL && (*q)->priority >= t->priority)
        q = &(*q)->next;
    t->next = *q;
    *q = t;
}

static void vd_dequeue(VdThread **q, VdThread *t)
{
    while (*q != NULL) {
        if (*q == t) {
            *q = t->next;
            t->next = NULL;
            return;
        }
        q = &(*q)->next;
    }
}

/* Interrupts must be disabled. Puts the caller on wait list q and sleeps until a
 * waker takes it off (vd_wake). */
static void vd_block_on(VdThread **q)
{
    VdThread *me = vd_thread_self();

    me->woken = 0;
    me->queue = q;
    vd_enqueue(q, me);
    VD_LOG(VD_DBG_MESG, "thread %d blocks on %p\n", (int)me->id, (void *)q);
    while (!me->woken)
        genwait_wait(me, "osMesg", 0);
}

/* Interrupts must be disabled. Wakes the first thread of q (if any) and returns it. */
static VdThread *vd_wake(VdThread **q)
{
    VdThread *t = *q;

    if (t == NULL)
        return NULL;
    *q = t->next;
    t->next = NULL;
    t->queue = NULL;
    t->woken = 1;
    genwait_wake_all(t);
    VD_LOG(VD_DBG_MESG, "thread %d woken from %p\n", (int)t->id, (void *)q);
    return t;
}

/* After waking t: libultra switches at once when t has a higher priority. */
static void vd_handoff(VdThread *t)
{
    kthread_t *kt, *cur;

    if (t == NULL || t->kt == NULL)
        return;
    kt = (kthread_t *)t->kt;
    cur = thd_get_current();
    if (kt == cur || kt->prio >= cur->prio)
        return;
    if (irq_inside_int()) {
        if (kt->state == STATE_READY)
            thd_schedule_next(kt);
    } else {
        thd_pass();
    }
}

/* ------------------------------------------------------------ message queues */

void osCreateMesgQueue(void *mqp, OSMesg *msg, s32 count)
{
    VdMesgQueue *mq = mqp;

    mq->mtqueue = NULL;
    mq->fullqueue = NULL;
    mq->validCount = 0;
    mq->first = 0;
    mq->msgCount = count;
    mq->msg = msg;
}

s32 osSendMesg(void *mqp, OSMesg msg, s32 flag)
{
    VdMesgQueue *mq = mqp;
    VdThread *w;
    irq_mask_t s = irq_disable();
    s32 last;

    while (mq->validCount >= mq->msgCount) {
        if (flag == OS_MESG_NOBLOCK || irq_inside_int()) {
            irq_restore(s);
            return -1;
        }
        vd_block_on(&mq->fullqueue);
    }
    last = (mq->first + mq->validCount) % mq->msgCount;
    mq->msg[last] = msg;
    mq->validCount++;
    w = vd_wake(&mq->mtqueue);
    irq_restore(s);
    vd_handoff(w);
    return 0;
}

s32 osJamMesg(void *mqp, OSMesg msg, s32 flag)
{
    VdMesgQueue *mq = mqp;
    VdThread *w;
    irq_mask_t s = irq_disable();

    while (mq->validCount >= mq->msgCount) {
        if (flag == OS_MESG_NOBLOCK || irq_inside_int()) {
            irq_restore(s);
            return -1;
        }
        vd_block_on(&mq->fullqueue);
    }
    mq->first = (mq->first + mq->msgCount - 1) % mq->msgCount;
    mq->msg[mq->first] = msg;
    mq->validCount++;
    w = vd_wake(&mq->mtqueue);
    irq_restore(s);
    vd_handoff(w);
    return 0;
}

s32 osRecvMesg(void *mqp, OSMesg *msg, s32 flag)
{
    VdMesgQueue *mq = mqp;
    VdThread *w;
    irq_mask_t s = irq_disable();

    while (mq->validCount == 0) {
        if (flag == OS_MESG_NOBLOCK || irq_inside_int()) {
            irq_restore(s);
            return -1;
        }
        vd_block_on(&mq->mtqueue);
    }
    if (msg != NULL)
        *msg = mq->msg[mq->first];
    mq->first = (mq->first + 1) % mq->msgCount;
    mq->validCount--;
    w = vd_wake(&mq->fullqueue);
    irq_restore(s);
    vd_handoff(w);
    return 0;
}

/* ------------------------------------------------------------------- events */

void osSetEventMesg(u32 event, void *mq, OSMesg msg)
{
    irq_mask_t s = irq_disable();

    if (event < OS_NUM_EVENTS) {
        vd_events[event].mq = mq;
        vd_events[event].msg = msg;
    }
    irq_restore(s);
}

void vd_post_event(int event)
{
    if (vd_try_post_event(event) < 0)
        VD_LOG(VD_DBG_EVT, "event %d lost (queue full)\n", event);
}

/* 0 = posted (or no queue registered), -1 = the queue is full */
int vd_try_post_event(int event)
{
    VdMesgQueue *mq;
    OSMesg msg;
    irq_mask_t s = irq_disable();

    mq = vd_events[event].mq;
    msg = vd_events[event].msg;
    irq_restore(s);
    if (mq == NULL)
        return 0;
    return osSendMesg(mq, msg, OS_MESG_NOBLOCK) < 0 ? -1 : 0;
}

/* ------------------------------------------------------------------ threads */

static void *vd_thread_tramp(void *p)
{
    VdThread *t = p;
    irq_mask_t s = irq_disable();

    while (!t->go)
        genwait_wait((void *)&t->go, "osStartThread", 0);
    irq_restore(s);
    t->entry(t->arg);
    /* A libultra thread that returns just stops. */
    for (;;)
        genwait_wait((void *)&t->started, "ultra-exit", 0);
    return NULL;
}

void osCreateThread(void *tp, s32 id, void (*entry)(void *), void *arg, void *sp, s32 pri)
{
    VdThread *t = tp;
    VdThread *i;
    irq_mask_t s;

    (void)sp;   /* KOS allocates its own (bigger) stacks */
    memset(t, 0, sizeof(*t));
    t->magic = VD_THREAD_MAGIC;
    t->id = id;
    t->entry = entry;
    t->arg = arg;
    t->priority = pri;
    s = irq_disable();
    for (i = vd_all_threads; i != NULL; i = i->all_next)
        if (i == t)
            break;
    if (i == NULL) {
        t->all_next = vd_all_threads;
        vd_all_threads = t;
    }
    irq_restore(s);
}

void osStartThread(void *tp)
{
    VdThread *t = tp;
    kthread_attr_t attr;
    kthread_t *kt;
    irq_mask_t s;
    size_t size;
    u8 *mem, *base;

    if (t->magic != VD_THREAD_MAGIC || t->started)
        return;
    /* the scheduler thread runs the F3D interpreter, the gfx thread the game */
    size = (t->id == 4 || t->id == 6) ? 128 * 1024 : 32 * 1024;
    mem = aligned_alloc(32, size);
    if (mem == NULL) {
        dbglog(DBG_ERROR, "osStartThread: no memory for thread %d\n", (int)t->id);
        return;
    }
    /* The game's cooperative tasks (Task_*) run on stacks inside the task pool:
       declare the stack from vd_stack_floor up so KOS's underrun check accepts them. */
    base = mem;
    if (vd_stack_floor != NULL && (u8 *)vd_stack_floor < base)
        base = vd_stack_floor;
    memset(&attr, 0, sizeof(attr));
    attr.create_detached = true;
    attr.stack_ptr = base;
    attr.stack_size = (size_t)(mem + size - base);
    attr.prio = vd_kos_prio(t->priority);
    attr.label = "ultra";
    t->stack_mem = mem;
    kt = thd_create_ex(&attr, vd_thread_tramp, t);
    if (kt == NULL) {
        dbglog(DBG_ERROR, "osStartThread: cannot create thread %d\n", (int)t->id);
        free(mem);
        t->stack_mem = NULL;
        return;
    }
    VD_LOG(VD_DBG_BOOT, "osStartThread: id %d pri %d (KOS %d)\n", (int)t->id, (int)t->priority,
           vd_kos_prio(t->priority));
    s = irq_disable();
    t->kt = kt;
    t->started = 1;
    t->go = 1;
    genwait_wake_all((void *)&t->go);
    irq_restore(s);
    vd_handoff(t);
}

void osSetThreadPri(void *tp, s32 pri)
{
    VdThread *t = tp ? (VdThread *)tp : vd_thread_self();
    irq_mask_t s = irq_disable();

    t->priority = pri;
    if (t->queue != NULL) {             /* keep wait lists in priority order */
        vd_dequeue(t->queue, t);
        vd_enqueue(t->queue, t);
    }
    if (t->kt != NULL)
        thd_set_prio((kthread_t *)t->kt, vd_kos_prio(pri));
    irq_restore(s);
    if (!irq_inside_int())
        thd_pass();                     /* lowering our own priority may let others run */
}

s32 osGetThreadPri(void *tp)
{
    VdThread *t = tp ? (VdThread *)tp : vd_thread_self();
    return t->priority;
}

/* Soft reset: destroy every libultra thread (called from the KOS main thread,
 * which has a higher priority than all of them). */
void vd_kill_all_threads(void)
{
    VdThread *t, *n, *list;
    kthread_t *me = thd_get_current();
    irq_mask_t s = irq_disable();

    list = vd_all_threads;
    vd_all_threads = NULL;
    memset(vd_events, 0, sizeof(vd_events));
    irq_restore(s);
    /* (no interrupts disabled here: thd_destroy frees memory) */
    for (t = list; t != NULL; t = n) {
        n = t->all_next;
        if (t->kt != NULL && (kthread_t *)t->kt != me) {
            thd_destroy((kthread_t *)t->kt);
            free(t->stack_mem);
        }
        t->kt = NULL;
        t->stack_mem = NULL;
        t->magic = 0;
    }
}

/* --------------------------------------------------------------- interrupts */

/* libultra returns the previous mask; OS_IM_NONE (1) disables everything. */
u32 osSetIntMask(u32 mask)
{
    irq_mask_t old = irq_disable();
    u32 prev = ((old & 0xF0) == 0xF0) ? OS_IM_NONE : OS_IM_ALL;

    if ((mask & 0x3FFF00u) != 0)
        irq_enable();
    return prev;
}
