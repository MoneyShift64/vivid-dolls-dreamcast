#!/usr/bin/env python3
"""Copy the names of decompiled functions (the "/* @ 0xADDR */" tag above each
function in src/game/*.c) into symbol_addrs.txt, so the disassembly uses them.

    python3 tools/sync_symbols.py
"""
import glob, os, re

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..'))
TAG = re.compile(r'/\* @ 0x([0-9A-Fa-f]{8})[^\n]*?(?:\*/)?\n(?:(?: \*[^\n]*|/\*[^\n]*)\n)*?'
                 r'(?:static\s+)?[A-Za-z_][\w \*]*?\b([A-Za-z_]\w*)\s*\(', re.S)

names = {}      # main-segment functions
ovl_names = {}  # (overlay, addr) -> name; overlay code lives in src/game/ovlN_*.c
for f in sorted(glob.glob(os.path.join(ROOT, 'src', 'game', '*.c'))):
    seg = re.match(r'(ovl\d)_', os.path.basename(f))
    for m in TAG.finditer(open(f).read()):
        if seg:
            ovl_names[(seg.group(1), int(m.group(1), 16))] = m.group(2)
        else:
            names[int(m.group(1), 16)] = m.group(2)

path = os.path.join(ROOT, 'symbol_addrs.txt')
lines = open(path).read().splitlines()
by_addr = {}
for i, l in enumerate(lines):
    m = re.match(r'(\w+) = 0x([0-9A-Fa-f]+);', l)
    if m:
        by_addr.setdefault(int(m.group(2), 16), []).append(i)
changed = added = 0
for a, n in sorted(names.items()):
    if a in by_addr:
        for i in by_addr[a]:
            old = re.match(r'(\w+) =', lines[i]).group(1)
            if old != n and 'type:func' in lines[i]:
                lines[i] = lines[i].replace(old + ' =', n + ' =', 1); changed += 1
    else:
        lines.append(f'{n} = 0x{a:08X}; // type:func'); added += 1
have = {re.match(r'(\w+) =', l).group(1) for l in lines if re.match(r'\w+ =', l)}
for (seg, a), n in sorted(ovl_names.items()):
    if n not in have:
        lines.append(f'{n} = 0x{a:08X}; // type:func segment:{seg}'); added += 1
open(path, 'w').write('\n'.join(lines) + '\n')
print(f'{len(names) + len(ovl_names)} tagged functions: {added} added, {changed} renamed')
