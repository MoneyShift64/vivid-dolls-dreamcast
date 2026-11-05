#!/usr/bin/env python3
"""Differential test of src/game/ovl2_b.c (overlay 2, 0x800E4EDC-0x800EAD80:
player drawing / edge walking / trail rewinding and two enemy tasks) against
the original MIPS code.

Every trial builds a random but realistic play field (2-bit cell grid with
claimed rectangles outlined by edge lines, random frame buffer and photo,
scroll position, two players, enemy block bitmap). Trails are produced by
running the ORIGINAL Player_Update for a random number of frames with random
input. From that snapshot the original and the C function are run; the two
GamePlayer structs (0x342A0 bytes each), the grid, the frame buffer, the whole
overlay BSS, the fake Obj and a log of the stubbed calls (task yield, sound,
Obj_SetPos/SetData/Hide, the shape fill 0x800E4A30) are compared.
The other overlay helpers (cell access, drawing step 0x800E4CFC, collision
tests 0x800E3C64/0x800E3D28) run as the original code.

Usage: test_ovl2_b.py [FuncName ...]   (TRIALS=n env var, default 300)"""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env
from mipsbuild import CODE_BASE
from draft_mips import build_clean

TRIALS = int(os.environ.get('TRIALS', '300'))

P1, P2 = 0x80400000, 0x80440000
PSIZE = 0x342A0
GRID = 0x80480000
FB = 0x80490000
IMG = 0x804C0000
BLOCKS = 0x804F0000
OBJ = 0x804F1000
ANIMS = 0x804F2000
DONE = 0x804F3000
OVL_BSS, OVL_BSS_SIZE = 0x800FB050, 0x2500

D_W, D_H, D_GRID, D_FB, D_IMG = 0x800FBA70, 0x800FBA74, 0x800FBA7C, 0x800FBA80, 0x800FBA84
D_SX, D_PAUSE, D_SY, D_STOP = 0x800FD2B8, 0x800FD2C0, 0x800FD2C8, 0x800FD2D8
D_P1, D_P2, D_BLOCKS = 0x800FD3F8, 0x800FD3FC, 0x800FD42C
D_LEVEL = 0x800B4AA0

YIELD, SOUND = 0x8000AEAC, 0x8001BA90
OBJ_SETPOS, OBJ_SETDATA, OBJ_HIDE = 0x80018EF4, 0x80018E34, 0x80018FA0
FILL_SHAPE = 0x800E4A30

ORIG = {
    'Player_DrawMove': 0x800E4EDC,
    'Player_EdgeMove': 0x800E664C,
    'Player_Update': 0x800E8F74,
    'Player_RewindTask': 0x800E93EC,
    'Player_FillTrailTask': 0x800E9B10,
    'Enemy_BounceTask': 0x800E9DC0,
    'Enemy_WanderTask': 0x800EA610,
}

u32 = lambda v: v & 0xFFFFFFFF


class Env:
    def __init__(self):
        blob, self.syms = build_clean(['src/game/ovl2_b.c'], name='ovl2_b')
        emu = self.emu = make_env(overlay=2)
        emu.write(CODE_BASE, blob)
        self.log = []
        self.yields = 0
        self.stop_at = None       # yield count at which the stop flag is set
        self.pause_sched = {}     # yield count -> new pause flag value

        def yld(e):
            self.yields += 1
            self.log.append(('Y',))
            if self.yields in self.pause_sched:
                e.w32(D_PAUSE, self.pause_sched[self.yields])
            if self.stop_at is not None and self.yields >= self.stop_at:
                e.w32(D_STOP, 1)
            if self.yields > 200000:
                raise SystemExit('runaway task')
        emu.stub(YIELD, yld)
        emu.stub(SOUND, lambda e: self.log.append(('S', e.arg(0))) or 0)
        emu.stub(OBJ_SETPOS, lambda e: self.log.append(('P', e.arg(0), e.arg(1), e.arg(2), e.arg(3))) or e.arg(0))
        emu.stub(OBJ_SETDATA, lambda e: self.log.append(('D', e.arg(0), e.arg(1))) or e.arg(0))
        emu.stub(OBJ_HIDE, lambda e: self.log.append(('H', e.arg(0))) or e.arg(0))
        emu.stub(FILL_SHAPE, lambda e: self.log.append(('F', e.arg(0))))

    def regions(self):
        return [(P1, PSIZE), (P2, PSIZE), (GRID, self.W * self.H // 4), (FB, self.W * self.H * 2),
                (OVL_BSS, OVL_BSS_SIZE), (OBJ, 0xB0), (DONE, 4), (BLOCKS, 0x400)]

    def snapshot(self):
        self.snap = [(a, self.emu.read(a, n)) for a, n in self.regions()]

    def restore(self):
        for a, b in self.snap:
            self.emu.write(a, b)
        self.log, self.yields = [], 0

    def state(self):
        return [self.emu.read(a, n) for a, n in self.regions()]


# ---------------------------------------------------------------- world builder

def set_cell(cells, W, x, y, v):
    cells[y * W + x] = v


def build_world(env, rng, clean=False):
    emu = env.emu
    W = env.W = rng.choice([256, 320, 320])
    H = env.H = rng.choice([224, 256])
    cells = bytearray(W * H)
    # outside of the field border: claimed; border: edge line; inside: open
    x0, y0 = rng.randrange(6, 14), rng.randrange(6, 14)
    x1, y1 = W - 1 - rng.randrange(6, 14), H - 1 - rng.randrange(6, 14)
    for y in range(H):
        for x in range(W):
            if x < x0 or x > x1 or y < y0 or y > y1:
                cells[y * W + x] = 1
            elif x in (x0, x1) or y in (y0, y1):
                cells[y * W + x] = 3
    # claimed rectangles outlined with edge lines
    env.corners = [(x0, y0), (x0, y1), (x1, y0), (x1, y1)]
    nrect, rmax = (rng.randrange(5, 40), rng.choice([8, 16, 30])) if clean else (rng.randrange(0, 7), 90)
    for _ in range(nrect):
        rx0 = rng.randrange(x0, x1 - 4)
        ry0 = rng.randrange(y0, y1 - 4)
        rx1 = min(x1, rx0 + rng.randrange(3, rmax))
        ry1 = min(y1, ry0 + rng.randrange(3, rmax))
        env.corners += [(rx0, ry0), (rx0, ry1), (rx1, ry0), (rx1, ry1)]
        for y in range(ry0, ry1 + 1):
            for x in range(rx0, rx1 + 1):
                edge = x in (rx0, rx1) or y in (ry0, ry1)
                cells[y * W + x] = 3 if edge else 1
    # some noise
    for _ in range(0 if clean else rng.choice([0, 0, 5, 40])):
        cells[rng.randrange(W * H)] = rng.randrange(4)
    packed = bytearray(W * H // 4)
    for i, c in enumerate(cells):
        packed[i >> 2] |= c << ((i & 3) * 2)
    env.cells = cells
    emu.write(GRID, packed)
    emu.write(FB, os.urandom(W * H * 2) if rng.random() < 0.5 else bytes(rng.getrandbits(8) for _ in range(64)) * (W * H // 32))
    emu.write(IMG, bytes(W * H * 2) if rng.random() < 0.3 else os.urandom(W * H * 2))
    emu.write(OVL_BSS, bytes(OVL_BSS_SIZE))
    emu.w32(D_W, W); emu.w32(D_H, H)
    emu.w32(D_GRID, GRID); emu.w32(D_FB, FB); emu.w32(D_IMG, IMG)
    sx = rng.choice([0, 0, rng.randrange(-24, 40), max(0, W - 320)])
    sy = rng.choice([0, 0, rng.randrange(-24, 48), max(0, H - 240)])
    if clean and rng.random() < 0.8:
        sx = sy = 0
    emu.w32(D_SX, sx); emu.w32(D_SY, sy)
    emu.w32(D_P1, P1); emu.w32(D_P2, P2)
    emu.w32(D_BLOCKS, BLOCKS)
    emu.w32(D_LEVEL, rng.randrange(12))
    emu.w32(D_PAUSE, 1 if rng.random() < 0.1 else 0)
    emu.w32(D_STOP, 0)
    # solid-block bitmap for the enemies (1 bit per 8x8 block, W/64 bytes per row)
    bm = bytearray(0x400)
    dens = rng.choice([0.0, 0.02, 0.1, 0.3])
    for i in range(W // 8 * H // 8):
        if rng.random() < dens:
            bm[i >> 3] |= 1 << (i & 7)
    # also mark blocks of claimed cells as solid sometimes
    if rng.random() < 0.5:
        for by in range(H // 8):
            for bx in range(W // 8):
                if cells[(by * 8 + 4) * W + bx * 8 + 4] == 1:
                    i = bx + by * (W // 8)
                    bm[i >> 3] |= 1 << (i & 7)
    emu.write(BLOCKS, bm)
    emu.write(OBJ, bytes(0xB0))
    emu.write(DONE, bytes(4))


def edge_cells(env):
    return [i for i, c in enumerate(env.cells) if c == 3]


def build_player(env, rng, addr, pid, on_edge=True):
    emu, W, H = env.emu, env.W, env.H
    # zero the struct but keep it cheap: header, trail start, bits, tail
    emu.write(addr, bytes(PSIZE))
    near = []
    if on_edge and rng.random() < 0.5:
        cx, cy = rng.choice(env.corners)
        near = [(cx + dx, cy + dy) for dx in range(-3, 4) for dy in range(-3, 4)
                if 0 <= cx + dx < W and 0 <= cy + dy < H and env.cells[(cy + dy) * W + cx + dx] == 3]
    if near:
        x, y = rng.choice(near)
    elif on_edge and rng.random() < 0.9:
        e = rng.choice(edge_cells(env))
        x, y = e % W, e // W
    else:
        x, y = rng.randrange(W), rng.randrange(H)
    if rng.random() < 0.1:
        x, y = rng.randrange(-4, W + 4), rng.randrange(-4, H + 4)
    hdr = [pid, x, y, rng.getrandbits(32), 0, 0, rng.getrandbits(32), 0]
    emu.write(addr, b''.join(struct.pack('>I', u32(v)) for v in hdr))
    emu.write(addr + 0x34024, os.urandom(0x258) if rng.random() < 0.5 else bytes(0x258))
    emu.w32(addr + 0x3427C, rng.choice([1, -1, 1, -1, 0, 5]))
    emu.w32(addr + 0x34280, rng.choice([1, -1, 1, -1, 0, -7]))
    emu.w32(addr + 0x34298, rng.choice([0, 0, 0, 1, 2, 30]))
    return x, y


def rand_input(rng, draw_bias=0.6):
    dpad = rng.choice([0, 1, 2, 4, 8, 5, 9, 6, 10, 3, 12, 15, rng.randrange(16)])
    return dpad | (0x10 if rng.random() < draw_bias else 0) | (rng.getrandbits(16) & ~0x1F if rng.random() < 0.1 else 0)


def grow_trail(env, rng, addr, frames):
    """Replay the ORIGINAL Player_Update to produce a realistic trail."""
    emu = env.emu
    held = rand_input(rng, 0.95) | 0x10
    for _ in range(frames):
        if rng.random() < 0.25:
            held = rand_input(rng, 0.95) | 0x10
        emu.call(ORIG['Player_Update'], a=[held, addr])
    env.log = []


def setup_basic(env, rng, trail_frames=None, clean=False):
    build_world(env, rng, clean)
    build_player(env, rng, P1, 0)
    build_player(env, rng, P2, 1)
    if trail_frames:
        emu = env.emu
        emu.w32(P1 + 0x34298, 0)
        grow_trail(env, rng, P1, trail_frames)
        if rng.random() < 0.3:
            emu.w32(P1 + 0x34298, rng.choice([0, 1, 3]))


# ---------------------------------------------------------------- per-function trials

def scramble_around(env, rng, values):
    """Random cells in the 7x7 neighbourhood of player 1 (dense corner/junction cases)."""
    emu, W, H = env.emu, env.W, env.H
    px, py = emu.r32(P1 + 4), emu.r32(P1 + 8)
    if not (3 <= px < W - 3 and 3 <= py < H - 3):
        return
    for dy in range(-3, 4):
        for dx in range(-3, 4):
            i = (py + dy) * W + px + dx
            v = rng.choice(values)
            env.cells[i] = v
            a = GRID + (i >> 2)
            b = emu.read(a, 1)[0] & ~(3 << ((i & 3) * 2)) | (v << ((i & 3) * 2))
            emu.write(a, bytes([b]))


def scroll_to_view_edge(env, rng):
    """Scroll so that player 1 is 0-2 pixels from an edge of the visible window."""
    emu = env.emu
    px, py = emu.r32(P1 + 4), emu.r32(P1 + 8)
    if rng.random() < 0.7:
        emu.w32(D_SX, px - 8 - rng.choice([0x12E, 0x12F, 0x130, 0x131, 0x132, -2, -1, 0, 1, 2]))
    if rng.random() < 0.7:
        emu.w32(D_SY, py - 8 - rng.choice([0xDE, 0xDF, 0xE0, 0xE1, 0xE2, -2, -1, 0, 1, 2]))


def tr_drawmove(env, rng):
    setup_basic(env, rng, rng.choice([0, 0, 3, 20]))
    emu = env.emu
    r = rng.random()
    if r < 0.15:
        emu.w32(P1 + 0x1C, rng.choice([0x1FFF, 0x2000, 0x2001, 0x1FFC]))
    elif r < 0.2:
        emu.w32(P1 + 0x1C, rng.randrange(0, 0x1FFF))
    if rng.random() < 0.5:   # put the player inside the open area
        opens = [i for i in range(0, len(env.cells), 7) if env.cells[i] == 0]
        if opens:
            e = rng.choice(opens)
            emu.w32(P1 + 4, e % env.W); emu.w32(P1 + 8, e // env.W)
    if rng.random() < 0.3:
        scramble_around(env, rng, [0, 0, 0, 3, 3, 1, 2])
    if rng.random() < 0.2:
        scroll_to_view_edge(env, rng)
    d = rng.choice(list(range(9)) * 3 + [9, 10, 0xFFFFFFFF])
    return d, P1


def tr_edgemove(env, rng):
    setup_basic(env, rng, clean=rng.random() < 0.5)
    if rng.random() < 0.7:   # 0-2 pixels from a corner, along one of its lines
        cx, cy = rng.choice(env.corners)
        cand = [(cx + dx, cy + dy) for dx in range(-3, 4) for dy in range(-3, 4)
                if 0 <= cx + dx < env.W and 0 <= cy + dy < env.H and env.cells[(cy + dy) * env.W + cx + dx] == 3]
        if cand:
            x, y = rng.choice(cand)
            env.emu.w32(P1 + 4, x); env.emu.w32(P1 + 8, y)
    if rng.random() < 0.6:
        scramble_around(env, rng, [3, 3, 3, 0, 1, 2])
    if rng.random() < 0.2:
        scroll_to_view_edge(env, rng)
    d = rng.choice(list(range(9)) * 3 + [9, 0x80000000])
    return d, P1


def tr_update(env, rng):
    setup_basic(env, rng, rng.choice([0, 0, 1, 5, 30, 80]))
    emu = env.emu
    if rng.random() < 0.1:
        emu.w32(P1 + 0x10, 1)
    n = emu.r32(P1 + 0x1C)
    if n >= 4 and rng.random() < 0.3:   # rewind point exactly on the edges of the view
        t = P1 + 0x20 + (n - 4) * 10
        tx = struct.unpack('>h', emu.read(t + 4, 2))[0]
        ty = struct.unpack('>h', emu.read(t + 6, 2))[0]
        emu.w32(D_SX, tx - 8 - rng.choice([0, -1, 0x12F, 0x130, 0x131, 0]))
        emu.w32(D_SY, ty - 8 - rng.choice([0, -1, 0xDF, 0xE0, 0xE1, 0]))
    return rand_input(rng, 0.5), P1


def tr_task(env, rng):
    setup_basic(env, rng, rng.choice([0, 1, 2, 5, 30, 80]))
    emu = env.emu
    n = emu.r32(P1 + 0x1C)
    if n and rng.random() < 0.2:            # a trail not a multiple of 4 is never made by the game
        pass
    env.stop_at = rng.choice([None, None, 1, 2, rng.randrange(1, 30)])
    return P1, DONE


def build_anims(env, rng):
    emu = env.emu
    p = ANIMS + 0x40
    sprites = ANIMS + 0x800
    for i in range(16):
        emu.w32(sprites + 4 * i, 0x81000000 + i * 0x100)

    def lst(n, loop):
        nonlocal p
        base = p
        ents = []
        for i in range(n + 1):
            ents.append((sprites + 4 * rng.randrange(16), rng.choice([0, 0, 1, 2, 5])))
        ents.append((sprites, rng.choice([-1, -1, -100])))
        for s, f in ents:
            emu.w32(p, s); emu.w32(p + 4, u32(f)); p += 8
        return base
    a = lst(rng.randrange(0, 5), False)
    m = lst(rng.randrange(0, 5), True)
    v = lst(rng.randrange(0, 5), False)
    emu.w32(ANIMS, a); emu.w32(ANIMS + 4, m); emu.w32(ANIMS + 8, v)
    emu.write(ANIMS + 0xC, bytes(4))


def tr_enemy(env, rng):
    build_world(env, rng)
    emu = env.emu
    for addr, pid in ((P1, 0), (P2, 1)):
        build_player(env, rng, addr, pid)
        if rng.random() < 0.5:
            emu.w32(addr + 0x34298, 0)
            grow_trail(env, rng, addr, rng.choice([5, 30, 80]))
        emu.w32(addr + 0x14, 1 if rng.random() < 0.2 else 0)
        if rng.random() < 0.5:
            emu.w32(addr + 0x1C, rng.choice([0, 4, 100]))
            emu.write(addr + 0x34024, os.urandom(0x258))
    build_anims(env, rng)
    if rng.random() < 0.15:   # (almost) no open cell left: the spot search has to yield
        W, H = env.W, env.H
        packed = bytearray(b'\x55' * (W * H // 4))
        for _ in range(rng.choice([0, 1, 3])):
            i = rng.randrange(W * H)
            packed[i >> 2] &= ~(3 << ((i & 3) * 2)) & 0xFF
        emu.write(GRID, packed)
    env.stop_at = rng.choice([1, 2, 600, 700, 1300, rng.randrange(600, 2500), rng.randrange(600, 4000)])
    if rng.random() < 0.3:
        env.pause_sched = {rng.randrange(600, 1500): 1, rng.randrange(1500, 2500): 0}
    return 0x12345678, OBJ, ANIMS


TESTS = [
    ('Player_DrawMove', tr_drawmove),
    ('Player_EdgeMove', tr_edgemove),
    ('Player_Update', tr_update),
    ('Player_RewindTask', tr_task),
    ('Player_FillTrailTask', tr_task),
    ('Enemy_BounceTask', tr_enemy),
    ('Enemy_WanderTask', tr_enemy),
]


def main():
    env = Env()
    emu = env.emu
    only = set(sys.argv[1:])
    fails = 0
    for name, setup in TESTS:
        if only and name not in only:
            continue
        ok = 0
        for t in range(TRIALS * {'Player_EdgeMove': 8, 'Player_DrawMove': 3}.get(name, 1)):
            rng = random.Random(f'{name}-{t}')
            env.stop_at, env.pause_sched = None, {}
            args = setup(env, rng)
            # the pause schedule / stop flag are part of the snapshot via the BSS
            env.snapshot()
            results = []
            for addr in (ORIG[name], env.syms[name]):
                env.restore()
                emu.call(addr, a=list(args), max_insns=2_000_000_000)
                results.append((env.state(), list(env.log)))
            (s0, l0), (s1, l1) = results
            if s0 == s1 and l0 == l1:
                ok += 1
            elif fails < 10:
                fails += 1
                print(f'  {name} trial {t} differs', end='')
                if l0 != l1:
                    i = next((i for i in range(min(len(l0), len(l1))) if l0[i] != l1[i]), min(len(l0), len(l1)))
                    print(f'; log differs at {i}/{len(l0)},{len(l1)}: {l0[i:i+3]} vs {l1[i:i+3]}', end='')
                for (a, n), b0, b1 in zip(env.regions(), s0, s1):
                    if b0 != b1:
                        j = next(j for j in range(n) if b0[j] != b1[j])
                        print(f'; mem {a + j:08X}: {b0[j:j+8].hex()} vs {b1[j:j+8].hex()}', end='')
                        break
                print()
        print(f'{name:22s} PASS {ok}/{t + 1}' if ok == t + 1 else f'{name:22s} FAIL {ok}/{t + 1}')
        if ok != t + 1:
            fails += 1
    sys.exit(1 if fails else 0)


if __name__ == '__main__':
    main()
