# Area "perf": graphics speed, photo-strip seams, coin input (2026-11-05)

Owner files: `port/gfx/**`, `port/ultra/os_sp.c`, `os_vi*.c`, `os_cont.c`, this file.

## Real-Dreamcast report 2 (2026-11-05, evening): wide-photo pan, projectiles, ending music
Measured with the new harness script `tools/port/dc_harness/perf_cases.py` (cases below);
"before" = the build the user tested, "after" = the current release.

| Case (harness, Flycast) | before | after |
|---|---|---|
| stage-clear pan of the 640x240 photo (girl 6, photo 3) | **30.1 fps**, F3D 26.0 ms/frame, 34 strip uploads/frame, 484 of 618 frames late, 25 audio underruns | **56.7 fps**, F3D 8.3 ms, 1.1 uploads/frame, 11 of 633 late, 0 underruns |
| same pan, tall 320x480 photo (reference) | 58.3 fps, F3D 8.3 ms, 1.8 uploads/frame | unchanged (identical path) |
| round with the spinning boss attacking non-stop (0-7 / 16-23 / 32-39 live effects) | 57.6 / 56.9 / - fps, F3D 6.4 / 7.3 / - ms | 57.8 / 56.5 / 55.6 fps, F3D 6.1 / 6.9 / 8.0 ms |
| ending (final texts, petals) | 55.6 fps, F3D 10.5 ms, 2 underruns | 56.2 fps, F3D 10.2 ms, 0 underruns; 414 tris, 185 headers, 853 kpx blended per frame (max 977 kpx: the heaviest scene of the game) |
| audio_test with F3D slowed to 24 ms per frame (`-DVD_TEST_SLOW_US=24000`, stands in for a heavy real-HW frame) | **381 underruns** in 51 s, 2535 audio frames for 3075 vblanks | **0 underruns**, 3076 audio frames |

"late" = a presented frame that took 2 or more vblanks. (~2 % of frames are late in every
scene of the harness, also before; that is Flycast's frame pacing.)

### 1. Wide photo pan: every strip re-uploaded every frame (fixed)
`PhotoView_Build` draws the photo as 40 strips; each is a LOADTILE of 321x7 texels from the
image at x & ~3 plus a TEXRECT offset for x & 3. The interpreter keys a strip texture on its
texel window (the "crop": exactly the texels the RDP reads, rows 1..6). Vertical pans are cheap
by luck: strip k at scroll y+6 is strip k+1 at y, so only one new strip per frame. A horizontal
pan by 1 px changes every strip's window, so **all 40 strips were hashed, decoded, converted
and DMA'd every frame** (34 per frame on average), plus three slow paths on top:
* window start not 4-texel aligned (x & 3 != 0, 3 frames out of 4): the RGBA16 extraction from
  TMEM fell back to a per-texel loop;
* the conversion copied every row into a scratch row first (edge columns);
* the 512-slot texture pool was scanned linearly for a free slot per upload and refilled
  every ~12 frames (40 dead strips per frame).
On the console that is ~40 x (4.5 KB TMEM copy + extraction + 8 KB conversion + cache write-back
+ 7 KB DMA) per frame, about 1.3 MB of memory traffic: the pan ran at 20-30 fps.

Fixes (port/gfx):
* **Aligned window textures for wide sources** (`wide_texture()` in f3d.c): for a tile load
  from an image of 1024+ bytes per row (only the 640-wide photo), the strip texture starts at a
  64-byte aligned position of the source memory and holds the window plus up to 32 texels of
  the real image on each side (384 wide, 512x8 in VRAM). While the window slides inside it, the
  content, key and texture stay the same: one strip upload per frame on average, at most 10.
  The alignment phase depends on the row address (row mod 8), so the 40 strips cross their
  alignment boundary in 8 different frames instead of all at once. Rows: the same 6-row window
  and edge padding as before. Columns: the same texels; only the PVR's extra bilinear tap at
  the window's left/right end reads the real neighbouring texel instead of a replicated one.
  320-wide images (title, tall and normal photos) and sprites never take this path.
* **Direct source path** (all RGBA16 tile windows whose rows are intact in TMEM, `pl_direct_ok`):
  the backend converts straight from the source image (new `F3DTexImage.stride`), no TMEM copy,
  no extraction; identical texels (unit test + replay check).
* `pvr_texfill.h`: RGBA16 rows with a pitch, 32-bit loads when aligned, edge columns handled
  at the row end (no scratch copy). Same output (test_texfill compares with the ARGB path).
* `pvr_backend.c`: free-slot stack instead of the linear scan.

### 2. Many projectiles
`dl_stats.py` shows what an effect costs: every live effect is a full object (38 F3D commands:
matrix, light set-up, TLUT + texture load, 4 vertices with lighting, 2 triangles, a new PVR
header). Uploads stay at 0.2 per frame (the sprites are cached), so re-decoding is not the
problem. Per-frame profiles by number of live effects (`perf_cases.py proj --prof`): from 0-7
to 32-39 effects the busy time grows by ~2 ms in Flycast, ~75 % of it in the interpreter
(command loop, emit_poly, a byte-wise `memcmp`), the rest audio (explosions start sound 0x11
every frame) and game logic. On the PVR everything is drawn blended (also the photo: OPA_SURF +
alpha compare is emulated with blending): 450 kpx per frame, 570 kpx at most, ~400 polygons,
well inside the PVR's fill rate. So on the console the slowdown is CPU time over the 16.7 ms
budget, where Flycast's model is optimistic (no cache misses). Changes:
* the RCP thread no longer starves the audio manager and the scheduler (section 3), so a late
  graphics frame no longer delays audio and input;
* `memcmp` (byte loop in newlib) replaced by word compares for the per-polygon state and
  per-vertex colour caches; a TLUT reloaded with the same colours (every sprite of a kind
  loads its palette) keeps its cached hash;
* the texture-key hash prefetches the next cache line / the next source row (`pref`; the
  photo strips, ~200 KB per frame, are hashed from RAM that is not in the cache). Flycast
  does not model this; it only matters on the console.
Not done (would change output order or is a bigger change): moving the opaque photo into the
PVR opaque list, merging the per-object header set-up.

### 3. Music stutter in the ending (and in any long frame)
The RCP thread (F3D) ran at KOS priority 15, above every libultra thread. While it
interpreted a frame (10-20 ms in the ending on the console), neither the scheduler nor the
audio manager could run, and the audio task of the next retrace also waited for the gfx
task's SP-done (the scheduler "yields" a running gfx task - a no-op here - and waits). The
audio manager can make up at most ~100 samples per frame, so runs of long frames emptied the
AICA stream. Fixes in `port/ultra/os_sp.c`:
* RCP thread priority just below the audio manager (libultra 14) and the scheduler (15),
  above every other game thread: a retrace preempts F3D, the audio frame is mixed on time;
* XBUS gfx tasks: SP done is posted when the task starts, DP done when F3D is over (to the
  scheduler: RSP finished, RDP still drawing). The task still completes - message, buffer
  swap - only with DP done, and the next gfx task waits for the RDP as before; but the RSP is
  free for the audio task;
* audio tasks complete at once from osSpTaskStartGo (NOBLOCK into the scheduler's own queue,
  through the RCP thread only if that queue is full), not queued behind a running F3D job.
Proof in the harness: the slowed build above (every F3D task padded to 24 ms): 381 underruns
before, 0 after.

### Harness additions (tools/port/dc_harness/)
* `perf_cases.py BUILD [wide] [tall] [ending] [proj] [--prof] [--all-bosses] [--frames N]`:
  boot -> round, then per case fps (frames built / vblanks), late frames, F3D time and strip
  uploads per gfx task (`vd_perf_ring`), interpreter counters per task (`vd_perf_f3d_stats`,
  os_sp.c), PVR load per frame (`vd_pvr_acc`, pvr_backend.c: triangles, headers, opaque and
  blended pixel area, upload bytes) and audio counters (`vd_audio_stats`). `wide` jumps to the
  only 640x240 photo (girl 6 photo 3, found from the jpeg64 xw/yw headers) by setting
  D_800B0608 / D_800B04CC before an L+R clear; `ending` marks the other 7 girls done and the
  photo as the third (D_800C178E, D_800B04CC) and clears; `proj` sets the spinning boss's
  attack-chance table (ovl2_D_800F9D00) to 1 and keeps P1's lives topped up, results by number
  of live effects (gEffects, ovl2_D_800FBA90). `--prof`: PC profile (per effect bucket for proj).
  Flycast's debug log goes to /dev/null (a long run wrote 20 GB of harness.log).
* `dl_stats.py BUILD [--min-fx N]`: opcode histogram and per-object command shapes of one frame
  with N live effects (command words only).
* `port/gfx/tools/seam_check.py BUILD --wide`: also models the wide photo's round and four
  points of its stage-clear pan (all phases 1.000, no seam).

### Tests run for this change
```
make -C port/gfx/tests run          # 139 checks (new: raw RGBA16 strip paths, narrow = bit-identical,
                                    #   wide = same texel everywhere for all 320 scroll positions, 11 uploads)
                                    # + texfill (new: pitch/alignment equivalence) + 32 ROM DLs
make -C port/gfx/tests run-shz
make -C port/gfx/tests replay F3D_REF=<previous f3d.c> && python3 -I port/gfx/tools/replay_check.py build/dc
                                    # title / select / round: 0 mismatches (interpreter output identical)
python3 -I port/gfx/tools/seam_check.py build/dc --wide        # PASS
python3 -I tools/port/dc_harness/boot_test.py build/dc         # PASS (title 60.0, round 58.0 fps)
python3 -I tools/port/dc_harness/cheat_test.py build/dc        # PASS
python3 -I port/audio/tests/audio_test.py build/dc             # PASS
python3 -I port/gfx/tools/perf_run.py build/dc                 # title 59.3, select 56.6, round 58.5 fps
                                                               # (round F3D avg 7.3 -> 5.9 ms, max 31 -> 11 ms)
```
Frame hashes: the photo scenes that do not use the wide path render identically (replay check
compares every interpreter output incl. texture digests). Frame-by-frame hash sequences of two
emulator runs are not comparable any more after the thread-priority change (game timing vs the
scripted input shifts by a vblank here and there; by frames-built counter 400 of 413 title
frames match, the rest are in-flight timing).

### Open
1. Real-console check (one GDEMU run): wide-photo pan (girl 6, third photo; or the L+R cheat
   path), a spinning-boss round, the ending music.
2. If projectiles still slow down on the console: the remaining per-object interpreter cost
   (~55 us per effect sprite in Flycast); ideas: cache compiled PVR headers per state, skip
   the light re-transform when the model-view matrix only translates.

## Status
Release `build/dc/vividdolls.cdi`. Harness (headless Flycast, `port/gfx/tools/perf_run.py`):

| Phase | fps | F3D avg / max |
|---|---|---|
| title | 59.5 | 6.5 / 9.1 ms |
| girl select | 56.6 | 8.1 ms (one ~100 ms spike: load) |
| round | 57.7 (per 50 frames 55-60) | 7.3 / 31 ms |

boot_test: title 59.0, round 58.2 fps. Flycast ignores cache misses, so a real console may be a
little slower. The user's real-Dreamcast test of the previous build: runs well.

## Photo-strip lines (user report from a real Dreamcast): fixed
Symptom: thin dark horizontal lines across the photo background, one every 12 px at 640x480,
on hardware only. The harness never showed them.

What the game draws: 40 strips per photo (`PhotoView_Build`). Each strip is a LOADTILE of
321x7 RGBA16 texels, line 81 (648 bytes), so row 6 wraps over the start of row 0 in the 4 KB
TMEM. Then a 1-cycle bilinear TEXRECT y 0.25..6.25, dsdx = dtdy = 1. The real RDP writes
scanlines 1..6 with t = 1..6 (integer t, so no vertical filtering), so the clobbered row 0
never shows. The interpreter matches this: its texel window is rows 1..6, and the TMEM wrap
emulation is correct.

**The cause was the PVR backend, not the TMEM emulation.** A 6-row window becomes a 512x8
PVR texture. The backend wrote rows 0..5 plus one replicated row (6) and assumed the rest is
never sampled. At 2x scale the first 640-line of each strip samples v = 0.25 texel. The
PVR's bilinear filter takes the other tap from row -1, and its address wraps to row 7 of the
power-of-two texture even with UV clamp on. Row 7 was stale, never-written VRAM, so that
line got a 25 % blend with garbage. GL emulators clamp per texel (GL_CLAMP_TO_EDGE), which
hides this.

Fix (`port/gfx/pvr_texfill.h`, used by `pvr_backend.c`): the texture layout and conversion
moved into a pure-C header. When the edge pad leaves spare space, row h2-1 is now a copy of
row 0 and column w2-1 a copy of column 0, so the wrapped filter tap reads the edge. Cost: one
extra row of stores per strip upload. The horizontal RDP blur (s = X - 0.25) is unchanged.

Numeric check (no pixels written, photo rule): `seam_check.py` (below). Brightness of each
line relative to the RDP model, by line phase within a strip:

| Model | title phase 0 | round phase 0 | other phases |
|---|---|---|---|
| hw-old (previous layout, wrapped neighbour, stale = black) | 0.750 | 0.750 | 1.00 |
| hw-new (current layout) | 1.000 | 1.000 | 0.994-1.006 |
| gl-new (per-texel clamp) | 1.000 | 1.000 | same as hw-new |

The Flycast frame shows no periodic dip either. The remaining mean |PVR - RDP| of 0.75/255
comes from the PVR's mild vertical filtering at 2x, which the RDP doesn't do.

Limit: a clamped texture whose height (or width) leaves no spare row (column), i.e. size
== power of two or power of two - 1, still wraps its first-edge tap onto the last row. That
doesn't apply to the photo. If a sprite ever shows a seam on hardware, enlarge h2/w2 for
those.

## Coin input (verified)
`os_cont.c`: X starts one coin pulse. The coin line stays on until the game has read
0x02000004 at least 6 times and 4 vblanks have passed, so a short press (2 frames or more)
adds exactly one credit, even when frames run late. `coin_test.py` passes at 59.6 fps
(release), at 30 fps (`-DVD_TEST_SLOW_US=22000`) and at 40 fps (`... -DVD_TEST_SLOW_ALT`),
with presses of 2/3/4/6 frames. A 1-frame tap can fall between two maple polls.

## Perf changes in this build (all tested)
* f3d.c: word-wise TMEM loads, source-hash memo, partial-row hashing (`pl_source_hash(p, r0)`:
  rows before the window's first row are left out of the key, see the comment), LOADTLUT
  memcpy, RGBA16 passed raw to the backend (`F3D_CAP_RGBA16`).
* pvr_backend.c: direct rendering (`pvr_dr_target`/`commit`, the SQ lock is held from
  list_begin to list_finish), PVR DMA for texture uploads of 2 KB or more, RGBA16 to
  565/1555 two texels per word.
* os_sp.c: always-on timing counters (`vd_perf_*`, read by perf_run.py), test-only slowdown
  `VD_TEST_SLOW_US` / `VD_TEST_SLOW_ALT` (off unless defined).

## Tests
```
make -C port/gfx/tests run                                   # 136 checks + texfill + 32 ROM DLs
python3 -I tools/port/dc_harness/boot_test.py build/dc
python3 -I port/audio/tests/audio_test.py build/dc
python3 -I port/gfx/tools/coin_test.py build/dc [2,2,3,6]
python3 -I port/gfx/tools/perf_run.py build/dc [--prof]
make -C port/gfx/tests seam_lib.so && python3 -I port/gfx/tools/seam_check.py build/dc   # exit 0 = no seam
```
Slowed coin builds: copy build/dc to a temp dir, delete its port/ultra/os_sp.o, then
`make -f port/Makefile.dc BUILD=<dir> DEFS="-DVD_PORT -DVD_DREAMCAST -DVD_DEBUG=0 -DVD_TEST_SLOW_US=22000"`
and `VD_FIRST_READ=<dir>/1ST_READ.BIN tools/port/make_cdi.sh baserom.z64 <dir>/vividdolls.cdi`.

## Open
1. Real-console check of the seam fix (one GDEMU run of the title and a round).
2. `port/notes/gfx.md` (not my file) still says "the rest of the power-of-two area is never
   sampled" in the PVR backend section; it should point to pvr_texfill.h.
