#!/usr/bin/env python3
"""Differential test for src/game/sprite.c + siff.c: load every sprite asset with
the original code and with the C version, and compare the entire game heap."""
import hashlib, os, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env, reset_heap, GAME_HEAP, GAME_HEAP_SIZE
from mipsbuild import build, CODE_BASE, ROOT
sys.path.insert(0, os.path.join(ROOT, 'tools'))
import assets as A

ORIG = {'Sprite_New': 0x800128A8, 'Sprite_NewPlain': 0x80012820,
        'Sprite_NewSharedPalette': 0x80012930, 'Sprite_Delete': 0x80012A38,
        'Asset_LoadLzss': 0x8001BC30}


def heap_state(emu):
    return emu.read(GAME_HEAP, GAME_HEAP_SIZE)


def run(emu, fns, fn, rom, delete=False):
    reset_heap(emu)
    obj, _ = emu.call(fns[fn], a=[0, rom])
    after_new = heap_state(emu)
    if delete:
        emu.call(fns['Sprite_Delete'], a=[obj])
    return obj, after_new, heap_state(emu)


def main():
    blob, syms = build([os.path.join(ROOT, f'src/game/{f}.c') for f in ('vd_math', 'matrix', 'heap', 'siff', 'sprite')], name='sprite')
    emu = make_env()
    emu.write(CODE_BASE, blob)
    rom = open(os.path.join(ROOT, 'baserom.z64'), 'rb').read()
    sprites = [(i, o) for i, o, comp, d in A.assets(rom) if b'Sp3d' in A.chunks(d)]
    fails = 0
    for n, (i, off) in enumerate(sprites):
        fn = ['Sprite_New', 'Sprite_NewPlain', 'Sprite_NewSharedPalette'][n % 3]
        a = run(emu, ORIG, fn, off, delete=True)
        b = run(emu, syms, fn, off, delete=True)
        if a != b:
            fails += 1
            if fails <= 3:
                if a[0] != b[0]:
                    print(f'asset {i} @{off:06X} {fn}: returned {a[0]:#x} vs {b[0]:#x}')
                else:
                    x, y = a[1], b[1]
                    d = next(k for k in range(len(x)) if x[k] != y[k]) if x != y else None
                    if d is not None:
                        print(f'asset {i} @{off:06X} {fn}: heap differs at {GAME_HEAP + d:#x}: '
                              f'orig {x[d & ~3:(d & ~3) + 16].hex()} C {y[d & ~3:(d & ~3) + 16].hex()}')
                    else:
                        print(f'asset {i} @{off:06X} {fn}: differs after Sprite_Delete')
    print(f'sprites: {len(sprites) - fails}/{len(sprites)} identical (heap compared after load and after delete)')
    # LZSS loader on every asset (sprites and photos)
    bad = 0
    allassets = [o for i, o, comp, d in A.assets(rom) if comp]
    for off in allassets:
        res = []
        for fns in (ORIG, syms):
            reset_heap(emu)
            p, _ = emu.call(fns['Asset_LoadLzss'], a=[off])
            res.append((p, hashlib.sha1(heap_state(emu)).hexdigest()))
        bad += res[0] != res[1]
    print(f'Asset_LoadLzss: {len(allassets) - bad}/{len(allassets)} identical')
    return 1 if fails or bad else 0


if __name__ == '__main__':
    sys.exit(main())
