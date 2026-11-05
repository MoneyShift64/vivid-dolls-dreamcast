#!/usr/bin/env python3
"""Differential test for the Siff writer helpers in src/game/siff.c."""
import os, random, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env
from mipsbuild import build, CODE_BASE, ROOT

ORIG = {'SiffIter_Init': 0x80009EC0, 'SiffWriter_Begin': 0x80009EE4, 'SiffWriter_End': 0x80009F10,
        'Siff_WriteString': 0x80009FD8, 'Siff_WriteU32': 0x8000A03C, 'Siff_WriteData': 0x8000A054}
IT, OUT, SRC = 0x80600000, 0x80610000, 0x80620000


def session(emu, fns, rng_seed):
    rng = random.Random(rng_seed)
    emu.write(IT, bytes([0xAA]) * 0x200); emu.write(OUT, bytes([0x55]) * 0x4000)
    emu.write(SRC, bytes(rng.randrange(256) for _ in range(0x400)))
    rets = [emu.call(fns['SiffIter_Init'], a=[IT])[0] * 0]
    cur = OUT
    stack = []
    for _ in range(rng.randrange(3, 25)):
        r = rng.random()
        if r < 0.25 and len(stack) < 10:
            cur, _ = emu.call(fns['SiffWriter_Begin'], a=[IT, cur, rng.randrange(1 << 32)]); stack.append(1)
        elif r < 0.45 and stack:
            cur = cur + rng.randrange(0, 7)      # leave an unaligned tail to be padded
            cur, _ = emu.call(fns['SiffWriter_End'], a=[IT, cur]); stack.pop()
        elif r < 0.65:
            s = bytes(rng.randrange(1, 127) for _ in range(rng.randrange(0, 20))) + b'\0'
            emu.write(SRC, s)
            cur, _ = emu.call(fns['Siff_WriteString'], a=[cur & ~3, rng.randrange(1 << 32), SRC])
        elif r < 0.8:
            cur, _ = emu.call(fns['Siff_WriteU32'], a=[cur & ~3, rng.randrange(1 << 32), rng.randrange(1 << 32)])
        else:
            cur, _ = emu.call(fns['Siff_WriteData'], a=[cur & ~3, rng.randrange(1 << 32), SRC + 0x100, rng.randrange(1, 200)])
        rets.append(cur)
    return rets, emu.read(IT, 0x104), emu.read(OUT, 0x4000)


def main():
    blob, syms = build([os.path.join(ROOT, f'src/game/{f}.c') for f in ('heap', 'siff')], name='siffw')
    emu = make_env(); emu.write(CODE_BASE, blob)
    bad = sum(session(emu, ORIG, s) != session(emu, syms, s) for s in range(300))
    print(f'Siff writer sessions: {300 - bad}/300 identical')
    return 1 if bad else 0


if __name__ == '__main__':
    sys.exit(main())
