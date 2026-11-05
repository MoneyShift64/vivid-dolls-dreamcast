# ovl2_c: overlay 2 enemies (RAM 0x800EAD80-0x800F00F4)

Files: `src/game/ovl2_c.c`, `src/game/ovl2_c.h`, test `tools/difftest/test_ovl2_c.py`
(`python3 tools/difftest/test_ovl2_c.py [trials] [names...]`, default 300 trials; the
three task functions get trials/3 runs of up to 3000 frames each).

Overlay 2 is the game itself. It is a "claim the area" game (Qix / Gals Panic style):
a claimed-area bitmap (1 bit per 8x8 cell) and up to two players; enemies bounce off
claimed cells and kill a player by touching the player or the line being drawn.

## Functions done (11 of 11; all PASS)

| Address | Name | What it does |
|---|---|---|
| 0x800EAD80 | `Minion_Task` | Task: small enemy; waits 10-19 s, appears at a random free spot, wanders, dies on touching a player/line or claimed area, repeats |
| 0x800EB4F0 | `Boss_UpdateArm` | Boss arm (4 segments + tip), bends wiggle +-120; hit-tests players |
| 0x800EB998 | `Boss_UpdateArmAim` | Boss arm whose bends steer towards player `b->target` |
| 0x800EBE9C | `Boss_MoveBounce` | Boss body drift with bounces and occasional random speed |
| 0x800EC0CC | `Boss_Task` | Task: boss with body + 4 arms; idle drift, attack A (ring of 8 shots), attack B (arm-tip shots) |
| 0x800EE230 | `Snake_MoveHead` | Snake head movement / steering / bouncing |
| 0x800EE510 | `Snake_FollowBody` | Snake body segments trail the head |
| 0x800EE7FC | `Snake_CurlFire` | Body curled; tail spits fire (`ovl2_func_800F32FC`) |
| 0x800EEB78 | `Snake_CurlBeam` | Body curled; tail fires a beam (`ovl2_func_800F3B2C`) |
| 0x800EEEEC | `Snake_Task` | Task: 8-segment snake boss; roam, fire attack, beam attack |
| 0x800EFCF0 | `Spinner_Move` | Bouncing, spinning enemy (modes 0 move / 1 settle / 2 spin up); its task is elsewhere |

## Data symbols (ready for symbol_addrs.txt)

```
gStageOver = 0x800FD2D8; // type:s32 ovl2 BSS - nonzero: stage over, enemy tasks quit
gGamePaused = 0x800FD2C0; // type:s32 ovl2 BSS - enemies freeze
gFieldScrollX = 0x800FD2B8; // type:s32 ovl2 BSS - field->screen offset
gFieldScrollY = 0x800FD2C8; // type:s32 ovl2 BSS
gBossCamX = 0x800FD2C4; // type:s16 ovl2 BSS - enemy position published for the camera
gBossCamY = 0x800FD2B4; // type:s16 ovl2 BSS
gAreaBitmap = 0x800FD42C; // type:u8* ovl2 BSS - claimed cells, 1 bit per 8x8 px (bit = cellX & 7)
gPlayers = 0x800FD3F8; // type:GamePlayer*[2] ovl2 BSS
gFieldWidth = 0x800FBA70; // type:s32 ovl2 BSS (returned by ovl2_func_800E29C4)
gFieldHeight = 0x800FBA74; // type:s32 ovl2 BSS (returned by ovl2_func_800E29D0)
gFieldPixels = 0x800FBA7C; // type:u8* ovl2 BSS - 2 bits per pixel field state
gEffectTaskArgs = 0x800FD4F0; // type:u8[] ovl2 BSS - argument passed to the effect task ovl2_func_800F4100
gDifficulty = 0x800B4AA0; // type:s32 main BSS - level/difficulty index 0..7, indexes the tables below
gPlayerActive = 0x800B04D4; // type:s32[2] main BSS - player 1/2 is playing
gMinionSpeedTbl = 0x800F9B40; // type:s32[8][2] ovl2 data - {base, range}, speed = (base+Rand(range))/10
gBossAttackRateTbl = 0x800F9BC0; // type:s32[16] ovl2 data - 1-in-N chance per frame
gBossUnstickTbl = 0x800F9C00; // type:s32[4][2] ovl2 data - {0,-8},{8,0},{0,8},{-8,0}
gSnakeAttackRateTbl = 0x800F9C20; // type:s32[16] ovl2 data
gSnakeSpeedTbl = 0x800F9C60; // type:s32[8][2] ovl2 data
gSnakeUnstickTbl = 0x800F9CE0; // type:s32[4][2] ovl2 data
gBitMask8 = 0x800F9A2C; // type:u8[8] ovl2 data - 0x01,0x02,..0x80 (bit masks used by the hit tests)
```

Function names worth adding for callees (from reading them, not decompiled here):
`ovl2_func_800E29C4` Field_GetWidth, `ovl2_func_800E29D0` Field_GetHeight,
`ovl2_func_800E2E24` Field_GetPixel (2-bit state, 4 outside), `ovl2_func_800E3C64`
Area_IsClaimed(bitmap, x, y) (1 within 16 px of the border), `ovl2_func_800E3D28`
Player_HitTest(player, x, y, w, h) (0 unless the player is drawing (+0x1C); then 1 if the
box around (x, y) contains the player position, else the bit of the player's line bitmap at
player+0x68048 under (x, y)), `func_8000AEAC` Task_Yield (saves all callee-saved
regs and switches to the scheduler; returns immediately if no current task),
`func_8000A76C` Task_Spawn(fn, priority, argBytes, args...), `func_8001BA90` Sound_Play(id)
-> s16 handle, `func_8001BB8C` Sound_Stop(handle).

## Structs

- `AnimFrame {void **frame; s32 duration;}`: animation sequence entries; `*frame` is
  passed to Obj_SetData; a negative duration ends the sequence (one-shot animations stop,
  looping ones restart at entry 0). Enemies get `AnimFrame **anims` (array of sequences).
  The index is pre-incremented, so entry 0 is only shown after a loop restart.
- `GamePlayer` (partial): +0x04 x, +0x08 y (s32), +0x14 hit flag (enemies set it to 1),
  +0x1C drawing flag, line bitmap at +0x68048.
- `BossBody` (0x2C): x, y, vx, vy, reach (arm spacing), scale, spin, anim idx/timer/frame,
  target player.
- `BossArm` (0x20): bend[4] + three tip floats. Quirk: `Boss_UpdateArm` writes
  {angle, x, y} but `Boss_UpdateArmAim` writes {x, y, angle}; the boss reads the latter
  when the arm tips shoot.
- `Mover` (0x34): snake segment: x, y, vx, vy, speed, scale, angle, turnTo, timer,
  cooldown, anim idx/timer/frame.
- `Spinner`: same first 0x1C bytes, then spin, spinSpeed, timer.
- Angles are 0..0x3FF (0x400 = 360 degrees); radians = a/512*pi; Obj.rot[2] degrees =
  a/512*180.
- Boss/Snake object arrays (24 Obj*): boss: arm k = objs[5k..5k+4] (tip last), body =
  objs[23]; snake: head = objs[7], segment i = objs[7-i]. All 24 are hidden on exit.
- Task random seeds start from an address: Minion = obj pointer, Boss = objs + 1,
  Snake = objs.

## Quirks kept

- `Boss_UpdateArmAim`: for a heading difference >= 0x200 it uses `baseAngle - 0x400`
  instead of the difference (only the sign matters, so behaviour is as intended).
- `Boss_Task`: during attacks the body rotation is the raw spin value (not degrees); the
  aiming arms index `gPlayers[target]` even when target is -1 after an attack B with no
  players.
- `Snake_Task`: every segment's random start heading is written to seg[0].angle (last one
  wins). While waiting for the beam the original passes an uninitialised stack word as the
  beam target; it is never used there (fire flag is 0), the C passes the last target.
- `Snake_CurlFire`: the fire sound plays on frames that are NOT multiples of 8.
- Pausing cancels the snake's fire attack and the boss's arm-tip shooting phase.

## Testing notes

- Task functions are run with Task_Yield stubbed: every frame records a snapshot of the
  objects, the overlay BSS and the players; a seeded script toggles pause, clears hit
  flags, changes which players are playing, sets the beam-done flag and the stage-over flag.
- Watch hooks log the inputs to sinf/cosf/Math_VecToAngle/Rand_Range/the hit tests and dump
  the enemy structs at each call of the arm/head functions, so internal float state is
  compared exactly (object positions alone are truncated to ints).
- The attack-rate tables in overlay data are lowered in RAM during tests so attacks happen
  often.

## Remaining

None in this range.
