# dataload: memory blocks and the boot-time data loader

Owner files: `tools/port/gen_dataload.py`, `tools/port/test_dataload.py`, `port/gen/*`,
`port/sys/dataload.c`, `port/sys/dataload.h`, this file.

## How to run
Regenerate the tables (about 10 s; needs clang and llvm-dwarfdump; `asm/` from `make setup`):
```
python3 tools/port/gen_dataload.py
```
Check them (about 10 s; needs `baserom.z64`, host `cc`, and for the link check the MIPS
MIPS binutils on PATH):
```
python3 tools/port/test_dataload.py
```
Current result: ALL PASSED (coverage, relocations, LE simulation on the real ROM, 12 209
C-typed leaves, the ENDIAN_AUDIT.md section D cases, 583 symbols placed / 0 unplaced, mipsel
link of all game C, C loader == Python model). Check [7] fails if any section-D case regresses
(verified by removing two overrides: 35 failures).

Rerun the generator whenever `src/game/*.c`/`*.h`, `symbol_addrs.txt` or the splat output
change (new externs, changed types).

## What it builds
### Blocks (`port/gen/vd_blocks.h`)
| block | N64 range | size | filled |
|---|---|---|---|
| main_data | 0x80038150-0x8003E130 | 0x5FE0 | ROM 0x38D50 (.data + .rodata) |
| main_bss | 0x8003E130-0x800C5400 | 0x872D0 | zero |
| ovl0_data | 0x800E4AB0-0x800E4DA0 | 0x2F0 | ROM 0x437E0 (ovl0 has no BSS: ovl0_bss size 0) |
| ovl1_data / bss | 0x800E0FC0-0x800E17A0 / -0x800E1900 | 0x7E0 / 0x160 | ROM 0x44A90 / zero |
| ovl2_data / bss | 0x800F6390-0x800FB050 / -0x800FD550 | 0x4CC0 / 0x2500 | ROM 0x5B600 / zero |
| ovl3_data / bss | 0x800E0B40-0x800E0F30 / -0x800E1000 | 0x3F0 / 0xD0 | ROM 0x60E00 / zero |
| ucode | 0x80033FA0-0x80038150 | 0x41B0 | zero: RSP microcode text, only its addresses/sizes are used |
| zbuf | 0x80115000-0x8013A800 | 0x25800 | zero |
| rcpbuf | 0x8013A800-0x80173600 | 0x38E00 | zero: yield buffer, DRAM stack, RDP FIFO (D_8013A800..D_80173510) |
| heap | 0x80173600-0x8069E600 | 0x52B000 | zero (gGameHeap; `Heap_Init(&gGameHeap, 0x52B000)` - not 2 MB as ARCHITECTURE.md says) |
| taskpool | 0x8069E600-0x807B2600 | 0x114000 | zero |
| fb0 / fb1 | 0x807B2600 / 0x807D7E00 | 0x25800 each | zero |

Main data really starts at 0x80038150 (ARCHITECTURE.md's table says 0x80038D50, which is
the ROM offset). Total static RAM of the blocks: about 7.9 MB (5.3 MB of it is the heap).

### Linker scripts
* `port/gen/vd_blocks.ld`: `SYMBOL = vd_blk_<block> + offset;` for every data/BSS symbol the
  game C references (583) plus gGameHeap, gFrameBuffer0/1, gZBuffer, gTaskPoolMem, and the
  address constant `D_611F0 = 0x611F0` (ROM offset of the assets, used by `ASSET_ROM`).
  No symbol prefix: for mipsel test builds / host.
* `port/gen/vd_blocks_sh.ld`: the same with the leading `_` that sh-elf GCC puts on C
  symbols (checked: KOS startup.S calls `_arch_main`).
* `port/gen/vd_func_aliases(_sh).ld`: 97 call names the game C uses for functions it defines
  under another name (`func_80003C4C = RadixSort_Records`, `ovl2_func_800E2910 = Board_Init` ...).
* Use them as **implicit linker scripts**: just add the file to the link command line
  (`kos-cc ... port/gen/vd_blocks_sh.ld port/gen/vd_func_aliases_sh.ld`), do not pass `-T`
  (that would replace the KOS script). Tested with sh-elf-ld.

### Loader tables (`port/gen/vd_dataload_tables.c`)
Per data block: ROM offset, size, swap runs `{offset, count, width, 0}` (width 1/2/4/8,
runs cover every byte exactly once), relocations `{offset, VD_RT(target block, target offset)}`.
2253 runs and 1703 relocations, about 31 KB of SH4 rodata. No ROM bytes.

`vd_dataload_types.json` (leaf layout of every C-typed symbol, placements) and
`dataload_report.txt` (human review list) are by-products for the test and for review.

### Loader (`port/sys/dataload.c/.h`)
* `int vd_dataload(const uint8_t *rom, size_t rom_size, uint32_t *bad_offset)`: zero all
  non-data blocks, then for each data block copy from the ROM, check that every word to be
  relocated holds the expected N64 address (wrong/modified ROM -> `VD_DL_ERR_RELOC`), swap
  (only on little-endian builds), write native pointers. Re-runnable (soft reset).
* `vd_dataload_apply(...)`: the core with caller-given block addresses (host tests).
* `vd_block_clear(VD_BLK_OVLn_BSS)` for Overlay_Load; `vd_n64_to_port(addr, ovl)` and
  `vd_port_to_n64(p)` for debugging and for any code that gets an N64 address at run time.
* C99, only `<string.h>`/`<stdint.h>`; compiles warning-free (`-Wall -Wextra -pedantic`)
  for i386, mipsel, mips (BE: no swap), armv7, x86_64. `vd_dataload()` returns
  `VD_DL_ERR_PTR64` on 64-bit hosts. Include paths: `port/sys`, `port/gen`.

## How widths are decided (priority, first wins)
1. `.word SYMBOL [+ n]` in the splat data whose value is inside a block: **relocation** (32-bit).
   `.word` to code (`Lmain_...`/`Lovl2_...` jump tables, `func_...`): 32-bit, **not relocated**
   (391; the game C compiles its own switches and never reads the ROM jump tables; the
   libultra exception table D_8003B670 is not used by the port). `.word` symbols that are
   really numbers (D_80000000 in RSP data, D_80001, D_611F0): plain values.
2. **C types** of every extern the game C references: each src/game/*.c is compiled for
   32-bit MIPS with `-g` together with a probe `__typeof__(X) *__vdprobe_X;` per referenced
   symbol, and the DWARF is walked (structs recursively, arrays, unions = first member,
   bitfields = storage unit, enums, pointers = 32-bit). If files disagree, the most specific
   declaration wins (non-byte over `u8[]`, larger size); all disagreements are in the report.
   Incomplete arrays `T x[]` extend up to: symbol_addrs `size:`, the next C symbol, a splat
   label that is a relocation target, a .data/.rodata section start, or a splat label whose
   explicit directives disagree with the element layout. Structs with a variable trailing
   array (Jpeg64Huff.huffval[1]) are extended to the symbol_addrs size.
3. symbol_addrs `type:` for symbols the C does not declare.
4. **Propagation**: a relocation stored in a C pointer field `T *` types its target as T
   (over the target's splat label) - e.g. `EffFrame *[]` tables give the EffFrame lists.
5. Explicit splat directives `.byte/.short/.float/.double/.asciz`.
   **TYPE_OVERRIDES** (in gen_dataload.py, from port/ENDIAN_AUDIT.md section D) give the real
   consumer type of data the game C only declares as `u8 x[]`; they beat every declaration:
   D_8003B110 = `u32[20]`; ovl2_D_800F8E1C/8E4C/922C/9318/93B4/9488 = `EffFrame *[]`;
   ovl2_D_800F95D4/9438/9874 = `AnimFrame *[]`; ovl2_D_800F97B0/96DC = `EnemyAnims`.
   Add a line there when the audit finds another one (or retype the extern in the game C).
6. C byte blobs (`u8 x[]`, `char x[]`: RSP microcode data, strings, opaque task data),
   limited to their splat label; relocations inside them still apply.
7. Remaining `.word`: 32-bit, **guessed** - listed in `dataload_report.txt`.
8. Alignment holes: bytes.

Guessed ranges now: main_data 1392 bytes (all libultra/libm private data - VI mode tables,
thread/PI globals, libm constants - plus zero padding of jump tables); ovl1 4 bytes and
ovl2 676 bytes, all of them zero words (so the width cannot matter). The ovl2 animation lists
(EffFrame/AnimFrame `{void **, s32}`) are now typed from the C structs through the overrides
above and pointer propagation (65 tables).

### Type decisions to know about
* **Gfx**: two u32 words (w0, w1) - static display lists read natively by the F3D interpreter.
* **Vtx** (SpriteVtx etc.): per field - s16 x,y,z, u16 flag, s16 s,t, **u8 r,g,b,a**. The C
  type beats splat's `.byte`/`.short`/`.word` for D_800386D0/10/50/90 (`SpriteVtx[4]`),
  D_80038AD0 (gTextQuadVtx) and D_80038F20 (gObjDefaultDL = `{Gfx; SpriteVtx[4]}`: +0..7 two
  u32, +8.. four Vtx) - checked by the test.
* **Mtx**: swapped as **32-bit words**. The game writes Mtx with 32-bit stores
  (`Mtx_ToFixed` packs `(int_hi << 16) | int_lo` into s32 words) and libultra's Mtx is
  `long m[4][4]`, so the F3D interpreter must read a Mtx as 16 native u32 words and take the
  halves with shifts (`w >> 16`, `w & 0xFFFF`), never as an s16 array. (None of the C-typed data
  in the blocks is a Mtx today, so this mainly fixes the convention.)
* **f64**: swapped as a full 8-byte reversal (IEEE little-endian double). Caution: if the DC
  build uses `-m4-single-only`, `double` is 32-bit and every struct with a double changes
  layout - the game C has 4 `f64` externs in the data blocks and libm uses doubles. Use `-m4-single`
  (64-bit doubles) for the game, or audit those.
* **OSViMode tables**: osViModeTable and D_8003B110 (the initial mode, bcopy'd into the u32[20]
  D_800A6850) are 32-bit words, so they read correctly as words on LE. Word 0 holds `type` in
  its top byte: a VI shim that wants it reads `w[0] >> 24`, not a byte at offset 0.
* **RSP data** (D_8003B790 audio, D_8003BA50/C250/CA50 gfx ucode data): bytes, stay big-endian.

## Open issues / requests to other areas
* **Overlay_Load** (system area): all overlays are resident; it must only clear the BSS
  (`vd_block_clear(VD_BLK_OVL1_BSS)` etc.) and must not use gOverlayTable's vram/end fields
  (code addresses / N64 overlay ends, deliberately not relocated) or DMA anything.
* **func_800E0000** (the overlay entry the main loop calls) cannot be aliased - 4 overlays share
  that address. The port must call the entry of the overlay last "loaded" (Attract/Game_Run/...).
* **osVirtualToPhysical** (used e.g. for the Z buffer in a Gfx word) should return the port
  pointer unchanged, so the F3D interpreter sees native addresses.
* `D_80038C00` is declared `FontDef *[32]` but only the first 4 slots are separate data: the
  rest overlaps the display lists D_80038C10.. (same 32-bit width, so loading is unaffected).
  If the game really writes slots >= 4 it would overwrite those lists - worth a look in object.c.
* `D_800A61A0` (audio) has a union whose members have different widths (BSS, no effect on loading).
* **RAM budget**: the ROM is 8 MB but its content ends at 0x23AFE0 (the rest is padding).
  Keep only the first ~2.3 MB in RAM (or stream assets from the disc), otherwise ROM 8 MB +
  blocks 7.9 MB do not fit in the DC's 16 MB. The heap (5.3 MB) could be made smaller only by
  changing the Heap_Init size in the game C.
* Block storage is plain C globals (.bss), so the KOS startup zeroes them once and
  vd_dataload zeroes them again (~8 MB memset, a few tens of ms) - harmless; needed for soft reset.
* The 100 functions left undefined after linking the game C (libultra, the 8 asm routines,
  bcopy/bzero, sinf/cosf/sqrtf, memcpy/memset) are listed at the end of `dataload_report.txt`;
  the port layer provides them.
