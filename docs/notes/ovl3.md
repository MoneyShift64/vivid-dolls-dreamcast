# Area "ovl3": overlay 3 = girl select screen (ROM 0x602C0-0x611F0, RAM 0x800E0000)

C: `src/game/ovl3_select.c` / `ovl3_select.h` (8 functions, all of overlay 3).
Test: `python3 tools/difftest/test_ovl3.py` (all 8 PASS: 300 trials each, 120 full runs
of the select task; about 20 s).

## What it is

Main_GameLoop calls ovl3(0) at the start of every level (after `Bgm_Request(14)`) and
stores `result & 0xFF` in D_800B0608: it is the **girl (stage) select screen**.

- Scene (Scene_Build tables at 0x800E0B40 / 0x800E0DA4): background (0x800E0F30),
  title banner at (160,20) (0x800E0F34), 8 girl icons (0x800E0F40[8]) on a rotating
  ellipse, an arrow below the front slot (0x800E0F60, 2 frames from 0x800E0F70, 4 ticks
  each), a frame at the front slot (0x800E0F64, 4 frames from 0x800E0F78), a big
  portrait at (100,120) and a name/profile sprite at (210,112) (0x800E0F38/3C, hidden;
  per-girl sprites 0x800E0F88[8] / 0x800E0FA8[8]), and a 2-digit countdown at (260/280,220),
  scale 0.5 (0x800E0F68/6C, digit sprites 0x800E0FD0[10]). All are ordinary sprites
  (Scene_Build kind 4/6) - no jpeg64 photo decoding happens in this overlay.
- Ring: icon i at angle `a/256*pi + i*pi/4` (a in 1/512 turns, float-accumulated),
  position `(int)(cos*120 + 160), (int)(sin*50 + 120)`. Slot index `sel` starts at 6;
  the girl at the front (angle pi/2, bottom of the ellipse) is `(sel + 2) & 7`
  (girl 0 at start). Ring angle starts at 0x80 and eases toward the target by 1/4 of the
  shortest difference per frame (at least 1).
- Girls already cleared (D_800C178E[girl] != 0) get Obj_UseSetupCC0 + alpha 0x80 (dark)
  and cannot be chosen.
- Input: P1 (D_800C0D48) / P2 (D_800C0D4A) / both ORed, by gPlayerActive[0..1].
  0x08 = rotate one way (target += 0x40, sel - 1), else 0x04 = the other way
  (target -= 0x40, sel + 1); both play sound 2. 0x10 = choose front girl (sound 1),
  ignored on a cleared girl.
- Timer: 659 frames (shows 10 s first; tens digit hidden when 0). On time-out the front
  girl is taken, or if she is cleared the first uncleared girl; if all are cleared the
  front girl anyway.
- After a choice: digits/arrow/frame hidden, chosen icon zooms toward (100,120) and
  scale 2.0 (1/8 per frame), the others fade out (alpha 255, -4 per frame); 60 frames
  later portrait + profile fade in (alpha +16/frame), then shown for 120 frames or until
  0x10. Scene_Destroy, 3 yields, `*result = 0x10000 | girl`.
- D_800C30C4 set during the select loop: gServiceRequest = 1 and leave at once.
  Quirk kept: the result is then `0x10000 | sel` (slot index, not girl = sel + 2).
- The entry (0x800E0000) runs the screen as task prio 0x14 (argument = pointer to its
  stack result word) and yields until the result is non-zero; gRetracesPerFrame = 1
  before and after. Quirk kept: with args[0] != 0 no task is created and it waits forever.

## Functions done

| Address | Name | What |
|---|---|---|
| 0x800E0000 | GirlSelect_Run | overlay entry: Camera_Init, Light_Init(gSceneLight), start task, wait, return result & 0xFFFF |
| 0x800E0094 | GirlSelect_PlaceIcons | place the 8 icons on the ellipse for ring angle a (unreferenced in the original; the task inlines the same code - the C task calls it) |
| 0x800E01A0 | GirlSelect_ShowTimer | countdown digits for frames/60 (unreferenced in the original; inlined in the task - the C task calls it) |
| 0x800E0278 | GirlSelect_Task | the select screen task (see above) |
| 0x800E09C4 | GirlSelect_UpdateZoom | Obj update: pos -> (100,120), scale x/y -> 2.0, 1/8 per frame, Obj_Invalidate |
| 0x800E0A5C | GirlSelect_UpdateFadeOut | Obj update: alpha = unkA8; unkA8 -= 4 (clamp 0); Obj_ClearUpdate when it was 0 |
| 0x800E0AB8 | GirlSelect_UpdateArrow | Obj update: frame ((unkA8>>2)&1) of 0x800E0F70, unkA8++ |
| 0x800E0AFC | GirlSelect_UpdateFrame | Obj update: frame ((unkA8>>2)&3) of 0x800E0F78, unkA8++ |

## Data symbols

```
GirlSelect_Run = 0x800E0000; // type:func (ovl3)
GirlSelect_PlaceIcons = 0x800E0094; // type:func (ovl3)
GirlSelect_ShowTimer = 0x800E01A0; // type:func (ovl3)
GirlSelect_Task = 0x800E0278; // type:func (ovl3)
GirlSelect_UpdateZoom = 0x800E09C4; // type:func (ovl3)
GirlSelect_UpdateFadeOut = 0x800E0A5C; // type:func (ovl3)
GirlSelect_UpdateArrow = 0x800E0AB8; // type:func (ovl3)
GirlSelect_UpdateFrame = 0x800E0AFC; // type:func (ovl3)
gGirlSelectObjDescs = 0x800E0B40; // type:SceneObjDesc size:0x264 (16 entries + terminator) (ovl3)
gGirlSelectAssetDescs = 0x800E0DA4; // type:SceneAssetDesc size:0x18C (32 entries + terminator) (ovl3)
gGirlSelectBg = 0x800E0F30; // type:Obj* (ovl3 BSS)
gGirlSelectTitle = 0x800E0F34; // type:Obj* (ovl3 BSS)
gGirlSelectPortrait = 0x800E0F38; // type:Obj*[2] portrait, name/profile (ovl3 BSS)
gGirlSelectIcons = 0x800E0F40; // type:Obj*[8] (ovl3 BSS)
gGirlSelectArrow = 0x800E0F60; // type:Obj* (ovl3 BSS)
gGirlSelectFrame = 0x800E0F64; // type:Obj* (ovl3 BSS)
gGirlSelectTimerTens = 0x800E0F68; // type:Obj* (ovl3 BSS)
gGirlSelectTimerUnits = 0x800E0F6C; // type:Obj* (ovl3 BSS)
gGirlSelectArrowSpr = 0x800E0F70; // type:Sprite*[2] (ovl3 BSS)
gGirlSelectFrameSpr = 0x800E0F78; // type:Sprite*[4] (ovl3 BSS)
gGirlSelectPortraitSpr = 0x800E0F88; // type:Sprite*[8] (ovl3 BSS)
gGirlSelectProfileSpr = 0x800E0FA8; // type:Sprite*[8] (ovl3 BSS)
gGirlSelectDigitSpr = 0x800E0FD0; // type:Sprite*[10] (ovl3 BSS)
gGirlCleared = 0x800C178E; // type:u8 size:0x8 (main BSS)
gButtonsP1 = 0x800C0D48; // type:u16 (pressed buttons, P1)
gButtonsP2 = 0x800C0D4A; // type:u16 (pressed buttons, P2)
```
(The C uses the `ovl3_D_800E0XXX` / `D_800C...` names. 0x800E0FC8-0x800E0FCF is unused BSS.)

## Structs / globals learnt

- Obj `unkA8[0]` is used by these update callbacks as a frame counter / alpha.
- Button bits on this screen: 0x10 = OK/choose, 0x08 / 0x04 = rotate (left/right).
- D_800B0608 (set by the main loop from this result) = selected girl 0..7.
- Overlay data ends at 0x800E0F30; the 0xD0-byte BSS (0x800E0F30-0x800E1000) holds
  only the Obj/Sprite slots filled by Scene_Build.

## Remaining

None (all 8 overlay-3 functions done).
