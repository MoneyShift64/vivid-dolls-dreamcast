/* Vivid Dolls - overlay 1: boot logo, title screen and the stage / bonus intros
 * (ROM 0x43AD0-0x45270, loaded at RAM 0x800E0000; BSS 0x800E17A0-0x800E1900).
 *
 * Main_GameLoop runs the overlay with args[0] =
 *   3  once at boot      -> Boot_Logo:    one full-screen sprite for 181 frames
 *   0  title screen      -> Title_Main:   photo background (jpeg64), logo halves slide
 *                                         in, "press start" blinks, credits shown;
 *                                         result 1 / 2 = player 1 / 2 started (one
 *                                         credit used), 4 = 301 frames idle without
 *                                         credits (attract demo), 0 = service abort
 *   1  before each round -> StageIntro_Main: 8 letters/digits ("STAGE g-p") slide in
 *                                         from the right, then a sprite zooms 4x -> 1x
 *   2  before the bonus  -> BonusIntro_Main: same with 10 letters
 * Each screen runs as a task (prio 0x14); the entry function yields until the task
 * stores its result (0x10000 | value) and returns the low 16 bits.
 *
 * All screens poll D_800C30C4 (-> gServiceRequest = 1) and gServiceRequest2
 * every frame and leave early when either is set.
 */
#ifndef VD_OVL1_ATTRACT_H
#define VD_OVL1_ATTRACT_H

#include "vd_common.h"

s32  Attract_Run(s32 *args);            /* 0x800E0000 overlay entry */
void Attract_UpdateCredits(void);       /* 0x800E00E4 */
void Title_Main(s32 *done);             /* 0x800E0288 task */
void StageIntro_Main(s32 *done);        /* 0x800E0970 task */
void BonusIntro_Main(s32 *done);        /* 0x800E0C80 task */
void Boot_Logo(s32 *done);              /* 0x800E0F40 task */

#endif
