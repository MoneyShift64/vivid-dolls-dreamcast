/* Vivid Dolls port - C helper of task_sh4.S. */
#include <stdint.h>
#include <string.h>
#include "vd_debug.h"

typedef struct Task {
    struct Task *next;
    struct Task *prev;
    uint32_t *sp;
    int32_t prio;
} Task;

typedef struct {
    uint32_t blockSize;
    /* ... (src/game/system.h TaskPool) */
} TaskPoolHead;

extern TaskPoolHead *D_800C51DC;          /* the task pool */

uint32_t *vd_task_prepare(Task *t);

/* Called by Task_SwitchNext with the task about to run; returns its frame.
 * A fresh frame (built by Task_Create: resume pc = the task function, pr =
 * Task_Exit, so pc != pr; a yielded frame always has pc == pr) gets its stack
 * arguments moved for the SH4 ABI: Task_Create copies all argument words to the
 * top of the block (the frame's sp), where the MIPS callee finds argument 5+ at
 * sp+16; an SH4 callee expects them at sp+0. */
uint32_t *vd_task_prepare(Task *t)
{
    uint32_t *frame = t->sp;
    uint8_t *args, *end;

    VD_LOG(VD_DBG_TASK, "task %p (prio %d) -> pc %08x%s\n", (void *)t, (int)t->prio,
           (unsigned)frame[0x3C / 4], frame[0x3C / 4] != frame[0x38 / 4] ? " (new)" : "");

    if (frame[0x3C / 4] != frame[0x38 / 4] && D_800C51DC != NULL) {
        args = (uint8_t *)frame + 0xC0;
        end = (uint8_t *)t + D_800C51DC->blockSize;
        if (end > args + 16)
            memmove(args, args + 16, (size_t)(end - args - 16));
    }
    return frame;
}
