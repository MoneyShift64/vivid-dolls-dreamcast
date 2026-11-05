#!/usr/bin/env python3
"""Decompilation progress: functions in src/game (tagged "@ 0x...") vs all game code.
Overlay functions live in files named after their overlay (src/game/ovl2_*.c etc.)."""
import glob, os, re

SEGS = {'asm/game.s': 'main game code', 'asm/3ED30.s': 'ovl0 test mode', 'asm/43AD0.s': 'ovl1 attract',
        'asm/45270.s': 'ovl2 game', 'asm/602C0.s': 'ovl3'}

SEG_OF = {'asm/game.s': 'main', 'asm/3ED30.s': 'ovl0', 'asm/43AD0.s': 'ovl1',
          'asm/45270.s': 'ovl2', 'asm/602C0.s': 'ovl3'}
done = set()
for f in glob.glob('src/game/*.c'):
    m = re.match(r'(ovl\d)_', os.path.basename(f))
    seg = m.group(1) if m else 'main'
    for m in re.finditer(r'/\* @ 0x([0-9A-Fa-f]{8})', open(f).read()):
        done.add((seg, int(m.group(1), 16)))
tot_f = tot_b = got_f = got_b = 0
print(f'{"segment":18s} {"functions":>14s} {"bytes":>18s}')
for path, name in SEGS.items():
    funcs = []
    cur = None
    for line in open(path):
        m = re.match(r'glabel (\S+)', line)
        if m:
            cur = [None, 0]; funcs.append(cur); continue
        m = re.match(r'\s*/\* [0-9A-F]+ ([0-9A-F]{8}) ', line)
        if m and cur is not None:
            if cur[0] is None: cur[0] = int(m.group(1), 16)
            cur[1] += 4
    f_done = [f for f in funcs if (SEG_OF[path], f[0]) in done]
    nf, nb = len(funcs), sum(f[1] for f in funcs)
    df, db = len(f_done), sum(f[1] for f in f_done)
    tot_f += nf; tot_b += nb; got_f += df; got_b += db
    print(f'{name:18s} {df:5d} / {nf:<6d} {db:8d} / {nb:<8d} ({100 * db / max(nb, 1):5.1f}%)')
print(f'{"TOTAL":18s} {got_f:5d} / {tot_f:<6d} {got_b:8d} / {tot_b:<8d} ({100 * got_b / tot_b:5.1f}%)')
