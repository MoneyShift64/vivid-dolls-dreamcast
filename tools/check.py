#!/usr/bin/env python3
"""Compare a built ROM's SHA1 against the expected one (works on Mac and Linux)."""
import hashlib, sys

rom, sha_file = sys.argv[1], sys.argv[2]
expected = open(sha_file).read().split()[0].lower()
actual = hashlib.sha1(open(rom, 'rb').read()).hexdigest()
if actual == expected:
    print(f'{rom}: OK - matches the original ROM')
else:
    print(f'{rom}: DOES NOT MATCH\n  expected {expected}\n  got      {actual}')
    print('  run "python3 tools/romdiff.py" to see where it differs')
    sys.exit(1)
