#!/usr/bin/env python3
"""Differential test of src/game/jpeg64.c (the jpeg64 photo codec) against the
original MIPS code.

SYNTHETIC DATA ONLY: random coefficient blocks, random pixels, random bit
streams and tiny made-up images whose bitstreams are produced by the game's own
(unused) encoder from random coefficients. The ROM's photos are never touched,
and no decoded pixels are written anywhere outside emulator memory.
"""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env, reset_heap, GAME_HEAP, ROM_READ
from mipsbuild import CODE_BASE
from draft_mips import build_clean
from emu import RET_ADDR
from unicorn.mips_const import UC_MIPS_REG_PC

ORIG = {
    'Jpeg64_Decode': 0x80019D70, 'Jpeg64_Init': 0x80019FAC, 'Jpeg64_GetBitPos': 0x8001A118,
    'Jpeg64_RgbToYCbCr': 0x8001A124, 'Jpeg64_YCbCrToRgba5551': 0x8001A2D4,
    'Jpeg64_Downsample': 0x8001A4A0, 'Jpeg64_Upsample': 0x8001A530, 'Jpeg64_Fdct': 0x8001A5D8,
    'Jpeg64_Idct': 0x8001AA40, 'Jpeg64_Quantize': 0x8001AEA8, 'Jpeg64_Dequant': 0x8001AFB0,
    'Jpeg64_EncodeMacroblock': 0x8001B064, 'Jpeg64_DecodeMacroblock': 0x8001B250,
    'Jpeg64_EncodeDC': 0x8001B4D0, 'Jpeg64_EncodeAC': 0x8001B55C, 'Jpeg64_DecodeDC': 0x8001B5F4,
    'Jpeg64_DecodeAC': 0x8001B708, 'Jpeg64_ReverseBits': 0x8001B84C, 'Jpeg64_PutBits': 0x8001B898,
    'Jpeg64_GetBit': 0x8001B9A8, 'Jpeg64_GetBits': 0x8001B9F8,
}

BITPOS, BITBUF = 0x800A6C90, 0x800A6C94
COS = 0x800A6C98
BSS, BSS_SIZE = 0x800A6C90, 0x800A7FB0 - 0x800A6C90     # all codec state + buffers
PRED = 0x800A7FA0
A_IN, A_OUT, A_BUF = 0x80400000, 0x80402000, 0x80404000  # test memory
A_STR = 0x80410000                                         # synthetic bit streams
A_IMG = 0x80500000                                         # decoded test image (emulator only)
A_PTR = 0x80408000
HUFF = {'dcl': 0x80039B50, 'dcc': 0x80039BE0, 'acl': 0x80039C70, 'acc': 0x80039E20}
ENC = {'dcl': (0x800392D0, 0x800392F0), 'dcc': (0x80039310, 0x80039330),
       'acl': (0x80039350, 0x80039550), 'acc': (0x80039750, 0x80039950)}
FAKE_ROM = 0x7F000000          # synthetic "ROM" addresses served by our Rom_Read stub

fb = lambda v: struct.unpack('>I', struct.pack('>f', v))[0]
s16b = lambda vals: struct.pack('>%dh' % len(vals), *vals)


def huff_args(k):
    t = HUFF[k]
    return [t, t + 0x24, t + 0x48, t + 0x6C]


class Ctx:
    def __init__(self):
        blob, self.syms = build_clean(['src/game/jpeg64.c'], name='jpeg64')
        self.emu = make_env()
        self.emu.write(CODE_BASE, blob)
        self.fake = {}
        real = self.emu.stubs[ROM_READ]

        def rom_read(e):
            src, dst, size = e.arg(0), e.arg(1), e.arg(2)
            if src >= FAKE_ROM:
                d = self.fake[src]
                e.write(dst, (d + bytes(size))[:size])
                return 0
            return real(e)
        self.emu.stub(ROM_READ, rom_read)
        # cosine tables built once by the original (both versions then share them)
        self.emu.call(ORIG['Jpeg64_Init'], a=[A_BUF])
        self.cos = self.emu.read(COS, 0x200)

    def finished(self):
        return self.emu.mu.reg_read(UC_MIPS_REG_PC) == RET_ADDR

    def state(self, *regions):
        return tuple(self.emu.read(a, n) for a, n in regions)


# ---------------------------------------------------------------- generators --
def rand_coefs(rng, sparse=True, lim=1023):
    """6 random quantised blocks (natural order), values in encodable range."""
    out = []
    for b in range(6):
        blk = [0] * 64
        blk[0] = rng.randrange(-lim, lim + 1)
        mode = rng.random()
        for i in range(1, 64):
            if mode < 0.15:
                p = 0.0
            elif mode < 0.3:
                p = 1.0
            else:
                p = rng.choice([0.05, 0.2, 0.5])
            if rng.random() < p:
                blk[i] = rng.choice([1, -1, 2, -2, rng.randrange(-lim, lim + 1)])
        if rng.random() < 0.2:
            blk[63] = rng.choice([1, -5, 300])
        out += blk
    return out


def encode_stream(ctx, rng, n_mb, lim=1023):
    """Encodes n_mb random macroblocks with the ORIGINAL encoder; returns bytes."""
    e = ctx.emu
    e.write(A_STR, bytes(0x4000))
    e.w32(BITBUF, A_STR); e.w32(BITPOS, 0)
    e.write(PRED, bytes(12))
    for _ in range(n_mb):
        # DC values must stay within +-1023 of each other so diffs fit category 11
        e.write(A_IN, s16b(rand_coefs(rng, lim=lim)))
        e.call(ORIG['Jpeg64_EncodeMacroblock'], a=[A_IN, PRED])
    nbytes = (e.r32(BITPOS) + 7) // 8
    return e.read(A_STR, nbytes + 4)


def fresh_bss(ctx, rng):
    e = ctx.emu
    e.write(BSS, bytes(rng.getrandbits(8) for _ in range(BSS_SIZE)))
    e.write(COS, ctx.cos)


# --------------------------------------------------------------------- tests --
def t_getbit(ctx, f, rng, name):
    e = ctx.emu
    e.write(A_BUF, bytes(rng.getrandbits(8) for _ in range(0x200)))
    e.w32(BITBUF, A_BUF + 0x40)
    e.w32(BITPOS, rng.choice([0, 1, 7, 8, rng.randrange(-64, 0), rng.randrange(0, 3000)]))
    if name == 'Jpeg64_GetBit':
        a = []
    else:
        a = [rng.choice([0, 1, 2, 8, 15, 16, 17, 24, rng.randrange(0, 20), 0x10003, 0x10000])]
    v0, _ = e.call(f, a=a)
    return v0, e.r32(BITPOS), e.r32(BITBUF)


def t_bitpos(ctx, f, rng, name):
    ctx.emu.w32(BITPOS, rng.getrandbits(32))
    return ctx.emu.call(f)[0]


def t_reverse(ctx, f, rng, name):
    v = rng.getrandbits(32)
    n = rng.choice([0, 1, 2, 5, 16, 17, 31, rng.randrange(0, 20), 0x10002, rng.getrandbits(32)])
    return ctx.emu.call(f, a=[v, n])[0]


def t_putbits(ctx, f, rng, name):
    e = ctx.emu
    e.write(A_BUF, bytes(rng.getrandbits(8) for _ in range(0x200)))
    e.w32(BITBUF, A_BUF + 0x40)
    e.w32(BITPOS, rng.choice([0, 3, rng.randrange(-60, 0), rng.randrange(0, 3000)]))
    v = rng.getrandbits(32)
    n = rng.choice([0, 1, 2, 3, 16, rng.randrange(0, 17)])
    e.call(f, a=[v, n])
    return e.read(A_BUF, 0x200), e.r32(BITPOS)


def t_encdc(ctx, f, rng, name):
    e = ctx.emu
    e.write(A_BUF, bytes(rng.getrandbits(8) for _ in range(0x200)))
    e.w32(BITBUF, A_BUF + 0x40)
    e.w32(BITPOS, rng.randrange(0, 2000))
    k = rng.choice(['dcl', 'dcc'] if name == 'Jpeg64_EncodeDC' else ['acl', 'acc'])
    codes, lens = ENC[k]
    if name == 'Jpeg64_EncodeDC':
        x = rng.choice([0, 1, -1, 2047, -2047, rng.randrange(-2047, 2048)])
        e.call(f, a=[x & 0xFFFFFFFF, codes, lens])
    else:
        x = rng.choice([0, 1, -1, 1023, -1023, rng.randrange(-1023, 1024)])
        run = rng.choice([0, 15, rng.randrange(0, 16)])
        if x == 0 and run not in (0, 15):
            run = 0
        e.call(f, a=[x & 0xFFFFFFFF, run, codes, lens])
    return e.read(A_BUF, 0x200), e.r32(BITPOS)


def t_decsym(ctx, f, rng, name):
    """DC/AC symbol decode on encoder-made streams and on random bytes."""
    e = ctx.emu
    dc = name == 'Jpeg64_DecodeDC'
    k = rng.choice(['dcl', 'dcc'] if dc else ['acl', 'acc'])
    if rng.random() < 0.6:
        # valid stream: a few symbols written by the original encoder
        e.write(A_BUF, bytes(0x200))
        e.w32(BITBUF, A_BUF); e.w32(BITPOS, 0)
        codes, lens = ENC[k]
        for _ in range(4):
            if dc:
                e.call(ORIG['Jpeg64_EncodeDC'], a=[rng.randrange(-2047, 2048) & 0xFFFFFFFF, codes, lens])
            else:
                x = rng.randrange(-1023, 1024)
                e.call(ORIG['Jpeg64_EncodeAC'], a=[x & 0xFFFFFFFF, rng.randrange(16) if x else 0, codes, lens])
        start = 0
    else:
        e.write(A_BUF, bytes(rng.getrandbits(8) for _ in range(0x200)))
        start = rng.randrange(0, 1000)
    e.w32(BITBUF, A_BUF); e.w32(BITPOS, start)
    if dc:
        v0, _ = e.call(f, a=huff_args(k), max_insns=2_000_000)
        res = (v0,)
    else:
        e.write(A_PTR, b'\xAA' * 8)
        v0, _ = e.call(f, a=[A_PTR, A_PTR + 2] + huff_args(k)[:2], stack=huff_args(k)[2:], max_insns=2_000_000)
        res = (e.read(A_PTR, 8),)
    return res + (e.r32(BITPOS), ctx.finished())


def t_mbdec(ctx, f, rng, name):
    e = ctx.emu
    s = encode_stream(ctx, rng, rng.randrange(1, 4))
    fresh_bss(ctx, rng)
    e.write(A_STR, s)
    e.w32(BITBUF, A_STR); e.w32(BITPOS, 0)
    pred = [rng.randrange(-300, 300) for _ in range(6)]
    e.write(A_OUT, bytes(rng.getrandbits(8) for _ in range(0x300)))
    e.write(A_IN, s16b(pred))
    out = []
    for _ in range(rng.randrange(1, 4)):
        e.call(f, a=[A_OUT, A_IN])
        out.append(e.read(A_OUT, 0x300) + e.read(A_IN, 12) + struct.pack('>I', e.r32(BITPOS)))
    return out


def t_mbenc(ctx, f, rng, name):
    e = ctx.emu
    e.write(A_STR, bytes(rng.getrandbits(8) for _ in range(0x2000)))
    e.w32(BITBUF, A_STR); e.w32(BITPOS, rng.randrange(0, 64))
    e.write(A_OUT, s16b([rng.randrange(-1000, 1000) for _ in range(6)]))
    for _ in range(rng.randrange(1, 3)):
        e.write(A_IN, s16b(rand_coefs(rng, lim=500)))
        e.call(f, a=[A_IN, A_OUT])
    return e.read(A_STR, 0x2000), e.read(A_OUT, 12), e.r32(BITPOS)


def t_quant(ctx, f, rng, name):
    e = ctx.emu
    mode = rng.random()
    if mode < 0.2:
        vals = [rng.randrange(-32768, 32768) for _ in range(384)]
    else:
        vals = [rng.choice([0, 0, 0, 1, -1, rng.randrange(-200, 200), rng.randrange(-2048, 2048)]) for _ in range(384)]
    e.write(A_IN, s16b(vals))
    e.write(A_OUT, bytes(rng.getrandbits(8) for _ in range(0x300)))
    sc = rng.choice([0.5, 1.0, 2.0, 1.0 / 3, rng.uniform(0.01, 4), -1.0])
    e.call(f, a=[A_OUT, A_IN, fb(sc)])
    return e.read(A_OUT, 0x300)


def t_dct(ctx, f, rng, name):
    e = ctx.emu
    fresh_bss(ctx, rng)
    if rng.random() < 0.15:            # arbitrary tables too (both read the same memory)
        e.write(COS, b''.join(struct.pack('>f', rng.uniform(-1, 1)) for _ in range(128)))
    mode = rng.random()
    if mode < 0.2:
        vals = [rng.randrange(-32768, 32768) for _ in range(384)]
    elif mode < 0.6:
        vals = [rng.randrange(-1024, 1024) for _ in range(384)]
    else:
        vals = [rng.choice([0, 0, 0, 0, 1, -1, rng.randrange(-300, 300)]) for _ in range(384)]
    e.write(A_IN, s16b(vals))
    e.write(A_OUT, bytes(rng.getrandbits(8) for _ in range(0x300)))
    alias = rng.random() < 0.1
    e.call(f, a=[A_IN if alias else A_OUT, A_IN])
    return e.read(A_OUT, 0x300), e.read(A_IN, 0x300)


def t_updown(ctx, f, rng, name):
    e = ctx.emu
    e.write(A_IN, bytes(rng.getrandbits(8) for _ in range(0x600)))
    e.write(A_OUT, bytes(rng.getrandbits(8) for _ in range(0x600)))
    e.call(f, a=[A_OUT, A_IN])
    return e.read(A_OUT, 0x600)


def t_color(ctx, f, rng, name):
    e = ctx.emu
    if name == 'Jpeg64_RgbToYCbCr':
        e.write(A_IN, bytes(rng.getrandbits(8) for _ in range(768)))
    else:
        mode = rng.random()
        if mode < 0.3:
            vals = [rng.randrange(-32768, 32768) for _ in range(768)]
        else:
            vals = []
            for _ in range(256):
                vals += [rng.randrange(-40, 300), rng.randrange(-160, 160), rng.randrange(-160, 160)]
        e.write(A_IN, s16b(vals))
    e.write(A_OUT, bytes(rng.getrandbits(8) for _ in range(0x600)))
    e.call(f, a=[A_OUT, A_IN])
    return e.read(A_OUT, 0x600)


def t_init(ctx, f, rng, name):
    e = ctx.emu
    fresh_bss(ctx, rng)
    p = rng.getrandbits(32)
    e.call(f, a=[p])
    return e.read(BSS, 0x208)


def t_decode(ctx, f, rng, name):
    """Whole-photo decode of a tiny made-up image (random coefficients encoded by
    the game's encoder), served from a fake ROM address."""
    e = ctx.emu
    w = 16 * rng.randrange(1, 5)
    h = 16 * rng.randrange(1, 4)
    nmb = (w // 16) * (h // 16)
    lim = rng.choice([40, 200, 1000])
    s = encode_stream(ctx, rng, nmb, lim=lim)
    size = 0x58 + len(s)
    hdr = bytearray(0x58)
    struct.pack_into('>4sI', hdr, 0, b'Siff', size - 8)
    struct.pack_into('>I', hdr, 0x34, w)
    struct.pack_into('>I', hdr, 0x40, h)
    struct.pack_into('>I', hdr, 0x4C, rng.choice([1, 2, 2, 3, 0x10002]))
    addr = FAKE_ROM + 0x1000 * (rng.getrandbits(12))
    ctx.fake[addr] = bytes(hdr) + s
    fresh_bss(ctx, rng)
    reset_heap(e)
    stride_w = w + rng.choice([0, 0, 16])
    e.write(A_IMG, bytes(0x10000))
    x, y = rng.randrange(0, stride_w - w + 1), rng.randrange(0, 3)
    e.w32(A_PTR, 0xDEAD); e.w32(A_PTR + 4, 0xBEEF)
    e.call(f, a=[addr, A_IMG, x, y], stack=[A_PTR, A_PTR + 4])
    return (e.read(A_IMG, 0x10000), e.read(A_PTR, 8), e.read(BSS, BSS_SIZE),
            e.read(GAME_HEAP, 0x8000))


TESTS = [
    ('Jpeg64_GetBit', t_getbit, 400), ('Jpeg64_GetBits', t_getbit, 400),
    ('Jpeg64_GetBitPos', t_bitpos, 300), ('Jpeg64_ReverseBits', t_reverse, 400),
    ('Jpeg64_PutBits', t_putbits, 400), ('Jpeg64_EncodeDC', t_encdc, 400),
    ('Jpeg64_EncodeAC', t_encdc, 400), ('Jpeg64_DecodeDC', t_decsym, 400),
    ('Jpeg64_DecodeAC', t_decsym, 400), ('Jpeg64_DecodeMacroblock', t_mbdec, 300),
    ('Jpeg64_EncodeMacroblock', t_mbenc, 300), ('Jpeg64_Dequant', t_quant, 300),
    ('Jpeg64_Quantize', t_quant, 300), ('Jpeg64_Idct', t_dct, 300), ('Jpeg64_Fdct', t_dct, 300),
    ('Jpeg64_Upsample', t_updown, 300), ('Jpeg64_Downsample', t_updown, 300),
    ('Jpeg64_YCbCrToRgba5551', t_color, 300), ('Jpeg64_RgbToYCbCr', t_color, 300),
    ('Jpeg64_Init', t_init, 300), ('Jpeg64_Decode', t_decode, 300),
]


def main(argv):
    only = set(argv)
    ctx = Ctx()
    bad_total = 0
    for name, fn, n in TESTS:
        if only and name not in only:
            continue
        bad = 0
        for s in range(n):
            a = fn(ctx, ORIG[name], random.Random(s), name)
            b = fn(ctx, ctx.syms[name], random.Random(s), name)
            if a != b:
                if bad == 0:
                    print(f'  {name}: first mismatch seed {s}')
                bad += 1
        print(f'{name:26s} {"PASS" if not bad else "FAIL"} {n - bad}/{n}')
        bad_total += bad
    return 1 if bad_total else 0


if __name__ == '__main__':
    sys.exit(main(sys.argv[1:]))
