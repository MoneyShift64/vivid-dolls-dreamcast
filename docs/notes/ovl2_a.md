# ovl2_a: overlay 2 (the game), 0x800E0000-0x800E4EDC

Overlay 2 (ROM 0x45270-0x602C0, loaded at 0x800E0000, BSS 0x800FB050-0x800FD550) is the
game itself. Vivid Dolls is a Gals Panic / Qix-style game: players move round the edge
of the claimed area of a photo, draw lines into the unclaimed part, and when a line
reaches an edge again, the side without the monster gets claimed and the photo shows
there. 70% (80% with DIP bit 5 of D_800C52A0) clears the photo.

Source: `src/game/ovl2_a.c` / `ovl2_a.h`. Test: `python3 tools/difftest/test_ovl2_a.py`
(about 15 min; `ONLY=Name1,Name2`, `NTRIALS=n`, `GAME_TRIALS=n` to run fewer).
All 43 functions in the range are done and pass (300 trials each).

## Structure / entry point

- `Game_Run` (0x800E0000) is the overlay's entry (called through Overlay_Load /
  Main_GameLoop with a pointer to `{s32 mode; s32 param}`): Camera_Init,
  Light_Init(&D_800C1730), D_800C51CC = 1, then spawns a task — mode 0:
  `Game_Main` (0x800E0BB8), mode 1: `ovl2_func_800F45DC` — with
  `func_8000A76C(fn, 0x14, 8, &done, param)`, and yields (`func_8000AEAC`) until the task
  writes `done`. Returns `done & 0xFFFF`. Any other mode waits forever.
- `func_8000A76C(fn, prio, argBytes, args...)` = task spawn (copies argBytes of the
  variadic args to the new task's stack; returns a task handle). `func_8000AEAC` =
  task yield (one frame). Sound: `func_8001BA90(id)` play SE (returns handle),
  `func_8001BB8C(h)` stop SE, `func_8001BBC0(id)` play music, `func_8000D6B8()` stop all.
  All of these skip when D_800C51D8 (attract/demo counter) is non-zero.
- `Game_Main(s32 *result, s32 param)` (7.5 KB) is one photo:
  1. allocates the block map (0x9600), cell map (0x12C00), photo (0x96000) and display
     image (0x96000) from gGameHeap, decodes photo `ovl2_D_800F9880[girl*4 + photo]`
     (+0x611F0) with Jpeg64_Decode, copies it and mosaics the copy (level 3, 0.5),
     makes a PhotoView + DL object, Board_Init, Scene_Build(ovl2_D_800F6450, ovl2_D_800F7E54),
     ovl2_func_800F1D6C, two Players (0,(64,64)) and (1,(128,64) or (128,104) in attract),
     Board_ClaimRect(64, 64, 80, 40) (the start area);
  2. spawns tasks: two player tasks `ovl2_func_800F10F0` (handles ovl2_D_800FB640/644),
     two `ovl2_func_800F4FC0` (FB64C/650), `ovl2_func_800F5780` (FB654),
     `ovl2_func_800F5D50` (FB658), then by Rand_Range(&D_800B04DC, 3) one of three
     enemy sets: `ovl2_D_800F6390[D_800B4AA0]` enemy tasks (FB600[]) of
     0x800EAD80/0x800EA610/0x800E9DC0 and one boss task (FB648) 0x800F00F4/0x800EEEEC/0x800EC0CC;
  3. frame loop: credits, join, lives, timer, percentage, stock icons, claim popup,
     yield, camera (follows P1, P2 or their middle, smoothing 1/4 per frame, scroll
     clamped to the photo) -> PhotoView_SetScroll;
  4. time up (FD424 == 0): 3 s pause, Player.timeUp = 1, timer reset, back to the loop
     if anybody still has lives; game over: 10 s continue countdown (coins restart it,
     P1 button 0x10 speeds it up), joining restarts the round (difficulty D_800B4AA0 -= 2),
     otherwise the ending icons;
  5. stage clear: reveal the whole photo, sparkles (ovl2_func_800F2384) for 300 frames or a
     pan across a 640-wide / 480-tall photo, score += percentage, >= 99%: "perfect"
     bonus (+1 stock); next photo (D_800B04CC++); after photo 3 the girl is marked done
     (D_800C178E[girl] = 1); all 8 done: final texts, stage score, ending;
  6. quit: mosaic fade-out (levels 1..6), Obj_Kill, Scene_Destroy, PhotoView_Delete,
     frees everything, `*result = res | 0x10000` (res 1 next photo, 2 girl done, 0xFF end).
  Abort conditions after every yield: D_800C30C4 (sets D_800C1674 = 1) or D_800AF260.

## Quirks kept / noted
- `Game_Main`: `res` is an uninitialised stack word in the original if the game is
  aborted before a result is chosen; the stage clear sparkle positions come from
  Rand_Range on an uninitialised local seed. The C uses 0 for both (the test zeroes the
  stack so the original reads 0 too).
- `Player_CloseArea`: if fewer than two edge directions are found next to the line's
  start, the original passes uninitialised values to Player_Trace (C: 0).
- `Player_Trace`: recorded steps carry the dx/dy of the previous direction with the new
  dir; loops forever with 0 or >1 non-matching candidates.
- `Player_IsInside` reads `path[i-2]`/`path[i-4]` even for i = 2 (index -2 lands in the
  Player header).
- `Board_Init` clears w*h/8 bytes of block map (only w*h/512 bits are used).
- `Hud_UpdateGainPopup` y drifts by `(f32)((f64)y - 0.5)`.
- Fill stack has 256 entries and no overflow check.
- `ovl2_D_800FD2C4`/`ovl2_D_800FD2B4` (monster x/y for the fill side test) are read
  with `lh`; if they are really s32 variables only the high half is used.

## Board (globals in ovl2 BSS)
2-bit cells, 4 per byte, cell i of a byte in bits 2i..2i+1:
0 unclaimed, 1 claimed (photo shown), 2 line being drawn, 3 edge; Board_GetCell gives 4
outside the board (counts as ">= 2" for the tracer). Block map: 1 bit per 8x8 block,
byte `((x>>3) + (y>>3)*(w>>3)) >> 3`, bit `(x>>3)&7` (only correct when w/8 is a
multiple of 8 - as for 320/640). Pixels are RGBA5551; edges are 0xFFFF, lines 0xFD29
(P1) / 0xA53F (P2). Players move 2 pixels per frame (two Player_Step calls).

## Structs
```c
typedef struct PathPoint { s8 dx, dy; s16 dir; s16 x, y; u16 saved; } PathPoint;  /* 10 bytes */
typedef struct TraceStep { s8 dx, dy; s16 dir; s16 x, y; } TraceStep;             /* 8 bytes */
typedef struct Player {          /* 0x3429C, Player_New (0x800E3EAC), heap */
    s32 id;                      /* 0x00 */
    s32 x, y;                    /* 0x04 */
    s32 unk0C;
    s32 touched;                 /* 0x10 line touched an edge cell */
    s32 timeUp;                  /* 0x14 */
    s32 active;                  /* 0x18 camera follows */
    s32 pathLen;                 /* 0x1C 0 = not drawing */
    PathPoint path[0x2000];      /* 0x00020 */
    TraceStep trace[2][0x2000];  /* 0x14020 */
    u16 traceLen[2];             /* 0x34020 */
    u8  lineBits[0x278];         /* 0x34024 8x8-block map of the line */
} Player;
```
Directions 1..8 = N, NE, E, SE, S, SW, W, NW (table ovl2_D_800F9950).

## Functions done
| addr | name | what |
|---|---|---|
| 0x800E0000 | Game_Run | overlay entry: spawn game task, wait for result |
| 0x800E00B8 | Hud_ShowStageScore | D_800C172C/24 (max 99) in big digits + labels |
| 0x800E0230 | Hud_UpdateTimer | count timer down, show seconds; "hurry" task at 30 s |
| 0x800E0460 | Hud_UpdateCredits | credits / coin display |
| 0x800E0604 | Hud_UpdateJoin | blinking press start / insert coin; start joins |
| 0x800E0820 | Hud_UpdateGainPopup | "+NN%" claim popup, voice, timer bonus |
| 0x800E0BB8 | Game_Main | the game task (see above) |
| 0x800E2910 | Board_Init | set board buffers, clear maps |
| 0x800E29C4 | Board_GetWidth | |
| 0x800E29D0 | Board_GetHeight | |
| 0x800E29DC | Board_GetPercent | claimed percent (f32) |
| 0x800E2A24 | Board_FloodFill | scanline flood fill from (x, y) |
| 0x800E2A98 | Board_MarkBlock | set 8x8 block bit |
| 0x800E2AE8 | Board_TestBlock | test 8x8 block bit |
| 0x800E2B34 | Board_GetBlockByte | block map byte |
| 0x800E2B6C | Board_SetEdge | cell 3 + pixel |
| 0x800E2BEC | Board_SetEdgeCell | cell 3 only |
| 0x800E2C40 | Board_SetLine | cell 2 + pixel |
| 0x800E2CC0 | Board_Reveal | cell 1 + photo pixel |
| 0x800E2D54 | Board_Clear | cell 0 + pixel |
| 0x800E2DC4 | Board_SetPixel | |
| 0x800E2DF4 | Board_GetPixel | |
| 0x800E2E24 | Board_GetCell | cell value, 4 outside |
| 0x800E2EBC | Board_RevealAll | copy the whole photo |
| 0x800E2F44 | Board_ShowLineBlocks | paint P1's line blocks white (debug) |
| 0x800E3010 | Fill_Scan | flood fill: walk a run, push new runs above/below |
| 0x800E3718 | Fill_Span | flood fill: one popped point |
| 0x800E3A44 | Fill_Push | |
| 0x800E3A78 | Fill_Pop | |
| 0x800E3A90 | Fill_TopX | |
| 0x800E3AAC | Fill_TopY | |
| 0x800E3AC8 | Board_FillPixel | claim pixel: count, block, cell 1, photo |
| 0x800E3BC0 | Bits_TestAny3 | block bit in any of 3 line maps |
| 0x800E3C64 | Bits_TestInner | block bit, 1 within 16 px of the border |
| 0x800E3D28 | Player_HitTest | box hits a drawing player's position or line |
| 0x800E3E4C | Player_InBox | non-drawing player inside a box |
| 0x800E3EAC | Player_New | allocate + clear a Player |
| 0x800E3F28 | Board_ClaimRect | edge rectangle + fill inside |
| 0x800E41B8 | Player_Trace | follow the edge from the line start to the line end |
| 0x800E445C | Player_IsInside | point-in-shape (crossing count) |
| 0x800E48B0 | Player_CommitPath | line -> edge, clear line map |
| 0x800E4A30 | Player_CloseArea | trace both ways, fill the side without the monster |
| 0x800E4CFC | Player_Step | one pixel of line drawing |

Remaining: none.

## Data symbols (overlay 2; addresses are only valid while ovl2 is loaded)
```
Game_Run = 0x800E0000; // type:func segment:ovl2   (and the other function names above)
ovl2_gCellMask = 0x800F9910; // type:u8 size:0x4
ovl2_gCellVal1 = 0x800F9914; // type:u8 size:0x4
ovl2_gCellVal2 = 0x800F9918; // type:u8 size:0x4
ovl2_gCellVal3 = 0x800F991C; // type:u8 size:0x4
ovl2_gCellShift = 0x800F9920; // type:u8 size:0x4
ovl2_gBlockBit = 0x800F9924; // type:u8 size:0x8
ovl2_gDirVec = 0x800F9950; // type:s16 size:0x24   [9][2] dx,dy
ovl2_gDirFromDelta = 0x800F9974; // type:u16 size:0x12   [3][3] by dy+1, dx+1
ovl2_gCrossTable = 0x800F9988; // type:u16 size:0xA2   [9][9]
ovl2_gLineBit = 0x800F9A2C; // type:u8 size:0x8
ovl2_gLineBitMask = 0x800F9A34; // type:u8 size:0x8
ovl2_gVoiceSeed = 0x800F9900; // type:s32 size:0x4
ovl2_gEnemyCount = 0x800F6390; // type:s32 size:0x40   per difficulty
ovl2_gPhotoRom = 0x800F9880; // type:u32 size:0x80   [8 girls][4]
ovl2_gSceneObjs = 0x800F6450; // type:SceneObjDesc
ovl2_gSceneAssets = 0x800F7E54; // type:SceneAssetDesc
ovl2_gEndIconPos = 0x800F63D0; // type:f32 size:0x40   [8][2] (modified by the ending)
ovl2_gEndIconTarget = 0x800F6410; // type:f32 size:0x40
ovl2_gBoardW = 0x800FBA70; // type:s32 size:0x4
ovl2_gBoardH = 0x800FBA74; // type:s32 size:0x4
ovl2_gBoardBlocks = 0x800FBA78; // type:u8* size:0x4
ovl2_gBoardCells = 0x800FBA7C; // type:u8* size:0x4
ovl2_gBoardImage = 0x800FBA80; // type:u16* size:0x4
ovl2_gBoardPhoto = 0x800FBA84; // type:u16* size:0x4
ovl2_gBoardFilled = 0x800FBA88; // type:s32 size:0x4
ovl2_gFillDepth = 0x800FB660; // type:s32 size:0x4
ovl2_gFillStack = 0x800FB670; // type:s16 size:0x400   [256][2]
ovl2_gPlayer1 = 0x800FD3F8; // type:Player* size:0x4
ovl2_gPlayer2 = 0x800FD3FC; // type:Player* size:0x4
ovl2_gScrollX = 0x800FD2B8; // type:s32 size:0x4
ovl2_gScrollY = 0x800FD2C8; // type:s32 size:0x4
ovl2_gMonsterY = 0x800FD2B4; // type:s16 (read with lh)
ovl2_gMonsterX = 0x800FD2C4; // type:s16 (read with lh)
ovl2_gTimer = 0x800FD424; // type:s32 size:0x4   frames, starts at 7200
ovl2_gPopupFrames = 0x800FD428; // type:s32 size:0x4
ovl2_gPopupPercent = 0x800FD404; // type:s32 size:0x4
ovl2_gPopupSound = 0x800FD400; // type:s32 size:0x4
ovl2_gPopupX = 0x800FD2D0; // type:f32 size:0x4
ovl2_gPopupY = 0x800FD2DC; // type:f32 size:0x4
ovl2_gLeaving = 0x800FD2D8; // type:s32 size:0x4   1 once the photo is over
ovl2_gTimeUpFlag = 0x800FD2CC; // type:s32 size:0x4
ovl2_gBlockBuf = 0x800FD42C; // type:u8* size:0x4
ovl2_gCellBuf = 0x800FD2BC; // type:u8* size:0x4
ovl2_gPhotoBuf = 0x800FD2D4; // type:u16* size:0x4
ovl2_gImageBuf = 0x800FD548; // type:u16* size:0x4
ovl2_gTaskP1 = 0x800FB640; ovl2_gTaskP2 = 0x800FB644; ovl2_gTaskBoss = 0x800FB648;  // type:void*
ovl2_gTaskEnemies = 0x800FB600; // type:void* size:? (count from ovl2_gEnemyCount)
ovl2_gEnemyState = 0x800FD508; // type:s32 size:?
ovl2_gHudObjs = 0x800FB138; // type:Obj* size:0xC8   (timer 138-144, percent 148/14C,
                            //   stock 154[3], lives 160[4]/170[4], big digits 180[4],
                            //   continue 190/194, time up 19C, ending icons 1A0[8],
                            //   credits 1C4-1CC, press start 1D0/1D4, perfect 1D8-1E0,
                            //   attract 1E4, popup 1E8[5])
```
Main-segment globals identified (used here):
```
D_800B04D4 = P1 lives (0 = not playing); D_800B04D8 = P2 lives; D_800B04DC = random seed
D_800C30C8 = credits; D_800C0D48 / D_800C0D4A = P1 / P2 buttons (0x100 start)
D_800C52A0 = DIP switches (u16; bits 2-3 lives, bit 5 80% target)
D_800C172C = accumulated score (sum of percentages); D_800C3174 = stock icons 0..3
D_800C53D8 = coin inputs (u16[2]); D_800B4AA0 = difficulty; D_800B0608 = girl 0..7
D_800B04CC = photo of the girl 0..2; D_800C178E = u8[8] girls completed
D_800C51D8 = attract/demo frame counter; D_800C30C4 / D_800AF260 = abort requests
func_8000A76C = Task_Spawn, func_8000AEAC = Task_Yield, func_8001BA90 = Sound_Play,
func_8001BB8C = Sound_Stop, func_8001BBC0 = Music_Play, func_8000D6B8 = Sound_StopAll
```
