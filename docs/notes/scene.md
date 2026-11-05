# Scene area (0x800113E0-0x80012820)

Code: `src/game/scene.c`, `src/game/scene.h`. Test: `python3 tools/difftest/test_scene.py`
(all 19 functions PASS; 300 random trials each, Scene_Init 50).

## Functions done
| Address | Name | What it does |
|---|---|---|
| 0x800113E0 | Main_ParseArgs | split a command line at spaces (NULs written over them) and set flags for -v/-d/-s/-l; stops at first arg without '-'; room for 31 args only |
| 0x80011550 | Scene_Init | init the node pool (0xC000 bytes, 0xE0 blocks), create 4 root groups each with one child group |
| 0x800116B8 | Scene_NewNode | allocate + zero a node of a type, identity local matrix; groups get head/tail sentinels, viewports full-screen defaults |
| 0x8001179C | Scene_CountFreeNodes | free blocks left in the pool |
| 0x800117BC | Scene_DeleteNode | recursive delete (children, sentinels), unlink from sibling list, free |
| 0x80011868 | Scene_UpdateWorld | world = local * parent (inlined 4x4 product), recurse |
| 0x80011C20 | Scene_ConvertMatrices | world MtxF -> fixed-point Mtx at 0x90 (func_8000A924), recurse |
| 0x80011C88 | Scene_BuildGfx | emit F3D commands for a subtree (G_DL 0x06, G_MTX 0x01020040 = MODELVIEW/LOAD/NOPUSH), returns new list end |
| 0x80011E60 | Camera_Init | eye (0,0,405.11322), target 0, up +Y, fov 33 |
| 0x80011ED0 | Camera_SetEye | set eye, re-orthogonalise up (two cross products) |
| 0x80011F40 | Camera_SetTarget | set target, re-orthogonalise up |
| 0x80011FB0 | Camera_GetForward | normalised target - eye (reciprocal in double) |
| 0x80012090 | Light_Init | light at (0,0,810.22644) aimed at origin, white, no ambient, identity view |
| 0x80012130 | Light_SetPos | set pos, recompute dir |
| 0x80012158 | Light_SetTarget | set target, recompute dir |
| 0x80012180 | Light_SetColor | colour clamped to 0..255 |
| 0x80012250 | Light_SetAmbient | ambient clamped to 0..255 |
| 0x80012320 | Light_Update | build N64 Lights1 at +0x40: dir * view, length 120 as s8; colours as bytes (float->u32 conversion kept) |
| 0x80012760 | Light_CalcDir | out = (target - pos) * (-120 / length) |

## Data symbols
```
Main_ParseArgs = 0x800113E0; // type:func
Scene_Init = 0x80011550; // type:func
Scene_NewNode = 0x800116B8; // type:func
Scene_CountFreeNodes = 0x8001179C; // type:func
Scene_DeleteNode = 0x800117BC; // type:func
Scene_UpdateWorld = 0x80011868; // type:func
Scene_ConvertMatrices = 0x80011C20; // type:func
Scene_BuildGfx = 0x80011C88; // type:func
Camera_Init = 0x80011E60; // type:func
Camera_SetEye = 0x80011ED0; // type:func
Camera_SetTarget = 0x80011F40; // type:func
Camera_GetForward = 0x80011FB0; // type:func
Light_Init = 0x80012090; // type:func
Light_SetPos = 0x80012130; // type:func
Light_SetTarget = 0x80012158; // type:func
Light_SetColor = 0x80012180; // type:func
Light_SetAmbient = 0x80012250; // type:func
Light_Update = 0x80012320; // type:func
Light_CalcDir = 0x80012760; // type:func
gArgVerbose = 0x800AF230; // type:s32 size:0x4 (-v)
gArgDebug = 0x800B04C8; // type:s32 size:0x4 (-d)
gArgS = 0x800B04B4; // type:s32 size:0x4 (-s)
gArgL = 0x800B0728; // type:s32 size:0x4 (-l)
gSceneNodePool = 0x800B4AB0; // type:u8 size:0xC000
gSceneRoot0 = 0x800C316C; // type:SceneNode* size:0x4
gSceneRoot1 = 0x800C30C0; // type:SceneNode* size:0x4
gSceneRoot2 = 0x800C0AB4; // type:SceneNode* size:0x4
gSceneRoot3 = 0x800AF234; // type:SceneNode* size:0x4
gSceneGroup0 = 0x800B061C; // type:SceneNode* size:0x4 (child of root0)
gSceneGroup1 = 0x800B04A0; // type:SceneNode* size:0x4 (child of root1)
gSceneGroup2 = 0x800AF240; // type:SceneNode* size:0x4 (child of root2)
gSceneGroup3 = 0x800B0500; // type:SceneNode* size:0x4 (child of root3)
gCameraEye = 0x800B0444; // type:Vec3f size:0xC
gCameraTarget = 0x800B04B8; // type:Vec3f size:0xC
gCameraUp = 0x800C1664; // type:Vec3f size:0xC
gCameraFov = 0x800C51D4; // type:f32 size:0x4
```
(The C still uses the `D_` names; rename together with symbol_addrs.txt.)

Callees outside the area, guessed roles: 0x80009D40 Pool_Init(pool, size, block),
0x80009DB4 Pool_Alloc, 0x80009DF4 Pool_CountFree, 0x80009E2C Pool_Free (a ring of
block pointers; header 0x10 bytes), 0x8000A924 MtxF -> fixed-point Mtx,
0x80010900 viewport commands (gfx, x, y, w, h, sx, sy; uses D_800A6908 index and
D_800A6910 Vp table), 0x80015988 Vec3 cross product (alias-safe).

## Functions remaining
None in this range.

## SceneNode layout (0xE0 bytes, pool block)
| Off | Type | Field |
|---|---|---|
| 0x00 | SceneNode* | next sibling (NULL only on a tail sentinel) |
| 0x04 | SceneNode* | prev sibling (NULL only on a head sentinel) |
| 0x08 | s32 | type: 0 sentinel, 1 group, 2 display list, 3 DL array, 4 viewport (others: drawn as nothing) |
| 0x0C | MtxF | local matrix (identity on creation) |
| 0x4C | MtxF | world = local * parent world |
| 0x8C | u32 | unused here |
| 0x90 | Mtx (s32[16]) | fixed-point world matrix for the RSP |
| 0xD0 | union | group: head, tail sentinels (0xD0/0xD4). DL: dl, setup_dl, finish_dl (0xD0/0xD4/0xD8). DL array: lists ptr, s32 count (loop index is u16). viewport: s16 x, y, w, h, sx, sy (0xD0..0xDA; default 0, 0, gScreenWidth-1, 239, 0x2000, 0x2000) |
| 0xDC | | unused |

Child list: group->head <-> children... <-> group->tail; children are walked with
`c = head->next; while (c->next) {...; c = c->next;}`, saving next before recursing so
deletion is safe. New children are appended just before the tail (only done inline in
Scene_Init here; sprite/object code elsewhere probably does the same).

Draw order for a type-2 node: setup_dl, G_MTX(gfx_mtx) + dl (only if dl), finish_dl.

## SceneLight layout (at least 0x54 bytes)
0x00 Vec3f dir (work), 0x0C Vec3f pos, 0x18 Vec3f target, 0x24 f32 color[3],
0x30 f32 ambient[3], 0x40 N64 Lights1 (ambient col/colc 0x40/0x44, light col/colc
0x48/0x4C, s8 dir[3] at 0x50, pads zeroed).

## Notes
- Testing: pool-based create/delete sequences use the real pool at 0x800B4AB0; tree
  tests build nodes at 0x80400000 including sentinels, unknown types, NULL DLs, negative
  DL counts, special floats and parent == own world aliasing; func_80010900 is stubbed
  and its 7 arguments (incl. stack args) are compared.
- Not distinguishable by the tests but kept as in the original: u16 loop index in DL
  arrays; Camera_GetForward reciprocal computed in double.
- Scene_NewNode(1) with an exhausted pool writes through NULL (original behaviour);
  Scene_DeleteNode on a node not in a list writes through NULL.
