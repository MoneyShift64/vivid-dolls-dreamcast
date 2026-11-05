#!/usr/bin/env python3
"""Display-list statistics of one in-round frame with many live effects (headless Flycast).

  python3 -I tools/port/dc_harness/dl_stats.py [build/dc] [--min-fx N]

Boots into the first round (girl 0, spinning boss), makes the boss attack whenever it
can (as perf_cases.py proj), waits until N effects are live, then walks the frame's
display list from emulator RAM (G_DL calls followed) and prints the opcode histogram
and one sample command sequence per distinct 'object' (run of commands between
G_MTX loads). Only command words are read: no texture or photo data.
"""
import os
import struct
import sys
from collections import Counter

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dcrun import DC, OUT  # noqa: E402

args = sys.argv[1:]
BUILD = args[0] if args and not args[0].startswith("-") else "build/dc"
MINFX = int(args[args.index("--min-fx") + 1]) if "--min-fx" in args else 24
dc = DC(os.path.join(BUILD, "vividdolls.cdi"), serial=os.path.join(OUT, "serial_dl.log"), log=os.devnull)
dc.load_syms(os.path.join(BUILD, "vividdolls.elf"))
GT = dc.syms["D_800B0730"]
NAMES = {0x01: "MTX", 0x03: "MOVEMEM", 0x04: "VTX", 0x06: "DL", 0xBF: "TRI1", 0xBC: "MOVEWORD", 0xBB: "TEXTURE",
         0xBA: "SETOTHERMODE_H", 0xB9: "SETOTHERMODE_L", 0xB8: "ENDDL", 0xB7: "SETGEOM", 0xB6: "CLRGEOM",
         0xB4: "RDPHALF_1", 0xB3: "RDPHALF_2", 0xE4: "TEXRECT", 0xE6: "LOADSYNC", 0xE7: "PIPESYNC", 0xE8: "TILESYNC",
         0xE9: "FULLSYNC", 0xF0: "LOADTLUT", 0xF2: "SETTILESIZE", 0xF3: "LOADBLOCK", 0xF4: "LOADTILE", 0xF5: "SETTILE",
         0xF6: "FILLRECT", 0xF7: "SETFILLCOLOR", 0xF8: "SETFOGCOLOR", 0xF9: "SETBLENDCOLOR", 0xFA: "SETPRIMCOLOR",
         0xFB: "SETENVCOLOR", 0xFC: "SETCOMBINE", 0xFD: "SETTIMG", 0xFE: "SETZIMG", 0xFF: "SETCIMG", 0x00: "SPNOOP"}


def task_funcs():
    head = dc.u32("D_800B05A4")
    tail = dc.u32("D_800C53FC")
    names, t, n = [], (dc.u32(head) if head else 0), 0
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


def wait(cond, limit, pad=()):
    for _ in range(limit):
        if cond():
            return True
        step(1, pad)
    return False


def effects_live():
    raw = dc.read(dc.addr("ovl2_D_800FBA90"), 64 * 0x4C)
    return sum(1 for i in range(64) if struct.unpack_from("<i", raw, i * 0x4C)[0] != 0)


def walk(dl):
    out, stack, pc = [], [], dl
    while len(out) < 20000:
        w0, w1 = struct.unpack("<II", dc.read(pc, 8))
        op = w0 >> 24
        out.append((op, w0, w1))
        pc += 8
        if op == 0x06:
            if not (w0 >> 16) & 0xFF:
                stack.append(pc)
            pc = w1
        elif op == 0xB8:
            if not stack:
                break
            pc = stack.pop()
        elif op in (0xE4, 0xE5):
            pass
    return out


wait(lambda: "Boot_Logo" in task_funcs(), 300)
wait(lambda: any(f in ("Title_Main", "title_slide") for f in task_funcs()), 900)
step(60); step(8, ["X"]); step(30); step(8, ["START"])
wait(lambda: "GirlSelect_Task" in task_funcs(), 600)
step(120); step(8, ["A"])
wait(lambda: "Game_Main" in task_funcs(), 2500)
step(120)
for i in range(16):
    dc.write(dc.addr("ovl2_D_800F9D00") + 4 * i, struct.pack("<i", 1))
moves = (["RIGHT"], ["DOWN"], ["LEFT"], ["UP"])
k = 0
while effects_live() < MINFX and k < 3000:
    dc.write(dc.addr("D_800B04D4"), struct.pack("<i", 3))
    step(1, moves[(k // 40) % 4])
    k += 1
print("live effects:", effects_live(), "after", k, "frames")
for _ in range(30):
    step(1)
    buf = dc.u32("D_800C0AB8") ^ 1
    dl = dc.u32(GT + buf * 0x2180 + 0x40)
    if (dl >> 24) == 0x8C:
        break
cmds = walk(dl)
h = Counter(NAMES.get(op, "%02X" % op) for op, _, _ in cmds)
print("commands:", len(cmds))
print("  " + ", ".join("%s %d" % kv for kv in h.most_common()))
# objects: split at G_MTX
objs, cur = [], []
for c in cmds:
    if c[0] == 0x01 and cur:
        objs.append(cur)
        cur = []
    cur.append(c)
objs.append(cur)
sig = Counter(tuple(NAMES.get(op, "%02X" % op) for op, _, _ in o) for o in objs)
print("objects (runs between G_MTX):", len(objs), " distinct shapes:", len(sig))
for s, n in sig.most_common(6):
    print("  x%d (%d cmds): %s" % (n, len(s), " ".join(s)))
