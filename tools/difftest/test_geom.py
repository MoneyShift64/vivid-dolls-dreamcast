#!/usr/bin/env python3
"""Differential test of src/game/geom.c (integer 2D geometry, mesh interpolation,
greedy triangulation, triangle/quad hit tests, look-at matrix, radix sorts)
against the original MIPS code. Both run in the emulator on identical memory;
return values and all touched memory (inputs, outputs, the whole game heap for
the heap-using functions) are compared."""
import os, random, struct, sys, zlib
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env, reset_heap, GAME_HEAP, GAME_HEAP_SIZE
from emu import STACK_TOP
from mipsbuild import CODE_BASE
from draft_mips import build_clean

IN = 0x80400000       # input data (points, triangles, records...)
IN2 = 0x80410000      # second input block
OUT = 0x80420000      # output buffer
REGION = 0x30000      # IN..OUT+0x10000 compared after every call
M32 = 0xFFFFFFFF

fb = lambda v: struct.unpack('>I', struct.pack('>f', v))[0]
pw = lambda vals: b''.join(struct.pack('>I', v & M32) for v in vals)
pf = lambda vals: b''.join(struct.pack('>f', v) for v in vals)


def s32(v):
    v &= M32
    return v - (1 << 32) if v & 0x80000000 else v


def cross(a, b, c):
    return s32((b[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b[1] - a[1]))


def rint(rng, small=True):
    r = rng.random()
    if small and r < 0.6:
        return rng.randrange(-6, 7)
    if r < 0.85:
        return rng.randrange(-2000, 2001)
    return s32(rng.getrandbits(32))


def rfloat(rng, scale=100.0):
    r = rng.random()
    if r < 0.1:
        return float(rng.randrange(-4, 5))
    return rng.uniform(-scale, scale)


# ---------------------------------------------------------------- setups
# each returns (args, writes[(addr, bytes)], uses_heap, pre_orig) where pre_orig is
# an optional hook run once before the original (see Mesh_InterpolateAt)

def setup_cross(rng):
    pts = [rint(rng, rng.random() < .5) for _ in range(6)]
    return [IN, IN + 8, IN + 16], [(IN, pw(pts))], False


def setup_seg(rng):
    small = rng.random() < 0.7
    pts = [rng.randrange(-4, 5) if small else rint(rng, False) for _ in range(8)]
    if rng.random() < 0.2:              # force collinear segments
        dx, dy = rng.randrange(-3, 4), rng.randrange(-3, 4)
        x0, y0 = rng.randrange(-5, 6), rng.randrange(-5, 6)
        ts = [rng.randrange(-4, 5) for _ in range(4)]
        pts = sum(([x0 + t * dx, y0 + t * dy] for t in ts), [])
    if rng.random() < 0.1:              # shared end point
        pts[4:6] = pts[0:2]
    return [IN, IN + 8, IN + 16, IN + 24], [(IN, pw(pts))], False


def plane_trap(px, py, t):
    (x0, y0, z0), (x1, y1, z1), (x2, y2, z2) = t
    a = s32((y1 - y0) * (z2 - z0) - (z1 - z0) * (y2 - y0))
    b = s32((z1 - z0) * (x2 - x0) - (x1 - x0) * (z2 - z0))
    c = s32((x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0))
    d = s32(-(x1 * a + y1 * b + z1 * c))
    num = s32(-(a * px + b * py + d))
    return c == -1 and num == -0x80000000   # original would trap (break 6)


def setup_interpz(rng):
    while True:
        small = rng.random() < 0.6
        t = [[rint(rng, small) for _ in range(3)] for _ in range(3)]
        if rng.random() < 0.1:
            t[2][0], t[2][1] = t[1][0], t[1][1]          # vertical triangle (C == 0)
        p = [rint(rng, small), rint(rng, small)]
        if not plane_trap(p[0], p[1], t):
            break
    return [IN, IN2], [(IN, pw(p)), (IN2, pw(sum(t, [])))], False


def setup_mesh(rng):
    nv = rng.randrange(3, 12)
    big = rng.random() < 0.15
    verts = []
    for _ in range(nv):
        x, y = (rint(rng, False), rint(rng, False)) if big else (rng.randrange(-20, 21), rng.randrange(-20, 21))
        verts.append([x, y] + [rint(rng, rng.random() < .5) for _ in range(3)])
    nt = rng.randrange(0, 10)
    tris = [[rng.randrange(nv) for _ in range(3)] for _ in range(nt)]
    if rng.random() < 0.5 and nv:
        v = verts[rng.randrange(nv)]
        p = [v[0], v[1]] if rng.random() < .3 else [v[0] + rng.randrange(-3, 4), v[1] + rng.randrange(-3, 4)]
    else:
        p = [rng.randrange(-22, 23), rng.randrange(-22, 23)]
    # skip the rare case where the original's division would trap
    for t in tris:
        tv = [verts[i] for i in t]
        if any(plane_trap(p[0], p[1], [(v[0], v[1], v[2 + k]) for v in tv]) for k in range(3)):
            return setup_mesh(rng)
    ntris_arg = nt if rng.random() < .9 else rng.choice([0, -1])
    w = [(IN, pw(p)), (IN2, pw(sum(tris, []))), (IN2 + 0x1000, pw(sum(verts, [])))]
    return [OUT, IN, IN2, IN2 + 0x1000, ntris_arg], w, False


def setup_greedy(rng):
    n = rng.choice([3, 3, 4, 4, 5, 5, 6, 6, 7, 8, 9, 10])
    span = rng.choice([4, 10, 100, 1000])
    pts = set()
    while len(pts) < n:
        pts.add((rng.randrange(-span, span + 1), rng.randrange(-span, span + 1)))
    pts = [list(p) for p in pts]
    rng.shuffle(pts)
    if rng.random() < 0.15:             # collinear points
        for i in range(min(n, 4)):
            pts[i] = [i * 3 - 4, i * 2 + 1]
    if rng.random() < 0.05:             # a duplicate point
        pts[-1] = list(pts[0])
    return [OUT, IN, n], [(IN, pw(sum(pts, [])))], True


def rand_tri(rng, grid=False):
    while True:
        if grid:
            t = [[float(rng.randrange(-8, 9)), float(rng.randrange(-8, 9)), 0.0] for _ in range(3)]
        else:
            t = [[rfloat(rng) for _ in range(3)] for _ in range(3)]
        ab = [t[1][i] - t[0][i] for i in range(3)]
        ac = [t[2][i] - t[0][i] for i in range(3)]
        n = [ab[1] * ac[2] - ab[2] * ac[1], ab[2] * ac[0] - ab[0] * ac[2], ab[0] * ac[1] - ab[1] * ac[0]]
        if sum(x * x for x in n) > 1e-2:
            return t


def line_at(rng, tri, grid):
    """A line through a point near/inside the triangle (barycentric weights)."""
    if grid:
        if rng.random() < 0.5:          # hit exactly on an edge or vertex
            i, j = rng.sample(range(3), 2)
            t = rng.choice([0.0, 0.5, 1.0, 0.25, -0.5, 1.5])
            p = [tri[i][k] + (tri[j][k] - tri[i][k]) * t for k in range(3)]
        else:
            p = [float(rng.randrange(-9, 10)), float(rng.randrange(-9, 10)), 0.0]
        start = [p[0], p[1], float(rng.randrange(1, 9))]
        d = [0.0, 0.0, rng.choice([-1.0, 1.0, -3.0])]
    else:
        w = [rng.uniform(-0.4, 1.2) for _ in range(2)]
        w.append(1.0 - w[0] - w[1])
        p = [sum(w[v] * tri[v][k] for v in range(3)) for k in range(3)]
        d = [rfloat(rng, 10.0) for _ in range(3)]
        if rng.random() < 0.05:
            d = [0.0, 0.0, 0.0]
        s = rng.uniform(-5, 5)
        start = [p[k] - d[k] * s for k in range(3)]
    return start + d


def setup_tri_isect(rng):
    grid = rng.random() < 0.3
    tri = rand_tri(rng, grid) if rng.random() < .9 else [[rfloat(rng) for _ in range(3)] for _ in range(3)]
    line = line_at(rng, tri, grid) if rng.random() < .8 else [rfloat(rng) for _ in range(6)]
    out = OUT
    if rng.random() < 0.1:
        out = IN                         # out aliases line->point
    return [out, IN, IN2], [(IN, pf(line)), (IN2, pf(sum(tri, [])))], False


def setup_tri_hit(rng):
    grid = rng.random() < 0.4
    tri = rand_tri(rng, grid)
    line = line_at(rng, tri, grid)
    return [OUT, IN, IN2], [(IN, pf(line)), (IN2, pf(sum(tri, [])))], False


def setup_quad_hit(rng):
    grid = rng.random() < 0.4
    while True:
        t = rand_tri(rng, grid)
        if grid:
            q4 = [float(rng.randrange(-8, 9)), float(rng.randrange(-8, 9)), 0.0]
        else:
            a, b = rng.uniform(-0.5, 1.0), rng.uniform(0.2, 1.2)
            q4 = [t[0][k] + (t[2][k] - t[1][k]) * b + (t[1][k] - t[0][k]) * a * 0.2 for k in range(3)]
            if rng.random() < 0.3:
                q4[2] += rng.uniform(-1, 1)          # non-planar quad
        quad = [t[0], t[1], t[2], q4]
        t2 = [quad[2], quad[3], quad[0]]
        ab = [t2[1][i] - t2[0][i] for i in range(3)]
        ac = [t2[2][i] - t2[0][i] for i in range(3)]
        n = [ab[1] * ac[2] - ab[2] * ac[1], ab[2] * ac[0] - ab[0] * ac[2], ab[0] * ac[1] - ab[1] * ac[0]]
        if sum(x * x for x in n) > 1e-2:
            break
    line = line_at(rng, rng.choice([t, t2]), grid)
    return [OUT, IN, IN2], [(IN, pf(line)), (IN2, pf(sum(quad, [])))], False


def setup_lookat(rng):
    eye = [rfloat(rng, 1000.0) for _ in range(3)]
    at = [rfloat(rng, 1000.0) for _ in range(3)]
    up = rng.choice([[0.0, 1.0, 0.0], [0.0, 0.0, 1.0], [rfloat(rng), rfloat(rng), rfloat(rng)]])
    if rng.random() < 0.03:
        at = list(eye)                   # zero direction -> inf/NaN
    m = OUT
    r = rng.random()
    w = [(IN, pf(eye)), (IN2, pf(at)), (OUT, pf([rfloat(rng) for _ in range(16)]))]
    if r < 0.08:
        m = IN                           # matrix aliases eye
    elif r < 0.16:
        m = IN2                          # matrix aliases at
    return [m, IN, IN2, fb(up[0]), fb(up[1]), fb(up[2])], w, False


def rand_records(rng, n, recsize):
    pool = [0, 1, 2, 0x7F, 0x80, 0xFF]
    return bytes(rng.choice(pool) if rng.random() < .5 else rng.getrandbits(8) for _ in range(n * recsize + 8))


def setup_radix_ptrs(rng):
    n = rng.choice([0, 1, 2, 5, 17, 40, rng.randrange(0, 80)])
    recsize = rng.randrange(1, 12)
    keyofs = rng.randrange(0, recsize)
    keylen = rng.randrange(1 if rng.random() < .97 else 0, recsize - keyofs + 1)
    desc = rng.choice([0, 0, 1, 7])
    data = rand_records(rng, n, recsize)
    order = list(range(n))
    rng.shuffle(order)
    ptrs = [IN2 + i * recsize for i in order]
    return [IN, n, keyofs, keylen, desc], [(IN, pw(ptrs) + b'\0' * 8), (IN2, data)], True


def setup_radix_recs(rng):
    n = rng.choice([0, 1, 2, 5, 17, 40, rng.randrange(0, 80)])
    recsize = rng.choice([4, 8, 12, 16, 1, 2, 3, 5, 6, 7, 9, rng.randrange(1, 20)])
    keyofs = rng.randrange(0, recsize)
    keylen = rng.randrange(1, recsize - keyofs + 1)
    desc = rng.choice([0, 0, 1])
    return [IN, n, recsize, keyofs, keylen, desc], [(IN, rand_records(rng, n, recsize))], True


TESTS = [
    # name, original, setup, returns int?
    ('Geom_Cross2i',           0x80002200, setup_cross,       True),
    ('Seg2i_Intersect',        0x80002248, setup_seg,         True),
    ('Tri3i_InterpolateZ',     0x80002434, setup_interpz,     True),
    ('Mesh_InterpolateAt',     0x80002558, setup_mesh,        True),
    ('Geom_GreedyTriangulate', 0x80002990, setup_greedy,      True),
    ('Tri_IntersectLine',      0x80002E70, setup_tri_isect,   True),
    ('Tri_HitTest',            0x8000306C, setup_tri_hit,     True),
    ('Quad_HitTest',           0x80002FA0, setup_quad_hit,    True),
    ('Mtx_LookAtV',             0x80003650, setup_lookat,      False),
    ('RadixSort_Ptrs',         0x800039F0, setup_radix_ptrs,  False),
    ('RadixSort_Records',      0x80003C4C, setup_radix_recs,  False),
]

ZERO_REGION = bytes(REGION)
ZERO_HEAP = bytes(GAME_HEAP_SIZE)


def run(emu, addr, args, writes, heap, is_int):
    emu.write(IN, ZERO_REGION)
    for a, b in writes:
        emu.write(a, b)
    if heap:
        emu.write(GAME_HEAP, ZERO_HEAP)
        reset_heap(emu)
    v0, _ = emu.call(addr, a=args)
    res = (v0 if is_int else None, emu.read(IN, REGION))
    if heap:
        res += (emu.read(GAME_HEAP, GAME_HEAP_SIZE),)
    return res


def main(trials=400):
    blob, syms = build_clean(['src/game/geom.c'], name='geom')
    emu = make_env()
    emu.write(CODE_BASE, blob)
    only = sys.argv[1:]
    bad_total = 0
    for name, addr, setup, is_int in TESTS:
        if only and name not in only:
            continue
        n_trials = 150 if name == 'Geom_GreedyTriangulate' else trials
        bad = 0
        for s in range(n_trials):
            rng = random.Random(zlib.crc32(name.encode()) ^ s * 7919)
            args, writes, heap = setup(rng)
            if name == 'Mesh_InterpolateAt':
                # The original reads the triangle's x coordinates from stale stack
                # slots (compiler hoisted the loads above the copy). Running it once
                # first leaves exactly those values in its stack frame, so the second
                # run shows the intended behaviour that the C implements.
                run(emu, addr, args, writes, heap, is_int)
            a = run(emu, addr, args, writes, heap, is_int)
            b = run(emu, syms[name], args, writes, heap, is_int)
            if a != b:
                if bad == 0:
                    print(f'  {name}: first mismatch seed {s} args {[hex(x & M32) for x in args]} v0 {a[0]} vs {b[0]}')
                    for i, (x, y) in enumerate(zip(a[1:], b[1:])):
                        if x != y:
                            off = next(k for k in range(len(x)) if x[k] != y[k])
                            print(f'    region {i} offset {off:#x}: {x[off:off+16].hex()} vs {y[off:off+16].hex()}')
                bad += 1
        print(f'{name:24s} {"PASS" if not bad else "FAIL"} {n_trials - bad}/{n_trials}')
        bad_total += bad
    return 1 if bad_total else 0


if __name__ == '__main__':
    sys.exit(main())
