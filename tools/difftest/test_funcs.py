#!/usr/bin/env python3
"""Generic differential test: call original and C (both MIPS, in the emulator)
with random float buffers / numbers and compare return values and memory.

Signature letters:  p = pointer to a 64-float buffer   P = same buffer as the previous 'p'
                    A = 4x4 affine matrix buffer   g = float in FPU reg   n = signed int   k = 0..2
                    f = float  i = small int  F = float return  I = int return  v = void
"""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env
from mipsbuild import build, CODE_BASE, ROOT
from draft_mips import build_clean

BUFS = 0x80600000
fb = lambda v: struct.unpack('>I', struct.pack('>f', v))[0]

SPECS = {
    # name: (original address, args, return)
    'Mtx_Mul':              (0x80006038, 'ppp', 'v'),
    'Mtx_Mul#alias_a':      (0x80006038, 'pPp', 'v'),
    'Mtx_Mul#alias_b':      (0x80006038, 'ppP', 'v'),
    'Mtx_MulAffine':        (0x80006778, 'ppp', 'v'),
    'Mtx_MulAffine#alias':  (0x80006778, 'pPp', 'v'),
    'Mtx_TransformPoint':   (0x80006958, 'ppp', 'v'),
    'Mtx_TransformVector':  (0x800069EC, 'ppp', 'v'),
    'Mtx_ProjectOntoPlane': (0x80006A68, 'pp', 'I'),
    'Mtx_Translate':        (0x80007308, 'pfff', 'v'),
    'Mtx_Scale':            (0x8000735C, 'pfff', 'v'),
    'Mtx_Identity':         (0x800078C4, 'p', 'v'),
    'Mtx_Copy':             (0x80007918, 'pp', 'v'),
}

LIBM_SPECS = {
    'vd_abs':          (0x80003E10, 'n', 'I'),
    'vd_fabsf':        (0x80003E24, 'g', 'F'),
    'vd_frexpf':       (0x80003E4C, 'gp', 'F'),
    'vd_ldexpf':       (0x80003EBC, 'gn', 'F'),
    'vd_expf':         (0x80003F08, 'g', 'F'),
    'vd_expf_cf':      (0x80004034, 'g', 'F'),
    'vd_logf':         (0x80004130, 'g', 'F'),
    'vd_logf_cf':      (0x800042AC, 'g', 'F'),
    'vd_powif':        (0x80004434, 'gn', 'F'),
    'vd_powf':         (0x80004488, 'gg', 'F'),
    'vd_powf_series':  (0x8000468C, 'gg', 'F'),
    'vd_atanf':        (0x80004708, 'g', 'F'),
    'vd_atan2f_pos':   (0x80004838, 'gg', 'F'),
    'vd_tan_reduce':   (0x80004A50, 'gp', 'F'),
    'vd_tanf':         (0x80004B14, 'g', 'F'),
    'vd_sinf':         (0x80004C3C, 'g', 'F'),
    'vd_cosf':         (0x80004D44, 'g', 'F'),
    'vd_sqrtf_newton': (0x80004D90, 'g', 'F'),
    'vd_cbrtf':        (0x80004E04, 'g', 'F'),
    'vd_csqrt':        (0x80004EA4, 'ggp', 'I'),
    'vd_ccbrt':        (0x80004F80, 'ggp', 'I'),
    'vd_cdiv':         (0x8000512C, 'gg' + 'ffpp', 'I'),
    'vd_mat3_swap_rows': (0x80005270, 'p' + 'kkk', 'v'),
    'vd_mat3_eliminate': (0x80005360, 'ppk', 'I'),
    'vd_mat3_invert':    (0x80005684, 'pp', 'I'),
    'vd_mtx_normalize_w': (0x80005870, 'pppp', 'I'),
    'vd_mtx_invert': (0x80005A58, 'pp', 'I'),
    'vd_mtx_invert#affine': (0x80005A58, 'pA', 'I'),
    'vd_mtx_mul4':   (0x80005FC0, 'ppp', 'v'),
}

PLACE_SPECS = {
    'Sprite_Place':         (0x80012A94, 'ppffff', 'v'),
    'Sprite_Place2':        (0x80012B38, 'ppffff', 'v'),
    'Sprite_PlaceCustom':   (0x80012BDC, 'ipp' + 'ffff', 'v'),
}


def rf(rng):
    r = rng.random()
    return 0.0 if r < 0.05 else rng.choice([1.0, -1.0, 0.5]) if r < 0.1 else rng.uniform(-8, 8)


def call(emu, addr, spec, seed):
    rng = random.Random(seed)
    args, nbuf, last = [], 0, None
    fregs = []
    for c in spec:
        if c == 'g':                       # float in f12/f14 (leading float args)
            v = rf(rng) * rng.choice([1, 1, 10, 0.01, 1000]); fregs.append(v); args.append(0); continue
        if c == 'k':                       # small index 0..2
            args.append(rng.randrange(0, 3)); continue
        if c == 'n':                       # int that may be negative
            args.append(rng.randrange(-40, 40) & 0xFFFFFFFF); continue
        if c in 'pA':
            last = BUFS + 0x200 * nbuf; nbuf += 1
            vals = [rf(rng) for _ in range(64)]
            if c == 'A':                   # affine 4x4: column 3 = (0, 0, 0, 1)
                vals[3] = vals[7] = vals[11] = 0.0; vals[15] = 1.0
            emu.write(last, b''.join(struct.pack('>f', v) for v in vals))
            args.append(last)
        elif c == 'P':
            args.append(last)
        elif c == 'f':
            args.append(fb(rf(rng)))       # floats after a pointer travel in integer registers / stack
        elif c == 'i':
            args.append(rng.randrange(0, 1024))
    v0, f0 = emu.call(addr, a=args, f=fregs)
    return v0, emu.f0_bits(), emu.read(BUFS, 0x200 * max(nbuf, 1))


def main(sources, specs, trials=300):
    blob, syms = build_clean(sources, name='funcs')
    emu = make_env(); emu.write(CODE_BASE, blob)
    bad_total = 0
    for name, (addr, args, ret) in specs.items():
        cname = name.split('#')[0]
        if cname not in syms:
            print(f'{name:26s} missing in C build'); bad_total += 1; continue
        bad = 0
        for s in range(trials):
            a = call(emu, addr, args, s)
            b = call(emu, syms[cname], args, s)
            if ret == 'v': a, b = a[2:], b[2:]
            elif ret == 'I': a, b = (a[0], a[2]), (b[0], b[2])
            else: a, b = (a[1], a[2]), (b[1], b[2])
            bad += a != b
        print(f'{name:26s} {"PASS" if not bad else "FAIL"} {trials - bad}/{trials}')
        bad_total += bad
    return 1 if bad_total else 0


if __name__ == '__main__':
    only = sys.argv[1] if len(sys.argv) > 1 else None
    if only == 'libm':
        sys.exit(main(['src/game/libm.c'], LIBM_SPECS))
    r = main(['src/game/vd_math.c', 'src/game/matrix.c'], SPECS)
    r |= main(['src/game/libm.c'], LIBM_SPECS)
    r |= main(['src/game/vd_math.c', 'src/game/matrix.c', 'src/game/heap.c', 'src/game/siff.c',
               'src/game/sprite.c'], PLACE_SPECS)
    sys.exit(r)
