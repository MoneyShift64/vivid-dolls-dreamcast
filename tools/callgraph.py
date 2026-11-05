#!/usr/bin/env python3
"""Build a function table + call graph from the split assembly.

    python3 tools/callgraph.py            -> docs/functions.csv
    python3 tools/callgraph.py FUNC       -> print callers/callees of FUNC
"""
import csv, glob, re, sys, collections

FILES = {'asm/game.s': 'main', 'asm/3ED30.s': 'ovl0', 'asm/43AD0.s': 'ovl1',
         'asm/45270.s': 'ovl2', 'asm/602C0.s': 'ovl3', 'asm/libultra.s': 'libultra'}


def load():
    funcs = collections.OrderedDict()
    for path, seg in FILES.items():
        cur = None
        for line in open(path):
            m = re.match(r'glabel (\S+)', line)
            if m:
                cur = m.group(1)
                funcs[cur] = {'seg': seg, 'size': 0, 'calls': set(), 'vram': None,
                              'syms': set(), 'fp': False, 'jtbl': False}
                continue
            if cur is None:
                continue
            m = re.match(r'\s*/\* ([0-9A-F]+) ([0-9A-F]{8}) [0-9A-F]{8} \*/\s+(\S+)\s*(.*)', line)
            if not m:
                continue
            f = funcs[cur]
            if f['vram'] is None:
                f['vram'] = int(m.group(2), 16)
            f['size'] += 4
            op, args = m.group(3), m.group(4)
            if op == 'jal':
                f['calls'].add(args.strip())
            if op in ('jr',) and '$ra' not in args:
                f['jtbl'] = True
            if '$f' in args:
                f['fp'] = True
            for s in re.findall(r'%(?:hi|lo)\(([A-Za-z_][A-Za-z0-9_]*)\)', args):
                f['syms'].add(s)
    callers = collections.defaultdict(set)
    for n, f in funcs.items():
        for c in f['calls']:
            callers[c].add(n)
    return funcs, callers


def main():
    funcs, callers = load()
    if len(sys.argv) > 1:
        n = sys.argv[1]
        f = funcs[n]
        print(f"{n} {f['seg']} vram {f['vram']:08X} size {f['size']:#x}")
        print('  calls  :', ', '.join(sorted(f['calls'])))
        print('  callers:', ', '.join(sorted(callers[n])))
        print('  data   :', ', '.join(sorted(s for s in f['syms'] if not s.startswith('func'))))
        return
    with open('docs/functions.csv', 'w', newline='') as fh:
        w = csv.writer(fh)
        w.writerow(['name', 'segment', 'vram', 'size', 'callers', 'callees', 'leaf', 'float', 'calls'])
        for n, f in funcs.items():
            if f['seg'] == 'libultra':
                continue
            w.writerow([n, f['seg'], f"0x{f['vram']:08X}", f['size'], len(callers[n]), len(f['calls']),
                        not f['calls'], f['fp'], ' '.join(sorted(f['calls']))])
    print('wrote docs/functions.csv')


if __name__ == '__main__':
    main()
