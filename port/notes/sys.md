# Port area "sys": libultra shim, boot, task switcher, input, Dreamcast build

Owner files: `port/ultra/**`, `port/sys/**` (except `dataload.c/.h`), `port/Makefile.dc`,
`tools/port/make_cdi.sh`, this file.

## Build and test
```
make -f port/Makefile.dc                 # -> build/dc/vividdolls.elf, build/dc/1ST_READ.BIN
tools/port/make_cdi.sh ROMFILE           # -> build/dc/vividdolls.cdi (ROMFILE = .u3 or .z64)
```
`source /opt/toolchains/dc/kos/environ.sh` first is optional (without it every command
runs through environ.sh). The ROM goes on the disc as `VIVIDOLL.ROM`; make_cdi.sh checks
it (full 8 MB SHA-1 if it is a full dump, and the SHA-1 of the used part), converts it to
.z64 order and keeps only the used first 0x23B000 bytes (the rest of the 8 MB is 0xFF).
Disc tool: `mkdcdisc` (KOS toolchain, /opt/toolchains/dc/bin); fallback mkisofs + makeip +
cdi4dc. With dc-tool/dcload the ROM can also be read from `/pc/VIVIDOLL.ROM`.

Checks done without a Dreamcast:
* `sinf`/`cosf` (port/ultra/libc_ultra.c) are libultra's algorithm and constants: compared
  bit for bit with the ROM's own sinf (0x8001C5A0) / cosf (0x8001C450) run in Unicorn on
  20 006 inputs (random, large, NaN/Inf, special values): 0 mismatches.
* The audio bank layout alBnkfNew assumes (libultra ALBank/ALInstrument/ALSound/
  ALWaveTable) was checked against both .ctl files of the ROM (offsets in range, sample
  ranges inside the ROM, shared envelopes exist -> swapped once).
* Everything compiles with sh-elf-gcc 15.2 (-m4-single) and links against the KOS
  library: `build/dc/vividdolls.elf` (text 367 KB, data 6 KB, bss 8.7 MB incl. the 7.9 MB
  blocks) and `build/dc/1ST_READ.BIN` (378 KB). The only game-C warnings with -Wall that
  matter for a new target were checked (just function-pointer casts to Task_Create).
  The game's Overlay_Load is dropped by --gc-sections (ours is used).
* `tools/port/make_cdi.sh` tested with the .u3 chip dump and with baserom.z64: both give a
  4.5 MB `build/dc/vividdolls.cdi` (data track unpadded; `VD_CDI_PAD=1` pads it for CD-R).

## What is where
| File | What |
|---|---|
| `port/ultra/ultra_internal.h` | shim types (VdThread in OSThread's 0x1B0 bytes, VdMesgQueue = libultra's OSMesgQueue layout), constants, internal API, `VD_ALIAS` |
| `port/ultra/os_thread.c` | threads, message queues (block/no-block, jam), events, osSetIntMask, soft-reset kill |
| `port/ultra/os_vi.c` | VI: vblank handler (60 Hz), retrace event every `retraceCount`, current/next frame buffer, osViBlack/SetMode (recorded only) |
| `port/ultra/os_pi.c` | PI DMA = memcpy from the ROM image (0xFF beyond it), message posted at once; cache ops no-ops; osVirtualToPhysical = identity |
| `port/ultra/os_sp.c` | osSpTaskLoad/StartGo: gfx task -> `f3d_run_task()` (PVR scene) + SP done (+ DP done for XBUS tasks); audio task -> SP done at once; osDpSetNextBuffer -> DP done; no yields |
| `port/ultra/os_cont.c` | Maple pads -> N64 controller data and Aleck64 I/O registers, coin pulse, test/service combos, soft-reset combo |
| `port/ultra/os_eeprom.c` | EEPROM 4K (512 bytes) in RAM, saved to VMU file `VIVIDOLL.EEP` by a background thread (with the user's icon); controller pak = not present |
| `tools/port/gen_vmu_icon.py` | build step: `port/assets/vmu_icon.png` (32x32 RGBA) -> `build/dc/gen/vd_vmu_icon.c` (16-colour ARGB4444 palette by k-means, 4 bpp, high nibble = left pixel). Needs only python3 (own PNG decoder; checked against PIL: same pixels, mean colour error 8/255) |
| `port/ultra/al_stub.c` | audio library: real heap/link/cents/alSeqFileNew/alBnkfNew (+ BE->native conversion), silent stub players, osAi* |
| `port/ultra/libc_ultra.c` | sinf/cosf (libultra), sqrtf (fsqrt), fabsf, sprintf, osInitialize, __osGetCurrFaultedThread, Main_ForceLink-only stubs |
| `port/sys/main.c` | KOS entry: video, ROM, vd_dataload, PVR + F3D init, shim init, Main_Boot, soft reset |
| `port/sys/vd_rom.c/.h`, `vd_sha1.c/.h` | ROM file search, byte-order detection/conversion, SHA-1 check, error texts |
| `port/sys/task_sh4.S`, `task_glue.c` | Task_Exit/Yield/SwitchNext/RunAll/SaveContext for the SH4 |
| `port/sys/overlay.c` | Overlay_Load (reload overlay data + clear BSS), func_800E0000 dispatch, Tlb_Read/Write no-ops, SDRAM-test area |
| `port/sys/vd_port_io.h` | VD_IO_REG16 / SDRAM test redirection used by the build-time adaptations |
| `port/sys/vd_sys_sh.ld` | `_D_800AF22C` (gTaskCurrent): needed by task_sh4.S, not in the generated script |

All func_XXXXXXXX names the game C uses for libultra functions are aliases
(`VD_ALIAS`, GCC alias attribute) of the real names, defined next to them.

## Threads and timing (libultra semantics on KOS)
* Each libultra thread is a KOS thread. KOS priority = 20 + (255 - pri) (libultra
  higher = runs first); pri 0 (the idle thread's spin) = PRIO_MAX, which KOS never ages, so
  it only runs when nothing else can. The KOS main thread (prio 10) is above all of them
  and only waits for soft-reset requests. The libultra stack argument is ignored: the shim
  allocates 32 KB (128 KB for the scheduler thread, which runs F3D, and the gfx thread).
* Blocking: a thread on an empty/full queue is put on the queue's wait list (priority
  order, FIFO among equals, as libultra) with interrupts disabled and sleeps on its own
  genwait object. A send/receive that wakes a higher-priority thread hands over at once
  (`thd_pass()`, or `thd_schedule_next()` when the send comes from the vblank interrupt).
* Events: OS_EVENT_SP/DP posted by os_sp.c, OS_EVENT_SI by osContStartReadData, VI
  retrace by the vblank handler (after making the "next" frame buffer current, as the VI
  manager does). Pre-NMI and fault events are never posted.
* KOS ages starving threads (a thread not scheduled for ~128 ms gets a temporary
  priority boost). This can let a long-blocked lower-priority thread run once before a
  higher one; libultra would not. Never seen to matter here, but worth knowing.
* The game's cooperative tasks run on stacks in the task pool. KOS asserts if a thread's
  sp is below its declared stack base, so every shim thread's stack is declared from
  `vd_stack_floor` (= the task pool block) up to its real top.

## Task switcher (task_sh4.S)
Frame (0xC0 bytes, on the task's stack): 0x00 r4-r7, 0x10 r8-r14, 0x2C mach, 0x30 macl,
0x34 fpscr, 0x38 pr, 0x3C resume pc, 0x40 fr12-fr15. Task_Create (game C) writes the
args to 0x00.., Task_Exit to 0x38 and the function to 0x3C, which this layout honours.
A fresh frame (pc != pr) gets its stack arguments moved down by 16 bytes before the
first start (MIPS passes argument 5+ at sp+16, SH4 at sp+0: tasks with 6 arguments
exist in ovl2). Not run yet (no Dreamcast/emulator here).

## Input (os_cont.c)
| Dreamcast | N64 pad (DIP bit 4 off, default) | Cabinet I/O (DIP bit 4 on) |
|---|---|---|
| D-pad / analog stick | d-pad bits / analog stick (-80..80) | joystick U/D/L/R (stick threshold 64) |
| A, B, Y | A, B, Z | buttons 1, 2, 3 |
| Start | Start | P1/P2 start (0x02000004 bits 0/1) |
| X | - | coin 1 (pad 1) / coin 2 (pad 2): 4-frame pulse per press (no coin jam) |
| L+R + Y (pad 1) | - | TEST switch (bit 5) |
| L+R + X (pad 1) | - | SERVICE switch (bit 4) |
| L, R triggers | L, R | - |
| A+B+X+Y+Start (any pad) | soft reset | soft reset |
DIP switches: `vd_dip_switches` (1 = ON), default 0 = all OFF (assumed factory setting;
the game reads `~0x02000000`). The coin counter / lockout writes to 0x0200000A are kept
and ignored.

## Soft reset
Detected in the vblank interrupt (edge: the combo must be released before it can fire
again); the KOS main thread then: lets a running F3D task finish, stops the retrace
event, destroys all libultra threads, flushes the PVR texture cache, re-initialises the
shim, re-runs `vd_dataload()` (fresh data, zeroed BSS/heap/task pool) and calls
`Main_Boot` again (boot logo). The EEPROM RAM copy is kept.

## Audio (until the audio milestone)
No audio task is ever sent: `alAudioFrame` returns an empty command list. The sequence
player reports playing from Play until Stop (music fades complete at once). A started
sound reports AL_PLAYING for about its sample length at 44.1 kHz x pitch (ADPCM: 16
samples / 9 bytes; looping sounds until stopped), so the game's "wait for the end of this
sound" works. alSeqFileNew / alBnkfNew convert the big-endian .seq/.ctl headers to native
order in place (shared envelopes, loops and books are swapped once); the audio area can
reuse that code.

## Game-C changes needed (owner of src/game, please)
1. **gfx.c**: `#define IO_REG16(a) (*(volatile u16 *)(a))` reads 0x02000000+, which is not
   RAM on the Dreamcast. Wanted: under `#ifdef VD_PORT` use
   `#include "vd_port_io.h"` / `#define IO_REG16(a) VD_IO_REG16(a)` (port/sys/vd_port_io.h).
   Until then Makefile.dc compiles a sed-edited copy of gfx.c (build/dc/gen/gfx.c).
2. **ovl0_test.c**: the SDRAM test writes 4 MB at 0x01000000. Wanted: under `VD_PORT`
   `#define SDRAM_TEST (vd_sdram_test_area)` and `#define SDRAM_WORDS VD_SDRAM_TEST_WORDS`
   (also done by sed in Makefile.dc for now).
3. **system.c Overlay_Load**: must not run on the port (clears/DMAs 0x800E0000).
   Wanted: `#ifndef VD_PORT` around it (port/sys/overlay.c provides it). For now the
   Makefile weakens the symbol with objcopy.
None of these change the MIPS build, so the difftests are unaffected.

## Open issues / to check in Flycast
1. **Not run yet** (no Dreamcast emulator on the build machine): the first Flycast boot is
   the next step. Most likely trouble spots: the task switcher (task_sh4.S), the thread
   hand-off (os_thread.c), and the KOS stack-underrun assert if a thread's sp ever leaves
   [vd_stack_floor, stack top].
2. osViBlack is recorded (`vd_vi_black`) but the PVR output is not blanked; the game
   draws black anyway until its first swap, check the boot.
3. EEPROM default content (no VMU file) is all 0x00; check that the game initialises it
   (it should rewrite it when the checksum is bad) and that the VMU file appears.
4. Test mode (ovl0) is untested; its SDRAM check is redirected (see above).
5. PAL-only TVs: video is forced to 60 Hz (NTSC 480i or VGA).
6. If the game ever spins on a volatile waiting for a lower-priority thread, KOS ageing
   is what lets it progress (libultra would hang too, so unlikely).
