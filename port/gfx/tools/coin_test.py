#!/usr/bin/env python3
"""Coin input test: on the title screen, single X presses of 2, 3 and 6 frames (33 ms+; a 1-frame 16 ms tap can fall between two KOS maple polls)
must each add exactly one credit; also reports the frame rate during the test.
  python3 -I port/gfx/tools/coin_test.py BUILD_DIR      (exit 0 = PASS)"""
import os
import sys
sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "tools", "port", "dc_harness"))
from dcrun import DC, OUT  # noqa: E402

B = sys.argv[1]
dc = DC(os.path.join(B, "vividdolls.cdi"), serial=os.path.join(OUT, "serial_coin.log"))
dc.load_syms(os.path.join(B, "vividdolls.elf"))


def tf():
    head = dc.u32("D_800B05A4"); tail = dc.u32("D_800C53FC"); names = []
    t = dc.u32(head) if head else 0; n = 0
    while t and t != tail and n < 64:
        sp = dc.u32(t + 8); names.append(dc.func_of(dc.u32(sp + 0x3C)) if sp else "?"); t = dc.u32(t); n += 1
    return names


def step(n, pad=()):
    for _ in range(n):
        dc.set_pad(0, list(pad)); dc.run(1)


for _ in range(1200):
    if any(f in ("Title_Main", "title_slide") for f in tf()):
        break
    step(1)
step(120)
ok = True
DURS = [int(x) for x in sys.argv[2].split(",")] if len(sys.argv) > 2 else (2, 2, 3, 6, 2, 2)
vi0, b0 = dc.u32("vd_vi_retrace_count"), dc.u32("D_800C1650")
for dur in DURS:
    c0 = dc.s32("gCredits")
    step(dur, ["X"])
    step(90)
    c1 = dc.s32("gCredits")
    good = c1 - c0 == 1
    ok &= good
    print("  X held %d frame(s): credits %d -> %d %s" % (dur, c0, c1, "OK" if good else "FAIL"))
vi1, b1 = dc.u32("vd_vi_retrace_count"), dc.u32("D_800C1650")
print("  frame rate during the test: %.1f fps" % (60.0 * (b1 - b0) / max(vi1 - vi0, 1)))
print("PASS" if ok else "FAILED")
sys.exit(0 if ok else 1)
