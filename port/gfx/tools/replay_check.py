#!/usr/bin/env python3
"""Differential check of two F3D interpreter builds on real game frames.

  make -C port/gfx/tests replay F3D_REF=/path/to/reference/f3d.c
  python3 -I port/gfx/tools/replay_check.py BUILD_DIR [--girl N] [--round-frames N] [--verbose]

Runs the game in headless Flycast (BUILD_DIR/vividdolls.cdi + .elf) along the
boot_test path (title, girl select, round with line drawing). After every
completed graphics task the emulator RAM is copied *in memory* into both replay
libraries (port/gfx/tests/replay-ref.so and replay-new.so), the frame's display
list is interpreted by each with a hashing backend, and the digests are compared.
Nothing is written to disk (the RAM holds decoded photo pixels).
Exit status 0 = all frames identical.
"""
import ctypes
import os
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "tools", "port", "dc_harness"))
from dcrun import DC, OUT  # noqa: E402

args = sys.argv[1:]
BUILD = args[0]
GIRL = int(args[args.index("--girl") + 1]) if "--girl" in args else 0
RF = int(args[args.index("--round-frames") + 1]) if "--round-frames" in args else 600
VERBOSE = "--verbose" in args
DUMP = [int(x) for x in args[args.index("--dump") + 1].split(",")] if "--dump" in args else []
TESTS = os.path.join(HERE, "..", "tests")

libs = {}
for name in ("ref", "new"):
    lib = ctypes.CDLL(os.path.join(TESTS, "replay-%s.so" % name), mode=os.RTLD_LOCAL)
    lib.rp_frame.argtypes = [ctypes.c_void_p, ctypes.c_uint32, ctypes.c_uint32,
                             ctypes.POINTER(ctypes.c_uint64), ctypes.POINTER(ctypes.c_uint32)]
    libs[name] = lib

dc = DC(os.path.join(BUILD, "vividdolls.cdi"), serial=os.path.join(OUT, "serial_replay.log"))
dc.load_syms(os.path.join(BUILD, "vividdolls.elf"))
GT = dc.syms["D_800B0730"]
last_done = [dc.u32("vd_gfx_tasks_done")]
stat = {"frames": 0, "bad": 0}
tot = {"ref": [0] * 16, "new": [0] * 16}
phase = ["boot"]


def check():
    done = dc.u32("vd_gfx_tasks_done")
    if done == last_done[0] or dc.u32("vd_gfx_busy") or dc.u32("vd_gfx_tasks_started") != done:
        return
    last_done[0] = done
    buf = dc.u32("D_800C0AB8") ^ 1
    dl = dc.u32(GT + buf * 0x2180 + 0x40)
    if (dl >> 24) != 0x8C:
        return
    d = {}
    for name, lib in libs.items():
        dg = ctypes.c_uint64()
        st = (ctypes.c_uint32 * 16)()
        lib.rp_frame(ctypes.c_void_p(dc.ram_ptr), dc.ram_size, dl, ctypes.byref(dg), st)
        d[name] = dg.value
        for i in range(16):
            tot[name][i] += st[i]
    stat["frames"] += 1
    if stat["frames"] in DUMP:
        print("  --- dump of checked frame %d (%s), new build" % (stat["frames"], phase[0]), flush=True)
        libs["new"].rp_set_verbose(1)
        libs["new"].rp_frame(ctypes.c_void_p(dc.ram_ptr), dc.ram_size, dl, ctypes.byref(ctypes.c_uint64()), None)
        libs["new"].rp_set_verbose(0)
    if d["ref"] != d["new"]:
        stat["bad"] += 1
        if stat["bad"] <= 10:
            print("  MISMATCH %s frame %d task %d dl %08x ref %016x new %016x" %
                  (phase[0], dc.frame, done, dl, d["ref"], d["new"]), flush=True)
            if VERBOSE:
                for name, lib in libs.items():
                    print("   ---", name, flush=True)
                    lib.rp_set_verbose(1)
                    lib.rp_frame(ctypes.c_void_p(dc.ram_ptr), dc.ram_size, dl, ctypes.byref(ctypes.c_uint64()), None)
                    lib.rp_set_verbose(0)


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
        check()


def wait_for(desc, names, limit, pad=()):
    phase[0] = desc
    for _ in range(limit):
        if any(f in names for f in task_funcs()):
            return
        step(1, pad)
    print("FAIL waiting for", desc)
    sys.exit(2)


prev = [None]


def report(desc):
    t = tot["new"]
    if prev[0] is not None:
        p = prev[0]
        nf = max(stat["frames"] - p[16], 1)
        names = ["uploads", "hits", "tris_out", "draws", "cmds", "img_upd", "img_tex", "loadblock", "loadtile",
                 "loadtlut", "load_bytes", "hash_bytes", "tex_lookups", "tris_in", "rects", "vtx"]
        print("   per frame: " + " ".join("%s=%.0f" % (names[i], (t[i] - p[i]) / nf) for i in range(16)))
    prev[0] = list(t) + [stat["frames"]]
    print("%-12s frames %5d  mismatches %d   (new: uploads %d hits %d tris %d img_updates %d img_texels %d)" %
          (desc, stat["frames"], stat["bad"], tot["new"][0], tot["new"][1], tot["new"][2], tot["new"][5],
           tot["new"][6]), flush=True)


wait_for("boot", ("Boot_Logo",), 300)
step(60)
wait_for("title", ("Title_Main", "title_slide"), 900)
step(60)
report("title-start")
step(180)
report("title")
step(8, ["X"])
step(30)
step(8, ["START"])
wait_for("girl select", ("GirlSelect_Task",), 600)
step(60)
for i in range(GIRL):
    step(8, ["RIGHT"])
    step(16)
step(60)
report("girl select")
step(8, ["A"])
wait_for("stage intro", ("StageIntro_Main",), 600)
wait_for("round", ("Game_Main",), 900)
phase[0] = "round"
moves = (["RIGHT", "A"], ["DOWN", "A"], ["LEFT", "A"], ["UP", "A"], ["RIGHT"], ["DOWN"], ["UP", "A"], ["LEFT"])
k = 0
while k < RF:
    step(50, moves[(k // 50) % len(moves)])
    k += 50
report("round")
print("PASS" if stat["bad"] == 0 else "FAILED")
sys.exit(0 if stat["bad"] == 0 else 1)
