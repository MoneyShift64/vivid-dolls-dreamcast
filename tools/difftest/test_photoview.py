#!/usr/bin/env python3
"""Differential test for src/game/photoview.c (photo scrolling, mosaic, rect copy)."""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env, reset_heap, GAME_HEAP, GAME_HEAP_SIZE
from mipsbuild import build, CODE_BASE, ROOT

ORIG = {'PhotoView_New': 0x800145F0, 'PhotoView_Delete': 0x800146B8, 'PhotoView_SetScroll': 0x80014714,
        'Image_Mosaic': 0x80014A00, 'Image_CopyRect': 0x80014DD8}
IMG, IMG2 = 0x80600000, 0x80400000
fb = lambda v: struct.unpack('>I', struct.pack('>f', v))[0]


def views(emu, fns, seed):
    rng = random.Random(seed)
    reset_heap(emu)
    w, h = rng.choice([(320, 240), (320, 480), (640, 240)])
    v, _ = emu.call(fns['PhotoView_New'], a=[0, IMG, w, h, rng.randrange(-5, w), rng.randrange(-5, h)])
    out = [v, emu.read(GAME_HEAP, GAME_HEAP_SIZE)]
    for _ in range(10):
        emu.call(fns['PhotoView_SetScroll'], a=[v, rng.randrange(-50, w + 50), rng.randrange(-50, h + 50)])
        out.append(emu.read(GAME_HEAP, GAME_HEAP_SIZE))
    emu.call(fns['PhotoView_Delete'], a=[v])
    out.append(emu.read(GAME_HEAP, GAME_HEAP_SIZE))
    return out


def mosaic(emu, fns, seed):
    rng = random.Random(seed)
    pix = bytes(rng.randrange(256) for _ in range(640 * 520 * 2))
    emu.write(IMG, pix)
    stride = rng.choice([320, 640])
    x, y = rng.randrange(0, 100), rng.randrange(0, 100)
    w, h = rng.randrange(0, 200), rng.randrange(0, 200)
    level = rng.randrange(0, 8)
    bright = rng.choice([1.0, 0.5, 0.0, 1.5, rng.uniform(0, 2), -0.5])
    emu.call(fns['Image_Mosaic'], a=[IMG, stride, 0, x], stack=[y, w, h, level, fb(bright)])
    return emu.read(IMG, 640 * 520 * 2)


def copyrect(emu, fns, seed):
    rng = random.Random(seed)
    emu.write(IMG, bytes(rng.randrange(256) for _ in range(640 * 480 * 2)))
    emu.write(IMG2, bytes(640 * 480 * 2))
    args = [IMG, rng.choice([320, 640]), 0, rng.randrange(0, 100)]
    stack = [rng.randrange(0, 100), rng.randrange(0, 200), rng.randrange(0, 200), IMG2, rng.choice([320, 640]), 0,
             rng.randrange(0, 100), rng.randrange(0, 100)]
    emu.call(fns['Image_CopyRect'], a=args, stack=stack)
    return emu.read(IMG2, 640 * 480 * 2)


def main():
    blob, syms = build([os.path.join(ROOT, f'src/game/{f}.c') for f in ('vd_math', 'matrix', 'heap', 'siff', 'sprite', 'photoview')],
                       name='photoview')
    emu = make_env(); emu.write(CODE_BASE, blob)
    emu.write(IMG, bytes(range(256)) * (640 * 480 * 2 // 256))
    total = 0
    for name, fn, n in (('photo views', views, 20), ('mosaic', mosaic, 40), ('rect copy', copyrect, 30)):
        bad = sum(fn(emu, ORIG, s) != fn(emu, syms, s) for s in range(n))
        print(f'{name}: {n - bad}/{n} identical')
        total += bad
    return 1 if total else 0


if __name__ == '__main__':
    sys.exit(main())
