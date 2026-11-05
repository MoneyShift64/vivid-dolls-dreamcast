/* Vivid Dolls - overlay 3: girl select screen (ROM 0x602C0-0x611F0, RAM 0x800E0000).
 *
 * Main_GameLoop runs overlay 3 at the start of every level (after Bgm_Request(14))
 * and stores (result & 0xFF) in D_800B0608 = the girl (stage) to play, 0..7.
 *
 * Screen (built by Scene_Build from the tables at 0x800E0B40 / 0x800E0DA4):
 * background, title banner, 8 girl icons on a rotating ellipse (centre 160,120,
 * radii 120 x 50), a 2-frame arrow and a 4-frame frame/highlight animation at the
 * front position, a 2-digit seconds countdown (659 frames = 10 s shown first) and,
 * after a choice, a big portrait + name/profile sprite that fade in.
 * Girls already cleared (D_800C178E[girl] != 0) are drawn dark (alpha 0x80) and
 * cannot be chosen. Left/right (button bits 0x08/0x04) rotate the ring by 1/8
 * turn, button 0x10 chooses the girl at the front (sound 1); rotating plays sound 2.
 * When the timer runs out the front girl is taken, or the first uncleared one.
 */
#ifndef VD_OVL3_SELECT_H
#define VD_OVL3_SELECT_H

#include "vd_common.h"
#include "object.h"

s32  GirlSelect_Run(s32 *args);            /* 0x800E0000 overlay entry */
void GirlSelect_PlaceIcons(s32 angle);     /* 0x800E0094 */
void GirlSelect_ShowTimer(s32 frames);     /* 0x800E01A0 */
void GirlSelect_Task(s32 *result);         /* 0x800E0278 */
void GirlSelect_UpdateZoom(Obj *obj);      /* 0x800E09C4 */
void GirlSelect_UpdateFadeOut(Obj *obj);   /* 0x800E0A5C */
void GirlSelect_UpdateArrow(Obj *obj);     /* 0x800E0AB8 */
void GirlSelect_UpdateFrame(Obj *obj);     /* 0x800E0AFC */

#endif
