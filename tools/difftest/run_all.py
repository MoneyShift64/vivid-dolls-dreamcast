#!/usr/bin/env python3
"""Run every differential test (original MIPS code vs the C in src/game).

    python3 tools/difftest/run_all.py

Needs: the Unicorn emulator (Python package), clang with the MIPS target,
MIPS binutils, and baserom.z64. See docs/DIFFTEST.md.
"""
import os, subprocess, sys, time

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, '..', '..'))
TESTS = ['test_funcs.py', 'test_heap.py', 'test_siff_writer.py', 'test_sprite.py',
         'test_screen_sprite.py', 'test_photoview.py', 'test_strutil.py']
# every other test_*.py in this folder (area tests)
import glob as _g
TESTS += sorted(os.path.basename(t) for t in _g.glob(os.path.join(HERE, 'test_*.py'))
                if os.path.basename(t) not in TESTS)


def main():
    os.chdir(ROOT)
    failed = []
    # pure maths functions: native build compared with the emulator
    lib = 'build/difftest/libvd_math.so'
    os.makedirs('build/difftest', exist_ok=True)
    subprocess.run(['cc', '-std=c99', '-O2', '-ffp-contract=off', '-shared', '-fPIC', '-o', lib,
                    'src/game/vd_math.c', '-lm'], check=True)
    runs = [('vd_math (native vs original)', [sys.executable, os.path.join(HERE, 'autotest.py'), lib,
                                              'src/game/vd_math.c', '--range', '80000000', '80003000', '-n', '300'])]
    runs += [(t, [sys.executable, os.path.join(HERE, t)]) for t in TESTS]
    for name, cmd in runs:
        t0 = time.time()
        r = subprocess.run(cmd, capture_output=True, text=True)
        tail = [l for l in r.stdout.strip().splitlines() if l][-6:]
        ok = r.returncode == 0 and 'FAIL ' not in r.stdout.replace('FAIL 0', '')
        print(f'== {name}: {"OK" if ok else "FAILED"} ({time.time() - t0:.0f}s)')
        for l in tail:
            print('   ' + l)
        if not ok:
            failed.append(name)
            if r.stderr:
                print(r.stderr[-2000:])
    print('\nALL TESTS PASSED' if not failed else f'\nFAILED: {", ".join(failed)}')
    return 1 if failed else 0


if __name__ == '__main__':
    sys.exit(main())
