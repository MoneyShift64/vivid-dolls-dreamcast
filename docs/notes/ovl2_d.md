# Overlay 2, part D (0x800F00F4 - end of ovl2 code)

Files: `src/game/ovl2_d.c`, `src/game/ovl2_d.h`, test `tools/difftest/test_ovl2_d.py`.

Test: `python3 tools/difftest/test_ovl2_d.py [names...]` (overlay 2 loaded with
`make_env(overlay=2)`). Options: `TRIALS=n` (default 300), `TASK_TRIALS=n` (the long
coroutine tasks default to 100-150 runs, each covering hundreds to ~15000 frames),
`SLOW=1` (Python stubs with a readable call log for the tasks too), `LOGSTAT=1`.

How it is tested:
* Every call starts from the same snapshot; compared afterwards: return value (only
  `Effect_Spawn` returns one), a log of stubbed calls, the whole overlay (code, data,
  BSS 0x800FB050-0x800FD550), main BSS 0x8003E130-0x800C5400, the 2 MB test area and
  the 2 MB game heap.
* Stubs (Python, logging their arguments): sound (`func_8001BA90` returns a running
  voice number, `func_8001BB8C`, `func_8001BBC0`), task yield `func_8000AEAC` (counts
  frames, drives inputs / round-over / abort flags per frame), task start
  `func_8000A76C` (the child task "finishes" a random number of frames later),
  Board_GetWidth/Height, Board_GetCell (a rectangle with an edge = 3, inside = 1,
  optional random claimed noise), Board_TestBlock, Bits_TestInner, Player_HitTest /
  Player_InBox (real box test against the player position), Player_Update (random
  wander), Board_Init, Jpeg64_Decode (only writes width/height: NO photo data is
  decoded), Image_CopyRect, Image_Mosaic, PhotoView_*, Scene_Build/Destroy.
* `Obj_*` calls: Python stubs logging the used arguments for the short functions; for
  the long tasks they are replaced by small MIPS routines (trampolines to 0x80610000)
  that fold id + arguments into a hash at 0x805FFF00 (hash + count compared with the
  memory), ~20x faster. `Obj_SetUpdate` also stores fn/arg into the (fake) object.
* Real code used as is: Rand_Range, sinf/cosf, Math_VecToAngle, bzero, Heap_Alloc/Free,
  Spinner_Move (0x800EFCF0, ovl2_c's), and my own C callees in the C run.
* A "Lifecycle" test spawns random effects and then runs every object's update
  callback (like Obj_UpdateAll) for up to 300 frames, orig vs C, with players moving.
* Mutation checks (wrong comparisons, constants, turn rates, anim commands, blink
  masks, timer values, scroll limits) were caught for every big function.

## Functions done (34)

| Address | Name | What |
|---|---|---|
| `0x800F1D40` | `Game_MemClear` | Clear n bytes. |
| `0x800F1D6C` | `Effect_ClearAll` | Mark all 64 effect slots free. |
| `0x800F1D90` | `Effect_Spawn` | Take a free effect slot, hook its pre-made sprite object (update fn + slot), reset; returns index or -1. |
| `0x800F1ED4` | `Effect_UpdatePetal` | Falling petal (screen coords), freed below y = 240. |
| `0x800F200C` | `Effect_SpawnPetal` | Petal at random x, colour = slot & 3, scale 1..2. |
| `0x800F210C` | `Effect_UpdateSmoke` | Smoke puff shrinking x0.7, freed below 0.1. |
| `0x800F2208` | `Effect_SpawnSmoke` | Smoke puff, scale 0.25. |
| `0x800F2268` | `Effect_UpdateFlash` | Fading flash (alpha = scale*255, x0.9), alt sprite set-up. |
| `0x800F2384` | `Effect_SpawnFlash` | Flash, frame = slot & 3. |
| `0x800F23F4` | `Effect_UpdateMoveTo` | Eases 1/16 per frame to a target, freed within 1 px. |
| `0x800F2580` | `Effect_SpawnMoveTo` | Move-to effect. |
| `0x800F265C` | `Effect_UpdateExplosion` | Explosion (sound 0x11 every frame), one-shot anim. |
| `0x800F2768` | `Effect_SpawnExplosion` | Explosion. |
| `0x800F2798` | `Missile_Update` | Homing missile (turn 1/16, speed 4, smoke trail, 8x8 hits, life). |
| `0x800F2BB0` | `Missile_Spawn` | Missile, life 240. |
| `0x800F2C44` | `Bullet_UpdateA` | Straight bullet speed 4, 16x16 hits, freed off-field. |
| `0x800F2F6C` | `Bullet_SpawnA` | |
| `0x800F2FD4` | `Bullet_UpdateB` | Same as A (other sprite). |
| `0x800F32FC` | `Bullet_SpawnB` | |
| `0x800F3364` | `Bullet_UpdateFast` | Same, speed 8. |
| `0x800F368C` | `Bullet_SpawnFast` | |
| `0x800F36F4` | `Boomerang_Update` | Spinning boomerang: homes on a player for 180 frames, then returns home, sets *done. |
| `0x800F3B2C` | `Boomerang_Spawn` | |
| `0x800F3C24` | `Net_Update` | Net: flies until a wall, sticks 300 frames, catches/holds players (PLAYER_NETTED). |
| `0x800F4060` | `Net_Spawn` | |
| `0x800F4100` | `Task_BlinkBanner` | Task: two banner sprites blink for 90 frames (sound 6). |
| `0x800F4210` | `Task_SlideBanner` | Task: banner slides in, waits, slides out (sound 7). |
| `0x800F43E0` | `Hud_UpdateTimer2` | Count the round timer down and show it (2 big / 3 small digits). |
| `0x800F45DC` | `Bonus_Task` | Bonus stage: remove 64 tiles over the photo with button 0x10 in 600 frames; pan on success. |
| `0x800F4FC0` | `Crawler_Task` | Edge crawler ("sparx"): spirals in on an edge cell every 3600 frames, crawls the edge. |
| `0x800F5780` | `Item_Task` | Random field item (extra life / freeze / speed-up / invincibility) collected by claiming its spot. |
| `0x800F5D50` | `Clock_Task` | Clock item alternating between -10 s and +20 s states. |
| `0x800F10F0` | `Player_Task` | Player life cycle: respawn on the edge, play, die (rewind / fill task), lives. |
| `0x800F00F4` | `SpinBoss_Task` | Spinning boss: roams/shrinks, spin attack with fast bullets, or 8 homing missiles. |

Remaining: none.

Quirks kept:
* `Bonus_Task` ends with `*done = <never-written stack word> | 0x10000`; the C uses 0
  for that word and the test primes the original's slot (sp+0x5C of its 0xA0 frame) to 0.
* `Crawler_Task`: the player 2 touch test only accepts dy in -1..0 (player 1: -1..1).
* `Net_Update`: a stuck net hides itself on odd `life` frames and is never shown again.
* `Boomerang_Update`: when the round ends it frees itself but still runs the rest of
  the frame (may set *done again).
* `Effect_UpdatePetal`/`Flash`/`MoveTo` use screen coordinates, everything else field
  coordinates minus the scroll (ovl2_D_800FD2B8/C8).
* `bonus_draw`: each visible tile gets Obj_SetData 4 times (sprite colour*4 + 0..3, the
  last one wins), the hide loop starts at `(i + 3) & 0xFFFC`.
* Player blink: bit 0 of the invincibility counter while >= 60, bit 1 below.

## symbol_addrs.txt lines

```
SpinBoss_Task = 0x800F00F4; // type:func
Player_Task = 0x800F10F0; // type:func
Game_MemClear = 0x800F1D40; // type:func
Effect_ClearAll = 0x800F1D6C; // type:func
Effect_Spawn = 0x800F1D90; // type:func
Effect_UpdatePetal = 0x800F1ED4; // type:func
Effect_SpawnPetal = 0x800F200C; // type:func
Effect_UpdateSmoke = 0x800F210C; // type:func
Effect_SpawnSmoke = 0x800F2208; // type:func
Effect_UpdateFlash = 0x800F2268; // type:func
Effect_SpawnFlash = 0x800F2384; // type:func
Effect_UpdateMoveTo = 0x800F23F4; // type:func
Effect_SpawnMoveTo = 0x800F2580; // type:func
Effect_UpdateExplosion = 0x800F265C; // type:func
Effect_SpawnExplosion = 0x800F2768; // type:func
Missile_Update = 0x800F2798; // type:func
Missile_Spawn = 0x800F2BB0; // type:func
Bullet_UpdateA = 0x800F2C44; // type:func
Bullet_SpawnA = 0x800F2F6C; // type:func
Bullet_UpdateB = 0x800F2FD4; // type:func
Bullet_SpawnB = 0x800F32FC; // type:func
Bullet_UpdateFast = 0x800F3364; // type:func
Bullet_SpawnFast = 0x800F368C; // type:func
Boomerang_Update = 0x800F36F4; // type:func
Boomerang_Spawn = 0x800F3B2C; // type:func
Net_Update = 0x800F3C24; // type:func
Net_Spawn = 0x800F4060; // type:func
Task_BlinkBanner = 0x800F4100; // type:func
Task_SlideBanner = 0x800F4210; // type:func
Hud_UpdateTimer2 = 0x800F43E0; // type:func
Bonus_Task = 0x800F45DC; // type:func
Crawler_Task = 0x800F4FC0; // type:func
Item_Task = 0x800F5780; // type:func
Clock_Task = 0x800F5D50; // type:func
gEffectAnimPetal = 0x800F8F84; // type:data  EffFrame *[4]
gEffectAnimBulletA = 0x800F8FC4; // type:data  EffFrame *[1]
gEffectAnimBulletB = 0x800F8FD8; // type:data
gEffectAnimBoomerang = 0x800F8FEC; // type:data
gEffectAnimBulletFast = 0x800F9008; // type:data
gEffectAnimNet = 0x800F9084; // type:data  [state] (0 flying, 1.. stuck)
gEffectAnimMissile = 0x800F90E0; // type:data  [0] missile, [1] explosion
gEffectAnimSmoke = 0x800F90F8; // type:data
gEffectAnimFlash = 0x800F9124; // type:data
gEffectAnimMoveTo = 0x800F9140; // type:data
gSpinBossAttackChance = 0x800F9D00; // type:data size:0x40  s32[16] per difficulty (1 in n per frame)
gSpinBossSpeed = 0x800F9D40; // type:data size:0x80  {s32 base, range}[] (x0.1)
gUnstickSteps = 0x800F9DC0; // type:data size:0x20  {s32 dx, dy}[4] = (0,-8) (8,0) (0,8) (-8,0)
gEffectSeed = 0x800F9DE0; // type:data size:0x4
gBonusSceneObjs = 0x800F9DF0; // type:data  SceneObjDesc[]
gBonusSceneAssets = 0x800FA87C; // type:data  SceneAssetDesc[]
gBonusPhotos = 0x800FAA98; // type:data  16 bytes per girl, [0] = photo asset
gCrawlerDirs = 0x800FAB10; // type:data size:0x20  {s32 dx, dy}[4]
gItemKinds = 0x800FAB30; // type:data size:0x184  s32[97]
gEffects = 0x800FBA90; // type:data size:0x1300  Effect[64]
gBonusTileObjs = 0x800FCD90; // type:data size:0x100  Obj *[64]
gBonusTileSprites = 0x800FCEF8; // type:data size:0x50  void *[5][4]
gBonusPressSprites = 0x800FCF4C; // type:data size:0x8
gTimerSmallDigits = 0x800FCF58; // type:data size:0x28  void *[10]
gTimerBigDigits = 0x800FCF80; // type:data size:0x28  void *[10]
gBonusTiles = 0x800FCFA8; // type:data size:0x300  BonusTile[64]
gScrollX = 0x800FD2B8; // type:data size:0x4  s32
gFreezeTimer = 0x800FD2C0; // type:data size:0x4  s32 item 0: crawlers/boss attacks frozen
gScrollY = 0x800FD2C8; // type:data size:0x4  s32
gPlayersHold = 0x800FD2CC; // type:data size:0x4  players wait before respawning while set
gRoundOver = 0x800FD2D8; // type:data size:0x4  set at the end of a round: effects and tasks stop
gEffectObjs = 0x800FD2F8; // type:data size:0x100  Obj *[64]
gPlayers = 0x800FD3F8; // type:data size:0x8  Player *[2]
gRoundTimer = 0x800FD424; // type:data size:0x4  frames
gBlinkBannerObjs = 0x800FD4F0; // type:data  Obj *[2] for Task_BlinkBanner
```
(Other agents may already have names for the shared ovl2 globals above, e.g.
ovl2_a calls 0x800FD2D8 gStageOver; prefer theirs where they differ.)

## Structures / globals learnt

* **Effect** (0x4C, 64 slots at 0x800FBA90; one pre-made sprite Obj per slot at
  0x800FD2F8): active 0x00, `done` ptr 0x04 (boomerang), x/y 0x08, home x/y 0x10,
  target x/y 0x18, scale 0x20 (also fade), angle 0x24 (0..0x3FF), anim table 0x28,
  current sprite slot 0x2C, frame 0x30, timer 0x34, target player 0x38, state 0x3C
  (missile life / boomerang outbound time / net animation), anim 0x40 (boomerang:
  spin degrees), life 0x44 (net), caught s16[2] 0x48 (net). Effects run from their
  object's update callback (Obj.update_arg = slot).
* **EffFrame** {void **data; s32 time;}: `Obj_SetData(obj, *data)`. Effects: time < 0
  loops (or ends, or holds, per function). Tasks (items, crawler, player, boss): the
  index is pre-incremented, so lists start at index 1 (as ovl2_b noted).
  Net: -1 = next animation, -2 = restart.
* **Player** block (0x3429C, see ovl2_a/ovl2_b): id 0x00, x/y 0x04, hit 0x14,
  active 0x18, drawing 0x1C; at the end: invincibility 0x34284 (also item 3),
  speed-up 0x34288 (item 2: two Player_Update per frame), sprite frame/timer/cur
  0x3428C/0x34290/0x34294, netted 0x34298.
* **Items** (ovl2_D_800FAB30 picks kind 0..3): 0 = freeze (ovl2_D_800FD2C0 = 600: the
  crawler stops and the boss does not attack), 1 = extra life (only if no player has 3),
  2 = speed-up, 3 = invincibility, for 600 frames. Clock: +20 s / -10 s (min 1 s).
* Main globals used: D_800B04D4[2] lives, D_800B060C[2] u16 inputs, D_800C51D8 attract
  demo flag (random player input, no life lost), D_800B4AA0 difficulty, D_800B0608 girl,
  D_800C30C4 (u16) / D_800AF260 abort requests (D_800C1674 = 1 on the first).
* Tasks are started with `func_8000A76C(fn, prio, argBytes, args...)`; the task gets
  the args as normal arguments (e.g. Player_Task(done?, objs, 16, 16, anims, player) -
  its first argument is never used).
