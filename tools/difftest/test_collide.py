#!/usr/bin/env python3
"""Differential test of src/game/collide.c (matrix builders, B-spline helpers,
stack/ring pools, LCG random numbers, stubs) against the original MIPS code."""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env, reset_heap, GAME_HEAP, GAME_HEAP_SIZE
from mipsbuild import CODE_BASE
from draft_mips import build_clean

OUT = 0x80400000      # output matrix
IN = 0x80400100       # input matrix / vectors
VA = 0x80400200
VB = 0x80400210
RES = 0x80400300      # small result buffer
POOL = 0x80410000     # pool memory
POOL_MAX = 0x4000


def fb(v):
    return struct.unpack('>I', struct.pack('>f', v))[0]


def rf(rng, scale=100.0):
    r = rng.random()
    if r < 0.05:
        return 0.0
    if r < 0.08:
        return -0.0
    if r < 0.12:
        return float(rng.randrange(-4, 5))
    if r < 0.15:
        return rng.uniform(-1e-3, 1e-3)
    return rng.uniform(-scale, scale)


def fbytes(vals):
    return b''.join(struct.pack('>f', v) for v in vals)


# ---------------- per-function trial runners: return comparable tuples ----------------

def t_rows(emu, f, rng):
    vals = [rf(rng) for _ in range(16)]
    if rng.random() < 0.15:                       # a zero row
        r = rng.randrange(3); vals[r * 4:r * 4 + 3] = [0.0, 0.0, 0.0]
    emu.write(IN, fbytes(vals))
    emu.write(OUT, bytes(rng.getrandbits(8) for _ in range(64)))
    alias = rng.random() < 0.25
    emu.call(f, a=[IN if alias else OUT, IN])
    return emu.read(OUT, 64), emu.read(IN, 64)


def t_persp(emu, f, rng):
    fovy = rng.choice([rng.uniform(1, 170), 45.0, 60.0, 90.0, rf(rng, 400)])
    aspect = rng.choice([rng.uniform(0.2, 3), 4.0 / 3.0, rf(rng, 5)])
    near = rng.choice([rng.uniform(0.01, 50), 1.0, 10.0, 0.5, rf(rng, 30)])
    far = rng.choice([rng.uniform(50, 20000), 1000.0, near, 1.0, 1.5, 0.5, rf(rng, 1e5)])
    scale = rng.choice([1.0, rng.uniform(-3, 3), 0.0])
    emu.write(OUT, bytes(rng.getrandbits(8) for _ in range(64)))
    v0, _ = emu.call(f, a=[OUT, fb(fovy), fb(aspect), fb(near), fb(far), fb(scale)])
    return v0 & 0xFFFF, emu.read(OUT, 64)


def t_rotaxis(emu, f, rng):
    s, c = rf(rng, 1.0), rf(rng, 1.0)
    x, y, z = rf(rng), rf(rng), rf(rng)
    if rng.random() < 0.05:
        x = y = z = 0.0
    emu.write(OUT, bytes(rng.getrandbits(8) for _ in range(64)))
    emu.call(f, a=[OUT, fb(s), fb(c), fb(x), fb(y), fb(z)])
    return (emu.read(OUT, 64),)


def t_rotbetween(emu, f, rng):
    a = [rf(rng) for _ in range(3)]
    b = [rf(rng) for _ in range(3)]
    r = rng.random()
    if r < 0.1:
        b = [v * 2.0 for v in a]                 # parallel
    elif r < 0.15:
        b = [-v for v in a]                      # opposite
    elif r < 0.18:
        a = [0.0, 0.0, 0.0]
    emu.write(VA, fbytes(a)); emu.write(VB, fbytes(b))
    emu.write(OUT, bytes(rng.getrandbits(8) for _ in range(64)))
    emu.call(f, a=[OUT, VA, VB])
    return emu.read(OUT, 64), emu.read(VA, 32)


LOOKAT = 0x80400400


def t_lookat(emu, f, rng):
    eye = [rf(rng, 1000) for _ in range(3)]
    at = [rf(rng, 1000) for _ in range(3)]
    up = rng.choice([[0.0, 1.0, 0.0], [0.0, 0.0, 1.0], [rf(rng, 2) for _ in range(3)]])
    r = rng.random()
    if r < 0.05:
        at = list(eye)                            # degenerate: NaNs everywhere
    elif r < 0.1:
        at = [eye[0] + up[0], eye[1] + up[1] * 3, eye[2] + up[2]]   # look along up
    elif r < 0.2:
        at = [eye[0] + rng.choice([0.0, 1.0, -1.0]), eye[1], eye[2] + rng.choice([0.0, 5.0])]
    alias = rng.random() < 0.1
    emu.write(VA, fbytes(eye)); emu.write(VB, fbytes(at))
    emu.write(OUT, bytes(rng.getrandbits(8) for _ in range(64)))
    emu.write(LOOKAT, bytes(rng.getrandbits(8) for _ in range(32)))
    emu.call(f, a=[OUT, LOOKAT, VA, VA if alias else VB] + [fb(v) for v in up])
    return emu.read(OUT, 64), emu.read(LOOKAT, 32), emu.read(VA, 32)


def spline_pts(rng):
    r = rng.random()
    if r < 0.2:                                  # linear: a == 0
        p0 = rf(rng); d = rf(rng, 10)
        return p0, p0 + d, p0 + d + d
    if r < 0.25:
        p = rf(rng); return p, p, p
    return rf(rng), rf(rng), rf(rng)


def t_eval(emu, f, rng):
    t = rng.choice([rng.random(), 0.0, 1.0, 0.5, rf(rng, 2)])
    p0, p1, p2 = spline_pts(rng)
    emu.call(f, a=[0, 0, fb(p1), fb(p2)], f=[t, p0])
    return (emu.f0_bits(),)


def t_deriv(emu, f, rng):
    return t_eval(emu, f, rng)


def t_solve(emu, f, rng):
    p0, p1, p2 = spline_pts(rng)
    r = rng.random()
    if r < 0.5:                                  # x on the curve
        t = rng.random(); u = 1 - t
        x = (u * u / 2) * p0 + (u * t + 0.5) * p1 + (t * t / 2) * p2
    elif r < 0.6:
        x = (p0 + p1) / 2                        # c == 0
    else:
        x = rf(rng)
    emu.write(RES, b'\xcc' * 16)
    v0, _ = emu.call(f, a=[RES, fb(x), fb(p0), fb(p1), fb(p2)])
    return v0, emu.read(RES, 16)


SEEDS = [0, 1, -1, 0x7FFFFFFF, -0x80000000, 12345, 0x41C64E6D]


def t_rand(emu, f, rng):
    seed = rng.choice(SEEDS) if rng.random() < 0.1 else rng.getrandbits(32)
    emu.w32(RES, seed)
    v0, _ = emu.call(f, a=[RES])
    return v0, emu.read(RES, 4)


def t_randrange(emu, f, rng):
    seed = rng.choice(SEEDS) if rng.random() < 0.1 else rng.getrandbits(32)
    rngv = rng.choice([0, 1, 2, 10, 100, 0x8000, 0x10000, -5, rng.getrandbits(32), rng.randrange(1, 1000)])
    emu.w32(RES, seed)
    v0, _ = emu.call(f, a=[RES, rngv & 0xFFFFFFFF])
    return v0, emu.read(RES, 4)


def pool_params(rng, hdr):
    isz = rng.choice([0, 1, 4, 7, 8, 9, 16, 31, 64, rng.randrange(0, 300)])
    msz = rng.choice([hdr, hdr + 3, hdr + 4, hdr + 8 + isz, rng.randrange(hdr, POOL_MAX - 0x100)])
    return msz, isz


def t_poolinit(emu, f, rng, hdr):
    msz, isz = pool_params(rng, hdr)
    emu.write(POOL, b'\xcc' * POOL_MAX)
    emu.call(f, a=[POOL, msz, isz])
    return (emu.read(POOL, POOL_MAX),)


def t_pool_seq(emu, fns, rng):
    """fns: dict of Pool_Init/Take/CountFree/Give addresses."""
    msz, isz = pool_params(rng, 12)
    msz = min(msz, 0x800)
    emu.write(POOL, b'\xcc' * POOL_MAX)
    emu.call(fns['init'], a=[POOL, msz, isz])
    taken, rets = [], []
    for _ in range(rng.randrange(1, 60)):
        r = rng.random()
        if r < 0.45:
            v0, _ = emu.call(fns['take'], a=[POOL]); rets.append(('t', v0))
            if v0:
                taken.append(v0)
        elif r < 0.75:
            if taken:
                emu.call(fns['give'], a=[POOL, taken.pop(rng.randrange(len(taken)))])
        else:
            v0, _ = emu.call(fns['count'], a=[POOL]); rets.append(('c', v0))
    return rets, emu.read(POOL, POOL_MAX)


def t_ring_seq(emu, fns, rng):
    msz, isz = pool_params(rng, 20)
    msz = min(max(msz, 20 + 2 * (((isz + 7) & ~7) + 4)), 0x800)   # at least 2 slots
    emu.write(POOL, b'\xcc' * POOL_MAX)
    emu.call(fns['init'], a=[POOL, msz, isz])
    rets = []
    for _ in range(rng.randrange(1, 60)):
        r = rng.random()
        if r < 0.35:
            v0, _ = emu.call(fns['get'], a=[POOL]); rets.append(('g', v0))
        elif r < 0.65:
            emu.call(fns['put'], a=[POOL, rng.getrandbits(32) | 1])
        elif r < 0.85:
            v0, _ = emu.call(fns['count'], a=[POOL]); rets.append(('c', v0))
        else:
            emu.call(fns['stop'], a=[POOL])
    return rets, emu.read(POOL, 0x900)


def t_stub(emu, f, rng):
    emu.write(RES, b'\xcc' * 16)
    emu.call(f, a=[rng.getrandbits(32) for _ in range(4)])
    return (emu.read(RES, 16),)


SIMPLE = [
    ('Mtx_FromRowsNormalized', 0x800088E0, t_rows),
    ('Mtx_LookAtReflect', 0x800095B0, t_lookat),
    ('Mtx_PerspectiveG', 0x80009AD4, t_persp),
    ('Rand_Next', 0x80009CB0, t_rand),
    ('Rand_Range', 0x80009CE8, t_randrange),
    ('Mtx_RotateAxisSinCosG', 0x8000A0A0, t_rotaxis),
    ('Mtx_RotateBetweenG', 0x8000A1F4, t_rotbetween),
    ('BSpline2_Eval', 0x8000A3D0, t_eval),
    ('BSpline2_Solve', 0x8000A440, t_solve),
    ('BSpline2_Deriv', 0x8000A574, t_deriv),
    ('Pool_Init', 0x800094D0, lambda e, f, r: t_poolinit(e, f, r, 12)),
    ('Ring_Init', 0x80009D40, lambda e, f, r: t_poolinit(e, f, r, 20)),
    ('Stub_80009E90', 0x80009E90, t_stub),
    ('Stub_80009E98', 0x80009E98, t_stub),
    ('Stub_8000A090', 0x8000A090, t_stub),
    ('Debug_Printf', 0x80009EA0, t_stub),
]

POOL_ORIG = {'init': 0x800094D0, 'take': 0x8000953C, 'count': 0x80009564, 'give': 0x80009590}
POOL_NAMES = {'init': 'Pool_Init', 'take': 'Pool_Take', 'count': 'Pool_CountFree', 'give': 'Pool_Give'}
RING_ORIG = {'init': 0x80009D40, 'get': 0x80009DB4, 'count': 0x80009DF4, 'put': 0x80009E2C, 'stop': 0x80009E6C}
RING_NAMES = {'init': 'Ring_Init', 'get': 'Ring_Get', 'count': 'Ring_Count', 'put': 'Ring_Put', 'stop': 'Ring_SetStop'}

# ---------------- Mesh_BuildVertexBatches (uses the game heap) ----------------
TRIS = 0x80420000
MOUT = 0x80440000
ORDER = 0x80460000
STACK_LO, STACK_HI = 0x807E0000, 0x80800000


def mesh(rng):
    r = rng.random()
    tris = []
    ng = rng.choice([1, 1, 2, 3, 5])
    if r < 0.4:                                   # grid mesh with shared vertices
        w, h = rng.randrange(2, 9), rng.randrange(2, 9)
        for y in range(h - 1):
            for x in range(w - 1):
                a, b, c, d = y * w + x, y * w + x + 1, (y + 1) * w + x, (y + 1) * w + x + 1
                g = rng.randrange(ng)
                tris += [[a, b, c, g], [b, d, c, g]]
        rng.shuffle(tris)
    elif r < 0.8:                                 # random triangles
        nv = rng.randrange(3, 70)
        for _ in range(rng.randrange(0, 90)):
            tris.append([rng.randrange(nv) for _ in range(3)] + [rng.randrange(ng)])
    elif r < 0.9:                                 # fans / strips
        nv = rng.randrange(4, 40)
        for i in range(1, nv - 1):
            tris.append([0 if rng.random() < .5 else i - 1, i, i + 1, rng.randrange(ng)])
    elif r < 0.95:
        tris = []                                 # empty
        if rng.random() < .5:
            tris = [[rng.randrange(5) for _ in range(3)] + [rng.randrange(3)]]
    else:                                         # odd values: negative / >16-bit fields
        nv = rng.randrange(3, 20)
        for _ in range(rng.randrange(1, 20)):
            tris.append([rng.randrange(nv) for _ in range(3)] + [rng.randrange(ng)])
        t = rng.choice(tris)
        if rng.random() < 0.5:                    # negative vertex: usually fails (returns 0)
            t[rng.randrange(3)] = rng.choice([-1, -2, -0x10000 + rng.randrange(3)])
        else:                                     # group 0xFFFF/0xFFFE after truncation: never selected
            t[3] = rng.choice([-1, -2])
        # (values >= 0x10000 make the original exhaust the 2 MB heap (vertex) or
        #  loop over 65536+ groups (group), which is too slow to emulate)
    return tris


def mask_stack(buf):
    """Words pointing into the stack (list sentinels left in freed heap memory)
    depend on each version's frame layout: replace them before comparing."""
    out = bytearray(buf)
    for i in range(0, len(out) - 3, 4):
        w = struct.unpack_from('>I', out, i)[0]
        if STACK_LO <= w < STACK_HI:
            struct.pack_into('>I', out, i, 0x5A5A5A5A)
    return bytes(out)


def t_mesh(emu, f, tris):
    reset_heap(emu)
    emu.write(TRIS, b''.join(struct.pack('>4i', *t) for t in tris) + b'\0' * 16)
    emu.write(MOUT, b'\xcc' * 0x8000)
    emu.write(ORDER, b'\xcc' * 0x1000)
    v0, _ = emu.call(f, a=[MOUT, ORDER, TRIS, len(tris)])
    return v0, emu.read(MOUT, 0x8000), emu.read(ORDER, 0x1000), mask_stack(emu.read(GAME_HEAP, 0x20000))


def test_mesh(emu, syms, trials):
    cases = [mesh(random.Random(s)) for s in range(trials)]
    # more than 256 batches: disjoint triangles -> returns 0
    cases.append([[3 * i, 3 * i + 1, 3 * i + 2, 0] for i in range(300)])
    # one batch would exceed 16 vertices: a big fan around vertex 0
    cases.append([[0, i, i + 1, 0] for i in range(1, 40)])
    bad = 0
    for i, tris in enumerate(cases):
        a = t_mesh(emu, 0x80008A10, tris)
        b = t_mesh(emu, syms['Mesh_BuildVertexBatches'], tris)
        if a != b:
            if bad == 0:
                print('  first mismatch case', i, 'v0', a[0], b[0], 'out', a[1] == b[1], 'order', a[2] == b[2],
                      'heap', a[3] == b[3])
            bad += 1
    return bad, len(cases)


EXTRA = [('Mesh_BuildVertexBatches', test_mesh)]


def main(trials=400):
    blob, syms = build_clean(['src/game/collide.c'], name='collide')
    emu = make_env(); emu.write(CODE_BASE, blob)
    bad_total = 0

    def report(name, bad, n):
        nonlocal bad_total
        print(f'{name:24s} {"PASS" if not bad else "FAIL"} {n - bad}/{n}')
        bad_total += bad

    for name, addr, fn in SIMPLE:
        bad = 0
        for s in range(trials):
            a = fn(emu, addr, random.Random(s))
            b = fn(emu, syms[name], random.Random(s))
            if a != b:
                if bad == 0:
                    print('  first mismatch seed', s, a[0] if not isinstance(a[0], bytes) else a[0].hex()[:96],
                          b[0] if not isinstance(b[0], bytes) else b[0].hex()[:96])
                bad += 1
        report(name, bad, trials)

    for label, orig, names, fn in (('Pool_Take/CountFree/Give', POOL_ORIG, POOL_NAMES, t_pool_seq),
                                   ('Ring_Get/Count/Put/SetStop', RING_ORIG, RING_NAMES, t_ring_seq)):
        mine = {k: syms[v] for k, v in names.items()}
        bad = 0
        for s in range(trials):
            a = fn(emu, orig, random.Random(s)); b = fn(emu, mine, random.Random(s))
            if a != b:
                if bad == 0:
                    print('  first mismatch seed', s, a[0][:8], b[0][:8])
                bad += 1
        report(label, bad, trials)

    for name, fn in EXTRA:
        bad, n = fn(emu, syms, trials)
        report(name, bad, n)
    return 1 if bad_total else 0


if __name__ == '__main__':
    sys.exit(main())
