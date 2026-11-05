#!/usr/bin/env python3
"""Compile an m2c draft (e.g. src/draft/game.c) into a host shared library so
individual functions can be called from Python and compared with the original
MIPS code running in the emulator.

    python3 tools/difftest/build_draft_lib.py src/draft/game.c build/difftest/libdraft_game.so

Externs become definitions (zero-filled), undefined callees become no-op stubs,
and functions that fail to compile are replaced by stubs (and listed).
"""
import os, re, subprocess, sys

HEADER = r'''
#include <math.h>
#include <stddef.h>
void *memmove(void *, const void *, unsigned long); void *memset(void *, int, unsigned long);
typedef signed char s8; typedef unsigned char u8; typedef short s16; typedef unsigned short u16;
typedef int s32; typedef unsigned int u32; typedef long long s64; typedef unsigned long long u64;
typedef float f32; typedef double f64;
#define M2C_FIELD(expr, type_ptr, offset) (*(type_ptr)((s8 *)(expr) + (offset)))
#define M2C_BITWISE(type, expr) ((type)(expr))
#define M2C_LWL(expr) (expr)
#define M2C_FIRST3BYTES(expr) (expr)
#define M2C_UNALIGNED32(expr) (expr)
#define M2C_ERROR(desc) (0)
#define M2C_TRAP_IF(cond) (0)
#define M2C_BREAK() (0)
#define M2C_SYNC() (0)
#define M2C_CARRY 0
#define MULT_HI(a, b) ((s32)(((s64)(s32)(a) * (s64)(s32)(b)) >> 32))
#define MULTU_HI(a, b) ((u32)(((u64)(u32)(a) * (u64)(u32)(b)) >> 32))
typedef s32 M2C_UNK; typedef s8 M2C_UNK8; typedef s16 M2C_UNK16; typedef s32 M2C_UNK32; typedef s64 M2C_UNK64;
'''

FUNC_RE = re.compile(r'^([A-Za-z_][\w \*\?]*?)\b([A-Za-z_]\w*)\(([^;{]*)\)\s*\{', re.M)


def split(src):
    """Return (preamble, [(name, text)]) for an m2c output file."""
    ms = list(FUNC_RE.finditer(src))
    pre = src[:ms[0].start()] if ms else src
    funcs = []
    for i, m in enumerate(ms):
        end = ms[i + 1].start() if i + 1 < len(ms) else len(src)
        funcs.append((m.group(2), src[m.start():end]))
    return pre, funcs


def fix_preamble(pre, func_names=()):
    out = []; defs = []
    for line in pre.splitlines():
        # m2c's own prototypes: we generate our own from the definitions
        if re.match(r'^[A-Za-z_][\w \*\?]*\b\w+\(.*\);', line) and 'extern' not in line.split('(')[0]:
            continue
        m = re.match(r'extern (.+?) (\**)(\w+)(\[\w*\])?;', line)
        if m:
            t, stars, name, arr = m.groups()
            t = t.replace('?', 'u8')
            arr = '[0x4000]' if arr is not None else ''
            if name in func_names:
                continue
            defs.append(f'{t} {stars}{name}{arr} __attribute__((aligned(16)));')
            continue
        out.append(line.replace('? ', 'u8 '))
    return '\n'.join(out + defs)


def build(src_path, out_so, exclude=()):
    src = open(src_path).read()
    # functions returning function pointers: simplify the declarator
    src = re.sub(r'\b\w+ \(\*(\w+)\(([^()]*)\)\)\([^()]*\)', r'void *\1(\2)', src)
    pre, funcs = split(src)
    body_names = {n for n, _ in funcs}
    bad = set(exclude)
    for attempt in range(40):
        used = set(re.findall(r'struct (_struct_\w+?_0x([0-9A-Fa-f]+))\b', src))
        defined = set(re.findall(r'^struct (\w+) \{', src, re.M))
        autostructs = [f'struct {n} {{ char data[0x{sz}]; }};' for n, sz in sorted(used) if n not in defined]
        parts = [HEADER] + autostructs + [fix_preamble(pre, body_names)]
        # prototypes so call order doesn't matter
        for n, t in funcs:
            sig = t[:t.index('{')].strip().replace('? ', 'u8 ')
            if n not in bad:
                parts.append(sig + ';')
        called = set(re.findall(r'\b([A-Za-z_]\w*)\(', ''.join(t for n, t in funcs if n not in bad)))
        for n, t in funcs:
            if n in bad:
                sig = t[:t.index('{')].strip().replace('? ', 'u8 ')
                parts.append(f'u32 {n}() {{ return 0; }} /* failed to compile */')
        known_c = {'sqrtf', 'sinf', 'cosf', 'fabsf', 'memcpy', 'memset', 'strlen', 'bcopy', 'bzero'}
        keywords = {'if', 'while', 'for', 'switch', 'return', 'sizeof', 'M2C_FIELD', 'M2C_BITWISE',
                    'M2C_ERROR', 'M2C_LWL', 'MULT_HI', 'MULTU_HI', 'M2C_FIRST3BYTES', 'M2C_UNALIGNED32',
                    'M2C_TRAP_IF', 'M2C_BREAK', 'M2C_SYNC'}
        parts.append('void bcopy(const void *a, void *b, unsigned long n) { memmove(b, a, n); }')
        parts.append('void bzero(void *a, unsigned long n) { memset(a, 0, n); }')
        for c in sorted(called - body_names - known_c - keywords):
            parts.append(f'u32 {c}() {{ return 0; }} /* stub */')
        for n, t in funcs:
            if n not in bad:
                parts.append(t.replace('? ', 'u8 ').replace('(?)', '(u8)'))
        unit = '\n'.join(parts)
        os.makedirs(os.path.dirname(out_so), exist_ok=True)
        cpath = out_so.replace('.so', '.c')
        open(cpath, 'w').write(unit)
        r = subprocess.run(['gcc', '-std=gnu99', '-O1', '-ffp-contract=off', '-fno-strict-aliasing', '-fwrapv',
                            '-w', '-ftrack-macro-expansion=0', '-shared', '-fPIC', '-o', out_so, cpath, '-lm'],
                           capture_output=True, text=True)
        if r.returncode == 0:
            return bad
        # find which functions contain the error lines
        lines = unit.split('\n')
        errs = set()
        for m in re.finditer(r':(\d+):\d+: error', r.stderr):
            ln = int(m.group(1)) - 1
            for k in range(ln, -1, -1):
                fm = FUNC_RE.match(lines[k] + '{') if lines[k].rstrip().endswith('{') else None
                if fm and fm.group(2) in body_names:
                    errs.add(fm.group(2)); break
        if not errs:
            print(r.stderr[:3000]); raise SystemExit('cannot attribute compile errors')
        bad |= errs
    raise SystemExit('too many attempts')


if __name__ == '__main__':
    bad = build(sys.argv[1], sys.argv[2])
    print(f'built {sys.argv[2]}; {len(bad)} functions stubbed (did not compile): {" ".join(sorted(bad))}')
