#!/usr/bin/env python3
"""Differential test of src/game/mainloop.c (sound/music wrappers, game start,
Main_GameLoop) against the original MIPS code. Callees that touch hardware,
audio or overlays are Python stubs that log their arguments; the logs, return
values and all globals involved are compared. Main_GameLoop never returns: the
overlay stub feeds it scripted results and stops the emulator after N overlay
runs."""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env
from mipsbuild import CODE_BASE
from draft_mips import build_clean
from unicorn.mips_const import UC_MIPS_REG_SP, UC_MIPS_REG_A1

ORIG = {'Sound_Play': 0x8001BA90, 'Sound_PlayAlways': 0x8001BB18, 'Sound_Stop': 0x8001BB8C,
        'Bgm_Request': 0x8001BBC0, 'Bgm_Cancel': 0x8001BC00, 'Main_NullHookA': 0x8001BE80,
        'Main_NullHookB': 0x8001BE88, 'Main_NullHookC': 0x8001BE90, 'Main_NullHookD': 0x8001BE98,
        'Main_NullHookE': 0x8001BEA0, 'Game_ResetProgress': 0x8001BEB0, 'Main_Start': 0x8001BF08,
        'Main_GameLoop': 0x8001C000}

STUBS = {'func_8000AEAC': 0x8000AEAC, 'Overlay_Load': 0x8000C0D4, 'ovl_entry': 0x800E0000,
         'func_8000A76C': 0x8000A76C, 'func_8000FA34': 0x8000FA34, 'func_8000D49C': 0x8000D49C,
         'func_8000D67C': 0x8000D67C, 'func_8000D1CC': 0x8000D1CC, 'func_8000D160': 0x8000D160,
         'func_8000D6B8': 0x8000D6B8}

SETTINGS = 0x800C52A0
WORDS = {  # s32 globals touched by this file
    'D_800C51D8': 0x800C51D8, 'D_800C1674': 0x800C1674, 'D_800AF260': 0x800AF260,
    'D_800B0720': 0x800B0720, 'D_800B0608': 0x800B0608, 'D_800B04CC': 0x800B04CC,
    'D_800B04D4': 0x800B04D4, 'D_800B04D8': 0x800B04D8, 'D_800B4AA0': 0x800B4AA0,
    'D_800C3174': 0x800C3174, 'D_800C172C': 0x800C172C, 'D_800B0570': 0x800B0570,
    'D_800B056C': 0x800B056C, 'D_800C51C8': 0x800C51C8, 'D_800C51C4': 0x800C51C4,
    'D_800AF268': 0x800AF268, 'D_800AF264': 0x800AF264, 'D_800B042C': 0x800B042C,
    'D_800B0428': 0x800B0428,
}
BYTES8 = 0x800C178C           # 0x800C178E..0x800C1795 cleared on a new game (+2 guard bytes each side)


class Stop(Exception):
    pass


class Ctx:
    def __init__(self):
        blob, self.syms = build_clean(['src/game/mainloop.c'], name='mainloop')
        self.emu = make_env()
        self.emu.write(CODE_BASE, blob)
        self.log = []
        self.rng = random.Random(0)
        self.ovl_calls = 0
        self.ovl_limit = 0
        self.rets = {}
        for name, addr in STUBS.items():
            self.emu.stub(addr, self._mk(name))

    def snap(self):
        e = self.emu
        return tuple(e.r32(a) for a in WORDS.values()) + (e.read(BYTES8, 12), e.r16(SETTINGS))

    def _mk(self, name):
        def fn(e):
            if name == 'ovl_entry':
                p = e.arg(0)
                self.log.append((name, e.read(p, 12), self.snap()))
                self.ovl_calls += 1
                rng = self.rng
                # occasionally raise service requests / change counters, like overlays do
                if rng.random() < 0.06:
                    e.w32(WORDS['D_800C1674'], 1)
                if rng.random() < 0.06:
                    e.w32(WORDS['D_800AF260'], rng.choice([1, 0x100]))
                if rng.random() < 0.3:
                    e.w32(WORDS['D_800C3174'], rng.choice([0, 1, 2, 3, 4, -1]))
                if rng.random() < 0.05:
                    e.w16(SETTINGS, rng.getrandbits(16) & ~0x80 if rng.random() < .8 else rng.getrandbits(16))
                if self.ovl_calls >= self.ovl_limit:
                    e.mu.emu_stop()
                return rng.choice([0, 1, 2, 3, 4, 0xFF, 0x104, 0x1FF, 0xFF02, rng.getrandbits(32)])
            if name == 'func_8000D49C':
                sp = e.mu.reg_read(UC_MIPS_REG_SP)
                self.log.append((name, e.arg(0), e.mu.reg_read(UC_MIPS_REG_A1), e.arg(2), e.arg(3),
                                 e.r32(sp + 0x10)))
                return self.rets.get(name, 0x1234)
            if name == 'func_8000A76C':
                a0 = e.arg(0)
                if a0 == self.syms['Main_GameLoop']:
                    a0 = ORIG['Main_GameLoop']        # our own Main_GameLoop stands in for the original
                self.log.append((name, a0, e.arg(1), e.arg(2), self.snap()))
                return 0
            self.log.append((name, e.arg(0) if name in ('Overlay_Load', 'func_8000D67C', 'func_8000D1CC') else None,
                             self.snap()))
            return 0
        return fn

    def setup(self, rng):
        e = self.emu
        for a in WORDS.values():
            e.w32(a, rng.choice([0, 0, 1, 2, 7, 15, 16, -1, rng.getrandbits(32)]))
        e.write(BYTES8, bytes(rng.getrandbits(8) for _ in range(12)))
        e.w16(SETTINGS, rng.getrandbits(16))
        self.log = []


def run(ctx, f, name, seed):
    rng = random.Random(seed)
    ctx.setup(rng)
    e = ctx.emu
    if name == 'Main_GameLoop':
        e.w32(WORDS['D_800C1674'], 1 if rng.random() < 0.1 else 0)
        e.w32(WORDS['D_800AF260'], 1 if rng.random() < 0.1 else 0)
        if rng.random() < 0.8:
            e.w16(SETTINGS, rng.getrandbits(16) & ~0x80)
        ctx.rng = random.Random(seed * 7 + 1)
        ctx.ovl_calls = 0
        ctx.ovl_limit = rng.randrange(1, 80)
        e.call(f, max_insns=5_000_000)
        return ctx.log, ctx.snap()
    if name in ('Sound_Play', 'Sound_PlayAlways'):
        if rng.random() < 0.5:
            e.w32(WORDS['D_800C51D8'], 0)
        ctx.rets['func_8000D49C'] = rng.choice([0, 5, 0x7FFF, 0x8000, 0x12345, rng.getrandbits(32)])
        n = rng.choice([0, 1, 23, rng.randrange(0, 24), rng.randrange(0, 24)])
        v0, _ = e.call(f, a=[n])
        return v0, ctx.log, ctx.snap()
    if name == 'Sound_Stop':
        if rng.random() < 0.5:
            e.w32(WORDS['D_800C51D8'], 0)
        h = rng.choice([0, 1, 0x7FFE, 0x7FFF, 0x8000, 0x17FFF, -1, rng.getrandbits(32)])
        e.call(f, a=[h & 0xFFFFFFFF])
        return ctx.log, ctx.snap()
    if name == 'Bgm_Request':
        if rng.random() < 0.5:
            e.w32(WORDS['D_800C51D8'], 0)
        n = rng.choice([0, 1, 5, 6, 7, 14, rng.randrange(0, 16), -3])
        e.call(f, a=[n & 0xFFFFFFFF])
        return ctx.log, ctx.snap()
    if name == 'Bgm_Cancel':
        if rng.random() < 0.5:
            e.w32(WORDS['D_800C51D8'], 0)
    e.call(f)
    return ctx.log, ctx.snap()


TESTS = [(n, 300) for n in ORIG if n != 'Main_GameLoop'] + [('Main_GameLoop', 400)]


def main(argv):
    ctx = Ctx()
    bad_total = 0
    for name, n in TESTS:
        if argv and name not in argv:
            continue
        bad = 0
        for s in range(n):
            a = run(ctx, ORIG[name], name, s)
            b = run(ctx, ctx.syms[name], name, s)
            if a != b:
                if bad == 0:
                    print(f'  {name}: first mismatch seed {s}')
                    la, lb = a[-2], b[-2]
                    for i, (x, y) in enumerate(zip(la, lb)):
                        if x != y:
                            print('   call', i, x, '\n          ', y)
                            break
                    if len(la) != len(lb):
                        print('   log lengths', len(la), len(lb))
                bad += 1
        print(f'{name:20s} {"PASS" if not bad else "FAIL"} {n - bad}/{n}')
        bad_total += bad
    return 1 if bad_total else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
