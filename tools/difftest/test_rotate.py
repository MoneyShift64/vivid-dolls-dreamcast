#!/usr/bin/env python3
"""Differential test for src/game/rotate.c (RAM 0x80006C10-0x800088E0): view /
projection matrices, rotations, matrix inversion. Calls the original and the C
version (both MIPS, in the emulator) with identical random inputs and compares
return values and every buffer.

Signature letters:  p = 64-float buffer (random)   P = same buffer as the previous one
                    u = buffer holding unit vectors (at 0, 3, 6, ...)
                    Z = 4x4 matrix with some zero entries (exercises pivoting)
                    S = 4x4 matrix with a whole zero column (singular)
                    f = float (integer register / stack)   d = angle in degrees
                    v = void return   I = int return
"""
import math, os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env
from mipsbuild import CODE_BASE
from draft_mips import build_clean

BUFS = 0x80600000
NBUF = 6
fb = lambda v: struct.unpack('>I', struct.pack('>f', v))[0]

SPECS = {
    'Mtx_LookAt':              (0x80006C10, 'pfffffffff', 'v'),
    'Mtx_Perspective':         (0x80006EA8, 'pdfff', 'v'),
    'Mtx_Ortho':               (0x80006F74, 'pfffffff', 'v'),
    'Mtx_RotateAxisSinCos':    (0x800070A4, 'puff', 'v'),
    'Mtx_RotateAxisSinCos#alias': (0x800070A4, 'pPff', 'v'),
    'Mtx_RotateAxisDeg':       (0x800071CC, 'pdfff', 'v'),
    'Mtx_Invert':              (0x800073B0, 'pp', 'I'),
    'Mtx_Invert#zeros':        (0x800073B0, 'pZ', 'I'),
    'Mtx_Invert#singular':     (0x800073B0, 'pS', 'I'),
    'Mtx_InvertGaussJordan':   (0x80007420, 'pp', 'v'),
    'Mtx_InvertGaussJordan#zeros': (0x80007420, 'pZ', 'v'),
    'Mtx_InvertGaussJordan#alias': (0x80007420, 'pP', 'v'),
    'Mtx_InvertPermuted':      (0x80007558, 'pp', 'I'),
    'Mtx_InvertPermuted#zeros': (0x80007558, 'pZ', 'I'),
    'Mtx_InvertPermuted#singular': (0x80007558, 'pS', 'I'),
    'Mtx_InvertPermuted#alias': (0x80007558, 'pP', 'I'),
    'Vec3_RotateFromTo':       (0x8000793C, 'ppp', 'v'),
    'Mtx_RotateBetweenUnit':   (0x80007A08, 'puu', 'I'),
    'Mtx_RotateBetweenUnit#raw': (0x80007A08, 'ppp', 'I'),
    'Mtx_RotateBetween':       (0x80007D28, 'ppp', 'I'),
    'Mtx_BasisChange':         (0x80007D84, 'ppppp', 'I'),
    'Mtx_BasisChange#unit':    (0x80007D84, 'puuuu', 'I'),
    'Mtx_RotateAxisRad':       (0x80008190, 'puf', 'v'),
    'Mtx_PreRotateX':          (0x800082DC, 'ppd', 'v'),
    'Mtx_PreRotateX#alias':    (0x800082DC, 'pPd', 'v'),
    'Mtx_PreRotateY':          (0x800083A8, 'ppd', 'v'),
    'Mtx_PreRotateY#alias':    (0x800083A8, 'pPd', 'v'),
    'Mtx_PreRotateZ':          (0x80008474, 'ppd', 'v'),
    'Mtx_PreRotateZ#alias':    (0x80008474, 'pPd', 'v'),
    'Mtx_PreRotateYXZ':        (0x80008540, 'ppddd', 'v'),
    'Mtx_PreRotateYXZ#alias':  (0x80008540, 'pPddd', 'v'),
    'Mtx_PreTranslate':        (0x800085B4, 'ppfff', 'v'),
    'Mtx_PreTranslate#alias':  (0x800085B4, 'pPfff', 'v'),
    'Mtx_PreTranslateX':       (0x80008624, 'ppf', 'v'),
    'Mtx_PreTranslateX#alias': (0x80008624, 'pPf', 'v'),
    'Mtx_PreTranslateY':       (0x80008688, 'ppf', 'v'),
    'Mtx_PreTranslateY#alias': (0x80008688, 'pPf', 'v'),
    'Mtx_PreTranslateZ':       (0x800086EC, 'ppf', 'v'),
    'Mtx_PreTranslateZ#alias': (0x800086EC, 'pPf', 'v'),
    'Mtx_PreScale':            (0x80008750, 'ppfff', 'v'),
    'Mtx_PreScale#alias':      (0x80008750, 'pPfff', 'v'),
    'Mtx_PreScaleX':           (0x800087BC, 'ppf', 'v'),
    'Mtx_PreScaleY':           (0x80008818, 'ppf', 'v'),
    'Mtx_PreScaleZ':           (0x80008874, 'ppf', 'v'),
    'Mtx_PreScaleZ#alias':     (0x80008874, 'pPf', 'v'),
    'Rotate_EmptyA':           (0x800088D0, 'p', 'v'),
    'Rotate_EmptyB':           (0x800088D8, 'p', 'v'),
}


def rf(rng):
    r = rng.random()
    return 0.0 if r < 0.05 else rng.choice([1.0, -1.0, 0.5]) if r < 0.1 else rng.uniform(-8, 8)


def unit(rng):
    while True:
        v = [rng.uniform(-1, 1) for _ in range(3)]
        n = math.sqrt(sum(x * x for x in v))
        if n > 0.1:
            return [x / n for x in v]


def call(emu, addr, spec, seed):
    rng = random.Random(seed)
    emu.write(BUFS, bytes(0x100 * NBUF))
    args, nbuf, last, prev_u = [], 0, None, None
    for c in spec:
        if c in 'puZS':
            last = BUFS + 0x100 * nbuf; nbuf += 1
            vals = [rf(rng) for _ in range(64)]
            if c == 'u':
                vals = []
                for _ in range(21):
                    vals += unit(rng)
                vals.append(0.0)
                if prev_u is not None and rng.random() < 0.15:   # parallel / anti-parallel
                    k = rng.choice([1.0, -1.0])
                    vals[0:3] = [k * x for x in prev_u[0:3]]
                prev_u = vals
            elif c == 'Z':
                for i in rng.sample(range(16), rng.randrange(1, 6)):
                    vals[i] = 0.0
            elif c == 'S':
                col = rng.randrange(4)
                for r in range(4):
                    vals[4 * r + col] = 0.0
            emu.write(last, b''.join(struct.pack('>f', v) for v in vals))
            args.append(last)
        elif c == 'P':
            args.append(last)
        elif c == 'f':
            args.append(fb(rf(rng)))
        elif c == 'd':
            args.append(fb(rng.choice([0.0, 90.0, -45.0, 180.0]) if rng.random() < 0.1
                           else rng.uniform(-400, 400)))
    v0, _ = emu.call(addr, a=args)
    return v0, emu.read(BUFS, 0x100 * NBUF)


def main(trials=400):
    blob, syms = build_clean(['src/game/rotate.c', 'src/game/matrix.c', 'src/game/vd_math.c'],
                             name='rotate')
    emu = make_env(); emu.write(CODE_BASE, blob)
    bad_total = 0
    for name, (addr, args, ret) in SPECS.items():
        cname = name.split('#')[0]
        if cname not in syms:
            print(f'{name:32s} missing in C build'); bad_total += 1; continue
        bad = 0
        for s in range(trials):
            a = call(emu, addr, args, s)
            b = call(emu, syms[cname], args, s)
            if ret == 'v':
                a, b = a[1], b[1]
            bad += a != b
        print(f'{name:32s} {"PASS" if not bad else "FAIL"} {trials - bad}/{trials}')
        bad_total += bad
    return 1 if bad_total else 0


if __name__ == '__main__':
    sys.exit(main())
