# Overlay 2, part B (0x800E4EDC - 0x800EAD80): player movement and enemy tasks

Files: `src/game/ovl2_b.c`, `src/game/ovl2_b.h`, test `tools/difftest/test_ovl2_b.py`.

Test: `python3 tools/difftest/test_ovl2_b.py` (all 7 functions, ~7 min; `TRIALS=n` env var
and function names as arguments for quick runs). Player_DrawMove runs 3x and Player_EdgeMove
8x the trial count, because their decision trees are large.

Every trial builds a random play field (2-bit grid: claimed rectangles with edge-line
outlines, optional noise; dense random 7x7 neighbourhoods around the player for the
corner and junction cases; scroll positions that put the player 0-2 pixels from the
view edges), random frame buffer and photo, two players, an enemy block bitmap and
animation lists. Trails come from replaying the ORIGINAL Player_Update with random input.
Compared: both GamePlayer structs (0x342A0 bytes each), the grid, the frame buffer, the
whole overlay BSS (0x800FB050-0x800FD550), the fake Obj, and a log of stubbed calls
(task yield 0x8000AEAC, sound 0x8001BA90, Obj_SetPos/SetData/Hide, shape fill
0x800E4A30). The other overlay helpers (cell access, drawing step 0x800E4CFC,
collision tests 0x800E3C64/0x800E3D28) run as the original code. Instruction coverage
of the originals: 100% except unreachable bounds-fail stubs (two-instruction blocks
behind checks that an earlier check already passed) in DrawMove/EdgeMove and the padding.
Mutations checked (all caught): slide-side order, diagonal axis order, the
"edge reached through open cell" rule, view-boundary off-by-ones (`>` vs `>=`) in
three places, field-boundary off-by-one, angle turn step, missing block-bit clear.

## Functions done (7)

| Address | Name | What |
|---|---|---|
| `0x800E4EDC` | `Player_DrawMove` | Draw button held: extend the trail 2 px in direction 0..8. Straight moves push 2 real steps + 2 (0,0) steps (a frame always pushes 4 TrailPoints); diagonals push 2+2 steps (horizontal first, or vertical first if only that way is open), falling back to vertical then horizontal. Target must be open, or an edge line reached through an open cell; trail limited to 0x2000 points; target must be 8 px inside field and view. |
| `0x800E664C` | `Player_EdgeMove` | Walk 2 px along edge lines (cells == 3). Straight blocked -> slide round a corner sideways, preferring the last direction on that axis (`edgeDirX/Y`). Diagonals: up-right tries vertical then horizontal, the others horizontal then vertical. |
| `0x800E8F74` | `Player_Update` | Per-frame control: input lock timer, draw (falls back to edge walk if no trail), edge walk, or rewind 1 frame of trail when the button is released (only if the rewind point is in view; otherwise returns at once without the closed-shape check); calls 0x800E4A30 when `closed`. |
| `0x800E93EC` | `Player_RewindTask` | Task: rewind the whole trail, 2 frames per yield, until empty or stop flag; `*done = 1`. |
| `0x800E9B10` | `Player_FillTrailTask` | Task: turn the trail into claimed cells (0x800E2CC0), 2 frames per yield; `*done = 1`. |
| `0x800E9DC0` | `Enemy_BounceTask` | Task (args: unused, Obj, EnemyAnims): wait 10-19 s, appear on a random open cell, bounce off solid blocks (1 in 10 bounces re-rolls the speed), vanish on touching a player (sets `hit`) or a solid block; repeat until the stop flag. RNG seed = the Obj address. |
| `0x800EA610` | `Enemy_WanderTask` | Same, but moves along an angle (0..1023) turning 4 units/frame towards a target angle re-rolled every 60-89 frames; bounces recompute the angle with Math_VecToAngle; sprite rotation `obj->rot[2]`. |

Quirks kept: animation lists start at index 1 (index 0 is only reached by the looping
"move" list after it wraps); the speed is `(f32)((f64)(f32)(base + rand) / 10.0)`;
the doubled loop bodies of the two trail tasks (two steps per yield, the second one
without a yield/stop check); Player_Update reads `trail[len-4]` even when len < 4.

Implementation note: Player_EdgeMove (10.5 KB of compiler-merged decision tree) was
reconstructed with a small symbolic executor over the assembly that summarised every
check block as "target in view && cell(c) == 3" with its true/false successors; the
whole tree turned out to be the regular pattern written as helpers in the C.

## Structures (see ovl2_b.h)

`GamePlayer` (player 1 = `ovl2_D_800FD3F8`, player 2 = `ovl2_D_800FD3FC`, >= 0x3429C bytes):
`id` 0x0 (0 = player 1; trail colour 0xFD29 / 0xA53F), `x` 0x4, `y` 0x8, `closed` 0x10
(set by 0x800E4CFC when a step reaches an edge line), `hit` 0x14 (set by enemies),
`trailLen` 0x1C, `trail[0x2000]` 0x20 (TrailPoint, 10 bytes: s8 dx, s8 dy, u16 dirCode
(from a table at 0x800F997A indexed by dx and dy), s16 x, s16 y (position before the step), u16 saved
frame-buffer pixel), fill work buffers 0x14020 / 0x24020 (0x10000 each, used by
0x800E41B8 called from 0x800E4A30), results 0x34020/0x34022 (s16), `blockBits` 0x34024
(1 bit per 8x8 block crossed by the trail, bit (bx & 7) LSB first, byte
(bx + by*(W/8)) >> 3), `edgeDirY` 0x3427C, `edgeDirX` 0x34280, `inputLock` 0x34298.

Play field grid (`ovl2_D_800FBA7C`): 2 bits per cell, cell i at bits (i&3)*2 of byte i>>2,
i = x + y*W. Values: 0 open, 1 claimed, 2 trail, 3 edge line; 0x800E2E24 returns 4
outside. Tables: 0x800F9910 clear masks (~(3<<2i)), 0x800F9914/18/1C value 1/2/3 at
position i, 0x800F9920 shifts.

`EnemyAnims` = 3 pointers (appear, move, vanish) to `AnimFrame {void **sprite; s32 frames;}`
lists ending with frames < 0.

## symbol_addrs.txt lines

```
Player_DrawMove = 0x800E4EDC; // type:func
Player_EdgeMove = 0x800E664C; // type:func
Player_Update = 0x800E8F74; // type:func
Player_RewindTask = 0x800E93EC; // type:func
Player_FillTrailTask = 0x800E9B10; // type:func
Enemy_BounceTask = 0x800E9DC0; // type:func
Enemy_WanderTask = 0x800EA610; // type:func
ovl2_D_800F9910 = 0x800F9910; // type:u8 size:0x4 cell clear masks ~(3 << 2i)
ovl2_D_800F9914 = 0x800F9914; // type:u8 size:0x4 cell value 1 at position i
ovl2_D_800F9918 = 0x800F9918; // type:u8 size:0x4 cell value 2 at position i
ovl2_D_800F991C = 0x800F991C; // type:u8 size:0x4 cell value 3 at position i
ovl2_D_800F9920 = 0x800F9920; // type:u8 size:0x4 cell shifts 0,2,4,6
ovl2_D_800F9940 = 0x800F9940; // type:u8 size:0x10 d-pad bits -> direction 0..8
ovl2_D_800F9A2C = 0x800F9A2C; // type:u8 size:0x8 1 << i
ovl2_D_800F9A34 = 0x800F9A34; // type:u8 size:0x8 ~(1 << i)
ovl2_D_800F9A40 = 0x800F9A40; // type:s32 size:0x60 Enemy_BounceTask speed {base, range}[12]
ovl2_D_800F9AC0 = 0x800F9AC0; // type:s32 size:0x60 Enemy_WanderTask speed {base, range}[12]
ovl2_D_800FBA70 = 0x800FBA70; // type:s32 play field width
ovl2_D_800FBA74 = 0x800FBA74; // type:s32 play field height
ovl2_D_800FBA7C = 0x800FBA7C; // type:u8* cell grid (2 bits per cell)
ovl2_D_800FBA80 = 0x800FBA80; // type:u16* frame buffer
ovl2_D_800FBA84 = 0x800FBA84; // type:u16* photo
ovl2_D_800FD2B8 = 0x800FD2B8; // type:s32 scroll x
ovl2_D_800FD2C0 = 0x800FD2C0; // type:s32 enemies frozen
ovl2_D_800FD2C8 = 0x800FD2C8; // type:s32 scroll y
ovl2_D_800FD2D8 = 0x800FD2D8; // type:s32 task stop flag
ovl2_D_800FD3F8 = 0x800FD3F8; // type:GamePlayer* player 1
ovl2_D_800FD3FC = 0x800FD3FC; // type:GamePlayer* player 2
ovl2_D_800FD42C = 0x800FD42C; // type:u8* enemy solid-block bitmap
D_800B4AA0 = 0x800B4AA0; // type:s32 difficulty level (speed table index)
```

Suggested names for callees in other areas (from how they are used here):
`func_8000AEAC` Task_Yield (saves all registers and switches task), `func_8001BA90`
Sound_Play, `ovl2_func_800E29C4` Field_Width, `ovl2_func_800E29D0` Field_Height,
`ovl2_func_800E2E24` Field_GetCell, `ovl2_func_800E2CC0` Field_Claim (cell 1 + photo
pixel), `ovl2_func_800E2D54` Field_Open (cell 0 + given pixel), `ovl2_func_800E2C40`
Field_SetTrail (cell 2 + colour), `ovl2_func_800E2DF4` Field_GetPixel,
`ovl2_func_800E4CFC` Draw_Step, `ovl2_func_800E4A30` Player_CloseShape,
`ovl2_func_800E3C64` Blocks_IsSolid, `ovl2_func_800E3D28` Player_Touches.

## Remaining

None in 0x800E4EDC-0x800EAD80.
