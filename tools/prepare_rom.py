#!/usr/bin/env python3
"""Turn the Vivid Dolls chip dump into baserom.z64.

    python3 tools/prepare_rom.py path/to/nus-zsaj-0.u3

The MAME-style chip dump stores each pair of bytes swapped (".v64" order).
This writes a correctly ordered copy as baserom.z64 and checks its SHA1.
Your original file is not changed.
"""
import hashlib, sys

EXPECTED = '747e9420cce23ff82b5922cd91ec43272d03fa4f'

if len(sys.argv) != 2:
    print(__doc__); sys.exit(1)
data = bytearray(open(sys.argv[1], 'rb').read())
if data[:4] == b'\x37\x80\x40\x12':          # byte-swapped (.v64 / chip dump)
    data[0::2], data[1::2] = data[1::2], data[0::2]
elif data[:4] != b'\x80\x37\x12\x40':
    print('This does not look like an N64/Aleck64 ROM.'); sys.exit(1)
open('baserom.z64', 'wb').write(data)
sha = hashlib.sha1(data).hexdigest()
if sha == EXPECTED:
    print('baserom.z64 written - SHA1 OK (Vivid Dolls, nus-zsaj-0.u3)')
else:
    print(f'baserom.z64 written, but SHA1 is {sha}\n(expected {EXPECTED} - different dump?)')
    sys.exit(1)
