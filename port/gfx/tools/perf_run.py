#!/usr/bin/env python3
"""Frame-rate / profile measurement of the Dreamcast build in headless Flycast.

  python3 -I port/gfx/tools/perf_run.py BUILD_DIR [--prof] [--round-frames N] [--hashes FILE]

BUILD_DIR holds vividdolls.cdi + vividdolls.elf (e.g. build/dc).
Path (same as boot_test.py): boot logo -> title -> coin -> start -> girl select -> A ->
stage intro -> round, with a fixed input script. Prints fps per phase =
60 * frames built / vblanks (D_800C1650 / vd_vi_retrace_count) and, with --prof, a
PC-sampling profile of the round. --hashes writes one line per presented frame
(frame, vblank, frames built, frame hash prefix) for automated before/after
comparison. Photo rule: frames are only hashed, never written out.
"""
import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "tools", "port", "dc_harness"))
from dcrun import DC, OUT  # noqa: E402

args = sys.argv[1:]
BUILD = args[0]
PROF = "--prof" in args
RF = int(args[args.index("--round-frames") + 1]) if "--round-frames" in args else 600
HASHES = args[args.index("--hashes") + 1] if "--hashes" in args else None

dc = DC(os.path.join(BUILD, "vividdolls.cdi"), serial=os.path.join(OUT, "serial_perf.log"))
dc.load_syms(os.path.join(BUILD, "vividdolls.elf"))
hf = open(HASHES, "w") if HASHES else None


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


def counters():
    return dc.u32("vd_vi_retrace_count"), dc.u32("D_800C1650")


def step(n, pad=()):
    for _ in range(n):
        dc.set_pad(0, list(pad))
        dc.run(1)
        if hf:
            vi, b = counters()
            st = dc.frame_stats()
            hf.write("%d %d %d %s\n" % (dc.frame, vi, b, st[0] if st else "-"))


def wait_for(desc, names, limit, pad=()):
    for _ in range(limit):
        if any(f in names for f in task_funcs()):
            return True
        step(1, pad)
    print("FAIL waiting for", desc)
    sys.exit(1)


import struct


def perf_snap():
    if "vd_perf_tasks" not in dc.syms:
        return None
    return [dc.u32("vd_perf_tasks"), dc.u32("vd_perf_f3d_us_sum"), dc.u32("vd_perf_wait_us_sum")] + \
        list(struct.unpack("<8I", dc.read(dc.syms["vd_perf_f3d_hist"], 32)))


def perf_reset_max():
    if "vd_perf_f3d_us_max" in dc.syms:
        dc.write(dc.syms["vd_perf_f3d_us_max"], b"\0\0\0\0")


def perf_line(a, b):
    if not a or not b or b[0] == a[0]:
        return ""
    n = b[0] - a[0]
    hist = [b[3 + i] - a[3 + i] for i in range(8)]
    return "   f3d avg %.2f ms (incl. pvr wait %.2f ms), max %.2f ms, hist(2ms) %s" % (
        (b[1] - a[1]) / n / 1000.0, (b[2] - a[2]) / n / 1000.0, dc.u32("vd_perf_f3d_us_max") / 1000.0,
        " ".join(str(h) for h in hist))


def fps_over(n, pad=()):
    vi0, b0 = counters()
    step(n, pad)
    vi1, b1 = counters()
    return 60.0 * (b1 - b0) / max(vi1 - vi0, 1)


res = {}
wait_for("boot logo", ("Boot_Logo",), 300)
step(60)
wait_for("title", ("Title_Main", "title_slide"), 900)
p0 = perf_snap(); perf_reset_max()
res["title"] = fps_over(240)
lines = {"title": perf_line(p0, perf_snap())}
step(8, ["X"])
step(30)
step(8, ["START"])
wait_for("girl select", ("GirlSelect_Task",), 600)
p0 = perf_snap(); perf_reset_max()
res["select"] = fps_over(120)
lines["select"] = perf_line(p0, perf_snap())
step(8, ["A"])
wait_for("stage intro", ("StageIntro_Main",), 600)
wait_for("round", ("Game_Main",), 900)
step(120)
moves = (["RIGHT", "A"], ["DOWN", "A"], ["LEFT", "A"], ["UP", "A"], ["RIGHT"], ["DOWN"])
if PROF:
    dc.prof_start(20000)
p0 = perf_snap(); perf_reset_max()
vi0, b0 = counters()
per = []
k = 0
while k < RF:
    n = min(50, RF - k)
    per.append(fps_over(n, moves[(k // 50) % len(moves)]))
    k += n
vi1, b1 = counters()
res["round"] = 60.0 * (b1 - b0) / max(vi1 - vi0, 1)
lines["round"] = perf_line(p0, perf_snap())
for k, v in res.items():
    print("%-8s %.1f fps" % (k, v))
    if lines.get(k):
        print(lines[k])
print("round per-50-frame fps:", " ".join("%.0f" % v for v in per))
print("still in round:", "Game_Main" in task_funcs())
if PROF:
    s = dc.prof_stop()
    print("profile samples:", len(s))
    print(dc.prof_report(s, 40))
if hf:
    hf.close()
