#!/usr/bin/env python3
"""Differential test of src/game/scene.c (scene graph nodes, camera, light)
against the original MIPS code. Random node trees are built in test memory
(and in the real node pool for create/delete), run through the original and
the C version from identical memory, and all node memory, display-list output
and globals are compared."""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env
from mipsbuild import CODE_BASE
from draft_mips import build_clean
from unicorn.mips_const import UC_MIPS_REG_SP

ORIG = {
    'Main_ParseArgs': 0x800113E0, 'Scene_Init': 0x80011550, 'Scene_NewNode': 0x800116B8,
    'Scene_CountFreeNodes': 0x8001179C, 'Scene_DeleteNode': 0x800117BC,
    'Scene_UpdateWorld': 0x80011868, 'Scene_ConvertMatrices': 0x80011C20,
    'Scene_BuildGfx': 0x80011C88, 'Camera_Init': 0x80011E60, 'Camera_SetEye': 0x80011ED0,
    'Camera_SetTarget': 0x80011F40, 'Camera_GetForward': 0x80011FB0, 'Light_Init': 0x80012090,
    'Light_SetPos': 0x80012130, 'Light_SetTarget': 0x80012158, 'Light_SetColor': 0x80012180,
    'Light_SetAmbient': 0x80012250, 'Light_Update': 0x80012320, 'Light_CalcDir': 0x80012760,
}

POOL, POOL_SIZE = 0x800B4AB0, 0xC000
POOL_INIT = 0x80009D40
NODE = 0xE0
ARGFLAGS = [0x800AF230, 0x800B04C8, 0x800B04B4, 0x800B0728]
ROOTS = [0x800C316C, 0x800C30C0, 0x800C0AB4, 0x800AF234, 0x800B061C, 0x800B04A0, 0x800AF240, 0x800B0500]
SCREEN_W = 0x800C30D0
CAM = [0x800B0444, 0x800B04B8, 0x800C1664]     # eye, target, up (Vec3f each)
FOV = 0x800C51D4
VIEWPORT_FN = 0x80010900

TREE = 0x80400000          # nodes built by the test
DLARR = 0x80440000         # SCENE_DL_LIST arrays
PARENT = 0x80448000        # parent matrix
GFX, GFX_SIZE = 0x80450000, 0x4000
STR = 0x80460000
LIGHT = 0x80470000
VEC = 0x80470100
VIEW = 0x80470200


def fbits(f):
    return struct.unpack('>I', struct.pack('>f', f))[0]


def rfloat(rng, special=True):
    r = rng.random()
    if special and r < 0.03: return rng.choice([float('inf'), float('-inf'), float('nan')])
    if r < 0.10: return rng.choice([0.0, -0.0, 1.0, -1.0, 0.5])
    if r < 0.30: return float(rng.randrange(-300, 300))
    if r < 0.50: return rng.uniform(-2.0, 2.0)
    return rng.uniform(-1000.0, 1000.0)


def rmtx(rng, special=False):
    return struct.pack('>16f', *[rfloat(rng, special) for _ in range(16)])


# ---------------------------------------------------------------- arg parser
def t_parseargs(emu, fn, rng):
    toks = []
    for _ in range(rng.randrange(0, 31)):
        toks.append(rng.choice([b'-v', b'-d', b'-s', b'-l', b'-x', b'-', b'-vd', b'-L', b'game', b'x',
                                b'-' + bytes([rng.randrange(1, 256)])]))
    seps = [b' ' * rng.choice([1, 1, 1, 2, 3]) for _ in toks]
    s = (b' ' * rng.choice([0, 0, 1, 2])) + b''.join(t + sp for t, sp in zip(toks, seps))
    if rng.random() < 0.5: s = s.rstrip(b' ')
    s = s.replace(b'\0', b'?')[:0x200]
    emu.write(STR, s + b'\0' + bytes(rng.getrandbits(8) | 1 for _ in range(16)))
    for a in ARGFLAGS: emu.w32(a, rng.getrandbits(32))
    ptr = STR if rng.random() > 0.03 else 0
    emu.call(fn, a=[ptr])
    return emu.read(STR, 0x220), [emu.r32(a) for a in ARGFLAGS]


# ---------------------------------------------------------------- pool based
def pool_reset(emu, rng):
    emu.write(POOL, bytes(POOL_SIZE))
    emu.w32(SCREEN_W, rng.choice([320, 640, rng.randrange(0, 1000)]))
    emu.call(POOL_INIT, a=[POOL, POOL_SIZE, NODE])


def snapshot_pool(emu):
    return emu.read(POOL, POOL_SIZE)


def t_init(emu, fns, rng):
    emu.write(POOL, bytes(rng.getrandbits(8) for _ in range(64)) * (POOL_SIZE // 64))
    emu.w32(SCREEN_W, rng.choice([320, 640]))
    for a in ROOTS: emu.w32(a, rng.getrandbits(32))
    emu.call(fns['Scene_Init'])
    return snapshot_pool(emu), [emu.r32(a) for a in ROOTS]


def run_pool_sequence(emu, fns, ops, seed):
    """ops is a list of abstract operations; Python tracks the tree (by creation
    index) identically for both runs, so addresses only come from the code."""
    rng = random.Random(seed)
    pool_reset(emu, rng)
    rets = []
    nodes = []          # index -> address
    parent = {}         # index -> parent index (None = root)
    types = {}
    alive = set()
    root, _ = emu.call(fns['Scene_NewNode'], a=[1])
    nodes.append(root); parent[0] = None; types[0] = 1; alive.add(0)
    rets.append(root)

    def append(g, n):
        tail = emu.r32(nodes[g] + 0xD4)
        tp = emu.r32(tail + 4)
        emu.w32(tp + 0, nodes[n]); emu.w32(nodes[n] + 4, tp)
        emu.w32(nodes[n] + 0, tail); emu.w32(tail + 4, nodes[n])

    def descendants(i):
        out = [i]
        for j in list(alive):
            if parent.get(j) == i: out += descendants(j)
        return out

    for op, x, y in ops:
        if op == 'new':
            typ = x
            v0, _ = emu.call(fns['Scene_NewNode'], a=[typ])
            rets.append(v0)
            if v0 == 0: continue
            idx = len(nodes); nodes.append(v0); types[idx] = typ
            groups = sorted(i for i in alive if types[i] == 1)
            g = groups[y % len(groups)]
            append(g, idx); parent[idx] = g; alive.add(idx)
            # scribble on the local matrix so deletes/frees move real data
            emu.write(v0 + 0x0C, struct.pack('>I', rng.getrandbits(32)))
        elif op == 'del':
            cand = sorted(i for i in alive if i != 0)
            if not cand: continue
            i = cand[x % len(cand)]
            emu.call(fns['Scene_DeleteNode'], a=[nodes[i]])
            for j in descendants(i): alive.discard(j)
        elif op == 'count':
            v0, _ = emu.call(fns['Scene_CountFreeNodes'])
            rets.append(v0)
    return rets, snapshot_pool(emu)


def pool_ops(rng):
    ops = []
    live = 1
    for _ in range(rng.randrange(1, 80)):
        r = rng.random()
        if r < 0.55 and live < 120:
            t = rng.choice([0, 1, 1, 2, 2, 3, 4, 5])
            ops.append(('new', t, rng.randrange(1000))); live += 3 if t == 1 else 1
        elif r < 0.85:
            ops.append(('del', rng.randrange(1000), 0)); live = max(1, live - 2)
        else:
            ops.append(('count', 0, 0))
    return ops


# ---------------------------------------------------------------- test trees
def build_tree(emu, rng, depth_max=4):
    """Lay out a random tree in test memory. Returns (root address, node count)."""
    emu.write(TREE, bytes(rng.getrandbits(8) for _ in range(256)) * (0x40000 // 256))
    emu.write(DLARR, bytes(rng.getrandbits(8) for _ in range(0x1000)))
    nxt = [TREE]
    dlnext = [DLARR]

    def alloc():
        a = nxt[0]; nxt[0] += NODE; return a

    def make(typ, depth):
        a = alloc()
        emu.w32(a + 8, typ)
        emu.write(a + 0x0C, rmtx(rng, special=rng.random() < 0.1))
        if typ == 1:
            head, tail = alloc(), alloc()
            for s in (head, tail):
                emu.w32(s + 8, 0)
            emu.w32(head + 4, 0); emu.w32(tail + 0, 0)
            emu.w32(a + 0xD0, head); emu.w32(a + 0xD4, tail)
            prev = head
            n = rng.randrange(0, 5) if depth < depth_max else 0
            for _ in range(n):
                t = rng.choice([1, 1, 2, 2, 2, 3, 3, 4, 0, 5, 9])
                c = make(t, depth + 1)
                emu.w32(prev + 0, c); emu.w32(c + 4, prev); prev = c
            emu.w32(prev + 0, tail); emu.w32(tail + 4, prev)
        elif typ == 2:
            for off in (0xD0, 0xD4, 0xD8):
                emu.w32(a + off, 0 if rng.random() < 0.3 else 0x80000000 | rng.getrandbits(23) & ~7)
        elif typ == 3:
            cnt = rng.choice([0, 1, 2, 3, 5, 8, -1, -7])
            arr = dlnext[0]; dlnext[0] += 0x40
            emu.w32(a + 0xD0, arr); emu.w32(a + 0xD4, cnt & 0xFFFFFFFF)
        elif typ == 4:
            for off in range(0xD0, 0xDC, 2):
                emu.w16(a + off, rng.choice([0, 1, 239, 319, 0x2000, rng.getrandbits(16)]))
        return a

    root = make(1 if rng.random() < 0.9 else rng.choice([2, 3, 4]), 0)
    return root, (nxt[0] - TREE) // NODE


def viewport_stub_factory(log):
    def stub(e):
        sp = e.mu.reg_read(UC_MIPS_REG_SP)
        args = (e.arg(0), e.arg(1), e.arg(2), e.arg(3), e.r32(sp + 0x10), e.r32(sp + 0x14), e.r32(sp + 0x18))
        log.append(args)
        g = e.arg(0)
        e.w32(g, 0xEE000000 | (len(log) & 0xFFFF)); e.w32(g + 4, args[1] ^ args[4])
        return g + 8
    return stub


def t_tree(emu, fns, kind, seed, log):
    rng = random.Random(seed)
    root, n = build_tree(emu, rng)
    emu.write(PARENT, rmtx(rng))
    emu.write(GFX, b'\xcc' * GFX_SIZE)
    del log[:]
    v0 = None
    if kind == 'update':
        par = PARENT if rng.random() < 0.85 else root + 0x4C      # aliasing: parent == own world
        emu.call(fns['Scene_UpdateWorld'], a=[root, par])
    elif kind == 'convert':
        emu.call(fns['Scene_ConvertMatrices'], a=[root])
    else:
        v0, _ = emu.call(fns['Scene_BuildGfx'], a=[GFX, root])
    return v0, emu.read(TREE, n * NODE), emu.read(GFX, GFX_SIZE), list(log)


# ---------------------------------------------------------------- camera
def t_camera(emu, fns, kind, seed):
    rng = random.Random(seed)
    for a in CAM:
        emu.write(a, struct.pack('>3f', *[rfloat(rng) for _ in range(3)]))
    emu.write(FOV, struct.pack('>I', rng.getrandbits(32)))
    emu.write(VEC, bytes(rng.getrandbits(8) for _ in range(16)))
    if kind == 'init':
        emu.call(fns['Camera_Init'])
    elif kind in ('eye', 'target'):
        x, y, z = (rfloat(rng) for _ in range(3))
        emu.call(fns['Camera_SetEye' if kind == 'eye' else 'Camera_SetTarget'], a=[0, 0, fbits(z)], f=[x, y])
    else:
        emu.call(fns['Camera_GetForward'], a=[VEC])
    return b''.join(emu.read(a, 12) for a in CAM) + emu.read(FOV, 4) + emu.read(VEC, 16)


# ---------------------------------------------------------------- light
def t_light(emu, fns, kind, seed):
    rng = random.Random(seed)
    emu.write(LIGHT, bytes(rng.getrandbits(8) for _ in range(0x60)))
    # sensible fields most of the time
    emu.write(LIGHT, struct.pack('>15f', *[rfloat(rng, special=rng.random() < 0.2) for _ in range(9)],
                                 *[rng.choice([rng.uniform(0, 255), rng.uniform(-300, 600), 255.0, 0.0,
                                               3e9, 2147483648.0, 5e9, -1e10]) for _ in range(6)]))
    emu.write(VEC, bytes(rng.getrandbits(8) for _ in range(16)))
    emu.write(VIEW, rmtx(rng) if rng.random() < 0.7 else
              struct.pack('>16f', 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, rfloat(rng), rfloat(rng), rfloat(rng), 1))
    if kind == 'init':
        emu.call(fns['Light_Init'], a=[LIGHT])
    elif kind in ('pos', 'target', 'color', 'ambient'):
        if kind in ('color', 'ambient'):
            vals = [rng.choice([rng.uniform(-50, 300), 255.0, 255.5, 0.0, -0.0, -1.0, float('nan'),
                                float('inf'), float('-inf'), 1e30]) for _ in range(3)]
        else:
            vals = [rfloat(rng) for _ in range(3)]
        name = {'pos': 'Light_SetPos', 'target': 'Light_SetTarget', 'color': 'Light_SetColor',
                'ambient': 'Light_SetAmbient'}[kind]
        emu.call(fns[name], a=[LIGHT] + [fbits(v) for v in vals])
    elif kind == 'update':
        emu.call(fns['Light_Update'], a=[LIGHT, VIEW])
    else:
        out = LIGHT if rng.random() < 0.5 else VEC
        emu.call(fns['Light_CalcDir'], a=[LIGHT, out])
    return emu.read(LIGHT, 0x60) + emu.read(VEC, 16)


def report(name, bad, n):
    print(f'{name:24s} {"PASS" if not bad else "FAIL"} {n - bad}/{n}')
    return bad


def main(trials=300):
    blob, syms = build_clean(['src/game/scene.c'], name='scene')
    emu = make_env(); emu.write(CODE_BASE, blob)
    C = {k: syms[k] for k in ORIG}
    log = []
    emu.stub(VIEWPORT_FN, viewport_stub_factory(log))
    fails = 0

    def compare(name, fa, fb, n=trials):
        bad = 0
        for s in range(n):
            a, b = fa(s), fb(s)
            if a != b:
                if not bad:
                    print(f'  {name}: first mismatch seed {s}')
                bad += 1
        return report(name, bad, n)

    fails += compare('Main_ParseArgs', lambda s: t_parseargs(emu, ORIG['Main_ParseArgs'], random.Random(s)),
                     lambda s: t_parseargs(emu, C['Main_ParseArgs'], random.Random(s)))
    fails += compare('Scene_Init', lambda s: t_init(emu, ORIG, random.Random(s)),
                     lambda s: t_init(emu, C, random.Random(s)), n=50)

    def seq(fns, s):
        return run_pool_sequence(emu, fns, pool_ops(random.Random(s)), s)
    fails += compare('Scene_NewNode/Delete/Count', lambda s: seq(ORIG, s), lambda s: seq(C, s))
    for kind, name in (('update', 'Scene_UpdateWorld'), ('convert', 'Scene_ConvertMatrices'),
                       ('gfx', 'Scene_BuildGfx')):
        fails += compare(name, lambda s: t_tree(emu, ORIG, kind, s, log), lambda s: t_tree(emu, C, kind, s, log))
    for kind, name in (('init', 'Camera_Init'), ('eye', 'Camera_SetEye'), ('target', 'Camera_SetTarget'),
                       ('fwd', 'Camera_GetForward')):
        fails += compare(name, lambda s: t_camera(emu, ORIG, kind, s), lambda s: t_camera(emu, C, kind, s))
    for kind, name in (('init', 'Light_Init'), ('pos', 'Light_SetPos'), ('target', 'Light_SetTarget'),
                       ('color', 'Light_SetColor'), ('ambient', 'Light_SetAmbient'),
                       ('update', 'Light_Update'), ('dir', 'Light_CalcDir')):
        fails += compare(name, lambda s: t_light(emu, ORIG, kind, s), lambda s: t_light(emu, C, kind, s))
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
