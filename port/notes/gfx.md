# Port area "gfx": F3D interpreter (RSP + RDP replacement)

Files (all under `port/gfx/`):

| File | What |
|---|---|
| `f3d.h`, `f3d.c` | portable C99 interpreter: RSP (matrices, vertices, lighting, texgen, clipping, culling, viewport) + RDP (tiles, 4 KB TMEM, TLUT, loads, combiner, other modes, rects) |
| `f3d_backend.h` | backend interface: frame begin/end, colour image, depth clear, fill rect, texture find/upload (by content key), screen-space triangles + state |
| `null_backend.[ch]` | recording backend for host tests |
| `pvr_backend.[ch]` | KOS PVR backend (**not compiled yet**: `sh-elf-gcc` is not installed; syntax-checked against the KOS headers with host gcc, no errors in this file) |
| `tests/test_f3d.c` | host unit tests (134 checks) |
| `tests/test_rom_dls.c` | runs the 32 static display lists from the player's ROM through the interpreter (command coverage) |
| `tools/f3d_disasm.py` | prints a symbolic listing of the static DLs from the ROM (stdout only) |

## How to test

```
make -C port/gfx/tests run        # unit tests + ROM static-DL smoke test (ROM=../../../baserom.z64)
make -C port/gfx/tests run-shz    # same unit tests through the SH4ZAM hooks (SH4ZAM's portable XMTRX emulation)
python3 -I port/gfx/tools/f3d_disasm.py baserom.z64 [symbol ...]   # listing of the static DLs
```

## Inventory: what the game sends

Sources read: `gfx.c` (Gfx_RenderFrame, Gfx_SetFullViewport, Gfx_SetViewport), `scene.c`
(Scene_BuildGfx), `sprite.c` (Sprite_Parse, Sprite_DrawScreen), `text.c`, `object.c`
(text glyph lists, Model_Load, Obj_* set-up/finish slots, Obj_SetColor, lights),
`photoview.c`, overlays (no display-list code of their own; ovl0 only makes scene nodes),
and the static lists in the main data segment (decoded from the ROM with `f3d_disasm.py`).
No other data segment (overlays) contains display lists.

### Microcode
libultra 2.0H Fast3D encoding in all variants (G_VTX 0x04 with `n-1`/`v0` nibbles and byte
length, G_TRI1 0xBF with indices ×10, G_ENDDL 0xB8, RDPHALF_1 0xB4 / RDPHALF_2 0xB3).
gGfxMode (0x800B0504) is only ever written by Video_Init (= 0), so the game always uses
microcode 0 (text 0x80037210, data 0x8003CA50, FIFO output). Its data segment has no
SGI version string (modes 2 and 3 are SGI "RSP SW Version 2.0H" builds); it starts with a
default 320×240 viewport. **Y axis:** the game stores a negative `vscale[1]` in modes 0/1
and a positive one in modes 2/3, so microcode 0/1 maps screen_y = vtrans + y·vscale
(no negation) while 2/3 negate as standard F3D. `f3d_set_ucode(gGfxMode)` selects this;
the result is an upright picture in every mode.

### RSP commands used
| Opcode | Where | Fields |
|---|---|---|
| G_MTX 0x01 | scene (0x01020040 MV load), text glyphs (0x01000040 MV mul), frame (0x01030040 projection load) | no push/pop in practice (implemented anyway) |
| G_MOVEMEM 0x03 | VIEWPORT (0x80), LOOKATX/Y (0x84/0x82), L0 (0x86), L1 (0x88 = ambient with 1 light) | 16 bytes |
| G_VTX 0x04 | sprites (≤16), models, text quad (4), screen quads (4) | |
| G_DL 0x06 | call only (no branch in game data) | |
| G_TRI1 0xBF | sprites, models, quads | flag always 0 |
| G_MOVEWORD 0xBC | SEGMENT 0 = 0, NUMLIGHT (0x80000040 = 1 light), PERSPNORM | |
| G_TEXTURE 0xBB | on/off, tile 0, scale 0x8000 (texels = tc/64), 0x07C0 with texgen | |
| G_SETOTHERMODE_H 0xBA | PIPELINE, CYCLETYPE (1-cycle / FILL), TEXTLUT (NONE/RGBA16), TEXTFILT (POINT/BILERP), TEXTPERSP, TEXTCONV, TEXTDETAIL, TEXTLOD, RGB/ALPHA dither | |
| G_SETOTHERMODE_L 0xB9 | ALPHACOMPARE = threshold (once, D_80038648), RENDERMODE | |
| G_SETGEOMETRYMODE / CLEAR 0xB7/0xB6 | ZBUFFER, SHADE, SHADING_SMOOTH, CULL_BACK, CULL_FRONT (mirrored models), FOG (model "alt"), LIGHTING (models **and** object sprites), TEXTURE_GEN (Model_Load when bits 8-9 of the vertex command = 2) | |
| G_ENDDL, G_SPNOOP | | |
| not used | G_POPMTX, G_CULLDL, G_LINE3D, G_MW_MATRIX/POINTS/FOG/LIGHTCOL, G_TEXRECTFLIP | implemented except LINE3D/MATRIX/POINTS |

### RDP commands used
| Opcode | Use |
|---|---|
| SETCIMG / SETZIMG | colour image = task frame buffer (RGBA16, width 320); Z image; cimg switched to the Z buffer for the Z clear |
| FILLRECT + SETFILLCOLOR (FILL mode) | Z clear (0xFFFC), optional frame clear (RGBA5551 ×2) |
| SETSCISSOR | full screen **(w-1)×(h-1) = 319×239**, per-viewport rects (Gfx_SetViewport) |
| SETTIMG | RGBA16 (palettes, RGBA16 textures, photo with width = image width), CI 16b (CI4/CI8 loaded as 16-bit) |
| SETTILE | load tile 7 (tmem 0, line 0 or the photo's 81), render tile 0 (line from width, clamp `cms=cmt=2` for sprites, mask/wrap or clamp-by-size for models), TLUT tiles 2 (sprites/fonts) and 7 (text), tmem 0x100 |
| LOADBLOCK | every sprite/text/model texture, dxt computed by the game (odd lines swapped in TMEM) |
| LOADTILE | photo strips only: 321×7 texels RGBA16, line 81 → **7 rows × 648 bytes > 4 KB, row 6 wraps over row 0 (texels 0..219)**. Emulated exactly (TMEM address wrap). |
| LOADTLUT | 256 entries (CI8) or 16 (CI4) from tmem 0x100 |
| SETTILESIZE | per tile; sprites in mode 1 change uls/ult per triangle |
| TEXRECT + RDPHALF_1/2 | screen sprites (Sprite_DrawScreen), text rectangles, photo strips; 1-cycle mode, dsdx/dtdy from scale |
| SETPRIMCOLOR | object colour (combiner PRIM), model default 0xFF000000 |
| SETENV/FOG/BLENDCOLOR, SETPRIMDEPTH, SETKEY*, SETCONVERT | init only (blend alpha = 8 = alpha-compare threshold) |
| SYNCs, FULLSYNC | ignored |

### Colour combiners (both cycles always equal)
1. `0,0,0,SHADE / 0,0,0,SHADE` – screen quads (fades), RDP init
2. `0,0,0,TEXEL0 / ...TEXEL0` – sprites, screen sprites, photo, text rectangles
3. `TEXEL0*SHADE / TEXEL0` – text quads, gModelSetupDL
4. `TEXEL0*SHADE+PRIM / TEXEL0*PRIM` – object sprites and models (gObj*SetupDL)
5. `0,0,0,PRIM / PRIM` – default object (gObjDefaultSetupDL)

All are reduced exactly to `texel × mod + off` per vertex (mod/off from shade, prim,
env) – PVR MODULATE(ALPHA) + offset (specular) colour.

### Render modes
| Mode | Lists | Backend mapping |
|---|---|---|
| OPA_SURF (blender bypassed) + alpha compare 8 | sprites, photo, screen sprites | cutout: alpha blend on 1-bit alpha, vertex alpha forced 255 |
| XLU_SURF (IM_RD, FORCE_BL, IN·A + MEM·(1-A)) | screen quads, default object, obj sprites "2" | alpha blend |
| AA_EN \| CVG_X_ALPHA \| ALPHA_CVG_SEL, IN·0 + IN·1 | text quads/rects | alpha-test (cutout) |
| AA_ZB_OPA_SURF (Z_CMP, Z_UPD) | models | opaque, depth test+write |
| FOG_SHADE_A / AA_ZB_OPA_SURF2 + G_FOG | model "alt" + flip | fog folded into vertex colours (fog factor from G_MW_FOG; the game never sets it, so factor 0 = no fog) |
| AA_ZB_XLU_SURF (ZMODE_XLU, Z_CMP) | model "alt2" | alpha blend, depth test, no write |

### Texture formats seen
CI4 (+16-colour TLUT, palette 0), CI8 (+256 TLUT), RGBA16, all through LOADBLOCK; RGBA16
LOADTILE for the photo. TLUTs are RGBA16. No IA/I texture is used by the game (the
decoder supports I4/I8/IA4/IA8/IA16 and RGBA32 anyway; tested).

## Interpreter design
* Address hook `void *(*addr)(u32)`: addresses < 0x10000000 are segmented/physical
  (`seg[(a>>24)&15] + (a & 0xFFFFFF)`), higher ones (DC pointers 0x8Cxxxxxx, N64 K0) go to
  the hook unchanged.
* Byte order: Gfx words, Vtx fields (s16/u8), Mtx as **u32 words** (Mtx_ToFixed writes whole
  words: endian-neutral), Vp s16, lights bytes; TMEM holds the source bytes as they are in
  memory; 16-bit texels and TLUT entries are read as host u16 (ARCHITECTURE.md §3).
* TMEM: 4096 bytes, LOADBLOCK with the dxt line counter (odd lines: 32-bit halves swapped),
  LOADTILE with odd-row swap, address wrap at 4 KB, TLUT as 256 logical entries
  (index = tile tmem − 0x100). Texels fetched exactly like the RDP (row × line, odd-row
  XOR 4, CI4 nibble order, TLUT applied when TEXTLUT ≠ NONE – also to 16-bit textures with
  index = texel >> 8, as on hardware).
* Textures are keyed by a 64-bit FNV hash of format/size/line + the TMEM bytes the tile
  covers + the TLUT entries used; the backend is asked `tex_find(key)` and only on a miss is
  the tile decoded to ARGB8888 and handed to `tex_upload`. CPU-modified textures (the photo)
  therefore re-upload automatically when their content changes.
* Clipping in homogeneous clip space against near, far and the four scissor edges (exact
  attribute interpolation); culling on the projected polygon (front = CCW with y up).
* Lighting as F3D: light/lookat directions transformed into model space by the modelview,
  normalised; ambient = light[numlights]. Texgen: s = (n·lookat_x + 1)·512·scale.
* Maths hooks: `f3d_mtx_mul` / `F3D_XFORM_LOAD` + `f3d_xform` (f3d.h). With
  `-DF3D_USE_SH4ZAM` they map to `shz_xmtrx_load_apply_store_unaligned_4x4` and
  `shz_xmtrx_transform_vec4` (FTRV); the MVP is loaded into XMTRX once per G_VTX.
  `f3d_mtx_selftest()` compares hooked and C results – call it once at boot on the DC.
  The SH4ZAM path passes all unit tests with SH4ZAM's software XMTRX (`make run-shz`).

## PVR backend
* Everything goes into the **TR list with autosort disabled** (`pvr_backend_fill_params`
  sets `autosort_disabled = 1`, only the TR list enabled) so polygons are rasterised in N64
  submission order; opaque = ONE/ZERO. Depth = 1/w, GEQUAL, write per Z_UPD.
* Textures: RGB565 / ARGB1555 / ARGB4444 by alpha content, padded to power of two
  (edge replication for clamp, tiling for small wrapped tiles), twiddled with
  `pvr_txr_load_ex`, stable pool + hash buckets, evicted when unused for 32 frames (or
  ≥ 2 frames under VRAM pressure – never textures the PVR may still be rendering).
* ×2 scale from the colour-image width (320 → 640).
* Full-screen fill also sets the PVR background colour; depth clear is implicit per scene.

## Integration (other areas – not done here)
`port/ultra` should, in `osSpTaskStartGo` for an M_GFXTASK: `f3d_set_ucode(gGfxMode)`
(once), `f3d_run_task((u32)task->data_ptr)`, then post the SP and DP done events.
At boot: `pvr_init_params_t p; pvr_backend_fill_params(&p); pvr_init(&p);
pvr_backend_init(&be); f3d_init(&be, identity_hook); f3d_mtx_selftest();`.
Build with `-DF3D_USE_SH4ZAM` and the SH4ZAM include path. `osViSwapBuffer` needs no
copy: the PVR flips at the end of each scene.

## Open issues / to check in Flycast
1. pvr_backend.c is uncompiled (no `sh-elf-gcc` yet). Build it once the toolchain is ready.
2. The game's scissor is 319×239 (`(w-1)*4`), so the right column / bottom row are never
   drawn – faithful, but shows as a 2-pixel black edge at 640×480. Could widen by one pixel
   in the backend if it looks wrong.
3. Microcode-0 Y convention is inferred from the viewport code (picture must be upright);
   confirm with the first boot-logo frame.
4. Alpha test is emulated with blending: correct for 1-bit alpha, but ARGB4444 textures
   with alpha ≥ 8 on an alpha-tested opaque mode are blended instead of drawn solid, and
   z-buffered cutout texels still write depth.
5. Model CI textures: gObjModelSetupDL/gModelSetupDL set TEXTLUT = NONE and Model_Load
   loads no TLUT, so a CI model texture renders as intensity on hardware too (emulated
   that way). Worth checking whether any model actually uses CI.
6. Sprites without a pal chunk are RGBA16 but drawn with TEXTLUT = RGBA16 (index = texel
   >> 8 on the RDP – emulated). Check whether any Sp3d asset is RGBA16.
7. Fog: the game never sends G_MW_FOG; factor defaults to 0 (no fog). If microcode 0's DMEM
   has non-zero fog defaults, "alt" models would be fogged – compare with a capture.
8. Photo strips: TEXRECT starts at 0.25-pixel offsets (xl = 1, yl = 1 in 10.2); the backend
   draws the exact quad, RDP sub-scanline coverage may differ by one row. Check the photo
   seams by automated comparison only (photos must not be viewed).
9. Performance: the combiner is re-reduced per vertex and state is rebuilt per triangle;
   cache per SETCOMBINE / other-mode change if the profile shows it.
