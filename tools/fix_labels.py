#!/usr/bin/env python3
"""Post-process splat output so jump-table labels link across files.

splat names branch / jump-table targets ".L<addr>", which the assembler keeps
private to one file. The jump tables live in the separate .rodata files, so we
rename the labels to "L<segment>_<addr>" and export the jump-table targets.
All four overlays share RAM 0x800E0000, hence the per-segment prefix.
"""
import os, re

SEGMENTS = {  # file stem -> segment name
    'entry': 'main', 'game': 'main', 'libultra': 'main', '38D50.data': 'main', '3E150.rodata': 'main', '3ED30.bss': 'main',
    '3ED30': 'ovl0', '437E0.rodata': 'ovl0',
    '43AD0': 'ovl1', '44A90.data': 'ovl1', '45270.bss': 'ovl1',
    '45270': 'ovl2', '5B600.data': 'ovl2', '601B0.rodata': 'ovl2', '602C0.bss': 'ovl2',
    '602C0': 'ovl3', '60E00.data': 'ovl3', '611F0.bss': 'ovl3',
}
for root, _, files in os.walk('asm'):
    for f in files:
        stem = f[:-2]
        if not f.endswith('.s') or stem not in SEGMENTS:
            continue
        p = os.path.join(root, f)
        s = open(p).read()
        seg = SEGMENTS[stem]
        s2 = re.sub(r'\.L([0-9A-F]{8})', lambda m: f'L{seg}_{m.group(1)}', s)
        if seg.startswith('ovl'):
            # overlay-local symbols (RAM 0x800E0000-0x800FFFFF) get the overlay name
            s2 = re.sub(r'\b((?:func|D|jtbl)_800[EF][0-9A-F]{4})\b', seg + r'_\1', s2)
        if s2 != s:
            open(p, 'w').write(s2)
print('labels fixed')
