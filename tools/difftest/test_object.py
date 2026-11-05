#!/usr/bin/env python3
"""Differential test of src/game/object.c (fonts/text, allocation list, models,
the object system and the scene tables) against the original MIPS code.

Before every call the emulator is put back into the same snapshot (scene system
and object list initialised, empty heap), the same setup is replayed with the
ORIGINAL functions, then the original or the C function is called. The return
value, the whole BSS (0x8003E130-0x800C5400), the whole game heap and the test
memory are compared, plus a log of callbacks made through a stub."""
import os, random, re, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
import emu as EMU
from unicorn import UC_HOOK_CODE
from game_env import make_env, reset_heap, GAME_HEAP, GAME_HEAP_SIZE
from mipsbuild import CODE_BASE, ROOT
from draft_mips import build_clean
sys.path.insert(0, os.path.join(ROOT, 'tools'))
import assets as A

BSS, BSS_SIZE = 0x8003E130, 0x800C5400 - 0x8003E130
TEST, TEST_SIZE = 0x80400000, 0x40000
FONT_TABLE = 0x80038C00          # D_80038C00[32] (data, rewritten by tests)
SCREEN_W = 0x800C30D0
SCENE_INIT = 0x80011550
GROUP_NODES = [0x800C316C, 0x800C30C0, 0x800C0AB4, 0x800AF234, 0x800B061C, 0x800B04A0]
STUB = 0x80480000                # callback stub address (Python)
ASSET_BASE = 0x611F0
SCREEN_SPRITE = 0x669DC
MODEL_ROM = 0x700000             # synthetic model assets are put in the unused ROM area

REGIONS = [(BSS, BSS_SIZE), (GAME_HEAP, GAME_HEAP_SIZE), (TEST, TEST_SIZE), (FONT_TABLE, 0x80)]

fb = lambda v: struct.unpack('>I', struct.pack('>f', v))[0]


def orig_table():
    s = open(os.path.join(ROOT, 'src/game/object.h')).read()
    return {n: int(a, 16) for n, a in re.findall(r'\b(\w+)\([^;]*?\);\s*/\* (0x[0-9A-F]{8}) \*/', s, re.S)}


ORIG = orig_table()


def make_fast_env():
    """make_env(), but with the stubs hooked only at their own addresses (N64Emu hooks
    every instruction with a Python callback, which makes long runs very slow)."""
    handles, real = [], EMU.Uc.hook_add

    def rec(self, htype, cb, *a, **k):
        h = real(self, htype, cb, *a, **k)
        handles.append((htype, h))
        return h
    EMU.Uc.hook_add = rec
    try:
        emu = make_env()
    finally:
        EMU.Uc.hook_add = real
    for t, h in handles:
        if t == UC_HOOK_CODE:
            emu.mu.hook_del(h)
    return emu


def hook_stubs(emu):
    for addr in emu.stubs:
        emu.mu.hook_add(UC_HOOK_CODE, emu._hook, begin=addr, end=addr)


class Env:
    def __init__(self):
        blob, self.syms = build_clean(['src/game/object.c'], name='object')
        emu = self.emu = make_fast_env()
        emu.write(CODE_BASE, blob)
        emu.rom = bytearray(emu.rom) + bytes(max(0, 0x800000 - len(emu.rom)))
        self.log = []
        emu.stub(STUB, lambda e: self.log.append((e.arg(0), e.r16(e.arg(0) + 0x5A))))
        hook_stubs(emu)
        rom = bytes(emu.rom)
        sp = sorted((len(d), o) for i, o, comp, d in A.assets(rom) if b'Sp3d' in A.chunks(d))
        self.small_sprites = [o for n, o in sp[:40]]
        self.models = []
        # baseline: scene system + object list initialised, empty heap
        emu.w32(SCREEN_W, 320)
        reset_heap(emu)
        emu.call(SCENE_INIT)
        emu.call(ORIG['ObjList_Init'])
        self.groups = [emu.r32(a) for a in GROUP_NODES]
        self.snap = [emu.read(a, n) for a, n in REGIONS]
        self.snap[2] = bytes(TEST_SIZE)

    def restore(self):
        for (a, n), b in zip(REGIONS, self.snap):
            self.emu.write(a, b)
        self.log = []

    def state(self):
        return [self.emu.read(a, n) for a, n in REGIONS]

    def o(self, name, *a):
        """Call an original function during setup."""
        return self.emu.call(ORIG[name], a=list(a))[0]


# ---------------------------------------------------------------- test data builders

class Mem:
    """Bump allocator in the test area (reset every trial)."""
    def __init__(self, emu):
        self.emu, self.p = emu, TEST + 0x100

    def alloc(self, data, align=16):
        self.p = (self.p + align - 1) & ~(align - 1)
        a = self.p
        self.emu.write(a, data)
        self.p += max(len(data), 1)
        return a

    def words(self, ws):
        return self.alloc(b''.join(struct.pack('>I', w & 0xFFFFFFFF) for w in ws))


def rand_bytes(rng, n):
    return bytes(rng.getrandbits(8) for _ in range(n))


def fake_font(env, m, rng, n=None, fmt=None):
    """Font struct whose 'sprites' are fake Sprite structs (only dl/format/setup/finish are read)."""
    n = n or rng.randrange(1, 12)
    sprites = []
    for i in range(n):
        spr = bytearray(rand_bytes(rng, 0x30))
        if fmt is not None:
            spr[0x26] = fmt
        sprites.append(m.alloc(bytes(spr)))
    glyphs = [m.alloc(struct.pack('>4h', *[rng.choice([0, 1, -3, rng.randrange(-200, 200),
                                                          rng.randrange(-32768, 32768)]) for _ in range(4)]))
              for _ in range(n)]
    sp = m.words(sprites)
    gp = m.words(glyphs)
    return m.words([n, sp, gp]), n


def rand_str(rng, n, maxlen=40):
    L = rng.choice([0, 1, 5, 31, 32, 33, rng.randrange(0, maxlen)])
    return bytes(0x20 + rng.randrange(n) for _ in range(L)) + b'\0'


def rfloat(rng, big=500.0):
    return rng.choice([0.0, 1.0, -1.0, 0.5, rng.uniform(-big, big), rng.uniform(-5, 5)])


def lzss_literal(data):
    out = bytearray(struct.pack('<I', len(data)))
    for i in range(0, len(data), 8):
        out.append(0)
        out += data[i:i + 8]
    return bytes(out) + bytes(16)


def make_model(rng, lighting_mix=True):
    """Random model asset (decompressed form) obeying the loader's alignment needs."""
    nv = rng.randrange(1, 12)
    nt = rng.randrange(0, 6)
    o1 = rng.randrange(0, 0x400) & ~7
    o2 = o1 + (rng.randrange(0, nv + 1) * 16)
    o3 = o1 + nv * 16
    o4 = o3 + nt * 16
    o5 = o4 + rng.randrange(0, 8) * 8
    o6 = o5 + rng.randrange(0, 8) * 8
    texs = b''
    for _ in range(nt):
        def dim():
            v = rng.choice([8, 16, 32, 64, 4, 128, 24, 40, 256, 1024])
            if rng.random() < 0.3:
                v |= 0x80
            if rng.random() < 0.1:
                v = rng.randrange(1, 0x10000) | 1
            return v
        texs += struct.pack('>IHHII', rng.getrandbits(32), dim(), dim(),
                            rng.randrange(0, 0x100), rng.randrange(0, 0x100))
    cmds, ngfx, mode = [], 1, 0
    for _ in range(rng.randrange(0, 30)):
        r = rng.random()
        if r < 0.4:
            cmds.append(rng.randrange(0, 0x4000)); ngfx += 1
        elif r < 0.7:
            c = 0x4000 | rng.randrange(0, 0x4000)
            cmds += [c, rng.randrange(0, 0x10000)]; ngfx += 3
        elif nt:
            cmds.append(0x8000 | (rng.randrange(0x40) << 8) | rng.randrange(nt)); ngfx += 7
    cmds.append(0xC000 | rng.randrange(0, 0x4000))
    hdr = struct.pack('>8I', rng.getrandbits(32), o1, o2, o3, o4, o5, o6,
                      ngfx + rng.choice([0, 0, 3]))
    body = rand_bytes(rng, o3 - o1) + texs + rand_bytes(rng, o6 - o4)
    return hdr + body + b''.join(struct.pack('>H', c) for c in cmds)


def put_model(env, rng, k=None):
    k = rng.randrange(64) if k is None else k
    off = MODEL_ROM + k * 0x1000
    data = lzss_literal(make_model(random.Random(rng.getrandbits(32))))
    assert len(data) < 0x1000
    env.emu.rom[off:off + len(data)] = data
    return off


def sprite_rom(env, rng):
    return rng.choice(env.small_sprites)


def masked(rng, off):
    return (off - ASSET_BASE) | (rng.getrandbits(8) << 24)


# ---------------------------------------------------------------- object helpers (setup with originals)

def new_raw_obj(env, rng, m, typ=None):
    """Obj_Create + random contents; data points at a matching fake structure."""
    e = env.emu
    typ = rng.choice([0, 1, 2, 3, 4, 5]) if typ is None else typ
    g = rng.choice(env.groups)
    obj = env.o('Obj_Create', g, typ & 0xFFFFFFFF)
    if typ == 0:
        tb = m.alloc(rand_bytes(rng, 0x1508) + struct.pack('>I', rng.randrange(2)) + bytes(4))
        e.w32(obj + 0x5C, tb)
    elif typ in (1, 2):
        e.w32(obj + 0x5C, m.alloc(rand_bytes(rng, 0x30)))
    elif typ == 3:
        e.w32(obj + 0x5C, m.alloc(rand_bytes(rng, 0x2C)))
    else:
        e.w32(obj + 0x5C, rng.getrandbits(32))
    return obj


def rand_flags(rng, allow=0xFFDF):
    return rng.getrandbits(16) & allow


# ---------------------------------------------------------------- the test cases
# Each case: setup(env, rng, m) -> (args list, check_v0)

def case_font_new(env, rng, m, name):
    e = env.emu
    fid = rng.choice(list(range(32)) + [32, 40, 0x7FFFFFFF])
    if fid < 32:
        n = rng.choice([0, 1, 2, 3, 4])
        defs = b''.join(struct.pack('>I4h', masked(rng, sprite_rom(env, rng)),
                                    *[rng.randrange(-100, 100) for _ in range(4)]) for _ in range(n))
        e.w32(FONT_TABLE + 4 * fid, m.alloc(defs + bytes(12)))
    return [fid], True


def case_font_delete(env, rng, m, name):
    e = env.emu
    fid = rng.randrange(32)
    n = rng.choice([1, 2, 3])
    defs = b''.join(struct.pack('>I4h', masked(rng, sprite_rom(env, rng)), 1, 2, 3, 4) for _ in range(n))
    e.w32(FONT_TABLE + 4 * fid, m.alloc(defs + bytes(12)))
    f = env.o(rng.choice(['Font_New', 'Font_NewSharedPalette']), fid)
    return [f], True


def case_textbuf_new(env, rng, m, name):
    if rng.random() < 0.1:            # nearly full heap: allocation fails
        env.emu.call(0x80003358, a=[GAME_HEAP, GAME_HEAP_SIZE - 0x1000])
    return [], True


def case_textbuf_delete(env, rng, m, name):
    tb = env.o('TextBuf_New')
    return [tb], False


def make_tb(env, rng, m):
    if rng.random() < 0.5:
        tb = env.o('TextBuf_New')
        env.emu.w32(tb + 0x1508, rng.randrange(2))
        return tb
    return m.alloc(rand_bytes(rng, 0x1508) + struct.pack('>I', rng.randrange(2)) + bytes(4))


def case_text_build(env, rng, m, name):
    tlut = 'Tlut' in name
    font, n = fake_font(env, m, rng, fmt=rng.choice([0, 1, 2]) if tlut else None)
    tb = make_tb(env, rng, m)
    s = m.alloc(rand_str(rng, n))
    args = [tb, font, s, fb(rfloat(rng)), fb(rfloat(rng))]
    if 'Attach' in name:
        node = rng.choice([0, m.alloc(rand_bytes(rng, 0xE0))])
        args = [node] + args
    if tlut:
        args.append(rng.getrandbits(32))
    return args, True


def case_text_width(env, rng, m, name):
    font, n = fake_font(env, m, rng)
    return [font, m.alloc(rand_str(rng, n))], True


def case_alloclist_init(env, rng, m, name):
    return [m.alloc(rand_bytes(rng, 16)), m.alloc(rand_bytes(rng, 16))], False


def make_alloclist(env, rng, m):
    head, tail = m.alloc(rand_bytes(rng, 16)), m.alloc(rand_bytes(rng, 16))
    env.o('AllocList_Init', head, tail)
    for _ in range(rng.randrange(0, 6)):
        p = env.emu.call(0x80003358, a=[GAME_HEAP, rng.randrange(1, 200)])[0]
        env.o('AllocList_Add', GAME_HEAP, p, tail)
    return head, tail


def case_alloclist_add(env, rng, m, name):
    head, tail = make_alloclist(env, rng, m)
    p = env.emu.call(0x80003358, a=[GAME_HEAP, rng.randrange(1, 200)])[0]
    return [rng.choice([GAME_HEAP, p]), rng.choice([p, rng.getrandbits(32)]), tail], False


def case_alloclist_freeall(env, rng, m, name):
    head, tail = make_alloclist(env, rng, m)
    return [GAME_HEAP, head], False


def case_model_new(env, rng, m, name):
    off = put_model(env, rng)
    mp = rng.choice([0, m.alloc(rand_bytes(rng, 0x2C))])
    return [mp, off], True


def case_model_load(env, rng, m, name):
    off = put_model(env, rng)
    return [m.alloc(rand_bytes(rng, 0x2C)), off, rng.choice([0, 1, 2, 0x80000000])], True


def case_model_delete(env, rng, m, name):
    off = put_model(env, rng)
    mp = env.o(rng.choice(['Model_New', 'Model_NewUnlit']), rng.choice([0, m.alloc(bytes(0x2C))]), off)
    return [mp], True


def case_memcopy(env, rng, m, name):
    src = m.alloc(rand_bytes(rng, 0x200))
    dst = rng.choice([m.alloc(rand_bytes(rng, 0x200)), src + rng.randrange(-8, 9)])
    return [dst, src, rng.choice([0, 1, 2, 7, rng.randrange(0, 0x180)])], True


def case_ctz(env, rng, m, name):
    while True:
        x = rng.getrandbits(32)
        if rng.random() < 0.5:
            x = (x & 0xFFFF0000) | (1 << rng.randrange(16))
        if x & 0xFFFF:
            return [x], True


def case_ctor(env, rng, m, name):
    g = rng.choice(env.groups)
    e = env.emu
    if name in ('Obj_NewDefaultDL', 'Obj_NewView'):
        return [g], True
    if name == 'Obj_NewDL':
        return [g, rng.choice([0, rng.getrandbits(32)]), rng.getrandbits(32), rng.choice([0, rng.getrandbits(32)])], True
    if name == 'Obj_NewModel':
        return [g, put_model(env, rng)], True
    if name in ('Obj_NewSpritePlain', 'Obj_NewSpritePlain2', 'Obj_NewSprite', 'Obj_NewSprite2'):
        return [g, sprite_rom(env, rng)], True
    if name == 'Obj_NewScreen':
        return [g, SCREEN_SPRITE], True
    if name in ('Obj_NewSpritePlainRef', 'Obj_NewSpriteRef', 'Obj_NewModelRef', 'Obj_NewScreenRef'):
        return [g, rng.getrandbits(32)], True
    font, n = fake_font(env, m, rng, fmt=rng.choice([0, 1]))
    s = m.alloc(rand_str(rng, n))
    if name == 'Obj_NewText':
        return [g, font, s], True
    if name == 'Obj_NewTextTlut':
        return [g, font, s, rng.getrandbits(32)], True
    tb = make_tb(env, rng, m)
    if name == 'Obj_NewTextRef':
        return [g, font, tb, s], True
    return [g, font, tb, s, rng.getrandbits(32)], True       # Obj_NewTextRefTlut


def case_create(env, rng, m, name):
    return [rng.choice(env.groups), rng.choice([0, 1, 2, 3, 4, 5, 5, 6, 0xFFFF, 0x10005])], True


def case_settext(env, rng, m, name):
    font, n = fake_font(env, m, rng, fmt=rng.choice([0, 1]))
    tb = make_tb(env, rng, m)
    obj = env.o('Obj_NewTextRef', rng.choice(env.groups), font, tb, m.alloc(b'\0'))
    env.emu.w16(obj + 0x5A, rand_flags(rng))
    args = [obj, m.alloc(rand_str(rng, n))]
    if 'Tlut' in name:
        args.append(rng.getrandbits(32))
    return args, True


def case_objlist_init(env, rng, m, name):
    env.emu.write(0x800A6B30, rand_bytes(rng, 0x160))
    return [], False


def case_setter(env, rng, m, name):
    e = env.emu
    obj = new_raw_obj(env, rng, m)
    e.w16(obj + 0x5A, rng.getrandbits(16))
    if name == 'Obj_SetData':
        e.w16(obj + 0x58, rng.choice([0, 1, 2, 3, 4, 5, 6, 0xFFFF, 0x8000]))
        return [obj, rng.getrandbits(32)], True
    if name == 'Obj_SetColor':
        return [obj] + [rng.choice([rng.getrandbits(32), rng.randrange(256)]) for _ in range(4)], True
    if name == 'Obj_SetPos':
        return [obj] + [rng.choice([0, -1, rng.randrange(-1000, 1000), rng.getrandbits(32)]) for _ in range(3)], True
    if name == 'Obj_SetUpdate':
        return [obj, rng.getrandbits(32), rng.getrandbits(32)], True
    if name == 'Obj_SetMatrix':
        return [obj, m.alloc(b''.join(struct.pack('>f', rfloat(rng)) for _ in range(16)))], True
    if name == 'Obj_SetLights':
        return [obj, rng.getrandbits(32)], True
    return [obj], True


def fill_floats(e, rng, obj, big=500.0, view=False):
    for off in range(0x78, 0xA8, 4):
        v = rfloat(rng, big)
        if view:
            v = rng.uniform(-70000, 70000) if rng.random() < 0.5 else v
        e.wf(obj + off, v)


def case_obj_misc(env, rng, m, name):
    e = env.emu
    if name == 'Obj_FreeData':
        k = rng.randrange(7)
        g = rng.choice(env.groups)
        if k == 0:
            font, n = fake_font(env, m, rng)
            obj = env.o('Obj_NewText', g, font, m.alloc(b' \0'))
        elif k == 1:
            obj = env.o('Obj_NewSprite', g, sprite_rom(env, rng))
        elif k == 2:
            obj = env.o('Obj_NewScreen', g, SCREEN_SPRITE)
        elif k == 3:
            obj = env.o('Obj_NewModel', g, put_model(env, rng))
        elif k == 4:
            obj = env.o('Obj_NewDL', g, 0, 0, 0)
            e.w16(obj + 0x5A, e.r16(obj + 0x5A) | 0x20)
        else:
            obj = new_raw_obj(env, rng, m)
            e.w16(obj + 0x58, rng.choice([0, 1, 3, 6, 0xFFFF]))
            e.w16(obj + 0x5A, rand_flags(rng))
        if rng.random() < 0.2:
            e.w16(obj + 0x5A, e.r16(obj + 0x5A) & ~0x20)
        return [obj], False
    obj = new_raw_obj(env, rng, m, typ=rng.choice([0, 1, 2, 3, 4, 5, 6, 0xFFFF]) if name in ('Obj_Attach', 'Obj_Detach') else None)
    e.w16(obj + 0x5A, rng.getrandbits(16))
    if name in ('Obj_Transform3D', 'Obj_Transform2D'):
        fill_floats(e, rng, obj)
    elif name == 'Obj_TransformView':
        fill_floats(e, rng, obj, view=True)
    elif name == 'Obj_TransformScreen':
        spr = e.call(0x800129B8, a=[0, SCREEN_SPRITE])[0]     # original Sprite_NewScreen
        e.w32(obj + 0x5C, spr)
        fill_floats(e, rng, obj, big=300.0)
        for off in (0x9C, 0xA0):
            e.wf(obj + off, rng.choice([1.0, 0.5, 2.0, rng.uniform(0.1, 3), -1.0]))
    return [obj], False


def case_update_all(env, rng, m, name):
    """A random population of objects in every state; Obj_UpdateAll is called 1-3 times."""
    e = env.emu
    objs = []
    for _ in range(rng.randrange(0, 10)):
        k = rng.randrange(6)
        g = rng.choice(env.groups)
        if k == 0:
            obj = env.o('Obj_NewSprite', g, sprite_rom(env, rng))
        elif k == 1:
            obj = env.o('Obj_NewScreen', g, SCREEN_SPRITE)
        elif k == 2:
            font, n = fake_font(env, m, rng)
            obj = env.o('Obj_NewText', g, font, m.alloc(rand_str(rng, n, 8)))
        elif k == 3:
            obj = env.o('Obj_NewView', g)
        else:
            obj = new_raw_obj(env, rng, m)
            e.w32(obj + 0x74, rng.choice([0, STUB, 0x800193F0]))
        fill_floats(e, rng, obj, big=200.0)
        e.w32(obj + 0x64, rng.choice([0, 0, STUB]))
        f = e.r16(obj + 0x5A)
        f |= rng.getrandbits(16) & rng.choice([0x0003, 0x0013, 0x0004, 0x0008, 0x07C3, 0x0010, 0x0001, 0x0602])
        e.w16(obj + 0x5A, f)
        objs.append(obj)
    return [rng.randrange(1, 4)], False


def scene_tables(env, rng, m, allow_default=True):
    e = env.emu
    # assets
    aslots, assets = [], []
    for _ in range(rng.randrange(0, 5)):
        t = rng.choice([0, 2, 4, 9] + ([1, 3, 0x7FFF] if allow_default else []))
        slot = m.alloc(struct.pack('>I', 0xDEAD0000 | rng.getrandbits(12)))
        if t == 0:
            off = put_model(env, rng)
        elif t == 9:
            off = SCREEN_SPRITE
        else:
            off = sprite_rom(env, rng)
        assets.append(struct.pack('>IIhh', slot, masked(rng, off), t, rng.getrandbits(15)))
        aslots.append(slot)
    assets_p = m.alloc(b''.join(assets) + bytes(12))
    # objects
    descs = []
    font, n = fake_font(env, m, rng)
    for _ in range(rng.randrange(0, 7)):
        kinds = list(range(1, 14)) + ([0, 9, 14, 200] if allow_default else [])
        kind = rng.choice(kinds)
        if not allow_default and kind == 9:
            kind = 13
        g = rng.choice(env.groups)
        pslot = m.alloc(struct.pack('>I', g))
        slot = m.alloc(struct.pack('>I', 0))
        if kind in (0, 9, 14, 200):
            e.w32(slot, new_raw_obj(env, rng, m))
            arg = rng.getrandbits(32)
        elif kind in (2, 3, 4, 5):
            arg = masked(rng, sprite_rom(env, rng))
        elif kind == 8:
            arg = m.alloc(struct.pack('>I', font))
        elif kind == 12:
            arg = m.words([rng.choice([0, rng.getrandbits(32)]) for _ in range(3)])
        else:
            arg = m.alloc(struct.pack('>I', rng.getrandbits(32)))
        if kind == 13:
            sc = [rng.choice([1.0, rng.uniform(-3, 3), rng.uniform(-7, 7)]) for _ in range(3)]   # |v| > 4 wraps the s16
        else:
            sc = rng.choice([[1.0, 1.0, 1.0], [1.0, 1.0, 2.0], [rfloat(rng) for _ in range(3)], [1.0, -1.0, 1.0]])
        descs.append(struct.pack('>IIIH3h3fBBH', slot, pslot, arg, rng.getrandbits(16),
                                 *[rng.randrange(-32768, 32768) for _ in range(3)], *sc,
                                 rng.choice([0, 1, 0x80]), kind, 0))
    objs_p = m.alloc(b''.join(descs) + bytes(0x24))
    return objs_p, assets_p


def case_scene_build(env, rng, m, name):
    return list(scene_tables(env, rng, m)), False


def case_scene_destroy(env, rng, m, name):
    objs, assets = scene_tables(env, rng, m, allow_default=False)
    env.o('Scene_Build', objs, assets)
    return [objs, assets], False


CASES = {
    'Font_New': case_font_new, 'Font_NewSharedPalette': case_font_new, 'Font_Delete': case_font_delete,
    'TextBuf_New': case_textbuf_new, 'TextBuf_Delete': case_textbuf_delete,
    'Text_Build': case_text_build, 'Text_BuildTlut': case_text_build,
    'Text_Attach': case_text_build, 'Text_AttachTlut': case_text_build,
    'Font_TextWidth': case_text_width,
    'AllocList_Init': case_alloclist_init, 'AllocList_Add': case_alloclist_add,
    'AllocList_FreeAll': case_alloclist_freeall,
    'Model_New': case_model_new, 'Model_NewUnlit': case_model_new, 'Model_Delete': case_model_delete,
    'Model_Load': case_model_load, 'Mem_Copy': case_memcopy, 'Math_Ctz16': case_ctz,
    'Obj_Create': case_create, 'Obj_SetText': case_settext, 'Obj_SetTextTlut': case_settext,
    'ObjList_Init': case_objlist_init, 'Obj_UpdateAll': case_update_all,
    'Scene_Build': case_scene_build, 'Scene_Destroy': case_scene_destroy,
}
for n in ('Obj_NewDL', 'Obj_NewDefaultDL', 'Obj_NewView', 'Obj_NewModel', 'Obj_NewSpritePlain',
          'Obj_NewSpritePlain2', 'Obj_NewSpritePlainRef', 'Obj_NewSprite', 'Obj_NewSprite2',
          'Obj_NewSpriteRef', 'Obj_NewScreen', 'Obj_NewText', 'Obj_NewTextTlut', 'Obj_NewModelRef',
          'Obj_NewScreenRef', 'Obj_NewTextRef', 'Obj_NewTextRefTlut'):
    CASES[n] = case_ctor
for n in ('Obj_Kill', 'Obj_SetData', 'Obj_SetModelAlt2', 'Obj_SetModelAlt', 'Obj_SetColor', 'Obj_SetPos',
          'Obj_FlipX', 'Obj_FlipY', 'Obj_FlipZ', 'Obj_Invalidate', 'Obj_Show', 'Obj_Hide', 'Obj_SetUpdate',
          'Obj_ClearUpdate', 'Obj_ClearTransform', 'Obj_SetMatrix', 'Obj_SetLights',
          'Obj_UseSetupCC0', 'Obj_UseSetupC80'):
    CASES[n] = case_setter
for n in ('Obj_FreeData', 'Obj_FreeNode', 'Obj_Transform3D', 'Obj_Transform2D', 'Obj_TransformView',
          'Obj_TransformScreen', 'Obj_Attach', 'Obj_Detach'):
    CASES[n] = case_obj_misc


def run_one(env, name, addr, seed):
    env.restore()
    rng = random.Random(seed)
    m = Mem(env.emu)
    args, check_v0 = CASES[name](env, rng, m, name)
    if name == 'Obj_UpdateAll':
        for _ in range(args[0]):
            env.emu.call(addr)
        v0 = 0
    else:
        v0, _ = env.emu.call(addr, a=args)
    return (v0 if check_v0 else None), list(env.log), env.state()


def describe(a, b):
    if a[0] != b[0]:
        return f'v0 {a[0]:#x} vs {b[0]:#x}'
    if a[1] != b[1]:
        return f'callback log {a[1]} vs {b[1]}'
    for (base, n), x, y in zip(REGIONS, a[2], b[2]):
        if x != y:
            d = next(i for i in range(n) if x[i] != y[i])
            return f'memory differs at {base + d:#x}: orig {x[d & ~3:(d & ~3) + 16].hex()} C {y[d & ~3:(d & ~3) + 16].hex()}'
    return '?'


def main():
    env = Env()
    only = sys.argv[1:]
    fails = 0
    for name, addr in ORIG.items():
        if only and name not in only:
            continue
        n = int(os.environ.get("TRIALS", 300))
        ok = 0
        first = None
        for seed in range(n):
            a = run_one(env, name, addr, seed * 7919 + 1)
            b = run_one(env, name, env.syms[name], seed * 7919 + 1)
            if a == b:
                ok += 1
            elif first is None:
                first = f'seed {seed}: {describe(a, b)}'
        print(f'{name:24s} {"PASS" if ok == n else "FAIL"} {ok}/{n}' + (f'   {first}' if first else ''))
        sys.stdout.flush()
        fails += ok != n
    return 1 if fails else 0


if __name__ == '__main__':
    sys.exit(main())
