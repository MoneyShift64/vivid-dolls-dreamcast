# Vivid Dolls → Dreamcast: port architecture (Phase 3)

Goal: run the verified game C (`src/game/*.c`, 534 functions) natively on the Dreamcast
(SH4, KallistiOS; compile with `-m4-single`, not `-m4-single-only`, so `double` stays 8 bytes and struct layouts match), behaving exactly like the arcade game. The game C is not rewritten;
the port supplies what sits *under* it: libultra, the RSP/RDP, the Aleck64 hardware,
the 8 assembly routines, and the original data. The CDI is built from the player's own
ROM, as on buildmycdi.cloud: **no ROM-derived data is compiled into the program**.

## 1. Big picture

```
 game C (src/game, unchanged apart from endian-neutral fixes)
   │  calls libultra (os*, al*, gu*), reads/writes globals, builds F3D display lists
   ▼
 port/ultra/   libultra shim on KOS: threads, message queues, events, VI, PI, SI,
               controller, EEPROM (RAM only)/PFS, timers, gu* maths (SH4ZAM)
 port/gfx/     F3D (Fast3D, libultra 2.0H) display-list interpreter → PVR
 port/audio/   libultra audio library + software synthesis → AICA stream (later milestone)
 port/sys/     boot, memory blocks, data loader, ROM file, task switcher, input mapping
   ▼
 KallistiOS + SH4ZAM
```

Both MIPS and SH4 are 32-bit, so pointers, `Gfx` words with addresses in them
(`PTR32`), and struct layouts are unchanged. The one big difference is **byte order**:
the N64 is big-endian, the Dreamcast little-endian.

## 2. Memory model

The game refers to its globals and data through fixed symbols (`D_8003xxxx`,
`gFoo = 0x800Cxxxx`, overlay `ovlN_D_800Exxxx`). The port keeps the original layout
inside named blocks, so code that relies on adjacency keeps working:

| Block | Original range | Port | Contents |
|---|---|---|---|
| `main_data` | 0x80038150–0x8003E130 (data + rodata of main) | RAM block, filled at boot from the ROM | tables, display lists, strings, float constants |
| `main_bss` | 0x8003E130–0x800C5400 | zeroed RAM block | all main globals |
| `ovlN_data` | each overlay's data/rodata | separate RAM blocks (overlays are linked at distinct addresses on DC, never swapped in) | overlay tables |
| `ovlN_bss` | each overlay's BSS | zeroed RAM block | overlay globals |
| heap | 0x80173600, 0x52B000 (5.3 MB, as the game sets it up) | RAM block | `gGameHeap` |
| frame buffers / Z | 0x807B2600, 0x807D7E00, 0x80115000 | RAM blocks (CPU-visible copies; the PVR has its own) | |
| task pool | 0x8069E600 | RAM block | |

A generated linker script defines every data/BSS symbol as `block_base + offset`.
`Overlay_Load` becomes a no-op apart from clearing the overlay's BSS, since all four
overlays are resident.

### Filling the data blocks at boot ("the loader")
At start-up the program reads the ROM file from the disc, then for each data block:
1. copies the bytes from the ROM;
2. byte-swaps them according to a **swap map** (which bytes are u16/s16, u32/s32/f32,
   bytes, doubles; f64 swaps as 8 bytes);
3. **relocates** every word that holds a pointer (N64 address → port address).

The swap map and relocation list are generated at build time by
`tools/port/gen_dataload.py` from:
* the splat data disassembly (`asm/data/*.s`): `.word SYMBOL` lines are pointers
  (the relocation list), `.half`/`.byte`/`.float`/`.double` give widths;
* the C types of every extern the game C declares (struct layouts dumped with clang
  for a 32-bit target) where the disassembly has only `.word`.
The generated tables contain **only offsets and types**, never ROM bytes.

ROM assets loaded at run time (`Rom_Read`) stay in ROM byte order; the code that parses
them is made endian-aware (section 4).

## 3. Graphics: F3D → PVR

The game's own scheduler (`Sched_*`, an SDK copy) and `Gfx_RenderFrame` stay as they
are. The port implements the hardware calls under them:
* `osSpTaskLoad`/`osSpTaskStartGo` with a graphics task: run the F3D interpreter on the
  task's display list, submit to the PVR, then post the SP and DP "done" events the
  scheduler waits for. Audio tasks go to the audio synthesiser instead.
* `osViSwapBuffer` / VI retrace: driven by the Dreamcast vblank (60 Hz).

Microcode: libultra 2.0H Fast3D family (G_VTX = 0x04, G_TRI1 = 0xBF, G_ENDDL = 0xB8;
three variants selected by `gGfxMode`, all F3D-encoded). The interpreter
(`port/gfx/f3d.c`) keeps RSP state (matrix stacks, 16-vertex buffer, lights, geometry
mode, segments, texture scale) and RDP state (tiles, TMEM, TLUT, combiner, other modes,
prim/env/fill colours, scissor, colour/Z image), and outputs triangles to a backend:
* `port/gfx/pvr_backend.c`: KOS PVR. Textures are converted from N64 formats
  (CI4/CI8 + RGBA16 TLUT, RGBA16, IA/I) to ARGB1555/ARGB4444 and cached by (address,
  size, format, palette, content hash); 320×240 is scaled ×2 to 640×480. Fill
  rectangles, texture rectangles and the colour combiner modes the game uses are
  mapped onto PVR blend/shading modes.
* `port/gfx/null_backend.c`: records the triangle stream for tests.

Textures the CPU changes (the photo being uncovered) are detected by content hash or
by the game's 8×8 dirty-block map and re-uploaded.

Endianness inside the interpreter: `Gfx`, `Vtx`, `Mtx`, `Lights` that the game builds
are native; static ones in the data blocks were swapped by the loader. Texture/TLUT
memory is read as **native** u16 for 16-bit formats, so 16-bit texture and palette
data coming from ROM assets is swapped once when the asset is parsed (sprite `pal8`,
etc.). 8- and 4-bit texels need no swapping.

## 4. Endianness in the game C

All fixes keep the C correct on big-endian MIPS too, so the existing difftests still
prove it matches the original. Two kinds:
1. **Reading ROM-format data** (Siff chunk headers, sprite `vrtx`/`info`/`pal8`/`dlst`
   operands, model command streams, jpeg64 headers, audio banks): use `BE16()`/`BE32()`
   accessors from `vd_common.h` (no-ops on big-endian builds).
2. **Type punning on native data** (a u32 read over two u16 globals, a byte read of a
   u16, etc.): rewrite the access so it doesn't depend on byte order.
The audit list lives in `port/ENDIAN_AUDIT.md`.

Validation without a Dreamcast: the game C plus the port layer is also compiled for
**little-endian MIPS** (`mipsel`) and run in the Unicorn emulator next to the original
big-endian ROM code, with the same scripted inputs. Display-list words, game state and
call logs are compared frame by frame. Same pointer size and same byte order as the SH4.

## 5. Threads, tasks, timing
* libultra threads → KOS threads with the same priorities (KOS is preemptive; the shim
  makes `osSendMesg`/`osRecvMesg`/events behave like libultra, including priority
  hand-off on wake-up, so the original thread interleaving is kept).
* The cooperative game tasks (`Task_*`, hand-written MIPS in the original) are
  reimplemented in `port/sys/task_sh4.S`: save and restore the SH4 callee-saved
  registers (r8–r15, pr, fr12–fr15, fpscr) in the task's context block. Same
  semantics as documented in `docs/notes/system.md`.
* `Tlb_*` → no-ops (all memory is directly addressed on the DC).
* Frame rate follows the game's `gRetracesPerFrame` at 60 Hz (NTSC/VGA). The video
  mode is forced to 60 Hz.

## 6. Input and cabinet
* Aleck64 JAMMA inputs (`IO_REG16` reads at 0x02000000+) and the N64 pads → Dreamcast
  controllers: stick/D-pad → joystick, A/B → buttons, **Start = start**, a button
  combo for **coin** (default: X on each pad), test/service switches on a hidden combo.
* **Soft reset: A + B + X + Y + Start** held together returns to the boot logo.
* DIP switches: an options screen (or defaults matching the arcade's factory settings).
* EEPROM (512 bytes) kept in RAM only: the game never saves (only test mode's
  system check writes it), so nothing goes to the VMU.

## 7. Audio (second milestone)
The game uses the libultra audio library (`alInit`, `alCSPlayer` sequence player,
`alSndPlayer`, ADPCM banks `B1`, sequences `S1`). The port compiles a C version of the
libultra audio library plus an audio-microcode HLE that mixes in software on the SH4
(ADPCM decode, resample, envelopes, reverb), and streams the result to the AICA with
`snd_stream`. Until then the al* calls are stubs and the game runs silent.

## 8. Build and disc
* `port/Makefile.dc`: compiles `src/game/*.c` + `port/**` with `kos-cc`, links with
  the generated linker script, makes `1ST_READ.BIN`.
* Memory budget: the blocks need about 7.9 MB, so only the used first 2.3 MB of the 8 MB
  ROM is kept in RAM (`Rom_Read` beyond that returns 0xFF padding).
* The disc contains `1ST_READ.BIN` and the player's ROM file, named `VIVIDOLL.ROM`.
  The ROM is checked by SHA-1 at boot; a wrong file gives a clear message on screen.
* `tools/port/make_cdi.sh ROM` builds the .cdi (the same steps buildmycdi.cloud can
  run in the browser).

## 9. Milestones
1. Boots in Flycast: boot logo → title screen (with the photo decoded by the game's own
   decoder) → girl select, silent. Coin + start work.
2. Full game playable, silent; soft reset.
3. Audio.
4. Polish: test mode, options, real-hardware check on GDEMU.
