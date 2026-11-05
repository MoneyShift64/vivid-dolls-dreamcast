#!/usr/bin/env python3
"""Differential test for screen sprites: Sprite_NewScreen + Sprite_DrawScreen."""
import os, random, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env, reset_heap, GAME_HEAP, GAME_HEAP_SIZE
from mipsbuild import build, CODE_BASE, ROOT

ORIG = {'Sprite_NewScreen': 0x800129B8, 'Sprite_DrawScreen': 0x80013D78, 'Sprite_PlaceScreen': 0x80012C84}
NODE = 0x80600000
SCREEN_W = 0x800C30D0


def session(emu, fns, seed):
    rng = random.Random(seed)
    emu.w32(SCREEN_W, rng.choice([320, 640]))
    reset_heap(emu)
    spr, _ = emu.call(fns['Sprite_NewScreen'], a=[0, 0x669DC])
    states = [spr, emu.read(GAME_HEAP, GAME_HEAP_SIZE)]
    for _ in range(20):
        emu.w32(SCREEN_W, rng.choice([320, 640]))
        x = rng.choice([0.0, rng.uniform(-400, 400), -0.3, 100.5])
        y = rng.choice([0.0, rng.uniform(-300, 300), 2.25])
        sx = rng.choice([1.0, 0.5, 2.0, rng.uniform(0.1, 4), -1.0])
        sy = rng.choice([1.0, 0.75, rng.uniform(0.1, 4), -0.5])
        emu.call(fns['Sprite_DrawScreen'], a=[spr], stack=[], f=[]) if False else None
        # o32: (ptr, f32, f32, f32, f32) -> a0, a1..a3 as float bits, 5th on the stack
        import struct
        b = lambda v: struct.unpack('>I', struct.pack('>f', v))[0]
        if rng.random() < 0.5:
            emu.call(fns['Sprite_DrawScreen'], a=[spr, b(x), b(y), b(sx), b(sy)])
        else:
            emu.write(NODE, bytes(0x100))
            emu.call(fns['Sprite_PlaceScreen'], a=[spr, NODE, b(x), b(y), b(sx), b(sy)])
            states.append(emu.read(NODE, 0x100))
        states.append(emu.read(GAME_HEAP, GAME_HEAP_SIZE))
    return states


def main():
    blob, syms = build([os.path.join(ROOT, f'src/game/{f}.c') for f in ('vd_math', 'matrix', 'heap', 'siff', 'sprite')], name='screen')
    emu = make_env(); emu.write(CODE_BASE, blob)
    bad = 0
    for s in range(15):
        a, b = session(emu, ORIG, s), session(emu, syms, s)
        if a != b:
            bad += 1
            for k, (x, y) in enumerate(zip(a, b)):
                if x != y:
                    if isinstance(x, int): print(f'seed {s} step {k}: {x:#x} vs {y:#x}'); break
                    d = next(i for i in range(len(x)) if x[i] != y[i])
                    print(f'seed {s} step {k}: heap differs at {GAME_HEAP + d:#x}: orig {x[d & ~7:(d & ~7) + 16].hex()} C {y[d & ~7:(d & ~7) + 16].hex()}')
                    break
    print(f'screen sprite sessions: {15 - bad}/15 identical (load + 20 random redraws each)')
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
