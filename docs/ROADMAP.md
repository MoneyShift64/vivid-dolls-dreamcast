# Vivid Dolls → Dreamcast: roadmap

Goal: a native Dreamcast version (KallistiOS) that plays exactly like the arcade game.
The port decodes the original data itself, the way the arcade board did, so nothing
needs converting by hand.

## Phase 1 – foundations ✅ (this package)
- [x] ROM split with splat; rebuilds **byte-identical** (`make` → "OK")
- [x] Memory map, 4 overlays, audio and asset areas identified (`docs/FORMATS.md`)
- [x] 209 libultra functions named (n64sym) + key game functions named
- [x] All asset formats understood: LZSS, Siff, sprites, jpeg64 photos
- [x] Portable C for those formats (`port/`), tested on all 506 assets (`make port-test`)

## Phase 2 – turn the game code into C ✅ (534 / 542 functions, 99.4% by size)
About 540 functions: 425 in `asm/game.s`, plus 8 + 6 + 95 + 8 in the overlays.
libultra (395 functions) is **not** needed – KallistiOS replaces it.

Approach: functionally equivalent C, not byte-matching (we don't have Visco's exact
GCC build). Each function gets a first draft from
[m2c](https://github.com/matt-kempster/m2c), then gets cleaned up, named, and checked
against the assembly.

Done so far (all verified, see docs/DIFFTEST.md):
- [x] `vd_math.c`: atan2, binary angles, planes, lines, rays, circles (25 functions)
- [x] `matrix.c`: multiply, affine multiply, transforms, translate/scale/identity (9)
- [x] `heap.c`: the K&R-style allocator + log2 helpers (10)
- [x] `siff.c`: Siff chunk reader/writer, LZSS asset loader (9)
- [x] `sprite.c`: Sprite_Parse (display-list builder), screen sprites, placement (12)
- [x] `photoview.c`: photo scrolling window, mosaic effect, rect copy (6)
- [x] `libm.c`: exp/log/pow/atan/tan/sin/cos/cbrt, complex helpers, 3x3/4x4 inverse, 4x4 multiply (28)
- [x] `strutil.c`: keyword matchers, CRC16, the game's own `sprintf`, hex dumps (19)
- [x] `geom.c`: integer segment tests, triangle/quad hit tests, mesh height lookup,
      greedy triangulation, radix sorts, look-at (11)
- [x] `rotate.c`: look-at, perspective, ortho, axis rotations, 4x4 inverse (with
      pivoting), rotate-between-vectors, basis change, all the "pre-multiply" transforms (27)
- [x] `collide.c`: mesh vertex batching (16-vertex loads), B-spline helpers, pools and
      ring buffers, random numbers, gu-style perspective/look-at-reflect clones (24)
- [x] `scene.c`: scene-graph node pool, tree update, display-list building, camera,
      lights, command-line flags (19) — node layout in `scene.h`
- [x] `text.c`: 8x8 font text objects rendered into 4-bit textures (13)
- [x] `object.c`: objects/entities: constructors, update loop, model loader, fonts,
      scene tables (70)
- [x] `system.c`: boot, threads, input thread, Rom_Read, Overlay_Load, RCP scheduler,
      task pool, UART debug port (38)
- [x] `audio.c`: audio manager, music and sound effects, oscillators (22)
- [x] `gfx.c`: frame rendering, viewports, video, controllers, coins/DIP switches,
      cabinet I/O, controller pak / EEPROM (41)
- [x] `mainloop.c` / `jpeg64.c`: Main_GameLoop, sound wrappers; the photo decoder and the
      unused encoder (34)
- [x] `ovl0_test.c`: operator test mode (8) · `ovl1_attract.c`: boot logo, title, intros (6)
- [x] `ovl2_a.c` … `ovl2_d.c`: the game itself – board, players, enemies, bosses, items,
      bonus stage, effects, HUD (95)
- [x] `ovl3_select.c`: girl select screen (8)
- [ ] 8 routines stay assembly and need Dreamcast-specific replacements: the cooperative
      task context switch (`Task_Exit`, `Task_Yield`, `Task_SwitchNext`, `Task_RunAll`,
      `Task_SaveContext`), `Tlb_Read`, `Tlb_Write`, and an unused register-spill stub.
      Their behaviour is documented in `docs/notes/system.md`.

Next: Phase 3. The C is written against the N64's memory layout (32-bit pointers in
structs, display lists, big-endian data), so the port layer replaces libultra/RDP calls
and byte-swaps ROM data on load.

## Phase 3 – Dreamcast layer
| N64 / Aleck64 | Dreamcast replacement |
|---|---|
| threads, message queues, timers | KOS threads, or a single main loop |
| PI DMA from cartridge | reading the ROM sections from the disc (or ROM image in RAM) |
| RDP/RSP display lists, sprite quads | PVR: each tile → PAL8 twiddled texture + 256-colour ARGB1555 palette |
| 320×240 / 640×240 frame buffer | PVR at 640×480, scaled |
| jpeg64 photos | `port/vd_jpeg64.c`, output converted to ARGB1555 |
| libultra audio (`alSyn`, banks `B1`, sequences `S1`) | N64 ADPCM decode + MIDI-style sequencer on the AICA (KOS sound streams) |
| EEPROM settings | VMU save |
| JAMMA joystick, coin, start, test/service switches | Dreamcast controller (coins mapped to a button) |

Dreamcast rules for this project:
- Maths in game logic uses **SH4ZAM** (gyrovorbis/sh4zam, v0.10.0+) for speed: e.g.
  `Mtx_Mul` / `Mtx_TransformPoint` map onto SH4ZAM's matrix ops. The verified C
  versions stay as the reference (and as a "bit-exact" build option), since FTRV
  rounds slightly differently from the N64.
  The photo decoder is the exception, if exact output matters more there: SH4ZAM's fast
  matrix ops (FTRV) round differently from the arcade's float maths. The decoder only runs
  when a photo loads, so its speed isn't critical. We'll measure both and decide.
- Standard soft reset: **A + B + X + Y + Start** together resets the game.
- Day-to-day testing in **Flycast**; only occasional real-hardware (GDEMU) runs.

## Phase 4 – polish
- Side-by-side comparison with MAME (`vivdolls`) for timing, sound and visuals
- Disc image (.cdi) build, VMU icon, widescreen option (maybe)
