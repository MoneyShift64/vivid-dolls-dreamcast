#!/usr/bin/env python3
"""Differential test: run C functions natively and the original MIPS code in the
emulator on the same random inputs, and compare every result bit for bit.

    python3 tools/difftest/autotest.py LIB.so SOURCE.c [FUNC ...] [--range LO HI] [-n N]

Only "self-contained" functions are tested: arguments that are numbers or
pointers to float/word buffers, no globals written, no byte/halfword access to
the buffers (those would differ between big- and little-endian memory).
"""
import ctypes, math, os, random, re, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from emu import N64Emu

BUF_WORDS = 64
EMU_BUF = 0x80600000

SIG_RE = re.compile(r'^([A-Za-z_][\w \*]*?)\s*\b(\w+)\(([^;{]*)\)\s*\{', re.M)


def parse_sigs(src):
    sigs = {}
    for m in SIG_RE.finditer(src):
        ret, name, args = m.group(1).strip(), m.group(2), m.group(3).strip()
        if args in ('', 'void'):
            al = []
        else:
            al = []
            for a0 in args.split(','):
                a0 = a0.split('/*')[0].strip()
                arr = re.search(r'\[\d*\]\s*$', a0)
                a0 = re.sub(r'\[\d*\]\s*$', '', a0).strip()
                a0 = re.sub(r'\b[A-Za-z_]\w*$', '', a0).strip() if (' ' in a0 or '*' in a0) else a0
                al.append(a0 + ('*' if arr else ''))
            al = [re.sub(r'\s*\*\s*', '*', a) for a in al]
        sigs[name] = (ret, al)
    return sigs


def kind(t):
    t = t.replace(' ', '').replace('const', '')
    if re.search(r'\[\d*\]$', t): return 'ptr'
    if '**' in t or '(' in t: return None
    if t.endswith('*'): return 'ptr'
    if t in ('f32',): return 'f32'
    if t in ('f64',): return 'f64'
    if t in ('s32', 'u32', 's16', 'u16', 's8', 'u8', 'M2C_UNK', 'M2C_UNK32', 'int'): return 'int'
    if t == 'void': return 'void'
    return None


def asm_funcs(paths):
    """name -> (vram, list of instruction strings)"""
    out = {}
    for p in paths:
        cur = None
        for line in open(p):
            m = re.match(r'glabel (\S+)', line)
            if m:
                cur = m.group(1); out[cur] = [None, []]; continue
            m = re.match(r'\s*/\* [0-9A-F]+ ([0-9A-F]{8}) [0-9A-F]{8} \*/\s+(.*)', line)
            if cur and m:
                if out[cur][0] is None: out[cur][0] = int(m.group(1), 16)
                out[cur][1].append(m.group(2))
    return out


def unsuitable(insns, allowed=()):
    for i in insns:
        op = i.split()[0] if i.split() else ''
        if op in ('lb', 'lbu', 'lh', 'lhu', 'sb', 'sh', 'lwl', 'lwr', 'swl', 'swr'):
            return 'byte/halfword memory access'
        if op == 'jal':
            tgt = i.split()[1]
            if tgt not in ('sqrtf',) and tgt not in allowed:
                return f'calls {tgt}'
        if re.search(r'%(hi|lo)\((D_|g)', i) and op.startswith('s'):
            return 'writes globals'
    return None


def fbits(v): return struct.unpack('<I', struct.pack('<f', v))[0]


def rand_float(rng):
    r = rng.random()
    if r < 0.05: return 0.0
    if r < 0.10: return rng.choice([1.0, -1.0, 0.5, 2.0])
    return rng.uniform(-8, 8)


def tags(src):
    """'/* @ 0x80001234' comments before C functions -> {c_name: address}"""
    out = {}
    for m in re.finditer(r'/\* @ 0x([0-9A-Fa-f]{8})[^*]*(?:\*(?!/)[^*]*)*\*/\s*\n([^\n{;]*?)\b(\w+)\(', src):
        out[m.group(3)] = int(m.group(1), 16)
    return out


def run(lib_path, src_path, names, lo=None, hi=None, n=300, asm_paths=None, overlay=None, quiet=False):
    lib = ctypes.CDLL(os.path.abspath(lib_path))
    src = open(src_path).read()
    sigs = parse_sigs(src)
    funcs = asm_funcs(asm_paths)
    tg = tags(src)
    allowed = set()
    if tg:   # clean source: map C names onto the original functions by address
        by_addr = {v[0]: (k, v) for k, v in funcs.items()}
        allowed = {by_addr[a][0] for a in tg.values() if a in by_addr}
        funcs = {c: by_addr[a][1] for c, a in tg.items() if a in by_addr}
    emu = N64Emu(overlay=overlay)
    rng = random.Random(1234)
    results = {}
    targets = names or [k for k, v in funcs.items() if v[0] is not None and lo <= v[0] < hi]
    for name in targets:
        rfd, wfd = os.pipe()
        pid = os.fork()
        if pid == 0:
            os.close(rfd)
            res = _test_one(name, sigs, funcs, allowed, lib, emu, rng, n, quiet)
            os.write(wfd, repr(res).encode()); os._exit(0)
        os.close(wfd)
        data = b''
        while True:
            chunk = os.read(rfd, 65536)
            if not chunk: break
            data += chunk
        os.close(rfd)
        _, status = os.waitpid(pid, 0)
        if data:
            results[name] = eval(data.decode())
        else:
            results[name] = ('FAIL', f'host crashed (signal {os.WTERMSIG(status) if os.WIFSIGNALED(status) else status})')
            if not quiet: print(f'{name:24s} FAIL {results[name][1]}', flush=True)
    return results


def _test_one(name, sigs, funcs, allowed, lib, emu, rng, n, quiet):
    if True:
        results = {}
        if name not in sigs or name not in funcs:
            return ('skip', 'no signature')
        ret, args = sigs[name]
        kinds = [kind(a) for a in args]
        rk = kind(ret)
        why = unsuitable(funcs[name][1], allowed)
        if None in kinds or rk is None or rk == 'ptr' or why:
            results[name] = ('skip', why or f'types {ret} {args}')
            if not quiet: print(f'{name:24s} skip ({results[name][1]})', flush=True)
            return results[name]
        fn = getattr(lib, name)
        ct = {'ptr': ctypes.c_void_p, 'f32': ctypes.c_float, 'f64': ctypes.c_double, 'int': ctypes.c_uint32}
        fn.argtypes = [ct[k] for k in kinds]
        fn.restype = {'void': None, 'f32': ctypes.c_float, 'f64': ctypes.c_double, 'int': ctypes.c_uint32}[rk]
        mism = 0; first = None
        for t in range(n):
            vals = []
            for k in kinds:
                if k == 'ptr': vals.append([fbits(rand_float(rng)) for _ in range(BUF_WORDS)])
                elif k in ('f32', 'f64'): vals.append(rand_float(rng))
                else: vals.append(rng.choice([0, 1, 2, 3, 7, 100, 255, 1023, rng.randrange(0, 2048)]))
            # ---- host ----
            hbufs = []; hargs = []
            for k, v in zip(kinds, vals):
                if k == 'ptr':
                    b = (ctypes.c_uint32 * BUF_WORDS)(*v); hbufs.append(b); hargs.append(ctypes.addressof(b))
                else: hargs.append(v)
            hret = fn(*hargs)
            # ---- emulator (o32 calling convention) ----
            ia, fa, da = [], [], []
            all_fp_so_far = True
            slot = 0; eptrs = []
            for i, (k, v) in enumerate(zip(kinds, vals)):
                if k == 'ptr':
                    addr = EMU_BUF + 0x400 * len(eptrs); eptrs.append(addr)
                    emu.write(addr, b''.join(struct.pack('>I', w) for w in v))
                    ia.append(addr); all_fp_so_far = False
                elif k == 'f32':
                    if i < 2 and all_fp_so_far: fa.append(v); ia.append(0)
                    else: ia.append(struct.unpack('>I', struct.pack('>f', v))[0])
                elif k == 'f64':
                    raise NotImplementedError
                else:
                    ia.append(v); all_fp_so_far = False
            try:
                v0, f0 = emu.call(funcs[name][0], a=ia, f=fa)
            except Exception as ex:
                mism += 1; first = first or ('emu error', str(ex), '')
                continue
            ok = True
            if rk == 'f32':
                hb = fbits(hret); eb = emu.f0_bits()
                ok = hb == eb or (math.isnan(hret) and math.isnan(f0))
            elif rk == 'int':
                ok = (hret & 0xFFFFFFFF) == v0
            for b, addr in zip(hbufs, eptrs):
                ew = struct.unpack(f'>{BUF_WORDS}I', emu.read(addr, 4 * BUF_WORDS))
                if list(b) != list(ew):
                    # treat NaN==NaN
                    for x, y in zip(b, ew):
                        if x != y and not ((x & 0x7F800000) == 0x7F800000 and (y & 0x7F800000) == 0x7F800000 and x & 0x7FFFFF and y & 0x7FFFFF):
                            ok = False; break
            if not ok:
                mism += 1
                if first is None: first = (vals if len(str(vals)) < 300 else 'big', hret, v0 if rk == 'int' else f0)
        results[name] = ('PASS' if mism == 0 else 'FAIL', f'{n - mism}/{n}' + ('' if first is None else f' first diff: host {first[1]} orig {first[2]}'))
        if not quiet: print(f'{name:24s} {results[name][0]} {results[name][1]}', flush=True)
        return results[name]


if __name__ == '__main__':
    a = sys.argv[1:]
    lib, src = a[0], a[1]
    n = 300; lo = hi = None; names = []; asm = ['asm/game.s']; ovl = None
    i = 2
    while i < len(a):
        if a[i] == '--range': lo, hi = int(a[i + 1], 16), int(a[i + 2], 16); i += 3
        elif a[i] == '-n': n = int(a[i + 1]); i += 2
        elif a[i] == '--asm': asm = [a[i + 1]]; i += 2
        elif a[i] == '--overlay': ovl = int(a[i + 1]); i += 2
        else: names.append(a[i]); i += 1
    r = run(lib, src, names, lo, hi, n, asm, ovl)
    print('PASS', sum(1 for v in r.values() if v[0] == 'PASS'), 'FAIL', sum(1 for v in r.values() if v[0] == 'FAIL'),
          'skipped', sum(1 for v in r.values() if v[0] == 'skip'))
