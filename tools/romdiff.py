#!/usr/bin/env python3
"""Show where build/vividdolls.z64 differs from baserom.z64."""
import sys
a = open('baserom.z64', 'rb').read(); b = open('build/vividdolls.z64', 'rb').read()
print(f'sizes: base {len(a):#x}  build {len(b):#x}')
d = [i for i in range(min(len(a), len(b))) if a[i] != b[i]]
print(f'{len(d)} differing bytes')
if d:
    runs = []; s = p = d[0]
    for x in d[1:]:
        if x > p + 16: runs.append((s, p)); s = x
        p = x
    runs.append((s, p))
    for s, e in runs[:int(sys.argv[1]) if len(sys.argv) > 1 else 10]:
        print(f'{s:#08x}-{e:#08x}  base {a[s:s+8].hex()}  build {b[s:s+8].hex()}')
