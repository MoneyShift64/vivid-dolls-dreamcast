#!/usr/bin/env python3
"""Differential test of src/game/strutil.c (keyword matchers, CRC16, sprintf,
text sanitisers and hex dumps) against the original MIPS code."""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env
from mipsbuild import CODE_BASE
from draft_mips import build_clean

STR = 0x80600000      # input text
LIST = 0x80601000     # keyword list / format string
OUT = 0x80602000      # output buffer
PP = 0x80603000       # pointer slot / int slot
ARGS = 0x80603100     # vararg words
SARG = 0x80604000     # string arguments for %s

WORDS = [b'begin', b'end', b'Image', b'IMAGE', b'sprite', b'x', b'_tmp', b'A1', b'a1', b'go', b'goto']


def kwlist(rng):
    ws = rng.sample(WORDS, rng.randrange(0, 6))
    if rng.random() < 0.2:
        ws.insert(rng.randrange(len(ws) + 1), b'')   # empty keyword ends the list early
    return b''.join(w + b'\0' for w in ws) + b'\0'


def text(rng):
    parts = []
    for _ in range(rng.randrange(1, 4)):
        parts.append(rng.choice(WORDS + [b' ', b'(', b'beginx', b'endZ', b'1', b'\xa4\xa2', b'?']))
    return b'\0' + b''.join(parts) + b'\0' * 4   # leading NUL so s[-1] is defined


def fmt(rng):
    out, args, sargs = b'', [], []
    for _ in range(rng.randrange(1, 6)):
        r = rng.random()
        if r < 0.25:
            lit = rng.choice([b'abc', b' ', b'\\%', b'\\n', b'%%', b'%q', b'%!d'])
            out += lit
            if lit == b'%!d':            # '!' counts as a width digit; consumes an int
                args.append(rng.getrandbits(32))
            continue
        flags = rng.choice([b'', b'', b'0', b'-', b'-0', b'00'])
        width = rng.choice([b'', b'', b'3', b'8', b'12', b'40'])
        conv = rng.choice(b'sxXdbcd')
        out += b'%' + flags + width + bytes([conv])
        if conv == ord('s'):
            sargs.append(rng.choice([b'', b'hi', b'Vivid Dolls', b'x' * 20]))
            args.append(None)
        else:
            args.append(rng.choice([0, 1, 9, 10, 255, 0x7FFFFFFF, 0x80000000, 0xFFFFFFFF,
                                    rng.getrandbits(32), rng.randrange(-1000, 1000) & 0xFFFFFFFF]))
    return out + b'\0', args, sargs


def run(emu, addr, kind, seed):
    rng = random.Random(seed)
    emu.write(OUT, b'\xcc' * 0x400)
    emu.write(LIST, b'\0' * 0x100)
    if kind in ('tbl', 'lower', 'upper', 'exact'):
        t = text(rng); emu.write(STR, t); emu.write(LIST, kwlist(rng))
        emu.write(PP, struct.pack('>I', STR + 1))
        a = [PP, LIST] + ([0x80038150 if rng.random() < .5 else 0x80038250] if kind == 'tbl' else [])
    elif kind in ('lowerat', 'upperat', 'exactat'):
        t = text(rng); emu.write(STR, t); emu.write(LIST, kwlist(rng))
        a = [PP, STR + 1, LIST]
    elif kind == 'crc':
        emu.write(STR, bytes(rng.getrandbits(8) for _ in range(256)))
        a = [STR, rng.randrange(1, 200)]
    elif kind == 'vsprintf':
        f, args, sargs = fmt(rng)
        emu.write(LIST, f)
        words, si, sp = [], 0, SARG
        for v in args:
            if v is None:
                emu.write(sp, sargs[si] + b'\0'); words.append(sp); sp += 0x40; si += 1
            else:
                words.append(v)
        emu.write(ARGS, b''.join(struct.pack('>I', w) for w in words) + b'\0' * 16)
        a = [OUT, LIST, ARGS]
    elif kind in ('euc', 'ascii', 'hexb', 'hexg', 'hexw'):
        n = rng.randrange(0, 48)
        pool = [0x41, 0x20, 0x7f, 0x0a, 0xa4, 0xa2, 0xb0, 0xff, 0x80, 0x9f, 0x7e]
        emu.write(STR, bytes(rng.choice(pool) if rng.random() < .7 else rng.getrandbits(8) for _ in range(64)))
        a = [OUT, STR, n] + ([rng.randrange(0, 5)] if kind == 'hexg' else [])
    v0, _ = emu.call(addr, a=a, f=[])
    return v0, emu.read(OUT, 0x400), emu.read(PP, 8)


TESTS = [
    ('Kw_MatchTable', 0x800007A0, 'tbl'), ('Kw_MatchLower', 0x80000858, 'lower'),
    ('Kw_MatchUpper', 0x80000918, 'upper'), ('Kw_MatchExact', 0x800009D8, 'exact'),
    ('Kw_MatchLowerAt', 0x80000A40, 'lowerat'), ('Kw_MatchUpperAt', 0x80000B04, 'upperat'),
    ('Kw_MatchExactAt', 0x80000BC8, 'exactat'), ('Crc16_Ccitt', 0x80000C40, 'crc'),
    ('vd_vsprintf', 0x80000CB0, 'vsprintf'), ('Str_SanitizeEuc', 0x80005D40, 'euc'),
    ('Str_SanitizeAscii', 0x80005EC0, 'ascii'), ('Str_HexDumpBytes', 0x80005EF8, 'hexb'),
    ('Str_HexDumpGrouped', 0x80005DF4, 'hexg'), ('Str_HexDumpWords', 0x80005F50, 'hexw'),
]


def main(trials=400):
    blob, syms = build_clean(['src/game/strutil.c'], name='strutil')
    emu = make_env(); emu.write(CODE_BASE, blob)
    bad_total = 0
    for name, addr, kind in TESTS:
        bad = 0
        for s in range(trials):
            a = run(emu, addr, kind, s); b = run(emu, syms[name], kind, s)
            if kind == 'ascii':          # void function: v0 is garbage
                a, b = a[1:], b[1:]
            if a != b:
                if bad == 0:
                    print('  first mismatch seed', s, a[0], b[0], a[1][:80], b[1][:80])
                bad += 1
        print(f'{name:22s} {"PASS" if not bad else "FAIL"} {trials - bad}/{trials}')
        bad_total += bad
    return 1 if bad_total else 0


if __name__ == '__main__':
    sys.exit(main())
