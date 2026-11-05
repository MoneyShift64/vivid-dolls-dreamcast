# Area "boot": first Dreamcast boot, headless emulator harness, freeze fixes

Owner files for this work: `port/ultra/**`, `port/sys/**`, `port/gfx/**` (fixes),
`port/Makefile.dc`, `tools/port/**`, this file.

## Status (2026-11-01)
In headless Flycast (HLE BIOS, VGA) the release build `build/dc/vividdolls.cdi` now goes
**boot logo → title screen (photo decoded by the game's decoder) → X = coin (credit 1)
→ Start → girl select → A → stage intro → first round**, the round plays (player moves,
timer runs, lives are lost to enemies, the round restarts / game over / continue / back to
title all happen in a 9000-frame random-input run), and **A+B+X+Y+Start** soft-resets to a
boot-logo frame that is pixel-identical to the first boot. No KOS errors on the serial port.
Speed (emulated): boot logo and title 60 fps; in a round 45–55 fps (see "Remaining").

## What was wrong
1. **The freeze on the boot logo (the user's report).** The F3D interpreter ran
   synchronously inside `osSpTaskStartGo`, i.e. in the scheduler thread. While it waited in
   the PVR (scene submission can block for several vblanks, especially on the first frames)
   the vblank kept sending retrace messages to the scheduler's 8-entry interrupt queue.
   When the queue was full, the SP-done / DP-done messages that followed (posted with
   `OS_MESG_NOBLOCK`, as libultra's event dispatch does) were **dropped**: the graphics task
   never completed (`curRSPTask`/`curRDPTask` stuck, task state 3), the game's
   frames-in-flight counter `D_800C53F8` stayed at 2 and `Main_GfxThread` never built
   another frame. Observed in the harness: 5 frames built, then `intQ` = 8/8 and nothing more.
2. **Dead pads in some builds** (found after the fix above: Start did nothing on the title
   in the release build, worked in the debug build). `osContInit` reported the controllers
   from the vblank snapshot, which is empty if the call (during `Main_InitSystems`) comes
   before the first vblank has run. The game reads the pads only if `osContInit` found one
   (`D_800B0430 != 0`), so the pads stayed dead for the whole session. Timing-dependent, so
   it can also happen on a real console.
3. **Slow F3D** (made the title run at ~20 fps and rounds at 5 fps): 64-bit byte-wise FNV
   texture hash (64-bit multiplies are library calls on the SH4), byte-wise TMEM loads, the
   KOS twiddler (`pvr_txr_load_ex`, a division per texel) on every re-uploaded photo strip,
   and a generic per-texel decode.

## What was changed
* `port/ultra/os_sp.c` (rewritten): an **"RCP" KOS thread** (KOS prio 15, above all libultra
  threads, which are 20+). `osSpTaskStartGo` queues the task and returns at once, like the
  real RSP; the RCP thread runs F3D (one PVR scene), then posts SP done (+ DP done for XBUS
  tasks). Completions are **never dropped**: if the scheduler queue is full the RCP thread
  sleeps 1 ms and retries (`vd_events_retried` counts it; 0 in all runs so far). Audio tasks
  and `osDpSetNextBuffer` post through the same thread, so the scheduler never waits on its
  own queue. A generation counter drops stale jobs/completions after a soft reset.
  Counters for debugging: `vd_gfx_tasks_started`, `vd_gfx_tasks_done`, `vd_gfx_busy`.
* `port/ultra/os_thread.c`: `vd_try_post_event()` (returns -1 when the queue is full);
  `vd_post_event()` logs lost events in debug builds.
* `port/ultra/os_cont.c`: `osContInit` always reports pads 1 and 2 as connected (a missing
  pad reads as "no buttons"; a pad plugged in later works).
* `port/gfx/f3d.c`: Fletcher-style 32-bit-word texture hash (two running sums + final
  multiply mix, 4× unrolled); word-wise LOADBLOCK/LOADTILE into TMEM (fast path for whole
  aligned rows without wrap, odd-row half swap); RGBA16 decode fast path in `get_texture`;
  TMEM aligned to 8. Host tests still pass (`make -C port/gfx/tests run`: 134 checks + 32
  ROM display lists).
* `port/gfx/pvr_backend.c`: textures are **non-twiddled** and written straight to VRAM with
  32-bit stores; for clamped sizes (≥ 8) only width+1 columns / height+1 rows are written
  (the rest of the power-of-two area is never sampled). The boot-logo frame is bit-identical
  to the twiddled version (same SHA-1). `PvrBackendStats.wait_us_last` = time spent in
  `pvr_wait_ready` for the last frame.
* **Debug log** (compile-time switch): `port/sys/vd_debug.h/.c`,
  `make -f port/Makefile.dc DEBUG=<mask>` builds into `build/dc-debug/`.
  Categories: 0x01 boot/threads, 0x02 message-queue block/wake (verbose), 0x04 game-task
  switches (verbose), 0x08 overlay loads/entries/returns, 0x10 graphics tasks queued/done
  with F3D time and texture stats, swaps, osViBlack, 0x20 lost/retried events.
  Messages go to a 64 KB RAM ring (`vd_dbg_ring`, `vd_dbg_head`) that the harness reads
  out (`port_debug.log`); categories 0x01/0x08/0x20 also go to `dbglog` (serial).
  Log points: osStartThread, queue block/wake, `vd_task_prepare` (every task switch),
  Overlay_Load / overlay entry+return, osSpTaskStartGo / RCP done, osViSwapBuffer, Main_Boot.
* `tools/port/make_cdi.sh`: `VD_FIRST_READ=...` selects another 1ST_READ.BIN (debug build).
* `port/Makefile.dc`: `DEBUG` variable (above), `vd_debug.c` is picked up by the wildcard.

## The harness (headless Dreamcast emulator)
`tools/port/dc_harness/`:
| File | What |
|---|---|
| `build.sh` | clones Flycast at `FLYCAST_COMMIT` (+ submodules; freetype from its GitHub mirror), applies `flycast_harness.patch`, builds the libretro core (GLES3, no Vulkan), builds `libharness.so`. Output in `$DC_HARNESS_DIR` (default `$DC_HARNESS_DIR (default ~/vd-emu)`), ~25 min on 2 CPUs |
| `flycast_harness.patch` | serial console to the file in `$FLYCAST_SERIAL`; `$FLYCAST_INTERP` = SH4 interpreter; exported `retro_dbg_get_regs` (SH4 registers), `retro_dbg_prof` / `retro_dbg_prof_read` (PC/PR sampled every N SH4 cycles) |
| `harness.c` | minimal libretro frontend (shared lib): EGL surfaceless + Mesa llvmpipe GLES context and FBO, core options, scripted pad input, RAM access, frame read-back |
| `dcrun.py` | Python driver (`DC` class + CLI): run N frames with `--input`, `--watch` ELF symbols, `--stats` (frame hash + non-black pixel count), `--regs`, `--save-frame` (boot logo only!), debug-ring capture, profiler (`prof_start/prof_stop/prof_report`) |
| `vdstate.py` | readers for the scheduler, gfx tasks, message queues, the game's task list |
| `boot_test.py` | regression test of the whole path above (exit 0 = PASS) |
| `ppm2png.py` | converts a saved (boot-logo) PPM to PNG |

Commands (from the project folder):
```
tools/port/dc_harness/build.sh                         # once
make -f port/Makefile.dc && tools/port/make_cdi.sh baserom.z64
python3 -I tools/port/dc_harness/boot_test.py build/dc  # ~3 min; prints each stage, PASS/FAILED
# debug build + log:
make -f port/Makefile.dc DEBUG=0x39
VD_FIRST_READ=build/dc-debug/1ST_READ.BIN tools/port/make_cdi.sh baserom.z64 build/dc-debug/vividdolls.cdi
python3 -I tools/port/dc_harness/dcrun.py build/dc-debug/vividdolls.cdi --elf build/dc-debug/vividdolls.elf \
    --frames 1500 --input "400:X:8,500:START:8,900:A:8" --every 100 \
    --watch vd_vi_retrace_count,D_800C1650,D_800C53F8,gCredits
# -> $DC_HARNESS_DIR/harness/out/{serial.log,port_debug.log,harness.log}
```
Python use: `from dcrun import DC; dc = DC(cdi); dc.load_syms(elf); dc.set_pad(0, ["X"]);
dc.run(10); dc.u32("gCredits"); dc.regs_str(); dc.frame_stats()`.
Notes: one `dc.run(1)` = one `retro_run` = one presented frame, which can be several
vblanks when the game is slow (use `vd_vi_retrace_count` for time). Emulated speed ~15–40
frames per second of host time. Flycast's SH4 timing ignores cache misses, so a real
console may be somewhat slower than the harness says. Photo rule: only boot-logo frames may
be saved/viewed; everything else is checked by hash / pixel counts / game state.

Useful game state: `D_800C1650` frames built, `D_800C53F8` frames in flight (stuck at 2 =
frames not retiring), `D_800C0AC0` the scheduler (`vdstate.sched`), `D_800B0730` the two
GfxTasks, the task list (`vdstate.tasks`, function of each task's resume pc), `gCredits`,
`ovl2_D_800FD424` round timer, `ovl2_D_800FD3F8` player 1 (x at +4, y at +8),
`D_800B04D4` lives P1, `vd_overlay_current` (static in overlay.c).

## Remaining issues
1. **In-round speed 45–55 fps** (emulated). A frame is lost whenever F3D + game logic of one
   frame exceed 16.7 ms (the N64 scheduler design starts a task only on a retrace). Profile in
   a round (release): F3D interpreter ~26 % (LOADTILE copies, `tex_key` hashing of ~70 photo
   strips per frame), `emit_poly` ~14 %, hash ~9 %, game logic small; idle ~28 % (lost
   frames). Ideas: hash the source while copying in LOADTILE and reuse it in `tex_key`;
   cache combiner reduction per state change; a dedicated photo path (one 512×512 texture
   updated by the game's dirty-block map instead of 70 strips).
2. Real-console check not done (GDEMU), as planned: the user's Flycast run is the next check.
3. `port/notes/endian.md` open issue "nothing writes a type into GfxTask.msgbuf" is not a
   problem: `D_800B2888` / `D_800B4A08` (= `gGfxBuffers` + 0x2158 and + 0x2180 + 0x2158) are set
   to 2 by the video init in gfx.c; the harness shows type 2 in both tasks.
4. KOS thread ageing / priorities: not seen to matter; `vd_events_retried` stayed 0.
5. Audio is still silent (milestone 3); EEPROM/VMU, test mode not exercised by the test.
