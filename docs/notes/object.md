# Object / entity system (0x80016D20 - 0x80019D70)

Files: `src/game/object.c`, `src/game/object.h`, test `tools/difftest/test_object.py`.

Test: `python3 tools/difftest/test_object.py` (all 70 functions, 300 trials each, ~3 min;
`TRIALS=n` env var and function names as arguments for quick runs). Every call starts
from the same snapshot (scene system initialised with the original `func_80011550`,
object list initialised, empty heap, gScreenWidth = 320); setup is replayed with the
ORIGINAL functions; return value, the whole BSS (0x8003E130-0x800C5400), the whole
2 MB game heap, the 256 KB test area, D_80038C00..+0x80 and a log of callbacks made
through a Python stub are compared. Sprites are real ROM sprites; model assets are
synthesised (literal-only LZSS) into the unused ROM area at 0x700000 because no model
asset was found among the 506 Siff assets. Mutation checks (bugs injected into
Model_Load, Text_Build, Font_TextWidth, Obj_UpdateAll, Scene_Build) were all caught.

The test hooks the emulator's stubs only at their own addresses (N64Emu hooks every
instruction with a Python callback, ~15x slower); see `make_fast_env()` - this could be
moved into `emu.py` for everyone.

## Functions done (70)

| Address | Name | What |
|---|---|---|
| `0x80016D20` | `Font_New` | Font with one sprite per glyph (see font_new). |
| `0x80016E58` | `Font_NewSharedPalette` | Same as Font_New, glyph sprites share one palette. |
| `0x80016F90` | `Font_Delete` | Free a font and all its glyph sprites. |
| `0x80017030` | `TextBuf_New` | Allocate a text display-list buffer (contents left uninitialised except `cur`). |
| `0x8001705C` | `TextBuf_Delete` | Free a TextBuf. |
| `0x80017080` | `Text_Build` | Build the display list for `str` at (x, y) into the TextBuf's other buffer and * return it (double buffering: tb->cur flips each call). |
| `0x80017304` | `Text_BuildTlut` | Text_Build that first loads a palette (TLUT) from `tlut`: 256 colours if the * font's first sprite is 8-bit (format 1), else 16. |
| `0x8001763C` | `Text_Attach` | Build text and, if `node` is given, hook the result (plus the font's sprite * set-up/finish lists) into it. Returns tb. |
| `0x800176C4` | `Text_AttachTlut` | Text_Attach with Text_BuildTlut. |
| `0x80017754` | `Font_TextWidth` | Width in pixels of (up to 32 characters of) `str`. |
| `0x800177B0` | `AllocList_Init` | Empty list between two sentinel nodes. |
| `0x800177F4` | `AllocList_Add` | Record (heap, ptr) in a new node (from the game heap) before `tail`. |
| `0x80017878` | `AllocList_FreeAll` | Free every recorded block and its node (nodes go back to `heap`). |
| `0x80017900` | `Model_New` | Load a model (allocated if m is NULL), with lighting set-up commands. |
| `0x80017984` | `Model_NewUnlit` | Model_New without the lighting/texture-gen commands. |
| `0x80017A08` | `Model_Delete` | Free a model's buffer, and the Model itself if Model_New allocated it. |
| `0x80017A64` | `Model_Load` | Load a model asset (LZSS). Header: u32[8] = { header0, off1..off6, gfx count }, * sections [off1,off3) vertices etc., [off3,off4) 16-byte texture records, * [off4,off5) images, [off5,off6) aux data; then a u16 command stream that is * translated into an F3D display list after the copied data: * 00xx triangle (three vertex nibbles, x10) * 01xx load vertices (next word = first vertex); with `lighting`, a change in * bits 8-9 also switches texture generation on (0x200) or off * 10xx load texture record (bits 0-5): bits 8-9 = 0 RGBA16, 1 CI4, 2 CI8, 3 none * 11xx end |
| `0x800182E4` | `Mem_Copy` | Byte copy; returns dst. |
| `0x8001831C` | `Math_Ctz16` | Number of trailing zero bits of a 16-bit value (log2 of a power of two). * Quirk: loops forever if the low 16 bits are 0. |
| `0x80018350` | `Obj_NewDL` | Raw display-list object; NULL setup/finish lists default to D_80038F20. |
| `0x800183F4` | `Obj_NewDefaultDL` | Raw display-list object using the built-in default lists. |
| `0x8001843C` | `Obj_NewView` | Viewport object (no display lists; its node is a kind-4 viewport node). |
| `0x80018464` | `Obj_NewModel` | Model object that loads and owns its model. |
| `0x800184E4` | `Obj_NewSpritePlain` | Sprite object that loads and owns a plain sprite (setup D_80038C80). |
| `0x80018578` | `Obj_NewSpritePlain2` | Same as Obj_NewSpritePlain with set-up list D_80038CC0. |
| `0x8001860C` | `Obj_UseSetupCC0` | Switch a sprite object to set-up list D_80038CC0. |
| `0x80018628` | `Obj_UseSetupC80` | Switch a sprite object to set-up list D_80038C80. |
| `0x80018644` | `Obj_NewSpritePlainRef` | Sprite object for an existing plain sprite (not owned). |
| `0x800186B0` | `Obj_NewSprite` | Sprite object that loads and owns a sprite (setup D_80038D00). |
| `0x80018754` | `Obj_NewSprite2` | Same as Obj_NewSprite with set-up list D_80038D50. |
| `0x800187F8` | `Obj_NewSpriteRef` | Sprite object for an existing sprite (not owned). |
| `0x80018874` | `Obj_NewScreen` | Screen-sprite object that loads and owns its sprite. |
| `0x80018908` | `Obj_NewText` | Text object with its own TextBuf; the text is built at (0, 0). |
| `0x800189D4` | `Obj_NewTextTlut` | Obj_NewText that loads a palette first (Text_BuildTlut). |
| `0x80018AB0` | `Obj_NewModelRef` | Model object for an existing model (not owned). |
| `0x80018B08` | `Obj_NewScreenRef` | Screen-sprite object for an existing screen sprite (not owned). |
| `0x80018B74` | `Obj_NewTextRef` | Text object using a caller-supplied TextBuf. |
| `0x80018C3C` | `Obj_NewTextRefTlut` | Obj_NewTextRef with Text_BuildTlut. |
| `0x80018D14` | `Obj_SetText` | Rebuild a text object's string and re-attach it. |
| `0x80018D64` | `Obj_SetTextTlut` | Obj_SetText with Text_BuildTlut. |
| `0x80018DBC` | `ObjList_Init` | Clear both sentinels and link them into an empty list. |
| `0x80018E20` | `Obj_Kill` | Request deletion (done over the next two Obj_UpdateAll calls). |
| `0x80018E34` | `Obj_SetData` | Replace the object's data (ignored if the object owns its data). |
| `0x80018E90` | `Obj_SetModelAlt2` | Use model set-up list D_80038EE0 (and re-attach). |
| `0x80018EA4` | `Obj_SetModelAlt` | Use model set-up list D_80038E60 / D_80038EA0 (and re-attach). |
| `0x80018EB8` | `Obj_SetColor` | Primitive colour (gDPSetPrimColor) in the object's set-up list. |
| `0x80018EF4` | `Obj_SetPos` | Set the position from integers and mark the transform dirty. |
| `0x80018F38` | `Obj_FlipX` | Mirror in X (3D objects) and re-transform. |
| `0x80018F4C` | `Obj_FlipY` | Mirror in Y (3D objects) and re-transform. |
| `0x80018F60` | `Obj_FlipZ` | Mirror in Z (3D objects) and re-transform. |
| `0x80018F74` | `Obj_Invalidate` | Mark the transform dirty. |
| `0x80018F88` | `Obj_Show` | Make visible (attach next update). |
| `0x80018FA0` | `Obj_Hide` | Hide (detach next update). |
| `0x80018FB8` | `Obj_SetUpdate` | Set the per-frame update callback and its argument. |
| `0x80018FC8` | `Obj_ClearUpdate` | Remove the update callback. |
| `0x80018FD4` | `Obj_ClearTransform` | Remove the transform callback (e.g. after Obj_SetMatrix). |
| `0x80018FE0` | `Obj_SetMatrix` | Set the node matrix directly. |
| `0x80019010` | `Obj_SetLights` | Use other lights: light 0 at lights+0x48, ambient at lights+0x40. |
| `0x80019040` | `Obj_UpdateAll` | Run every object once: deferred deletion (kill -> free node -> free object), * update callback, transform callback when dirty, then attach/detach. |
| `0x80019174` | `Obj_Create` | Allocate a zeroed object, append it to the object list and give it a scene node * (a viewport node for OBJ_VIEW, else a drawable node with default lights, * white primitive colour and scale 1) linked last under `parent`. |
| `0x8001933C` | `Obj_FreeData` | Free owned data (OBJF_OWNS_DATA) according to the object type. |
| `0x800193C4` | `Obj_FreeNode` | Free the object's scene node. |
| `0x800193F0` | `Obj_Transform3D` | Node matrix = translate(pos) * scale * rotate [* flips] * translate(-pivot). |
| `0x80019520` | `Obj_Transform2D` | Same for 2D sprites, in screen coordinates (origin top-left, y down, 320x240). |
| `0x800195F4` | `Obj_TransformView` | Viewport: pos[0..2], rot[0..2] truncated into the node's six s16 fields. |
| `0x80019688` | `Obj_TransformScreen` | Screen sprites rebuild their rectangles from pos and scale. |
| `0x800196BC` | `Obj_Attach` | Point the scene node at the object's display lists. |
| `0x800197C8` | `Obj_Detach` | Unhook the display lists from the scene node (hide). |
| `0x80019800` | `Scene_Build` | Load every asset in `assets`, then create every object in `objs` and apply its * colour, position, scale and visibility. Kind 13 (viewport) maps the fields to * pos/rot instead. Unknown kinds (9, 0, >13) create nothing but still apply the * settings to whatever object is already in the slot. |
| `0x80019C9C` | `Scene_Destroy` | Kill every object of the table and free every asset (models with Model_Delete, * all other types as sprites), clearing the slots. |

Remaining: none in 0x80016D20-0x80019CA0 (0x80019C9C `Scene_Destroy` is included).

## symbol_addrs.txt lines

```
Font_New = 0x80016D20; // type:func
Font_NewSharedPalette = 0x80016E58; // type:func
Font_Delete = 0x80016F90; // type:func
TextBuf_New = 0x80017030; // type:func
TextBuf_Delete = 0x8001705C; // type:func
Text_Build = 0x80017080; // type:func
Text_BuildTlut = 0x80017304; // type:func
Text_Attach = 0x8001763C; // type:func
Text_AttachTlut = 0x800176C4; // type:func
Font_TextWidth = 0x80017754; // type:func
AllocList_Init = 0x800177B0; // type:func
AllocList_Add = 0x800177F4; // type:func
AllocList_FreeAll = 0x80017878; // type:func
Model_New = 0x80017900; // type:func
Model_NewUnlit = 0x80017984; // type:func
Model_Delete = 0x80017A08; // type:func
Model_Load = 0x80017A64; // type:func
Mem_Copy = 0x800182E4; // type:func
Math_Ctz16 = 0x8001831C; // type:func
Obj_NewDL = 0x80018350; // type:func
Obj_NewDefaultDL = 0x800183F4; // type:func
Obj_NewView = 0x8001843C; // type:func
Obj_NewModel = 0x80018464; // type:func
Obj_NewSpritePlain = 0x800184E4; // type:func
Obj_NewSpritePlain2 = 0x80018578; // type:func
Obj_UseSetupCC0 = 0x8001860C; // type:func
Obj_UseSetupC80 = 0x80018628; // type:func
Obj_NewSpritePlainRef = 0x80018644; // type:func
Obj_NewSprite = 0x800186B0; // type:func
Obj_NewSprite2 = 0x80018754; // type:func
Obj_NewSpriteRef = 0x800187F8; // type:func
Obj_NewScreen = 0x80018874; // type:func
Obj_NewText = 0x80018908; // type:func
Obj_NewTextTlut = 0x800189D4; // type:func
Obj_NewModelRef = 0x80018AB0; // type:func
Obj_NewScreenRef = 0x80018B08; // type:func
Obj_NewTextRef = 0x80018B74; // type:func
Obj_NewTextRefTlut = 0x80018C3C; // type:func
Obj_SetText = 0x80018D14; // type:func
Obj_SetTextTlut = 0x80018D64; // type:func
ObjList_Init = 0x80018DBC; // type:func
Obj_Kill = 0x80018E20; // type:func
Obj_SetData = 0x80018E34; // type:func
Obj_SetModelAlt2 = 0x80018E90; // type:func
Obj_SetModelAlt = 0x80018EA4; // type:func
Obj_SetColor = 0x80018EB8; // type:func
Obj_SetPos = 0x80018EF4; // type:func
Obj_FlipX = 0x80018F38; // type:func
Obj_FlipY = 0x80018F4C; // type:func
Obj_FlipZ = 0x80018F60; // type:func
Obj_Invalidate = 0x80018F74; // type:func
Obj_Show = 0x80018F88; // type:func
Obj_Hide = 0x80018FA0; // type:func
Obj_SetUpdate = 0x80018FB8; // type:func
Obj_ClearUpdate = 0x80018FC8; // type:func
Obj_ClearTransform = 0x80018FD4; // type:func
Obj_SetMatrix = 0x80018FE0; // type:func
Obj_SetLights = 0x80019010; // type:func
Obj_UpdateAll = 0x80019040; // type:func
Obj_Create = 0x80019174; // type:func
Obj_FreeData = 0x8001933C; // type:func
Obj_FreeNode = 0x800193C4; // type:func
Obj_Transform3D = 0x800193F0; // type:func
Obj_Transform2D = 0x80019520; // type:func
Obj_TransformView = 0x800195F4; // type:func
Obj_TransformScreen = 0x80019688; // type:func
Obj_Attach = 0x800196BC; // type:func
Obj_Detach = 0x800197C8; // type:func
Scene_Build = 0x80019800; // type:func
Scene_Destroy = 0x80019C9C; // type:func
gFontDefs = 0x80038C00; // type:data size:0x10  FontDef *[4] (see quirk below)
gModelSetupDL = 0x80038C10; // type:data size:0x70  Gfx[], Model.setup_dl
gObjSpritePlainSetupDL = 0x80038C80; // type:data size:0x40
gObjSpritePlainSetupDL2 = 0x80038CC0; // type:data size:0x40
gObjSpriteSetupDL = 0x80038D00; // type:data size:0x50  (also text objects)
gObjSpriteSetupDL2 = 0x80038D50; // type:data size:0x50
gObjScreenSetupDL = 0x80038DA0; // type:data size:0x40
gObjModelSetupDL = 0x80038DE0; // type:data size:0x80
gObjModelAltSetupDL = 0x80038E60; // type:data size:0x40  (OBJF 0x200)
gObjModelAltFlipSetupDL = 0x80038EA0; // type:data size:0x40  (OBJF 0x200 + a flip flag)
gObjModelAlt2SetupDL = 0x80038EE0; // type:data size:0x40  (OBJF 0x400)
gObjDefaultDL = 0x80038F20; // type:data size:0x48  Obj_NewDL default setup/finish
gObjDefaultSetupDL = 0x80038F68; // type:data size:0x40  Obj_NewDefaultDL
gObjDefaultDrawDL = 0x80038FA8; // type:data  Obj_NewDefaultDL data
gSceneTextBlank = 0x8003D8D0; // type:data size:0x8  " " (initial text of kind-8 scene objects)
gObjListHead = 0x800A6B30; // type:data size:0xB0  Obj sentinel (head)
gObjListTail = 0x800A6BE0; // type:data size:0xB0  Obj sentinel (tail; .prev at 0x800A6BE4)
gDefaultLights = 0x800C1770; // type:data size:0x18  Lights1: ambient +0, light 0 +8
```
(jump tables: 0x8003D880 Obj_SetData, 0x8003D898 Obj_FreeData, 0x8003D8B0 Obj_Attach,
0x8003D8E0 Scene_Build; 0x8003D8D8 = 1.0 double.)

## Structures / globals learnt

* **Obj** (0xB0, game heap): next/prev (global list between the two sentinel Objs at
  0x800A6B30 / 0x800A6BE0), `setup[6]` Gfx at 0x08 (call DL / num-lights / 2 lights /
  prim colour `FA000000 rgba` / end), `finish[4]` Gfx at 0x38, `type` s16 at 0x58
  (0 text, 1 sprite, 2 screen sprite, 3 model, 4 raw DL, 5 viewport), `flags` u16 at
  0x5A, `data` 0x5C, `data2` 0x60 (font of text objects), `update` callback 0x64 +
  arg 0x68, scene node 0x6C, `transform` callback 0x74, pos/rot/pivot/scale f32[3] at
  0x78/0x84/0x90/0x9C.
* **flags**: 1 attach DLs to node, 2 run transform, 4 kill (frees node), 8 dead (frees
  data+object next frame), 0x10 detach (hide), 0x20 object owns data, 0x40/0x80/0x100
  flip X/Y/Z (3D transform only), 0x200/0x400 alternative model set-up lists.
* **Scene nodes** (0xE0 blocks from the pool at D_800B4AB0, made by `func_800116B8(kind)`,
  freed by `func_800117BC`): next/prev at 0/4, kind at 8, MtxF at 0x0C. At 0xD0 a
  union: drawable (kind 2) = dl / setup_dl / finish_dl; group (kind 1) = head/tail
  child sentinels (objects are linked before `parent->tail`); viewport (kind 4) = six
  s16. scene.h currently names 0xD4 `setup_dl` only, so object.h has its own view
  `ObjSceneNode` - the scene owner may want to add the union to SceneNode.
* Group nodes created by `func_80011550`: D_800C316C, D_800C30C0, D_800C0AB4,
  D_800AF234 (+ child groups D_800B061C, D_800B04A0, ...); used as parents in the test.
* Transform callbacks: `Obj_Transform3D` = Translate(pos) -> `func_80008750` (scale) ->
  `func_80008540` (rotate, args rot[1], rot[0], rot[2]) -> optional flip matrix ->
  `func_800085B4` (translate by -pivot) into node->mtx. `Obj_Transform2D` is the same
  with pos (x-160, 120-y, z) and no flips. The constructors store the ORIGINAL callback
  addresses (`func_800193F0` etc.) so the heap matches; a port must map them to the C
  versions (macros `XFORM_*` in object.c).
* **Font**: {count, Sprite **, FontGlyph **}; built from `FontDef` tables (12 bytes:
  rom, s16 x0, x1, y, pad; ends with rom 0). Glyph index = char - 0x20. **Quirk:**
  D_80038C00 is indexed with ids < 32 but only 4 pointers exist before the model DL at
  0x80038C10 (and they are 0 in the ROM, so filled at run time; Font_New and
  Font_NewSharedPalette have no direct callers).
* **TextBuf** (0x1510): two buffers of 32 fixed-point Mtx (0x08) and 80 Gfx (0x1008),
  `cur` at 0x1508 flips on every build. Max 32 characters. Text_Build writes two zero
  Gfx for a space, Text_BuildTlut writes nothing for it (kept). `func_8000A924` =
  MtxF -> fixed-point Mtx (guMtxF2L-like).
* **Model** (0x2C): asset = u32[8] header + sections + u16 command stream, translated
  into an F3D DL (G_VTX / G_TRI1 / texture loads for RGBA16, CI4, CI8, optional
  texture-gen switching when loaded with lighting). See the comment on `Model_Load`.
  `Math_Ctz16` loops forever on 0 (a 0 texture width/height would hang the game).
* **Scene tables** (`Scene_Build` / `Scene_Destroy`, used by all overlays): asset table
  of 12-byte entries {slot, rom, s16 type: 0 model, 2 plain sprite, 4 sprite, 9 screen}
  ended by slot 0 (Scene_Destroy stops at rom 0 instead); object table of 0x24-byte
  entries {slot, parent slot, arg, u16 alpha, s16 x,y,z, f32 sx,sy,sz, u8 visible,
  u8 kind 1..13}. Kind 13 = viewport (fields reinterpreted: z-1, sx-1, sy/sz x8192
  truncated to s16); kinds 0, 9, >13 create nothing but still apply colour/position to
  the object already in the slot.
* `AllocList_*` (0x800177B0-0x80017878): list of (heap, ptr) records with sentinel
  nodes; no callers found in this range.
