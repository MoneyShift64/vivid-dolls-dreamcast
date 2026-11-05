#!/usr/bin/env python3
"""Differential test of src/game/ovl1_attract.c (overlay 1, 0x800E0000-0x800E0FC0: boot
logo, title screen, stage / bonus intros) against the original MIPS code.

Overlay 1 is loaded at 0x800E0000. Everything the code calls that touches the task
system, objects, scenes, the photo view or the photo decoder is a Python stub that
records its arguments; the call logs are compared. Heap_Alloc / Heap_Free run for real.
Task_Yield runs a per-frame script (credits, buttons, service flags) that depends only
on the frame number, so both versions see the same inputs. Main BSS, overlay 1 data +
BSS, the test memory and (for the title) the game heap are compared.

The photo decoder is never run: Jpeg64_Decode is a stub that only reports a size,
so no ROM photo data is ever read or decoded.

    python3 tools/difftest/test_ovl1.py        (ONLY=Name,Name to select)
"""
import os, random, struct, sys, zlib
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env, reset_heap, GAME_HEAP, GAME_HEAP_SIZE
from mipsbuild import CODE_BASE
from draft_mips import build_clean, symbol_map
from unicorn.mips_const import (UC_MIPS_REG_SP, UC_MIPS_REG_S0, UC_MIPS_REG_S1, UC_MIPS_REG_S2,
                                UC_MIPS_REG_S3, UC_MIPS_REG_S4, UC_MIPS_REG_S5, UC_MIPS_REG_S6,
                                UC_MIPS_REG_S7)

SM = symbol_map()
BSS, BSS_SIZE = 0x8003E130, 0x800C5400 - 0x8003E130
OVL, OVL_SIZE = 0x800E0FC0, 0x800E1900 - 0x800E0FC0      # overlay 1 data + BSS
TEST, TEST_SIZE = 0x80400000, 0x100000
STACK_LO, STACK_SIZE = 0x807F0000, 0xF000
SREGS = [UC_MIPS_REG_S0, UC_MIPS_REG_S1, UC_MIPS_REG_S2, UC_MIPS_REG_S3, UC_MIPS_REG_S4,
         UC_MIPS_REG_S5, UC_MIPS_REG_S6, UC_MIPS_REG_S7]

CREDITS, COINS = 0x800C30C8, 0x800C52F4
PERCREDIT, ALTSEL, ALTPER = 0x800B056C, 0x800B4A30, 0x800AF264
SVC, SVCREQ, SVCREQ2 = 0x800C30C4, 0x800C1674, 0x800AF260
BTN1, BTN2 = 0x800C0D48, 0x800C0D4A
GIRL, PHOTO = 0x800B0608, 0x800B04CC

FAKE_VIEW = TEST + 0xF0000
FAKE_OBJ = 0x81000000 - 0x100                       # never dereferenced (stubs)

ORIG = {'Attract_Run': 0x800E0000, 'Attract_UpdateCredits': 0x800E00E4,
        'Title_Main': 0x800E0288, 'StageIntro_Main': 0x800E0970,
        'BonusIntro_Main': 0x800E0C80, 'Boot_Logo': 0x800E0F40}

STUBS = {'Obj_SetData': 2, 'Obj_SetPos': 4, 'Obj_Show': 1, 'Obj_Hide': 1, 'Obj_Kill': 1,
         'Obj_Invalidate': 1, 'Scene_Build': 2, 'Scene_Destroy': 2, 'Camera_Init': 0,
         'Light_Init': 1, 'Image_CopyRect': 12, 'PhotoView_Delete': 1}


class Env:
    def __init__(self):
        blob, self.syms = build_clean(['src/game/ovl1_attract.c'], name='ovl1_attract')
        self.c2o = {self.syms[n]: a for n, a in ORIG.items()}
        e = self.emu = make_env(overlay=1)
        e.write(CODE_BASE, blob)
        self.log = []
        self.script = None
        self.nyield = 0
        self.done_ptr = []
        self.jpeg_size = (320, 240)
        for name, n in STUBS.items():
            e.stub(SM[name], self._logger(name, n))
        e.stub(SM['Obj_NewDL'], self._newdl)
        e.stub(SM['PhotoView_New'], self._viewnew)
        e.stub(SM['Jpeg64_Decode'], self._jpeg)
        e.stub(SM['Task_Create'], self._spawn)
        e.stub(SM['Task_Yield'], self._yield)
        reset_heap(e)
        self.regions = [(BSS, BSS_SIZE), (OVL, OVL_SIZE), (TEST, TEST_SIZE),
                        (GAME_HEAP, GAME_HEAP_SIZE)]
        self.snap = [e.read(a, n) for a, n in self.regions]
        self.snap[2] = bytes(TEST_SIZE)

    def _args(self, n):
        e = self.emu
        sp = e.mu.reg_read(UC_MIPS_REG_SP)
        return [e.arg(i) for i in range(min(n, 4))] + [e.r32(sp + 0x10 + 4 * i) for i in range(max(0, n - 4))]

    def _logger(self, name, n):
        def f(e):
            self.log.append((name, tuple(self._args(n))))
            return 0
        return f

    def _newdl(self, e):
        self.log.append(('Obj_NewDL', tuple(self._args(4))))
        return FAKE_OBJ

    def _viewnew(self, e):
        self.log.append(('PhotoView_New', tuple(self._args(6))))
        return FAKE_VIEW

    def _jpeg(self, e):
        a = self._args(6)
        self.log.append(('Jpeg64_Decode', tuple(a[:4])))
        e.w32(a[4], self.jpeg_size[0])
        e.w32(a[5], self.jpeg_size[1])

    def _spawn(self, e):
        a = self._args(5)
        fn = self.c2o.get(a[0], a[0])
        # a[3] is &done on the caller's stack: logged as 'STACK' (frame layouts differ)
        self.log.append(('spawn', fn, a[1], a[2], 'STACK' if a[3] >= STACK_LO else a[3], a[4]))
        self.done_ptr.append(a[3])
        return 0x80480000

    def _yield(self, e):
        self.log.append(('yield',))
        self.nyield += 1
        if self.script:
            self.script(e, self.nyield)

    def restore(self):
        for (a, n), b in zip(self.regions, self.snap):
            self.emu.write(a, b)
        self.log = []
        self.nyield = 0
        self.done_ptr = []


def run(env, T, name, trials, setup, args_fn, ret=False, heap=False, max_insns=60_000_000, cover=None):
    e = env.emu
    if os.environ.get('ONLY') and name not in os.environ['ONLY'].split(','):
        return
    ok = n = 0
    rng = random.Random(zlib.crc32(name.encode()))
    first = None
    for t in range(trials):
        seed = rng.getrandbits(32)
        res = []
        for addr in (ORIG[name], env.syms[name]):
            env.restore()
            r = random.Random(seed)
            setup(r, t)
            a = args_fn(r)
            e.write(STACK_LO, bytes(STACK_SIZE))
            for reg in SREGS:
                e.mu.reg_write(reg, 0)
            try:
                v0, _ = e.call(addr, a=a, max_insns=max_insns)
                err = None
            except Exception as ex:      # noqa
                v0, err = None, str(ex)
            regs = [e.read(b, s) for b, s in env.regions[:3]]
            if heap:
                regs.append(e.read(GAME_HEAP, GAME_HEAP_SIZE))
            res.append((v0, list(env.log), regs, err))
        (v1, l1, s1, e1), (v2, l2, s2, e2) = res
        if e1 is not None:
            continue
        n += 1
        msg = None
        if e2 is not None:
            msg = 'C faulted: ' + e2
        elif ret and (v1 & 0xFFFFFFFF) != (v2 & 0xFFFFFFFF):
            msg = f'v0 {v1:08X} vs {v2:08X}'
        elif l1 != l2:
            k = next((i for i in range(min(len(l1), len(l2))) if l1[i] != l2[i]), min(len(l1), len(l2)))
            msg = f'call log differs at {k}: {l1[k:k+2]} vs {l2[k:k+2]} (len {len(l1)} vs {len(l2)})'
        else:
            for nm, base, x, y in zip(['BSS', 'OVL', 'TEST', 'HEAP'], [BSS, OVL, TEST, GAME_HEAP], s1, s2):
                if x != y:
                    i = next(k for k in range(len(x)) if x[k] != y[k])
                    msg = f'{nm} differs at {base + i:08X}: {x[i]:02X} vs {y[i]:02X}'
                    break
        if cover is not None:
            cover(l1, e.r32(TEST))
        if msg is None:
            ok += 1
        elif first is None:
            first = f'trial {t}: {msg}'
    status = 'PASS' if ok == n and n > 0 else 'FAIL'
    print(f'{name:22s} {status} {ok}/{n}' + (f'   ({first})' if first else ''))
    if status != 'PASS':
        T['fail'] += 1


def fill_objs(e, base, count, real=False):
    """object pointer slots: fake addresses (only passed to stubs) or real Obj blocks"""
    for i in range(count):
        e.w32(base + 4 * i, (TEST + 0x1000 + 0x100 * i) if real else (0x80600000 + 0x100 * i))


def common_globals(e, r):
    e.w32(CREDITS, r.choice([0, 0, 0, 1, 2, 9, 10, 99, 100, 150, r.randrange(-12, 200)]))
    e.w32(COINS, r.randrange(0, 10))
    e.w32(PERCREDIT, r.choice([0, 1, 2, 3, 4, r.randrange(0, 10)]))
    e.w32(ALTSEL, r.choice([0, 0, 1]))
    e.w32(ALTPER, r.randrange(0, 10))
    e.w16(SVC, 0)
    e.w32(SVCREQ, 0)
    e.w32(SVCREQ2, 0)
    e.w16(BTN1, 0)
    e.w16(BTN2, 0)
    for i in range(14):                         # digit sprites 0x800E17F0.. and "/" 0x800E1818
        e.w32(0x800E17F0 + 4 * i, 0x80610000 + 0x40 * i)


def main():
    env = Env()
    e = env.emu
    T = {'fail': 0}
    N = int(os.environ.get('TRIALS', 300))
    NT = int(os.environ.get('TASK_TRIALS', 120))

    # --- credits display
    def credits_setup(r, t):
        common_globals(e, r)
        fill_objs(e, 0x800E17A0, 12)
        env.script = None
    run(env, T, 'Attract_UpdateCredits', N, credits_setup, lambda r: [])

    # --- overlay entry (args[0] = 0..3; other values wait forever in the original)
    def entry_setup(r, t):
        k = r.randrange(0, 6)
        val = r.choice([0x10000, 0x10001, 0x10002, 0x10004, 0x12345678, 1])

        def script(em, ny):
            if ny > k and env.done_ptr:
                em.w32(env.done_ptr[0], val)
        env.script = script
        e.w32(0x800C51CC, r.getrandbits(32))
        e.write(TEST, struct.pack('>II', t % 4, r.getrandbits(32)))
    run(env, T, 'Attract_Run', N, entry_setup, lambda r: [TEST], ret=True)

    # --- frame scripts for the tasks
    def make_script(r, plan, limit):
        seed_base = r.getrandbits(32)
        ev = r.randrange(1, 140)

        def script(em, ny):
            rr = random.Random(seed_base * 7919 + ny)
            if plan in ('credit', 'start') and ny == ev:
                em.w32(CREDITS, rr.choice([1, 2, 5]))
            if plan == 'start' and ny > ev and rr.random() < 0.1:
                em.w16(BTN1, rr.choice([0, 0x100, 0x110, 0x80]))
                em.w16(BTN2, rr.choice([0, 0x100, 0x20]))
            if plan == 'noise' and rr.random() < 0.05:
                em.w32(CREDITS, rr.choice([0, 0, 1, 3, 120]))
                em.w32(COINS, rr.randrange(0, 10))
                em.w16(BTN1, rr.choice([0, 0x100]))
                em.w16(BTN2, rr.choice([0, 0x100]))
            if plan == 'svc' and ny == ev:
                em.w16(SVC, rr.choice([1, 0x8000]))
            if plan == 'svc2' and ny == ev:
                em.w32(SVCREQ2, rr.choice([1, -1]))
            if ny > limit:
                em.w32(SVCREQ2, 1)
        return script

    # --- title screen
    def title_setup(r, t):
        common_globals(e, r)
        if r.random() < 0.6:
            e.w32(CREDITS, 0)
        fill_objs(e, 0x800E17A0, 12)
        e.w32(FAKE_VIEW + 0, 0x80610000)
        e.w32(FAKE_VIEW + 4, 0x80610100)
        e.w32(FAKE_VIEW + 8, 0x80610200)
        env.jpeg_size = r.choice([(320, 240), (640, 240), (320, 480), (640, 480)])
        plan = r.choice(['idle', 'credit', 'start', 'start', 'start', 'noise', 'svc', 'svc2'])
        if plan == 'svc' and r.random() < 0.2:
            e.w16(SVC, 1)              # raised before the task starts
        env.script = make_script(r, plan, r.randrange(150, 700))
        e.w32(TEST, 0xDEADBEEF)
    results = {}

    def title_cover(log, res):
        results[res] = results.get(res, 0) + 1
    run(env, T, 'Title_Main', NT, title_setup, lambda r: [TEST], heap=True,
        max_insns=200_000_000, cover=title_cover)
    if results:
        print('    Title_Main results:', ', '.join(f'{k:#x} x{v}' for k, v in sorted(results.items())))

    # --- intros
    def intro_setup(r, t):
        common_globals(e, r)
        fill_objs(e, 0x800E1820, 9, real=True)
        fill_objs(e, 0x800E18A0, 11, real=True)
        for i, a in enumerate(range(0x800E1848, 0x800E1898, 4)):   # intro sprites / digits
            e.w32(a, 0x80620000 + 0x40 * i)
        e.write(TEST + 0x1000, bytes(r.getrandbits(8) for _ in range(0xB00)))
        e.w32(GIRL, r.randrange(0, 8))
        e.w32(PHOTO, r.randrange(0, 3))
        plan = r.choice(['run', 'run', 'noise', 'svc', 'svc2'])
        env.script = make_script(r, plan, 10_000)
        e.w32(TEST, 0xDEADBEEF)
    run(env, T, 'StageIntro_Main', NT, intro_setup, lambda r: [TEST])
    run(env, T, 'BonusIntro_Main', NT, intro_setup, lambda r: [TEST])

    # --- boot logo
    def boot_setup(r, t):
        common_globals(e, r)
        env.script = make_script(r, r.choice(['noise', 'svc', 'svc2']), 10_000)
        e.w32(TEST, r.getrandbits(32))
    run(env, T, 'Boot_Logo', 20, boot_setup, lambda r: [TEST])

    env.script = None
    print('ALL PASS' if T['fail'] == 0 else f"{T['fail']} FAILED")
    sys.exit(1 if T['fail'] else 0)


if __name__ == '__main__':
    main()
