#!/usr/bin/env python3
"""Differential test of src/game/text.c (8x8 font text objects: glyph
expansion, string rendering, palettes, quad/texrect display lists, placing on
scene nodes) against the original MIPS code. The font is read from the real
ROM (0x196710) through the Rom_Read stub, as the game does."""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env, reset_heap, GAME_HEAP, GAME_HEAP_SIZE
from mipsbuild import CODE_BASE
from draft_mips import build_clean

FONT = 0x196710
SCREEN_W = 0x800C30D0
OBJ = 0x80600000        # TextObj (0x180)
NODE = 0x80601000       # SceneNode (0xE0)
STR = 0x80602000        # input string
GLYPH = 0x80603000      # glyph bytes
OUT = 0x80604000        # bitmap output area
OUTSZ = 0x2000
HEAP_ALLOC = 0x80003358

fb = lambda v: struct.unpack('>I', struct.pack('>f', v))[0]
u32 = lambda v: v & 0xFFFFFFFF

ORIG = {
    'Text_Create': 0x80015AA0, 'Text_Delete': 0x80015BE4, 'Text_Place': 0x80015C40,
    'Text_PlaceRotated': 0x80015D04, 'Text_PlaceScreen': 0x80015E14,
    'Text_SetColors': 0x80015EA4, 'Text_SetGradient': 0x80016094,
    'Text_SetString': 0x800161E8, 'Text_BuildQuadDL': 0x80016298,
    'Text_BuildRectDL': 0x8001650C, 'Text_RenderString': 0x800169B4,
    'Text_RenderInto': 0x80016B00, 'Text_ExpandGlyph': 0x80016C3C,
}


def rand_text(rng, maxlen):
    """ASCII, EUC-JP pairs, Shift-JIS pairs and odd control bytes."""
    out = b''
    target = rng.randrange(0, maxlen + 1)
    while len(out) < target:
        r = rng.random()
        if r < 0.5:
            out += bytes([rng.randrange(0x20, 0x7F)])
        elif r < 0.7:
            out += bytes([rng.randrange(0xA1, 0xFF), rng.randrange(0xA1, 0xFF)])
        elif r < 0.85:
            out += bytes([rng.choice([rng.randrange(0x81, 0xA0), rng.randrange(0xE0, 0xF0)]),
                          rng.randrange(0x40, 0xFD)])
        else:
            out += bytes([rng.randrange(1, 0x100)])
    return out[:target]


def rand_col(rng):
    return rng.choice([0, 0xFFFF, 0x0001, 0xF801, 0x07C1, 0x003F, rng.getrandbits(16)])


def rand_f(rng, lo=0.1, hi=4.0):
    r = rng.random()
    if r < 0.05:
        return rng.choice([1.0, 0.5, 2.0, -1.0])
    if r < 0.1:
        return -rng.uniform(lo, hi)
    return rng.uniform(lo, hi)


def setup(emu, rng, heap=True):
    """Identical, seed-driven memory set-up for both implementations."""
    if heap:
        reset_heap(emu)
        for _ in range(rng.randrange(0, 3)):    # shift where blocks land
            emu.call(HEAP_ALLOC, a=[GAME_HEAP, rng.randrange(1, 0x400)])
    emu.write(OBJ, bytes(rng.getrandbits(8) for _ in range(0x180)))
    node = b''.join(struct.pack('>f', rng.uniform(-3, 3)) for _ in range(0xE0 // 4))
    emu.write(NODE, node)
    emu.write(OUT, bytes(rng.getrandbits(8) for _ in range(64)) * (OUTSZ // 64))
    emu.w32(SCREEN_W, rng.choice([320, 320, 640, 512, 384]))


def snapshot(emu, heap):
    s = [emu.read(OBJ, 0x180), emu.read(NODE, 0xE0), emu.read(OUT, OUTSZ), emu.read(SCREEN_W, 4)]
    if heap:
        s.append(emu.read(GAME_HEAP, GAME_HEAP_SIZE))
    return s


def run(emu, name, addr, seed, syms):
    rng = random.Random(seed)
    heap = name in ('Text_Create', 'Text_Delete', 'Text_SetString', 'Text_RenderString')
    setup(emu, rng, heap)
    ret = True    # compare v0?
    if name == 'Text_ExpandGlyph':
        if rng.random() < 0.5:
            glyph = emu.rom[FONT + rng.randrange(0, 182) * 8:][:8]
        else:
            glyph = bytes(rng.getrandbits(8) for _ in range(8))
        emu.write(GLYPH, glyph)
        stride = rng.choice([4, 8, 16, 40, 120, rng.randrange(-64, 65) * 4])
        dst = OUT + 0x1000 + rng.randrange(0, 16) * 4
        pick = lambda: rng.choice([rng.randrange(0, 16), rng.randrange(0, 256), rng.getrandbits(32)])
        a = [dst, GLYPH, u32(stride), pick(), pick(), pick()]
        ret = False
    elif name in ('Text_RenderString', 'Text_RenderInto'):
        s = rand_text(rng, 60)
        emu.write(STR, s + b'\0')
        cols = [rng.randrange(0, 256) for _ in range(3)] if rng.random() < 0.5 else [3, 1, 2]
        if name == 'Text_RenderString':
            a = [STR, FONT] + cols
        else:
            a = [OUT, STR, FONT] + cols
    elif name in ('Text_SetColors', 'Text_SetGradient'):
        a = [OBJ] + [rand_col(rng) for _ in range(4)]
        ret = False
    elif name == 'Text_Create':
        s = rand_text(rng, 30)          # >30 chars overflows the original's stack buffer
        emu.write(STR, s + b'\0')
        a = [OBJ if rng.random() < 0.5 else 0, STR] + [rand_col(rng) for _ in range(4)]
    elif name in ('Text_Delete', 'Text_SetString'):
        s = rand_text(rng, 30)
        emu.write(STR, s + b'\0')
        obj = OBJ if rng.random() < 0.5 else 0
        o, _ = emu.call(ORIG['Text_Create'], a=[obj, STR] + [rand_col(rng) for _ in range(4)])
        if name == 'Text_Delete':
            a = [o]
        else:
            emu.write(STR, rand_text(rng, 40) + b'\0')
            a = [o, STR]
    elif name == 'Text_BuildQuadDL':
        n = rng.choice([rng.randrange(0, 34), rng.randrange(0, 34) * 2, rng.getrandbits(32)])
        emu.w32(OBJ + 0x170, n)
        ret = False
        a = [OBJ]
    elif name == 'Text_BuildRectDL':
        n = rng.choice([rng.randrange(0, 34), rng.randrange(0, 34) * 2, rng.getrandbits(32)])
        emu.w32(OBJ + 0x170, n)
        x = rng.choice([rng.randrange(-300, 300), rng.randrange(-200, 40), rng.randrange(-0x8000, 0x8000)])
        y = rng.choice([rng.randrange(-200, 200), rng.randrange(100, 400), rng.randrange(-0x8000, 0x8000)])
        a = [OBJ, u32(x), u32(y), fb(rand_f(rng, 0.05, 6.0)), fb(rand_f(rng, 0.05, 6.0))]
        ret = False
    elif name in ('Text_Place', 'Text_PlaceRotated', 'Text_PlaceScreen'):
        emu.w32(OBJ + 0x170, rng.randrange(0, 34))
        x, y = rng.uniform(-200, 200), rng.uniform(-200, 200)
        if name == 'Text_Place':
            a = [NODE, OBJ, fb(x), fb(y), fb(rng.uniform(-50, 50)), fb(rand_f(rng))]
        elif name == 'Text_PlaceRotated':
            a = [NODE, OBJ, fb(x), fb(y), fb(rng.uniform(-50, 50)), fb(rng.uniform(-7, 7)), fb(rand_f(rng))]
        else:
            a = [NODE, OBJ, fb(x), fb(y), fb(rand_f(rng, 0.05, 6.0)), fb(rand_f(rng, 0.05, 6.0))]
        ret = False
    v0, _ = emu.call(addr, a=a)
    return (v0 if ret else None), snapshot(emu, heap)


TRIALS = {'Text_Create': 300, 'Text_Delete': 300, 'Text_SetString': 300, 'Text_RenderString': 300}


def main(trials=400):
    blob, syms = build_clean(['src/game/text.c'], name='text')
    emu = make_env(); emu.write(CODE_BASE, blob)
    bad_total = 0
    for name, addr in ORIG.items():
        n = TRIALS.get(name, trials)
        bad = 0
        for s in range(n):
            ra = run(emu, name, addr, s, syms)
            rb = run(emu, name, syms[name], s, syms)
            if ra != rb:
                if bad == 0:
                    print(f'  {name}: first mismatch seed {s}: v0 {ra[0]} vs {rb[0]}')
                    for k, (x, y) in enumerate(zip(ra[1], rb[1])):
                        if x != y:
                            d = next(i for i in range(len(x)) if x[i] != y[i])
                            print(f'    region {k} differs at +{d:#x}: {x[d:d+16].hex()} vs {y[d:d+16].hex()}')
                bad += 1
        print(f'{name:20s} {"PASS" if not bad else "FAIL"} {n - bad}/{n}')
        bad_total += bad
    return 1 if bad_total else 0


if __name__ == '__main__':
    sys.exit(main())
