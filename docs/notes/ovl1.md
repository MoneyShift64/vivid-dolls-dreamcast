# Area "ovl1": overlay 1 - boot logo, title screen, stage / bonus intros

ROM 0x43AD0-0x45270, loaded at 0x800E0000 (code 0x800E0000-0x800E0FC0, data to
0x800E17A0, BSS 0x800E17A0-0x800E1900).

C: `src/game/ovl1_attract.c` / `ovl1_attract.h` (all 6 functions).
Test: `python3 tools/difftest/test_ovl1.py` (all PASS: 300 trials for the two small
functions, 120 runs per task, 20 for the boot logo; about 65 s).
Every task is run to completion with a scripted Task_Yield stub (credits inserted,
start buttons, random noise, D_800C30C4 / gServiceRequest2 raised at a random frame);
the call logs of all stubs (Obj_*, Scene_*, PhotoView_*, Image_CopyRect, Task_Create,
yields) and main BSS, overlay data+BSS, test memory and game heap are compared.
All title results (0, 1, 2, 4) are reached.

Photo rule: the title screen decodes a jpeg64 photo (asset 0x3EB4) as its background.
The test stubs Jpeg64_Decode (only writes a width/height); no photo is read or decoded.

## Functions done

| Address | Name | What |
|---|---|---|
| 0x800E0000 | Attract_Run | overlay entry: Camera_Init, Light_Init, gRetracesPerFrame = 1, Task_Create(screen task for args[0] 0..3, prio 0x14, &done, 0), yield until done != 0, return done & 0xFFFF (other args[0]: no task, waits forever - kept) |
| 0x800E00E4 | Attract_UpdateCredits | credit counter (copy of ovl2 Hud_UpdateCredits with ovl1 objects): 2 digits (max 99), or "coins / coins-per-credit" when 0 credits and D_800B056C >= 2 |
| 0x800E0288 | Title_Main | title task: 2 x 0x96000 heap buffers cleared, Jpeg64_Decode(0x3EB4 + 0x611F0), Image_CopyRect into the 2nd, PhotoView + Obj_NewDL background, Scene_Build; logo halves slide in (3 phases of 30 frames, /8 then /4 then /4 with logo shown; skipped when a credit is in); then wait: start P1/P2 with a credit -> 1/2 (credit used), 301 idle frames without credits -> 4 (attract demo), service flags -> 0. Teardown, *done = 0x10000 \| result |
| 0x800E0970 | StageIntro_Main | "STAGE g-p": 8 sprites slide from x 420..1180 to 120..200 (1/8 per frame, 61 frames); sprites 5/7 = digit[D_800B0608] / digit[D_800B04CC]; then sprite 0x800E1840 zooms scale 4.0 * 0.9 (f64) per frame, min 1.0, 61 frames |
| 0x800E0C80 | BonusIntro_Main | same with 10 fixed sprites (0x800E18A0..), zoom sprite 0x800E18C8 |
| 0x800E0F40 | Boot_Logo | one full-screen sprite (asset 0x8B34) for 181 frames, no early exit |

The two intros share one static helper in C (`intro_animate`); the original has two copies.

## Data symbols

```
Attract_Run = 0x800E0000; // type:func (ovl1)
Attract_UpdateCredits = 0x800E00E4; // type:func (ovl1)
Title_Main = 0x800E0288; // type:func (ovl1)
StageIntro_Main = 0x800E0970; // type:func (ovl1)
BonusIntro_Main = 0x800E0C80; // type:func (ovl1)
Boot_Logo = 0x800E0F40; // type:func (ovl1)
gTitleObjDescs = 0x800E0FC0; // type:SceneObjDesc size:0x18C (ovl1, 10 entries + terminator)
gTitleAssetDescs = 0x800E114C; // type:SceneAssetDesc size:0xB4 (ovl1)
gStageIntroTarget = 0x800E1200; // type:f32 size:0x40 (ovl1, [8][2])
gStageIntroStart = 0x800E1240; // type:f32 size:0x40 (ovl1, [8][2])
gStageIntroObjDescs = 0x800E1280; // type:SceneObjDesc size:0x168 (ovl1)
gStageIntroAssetDescs = 0x800E13E8; // type:SceneAssetDesc size:0xD8 (ovl1)
gBonusIntroTarget = 0x800E14C0; // type:f32 size:0x50 (ovl1, [10][2])
gBonusIntroStart = 0x800E1510; // type:f32 size:0x50 (ovl1, [10][2])
gBonusIntroObjDescs = 0x800E1560; // type:SceneObjDesc size:0x1B0 (ovl1)
gBonusIntroAssetDescs = 0x800E1710; // type:SceneAssetDesc size:0xC (ovl1, empty)
gBootLogoObjDescs = 0x800E1720; // type:SceneObjDesc size:0x48 (ovl1)
gBootLogoAssetDescs = 0x800E1768; // type:SceneAssetDesc size:0xC (ovl1, empty)
gStageIntroZoomFactor = 0x800E1780; // type:f64 (ovl1, 0.9)
gBonusIntroZoomFactor = 0x800E1790; // type:f64 (ovl1, 0.9)
gTitleObjs = 0x800E17A0; // type:Obj* size:0x30 (ovl1 BSS; [3] press start, [4] 2nd blink, [9..11] credit digits)
gTitleCreditDigit0 = 0x800E17C4; // type:Obj* (ovl1 BSS)
gTitleCreditDigit1 = 0x800E17C8; // type:Obj* (ovl1 BSS)
gTitleCreditDigit2 = 0x800E17CC; // type:Obj* (ovl1 BSS)
gTitleDigitSprites = 0x800E17F0; // type:Sprite* size:0x28 (ovl1 BSS)
gTitleSlashSprite = 0x800E1818; // type:Sprite* (ovl1 BSS)
gStageIntroObjs = 0x800E1820; // type:Obj* size:0x20 (ovl1 BSS)
gStageIntroZoomObj = 0x800E1840; // type:Obj* (ovl1 BSS)
gStageIntroDigits = 0x800E186C; // type:Sprite* (ovl1 BSS, indexed by girl / photo)
gBonusIntroObjs = 0x800E18A0; // type:Obj* size:0x28 (ovl1 BSS)
gBonusIntroZoomObj = 0x800E18C8; // type:Obj* (ovl1 BSS)
gBootLogoObj = 0x800E18D0; // type:Obj* (ovl1 BSS)
```
(If these are added, prefix them as the project does for overlay symbols, e.g.
`ovl1_`; the C still uses the `ovl1_D_800EXXXX` names.)

## Globals learnt

- 0x800B056C: coins per credit (u32, compared with sltiu); 0x800B4A30 selects
  0x800AF264 as the divisor shown instead; 0x800C52F4 (gCoins) = coins towards the next
  credit (index into the digit sprites).
- 0x800C30C4 (u16): service/test switch - every ovl1 screen sets gServiceRequest = 1 and
  leaves when it is non-zero; gServiceRequest2 non-zero also leaves.
- 0x800C0D48 / 0x800C0D4A: player 1 / 2 buttons, 0x100 = start.
- Title objects (from the SceneObjDesc table at 0x800E0FC0): [0] kind 3 hidden (shown
  in slide phase 3), [1]/[2] the two logo halves starting at x 576 / -256 and ending at
  (216,92) / (94,50) after the slide (then set to (0xD8,0x5C) / (0x5E,0x32)), [3]
  blinking every 16 frames, [4] blinking only with credits, [5] at (160,218), [8] at
  (252,230), [9..11] the credit digits at x 290/297/304.

## Quirks kept / notes

- Title_Main: the result register (s3) is uninitialised in the original if a service
  flag ends the final wait loop on its first frame (or after the slide was skipped
  and the flag is up immediately). The C uses 0; the test zeroes s0-s7 before every
  call so both match. Main_GameLoop ignores the result when a service flag is set.
- Attract_Run with args[0] outside 0..3 never returns (not tested).
- Negative gCredits index the digit table out of bounds (same address in C).

## Remaining

None.
