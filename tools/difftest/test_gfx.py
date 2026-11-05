#!/usr/bin/env python3
"""Differential test of src/game/gfx.c (frame rendering, viewports, video mode,
cabinet I/O, controllers, controller pak / EEPROM wrappers, geometry helpers)
against the original MIPS code.

Before every call the emulator is put back into the same baseline (scene graph
initialised with a few drawable nodes), the same random setup is written, then
the original or the C function is called. OS / hardware calls are Python stubs
that return the same pseudo-random results to both and record their arguments
(and the display-list head). Compared: return value (and f0 bits), all of BSS
and data 0x80038000-0x800C5400, the RDP buffers, test memory, the Aleck64 I/O
page and the stub call logs."""
import os, random, re, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env
from mipsbuild import CODE_BASE, ROOT
from draft_mips import build_clean
from unicorn.mips_const import UC_MIPS_REG_SP

DATA, DATA_END = 0x80038000, 0x800C5400
RDPBUF, RDPBUF_END = 0x8013A000, 0x80158000
TEST, TEST_SIZE = 0x80400000, 0x80000
IO, IO_SIZE = 0x02000000, 0x1000
FB0, FB_SIZE = 0x807B2600, 0x4B000           # both frame buffers
REGIONS = [(DATA, DATA_END - DATA), (RDPBUF, RDPBUF_END - RDPBUF), (TEST, TEST_SIZE), (IO, IO_SIZE)]

SCREEN_W, SCREEN_H, MODE = 0x800C30D0, 0x800C5330, 0x800B0504
DL_HEAD = 0x800B053C
SCENE_INIT, SCENE_NEW = 0x80011550, 0x800116B8
ROOTS = [0x800C316C, 0x800C30C0, 0x800C0AB4, 0x800AF234]
GROUPS = [0x800B061C, 0x800B04A0, 0x800AF240, 0x800B0500]

fb = lambda v: struct.unpack('>I', struct.pack('>f', v))[0]


def orig_table():
    s = open(os.path.join(ROOT, 'src/game/gfx.h')).read()
    return {n: int(a, 16) for n, a in re.findall(r'\b(\w+)\([^;]*?\);\s*/\* (0x[0-9A-F]{8})', s, re.S)}


ORIG = orig_table()
VOID = set(re.findall(r'^void\s+(\w+)\(', open(os.path.join(ROOT, 'src/game/gfx.h')).read(), re.M))
FLOAT = set(re.findall(r'^f32\s+(\w+)\(', open(os.path.join(ROOT, 'src/game/gfx.h')).read(), re.M))


# ---------------------------------------------------------------- stubs
class Ctx:
    rng = random.Random(0)
    log = []


def sp(e):
    return e.mu.reg_read(UC_MIPS_REG_SP)


def nostack(v):
    """Pointers to locals differ between the original and the C build."""
    return 'stack' if 0x807F0000 <= v < 0x80800000 else v


def logger(name, nargs, nstack=0, ret=None, effect=None):
    def stub(e):
        s = sp(e)
        Ctx.log.append((name, tuple(nostack(e.arg(i)) for i in range(nargs)),
                        tuple(nostack(e.r32(s + 0x10 + 4 * i)) for i in range(nstack)), e.r32(DL_HEAD)))
        if effect:
            effect(e)
        if ret is None:
            return 0
        return ret(e)
    return stub


def rret(*choices):
    return lambda e: Ctx.rng.choice(choices)


def pads_effect(e):
    a = e.arg(0)
    data = b''
    for _ in range(4):
        btn = Ctx.rng.getrandbits(16)
        sx, sy = (Ctx.rng.choice([0, 7, 8, -7, -8, 37, -37, 38, -38, 67, 68, -68, 127, -128,
                                  Ctx.rng.randrange(-128, 128)]) & 0xFF for _ in range(2))
        err = Ctx.rng.choice([0, 0, 0, 0x8, 0x4, Ctx.rng.getrandbits(8)])
        data += struct.pack('>HBBBB', btn, sx, sy, err, Ctx.rng.getrandbits(8))
    e.write(a, data[:24])


def plug_effect(e):
    e.write(e.arg(1), bytes([Ctx.rng.getrandbits(8)]))


def fileno_effect(idx):
    def f(e):
        e.w32(e.r32(sp(e) + 0x10 + 4 * idx), Ctx.rng.choice([0, 1, 5, 15, Ctx.rng.getrandbits(32)]))
    return f


PFS_RET = (0, 0, 0, 0, 1, 2, 3, 5)

STUBS = {
    0x80022048: logger('osContGetReadData', 1, effect=pads_effect),
    0x8001CF20: logger('osSendMesg', 3, ret=rret(0, -1)),
    0x8001C9C0: logger('osCreateMesgQueue', 3),
    0x8001D2E0: logger('osVirtualToPhysical', 1, ret=lambda e: e.arg(0) & 0x1FFFFFFF),
    0x8001C870: logger('osSetIntMask', 1, ret=lambda e: Ctx.rng.getrandbits(32)),
    0x80021940: logger('osViSetMode', 1),
    0x80021E40: logger('osViBlack', 1),
    0x80021990: logger('osViSetSpecialFeatures', 1),
    0x8001C990: logger('osWritebackDCacheAll', 0),
    0x8001BE98: logger('func_8001BE98', 1),
    0x8000B06C: logger('func_8000B06C', 0),
    0x80019040: logger('Obj_UpdateAll', 0),
    0x80024360: logger('osPfsIsPlug', 2, ret=rret(0, 0, 1, 1, 1, -1), effect=plug_effect),
    0x80024A30: logger('osPfsInitPak', 3, ret=rret(*PFS_RET)),
    0x800230D0: logger('osPfsAllocateFile', 4, 3, ret=rret(*PFS_RET), effect=fileno_effect(2)),
    0x800241D0: logger('osPfsFindFile', 4, 2, ret=rret(*PFS_RET), effect=fileno_effect(1)),
    0x80023B60: logger('osPfsReadWriteFile', 4, 2, ret=rret(*PFS_RET)),
    0x800236C0: logger('osPfsDeleteFile', 4, 1, ret=rret(*PFS_RET)),
    0x800229C0: logger('osPfsChecker', 1, ret=rret(*PFS_RET)),
    0x80024850: logger('osEepromProbe', 1, ret=rret(0, 1, 2)),
    0x800249C0: logger('osEepromLongRead', 4, ret=rret(0, 0, -1, 4)),
    0x800248D0: logger('osEepromLongWrite', 4, ret=rret(0, 0, -1, 4)),
}


# ---------------------------------------------------------------- random values
def rfloat(rng, special=True):
    r = rng.random()
    if special and r < 0.03: return rng.choice([float('inf'), float('-inf'), float('nan')])
    if r < 0.10: return rng.choice([0.0, -0.0, 1.0, -1.0, 0.5])
    if r < 0.30: return float(rng.randrange(-300, 300))
    if r < 0.50: return rng.uniform(-2.0, 2.0)
    return rng.uniform(-1000.0, 1000.0)


def rbytes(rng, n):
    return bytes(rng.getrandbits(8) for _ in range(n))


def w16s(e, a, vals):
    e.write(a, struct.pack('>%dH' % len(vals), *[v & 0xFFFF for v in vals]))


def w32s(e, a, vals):
    e.write(a, struct.pack('>%dI' % len(vals), *[v & 0xFFFFFFFF for v in vals]))


def wfs(e, a, vals):
    e.write(a, struct.pack('>%df' % len(vals), *vals))


# ---------------------------------------------------------------- environment
def make_baseline(emu):
    """Scene graph initialised (original code) with some drawable nodes."""
    rng = random.Random(1234)
    emu.w32(SCREEN_W, 320)
    emu.write(0x800B4AB0, bytes(0xC000))
    emu.call(SCENE_INIT)

    def append(group, node):
        tail = emu.r32(group + 0xD4)
        tp = emu.r32(tail + 4)
        emu.w32(tp, node); emu.w32(node + 4, tp)
        emu.w32(node, tail); emu.w32(tail + 4, node)

    groups = [emu.r32(a) for a in GROUPS]
    for k in range(14):
        t = [2, 2, 2, 3, 4, 1][k % 6]
        n, _ = emu.call(SCENE_NEW, a=[t])
        append(groups[rng.randrange(4)], n)
        emu.write(n + 0x0C, struct.pack('>16f', *[rng.uniform(-3, 3) for _ in range(16)]))
        if t == 2:
            for off in (0xD0, 0xD4, 0xD8):
                emu.w32(n + off, 0 if rng.random() < 0.3 else 0x80300000 + 8 * rng.randrange(0x1000))
        elif t == 3:
            emu.w32(n + 0xD0, 0x80470000 + 0x40 * k); emu.w32(n + 0xD4, rng.randrange(0, 4))
        elif t == 4:
            w16s(emu, n + 0xD0, [rng.randrange(0, 200), rng.randrange(0, 100), rng.randrange(10, 320),
                                 rng.randrange(10, 240), rng.choice([0x2000, 0x1000, 0x3000]),
                                 rng.choice([0x2000, 0x1800])])
    return [(a, emu.read(a, n)) for a, n in REGIONS]


def restore(emu, base):
    for a, b in base:
        emu.write(a, b)


def snapshot(emu, extra=()):
    return [emu.read(a, n) for a, n in list(REGIONS) + list(extra)]


def run(emu, base, fn_addr, setup, seed, extra=()):
    restore(emu, base)
    rng = random.Random(seed)
    Ctx.rng = random.Random(seed * 31 + 7)
    Ctx.log = []
    a, f = setup(emu, rng)
    v0, _ = emu.call(fn_addr, a=a, f=f)
    return v0, emu.f0_bits(), snapshot(emu, extra), list(Ctx.log)


def strip(name, res):
    v0, f0, mem, log = res
    if name in VOID: v0 = 0
    if name not in FLOAT: f0 = 0
    return v0, f0, mem, log


def diff_desc(x, y):
    if x[0] != y[0]: return f'v0 {x[0]:#x} vs {y[0]:#x}'
    if x[1] != y[1]: return f'f0 {x[1]:#x} vs {y[1]:#x}'
    regs = list(REGIONS) + [(FB0, FB_SIZE)]
    for (base, _), p, q in zip(regs, x[2], y[2]):
        if p != q:
            i = next(i for i in range(len(p)) if p[i] != q[i])
            return f'memory {base + i:#x}: {p[i:i+8].hex()} vs {q[i:i+8].hex()}'
    for i, (p, q) in enumerate(zip(x[3], y[3])):
        if p != q: return f'log[{i}] {p} vs {q}'
    return f'log length {len(x[3])} vs {len(y[3])}'


# ---------------------------------------------------------------- setups
NONE = lambda e, r: ([], [])


def s_pad_read(e, r):
    for a in (0x800A67F8,):
        e.write(a, rbytes(r, 24))
    for a in (0x800A6838, 0x800A6830, 0x800C5334, 0x800B0596, 0x800B043A, 0x800B058C, 0x800AF222,
              0x800AF238, 0x800B04AC, 0x800C5298, 0x800C30A0):
        e.write(a, rbytes(r, 8))
    for a in (0x800B04A4, 0x800C167A):
        w16s(e, a, [r.choice([0, 0, 1, 2, 8, r.getrandbits(16)]) for _ in range(4)])
    w32s(e, 0x800A6844, [r.getrandbits(32)])
    w32s(e, 0x800A67E0, [r.getrandbits(32), r.getrandbits(32)])
    return [], []


def s_si(e, r):
    e.w16(0x800A6840, r.getrandbits(16))
    e.w32(0x800C51D0, r.choice([0, 1, 2, 3, r.getrandbits(32)]))
    e.w32(0x800B4AA4, 0)
    e.w32(0x800B04D0, r.choice([0, 0, 1, r.getrandbits(32)]))
    for a in (0x800C5304, 0x800B0610, 0x800B4A38, 0x800C30D4):
        e.write(a, rbytes(r, 8))
    s_pad_read(e, r)
    return [], []


def s_coin_rates(e, r):
    for a in (0x800B0428, 0x800AF264, 0x800C51C4, 0x800B056C):
        e.write(a, rbytes(r, 8))
    e.w16(0x800C1678, r.getrandbits(16))
    return [], []


def s_io(e, r):
    e.write(IO, rbytes(r, 16))
    for a in (0x800B0498, 0x800C52A8, 0x800C532C, 0x800B4A80):
        w16s(e, a, [r.choice([0, r.getrandbits(16), r.getrandbits(9)]) for _ in range(2)])
    w16s(e, 0x800C52F0, [r.choice([0, 0, 1, 2, 8, r.getrandbits(16)]) for _ in range(2)])
    e.w16(0x800C52A0, r.getrandbits(16))
    return [], []


def s_coin(e, r):
    e.write(IO, rbytes(r, 16))
    small = lambda: r.choice([0, 0, 1, 1, 2, 3, r.getrandbits(16)])
    e.w16(0x800C16E0, r.choice([0, 0, 1, r.getrandbits(16)]))
    for a in (0x800C0AB0, 0x800B0438, 0x800C1898, 0x800C30C4, 0x800C31B0, 0x800AF220, 0x800C178C):
        e.w16(a, small())
    for a in (0x800C0D4C, 0x800C16E2, 0x800C53D8, 0x800B0574, 0x800C1654):
        w16s(e, a, [small(), small()])
    w16s(e, 0x800B0274, [r.choice([0, 0, 1, 0x20, 0x21, 0x3F, r.getrandbits(16)]) for _ in range(2)])
    w32s(e, 0x800B04F8, [r.choice([0, 1, 2, 3, 28, 29, 30, 31, r.getrandbits(32)]) for _ in range(2)])
    w32s(e, 0x800C16B8, [r.getrandbits(32) for _ in range(2)])
    w32s(e, 0x800C52F4, [r.choice([0, 0, 1, 2, 3, 5, r.getrandbits(32)]) for _ in range(2)])
    w32s(e, 0x800B4A30, [r.choice([0, 1, r.getrandbits(32)]) for _ in range(2)])
    for a in (0x800AF264, 0x800B0428, 0x800B056C, 0x800C51C4):
        w32s(e, a, [r.choice([0, 1, 2, 3, r.getrandbits(32)]) for _ in range(2)])
    w32s(e, 0x800C30C8, [r.getrandbits(32) for _ in range(2)])
    e.w16(0x800C1678, r.choice([0xFFFF, r.getrandbits(16)]))
    return [], []


def s_screen(e, r):
    e.w32(SCREEN_W, r.choice([320, 640, 320, r.randrange(1, 1024), r.getrandbits(32)]))
    e.w32(SCREEN_H, r.choice([240, 480, 240, r.randrange(1, 1024)]))
    e.w32(MODE, r.choice([0, 1, 2, 3, 4, r.getrandbits(32)]))


def s_video_init(e, r):
    e.write(FB0, bytes([r.getrandbits(8) | 1]) * FB_SIZE)
    e.write(RDPBUF, rbytes(r, 64) * ((RDPBUF_END - RDPBUF) // 64))
    s_screen(e, r)
    e.w32(0x800386C0, r.randrange(-100, 100)); e.w32(0x800386C4, r.randrange(-100, 100))
    return [], []


def s_vi_mode(e, r):
    s_screen(e, r)
    for a in (0x800A6850, 0x800A68A8):
        e.write(a, rbytes(r, 0x50))
    e.w32(0x800A68A0, r.choice([0x8003A3E0, 0x8003A430, 0x80410000, 0x800A6850]))
    e.w32(0x800386C0, r.choice([0, r.randrange(-100, 100), r.getrandbits(32)]))
    e.w32(0x800386C4, r.choice([0, r.randrange(-100, 100), r.getrandbits(32)]))
    return [], []


def s_vi_start(e, r):
    s_vi_mode(e, r)
    e.w32(0x800A68A0, r.choice([0x80410000, 0x800A68A8, 0x800A6850]))
    return [], []


GFX_BUF = 0x80420000


def s_full_vp(e, r):
    s_screen(e, r)
    e.write(0x800A68F8, rbytes(r, 0x18))
    e.write(GFX_BUF, rbytes(r, 0x40))
    return [GFX_BUF], []


def s_vp(e, r):
    s_screen(e, r)
    e.w32(0x800A6908, r.choice([0, 1, 5, 7, 8, 9, 100]))
    e.write(0x800A6910, rbytes(r, 0x80))
    e.write(GFX_BUF, rbytes(r, 0x40))
    v = lambda: r.choice([0, 1, 10, 100, 320, 0x2000, -5, r.getrandbits(16), r.getrandbits(32)])
    return [GFX_BUF, v(), v(), v(), v(), r.choice([0x2000, 0x1000, v()]), r.choice([0x2000, 0x4000, v()])], []


NODE = 0x80430000
XF = 0x80431000


def s_quad(nbytes, extra_bytes=0, dl=False, xform=False):
    def s(e, r):
        e.write(NODE, rbytes(r, 0xE0))
        wfs(e, NODE + 0x0C, [rfloat(r, special=r.random() < 0.1) for _ in range(16)])
        for a in (0x800386D0, 0x80038710, 0x80038750, 0x80038790):
            e.write(a, rbytes(r, 0x40))
        args = [NODE]
        if dl:
            args.append(0x80300000 | r.getrandbits(20))
        args += [r.getrandbits(32) if r.random() < 0.5 else r.getrandbits(8) for _ in range(nbytes)]
        if xform:
            wfs(e, XF, [rfloat(r) for _ in range(9)])
            args.append(XF)
        else:
            args += [fb(rfloat(r)) for _ in range(4)]
        return args, []
    return s


BUF = 0x80440000


def s_pfs(nargs):
    def s(e, r):
        s_si(e, r)
        e.write(0x800A6990, rbytes(r, 0x68 * 4))
        e.write(BUF, rbytes(r, 0x200))
        args = [r.randrange(0, 4)]
        if nargs >= 2:
            args.append(r.choice([0, 0x100, r.getrandbits(16)]) if nargs == 2 else BUF)
        if nargs == 4:
            args += [r.choice([0, 0x20, r.getrandbits(12)]), r.choice([0x20, 0x100, r.getrandbits(12)])]
        return args, []
    return s


def s_pfs_err(e, r):
    pfs = 0x80450000
    e.write(pfs, rbytes(r, 0x68))
    return [r.choice([0, 1, 2, 2, 3, 3, 4, 5, -1, r.getrandbits(32)]), pfs, 0x8003D7C0], []


def s_eeprom(e, r):
    s_si(e, r)
    e.w32(0x800B0544, r.getrandbits(32))
    return [BUF], []


P, A, B, O = 0x80460000, 0x80460020, 0x80460040, 0x80460060


def s_line_param(e, r):
    wfs(e, P, [rfloat(r) for _ in range(3)])
    d = [rfloat(r) for _ in range(3)]
    if r.random() < 0.1: d = [0.0, 0.0, r.choice([0.0, 1e-9, 1e-8])]
    wfs(e, A, [rfloat(r) for _ in range(3)] + d)
    return [P, A], []


def s_segment(e, r):
    wfs(e, P, [rfloat(r) for _ in range(3)])
    a = [rfloat(r) for _ in range(3)]
    b = [rfloat(r) for _ in range(3)] if r.random() > 0.1 else list(a)
    if r.random() < 0.3:  # point near the segment
        t = r.uniform(-0.5, 1.5)
        wfs(e, P, [a[i] + (b[i] - a[i]) * t + r.uniform(-5, 5) for i in range(3)])
    wfs(e, A, a); wfs(e, B, b)
    return [P, A, B], []


def s_plane(e, r):
    wfs(e, P, [rfloat(r) for _ in range(3)])
    n = [rfloat(r) for _ in range(3)]
    if r.random() < 0.1: n = [0.0, 0.0, r.choice([0.0, 1e-9])]
    wfs(e, A, n + [rfloat(r)])
    e.write(O, rbytes(r, 16))
    out = r.choice([O, O, O, P, A])
    return [out, P, A], []


def s_plane_d(e, r):
    wfs(e, P, [rfloat(r) for _ in range(3)])
    n = [rfloat(r) for _ in range(3)]
    if r.random() < 0.1: n = [0.0, 0.0, r.choice([0.0, 1e-9])]
    e.write(A, struct.pack('>4d', *(n + [rfloat(r)])))
    e.write(O, rbytes(r, 32))
    out = r.choice([O, O, O, A])
    return [out, P, A], []


def s_cross(e, r):
    for a in (A, B, O):
        wfs(e, a, [rfloat(r) for _ in range(3)])
    a, b = r.choice([(A, B), (A, B), (A, A)])
    out = r.choice([O, O, a, b])
    return [out, a, b], []


TASK = 0x80400000


def s_render(e, r):
    e.write(TASK, rbytes(r, 0x2180))
    e.w32(TASK + 0x2178, 0x80300000 + 0x40 * r.randrange(0x1000))
    s_screen(e, r)
    if r.random() < 0.8:
        e.w32(SCREEN_W, r.choice([320, 640])); e.w32(SCREEN_H, r.choice([240, 480]))
    e.w32(MODE, r.choice([0, 1, 2, 3, 4, -1]))
    e.w16(0x800C532E, r.getrandbits(16))
    e.w32(0x800B0540, r.getrandbits(32))
    e.w32(0x800386C8, r.choice([0, 1]))
    e.w32(0x800B05A0, r.choice([0, 1, 2]))
    for a in (0x800B0724, 0x800B0618, 0x800B0594):
        e.write(a, rbytes(r, 1))
    e.wf(0x800C51D4, r.choice([33.0, r.uniform(10, 90), rfloat(r)]))
    e.w32(0x800C1788, r.choice([10, r.randrange(1, 100), 0]))
    e.w32(0x800C53D0, r.choice([3000, r.randrange(100, 10000)]))
    for a in (0x800B0444, 0x800B04B8, 0x800C1664):
        wfs(e, a, [rfloat(r, special=r.random() < 0.1) for _ in range(3)])
    e.w32(0x800B0548, r.choice([0, 0, 0, 1]))
    wfs(e, 0x800C1730, [rfloat(r) for _ in range(9)] + [r.uniform(0, 255) for _ in range(6)])
    e.w32(0x800A6908, r.randrange(0, 10))
    e.w32(0x800B0568, r.getrandbits(32))
    e.w32(0x800C1650, r.getrandbits(32))
    return [TASK], []


# ---------------------------------------------------------------- main
def main(trials=300):
    blob, syms = build_clean(['src/game/gfx.c'], name='gfx')
    emu = make_env()
    emu.mu.mem_map(IO, IO_SIZE)
    emu.write(CODE_BASE, blob)
    for addr, fn in STUBS.items():
        emu.stub(addr, fn)
    base = make_baseline(emu)

    tests = [
        ('Pad_Read', s_pad_read, trials), ('Pad_ClearState', s_si, trials),
        ('Si_IsReleased', s_si, trials), ('Si_Release', s_si, trials), ('Si_Acquire', s_si, trials),
        ('Coin_InitRates', s_coin_rates, trials), ('Io_ReadButtons', s_io, trials),
        ('Io_ReadDipSwitches', s_io, trials), ('Coin_Update', s_coin, trials * 2),
        ('Video_Init', s_video_init, 40), ('Gfx_RenderFrame', s_render, trials),
        ('Gfx_SetFullViewport', s_full_vp, trials), ('Gfx_SetViewport', s_vp, trials),
        ('Video_ApplyMode', s_vi_mode, trials), ('Video_ApplyHStart', s_vi_start, trials),
        ('Video_ApplyVStart', s_vi_start, trials),
        ('ScreenQuad_SetAlpha0', s_quad(1), trials), ('ScreenQuad_SetAlpha1', s_quad(1), trials),
        ('ScreenQuad_SetColor2', s_quad(3), trials), ('ScreenQuad_SetColorA2', s_quad(4), trials),
        ('ScreenQuad_SetColor3', s_quad(3), trials), ('ScreenQuad_SetColorA3', s_quad(4), trials),
        ('ScreenQuad_SetDL', s_quad(0, dl=True), trials),
        ('ScreenQuad_SetDLXform', s_quad(0, dl=True, xform=True), trials),
        ('Pfs_Init', NONE, 20), ('Pfs_CreateFile', s_pfs(2), trials), ('Pfs_ReadFile', s_pfs(4), trials),
        ('Pfs_WriteFile', s_pfs(4), trials), ('Pfs_DeleteFile', s_pfs(1), trials),
        ('Pfs_GetPluggedMask', s_si, trials), ('Pfs_CheckFile', s_pfs(1), trials),
        ('Pfs_Nop', NONE, 20), ('Pfs_HandleError', s_pfs_err, trials),
        ('Eeprom_Probe', s_eeprom, trials), ('Eeprom_Read', s_eeprom, trials),
        ('Eeprom_Write', s_eeprom, trials),
        ('Line3_ProjectParam', s_line_param, trials), ('Segment3_DistanceToPoint', s_segment, trials),
        ('Plane_ProjectPoint', s_plane, trials), ('Plane_ProjectPointD', s_plane_d, trials),
        ('Vec3_Cross', s_cross, trials),
    ]
    only = set(sys.argv[1:])
    fails = 0
    for name, setup, n in tests:
        if only and name not in only:
            continue
        extra = [(FB0, FB_SIZE)] if name == 'Video_Init' else []
        bad = 0
        for s in range(n):
            x = strip(name, run(emu, base, ORIG[name], setup, s, extra))
            y = strip(name, run(emu, base, syms[name], setup, s, extra))
            if x != y:
                if not bad:
                    print(f'  {name}: seed {s}: {diff_desc(x, y)}')
                bad += 1
        print(f'{name:26s} {"PASS" if not bad else "FAIL"} {n - bad}/{n}')
        fails += bad
    missing = set(ORIG) - {t[0] for t in tests}
    if missing:
        print('untested:', sorted(missing)); fails += 1
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
