#!/usr/bin/env python3
"""Differential test of src/game/ovl2_c.c (overlay 2 enemies: minion, boss with
four arms, snake, spinner) against the original MIPS code of overlay 2.

Every trial builds the same random play field (claimed-area bitmap, field pixel
map, two players with line bitmaps), fake objects, animation tables and enemy
state, then runs the original and the C function from identical memory. The
whole overlay BSS, main BSS and test memory are compared, plus a log of every
call to the stubbed sound / task / projectile functions.

Task functions (Minion_Task, Boss_Task, Snake_Task) are cooperative tasks that
loop forever around Task_Yield (0x8000AEAC). Task_Yield is stubbed: each call
records a snapshot of the game state, may poke the game globals (pause, player
hit flags, stage-over flag, flags written by spawned beams) from a seeded
script, and after N frames stops the emulator."""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env
from unicorn import UC_HOOK_CODE
from unicorn.mips_const import UC_MIPS_REG_SP, UC_MIPS_REG_F12, UC_MIPS_REG_F14
from mipsbuild import CODE_BASE
from draft_mips import build_clean

# ---- addresses ----
OVL_BSS, OVL_BSS_END = 0x800FB050, 0x800FD550
MAIN_BSS, MAIN_BSS_END = 0x8003E130, 0x800C5400
FIELD_W, FIELD_H, FIELD_MAP = 0x800FBA70, 0x800FBA74, 0x800FBA7C
STAGE_OVER, PAUSED, SCROLL_X, SCROLL_Y = 0x800FD2D8, 0x800FD2C0, 0x800FD2B8, 0x800FD2C8
AREA_PTR, PLAYERS = 0x800FD42C, 0x800FD3F8
LEVEL, PLAYING = 0x800B4AA0, 0x800B04D4

P1, P2 = 0x80400000, 0x80480000        # players (line bitmap at +0x68048)
LINE_OFS = 0x68048
AREA = 0x80500000                       # claimed-area bitmap
MAP = 0x80510000                        # field pixel map, 2 bits per pixel
OBJS = 0x80520000                       # 24 fake objects
OBJ_PTRS = 0x80522000                   # Obj *[24]
STATE = 0x80530000                      # enemy state / arguments
ANIMS = 0x80540000                      # AnimFrame *[16]
ANIM_DATA = 0x80540100
TEST_END = 0x80560000

# stubbed callees: name, number of arguments logged
STUBS = {0x8001BA90: ('Sound_Play', 1), 0x8001BB8C: ('Sound_Stop', 1), 0x8000A76C: ('Task_Spawn', 5),
         0x800F2BB0: ('shot', 4), 0x800F2F6C: ('armshot', 3), 0x800F32FC: ('fire', 3),
         0x800F3B2C: ('beam', 5), 0x800F4060: ('spawn', 3)}
YIELD = 0x8000AEAC

ORIG = {'Boss_UpdateArm': 0x800EB4F0, 'Boss_UpdateArmAim': 0x800EB998,
        'Boss_MoveBounce': 0x800EBE9C, 'Snake_MoveHead': 0x800EE230,
        'Snake_FollowBody': 0x800EE510, 'Snake_CurlFire': 0x800EE7FC,
        'Snake_CurlBeam': 0x800EEB78, 'Spinner_Move': 0x800EFCF0,
        'Minion_Task': 0x800EAD80, 'Boss_Task': 0x800EC0CC, 'Snake_Task': 0x800EEEEC}

fb = lambda v: struct.unpack('>I', struct.pack('>f', v))[0]


class Stop(Exception):
    pass


class Env:
    def __init__(self):
        blob, self.syms = build_clean(['src/game/ovl2_c.c'], name='ovl2_c')
        emu = self.emu = make_env(overlay=2)
        emu.write(CODE_BASE, blob)
        self.log = []
        for addr, (name, nargs) in STUBS.items():
            emu.stub(addr, self._mk_stub(name, nargs))
        emu.stub(YIELD, self._yield)
        # watch points (not stubs): log the inputs of shared callees so internal
        # state (float positions, speeds, angles, seeds) is compared too
        f12 = lambda e: e.mu.reg_read(UC_MIPS_REG_F12) & 0xFFFFFFFF
        f14 = lambda e: e.mu.reg_read(UC_MIPS_REG_F14) & 0xFFFFFFFF
        watch = {0x8001C5A0: lambda e: ('sinf', f12(e)), 0x8001C450: lambda e: ('cosf', f12(e)),
                 0x80000634: lambda e: ('VecToAngle', f12(e), f14(e)),
                 0x80009CE8: lambda e: ('Rand', e.arg(1), e.r32(e.arg(0))),
                 0x800E3C64: lambda e: ('AreaHit', e.arg(1), e.arg(2)),
                 0x800E3D28: lambda e: ('PlayerHit', e.arg(0), e.arg(1), e.arg(2), e.arg(3))}
        # struct-state dumps at the entry of functions called by the tasks (both versions)
        dump = {'Boss_UpdateArm': lambda e: ('arm', e.read(e.arg(0), 0x2C), e.read(e.arg(3), 0x1C), e.arg(1)),
                'Boss_UpdateArmAim': lambda e: ('aim', e.read(e.arg(0), 0x2C), e.read(e.arg(3), 0x1C)),
                'Boss_MoveBounce': lambda e: ('bounce', e.read(e.arg(0), 0x2C)),
                'Snake_MoveHead': lambda e: ('head', e.read(e.arg(0), 0x34 * 8))}
        # (only functions the C task code does not get inlined into its callers)
        for name, fn in dump.items():
            watch[ORIG[name]] = fn
            watch[self.syms[name]] = fn
        self.watch = watch
        for a in watch:
            emu.mu.hook_add(UC_HOOK_CODE, self._watch, begin=a, end=a)
        self.yield_fn = None
        self.beam_done = None
        self.base = {a: emu.read(a, n) for a, n in self.regions()}

    def regions(self):
        return [(OVL_BSS, OVL_BSS_END - OVL_BSS), (MAIN_BSS, MAIN_BSS_END - MAIN_BSS),
                (0x80400000, TEST_END - 0x80400000)]

    def _mk_stub(self, name, nargs):
        def fn(e):
            sp = e.mu.reg_read(UC_MIPS_REG_SP)
            args = ([e.arg(i) for i in range(4)] + [e.r32(sp + 0x10)])[:nargs]
            if name == 'beam':
                # 5th argument points at a flag in the caller's stack frame (differs
                # between the two versions): remember it, log only that it is there
                self.beam_done = args[4]
                args[4] = 'stack' if 0x807F0000 <= args[4] < 0x80800000 else args[4]
            self.log.append((name, tuple(args)))
            if name == 'Sound_Play':     # handle as an s16 with junk in the upper half
                return 0x0001FF11 if args[0] == 0xE else 0x40 + args[0]
            if name == 'Task_Spawn':
                return 0x80470000
            return 0
        return fn

    def _watch(self, mu, addr, size, user):
        self.log.append(self.watch[addr](self.emu))

    def _yield(self, e):
        self.log.append(('yield',))
        if self.yield_fn is not None:
            self.yield_fn(e)

    def state(self):
        return [self.emu.read(a, n) for a, n in self.regions()]

    def restore(self):
        for a, n in self.regions():
            self.emu.write(a, self.base[a])
        self.log = []
        self.beam_done = None


# ---------------------------------------------------------------- random game state

def rand_field(env, rng, density=None):
    """Field size, claimed-area bitmap, pixel map, players, scroll, level."""
    e = env.emu
    w = rng.choice([320, 384, 256])
    h = rng.choice([240, 256, 224])
    e.w32(FIELD_W, w)
    e.w32(FIELD_H, h)
    d = density if density is not None else rng.choice([0.0, 0.05, 0.2, 0.5, 0.9])
    nbytes = (w // 8) * (h // 8) // 8 + 8
    area = bytes(sum((rng.random() < d) << b for b in range(8)) for _ in range(nbytes))
    e.write(AREA, area)
    e.w32(AREA_PTR, AREA)
    md = rng.choice([0.0, 0.3, 0.95, 1.0, 'busy', 'full'])
    n = w * h // 4 + 8
    if md == 'full':            # no free pixel at all: the minion searches forever
        mp = b'\xff' * n
    elif md == 'busy':          # hardly any free pixel
        mp = bytes(0 if rng.random() < 0.003 else 0xFF for _ in range(n))
    else:
        mp = bytes(rng.getrandbits(8) if rng.random() < md else 0 for _ in range(n))
    e.write(MAP, mp)
    e.w32(FIELD_MAP, MAP)
    for i, p in enumerate((P1, P2)):
        e.w32(PLAYERS + 4 * i, p)
        e.w32(p + 0x04, rng.randrange(-20, w + 20))
        e.w32(p + 0x08, rng.randrange(-20, h + 20))
        e.w32(p + 0x14, rng.random() < 0.3)
        e.w32(p + 0x1C, rng.random() < 0.6)
        ld = rng.choice([0.0, 0.02, 0.2])
        e.write(p + LINE_OFS, bytes(sum((rng.random() < ld) << b for b in range(8)) for _ in range(nbytes)))
    e.w32(SCROLL_X, rng.choice([0, rng.randrange(-200, 200)]))
    e.w32(SCROLL_Y, rng.choice([0, rng.randrange(-200, 200)]))
    e.w32(PAUSED, rng.random() < 0.15)
    e.w32(STAGE_OVER, 0)
    e.w32(LEVEL, rng.randrange(8))
    e.w32(PLAYING, rng.random() < 0.7)
    e.w32(PLAYING + 4, rng.random() < 0.7)
    return w, h


def rand_objs(env, rng):
    e = env.emu
    for i in range(24):
        o = OBJS + 0xB0 * i
        e.write(o, bytes(rng.getrandbits(8) for _ in range(0xB0)))
        e.w16(o + 0x58, rng.choice([0, 1, 1, 2, 3, 4, 5]))
        e.w16(o + 0x5A, rng.getrandbits(16) & ~(0x20 if rng.random() < 0.8 else 0))
        e.w32(OBJ_PTRS + 4 * i, o)


def rand_anims(env, rng, nseq=16):
    """AnimFrame *anims[nseq]; each sequence has 1..5 frames then an end marker."""
    e = env.emu
    p = ANIM_DATA
    for s in range(nseq):
        n = rng.randrange(1, 6)
        e.w32(ANIMS + 4 * s, p)
        for k in range(n):
            e.w32(p, p + 0x800)                           # frame -> data word
            e.w32(p + 0x800, 0x81000000 + s * 0x100 + k)
            e.w32(p + 4, rng.choice([0, 0, 1, 2, 3, 5]))
            p += 8
        e.w32(p, p + 0x800)
        e.w32(p + 0x800, 0xDEAD0000 + s)
        e.w32(p + 4, rng.choice([-1, -1, -5]))
        p += 8


def rf(rng, lo, hi):
    r = rng.random()
    if r < 0.05:
        return 0.0
    if r < 0.08:
        return -0.0
    return rng.uniform(lo, hi)


def pos(rng, w, h):
    return rf(rng, -30, w + 30), rf(rng, -30, h + 30)


def wfloats(e, a, vals):
    for i, v in enumerate(vals):
        e.wf(a + 4 * i, v)


def rand_boss(env, rng, w, h):
    e = env.emu
    x, y = pos(rng, w, h)
    wfloats(e, STATE, [x, y, rf(rng, -3, 3), rf(rng, -3, 3), rf(rng, 0.1, 1.5), rf(rng, 0.1, 2)])
    e.w32(STATE + 0x18, rng.choice([rng.randrange(0x400), rng.randrange(-5000, 5000)]))
    e.w32(STATE + 0x1C, rng.randrange(4))
    e.w32(STATE + 0x20, rng.randrange(3))
    e.w32(STATE + 0x24, rng.getrandbits(32))
    e.w32(STATE + 0x28, rng.randrange(2))


def rand_arm(env, rng, a):
    e = env.emu
    wfloats(e, a, [rng.choice([rf(rng, -130, 130), float(rng.randrange(-30, 31) * 4)]) for _ in range(4)])
    wfloats(e, a + 0x10, [rf(rng, -100, 100) for _ in range(4)])


def rand_mover(env, rng, a, w, h, n=1):
    e = env.emu
    for i in range(n):
        m = a + 0x34 * i
        x, y = pos(rng, w, h)
        wfloats(e, m, [x, y, rf(rng, -3, 3), rf(rng, -3, 3), rf(rng, 0.2, 3), rf(rng, 0.2, 2)])
        e.w32(m + 0x18, rng.choice([rng.randrange(0x400), rng.randrange(-3000, 3000)]))
        e.w32(m + 0x1C, rng.choice([rng.randrange(0x400), rng.randrange(-3000, 3000)]))
        e.w32(m + 0x20, rng.choice([0, 0, rng.randrange(-70, 70)]))
        e.w32(m + 0x24, rng.choice([0, 0, 1, rng.randrange(-70, 70)]))
        e.w32(m + 0x28, rng.randrange(4))
        e.w32(m + 0x2C, rng.choice([0, 0, 1, 2]))
        e.w32(m + 0x30, rng.getrandbits(32))


SEED = STATE + 0x1F0
CURL = STATE + 0x1F4
DONE = STATE + 0x1F8
ARM = STATE + 0x100


# ---------------------------------------------------------------- per-function setups
# each returns (args) after writing memory; called once per side with the same rng seed

def s_arm(env, rng):
    w, h = rand_field(env, rng)
    rand_objs(env, rng)
    rand_anims(env, rng)
    rand_boss(env, rng, w, h)
    rand_arm(env, rng, ARM)
    env.emu.w32(SEED, rng.getrandbits(32))
    env.emu.w32(CURL, rng.randrange(2))
    base = rng.choice([0x80, 0x180, 0x280, 0x380, rng.randrange(-0x800, 0x800)])
    return [STATE, base, OBJ_PTRS + 4 * rng.randrange(0, 19), ARM, CURL, ANIMS, SEED, rng.choice([0, 3])]


def s_bounce(env, rng):
    w, h = rand_field(env, rng)
    rand_boss(env, rng, w, h)
    env.emu.w32(SEED, rng.getrandbits(32))
    return [STATE, SEED]


def s_head(env, rng):
    w, h = rand_field(env, rng)
    rand_objs(env, rng)
    rand_mover(env, rng, STATE, w, h)
    env.emu.w32(SEED, rng.getrandbits(32))
    return [STATE, OBJ_PTRS + 4 * rng.randrange(24), ANIMS, SEED, rng.choice([0, 0, 0, 1])]


def s_body(env, rng):
    w, h = rand_field(env, rng)
    rand_objs(env, rng)
    rand_anims(env, rng)
    rand_mover(env, rng, STATE, w, h, 8)
    return [STATE, OBJ_PTRS + 4 * rng.randrange(0, 17), ANIMS]


def s_curlfire(env, rng):
    a = s_body(env, rng)
    return a + [rng.choice([0, 1, 8, -8, rng.randrange(-64, 65)]), rng.choice([0, 0, 1, 5]),
                rng.choice([0, 8, 7, rng.randrange(200)])]


def s_curlbeam(env, rng):
    a = s_body(env, rng)
    env.emu.w32(CURL, rng.choice([0, 1]))
    return a + [rng.choice([0, 1, 8, -8, rng.randrange(-64, 65)]), CURL, DONE, rng.choice([0, 1, -1])]


def s_spinner(env, rng):
    w, h = rand_field(env, rng)
    rand_objs(env, rng)
    rand_mover(env, rng, STATE, w, h)
    e = env.emu
    e.w32(STATE + 0x20, rng.choice([0, 0x40, -0x40, rng.randrange(-0x50, 0x50)]))
    e.w32(STATE + 0x1C, rng.choice([0, 0x200, 0x201, 0x3FC, rng.randrange(0x400), rng.randrange(-100, 0x500)]))
    e.w32(SEED, rng.getrandbits(32))
    return [STATE, OBJ_PTRS + 4 * rng.randrange(24), ANIMS, SEED, rng.choice([0, 0, 1, 2, 3])]


SIMPLE = [('Boss_UpdateArm', s_arm), ('Boss_UpdateArmAim', s_arm), ('Boss_MoveBounce', s_bounce),
          ('Snake_MoveHead', s_head), ('Snake_FollowBody', s_body), ('Snake_CurlFire', s_curlfire),
          ('Snake_CurlBeam', s_curlbeam), ('Spinner_Move', s_spinner)]


def run_simple(env, name, setup, trials):
    ok = 0
    first = None
    for t in range(trials):
        res = []
        for addr in (ORIG[name], env.syms[name]):
            env.restore()
            rng = random.Random(t * 7919 + sum(map(ord, name)))
            args = setup(env, rng)
            env.emu.call(addr, a=args)
            res.append((env.state(), list(env.log)))
        if res[0] == res[1]:
            ok += 1
        elif first is None:
            first = (t, diff(res[0], res[1]))
    print(f'{name:20s} {"PASS" if ok == trials else "FAIL"} {ok}/{trials}')
    if first:
        print('   first failure: trial', first[0], first[1])
    return ok == trials


def diff(a, b):
    (sa, la), (sb, lb) = a, b
    if la != lb:
        for i, (x, y) in enumerate(zip(la, lb)):
            if x != y:
                return f'log differs at {i}: {x} vs {y}'
        return f'log length {len(la)} vs {len(lb)}'
    names = ['ovl BSS', 'main BSS', 'test mem']
    bases = [OVL_BSS, MAIN_BSS, 0x80400000]
    for n, base, x, y in zip(names, bases, sa, sb):
        if x != y:
            i = next(i for i in range(len(x)) if x[i] != y[i])
            return f'{n} differs at {base + i:08X}: {x[i:i + 8].hex()} vs {y[i:i + 8].hex()}'
    return '?'


# ---------------------------------------------------------------- tasks

class Script:
    """Per-frame events applied by the Task_Yield stub (same for both runs)."""
    def __init__(self, env, rng, limit, quit_at, extra=None):
        self.env, self.rng, self.limit, self.quit_at, self.extra = env, rng, limit, quit_at, extra
        self.n = 0
        self.snaps = []

    def __call__(self, e):
        rng = self.rng
        self.snaps.append(e.read(OBJS, 24 * 0xB0) + e.read(OVL_BSS, OVL_BSS_END - OVL_BSS)
                          + e.read(P1, 0x20) + e.read(P2, 0x20) + e.read(STATE, 0x200))
        self.n += 1
        r = rng.random()
        if r < 0.01:
            e.w32(PAUSED, e.r32(PAUSED) ^ 1)
        elif r < 0.04:
            e.w32(P1 + 0x14, 0)
            e.w32(P2 + 0x14, 0)
        elif r < 0.05:
            e.w32(PLAYING + 4 * rng.randrange(2), rng.randrange(2))
        if self.extra:
            self.extra(e, rng, self.n)
        if self.n == self.quit_at:
            e.w32(STAGE_OVER, 1)
        if self.n >= self.limit:
            e.mu.emu_stop()


def run_task(env, name, setup, trials, limit):
    ok = 0
    first = None
    for t in range(trials):
        res = []
        for addr in (ORIG[name], env.syms[name]):
            env.restore()
            rng = random.Random(t * 7919 + sum(map(ord, name)))
            args, extra = setup(env, rng)
            lim = rng.choice([limit, limit // 3, limit // 10])
            quit_at = rng.choice([None, None, rng.randrange(1, lim + 1)])
            env.yield_fn = sc = Script(env, random.Random(rng.getrandbits(32)), lim, quit_at, extra)
            env.emu.call(addr, a=args, max_insns=2_000_000_000)
            env.yield_fn = None
            res.append((env.state(), list(env.log), sc.snaps))
        if res[0] == res[1]:
            ok += 1
        elif first is None:
            (sa, la, pa), (sb, lb, pb) = res
            if pa != pb:
                i = next((i for i in range(min(len(pa), len(pb))) if pa[i] != pb[i]), min(len(pa), len(pb)))
                msg = f'frame snapshot {i} differs (of {len(pa)}/{len(pb)})'
                if i < min(len(pa), len(pb)):
                    j = next(j for j in range(len(pa[i])) if pa[i][j] != pb[i][j])
                    msg += f' at byte {j:#x}'
                first = (t, msg + '; ' + diff((sa, la), (sb, lb)))
            else:
                first = (t, diff((sa, la), (sb, lb)))
    print(f'{name:20s} {"PASS" if ok == trials else "FAIL"} {ok}/{trials}')
    if first:
        print('   first failure: trial', first[0], first[1])
    return ok == trials


def s_minion(env, rng):
    rand_field(env, rng, density=rng.choice([0.0, 0.05, 0.3, 0.7]))
    rand_objs(env, rng)
    rand_anims(env, rng)
    return [rng.getrandbits(32), OBJS + 0xB0 * rng.randrange(24), ANIMS], None


ATTACK_RATE_SNAKE = 0x800F9C20


def s_snake(env, rng):
    w, h = rand_field(env, rng, density=rng.choice([0.0, 0.05, 0.3]))
    rand_objs(env, rng)
    rand_anims(env, rng)
    e = env.emu
    lvl = e.r32(LEVEL)
    e.w32(ATTACK_RATE_SNAKE + 4 * lvl, rng.choice([900, 100, 20, 3]))

    def extra(e, rng, n):
        if env.beam_done is not None and rng.random() < 0.03:
            e.w32(env.beam_done, 1)
    return [rng.getrandbits(32), OBJ_PTRS, rng.randrange(16, w - 16), rng.randrange(16, h - 16), ANIMS], extra


ATTACK_RATE_BOSS = 0x800F9BC0


def s_boss(env, rng):
    w, h = rand_field(env, rng, density=rng.choice([0.0, 0.05, 0.3]))
    rand_objs(env, rng)
    rand_anims(env, rng)
    e = env.emu
    lvl = e.r32(LEVEL)
    e.w32(ATTACK_RATE_BOSS + 4 * lvl, rng.choice([900, 100, 20, 2]))
    return [rng.getrandbits(32), OBJ_PTRS, rng.randrange(16, w - 16), rng.randrange(16, h - 16), ANIMS], None


TASKS = [('Minion_Task', s_minion, 3000), ('Snake_Task', s_snake, 3000), ('Boss_Task', s_boss, 3000)]


def main():
    env = Env()
    trials = int(sys.argv[1]) if len(sys.argv) > 1 else 300
    only = sys.argv[2:] 
    good = True
    for name, setup in SIMPLE:
        if not only or name in only:
            good &= run_simple(env, name, setup, trials)
    for name, setup, limit in TASKS:
        if not only or name in only:
            good &= run_task(env, name, setup, max(trials // 3, 1), limit)
    sys.exit(0 if good else 1)


if __name__ == '__main__':
    main()
