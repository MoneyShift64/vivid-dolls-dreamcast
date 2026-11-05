#!/usr/bin/env python3
"""Photo-strip seam check: PVR output model vs the real RDP, numbers only.

  make -C port/gfx/tests seam_lib.so
  python3 -I port/gfx/tools/seam_check.py BUILD_DIR [--round-frames N] [--wide]

Runs the game in headless Flycast along the boot_test path. On the title, girl
select and in the round the RAM of a completed graphics task is copied *in
memory* into port/gfx/tests/seam_lib.so, which interprets the display list with
f3d.c and rasterises every bilinear TEXRECT of the photo (6-row strip windows)
three ways at 640x480 (see seam_lib.c):
  hw-new  PVR sampling with the hardware's wrapped filter neighbour, current layout
  hw-old  same, layout before the wrap-side padding fix (stale VRAM = black)
  gl-new  per-texel clamp (what GL emulators such as the harness show)
and as the RDP draws it (reference). Printed per model: mean |model - RDP| and
the per-line brightness relative to the RDP by line phase within a strip (12
lines at 640x480). A dark seam shows as a phase with ratio well below 1.
Also prints the same per-line profile of the harness's real frame (Flycast) vs
the RDP model. Exit 0 if hw-new has no seam (every phase within 2 %).
Photo rule: nothing is written to disk; only aggregate numbers are printed.
"""
import ctypes
import os
import sys

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "tools", "port", "dc_harness"))
from dcrun import DC, OUT  # noqa: E402

args = sys.argv[1:]
BUILD = args[0]
RF = int(args[args.index("--round-frames") + 1]) if "--round-frames" in args else 300
WIDE = "--wide" in args      # also the 640x240 photo (girl 6, photo 3): its round and stage-clear pan

lib = ctypes.CDLL(os.path.join(HERE, "..", "tests", "seam_lib.so"), mode=os.RTLD_LOCAL)
lib.sm_frame.argtypes = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32]
lib.sm_frame.restype = ctypes.POINTER(ctypes.c_float)
lib.sm_set_height_range(6, 7)          # the photo strips' texel windows (6 rows)

dc = DC(os.path.join(BUILD, "vividdolls.cdi"), serial=os.path.join(OUT, "serial_seam.log"))
dc.load_syms(os.path.join(BUILD, "vividdolls.elf"))
GT = dc.syms["D_800B0730"]
NAMES = ["hw-new", "hw-old", "gl-new"]
ok = [True]


def task_funcs():
    head = dc.u32("D_800B05A4")
    tail = dc.u32("D_800C53FC")
    names = []
    t = dc.u32(head) if head else 0
    n = 0
    while t and t != tail and n < 64:
        sp = dc.u32(t + 8)
        names.append(dc.func_of(dc.u32(sp + 0x3C)) if sp else "?")
        t = dc.u32(t)
        n += 1
    return names


def step(n, pad=()):
    for _ in range(n):
        dc.set_pad(0, list(pad))
        dc.run(1)


def wait_for(names, limit, pad=()):
    for _ in range(limit):
        if any(f in names for f in task_funcs()):
            return
        step(1, pad)
    print("FAIL waiting for", names)
    sys.exit(2)


def model_frame():
    """models the last submitted graphics task; returns the 4 model buffers"""
    for _ in range(30):
        step(1)
        # the last submitted task's list (in flight or just done) is not touched
        # by the game until the next-but-one frame
        buf = dc.u32("D_800C0AB8") ^ 1
        dl = dc.u32(GT + buf * 0x2180 + 0x40)
        if (dl >> 24) != 0x8C:
            continue
        p = lib.sm_frame(ctypes.c_void_p(dc.ram_ptr), dc.ram_size, dl)
        a = np.ctypeslib.as_array(p, shape=(4, 480, 640)).copy()
        return a
    return None


def phase_profile(m, ref, mask):
    """per 640-line brightness ratio m/ref over the masked pixels, by line phase
    within a strip; strips start at N64 line 6k+1 = 640 line 12k+2"""
    rows = []
    for ph in range(12):
        ys = [y for y in range(480) if (y - 2) % 12 == ph]
        mm = mask[ys]
        s_m = m[ys][mm].sum()
        s_r = ref[ys][mm].sum()
        rows.append(s_m / s_r if s_r > 0 else float("nan"))
    return rows


def analyse(desc):
    a = model_frame()
    if a is None:
        print("%-12s no completed frame" % desc)
        ok[0] = False
        return
    ref = a[3]
    drawn = ref >= 0
    print("%-12s photo-strip pixels modelled: %d (%.0f %% of the screen)" % (desc, drawn.sum(), 100.0 * drawn.mean()))
    if drawn.sum() < 20000:
        print("             (no photo background in this frame)")
        return
    for k, name in enumerate(NAMES):
        m = a[k]
        mask = drawn & (m >= 0)
        err = np.abs(m[mask] - ref[mask]).mean() / 3.0
        prof = phase_profile(m, ref, mask)
        worst = min(prof)
        print("  %-7s mean |model-RDP| %5.2f /255   line ratio by phase 0..11: %s   min %.3f" %
              (name, err, " ".join("%.3f" % r for r in prof), worst))
        if name == "hw-new" and (worst < 0.98 or max(prof) > 1.02):
            ok[0] = False
    # the harness's own frame (Flycast GL), same profile vs the RDP model
    rgb, fw, fh = dc.frame_rgb()
    if rgb is not None and (fw, fh) == (640, 480):
        f = np.frombuffer(bytes(rgb), dtype=np.uint8).reshape(480, 640, 3).astype(np.float32).sum(axis=2)
        prof = phase_profile(f, ref, drawn)
        print("  %-7s line ratio by phase (other layers included): %s" % ("flycast", " ".join("%.3f" % r for r in prof)))
        del f
    del rgb, a


wait_for(("Boot_Logo",), 300)
step(60)
wait_for(("Title_Main", "title_slide"), 900)
step(240)
analyse("title")
step(8, ["X"])
step(30)
step(8, ["START"])
wait_for(("GirlSelect_Task",), 600)
step(90)
analyse("girl select")
step(8, ["A"])
wait_for(("StageIntro_Main",), 600)
wait_for(("Game_Main",), 900)
step(RF)
analyse("round")
if WIDE:
    import struct

    def cheat():
        for _ in range(10):
            dc.set_trig(0, 0x7FFF, 0x7FFF)
            step(1)
        dc.set_trig(0, 0, 0)

    dc.write(dc.syms["D_800B0608"], struct.pack("<i", 6))
    dc.write(dc.syms["D_800B04CC"], struct.pack("<i", 1))
    cheat()
    for _ in range(6000):
        if dc.s32("ovl2_D_800FBA70") == 640 and dc.s32("ovl2_D_800FD2D8") == 0 and "Game_Main" in task_funcs():
            break
        step(1)
    print("wide photo: board %dx%d" % (dc.s32("ovl2_D_800FBA70"), dc.s32("ovl2_D_800FBA74")))
    step(RF)
    analyse("wide round")
    cheat()
    for _ in range(600):
        if dc.s32("ovl2_D_800FD2D8") == 1:
            break
        step(1)
    step(40)
    for k in range(4):
        step(37)
        print("  scroll x %d" % dc.s32("ovl2_D_800FD2B8"))
        analyse("wide pan %d" % k)
print("PASS" if ok[0] else "FAILED")
sys.exit(0 if ok[0] else 1)
