#!/usr/bin/env python3
"""Differential test for src/game/heap.c: run the same random sequence of heap
operations through the original code and the C version (both as MIPS code in the
emulator, on the same memory) and compare every return value and the final memory."""
import os, random, sys
sys.path.insert(0, os.path.dirname(__file__))
from emu import N64Emu
from mipsbuild import build, CODE_BASE, ROOT

ORIG = {'Heap_Init': 0x80003300, 'Heap_Alloc': 0x80003358, 'Heap_Free': 0x800033D8,
        'Heap_AllocFront': 0x800034AC, 'Heap_TotalFree': 0x80003538, 'Heap_LargestFree': 0x80003564,
        'Math_Log2Ceil': 0x800035A0, 'Math_Log2Floor': 0x800035D0, 'Math_Log2Exact': 0x80003610}
HEAP, HEAP_SIZE = 0x80500000, 0x20000


def run_sequence(emu, fns, ops):
    emu.write(HEAP, bytes(HEAP_SIZE))
    emu.call(fns['Heap_Init'], a=[HEAP, HEAP_SIZE])
    live, rets = [], []
    for op, arg in ops:
        if op == 'alloc' or op == 'front':
            v0, _ = emu.call(fns['Heap_Alloc' if op == 'alloc' else 'Heap_AllocFront'], a=[HEAP, arg])
            if v0:
                live.append(v0)
                emu.write(v0, bytes([len(live) & 0xFF]) * min(arg, 64))   # scribble on the block
            rets.append(v0)
        elif op == 'free':
            if live and arg != 'bogus':
                p = live.pop(arg % len(live))
            else:
                p = HEAP + 0x1234
            v0, _ = emu.call(fns['Heap_Free'], a=[HEAP, p])
            rets.append(v0)
        elif op == 'total':
            rets.append(emu.call(fns['Heap_TotalFree'], a=[HEAP])[0])
        elif op == 'largest':
            rets.append(emu.call(fns['Heap_LargestFree'], a=[HEAP])[0])
    return rets, emu.read(HEAP, HEAP_SIZE)


def main():
    blob, syms = build([os.path.join(ROOT, 'src/game/heap.c')], name='heap')
    emu = N64Emu()
    emu.write(CODE_BASE, blob)
    rng = random.Random(7)
    fails = 0
    for trial in range(150):
        ops = []
        for _ in range(rng.randrange(5, 120)):
            r = rng.random()
            if r < 0.4: ops.append(('alloc', rng.choice([1, 8, 15, 16, 17, 100, 1000, rng.randrange(1, 9000)])))
            elif r < 0.55: ops.append(('front', rng.randrange(1, 5000)))
            elif r < 0.85: ops.append(('free', rng.randrange(1000)))
            elif r < 0.88: ops.append(('free', 'bogus'))
            elif r < 0.94: ops.append(('total', 0))
            else: ops.append(('largest', 0))
        a = run_sequence(emu, ORIG, ops)
        b = run_sequence(emu, syms, ops)
        if a != b:
            fails += 1
            if fails < 3:
                for i, (x, y) in enumerate(zip(a[0], b[0])):
                    if x != y: print(f'trial {trial} op {i} {ops[i]}: orig {x:#x} C {y:#x}'); break
                else:
                    print(f'trial {trial}: return values match, memory differs')
    print(f'heap sequences: {150 - fails}/150 identical')
    # log2 helpers
    bad = 0
    for x in list(range(0, 300)) + [rng.randrange(0, 1 << 31) for _ in range(300)] + [1 << k for k in range(31)]:
        for n in ('Math_Log2Ceil', 'Math_Log2Floor', 'Math_Log2Exact'):
            if emu.call(ORIG[n], a=[x])[0] != emu.call(syms[n], a=[x])[0]:
                bad += 1
    print(f'log2 helpers: {"PASS" if bad == 0 else f"{bad} mismatches"}')
    return 1 if fails or bad else 0


if __name__ == '__main__':
    sys.exit(main())
