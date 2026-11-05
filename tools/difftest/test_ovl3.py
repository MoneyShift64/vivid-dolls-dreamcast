#!/usr/bin/env python3
"""Differential test of src/game/ovl3_select.c (overlay 3: girl select screen)
against the original MIPS code, with overlay 3 loaded.

Obj_*, Scene_Build/Destroy, sound, Camera/Light init, Task_Create and the per-frame
Task_Yield are Python stubs that log their arguments (C function pointers are mapped
back to the original addresses); cosf/sinf run for real. The logs, the result,
the whole overlay (code, data, BSS), the main BSS and the test memory are compared.

    python3 tools/difftest/test_ovl3.py [names...]     (TRIALS=n to change 300)
"""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env
from emu import STACK_TOP
from mipsbuild import CODE_BASE
from draft_mips import build_clean, symbol_map
from unicorn.mips_const import UC_MIPS_REG_SP

TRIALS = int(os.environ.get('TRIALS', '300'))
TASK_TRIALS = int(os.environ.get('TASK_TRIALS', '120'))
SMAP = symbol_map()

FUNCS = [
    ('GirlSelect_Run', 0x800E0000), ('GirlSelect_PlaceIcons', 0x800E0094),
    ('GirlSelect_ShowTimer', 0x800E01A0), ('GirlSelect_Task', 0x800E0278),
    ('GirlSelect_UpdateZoom', 0x800E09C4), ('GirlSelect_UpdateFadeOut', 0x800E0A5C),
    ('GirlSelect_UpdateArrow', 0x800E0AB8), ('GirlSelect_UpdateFrame', 0x800E0AFC),
]

OVL = (0x800E0000, 0x800E1000)        # code + data + BSS (0x800E0F30-0x800E1000)
MAIN_BSS = (0x8003E130, 0x800C5400)
TEST_REGION = (0x80400000, 0x80480000)
FAKE_OBJ = 0x80400000                 # 16 fake Obj blocks (0xB0 each)
RESULT = 0x80470000
SLOT_OBJS = [0x800E0F30, 0x800E0F34, 0x800E0F38, 0x800E0F3C] + \
            [0x800E0F40 + 4 * i for i in range(8)] + [0x800E0F60, 0x800E0F64, 0x800E0F68, 0x800E0F6C]
SPRITE_SLOTS = (0x800E0F70, 0x800E0FF8)
CLEARED = 0x800C178E
P1, P2, SERVICE, SERVICE_REQ = 0x800C0D48, 0x800C0D4A, 0x800C30C4, 0x800C1674
ACTIVE = 0x800B04D4
RETRACES = 0x800C51CC

OBJ_FUNCS = {'Obj_SetData': 2, 'Obj_SetPos': 4, 'Obj_SetUpdate': 3, 'Obj_Hide': 1, 'Obj_Show': 1,
             'Obj_SetColor': 5, 'Obj_UseSetupCC0': 1, 'Obj_ClearUpdate': 1, 'Obj_Invalidate': 1}


def f2u(v):
    return struct.unpack('>I', struct.pack('>f', v))[0]


class Ctx:
    def __init__(self, rng_seed, on_yield=None, max_yields=None):
        self.rng = random.Random(rng_seed)
        self.log = []
        self.yields = 0
        self.on_yield = on_yield
        self.max_yields = max_yields
        self.task_done = None
        self.voice = 0x100


class Runner:
    def __init__(self):
        self.emu = make_env(overlay=3)
        blob, self.syms = build_clean(['src/game/ovl3_select.c'], name='ovl3')
        self.emu.write(CODE_BASE, blob)
        self.cmap = {self.syms[n]: a for n, a in FUNCS}
        self.ctx = [None]
        self.install_stubs()

    def fn(self, v):
        return self.cmap.get(v, v)

    def install_stubs(self):
        emu, ctx_ref = self.emu, self.ctx

        def stack_arg(e, i):
            return e.r32(e.mu.reg_read(UC_MIPS_REG_SP) + 0x10 + 4 * (i - 4))

        def mk_obj(name, n):
            def f(e):
                c = ctx_ref[0]
                args = [e.arg(i) if i < 4 else stack_arg(e, i) for i in range(n)]
                if name == 'Obj_SetUpdate':
                    args[1] = self.fn(args[1])
                    e.w32(args[0] + 0x64, args[1])
                    e.w32(args[0] + 0x68, args[2])
                c.log.append((name, tuple(args)))
                return args[0]
            return f
        for name, n in OBJ_FUNCS.items():
            emu.stub(SMAP[name], mk_obj(name, n))

        def snd(e):
            c = ctx_ref[0]
            c.voice += 1
            c.log.append(('snd', e.arg(0)))
            return c.voice
        emu.stub(SMAP['Sound_Play'], snd)

        def mk_log(tag, n):
            def f(e):
                ctx_ref[0].log.append((tag,) + tuple(e.arg(i) for i in range(n)))
            return f
        emu.stub(SMAP['Scene_Build'], mk_log('scenebuild', 2))
        emu.stub(SMAP['Scene_Destroy'], mk_log('scenedestroy', 2))
        emu.stub(SMAP['Camera_Init'], mk_log('camera', 0))
        emu.stub(SMAP['Light_Init'], mk_log('light', 1))

        def task_new(e):
            c = ctx_ref[0]
            # the result pointer is on the caller's stack: log only that it is non-NULL
            c.log.append(('tasknew', self.fn(e.arg(0)), e.arg(1), e.arg(2), e.arg(3) != 0,
                          stack_arg(e, 4)))
            c.task_done = (e.arg(3), c.yields + c.rng.randrange(0, 40),
                           c.rng.choice([1, 0x10000, 0x10007, 0x12345678, 0xFFFF0000, 0x1FFFF]))
            return 0x80460000
        emu.stub(SMAP['Task_Create'], task_new)

        def yld(e):
            c = ctx_ref[0]
            c.yields += 1
            c.log.append(('yield', c.yields))
            if c.task_done and c.yields >= c.task_done[1]:
                e.w32(c.task_done[0], c.task_done[2])
                c.task_done = None
            if c.on_yield:
                c.on_yield(e, c)
            if c.max_yields is not None and c.yields >= c.max_yields:
                e.mu.emu_stop()
            if c.yields > 5000:
                raise RuntimeError('too many yields')
        emu.stub(SMAP['Task_Yield'], yld)

    def snapshot(self):
        return [(a, self.emu.read(a, b - a)) for a, b in (OVL, MAIN_BSS, TEST_REGION)]

    def restore(self, snap):
        for a, d in snap:
            self.emu.write(a, d)

    def run(self, name, setup, seed, ret=False):
        emu = self.emu
        rng = random.Random(seed)
        args, kw = setup(emu, rng)
        if kw.get('max_yields') is not None:
            ret = False             # stopped from a stub: v0 is meaningless
        snap = self.snapshot()
        res = []
        for addr in (dict(FUNCS)[name], self.syms[name]):
            self.restore(snap)
            self.ctx[0] = Ctx(seed ^ 0x3C3C, **kw)
            try:
                v0, _ = emu.call(addr, a=args)
            except Exception as ex:
                return f'{addr:#x}: {ex!r}'
            res.append((v0, emu.f0_bits(), self.ctx[0].log, self.snapshot()))
        (va, fa, la, sa), (vb, fb, lb, sb) = res
        if ret and va != vb:
            return f'v0 {va:#x} != {vb:#x}'
        if la != lb:
            for k, (x, y) in enumerate(zip(la, lb)):
                if x != y:
                    return f'log[{k}] {x} != {y}'
            return f'log length {len(la)} != {len(lb)}'
        for (a, da), (_, db) in zip(sa, sb):
            if da != db:
                k = next(i for i in range(len(da)) if da[i] != db[i])
                return f'mem @{a + k:#x}: {da[k:k+8].hex()} != {db[k:k+8].hex()}'
        return None


def setup_world(emu, rng):
    emu.write(TEST_REGION[0], bytes(TEST_REGION[1] - TEST_REGION[0]))
    for i in range(16):
        obj = FAKE_OBJ + i * 0xB0
        for k in range(0, 0xB0, 4):
            emu.w32(obj + k, rng.getrandbits(32))
    for i, s in enumerate(SLOT_OBJS):
        emu.w32(s, FAKE_OBJ + i * 0xB0)
    for a in range(SPRITE_SLOTS[0], SPRITE_SLOTS[1], 4):
        emu.w32(a, 0x81000000 + (a & 0xFFF))
    mode = rng.random()
    for i in range(8):
        if mode < 0.15:
            v = 0
        elif mode < 0.25:
            v = 1 if i != 7 else rng.choice([0, 1])
        elif mode < 0.3:
            v = rng.choice([1, 2, 0xFF])
        else:
            v = rng.choice([0, 0, 1])
        emu.write(CLEARED + i, bytes([v]))
    emu.w32(ACTIVE, rng.choice([0, 1, 3]))
    emu.w32(ACTIVE + 4, rng.choice([0, 1, 2]))
    emu.w16(P1, 0)
    emu.w16(P2, 0)
    emu.w16(SERVICE, 0)
    emu.w32(SERVICE_REQ, 0)
    emu.w32(RETRACES, rng.choice([1, 2, 3]))


def main():
    R = Runner()
    want = sys.argv[1:]
    fails = 0

    def s_place(emu, rng):
        setup_world(emu, rng)
        return [rng.choice([0, 0x80, 0x1FF, 0x200, -1, rng.randrange(0, 0x200),
                            rng.randrange(-100000, 100000)]) & 0xFFFFFFFF], {}

    def s_timer(emu, rng):
        setup_world(emu, rng)
        # (the game only passes 0..658; far-out values would index outside memory)
        return [rng.choice([0, 59, 60, 599, 600, 659, 658, 5999, 6000, -1, -60, -61, -600, -601,
                            rng.randrange(0, 700), rng.randrange(-20000, 20000)]) & 0xFFFFFFFF], {}

    def s_obj(i_slot=None):
        def f(emu, rng):
            setup_world(emu, rng)
            obj = FAKE_OBJ + rng.randrange(16) * 0xB0
            for off in (0x78, 0x7C, 0x9C, 0xA0):
                emu.w32(obj + off, f2u(rng.choice([rng.uniform(-400, 400), 0.0, 1.0, 2.0, 100.0, 120.0,
                                                    rng.uniform(0, 3)])))
            emu.w32(obj + 0xA8, rng.choice([0, 1, 3, 4, 5, 0xFF, 0xFC, 0x7FFFFFFF, 0xFFFFFFFF,
                                            0x80000000, rng.getrandbits(32), rng.randrange(0, 300)]))
            return [obj], {}
        return f

    def s_run(emu, rng):
        setup_world(emu, rng)
        emu.w32(RESULT, rng.choice([0, 0, 0, 1, 0xFFFFFFFF]))
        kw = {}
        if emu.r32(RESULT) != 0:
            kw['max_yields'] = rng.randrange(1, 30)
        return [RESULT], kw

    def s_task(emu, rng):
        setup_world(emu, rng)
        emu.w32(RESULT, 0xDEADBEEF)
        style = rng.random()
        if style < 0.3:
            p_ok, p_lr = 0.0, rng.choice([0.0, 0.0, 0.1, 0.5])     # time-out runs (long)
            if rng.random() < 0.6:      # front girl (0 at the start) cleared: fallback search
                k = rng.randrange(1, 9)
                for i in range(8):
                    emu.write(CLEARED + i, bytes([1 if i < k else rng.choice([0, 0, 1])]))
        else:
            p_ok, p_lr = rng.choice([0.002, 0.01, 0.05, 0.3]), rng.choice([0.0, 0.05, 0.2, 0.6])
        abort = rng.randrange(1, 400) if rng.random() < 0.2 else None
        bits = [0x10, 0x08, 0x04, 0x100, 0x20, 0x01, 0x0C]
        if p_ok == 0.0:
            bits = bits[1:]             # time-out runs: never press OK

        def on_yield(e, c):
            for a in (P1, P2):
                v = 0
                if c.rng.random() < p_ok:
                    v |= 0x10
                if c.rng.random() < p_lr:
                    v |= c.rng.choice([0x08, 0x04, 0x0C])
                if c.rng.random() < 0.05:
                    v |= c.rng.choice(bits)
                e.w16(a, v)
            if abort and c.yields == abort:
                e.w16(SERVICE, 1)
        return [RESULT], {'on_yield': on_yield}

    tests = [
        ('GirlSelect_Run', s_run, TRIALS, True),
        ('GirlSelect_PlaceIcons', s_place, TRIALS, False),
        ('GirlSelect_ShowTimer', s_timer, TRIALS, False),
        ('GirlSelect_UpdateZoom', s_obj(), TRIALS, False),
        ('GirlSelect_UpdateFadeOut', s_obj(), TRIALS, False),
        ('GirlSelect_UpdateArrow', s_obj(), TRIALS, False),
        ('GirlSelect_UpdateFrame', s_obj(), TRIALS, False),
        ('GirlSelect_Task', s_task, TASK_TRIALS, False),
    ]
    for name, setup, n, ret in tests:
        if want and name not in want:
            continue
        ok = 0
        first = None
        for t in range(n):
            err = R.run(name, setup, 0x0E3000 + t * 7919, ret=ret)
            if err is None:
                ok += 1
            elif first is None:
                first = f'trial {t}: {err}'
        print(f'{name:28s} {"PASS" if ok == n else "FAIL"} {ok}/{n}' + (f'  ({first})' if first else ''))
        fails += ok != n
    sys.exit(1 if fails else 0)


if __name__ == '__main__':
    main()
