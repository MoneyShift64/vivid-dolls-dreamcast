/* Vivid Dolls port - the game's cooperative task switcher on the SH4.
 *
 * Replaces the five hand-written MIPS routines (docs/notes/system.md):
 *   Task_Exit (0x8000AE90), Task_Yield (0x8000AEAC), Task_SwitchNext (0x8000AF94),
 *   Task_RunAll (0x8000B06C), Task_SaveContext (0x8000B150).
 *
 * Same behaviour as the original:
 *   - a context frame of 0xC0 bytes lives on the task's stack, Task->sp points at it;
 *   - Task_Yield saves the frame; if the current task (gTaskCurrent = D_800AF22C)
 *     has a next it switches to it, otherwise it returns at once;
 *   - Task_SwitchNext(t) makes t->next current, restores its frame and resumes it;
 *   - Task_RunAll saves the caller's frame in the tail sentinel and switches to the
 *     first task; reaching the tail restores that frame, so Task_RunAll returns;
 *   - Task_Exit frees the current task and switches to its next.
 *
 * SH4 frame layout (only the slots Task_Create writes keep the MIPS offsets:
 * a0-a3 -> r4-r7 at 0x00, ra -> pr at 0x38, resume pc at 0x3C):
 *   0x00 r4 r5 r6 r7   0x10 r8 .. r14 (to 0x28)   0x2C mach  0x30 macl  0x34 fpscr
 *   0x38 pr            0x3C pc                    0x40 fr12 fr13 fr14 fr15
 * r0-r3, fr0-fr11, fpul, t are caller-saved in the SH GCC ABI; gbr (KOS TLS) is
 * per KOS thread and not touched. Built for -m4-single: FPSCR.SZ = 0, so fmov.s
 * moves 32 bits.
 *
 * A fresh task (frame pc != frame pr: pc = the task function, pr = Task_Exit) is
 * prepared by vd_task_prepare() before its first start: the MIPS ABI passes
 * arguments 5+ at sp+16, the SH4 ABI at sp+0.
 */

    .text
    .align 2

#define FRAME 0xC0

/* store callee-saved state into the frame at register rb (uses r0, r1) */
.macro SAVE_CTX rb
    mov.l   r4, @(0x00, \rb)
    mov.l   r5, @(0x04, \rb)
    mov.l   r6, @(0x08, \rb)
    mov.l   r7, @(0x0C, \rb)
    mov.l   r8, @(0x10, \rb)
    mov.l   r9, @(0x14, \rb)
    mov.l   r10, @(0x18, \rb)
    mov.l   r11, @(0x1C, \rb)
    mov.l   r12, @(0x20, \rb)
    mov.l   r13, @(0x24, \rb)
    mov.l   r14, @(0x28, \rb)
    sts     mach, r0
    mov.l   r0, @(0x2C, \rb)
    sts     macl, r0
    mov.l   r0, @(0x30, \rb)
    sts     fpscr, r0
    mov.l   r0, @(0x34, \rb)
    sts     pr, r0
    mov.l   r0, @(0x38, \rb)
    mov     \rb, r1
    add     #0x40, r1
    fmov.s  fr12, @r1
    add     #4, r1
    fmov.s  fr13, @r1
    add     #4, r1
    fmov.s  fr14, @r1
    add     #4, r1
    fmov.s  fr15, @r1
.endm

/* void Task_SaveContext(u32 *ctx) */
    .global _Task_SaveContext
    .global _func_8000B150
_Task_SaveContext:
_func_8000B150:
    SAVE_CTX r4
    rts
    nop

/* void Task_Yield(void) */
    .global _Task_Yield
    .global _func_8000AEAC
_Task_Yield:
_func_8000AEAC:
    add     #-96, r15
    add     #-96, r15
    SAVE_CTX r15
    sts     pr, r0
    mov.l   r0, @(0x3C, r15)        /* resume where we were called from */
    mov.l   .Lcur_y, r1
    mov.l   @r1, r2                 /* r2 = current task */
    mov.l   @r2, r0                 /* current->next */
    tst     r0, r0
    bf      1f
    add     #96, r15                /* no next: return at once */
    rts
    add     #96, r15
1:
    mov.l   r15, @(8, r2)           /* current->sp = frame */
    bra     _Task_SwitchNext
    mov     r2, r4

    .align 2
.Lcur_y:
    .long   _D_800AF22C

/* void Task_SwitchNext(Task *t): run t->next */
    .global _Task_SwitchNext
    .global _func_8000AF94
_Task_SwitchNext:
_func_8000AF94:
    mov.l   @r4, r4                 /* t = t->next */
    mov.l   .Lcur_s, r1
    mov.l   r4, @r1                 /* gTaskCurrent = t */
    mov.l   .Lprep, r1
    jsr     @r1                     /* r0 = vd_task_prepare(t) = t->sp */
    nop
    mov     r0, r15
    mov     r15, r1
    add     #0x40, r1
    fmov.s  @r1+, fr12
    fmov.s  @r1+, fr13
    fmov.s  @r1+, fr14
    fmov.s  @r1+, fr15
    mov.l   @(0x34, r15), r0
    lds     r0, fpscr
    mov.l   @(0x30, r15), r0
    lds     r0, macl
    mov.l   @(0x2C, r15), r0
    lds     r0, mach
    mov.l   @(0x38, r15), r0
    lds     r0, pr
    mov.l   @(0x28, r15), r14
    mov.l   @(0x24, r15), r13
    mov.l   @(0x20, r15), r12
    mov.l   @(0x1C, r15), r11
    mov.l   @(0x18, r15), r10
    mov.l   @(0x14, r15), r9
    mov.l   @(0x10, r15), r8
    mov.l   @(0x0C, r15), r7
    mov.l   @(0x08, r15), r6
    mov.l   @(0x04, r15), r5
    mov.l   @(0x00, r15), r4
    mov.l   @(0x3C, r15), r0
    add     #96, r15
    jmp     @r0
    add     #96, r15

    .align 2
.Lcur_s:
    .long   _D_800AF22C
.Lprep:
    .long   _vd_task_prepare

/* void Task_RunAll(void): run every task once, return when the tail is reached */
    .global _Task_RunAll
    .global _func_8000B06C
_Task_RunAll:
_func_8000B06C:
    add     #-96, r15
    add     #-96, r15
    SAVE_CTX r15
    sts     pr, r0
    mov.l   r0, @(0x3C, r15)
    mov.l   .Ltail, r1
    mov.l   @r1, r1                 /* r1 = tail sentinel */
    mov.l   r15, @(8, r1)           /* tail->sp = frame */
    mov.l   .Lhead, r1
    bra     _Task_SwitchNext
    mov.l   @r1, r4                 /* r4 = head sentinel */

    .align 2
.Ltail:
    .long   _D_800C53FC
.Lhead:
    .long   _D_800B05A4

/* Task_Exit: where a task function returns to */
    .global _Task_Exit
    .global _func_8000AE90
_Task_Exit:
_func_8000AE90:
    mov.l   .Lcur_e, r1
    mov.l   .Lfree, r0
    jsr     @r0                     /* r0 = Task_Free(gTaskCurrent) */
    mov.l   @r1, r4
    bra     _Task_SwitchNext
    mov     r0, r4

    .align 2
.Lcur_e:
    .long   _D_800AF22C
.Lfree:
    .long   _Task_Free
