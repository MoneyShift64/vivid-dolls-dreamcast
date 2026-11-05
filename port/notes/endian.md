# Endian area: notes

Owner of: endian-related edits in `src/game/*.c|h`, `port/ENDIAN_AUDIT.md` (the full finding
list), this file.

## What was done
* `src/game/vd_common.h`: `VD_BIG_ENDIAN` (from `__BYTE_ORDER__`, `#error` if unknown),
  `BE16/BE16S/BE32/BEF32` (read big-endian through a byte pointer), `WBE16/WBE32` (write),
  `SWAP16/SWAP32`, `BE16_TO_NATIVE/BE32_TO_NATIVE`, `SwapArray16` (LE builds only).
  On big-endian builds the accessors are the plain loads/stores the code used before.
* Fixed (details per finding in ENDIAN_AUDIT.md):
  - siff.c: chunk reader uses BE32; writer writes BE32 (ROM format on both byte orders).
  - sprite.c: all asset fields via BE16; LE builds swap the copied TLUT (and RGBA16 tiles)
    to native once after parsing; screen-sprite `rect_cmds` converted while copying.
  - text.c: `Text_ExpandGlyph` stores the 8 CI4 texels with WBE32 (byte order of TMEM).
  - jpeg64.c: header words via BE32/BE16.
  - object.c: `Model_Load` header and command stream via BE32/BE16; LE builds convert vertices,
    texture records and RGBA16 images in place (no model asset in the ROM - untested on data).
  - audio.c: the raw S1 `seqCount` read before alSeqFileNew via BE16S.
  - system.c: `Mtx_FromFixed` no longer reads the s32 Mtx through a u16 view.
  - gfx.c / object.c (typing only, same code on BE): the static screen-quad vertices are
    declared `SpriteVtx[4]` (were `u8[4][16]`), and `D_80038F20` is declared
    `{ SpriteGfx dl[1]; SpriteVtx vtx[4]; }` so the boot loader swaps those vertices per field.
* Everything else was checked and is byte-order independent (ENDIAN_AUDIT.md section B).
  No bitfields exist.

## How to test
Big-endian (must stay identical to the original):
```
(from the project folder, with MIPS binutils on PATH)
python3 tools/difftest/test_siff_writer.py; python3 tools/difftest/test_sprite.py
python3 tools/difftest/test_screen_sprite.py; python3 tools/difftest/test_text.py
python3 tools/difftest/test_jpeg64.py; python3 tools/difftest/test_audio.py
python3 tools/difftest/test_object.py; python3 tools/difftest/test_system.py
python3 tools/difftest/test_gfx.py; python3 tools/difftest/test_funcs.py
```
All PASS after the changes (also run: scene, mainloop, ovl0, ovl1, ovl3, ovl2_a..d).

Syntax check on both byte orders (all 26 files, warning-free). The bare command from the
brief fails without a MIPS sysroot (`stdint.h` -> `bits/libc-header-start.h`), so add the
difftest's freestanding flags:
```
clang --target=mipsel-linux-gnu -m32 -O2 -fsyntax-only -ffreestanding -nostdlibinc -isystem tools/difftest/include -Isrc/game -Wall src/game/X.c
clang --target=mips-linux-gnu   -m32 -O2 -fsyntax-only -ffreestanding -nostdlibinc -isystem tools/difftest/include -Isrc/game -Wall src/game/X.c
```
(script: `(scratch scripts, not in the repo) chk.sh`).

Little-endian checks (scratch tools, outside the project, in `(scratch scripts, not in the repo) `):
the game C is built for **mipsel** and run in a little-endian Unicorn next to the original
big-endian code; results are compared after swapping each field by its type.
* `le_sprite.py`: all 472 sprites (Sprite_New / NewPlain / NewSharedPalette in rotation, plus
  Sprite_NewScreen): Sprite struct, TLUT, tiles, vertices, display list, rect commands -
  **0 differences** (a mutation without the TLUT swap gives 315 differences).
* `le_text.py`: 200 random strings through Text_RenderString (bitmap byte-identical) and 100
  Siff writer sessions (output byte-identical, read back with SiffIter_Enter) - all identical
  (without the WBE32 in Text_ExpandGlyph: 1/200).
Run with `python3 (scratch scripts, not in the repo) le_sprite.py`.
`lebuild.py` there is a small reusable mipsel build + LE emulator (`build_le`, `LEEmu`,
`swap_regions`) if another area wants to do the same; it could move into tools/difftest.
jpeg64 was not LE-checked on ROM data on purpose (that would decode photos); the change is
three header reads.

## Requests to other areas
* **dataload**: ENDIAN_AUDIT.md section D. Main points: the static Vtx at
  D_800386D0/10/50/90 and D_80038F20+8 (now C-typed as Vtx, please make sure the C type wins
  over splat's `.byte`/`.word`); D_8003B110 should be swapped as 20 u32 words (it is bcopy'd
  into the u32[20] D_800A6850 and used as words - your notes currently keep it as BE bytes);
  the `u8[]` animation blobs in ovl2 and their real consumer types.
* **gfx (F3D interpreter)**: ENDIAN_AUDIT.md section E (Vtx/Mtx/Vp/Lights/texel conventions)
  matches gfx.md; CI4 bitmaps (text, sprites) are bytes, high nibble first.
* **audio**: the port's `alSeqFileNew` / `alBnkfNew` must convert the S1 sequence-file
  header and both `.ctl` bank files to native order in place (all structure fields, including
  ADPCM books/loops), as they relocate offsets; `alCSeqNew` reads the compact sequence (BE).
  The game C reads `seqArray[].offset/len`, `bankArray`, `instArray`, `soundCount`,
  `soundArray` natively after those calls.
* **libultra/input shim**: `PadData` and `IO_REG16` values native; OSViMode word 0 holds
  `type` in its top byte when read as a word.

## Open issues
* Model_Load's LE conversion assumes [off1,off2) is an array of 16-byte Vtx and treats
  section2 / aux as bytes; there are no model assets to confirm this.
* Not endian, noticed on the way: `Main_GfxThread` retires frames on a type-2 message
  that arrives through `GfxTask.msg -> msgbuf`, but nothing in the game C writes a type into
  `msgbuf` (BSS, 0). Worth a look by whoever owns system/gfx (maybe the original relies on
  something not decompiled, or type-2 never happens and frames retire another way).
* Difftests write `*.c.mips.o` next to the sources in `src/game/` (draft_mips.compile_c);
  harmless build products.
* The `D_800B0430` s32/u8[] pair is endian-safe only because the test is `!= 0`
  (ENDIAN_AUDIT.md B).
