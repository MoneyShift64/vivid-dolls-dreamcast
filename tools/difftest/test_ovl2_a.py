#!/usr/bin/env python3
"""Differential test of src/game/ovl2_a.c (overlay 2, 0x800E0000-0x800E4EDC: game task,
HUD, board / flood fill, player lines) against the original MIPS code.

Overlay 2 is loaded at 0x800E0000. Every external the code calls that touches
hardware, the task system or other parts of the game (objects, sound, tasks, yields,
the photo decoder and photo view, other ovl2 functions) is replaced by a Python stub
that records its arguments; the call logs are compared. Heap, Rand_Range and bzero
run for real. Before every call the emulator is put back into the same snapshot,
the same state is set up (often with the ORIGINAL functions), then the original or
the C function is called; return values, the call log, main BSS, overlay data/BSS,
the test memory (and the heap for the game task) are compared.

The photo decoder is never run: Jpeg64_Decode is a stub that only reports a size."""
import os, random, struct, sys, zlib
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env, reset_heap, GAME_HEAP, GAME_HEAP_SIZE
from mipsbuild import CODE_BASE, ROOT
from draft_mips import build_clean, symbol_map
from unicorn.mips_const import UC_MIPS_REG_SP

SM = symbol_map()
BSS, BSS_SIZE = 0x8003E130, 0x800C5400 - 0x8003E130
OVL, OVL_SIZE = 0x800F6000, 0x800FD550 - 0x800F6000      # overlay 2 data + BSS
TEST, TEST_SIZE = 0x80400000, 0x180000
STACK_LO, STACK_SIZE = 0x807F0000, 0xF000

# overlay 2 BSS
BW, BH, BBLK, BCELL, BIMG, BPHOTO, BFILL = (0x800FBA70, 0x800FBA74, 0x800FBA78, 0x800FBA7C,
                                           0x800FBA80, 0x800FBA84, 0x800FBA88)
FILL_N = 0x800FB660
P1PTR, P2PTR = 0x800FD3F8, 0x800FD3FC
MON_Y, MON_X = 0x800FD2B4, 0x800FD2C4
SCROLL_X, SCROLL_Y = 0x800FD2B8, 0x800FD2C8
TIMER, POPUP_N, POPUP_PTS, POPUP_SND = 0x800FD424, 0x800FD428, 0x800FD404, 0x800FD400
POPUP_X, POPUP_Y = 0x800FD2D0, 0x800FD2DC

# main globals
G = {n: SM[n] for n in ['D_800C51D8', 'D_800B04D4', 'D_800B04D8', 'D_800C30C8', 'D_800C30C4',
                        'D_800AF260', 'D_800C0D48', 'D_800C0D4A', 'D_800C52A0', 'D_800C172C',
                        'D_800B056C', 'D_800B4A30', 'D_800C52F4', 'D_800AF264', 'D_800C3174',
                        'D_800C53D8', 'D_800B4AA0', 'D_800B0608', 'D_800B04CC'] if n in SM}
for n, a in [('D_800C178E', 0x800C178E), ('D_800B04DC', 0x800B04DC), ('D_800C1674', 0x800C1674)]:
    G[n] = a
for n in ['D_800C51D8', 'D_800B04D4', 'D_800B04D8', 'D_800C30C8', 'D_800C30C4', 'D_800AF260',
          'D_800C0D48', 'D_800C0D4A', 'D_800C52A0', 'D_800C172C', 'D_800B056C', 'D_800B4A30',
          'D_800C52F4', 'D_800AF264', 'D_800C3174', 'D_800C53D8', 'D_800B4AA0', 'D_800B0608',
          'D_800B04CC']:
    G.setdefault(n, int(n[2:], 16))

PLAYER_SIZE = 0x3429C
P1, P2 = TEST + 0xB0000, TEST + 0xF0000         # player structs in test memory
FAKE_VIEW = TEST + 0x170000
FAKE_OBJ = 0x81000000 - 0x100                       # never dereferenced (stubs)

ORIG = {
    'Game_Run': 0x800E0000, 'Hud_ShowStageScore': 0x800E00B8, 'Hud_UpdateTimer': 0x800E0230,
    'Hud_UpdateCredits': 0x800E0460, 'Hud_UpdateJoin': 0x800E0604,
    'Hud_UpdateGainPopup': 0x800E0820, 'Game_Main': 0x800E0BB8,
    'Board_Init': 0x800E2910, 'Board_GetWidth': 0x800E29C4, 'Board_GetHeight': 0x800E29D0,
    'Board_GetPercent': 0x800E29DC, 'Board_FloodFill': 0x800E2A24, 'Board_MarkBlock': 0x800E2A98,
    'Board_TestBlock': 0x800E2AE8, 'Board_GetBlockByte': 0x800E2B34, 'Board_SetEdge': 0x800E2B6C,
    'Board_SetEdgeCell': 0x800E2BEC, 'Board_SetLine': 0x800E2C40, 'Board_Reveal': 0x800E2CC0,
    'Board_Clear': 0x800E2D54, 'Board_SetPixel': 0x800E2DC4, 'Board_GetPixel': 0x800E2DF4,
    'Board_GetCell': 0x800E2E24, 'Board_RevealAll': 0x800E2EBC, 'Board_ShowLineBlocks': 0x800E2F44,
    'Fill_Scan': 0x800E3010, 'Fill_Span': 0x800E3718, 'Fill_Push': 0x800E3A44,
    'Fill_Pop': 0x800E3A78, 'Fill_TopX': 0x800E3A90, 'Fill_TopY': 0x800E3AAC,
    'Board_FillPixel': 0x800E3AC8, 'Bits_TestAny3': 0x800E3BC0, 'Bits_TestInner': 0x800E3C64,
    'Player_HitTest': 0x800E3D28, 'Player_InBox': 0x800E3E4C, 'Player_New': 0x800E3EAC,
    'Board_ClaimRect': 0x800E3F28, 'Player_Trace': 0x800E41B8, 'Player_IsInside': 0x800E445C,
    'Player_CommitPath': 0x800E48B0, 'Player_CloseArea': 0x800E4A30, 'Player_Step': 0x800E4CFC,
}

STUBS = ['Obj_SetData', 'Obj_SetPos', 'Obj_Show', 'Obj_Hide', 'Obj_Kill', 'Scene_Build',
         'Scene_Destroy', 'Camera_Init', 'Light_Init', 'Image_CopyRect', 'PhotoView_SetScroll',
         'PhotoView_Delete', 'func_8001BA90', 'func_8001BBC0', 'func_8000D6B8',
         'ovl2_func_800F1D6C', 'ovl2_func_800F200C', 'ovl2_func_800F2384', 'ovl2_func_800F2580']

fb = lambda v: struct.unpack('>I', struct.pack('>f', v))[0]
bf = lambda u: struct.unpack('>f', struct.pack('>I', u & 0xFFFFFFFF))[0]


class Env:
    def __init__(self):
        blob, self.syms = build_clean(['src/game/ovl2_a.c'], name='ovl2_a')
        self.c2o = {self.syms[n]: a for n, a in ORIG.items() if n in self.syms}
        emu = self.emu = make_env(overlay=2)
        emu.write(CODE_BASE, blob)
        self.log = []
        self.script = None
        self.nyield = 0
        self.nspawn = 0
        self.done_ptr = None
        for name in STUBS:
            addr = SM.get(name) or int(name.split('_')[-1], 16)
            emu.stub(addr, self._logger(name))
        emu.stub(SM['Obj_NewDL'], self._newdl)
        emu.stub(SM['PhotoView_New'], self._viewnew)
        emu.stub(SM['Image_Mosaic'], self._mosaic)
        emu.stub(SM['Jpeg64_Decode'], self._jpeg)
        emu.stub(0x8000A76C, self._spawn)
        emu.stub(0x8000AEAC, self._yield)
        emu.w32(0x800C30D0, 320)
        reset_heap(emu)
        self.regions = [(BSS, BSS_SIZE), (OVL, OVL_SIZE), (TEST, TEST_SIZE),
                        (GAME_HEAP, GAME_HEAP_SIZE), (STACK_LO, STACK_SIZE)]
        self.snap = [emu.read(a, n) for a, n in self.regions]
        self.snap[2] = bytes(TEST_SIZE)
        self.snap[4] = bytes(STACK_SIZE)
        self.jpeg_size = (320, 240)

    # ---- stubs ----
    def _args(self, n):
        e = self.emu
        sp = e.mu.reg_read(UC_MIPS_REG_SP)
        return [e.arg(i) for i in range(min(n, 4))] + [e.r32(sp + 0x10 + 4 * i) for i in range(max(0, n - 4))]

    def _logger(self, name):
        n = {'Obj_SetPos': 4, 'Obj_SetData': 2, 'Scene_Build': 2, 'Scene_Destroy': 2,
             'Image_CopyRect': 12, 'PhotoView_SetScroll': 3, 'ovl2_func_800F2384': 2,
             'ovl2_func_800F2580': 4}.get(name, 1)

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

    def _mosaic(self, e):
        self.log.append(('Image_Mosaic', tuple(self._args(9))))

    def _jpeg(self, e):
        a = self._args(6)
        self.log.append(('Jpeg64_Decode', tuple(a[:4])))
        e.w32(a[4], self.jpeg_size[0])
        e.w32(a[5], self.jpeg_size[1])

    def _spawn(self, e):
        a = self._args(3)
        nw = (a[2] + 3) // 4
        words = self._args(3 + nw)[3:]
        fn = self.c2o.get(a[0], a[0])
        norm = tuple('STACK' if STACK_LO <= w < STACK_LO + STACK_SIZE else w for w in words)
        self.log.append(('spawn', fn, a[1], a[2], norm))
        if self.done_ptr is not None:
            self.done_ptr.append(words[0])
        self.nspawn += 1
        return 0x80480000 + 0x10 * self.nspawn

    def _yield(self, e):
        self.log.append(('yield',))
        self.nyield += 1
        if self.script:
            self.script(e, self.nyield)

    # ---- helpers ----
    def restore(self):
        for (a, n), b in zip(self.regions, self.snap):
            self.emu.write(a, b)
        self.log = []
        self.nyield = 0
        self.nspawn = 0

    def state(self, heap=False):
        r = [self.emu.read(a, n) for a, n in self.regions[:3]]
        if heap:
            r.append(self.emu.read(GAME_HEAP, GAME_HEAP_SIZE))
        return r

    def o(self, name, *a):
        return self.emu.call(ORIG[name], a=list(a))[0]


def diff_regions(env, sa, sb):
    names = ['BSS', 'OVL', 'TEST', 'HEAP']
    bases = [BSS, OVL, TEST, GAME_HEAP]
    for nm, base, x, y in zip(names, bases, sa, sb):
        if x != y:
            i = next(k for k in range(len(x)) if x[k] != y[k])
            return f'{nm} differs at {base + i:08X}: {x[i]:02X} vs {y[i]:02X}'
    return None


class Tester:
    def __init__(self, env):
        self.env = env
        self.fail = 0

    def run(self, name, trials, setup, args_fn, ret=True, fret=False, heap=False, max_insns=50_000_000, cover=None):
        env, e = self.env, self.env.emu
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
                ctx = setup(r, t)
                a, f = args_fn(r, ctx)
                e.write(STACK_LO, bytes(STACK_SIZE))   # uninitialised locals read as 0
                try:
                    v0, _ = e.call(addr, a=a, f=f, max_insns=max_insns)
                    err = None
                except Exception as ex:      # noqa
                    v0, err = None, str(ex)
                res.append((v0, e.f0_bits(), list(env.log), env.state(heap), err))
            (v1, f1, l1, s1, e1), (v2, f2, l2, s2, e2) = res
            if e1 is not None:
                continue                      # original faulted: not a valid input
            n += 1
            msg = None
            if e2 is not None:
                msg = 'C faulted: ' + e2
            elif ret and (v1 & 0xFFFFFFFF) != (v2 & 0xFFFFFFFF):
                msg = f'v0 {v1:08X} vs {v2:08X}'
            elif fret and f1 != f2:
                msg = f'f0 {f1:08X} vs {f2:08X}'
            elif l1 != l2:
                k = next((i for i in range(min(len(l1), len(l2))) if l1[i] != l2[i]), min(len(l1), len(l2)))
                msg = f'call log differs at {k}: {l1[k:k+2]} vs {l2[k:k+2]} (len {len(l1)} vs {len(l2)})'
            else:
                msg = diff_regions(env, s1, s2)
            if cover is not None:
                cover(l1)
            if msg is None:
                ok += 1
            elif first is None:
                first = f'trial {t}: {msg}'
        status = 'PASS' if ok == n and n > 0 else 'FAIL'
        print(f'{name:22s} {status} {ok}/{n}' + (f'   ({first})' if first else ''))
        if status != 'PASS':
            self.fail += 1


# ----------------------------------------------------------------- board setup

class Board:
    """A board in test memory: image, photo, cells, block map."""
    def __init__(self, env, r, w=None, h=None, cells='random'):
        e = env.emu
        self.w = w or r.choice([8, 16, 24, 40, 64, 96])
        self.h = h or r.choice([8, 16, 30, 48, 64])
        npx = self.w * self.h
        self.img = TEST + 0x100
        self.photo = self.img + ((npx * 2 + 0xF) & ~0xF)
        self.cells = self.photo + ((npx * 2 + 0xF) & ~0xF)
        self.blocks = self.cells + ((npx // 4 + 0x1F) & ~0xF)
        assert self.blocks + npx // 8 + 0x10 < P1
        e.write(self.img, bytes(r.getrandbits(8) for _ in range(npx * 2)))
        e.write(self.photo, bytes(r.getrandbits(8) for _ in range(npx * 2)))
        e.call(ORIG['Board_Init'], a=[self.img, self.photo, self.cells, self.blocks, self.w, self.h])
        if cells == 'random':
            e.write(self.cells, bytes(r.getrandbits(8) for _ in range(npx // 4)))
            e.write(self.blocks, bytes(r.getrandbits(8) for _ in range(npx // 8)))
        e.w32(BFILL, r.choice([0, 1, 100, r.randrange(npx + 1)]))

    def rand_xy(self, r, margin=2):
        return (r.randrange(-margin, self.w + margin), r.randrange(-margin, self.h + margin))


def setcell(env, x, y, v):
    """cells via the original functions (keeps the image consistent)"""
    if v == 3:
        env.o('Board_SetEdge', x, y, 0xFFFF)
    elif v == 2:
        env.o('Board_SetLine', x, y, 0x1234)
    elif v == 1:
        env.o('Board_Reveal', x, y)
    else:
        env.o('Board_Clear', x, y, 0)


def new_player(env, addr, pid, x, y):
    e = env.emu
    e.write(addr, bytes(PLAYER_SIZE))
    e.w32(addr + 0, pid)
    e.w32(addr + 4, x)
    e.w32(addr + 8, y)
    return addr


def game_board(env, r, w=96, h=64):
    """Board with a claimed start rectangle, like the game (even coordinates)."""
    b = Board(env, r, w, h, cells='clear')
    rx = 2 * r.randrange(4, 10)
    ry = 2 * r.randrange(4, 8)
    rw = 2 * r.randrange(6, (w - rx) // 2 - 4)
    rh = 2 * r.randrange(4, (h - ry) // 2 - 4)
    env.emu.w32(G['D_800C51D8'], 0)
    env.o('Board_ClaimRect', rx, ry, rw, rh)
    b.rect = (rx, ry, rw, rh)
    return b


def draw_loop(env, r, b, p):
    """Player p leaves the top or bottom edge of the start rectangle, walks out and comes
    back to the same edge (2-pixel moves, as in the game). Returns True if valid."""
    e = env.emu
    rx, ry, rw, rh = b.rect
    top = r.random() < 0.5 or ry + rh + 8 >= b.h
    if top and ry < 8:
        return False
    sy = ry if top else ry + rh
    vy = -2 if top else 2
    xs = sorted(r.sample(range(rx // 2 + 1, (rx + rw) // 2), 2))
    x0, x1 = 2 * xs[0], 2 * xs[1]
    if r.random() < 0.5:
        x0, x1 = x1, x0
    lim = (sy - 2) // 2 if top else (b.h - sy - 3) // 2
    if lim < 1:
        return False
    up = 2 * r.randrange(1, min(lim, 8) + 1) * (1 if True else 1)
    e.w32(p + 4, x0)
    e.w32(p + 8, sy)
    moves = [(0, vy // 2)] * up + [((1 if x1 > x0 else -1), 0)] * (2 * abs(x1 - x0) // 2) + [(0, -vy // 2)] * up
    for dx, dy in moves:
        env.o('Player_Step', p, dx & 0xFFFFFFFF, dy & 0xFFFFFFFF)
    return True


# ----------------------------------------------------------------- tests

def main():
    env = Env()
    only = os.environ.get('ONLY')
    T = Tester(env)
    e = env.emu
    s32 = lambda v: v & 0xFFFFFFFF
    N = int(os.environ.get("NTRIALS", 300))

    # --- simple board functions on random boards
    def bsetup(r, t):
        return Board(env, r)

    def xy_args(extra=0):
        def f(r, b):
            x, y = b.rand_xy(r, 0)
            x = min(max(x, 0), b.w - 1)
            y = min(max(y, 0), b.h - 1)
            return [x, y] + [r.getrandbits(16) for _ in range(extra)], []
        return f

    T.run('Board_Init', N, lambda r, t: Board(env, r),
          lambda r, b: ([b.img, b.photo, b.cells, b.blocks, b.w, b.h], []), ret=False)
    T.run('Board_GetWidth', N, bsetup, lambda r, b: ([], []))
    T.run('Board_GetHeight', N, bsetup, lambda r, b: ([], []))
    T.run('Board_GetPercent', N, bsetup, lambda r, b: ([], []), ret=False, fret=True)
    for nm in ['Board_MarkBlock', 'Board_SetEdgeCell', 'Board_Reveal', 'Board_FillPixel']:
        T.run(nm, N, bsetup, xy_args(), ret=False)
    for nm in ['Board_TestBlock', 'Board_GetBlockByte', 'Board_GetPixel']:
        T.run(nm, N, bsetup, xy_args())
    for nm in ['Board_SetEdge', 'Board_SetLine', 'Board_Clear', 'Board_SetPixel']:
        T.run(nm, N, bsetup, xy_args(1), ret=False)
    T.run('Board_GetCell', N, bsetup, lambda r, b: (list(map(s32, b.rand_xy(r, 3))), []))
    T.run('Board_RevealAll', N, bsetup, lambda r, b: ([], []), ret=False)

    def lineblocks_setup(r, t):
        b = Board(env, r)
        e.write(P1 + 0x34024, bytes(r.getrandbits(8) for _ in range(0x278)))
        e.w32(P1PTR, P1)
        return b
    T.run('Board_ShowLineBlocks', N, lineblocks_setup, lambda r, b: ([], []), ret=False)

    # --- fill stack
    def stack_setup(r, t):
        n = r.randrange(0, 200)
        e.write(0x800FB670, bytes(r.getrandbits(8) for _ in range(0x400)))
        e.w32(FILL_N, n)
        return n
    T.run('Fill_Push', N, stack_setup, lambda r, n: ([s32(r.randrange(-500, 500)), s32(r.randrange(-500, 500))], []), ret=False)
    T.run('Fill_Pop', N, stack_setup, lambda r, n: ([], []), ret=False)
    T.run('Fill_TopX', N, stack_setup, lambda r, n: ([], []))
    T.run('Fill_TopY', N, stack_setup, lambda r, n: ([], []))

    # --- flood fill on boards with edge rectangles and noise
    def fill_board(r, t):
        b = Board(env, r, r.choice([16, 32, 48, 64]), r.choice([16, 24, 40]), cells='clear')
        for _ in range(r.randrange(0, 4)):
            x0, y0 = r.randrange(b.w), r.randrange(b.h)
            x1, y1 = r.randrange(x0, b.w), r.randrange(y0, b.h)
            v = r.choice([3, 3, 1, 2])
            for x in range(x0, x1 + 1):
                setcell(env, x, y0, v)
                setcell(env, x, y1, v)
            for y in range(y0, y1 + 1):
                setcell(env, x0, y, v)
                setcell(env, x1, y, v)
        for _ in range(r.randrange(0, 20)):
            setcell(env, r.randrange(b.w), r.randrange(b.h), r.choice([1, 2, 3]))
        e.w32(BFILL, r.randrange(1000))
        return b

    def fill_xy(r, b):
        return [r.randrange(b.w), r.randrange(b.h)], []
    T.run('Board_FloodFill', N, fill_board, fill_xy, ret=False)
    T.run('Fill_Span', N, lambda r, t: (fill_board(r, t), e.w32(FILL_N, 0))[0], fill_xy, ret=False)
    T.run('Fill_Scan', N, lambda r, t: (fill_board(r, t), e.w32(FILL_N, 0))[0],
          lambda r, b: ([s32(r.choice([-1, 1])), r.randrange(b.w), r.randrange(b.h),
                         r.choice([0, 1, 5]), r.choice([0, 1, 7])], []), ret=False)

    def rect_setup(r, t):
        b = Board(env, r, r.choice([32, 64, 96]), r.choice([24, 48]), cells='clear')
        e.w32(G['D_800C51D8'], r.choice([0, 0, 1]))
        return b

    def rect_args(r, b):
        x, y = r.randrange(0, b.w // 3), r.randrange(0, b.h - 6)
        w = r.randrange(2, b.w // 3)
        h = r.randrange(2, b.h - y - 1)
        return [x, y, w, h], []
    T.run('Board_ClaimRect', N, rect_setup, rect_args, ret=False)

    # --- line maps
    def bits_setup(r, t):
        b = Board(env, r, r.choice([64, 128, 320, 640]), r.choice([48, 240]), cells='clear')
        for a in (TEST + 0x140000, TEST + 0x141000, TEST + 0x142000):
            e.write(a, bytes(r.choice([0, 0, 0xFF, r.getrandbits(8)]) for _ in range(0x1000)))
        return b
    T.run('Bits_TestAny3', N, bits_setup,
          lambda r, b: ([TEST + 0x140000, TEST + 0x141000, TEST + 0x142000, r.randrange(b.w), r.randrange(b.h)], []))
    T.run('Bits_TestInner', N, bits_setup,
          lambda r, b: ([TEST + 0x140000, s32(r.randrange(-4, b.w + 4)), s32(r.randrange(-4, b.h + 4))], []))

    def player_rand(r, t):
        b = Board(env, r, r.choice([64, 128, 320]), r.choice([48, 240]), cells='clear')
        new_player(env, P1, 0, r.randrange(b.w), r.randrange(b.h))
        e.w32(P1 + 0x1C, r.choice([0, 0, 1, 50]))
        e.write(P1 + 0x34024, bytes(r.choice([0, 0xFF, r.getrandbits(8)]) for _ in range(0x278)))
        return b

    def box_args(r, b):
        px, py = e.r32(P1 + 4), e.r32(P1 + 8)
        x = px + r.randrange(-12, 13) if r.random() < 0.6 else r.randrange(-4, b.w + 4)
        y = py + r.randrange(-12, 13) if r.random() < 0.6 else r.randrange(-4, b.h + 4)
        return [P1, s32(x), s32(y), r.choice([1, 2, 7, 8, 16, 24]), r.choice([1, 2, 7, 8, 16, 24])], []
    T.run('Player_HitTest', N, player_rand, box_args)
    T.run('Player_InBox', N, player_rand, box_args)

    def pnew_setup(r, t):
        reset_heap(e)
        return None
    T.run('Player_New', N, pnew_setup,
          lambda r, c: ([r.randrange(2), r.randrange(0, 320), r.randrange(0, 240)], []), heap=True)

    # --- player lines
    def step_setup(r, t):
        b = game_board(env, r)
        p = new_player(env, P1, r.randrange(2), 0, 0)
        e.w32(P1 + 4, r.randrange(1, b.w - 1))
        e.w32(P1 + 8, r.randrange(1, b.h - 1))
        e.w32(P1 + 0x1C, r.randrange(0, 40))
        e.w32(P1 + 0x10, r.choice([0, 1]))
        return b
    T.run('Player_Step', N, step_setup,
          lambda r, b: ([P1, s32(r.choice([-1, 0, 1])), s32(r.choice([-1, 0, 1]))], []), ret=False)

    def loop_setup(r, t):
        while True:
            b = game_board(env, r)
            new_player(env, P1, r.randrange(2), 0, 0)
            if draw_loop(env, r, b, P1):
                break
            env.restore()
        e.w16(MON_X, r.randrange(0, b.w))
        e.w16(MON_Y, r.randrange(0, b.h))
        e.w32(SCROLL_X, r.randrange(0, 40))
        e.w32(SCROLL_Y, r.randrange(0, 40))
        return b
    T.run('Player_CommitPath', N, loop_setup, lambda r, b: ([P1], []), ret=False)
    T.run('Player_Trace', N, loop_setup,
          lambda r, b: ([P1, P1 + 0x14020 + 0x10000 * r.randrange(2), r.choice([3, 7])], []))

    def traced_setup(r, t):
        b = loop_setup(r, t)
        # trace both ways with the original, like Player_CloseArea
        for k, d in enumerate((3, 7)):
            n = env.o('Player_Trace', P1, P1 + 0x14020 + 0x10000 * k, d)
            e.w16(P1 + 0x34020 + 2 * k, n)
        return b
    def inside_args(r, b):
        k = r.random()
        if k < 0.35:                      # near a point of the line
            i = r.randrange(max(1, e.r32(P1 + 0x1C)))
            x, y = struct.unpack('>hh', e.read(P1 + 0x24 + 10 * i, 4))
        elif k < 0.7:                     # near a point of a trace
            t = r.randrange(2)
            i = r.randrange(max(1, e.r16(P1 + 0x34020 + 2 * t)))
            x, y = struct.unpack('>hh', e.read(P1 + 0x14024 + 0x10000 * t + 8 * i, 4))
        else:
            x, y = r.randrange(-2, b.w + 2), r.randrange(-2, b.h + 2)
        if k < 0.7:
            x += r.randrange(-3, 4)
            y += r.randrange(-2, 3)
        return [P1, x & 0xFFFFFFFF, y & 0xFFFFFFFF], []
    T.run('Player_IsInside', N, traced_setup, inside_args)
    T.run('Player_CloseArea', N, loop_setup, lambda r, b: ([P1], []), ret=False)

    # --- HUD
    def hud_setup(r, t):
        for i, a in enumerate(range(0x800FB138, 0x800FB200, 4)):
            e.w32(a, 0x80600000 + 0x100 * i)          # fake objects
        e.w32(TIMER, r.choice([0, 1, 1800, 1801, 1799, 7200, 60, r.randrange(0, 20000)]))
        e.w32(G['D_800C30C8'], r.choice([0, 0, 1, 5, 99, 100, 150]))
        e.w32(G['D_800B056C'], r.choice([0, 1, 2, 3]))
        e.w32(G['D_800B4A30'], r.choice([0, 1]))
        e.w32(G['D_800C52F4'], r.randrange(0, 10))
        e.w32(G['D_800AF264'], r.randrange(0, 10))
        e.w32(G['D_800B04D4'], r.choice([0, 0, 3]))
        e.w32(G['D_800B04D8'], r.choice([0, 0, 2]))
        e.w16(G['D_800C0D48'], r.choice([0, 0x100, 0x110, r.getrandbits(16)]))
        e.w16(G['D_800C0D4A'], r.choice([0, 0x100, r.getrandbits(16)]))
        e.w16(G['D_800C52A0'], r.getrandbits(16))
        e.w32(G['D_800C172C'], r.choice([0, 23, 24, 240, 2376, 2400, 5000, r.randrange(-100, 3000)]))
        e.w32(POPUP_N, r.choice([0, 0, 1, 60, r.randrange(60)]))
        e.w32(POPUP_PTS, r.choice([0, 1, 2, 9, 10, 14, 15, 24, 25, 44, 45, 99, r.randrange(100)]))
        e.w32(POPUP_SND, r.choice([0, 1]))
        e.wf(POPUP_X, r.uniform(-20, 330))
        e.wf(POPUP_Y, r.uniform(-20, 250))
        e.w32(0x800F9900, r.getrandbits(32))
        return None
    T.run('Hud_ShowStageScore', N, hud_setup, lambda r, c: ([], []))
    T.run('Hud_UpdateTimer', N, hud_setup, lambda r, c: ([], []), ret=False)
    T.run('Hud_UpdateCredits', N, hud_setup, lambda r, c: ([], []), ret=False)
    T.run('Hud_UpdateJoin', N, hud_setup, lambda r, c: ([s32(r.choice([0, 0x20, 0x40, 0x60, r.getrandbits(32)]))], []), ret=False)
    T.run('Hud_UpdateGainPopup', N, hud_setup, lambda r, c: ([], []), ret=False)

    # --- overlay entry
    def run_setup(r, t):
        env.done_ptr = []
        k = r.randrange(0, 5)
        val = r.choice([1, 0x10001, 0x100FF, 0x12345678])

        def script(em, ny):
            if ny > k and env.done_ptr:
                em.w32(env.done_ptr[0], val)
        env.script = script
        e.write(TEST, struct.pack('>II', r.choice([0, 1]), r.getrandbits(32)))
        return None
    T.run('Game_Run', N, run_setup, lambda r, c: ([TEST], []))
    env.done_ptr = None

    # --- the game task
    def game_setup(r, t):
        reset_heap(e)
        for i, a in enumerate(range(0x800FB138, 0x800FB200, 4)):
            e.w32(a, 0x80600000 + 0x100 * i)
        e.w32(FAKE_VIEW + 0, 0x80610000)
        e.w32(FAKE_VIEW + 4, 0x80610100)
        e.w32(FAKE_VIEW + 8, 0x80610200)
        env.jpeg_size = r.choice([(320, 240), (640, 240), (320, 480)])
        e.w32(G['D_800C51D8'], r.choice([0, 0, 0, 0, 40, 300]))
        e.w32(G['D_800B04D4'], r.choice([1, 3, 3, 0]))
        e.w32(G['D_800B04D8'], r.choice([0, 0, 2]))
        e.w32(G['D_800C30C8'], r.choice([0, 0, 1, 3]))
        e.w32(G['D_800C30C4'], 0)
        e.w32(G['D_800AF260'], 0)
        e.w16(G['D_800C0D48'], 0)
        e.w16(G['D_800C0D4A'], 0)
        e.w16(G['D_800C52A0'], r.getrandbits(16))
        e.w32(G['D_800C3174'], r.randrange(0, 5))
        e.w32(G['D_800B4AA0'], r.randrange(0, 8))
        e.w32(G['D_800B0608'], r.randrange(0, 8))
        e.w32(G['D_800B04CC'], r.randrange(0, 3))
        e.w32(G['D_800B04DC'], r.getrandbits(32))
        # sometimes every other girl is done already, so a clear leads to the ending
        done = r.random() < 0.3
        e.write(G['D_800C178E'], bytes([1 if (done or r.random() < 0.5) else 0 for _ in range(8)]))
        if done:
            e.w32(G['D_800B04CC'], 2)
            e.write(G['D_800C178E'] + e.r32(G['D_800B0608']), b'\0')
        e.w32(0x800FD2C0, r.choice([0, 5]))
        plan = r.choice(['clear', 'clear', 'perfect', 'timeup', 'gameover', 'abort', 'abort2', 'cam'])
        limit = r.randrange(150, 2500)
        ev_frame = r.randrange(1, 60)

        def script(em, ny):
            rr = random.Random(seed_base * 7919 + ny)
            p1, p2 = em.r32(P1PTR), em.r32(P2PTR)
            if p1 and p2 and 0x80000000 <= p1 < 0x80800000:
                if rr.random() < 0.2:
                    em.w32(p1 + 0x18, rr.choice([0, 1]))
                    em.w32(p2 + 0x18, rr.choice([0, 1]))
                    em.w32(p1 + 4, rr.randrange(0, 640))
                    em.w32(p1 + 8, rr.randrange(0, 480))
                    em.w32(p2 + 4, rr.randrange(0, 640))
                    em.w32(p2 + 8, rr.randrange(0, 480))
            if rr.random() < 0.05:
                em.w16(G['D_800C0D48'], rr.choice([0, 0x10, 0x100]))
                em.w16(G['D_800C0D4A'], rr.choice([0, 0x100]))
                em.w16(G['D_800C53D8'], rr.choice([0, 0, 0, 1]))
            if rr.random() < 0.02:
                em.w32(POPUP_N, 30)
                em.w32(POPUP_PTS, rr.randrange(0, 50))
                em.w32(POPUP_SND, 1)
            if ny == ev_frame:
                if plan in ('clear', 'perfect'):
                    w, h = env.jpeg_size
                    em.w32(BFILL, w * h if plan == 'perfect' else int(w * h * rr.uniform(0.67, 0.9)))
                elif plan == 'timeup':
                    em.w32(TIMER, 2)
                elif plan == 'gameover':
                    em.w32(G['D_800B04D4'], 0)
                    em.w32(G['D_800B04D8'], 0)
                elif plan == 'abort':
                    em.w32(G['D_800AF260'], 1)
                elif plan == 'abort2':
                    em.w16(G['D_800C30C4'], 1)
            if ny > limit:
                em.w32(G['D_800AF260'], 1)
        seed_base = r.getrandbits(32)
        env.script = script
        e.w32(TEST, 0xDEADBEEF)
        return None
    paths = {}

    def cover(log):
        marks = {'stage clear': ('func_8000D6B8', ()), 'perfect': ('ovl2_func_800F2580', (0xA0, 0x94, 0xEA, 0x14)),
                 'time up': ('func_8001BBC0', (0,)), 'continue': ('func_8001BBC0', (4,)),
                 'final texts': ('func_8001BBC0', (0xE,)), 'ending icons': ('func_8001BBC0', (5,)),
                 'round restart': None}
        for k, m in marks.items():
            if m and any(c[0] == m[0] and c[1][:len(m[1])] == m[1] for c in log if len(c) > 1):
                paths[k] = paths.get(k, 0) + 1
        if sum(1 for c in log if c[0] == 'func_8001BBC0' and c[1][0] >= 6) > 1:
            paths['round restart'] = paths.get('round restart', 0) + 1
    T.run('Game_Main', int(os.environ.get('GAME_TRIALS', 300)), game_setup,
          lambda r, c: ([TEST, r.getrandbits(32)], []), ret=False, heap=True, max_insns=400_000_000, cover=cover)
    if paths: print('    Game_Main paths covered:', ', '.join(f'{k} {v}' for k, v in sorted(paths.items())))
    env.script = None

    print('ALL PASS' if T.fail == 0 else f'{T.fail} FAILED')
    sys.exit(1 if T.fail else 0)


if __name__ == '__main__':
    main()
