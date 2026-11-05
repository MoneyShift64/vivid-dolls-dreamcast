# Gfx area (0x8000F3E0-0x80015AA0)

Code: `src/game/gfx.c`, `src/game/gfx.h`. Test: `python3 tools/difftest/test_gfx.py`
(optionally followed by function names). All 41 functions PASS (300 random trials each,
Coin_Update 600, Video_Init 40, Pfs_Init/Pfs_Nop 20). Takes about 30 s.

The range is broader than graphics: controllers, Aleck64 cabinet I/O, video mode,
the frame render, viewports, full-screen quads, controller pak / EEPROM wrappers and
a few geometry helpers.

## Functions done
| Address | Name | What it does |
|---|---|---|
| 0x8000F3E0 | Pad_Read | osContGetReadData, stick dead zone 7 / clamp 60, stick as d-pad bits (0x800/0x400/0x100/0x200, real d-pad masked out), pressed edges, 8-frame auto-repeat (stick-mapped and raw) |
| 0x8000F6EC | Pad_ClearState | zero 4 per-pad u16 tables |
| 0x8000F750 | Si_IsReleased | return D_800A6840 |
| 0x8000F75C | Si_Release | D_800A6840 = 1; in SI mode 2 post D_800C5328 to D_800B0520 |
| 0x8000F7A4 | Si_Acquire | D_800A6840 = 0, spin while D_800B4AA4, clear pad tables, run a pending Pad_Read, in SI mode 1 post to the SI queue |
| 0x8000F890 | Coin_InitRates | 1 coin = 1 credit for both slots / both coinage modes, output mask 0xFFFF |
| 0x8000F8F4 | Io_ReadButtons | read players from 0x02000002 (P1 low byte, P2 high byte, active low) + one extra button each from 0x02000004 (bit 8); edges + auto-repeat |
| 0x8000FA34 | Io_ReadDipSwitches | D_800C52A0 = ~*(u16*)0x02000000 |
| 0x8000FA4C | Coin_Update | coin switches (0x02000004 bits 2/3), test (bit 4) and service (bit 5); coin = switch released after 2..29 frames, 30+ = jam; coins -> credits; coin counter pulses (64 frames each) and lockouts written to 0x0200000A |
| 0x8000FDB0 | Video_Init | clear both frame buffers and the RDP buffer, 320x240, near 10 / far 3000, copy VI mode, Video_ApplyMode, osViBlack(1) |
| 0x8000FF64 | Gfx_RenderFrame | build the frame's display list and OSTask, send to the scheduler (see below) |
| 0x800107D0 | Gfx_SetFullViewport | full-screen Vp (Y flipped when mode < 2), pipe sync, scissor; resets the Vp slot counter |
| 0x80010900 | Gfx_SetViewport | viewport for SCENE_VIEWPORT nodes: rect + scale (0x2000 = 1.0), uses one of 8 Vp slots per frame |
| 0x80010BF4 | Video_ApplyMode | width/xScale/origins from gScreenWidth (int masked), restore saved mode into current entry, osViSetMode, save, adjust start, osViSetSpecialFeatures(0xA2) |
| 0x80010D28 | Video_ApplyHStart | hStart halves + D_800386C0, each signed % 65535 |
| 0x80010DB0 | Video_ApplyVStart | vStart of both fields + D_800386C4 |
| 0x80010EB0 | ScreenQuad_SetAlpha0 | set alpha of quad D_800386D0, place node (translate x,y,-405.11322; scale sx,sy), dl = D_800387D0 |
| 0x80010F48 | ScreenQuad_SetAlpha1 | same for D_80038710 / D_80038828 |
| 0x80010FE0 | ScreenQuad_SetColor2 | rgb of D_80038750, dl D_80038880 |
| 0x80011088 | ScreenQuad_SetColorA2 | rgba of D_80038750, dl D_80038880 |
| 0x80011140 | ScreenQuad_SetColor3 | rgb of D_80038790, dl D_800388D8 |
| 0x800111E8 | ScreenQuad_SetColorA3 | rgba of D_80038790, dl D_800388D8 |
| 0x800112A0 | ScreenQuad_SetDL | place node, dl = argument |
| 0x80011334 | ScreenQuad_SetDLXform | translate, scale, rotate Y/X/Z (Mtx_PreRotateYXZ), dl = argument |
| 0x80014E90 | Pfs_Init | osCreateMesgQueue(D_800B0520, D_800C5328, 1) |
| 0x80014EC0 | Pfs_CreateFile | osPfsAllocateFile(company 1, game 1, D_80038AB0, D_80038AC0, length) |
| 0x80014FC0 | Pfs_ReadFile | find file + osPfsReadWriteFile(read) |
| 0x80015110 | Pfs_WriteFile | find file + osPfsReadWriteFile(write) |
| 0x80015260 | Pfs_DeleteFile | osPfsDeleteFile |
| 0x8001534C | Pfs_GetPluggedMask | osPfsIsPlug bit pattern |
| 0x80015384 | Pfs_CheckFile | 1 if the file is found, else -1 |
| 0x80015474 | Pfs_Nop | empty |
| 0x8001547C | Pfs_HandleError | err 2 -> osPfsInitPak again, 3 -> osPfsChecker, else -1 (tag string unused) |
| 0x800154D0 | Eeprom_Probe | D_800B0544 = probe ? 0 : -1 |
| 0x80015524 | Eeprom_Read | osEepromLongRead(0, buf, 0x200) |
| 0x8001556C | Eeprom_Write | osEepromLongWrite(0, buf, 0x200) |
| 0x800155C0 | Line3_ProjectParam | t of the closest point on a Line3f (double arithmetic) |
| 0x80015664 | Segment3_DistanceToPoint | distance from point to segment a-b |
| 0x8001581C | Plane_ProjectPoint | project point onto Planef; 0 if normal is ~0 |
| 0x800158D4 | Plane_ProjectPointD | same with f64 plane and f64 output |
| 0x80015988 | Vec3_Cross | cross product, alias safe |

All pak / EEPROM functions are wrapped in Si_Acquire / Si_Release. The pak functions
share the start: if osPfsIsPlug works and the channel bit is clear, or osPfsInitPak
fails, return -1.

## Data symbols
```
Pad_Read = 0x8000F3E0; // type:func
Pad_ClearState = 0x8000F6EC; // type:func
Si_IsReleased = 0x8000F750; // type:func
Si_Release = 0x8000F75C; // type:func
Si_Acquire = 0x8000F7A4; // type:func
Coin_InitRates = 0x8000F890; // type:func
Io_ReadButtons = 0x8000F8F4; // type:func
Io_ReadDipSwitches = 0x8000FA34; // type:func
Coin_Update = 0x8000FA4C; // type:func
Video_Init = 0x8000FDB0; // type:func
Gfx_RenderFrame = 0x8000FF64; // type:func
Gfx_SetFullViewport = 0x800107D0; // type:func
Gfx_SetViewport = 0x80010900; // type:func
Video_ApplyMode = 0x80010BF4; // type:func
Video_ApplyHStart = 0x80010D28; // type:func
Video_ApplyVStart = 0x80010DB0; // type:func
ScreenQuad_SetAlpha0 = 0x80010EB0; // type:func
ScreenQuad_SetAlpha1 = 0x80010F48; // type:func
ScreenQuad_SetColor2 = 0x80010FE0; // type:func
ScreenQuad_SetColorA2 = 0x80011088; // type:func
ScreenQuad_SetColor3 = 0x80011140; // type:func
ScreenQuad_SetColorA3 = 0x800111E8; // type:func
ScreenQuad_SetDL = 0x800112A0; // type:func
ScreenQuad_SetDLXform = 0x80011334; // type:func
Pfs_Init = 0x80014E90; // type:func
Pfs_CreateFile = 0x80014EC0; // type:func
Pfs_ReadFile = 0x80014FC0; // type:func
Pfs_WriteFile = 0x80015110; // type:func
Pfs_DeleteFile = 0x80015260; // type:func
Pfs_GetPluggedMask = 0x8001534C; // type:func
Pfs_CheckFile = 0x80015384; // type:func
Pfs_Nop = 0x80015474; // type:func
Pfs_HandleError = 0x8001547C; // type:func
Eeprom_Probe = 0x800154D0; // type:func
Eeprom_Read = 0x80015524; // type:func
Eeprom_Write = 0x8001556C; // type:func
Line3_ProjectParam = 0x800155C0; // type:func
Segment3_DistanceToPoint = 0x80015664; // type:func
Plane_ProjectPoint = 0x8001581C; // type:func
Plane_ProjectPointD = 0x800158D4; // type:func
Vec3_Cross = 0x80015988; // type:func
osContGetReadData = 0x80022048; // type:func (guess from use)
osViSetSpecialFeatures = 0x80021990; // type:func (guess: called with 0xA2)
osWritebackDCacheAll = 0x8001C990; // type:func (guess)
osPfsIsPlug = 0x80024360; // type:func (guess from use)
osPfsInitPak = 0x80024A30; // type:func (guess from use)
osPfsAllocateFile = 0x800230D0; // type:func (guess from use)
osPfsReadWriteFile = 0x80023B60; // type:func (guess from use)
osPfsDeleteFile = 0x800236C0; // type:func (guess from use)
osEepromProbe = 0x80024850; // type:func (guess from use)
osEepromLongRead = 0x800249C0; // type:func (guess from use)
osEepromLongWrite = 0x800248D0; // type:func (guess from use)
gPads = 0x800A67F8; // type:OSContPad size:0x18
gPadPresent = 0x800C30A0; // type:s16 size:0x8
gPadStickX = 0x800B04AC; // type:s16 size:0x8
gPadStickY = 0x800C5298; // type:s16 size:0x8
gPadHeld = 0x800C5334; // type:u16 size:0x8 (with stick directions)
gPadPressed = 0x800B0596; // type:u16 size:0x8
gPadRepeat = 0x800B043A; // type:u16 size:0x8
gPadRawHeld = 0x800B058C; // type:u16 size:0x8
gPadRawPressed = 0x800AF222; // type:u16 size:0x8
gPadRawRepeat = 0x800AF238; // type:u16 size:0x8
gSiReleased = 0x800A6840; // type:u16 size:0x2
gSiMode = 0x800C51D0; // type:s32 size:0x4
gSiQueue = 0x800C30A8; // type:OSMesgQueue
gPfs = 0x800A6990; // type:OSPfs size:0x1A0 (4 x 0x68)
gEepromPresent = 0x800B0544; // type:s32 size:0x4
gIoHeld = 0x800B0498; // type:u16 size:0x4 (P1, P2)
gIoPressed = 0x800C532C; // type:u16 size:0x4
gIoRepeat = 0x800B4A80; // type:u16 size:0x4
gDipSwitches = 0x800C52A0; // type:u16 size:0x2
gCredits = 0x800C30C8; // type:s32 size:0x8
gCoins = 0x800C52F4; // type:u32 size:0x8
gCoinTotal = 0x800C16B8; // type:u32 size:0x8
gCoinJam = 0x800B0574; // type:u16 size:0x4
gSharedCoins = 0x800C16E0; // type:u16 size:0x2
gIoOutputMask = 0x800C1678; // type:u16 size:0x2
gScreenHeight = 0x800C5330; // type:s32 size:0x4
gGfxMode = 0x800B0504; // type:s32 size:0x4 (microcode select 0..3)
gDisplayListHead = 0x800B053C; // type:Gfx* size:0x4
gFrameCount = 0x800C1650; // type:s32 size:0x4
gClearFrameBuffer = 0x800B05A0; // type:s32 size:0x4
gClearColorR = 0x800B0724; // type:u8 size:0x1
gClearColorG = 0x800B0618; // type:u8 size:0x1
gClearColorB = 0x800B0594; // type:u8 size:0x1
gClipNear = 0x800C1788; // type:s32 size:0x4
gClipFar = 0x800C53D0; // type:s32 size:0x4
gProjMtxF = 0x800B05B8; // type:MtxF size:0x40
gViewMtxF = 0x800C16E8; // type:MtxF size:0x40
gSceneLight = 0x800C1730; // type:SceneLight size:0x54
gFullViewport = 0x800A68F8; // type:Vp size:0x10
gViewportCount = 0x800A6908; // type:s32 size:0x4
gViewports = 0x800A6910; // type:Vp size:0x80
gViMode = 0x800A6850; // type:OSViMode size:0x50
gViModeSaved = 0x800A68A8; // type:OSViMode size:0x50
gViModeCurrent = 0x800A68A0; // type:OSViMode* size:0x4
gScreenAdjustX = 0x800386C0; // type:s32 size:0x4
gScreenAdjustY = 0x800386C4; // type:s32 size:0x4
gFrameBuffer0 = 0x807B2600; // type:u16 size:0x25800
gFrameBuffer1 = 0x807D7E00; // type:u16 size:0x25800
gZBuffer = 0x80115000; // type:u16
gSchedQueue = 0x800B0568; // type:OSMesgQueue* size:0x4
```
(The C still uses the `D_` names; rename together with symbol_addrs.txt.)

## Gfx_RenderFrame (0x8000FF64)
Per-frame task struct (`GfxTask` in gfx.h, 0x217C bytes): 0x08 kind (0x63),
0x0C frame buffer copy, 0x10-0x4F libultra OSTask, 0x50 reply queue (D_800C30E0),
0x54 msg (-> 0x2158), 0x58 perspective Mtx, 0x98 ortho Mtx, 0x118 LookAt,
0x158 display list (0x400 Gfx), 0x2158 msg buffer, 0x2178 colour frame buffer.

Order: toggle D_800B0540 on P2 button 0x100; DL head = task->dl; timing markers
(func_8001BE98) around func_8000B06C; Obj_UpdateAll; segment 0, G_DL D_800386A0,
(once) D_80038560, D_80038648; full viewport; Z image + fill Z buffer (0xFFFC);
colour image; optional fill with the clear colour (RGBA5551 packed twice); 1-cycle;
ortho (-w/2..w/2, -h/2..h/2, -1..3000) -> task->ortho; perspective (fov, w/h, near,
far) -> task->persp; LookAt into D_800C16E8 (identity if D_800B0548); view copied into
gSceneGroup1's local matrix; Light_Update; Scene_UpdateWorld for all 4 roots;
LookAt / perspnorm / 1 light / light / ambient; roots drawn as ortho(0), persp(1),
ortho(2), ortho(3); full sync, end. Task: rspboot D_80033FA0 (unless mode 1); mode
0: ucode D_80037210/D_8003CA50, FIFO output D_8013B510-D_80173510, yield 0xFC0;
mode 2: D_80034E90/D_8003BA50; mode 3: D_80035E10/D_8003C250 + FIFO; other modes
leave ucode fields. Then writeback, osSendMesg(D_800B0568, task), frame count++.

## Notes / quirks
- Gfx_SetViewport takes s32 arguments and truncates them to s16 (the original
  re-extends every argument). With a negative slot index the original's Vp write
  overlaps the index itself; clang may reorder those stores, so the test only uses
  indices >= 0 (the game always resets it to 0 each frame).
- Pfs functions read `1 << channel` (shift amount masked to 5 bits by the CPU);
  channels are 0..3.
- Pfs_GetPluggedMask returns an uninitialised byte if osPfsIsPlug doesn't write it.
- Video_OffsetStart keeps the signed `% 65535` on both halves.
- Si_Acquire spins on D_800B4AA4 (volatile in C).
- The I/O registers 0x02000000-0x0200000A are KUSEG addresses (mapped by the test).
- Test stubs: osContGetReadData writes random pads; Pfs/EEPROM stubs return random
  error codes (0/1/2/3/5) and write file numbers / plug masks; pointers to stack
  locals are normalised in the call logs. Gfx_RenderFrame runs the real matrix,
  light and scene code on a scene built with Scene_Init + 14 nodes (DL, DL list,
  viewport, group); stub logs include the DL head at each call.

## Functions remaining
None in this range.
