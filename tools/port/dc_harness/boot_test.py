#!/usr/bin/env python3
"""Boot regression test for the Vivid Dolls Dreamcast port (headless Flycast).

  python3 -I tools/port/dc_harness/boot_test.py [build/dc]   (needs the .cdi and .elf there)

Scripted path: boot logo -> title -> X (coin) -> Start -> girl select -> A -> stage intro
-> first round -> move around -> A+B+X+Y+Start (soft reset) -> boot logo again.
Progress is checked only through game state read from emulator RAM (task list, overlay,
globals) and frame hashes / non-black pixel counts: no frame after the boot logo is ever
written to a file (photo rule, PORT_BRIEF.md). --save-logo saves one boot-logo frame.
Exit status 0 = every stage reached.
"""
import os
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dcrun import DC, OUT  # noqa: E402

BUILD = sys.argv[1] if len(sys.argv) > 1 and not sys.argv[1].startswith("-") else "build/dc"
SAVE_LOGO = "--save-logo" in sys.argv

dc = DC(os.path.join(BUILD, "vividdolls.cdi"), serial=os.path.join(OUT, "serial.log"))
dc.load_syms(os.path.join(BUILD, "vividdolls.elf"))
ok = True


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


def wait_for(desc, cond, limit, pad=()):
    global ok
    for _ in range(limit):
        if cond():
            vi, built = counters()
            print("  OK   %-34s frame %5d  vblank %5d  frames built %5d  tasks %s" %
                  (desc, dc.frame, vi, built, ",".join(sorted(set(task_funcs())))), flush=True)
            return True
        step(1, pad)
    vi, built = counters()
    print("  FAIL %-34s frame %5d  vblank %5d  frames built %5d  tasks %s\n       %s" %
          (desc, dc.frame, vi, built, ",".join(task_funcs()), dc.regs_str()), flush=True)
    ok = False
    return False


def has(*names):
    return lambda: any(f in names for f in task_funcs())


def player():
    p = dc.u32("ovl2_D_800FD3F8")
    return (dc.s32(p + 4), dc.s32(p + 8)) if p else None


def fps_over(n, pad=()):
    vi0, b0 = counters()
    step(n, pad)
    vi1, b1 = counters()
    return 60.0 * (b1 - b0) / max(vi1 - vi0, 1)


print("Vivid Dolls boot test:", BUILD)
wait_for("boot logo (Boot_Logo task)", has("Boot_Logo"), 300)
step(60)
st = dc.frame_stats()
print("       boot-logo frame: hash %s, %d non-black pixels" % (st[0], st[1]))
if SAVE_LOGO:
    dc.save_ppm(os.path.join(OUT, "boot_logo.ppm"))
    print("       saved", os.path.join(OUT, "boot_logo.ppm"))
wait_for("title screen (Title_Main)", has("Title_Main", "title_slide"), 900)
print("       title: %.1f fps, frame non-black %d" % (fps_over(120), dc.frame_stats()[1]))
step(8, ["X"])
wait_for("coin -> 1 credit", lambda: dc.s32("gCredits") >= 1, 120)
step(30)
step(8, ["START"])
wait_for("girl select (ovl3)", has("GirlSelect_Task"), 600, ())
step(120)
step(8, ["A"])
wait_for("stage intro (StageIntro_Main)", has("StageIntro_Main"), 600)
wait_for("first round (Game_Main)", has("Game_Main"), 900)
step(200)
p0, t0 = player(), dc.s32("ovl2_D_800FD424")
f = 0.0
for d in (["RIGHT", "A"], ["DOWN", "A"], ["LEFT", "A"], ["UP", "A"]):
    f += fps_over(40, d) / 4
p1, t1 = player(), dc.s32("ovl2_D_800FD424")
moved = p0 != p1 and t1 < t0
print("  %s   round runs: player %s -> %s, timer %d -> %d, %.1f fps" %
      ("OK  " if moved else "FAIL", p0, p1, t0, t1, f))
ok = ok and moved
step(10, ["A", "B", "X", "Y", "START"])
wait_for("soft reset -> boot logo", has("Boot_Logo"), 300)
print("PASS" if ok else "FAILED")
sys.exit(0 if ok else 1)
