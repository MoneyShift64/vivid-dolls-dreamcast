#!/usr/bin/env python3
"""Differential test of src/game/ovl2_d.c (overlay 2, 0x800F00F4 - end: effect
slots, projectiles, banner tasks, timer HUD, round state machines) against the
original MIPS code, with overlay 2 loaded.

Obj_* calls, sound, the task-switch (yield) and the collision helpers of other
ovl2 parts are Python stubs that log their arguments (and return deterministic
pseudo-random results); the logs, the return value, the whole overlay (code,
data, BSS), the main BSS and the test memory are compared.

    python3 tools/difftest/test_ovl2_d.py [names...]     (TRIALS=n to change 300)
"""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env, reset_heap, GAME_HEAP, GAME_HEAP_SIZE
from emu import STACK_TOP
from mipsbuild import CODE_BASE
from draft_mips import build_clean, symbol_map
from unicorn.mips_const import UC_MIPS_REG_SP, UC_MIPS_REG_F12, UC_MIPS_REG_F14

TRIALS = int(os.environ.get('TRIALS', '300'))
SMAP = symbol_map()

EFF = 0x800FBA90          # Effect[64], 0x4C each
OBJS = 0x800FD2F8         # Obj *[64]
SCROLL_X, SCROLL_Y, ROUND_OVER = 0x800FD2B8, 0x800FD2C8, 0x800FD2D8
PLAYERS = 0x800FD3F8
SEED = 0x800F9DE0
TIMER = 0x800FD424
OVL_BSS = (0x800FB050, 0x800FD550)

FAKE_OBJ = 0x80400000     # 64 fake Obj blocks (0xB0)
PLAYER_MEM = (0x80500000, 0x80540000)   # players are > 0x34298 bytes
ANIM = 0x80580000         # synthetic animation tables
SLOTS = 0x80590000        # synthetic sprite slots
TASKOBJ = 0x805A0000
TEST_REGION = (0x80400000, 0x80600000)

OBJ_FUNCS = ['Obj_SetData', 'Obj_SetPos', 'Obj_SetUpdate', 'Obj_Hide', 'Obj_Show',
             'Obj_SetColor', 'Obj_UseSetupCC0', 'Obj_UseSetupC80', 'Obj_Kill', 'Obj_NewDL']

ARITY = {'Obj_SetData': 2, 'Obj_SetPos': 4, 'Obj_SetUpdate': 3, 'Obj_SetColor': 5, 'Obj_NewDL': 4}
RETURNS = {'Effect_Spawn'}      # everything else is void

# (C name, original address)
FUNCS = [
    ('Game_MemClear', 0x800F1D40), ('Effect_ClearAll', 0x800F1D6C), ('Effect_Spawn', 0x800F1D90),
    ('Effect_UpdatePetal', 0x800F1ED4), ('Effect_SpawnPetal', 0x800F200C),
    ('Effect_UpdateSmoke', 0x800F210C), ('Effect_SpawnSmoke', 0x800F2208),
    ('Effect_UpdateFlash', 0x800F2268), ('Effect_SpawnFlash', 0x800F2384),
    ('Effect_UpdateMoveTo', 0x800F23F4), ('Effect_SpawnMoveTo', 0x800F2580),
    ('Effect_UpdateExplosion', 0x800F265C), ('Effect_SpawnExplosion', 0x800F2768),
    ('Missile_Update', 0x800F2798), ('Missile_Spawn', 0x800F2BB0),
    ('Bullet_UpdateA', 0x800F2C44), ('Bullet_SpawnA', 0x800F2F6C),
    ('Bullet_UpdateB', 0x800F2FD4), ('Bullet_SpawnB', 0x800F32FC),
    ('Bullet_UpdateFast', 0x800F3364), ('Bullet_SpawnFast', 0x800F368C),
    ('Boomerang_Update', 0x800F36F4), ('Boomerang_Spawn', 0x800F3B2C),
    ('Net_Update', 0x800F3C24), ('Net_Spawn', 0x800F4060),
    ('Task_BlinkBanner', 0x800F4100), ('Task_SlideBanner', 0x800F4210),
    ('Hud_UpdateTimer2', 0x800F43E0), ('Bonus_Task', 0x800F45DC),
    ('SpinBoss_Task', 0x800F00F4), ('Player_Task', 0x800F10F0), ('Crawler_Task', 0x800F4FC0), ('Item_Task', 0x800F5780), ('Clock_Task', 0x800F5D50),
]
TASK_TRIALS = {'SpinBoss_Task': 150, 'Player_Task': 150, 'Crawler_Task': 150, 'Bonus_Task': 100, 'Item_Task': 150, 'Clock_Task': 150}
ADDR = dict(FUNCS)
UPDATES = {a: n for n, a in FUNCS if 'Update' in n and n != 'Hud_UpdateTimer2'}


class Ctx:
    """Per-run state of the Python stubs (identical for both runs)."""
    def __init__(self, seed, yield_stop=None, wh=(640, 480), on_yield=None, decode_wh=(320, 240)):
        self.seed = seed
        self.rng = random.Random(seed ^ 0x5A5A)
        self.on_yield = on_yield
        self.decode_wh = decode_wh
        self.board = None
        self.task_done = None
        self.board_noise = 0
        self.cell_period = 37
        self.cell_free = 10
        self.log = []
        self.yields = 0
        self.yield_stop = yield_stop
        self.wh = wh
        self.voice = 0x100

    def h(self, *a):
        return hash((self.seed,) + a) & 0xFFFF


HASH = 0x805FFF00      # (hash, count) of the fast Obj_* stubs
STUB_CODE = 0x80610000 # their code


def asm_blob(src, base):
    """Assemble MIPS source (big-endian, linked at base) -> bytes."""
    import subprocess, tempfile
    from mipsbuild import BIN
    d = tempfile.mkdtemp()
    open(f'{d}/s.s', 'w').write('.set noreorder\n.set noat\n.text\n' + src)
    subprocess.run([BIN + 'as', '-EB', '-march=mips32', '-o', f'{d}/s.o', f'{d}/s.s'], check=True)
    subprocess.run([BIN + 'ld', '-EB', '-m', 'elf32btsmip', '-Ttext', hex(base), '-o', f'{d}/s.elf', f'{d}/s.o'],
                   check=True, capture_output=True)
    subprocess.run([BIN + 'objcopy', '-O', 'binary', '-j', '.text', f'{d}/s.elf', f'{d}/s.bin'], check=True)
    return open(f'{d}/s.bin', 'rb').read()


def install_fast_obj_stubs(emu):
    """Replace the Obj_* functions with tiny MIPS routines that fold their id and
    arguments into a hash at HASH (much faster than Python stubs; used by the
    long task tests). Same side effects as the Python stubs."""
    for k, name in enumerate(OBJ_FUNCS):
        n = ARITY.get(name, 1)
        regs = ['$a0', '$a1', '$a2', '$a3'][:min(n, 4)]
        src = (f'lui $t0, {HASH >> 16:#x}\n ori $t0, $t0, {HASH & 0xFFFF:#x}\n'
               'lw $t1, 0($t0)\n lw $t3, 4($t0)\n addiu $t3, $t3, 1\n sw $t3, 4($t0)\n'
               'lui $t5, 0x0100\n ori $t5, $t5, 0x0193\n'
               f'addiu $t2, $zero, {k + 1}\n xor $t1, $t1, $t2\n mul $t1, $t1, $t5\n')
        for r in regs:
            src += f'xor $t1, $t1, {r}\n mul $t1, $t1, $t5\n'
        if n > 4:
            src += 'lw $t4, 0x10($sp)\n xor $t1, $t1, $t4\n mul $t1, $t1, $t5\n'
        src += 'sw $t1, 0($t0)\n'
        if name == 'Obj_SetUpdate':
            src += 'sw $a1, 0x64($a0)\n sw $a2, 0x68($a0)\n'
        if name == 'Obj_NewDL':
            src += 'lui $v0, 0x805E\n jr $ra\n ori $v0, $v0, 0x2000\n'
        else:
            src += 'jr $ra\n addu $v0, $a0, $zero\n'
        a = SMAP[name]
        body = STUB_CODE + 0x100 * k        # (the originals are too short to hold it)
        emu.write(body, asm_blob(src, body))
        emu.write(a, asm_blob(f'j {body:#x}\n nop\n', a))


def install_stubs(emu, ctx_ref, fast=False):
    def stack_arg(e, i):
        return e.r32(e.mu.reg_read(UC_MIPS_REG_SP) + 0x10 + 4 * (i - 4))

    def mk_obj(name):
        def f(e):
            c = ctx_ref[0]
            args = [e.arg(0), e.arg(1), e.arg(2), e.arg(3)]
            if name == 'Obj_SetColor':
                args.append(stack_arg(e, 4))
            args = args[:ARITY.get(name, 1)]
            c.log.append((name, tuple(args)))
            if name == 'Obj_NewDL':
                return 0x805E2000
            if name == 'Obj_SetUpdate':
                e.w32(args[0] + 0x64, args[1])
                e.w32(args[0] + 0x68, args[2])
            return args[0]
        return f
    if fast:
        install_fast_obj_stubs(emu)
    else:
        for n in OBJ_FUNCS:
            emu.stub(SMAP[n], mk_obj(n))

    def snd(e):
        c = ctx_ref[0]
        c.voice += 1
        c.log.append(('snd', e.arg(0)))
        return c.voice
    emu.stub(0x8001BA90, snd)
    emu.stub(0x8001BB8C, lambda e: ctx_ref[0].log.append(('sndstop', e.arg(0) & 0xFFFF)))

    def yld(e):
        c = ctx_ref[0]
        c.yields += 1
        c.log.append(('yield', c.yields))
        if c.yield_stop is not None and c.yields == c.yield_stop:
            e.w32(ROUND_OVER, 1)
        if c.task_done and c.yields >= c.task_done[1]:
            e.w32(c.task_done[0], 1)
            c.task_done = None
        if c.on_yield:
            c.on_yield(e, c)
        if c.yields > 20000:
            raise RuntimeError('too many yields')
    emu.stub(0x8000AEAC, yld)
    emu.stub(0x800E29C4, lambda e: ctx_ref[0].wh[0])
    emu.stub(0x800E29D0, lambda e: ctx_ref[0].wh[1])

    def wall(e):
        c = ctx_ref[0]
        x, y = e.arg(0), e.arg(1)
        c.log.append(('wall', x, y))
        return 1 if c.h('w', x, y) % 6 == 0 else 0
    emu.stub(0x800E2AE8, wall)

    def cell(e):
        c = ctx_ref[0]
        x, y = e.arg(0), e.arg(1)
        c.log.append(('cell', x, y))
        if c.board is not None:
            x0, y0, x1, y1 = c.board
            xs = ((x + 0x80000000) & 0xFFFFFFFF) - 0x80000000
            ys = ((y + 0x80000000) & 0xFFFFFFFF) - 0x80000000
            if x0 <= xs <= x1 and y0 <= ys <= y1:
                return 3 if xs in (x0, x1) or ys in (y0, y1) else 1
            if c.board_noise and c.h('n', x, y) % 100 < c.board_noise:
                return 1
            return 0
        v = c.h('cell', x, y, c.yields // c.cell_period) % 16
        return 0 if v < c.cell_free else (1 if v >= 14 else 2)
    emu.stub(0x800E2E24, cell)

    def inner(e):
        c = ctx_ref[0]
        b, x, y = e.arg(0), e.arg(1), e.arg(2)
        c.log.append(('inner', b, x, y))
        if c.board is None:
            return 0
        x0, y0, x1, y1 = c.board
        xs = ((x + 0x80000000) & 0xFFFFFFFF) - 0x80000000
        ys = ((y + 0x80000000) & 0xFFFFFFFF) - 0x80000000
        return 1 if x0 < xs < x1 and y0 < ys < y1 else 0
    emu.stub(0x800E3C64, inner)

    def mk_hit(tag):
        def f(e):
            c = ctx_ref[0]
            a = (e.arg(0), e.arg(1), e.arg(2), e.arg(3), stack_arg(e, 4))
            c.log.append((tag,) + a)
            p = a[0]
            px, py = e.r32(p + 4), e.r32(p + 8)
            dx = ((a[1] - px + 0x80000000) & 0xFFFFFFFF) - 0x80000000
            dy = ((a[2] - py + 0x80000000) & 0xFFFFFFFF) - 0x80000000
            return 1 if abs(dx) < a[3] and abs(dy) < a[4] else 0
        return f
    emu.stub(0x800E3D28, mk_hit('hit'))

    def mk_log(tag, nargs, ret=None, nlog=None):
        def f(e):
            c = ctx_ref[0]
            a = [e.arg(i) if i < 4 else stack_arg(e, i) for i in range(nargs)]
            c.log.append((tag,) + tuple(a[:nlog]))
            return ret(e, c, a) if ret else None
        return f

    def jpeg(e, c, a):
        e.w32(a[4], c.decode_wh[0])
        e.w32(a[5], c.decode_wh[1])
    def pv_new(e, c, a):
        for k, v in enumerate((0x805E0000, 0x805E0100, 0x805E0200)):
            e.w32(0x805E1000 + 4 * k, v)
        return 0x805E1000
    emu.stub(SMAP['Jpeg64_Decode'], mk_log('jpeg', 6, ret=jpeg, nlog=4))
    emu.stub(SMAP['Image_CopyRect'], mk_log('copyrect', 12))
    emu.stub(SMAP['Image_Mosaic'], mk_log('mosaic', 9))
    emu.stub(SMAP['PhotoView_New'], mk_log('pvnew', 6, ret=pv_new))
    emu.stub(SMAP['PhotoView_SetScroll'], mk_log('pvscroll', 3))
    emu.stub(SMAP['PhotoView_Delete'], mk_log('pvdel', 1))
    emu.stub(SMAP['Scene_Build'], mk_log('scenebuild', 2))
    emu.stub(SMAP['Scene_Destroy'], mk_log('scenedestroy', 2))
    emu.stub(0x800E2910, mk_log('boardinit', 6))
    emu.stub(0x8001BBC0, mk_log('music', 1))

    def task_new(e):
        c = ctx_ref[0]
        a = [e.arg(0), e.arg(1), e.arg(2), e.arg(3)]
        c.log.append(('tasknew',) + tuple(a))
        c.task_done = (stack_arg(e, 4), c.yields + c.rng.randrange(0, 200))
        return 0x805E3000
    emu.stub(0x8000A76C, task_new)

    def player_update(e):
        c = ctx_ref[0]
        inp, p = e.arg(0), e.arg(1)
        c.log.append(('pupdate', inp, p))
        r = c.rng.random()
        if r < 0.3:
            k = c.rng.randrange(4)
            dx, dy = ((1, 0), (-1, 0), (0, 1), (0, -1))[k]
            e.w32(p + 4, e.r32(p + 4) + dx)
            e.w32(p + 8, e.r32(p + 8) + dy)
        elif r < 0.32:
            e.w32(p + 0x1C, c.rng.choice([0, 1]))
    emu.stub(0x800E8F74, player_update)
    emu.stub(0x800E3E4C, mk_hit('touch'))


def f2u(v):
    return struct.unpack('>I', struct.pack('>f', v))[0]


def make_anims(emu, rng, n_anims=8, n_frames=6):
    """Synthetic animation table: entry 0 of each animation has time >= 0,
    the last entry is a command (-1/-2/-3)."""
    tbl = ANIM
    frames = ANIM + 0x2000
    for a in range(2048):      # long table: the boomerang's spin shares the index field
        emu.w32(tbl + 4 * a, frames + (a % n_anims) * n_frames * 8)
    for a in range(n_anims):
        fl = frames + a * n_frames * 8
        for k in range(n_frames):
            slot = SLOTS + (a * n_frames + k) * 4
            emu.w32(slot, 0x81000000 + rng.getrandbits(16))
            if k == 0:
                t = rng.choice([0, 0, 1, 2])
            elif k == n_frames - 1:
                t = rng.choice([-1, -2, -3])
            else:
                t = rng.choice([0, 1, 2, 3, -1, -2, -3]) if rng.random() < 0.3 else rng.choice([0, 1, 2])
            emu.w32(fl + 8 * k, slot)
            emu.w32(fl + 8 * k + 4, t & 0xFFFFFFFF)
    return tbl


def setup_world(emu, rng):
    # overlay BSS: random garbage, then the structures we know
    emu.write(OVL_BSS[0], bytes(rng.getrandbits(8) for _ in range(256)) * ((OVL_BSS[1] - OVL_BSS[0]) // 256))
    emu.write(TEST_REGION[0], bytes(0x100000))
    for i in range(64):
        obj = FAKE_OBJ + i * 0xB0
        emu.w32(OBJS + 4 * i, obj)
        for k in range(0, 0xB0, 4):
            emu.w32(obj + k, rng.getrandbits(32))
        e = EFF + i * 0x4C
        emu.w32(e, 1 if rng.random() < 0.5 else 0)
        emu.w32(obj + 0x68, e)
    for n, p in enumerate(PLAYER_MEM):
        emu.write(p, bytes(0x40))
        emu.w32(PLAYERS + 4 * n, p)
        emu.w32(p + 4, rng.randrange(0, 640))
        emu.w32(p + 8, rng.randrange(0, 480))
        emu.w32(p + 0x14, 1 if rng.random() < 0.3 else 0)
        emu.w32(p + 0x34298, 0)
    emu.w32(SCROLL_X, rng.randrange(-50, 400) & 0xFFFFFFFF)
    emu.w32(SCROLL_Y, rng.randrange(-50, 300) & 0xFFFFFFFF)
    emu.w32(ROUND_OVER, 1 if rng.random() < 0.1 else 0)
    emu.w32(SEED, rng.getrandbits(32))
    emu.w32(TIMER, rng.choice([0, 1, 59, 60, 1799, 1800, 1801, 5999, 6000, rng.randrange(0, 20000)]))
    for a in (0x800FCF58, 0x800FCF80):
        for k in range(10):
            emu.w32(a + 4 * k, 0x81100000 + a % 0x1000 + k)


def random_effect(emu, rng, i, anims):
    """Fill slot i with a plausible random state."""
    e = EFF + i * 0x4C
    p0 = emu.r32(PLAYER_MEM[0] + 4), emu.r32(PLAYER_MEM[0] + 8)
    near = rng.random() < 0.4
    def coord(lim, c):
        if near:
            return float(c + rng.uniform(-12, 12))
        return rng.choice([rng.uniform(-10, lim + 10), float(rng.randrange(0, lim)), 0.0, lim - 0.5,
                           lim - 4.0, lim - 8.0, 4.0, 8.0])
    x, y = coord(640, p0[0]), coord(480, p0[1])
    vals = {
        0x00: 1, 0x04: 0x805B0000 + 4 * rng.randrange(16),
        0x08: f2u(x), 0x0C: f2u(y),
        0x10: f2u(rng.choice([x + rng.uniform(-9, 9), rng.uniform(0, 640)])),
        0x14: f2u(rng.choice([y + rng.uniform(-9, 9), rng.uniform(0, 480)])),
        0x18: f2u(rng.choice([x + rng.uniform(-1.5, 1.5), rng.uniform(0, 640)])),
        0x1C: f2u(rng.choice([y + rng.uniform(-1.5, 1.5), rng.uniform(0, 480)])),
        0x20: f2u(rng.choice([rng.uniform(0.05, 2.0), 0.1, 0.2, 0.11, 0.22, 1.0])),
        0x24: rng.choice([rng.randrange(0, 0x400), 0, 0x100, 0x200, 0x3FF]),
        0x28: anims,
        0x2C: SLOTS + 4 * rng.randrange(48),
        0x30: rng.randrange(0, 5),
        0x34: rng.choice([0, 0, 1, 2]),
        0x38: rng.randrange(0, 2),
        0x3C: rng.choice([0, 0, 1, 2, 3, 4, rng.randrange(0, 240)]),
        0x40: rng.choice([rng.randrange(0, 4), rng.randrange(0, 2000)]),
        0x44: rng.choice([0, 1, 2, 3, rng.randrange(0, 300)]),
    }
    for off, v in vals.items():
        emu.w32(e + off, v)
    emu.w16(e + 0x48, rng.choice([0, 0, 1]))
    emu.w16(e + 0x4A, rng.choice([0, 0, 1]))
    return e


def snapshot(emu):
    return [(a, emu.read(a, b - a)) for a, b in
            ((0x800E0000, OVL_BSS[1]), (0x8003E130, 0x800C5400), TEST_REGION,
             (GAME_HEAP, GAME_HEAP + GAME_HEAP_SIZE))]


def restore(emu, snap):
    for a, d in snap:
        emu.write(a, d)


def compare_state(emu):
    return snapshot(emu)


class Runner:
    def __init__(self, fast=False):
        self.emu = make_env(overlay=2)
        blob, self.syms = build_clean(['src/game/ovl2_d.c'], name='ovl2_d')
        self.emu.write(CODE_BASE, blob)
        self.ctx = [None]
        install_stubs(self.emu, self.ctx, fast)

    def run(self, fn_orig, fn_c, setup, seed, driver=None):
        """setup(emu, rng) -> (args, ctx kwargs). driver(emu, call, which) runs
        a sequence instead of one call."""
        emu = self.emu
        rng = random.Random(seed)
        args, kw = setup(emu, rng)
        snap = snapshot(emu)
        results = []
        for which, addr in (('orig', fn_orig), ('c', fn_c)):
            restore(emu, snap)
            self.ctx[0] = Ctx(seed, **kw)
            try:
                if driver:
                    v0 = driver(emu, which, args)
                else:
                    v0, _ = emu.call(addr, a=args)
            except Exception as ex:
                return f'{which}: {ex!r}'
            results.append((v0, self.ctx[0].log, compare_state(emu)))
        (v0a, la, sa), (v0b, lb, sb) = results
        if os.environ.get('LOGSTAT'):
            import collections
            print('   ', dict(collections.Counter(x[0] for x in la)), 'objcalls', emu.r32(HASH + 4))
        if not driver and self.cur in RETURNS and v0a != v0b:
            return f'v0 {v0a:#x} != {v0b:#x}'
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

    def c_addr(self, name):
        return self.syms[name]


def main():
    R0 = Runner()
    RF = None
    want = sys.argv[1:]
    fails = 0

    def setup_update(emu, rng):
        setup_world(emu, rng)
        anims = make_anims(emu, rng) if rng.random() < 0.85 else 0x800F90E0
        i = rng.randrange(64)
        random_effect(emu, rng, i, anims)
        if anims == 0x800F90E0:
            emu.w32(EFF + i * 0x4C + 0x30, 0)
            emu.w32(EFF + i * 0x4C + 0x3C, rng.choice([0, 1]))
            emu.w32(EFF + i * 0x4C + 0x40, rng.choice([0, 1]))
        return [FAKE_OBJ + i * 0xB0], {'wh': rng.choice([(640, 480), (640, 480), (320, 240), (1000, 1000)])}

    def setup_spawn(emu, rng):
        setup_world(emu, rng)
        if rng.random() < 0.1:
            for i in range(64):
                emu.w32(EFF + i * 0x4C, 1)
        args = [rng.choice([rng.randrange(-100, 700), 0, 319]) & 0xFFFFFFFF for _ in range(4)]
        args.append(0x805B0000 + 4 * rng.randrange(16))
        return args, {}

    def setup_memclear(emu, rng):
        setup_world(emu, rng)
        return [0x805C0000 + rng.randrange(64), rng.choice([0, 1, 2, rng.randrange(0, 3000)])], {}

    def setup_spawn_generic(emu, rng):
        a, kw = setup_spawn(emu, rng)
        a[0] = rng.choice([0x800F1ED4, 0x800F210C, 0x12345678])
        a[1] = rng.choice([0x800F90E0, ANIM])
        return a, kw

    def setup_task(emu, rng):
        setup_world(emu, rng)
        for k in range(2):
            emu.w32(TASKOBJ + 4 * k, FAKE_OBJ + rng.randrange(64) * 0xB0)
        stop = rng.choice([None, None, 1, 2, rng.randrange(1, 100)])
        return [0x805D0000, TASKOBJ], {'yield_stop': stop}

    def setup_bonus(emu, rng):
        setup_world(emu, rng)
        reset_heap(emu)
        for n in range(2):
            p, _ = emu.call(SMAP['Heap_Alloc'], a=[GAME_HEAP, 0x3429C])
            emu.w32(PLAYERS + 4 * n, p)
        emu.w32(SMAP.get('D_800B0608', 0x800B0608), rng.randrange(8))
        emu.w32(0x800B04D4, rng.choice([0, 1, 3]))
        emu.w32(0x800B04D8, rng.choice([0, 1, 3]))
        emu.w16(0x800C30C4, 0)
        emu.w32(0x800AF260, 0)
        emu.w32(0x800C1674, 0)
        emu.w16(0x800C0D48, 0)
        emu.w16(0x800C0D4A, 0)
        press = rng.choice([0.0, 0.05, 0.12, 0.3, 0.6, 1.0])
        abort = rng.choice([None] * 6 + [(rng.randrange(1, 900), rng.randrange(2))])
        def on_yield(e, c):
            e.w16(0x800C0D48, (0x10 if c.rng.random() < press else 0) | c.rng.choice([0, 0, 0x100, 0x20]))
            e.w16(0x800C0D4A, (0x10 if c.rng.random() < press else 0) | c.rng.choice([0, 0, 0x100]))
            if abort and c.yields == abort[0]:
                if abort[1]:
                    e.w16(0x800C30C4, 1)
                else:
                    e.w32(0x800AF260, 1)
        dwh = rng.choice([(320, 240), (640, 240), (320, 480)])
        wh = dwh if rng.random() < 0.8 else rng.choice([(320, 240), (640, 240), (320, 480), (640, 480)])
        # the original ORs into a never-written stack slot (sp+0x5C of its 0xA0 frame)
        emu.w32(STACK_TOP - 0x100 - 0xA0 + 0x5C, 0)
        emu.w32(0x805D0000, 0xDEADBEEF)
        return [0x805D0000, 0], {'on_yield': on_yield, 'decode_wh': dwh, 'wh': wh}

    def setup_item(emu, rng):
        setup_world(emu, rng)
        emu.w32(ROUND_OVER, 0)
        emu.w32(0x800B04D4, rng.choice([0, 1, 2, 3, 4]))
        emu.w32(0x800B04D8, rng.choice([0, 1, 2, 3, 4]))
        emu.w32(TIMER, rng.choice([0, 100, 600, 601, 3000]))
        stop = rng.randrange(1, 9000)
        period = rng.choice([5, 37, 200, 1000])
        free = rng.choice([4, 10, 13])
        def on_yield(e, c):
            c.cell_period = period
            c.cell_free = free
        return [FAKE_OBJ + rng.randrange(64) * 0xB0], {'yield_stop': stop, 'on_yield': on_yield,
                                                        'wh': rng.choice([(320, 240), (640, 240), (320, 480)])}

    def setup_crawler(emu, rng):
        setup_world(emu, rng)
        emu.w32(ROUND_OVER, 0)
        objs = [FAKE_OBJ + rng.randrange(64) * 0xB0 for _ in range(5)]
        for k, o in enumerate(objs):
            emu.w32(TASKOBJ + 4 * k, o)
        wh = rng.choice([(320, 240), (640, 240), (320, 480), (64, 48)])
        x0, y0 = rng.randrange(0, wh[0] // 2), rng.randrange(0, wh[1] // 2)
        x1, y1 = rng.randrange(x0 + 2, wh[0]), rng.randrange(y0 + 2, wh[1])
        board = rng.choice([None, (x0, y0, x1, y1), (x0, y0, x1, y1), (x0, y0, x1, y1)])
        if board:
            for n, pm in enumerate(PLAYER_MEM):     # players on the edge
                emu.w32(pm + 4, rng.choice([x0, x1, rng.randrange(x0, x1 + 1)]))
                emu.w32(pm + 8, rng.choice([y0, y1]))
        emu.w32(0x800FD2C0, rng.choice([0, 0, 0, 1]))
        stop = rng.randrange(3700, 12000)
        shrink = rng.choice([None, rng.randrange(3700, 9000)])
        def on_yield(e, c):
            if c.yields == 1:
                c.board = board
            if shrink and c.yields == shrink and c.board:
                bx0, by0, bx1, by1 = c.board
                c.board = (bx0 + 1, by0 + 1, max(bx0 + 2, bx1 - 1), max(by0 + 2, by1 - 1))
            if c.rng.random() < 0.02:
                for pm in PLAYER_MEM:
                    e.w32(pm + 0x14, 0)
            if c.rng.random() < 0.01:
                e.w32(0x800FD2C0, c.rng.choice([0, 0, 5]))
        c0 = {'yield_stop': stop, 'on_yield': on_yield, 'wh': wh}
        return [TASKOBJ, 0x800F93B4, rng.choice([0, 1]), 0], c0

    def setup_player(emu, rng):
        setup_world(emu, rng)
        emu.w32(ROUND_OVER, 0)
        n = rng.randrange(2)
        p = PLAYER_MEM[n]
        emu.w32(p, n)
        for off in (0x18, 0x1C, 0x34284, 0x34288, 0x3428C, 0x34290, 0x34294):
            emu.w32(p + off, rng.choice([0, 0, 1, 50]) if off != 0x34294 else SLOTS)
        emu.w32(p + 0x3428C, 0)
        emu.w32(p + 0x34290, 0)
        objs = [FAKE_OBJ + rng.randrange(64) * 0xB0 for _ in range(5)]
        for k, o in enumerate(objs):
            emu.w32(TASKOBJ + 4 * k, o)
        offscreen = rng.random() < 0.1
        sx, sy = (0, 0) if not offscreen else (rng.randrange(0, 50), rng.randrange(0, 50))
        emu.w32(SCROLL_X, sx)
        emu.w32(SCROLL_Y, sy)
        x0, y0 = rng.randrange(8, 150), rng.randrange(8, 110)
        x1, y1 = rng.randrange(x0 + 4, 312), rng.randrange(y0 + 4, 232)
        if offscreen:
            x0, y0, x1, y1 = 400, 300, 500, 400
        emu.w32(p + 4, rng.choice([x0, x1, rng.randrange(0, 320)]))
        emu.w32(p + 8, rng.choice([y0, y1, rng.randrange(0, 240)]))
        lives = rng.choice([0, 1, 2, 3])
        emu.w32(0x800B04D4 + 4 * n, lives)
        emu.w32(0x800C51D8, rng.choice([0, 0, 1]))
        emu.w32(0x800FD2CC, 0)
        stop = rng.randrange(50, 4000) if not offscreen else rng.randrange(5, 60)
        noise = rng.choice([0, 30, 60])
        def on_yield(e, c):
            if c.yields == 1:
                c.board = (x0, y0, x1, y1)
                c.board_noise = noise
            r = c.rng.random()
            if r < 0.01:
                e.w32(p + 0x14, 1)
            elif r < 0.015:
                e.w32(0x800B04D4 + 4 * n, c.rng.choice([0, 1, 3]))
            elif r < 0.02:
                e.w32(0x800FD2CC, c.rng.choice([0, 0, 1]))
            elif r < 0.03:
                e.w32(p + 0x34288, c.rng.choice([0, 3]))
            e.w16(0x800B060C + 2 * n, c.rng.getrandbits(16))
        return ([0x805D0000, TASKOBJ, 16, 16, 0x800F922C if n == 0 else 0x800F9318, p],
                {'yield_stop': stop, 'on_yield': on_yield})

    def setup_spinboss(emu, rng):
        setup_world(emu, rng)
        emu.w32(ROUND_OVER, 0)
        for k in range(24):
            emu.w32(TASKOBJ + 4 * k, FAKE_OBJ + rng.randrange(64) * 0xB0)
        emu.w32(0x800B4AA0, rng.randrange(8))
        emu.w32(0x800B04D4, rng.choice([0, 1, 3]))
        emu.w32(0x800B04D8, rng.choice([0, 1, 3]))
        emu.w32(0x800FD2C0, rng.choice([0, 0, 0, 5]))
        emu.w32(0x800FD42C, 0x80410000)
        x0, y0 = rng.randrange(0, 300), rng.randrange(0, 200)
        board = (x0, y0, x0 + rng.randrange(10, 300), y0 + rng.randrange(10, 200))
        stop = rng.randrange(10, 8000) if rng.random() < 0.75 else rng.randrange(9000, 15000)
        def on_yield(e, c):
            if c.yields == 1:
                c.board = board if c.rng.random() < 0.7 else None
            r = c.rng.random()
            if r < 0.01:
                e.w32(0x800FD2C0, c.rng.choice([0, 0, 3]))
            elif r < 0.03:
                pm = PLAYER_MEM[c.rng.randrange(2)]
                e.w32(pm + 0x14, 0)
            elif r < 0.035:
                e.w32(0x800B04D4 + 4 * c.rng.randrange(2), c.rng.choice([0, 2]))
        return ([0, TASKOBJ, rng.choice([160, rng.randrange(0, 640)]), rng.choice([200, rng.randrange(0, 480)]),
                 0x800F9488], {'yield_stop': stop, 'on_yield': on_yield,
                               'wh': rng.choice([(320, 240), (640, 240), (320, 480)])})

    # ---- lifecycle: spawn effects, then run all update callbacks for N frames ----
    SPAWNS = ['Effect_SpawnPetal', 'Effect_SpawnSmoke', 'Effect_SpawnFlash', 'Effect_SpawnMoveTo',
              'Effect_SpawnExplosion', 'Missile_Spawn', 'Bullet_SpawnA', 'Bullet_SpawnB',
              'Bullet_SpawnFast', 'Boomerang_Spawn', 'Net_Spawn']

    def setup_life(emu, rng):
        setup_world(emu, rng)
        emu.w32(ROUND_OVER, 0)
        for i in range(64):
            emu.w32(EFF + i * 0x4C, 0)
            emu.w32(FAKE_OBJ + i * 0xB0 + 0x64, 0)
        # real sprite slots in BSS are random already; players somewhere in the field
        plan = []
        for _ in range(rng.randrange(1, 8)):
            name = rng.choice(SPAWNS)
            args = [rng.randrange(0, 640), rng.randrange(0, 480), rng.randrange(0, 2) if 'Missile' in name or 'Boom' in name else rng.randrange(0, 0x400),
                    rng.randrange(0, 0x400), 0x805B0000 + 4 * rng.randrange(16)]
            if name == 'Net_Spawn' or name.startswith('Bullet'):
                args[2] = rng.randrange(0, 0x400)
            plan.append((name, args))
        frames = rng.randrange(5, 300)
        stop = rng.choice([None, None, rng.randrange(1, frames + 1)])
        return (plan, frames, stop, rng.randrange(1 << 30)), {'wh': (640, 480)}

    def drive_life(emu, which, args):
        plan, frames, stop, s = args
        rng = random.Random(s)
        for name, a in plan:
            emu.call(ADDR[name] if which == 'orig' else R.c_addr(name), a=a)
        for f in range(frames):
            if stop is not None and f == stop:
                emu.w32(ROUND_OVER, 1)
            # players wander
            for p in PLAYER_MEM:
                emu.w32(p + 4, (emu.r32(p + 4) + rng.randrange(-3, 4)) & 0xFFFFFFFF)
                emu.w32(p + 8, (emu.r32(p + 8) + rng.randrange(-3, 4)) & 0xFFFFFFFF)
                if rng.random() < 0.02:
                    emu.w32(p + 0x14, 0)
            for i in range(64):
                obj = FAKE_OBJ + i * 0xB0
                fn = emu.r32(obj + 0x64)
                if fn in UPDATES:
                    emu.call(fn if which == 'orig' else R.c_addr(UPDATES[fn]), a=[obj])
            if rng.random() < 0.05:
                n, a = rng.choice(plan)
                emu.call(ADDR[n] if which == 'orig' else R.c_addr(n), a=a)
        return 0

    tests = []
    for name, addr in FUNCS:
        if name in UPDATES.values():
            tests.append((name, addr, setup_update, None))
        elif name == 'Game_MemClear':
            tests.append((name, addr, setup_memclear, None))
        elif name == 'Effect_Spawn':
            tests.append((name, addr, setup_spawn_generic, None))
        elif name.startswith('Task_'):
            tests.append((name, addr, setup_task, None))
        elif name == 'Bonus_Task':
            tests.append((name, addr, setup_bonus, None))
        elif name == 'SpinBoss_Task':
            tests.append((name, addr, setup_spinboss, None))
        elif name == 'Player_Task':
            tests.append((name, addr, setup_player, None))
        elif name == 'Crawler_Task':
            tests.append((name, addr, setup_crawler, None))
        elif name in ('Item_Task', 'Clock_Task'):
            tbl = 0x800F8E1C if name == 'Item_Task' else 0x800F8E4C
            tests.append((name, addr, (lambda tbl: lambda emu, rng: (lambda r: ([r[0][0], tbl], r[1]))(setup_item(emu, rng)))(tbl), None))
        else:
            tests.append((name, addr, setup_spawn, None))
    tests.append(('Lifecycle', 0, setup_life, drive_life))

    for name, addr, setup, driver in tests:
        if want and name not in want:
            continue
        n = TRIALS if name != 'Lifecycle' else max(20, TRIALS // 5)
        if name in TASK_TRIALS:
            n = min(n, int(os.environ.get('TASK_TRIALS', TASK_TRIALS[name])))
        ok = 0
        first = None
        if name in TASK_TRIALS and not os.environ.get('SLOW'):
            RF = RF or Runner(fast=True)
            R = RF
        else:
            R = R0
        for t in range(n):
            R.cur = name
            err = R.run(addr, R.c_addr(name) if driver is None else 0, setup, hash((name, t)) & 0xFFFFFFF, driver)
            if err is None:
                ok += 1
            elif first is None:
                first = f'trial {t}: {err}'
        print(f'{name:24s} {"PASS" if ok == n else "FAIL"} {ok}/{n}' + (f'   {first}' if first else ''))
        fails += ok != n
    sys.exit(1 if fails else 0)


if __name__ == '__main__':
    main()
