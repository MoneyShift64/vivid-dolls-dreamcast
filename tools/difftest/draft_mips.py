"""Compile selected m2c draft functions (or clean C) for MIPS and link them against
the ORIGINAL game: every other function and every global resolves to its real
address, so a draft can be dropped into the running game image and compared
with the original function in the emulator.

    blob, syms = build_drafts(['func_80002200', ...])
"""
import os, re, subprocess
from mipsbuild import CFLAGS, BIN, CODE_BASE, ROOT, SUPPORT_C
import build_draft_lib as B

DRAFT_HDR = B.HEADER.replace('#include <math.h>\n', '#include <math.h>\n')


def symbol_map():
    """name -> address for every symbol the game knows."""
    m = {}
    for line in open(os.path.join(ROOT, 'symbol_addrs.txt')):
        r = re.match(r'(\w+) = 0x([0-9A-Fa-f]+);', line)
        if r:
            m[r.group(1)] = int(r.group(2), 16)
    for f in ('undefined_syms_auto.txt', 'undefined_funcs_auto.txt', 'absolute_syms.txt'):
        p = os.path.join(ROOT, f)
        if os.path.exists(p):
            for line in open(p):
                r = re.match(r'\s*(\w+) = 0x([0-9A-Fa-f]+);', line)
                if r:
                    m[r.group(1)] = int(r.group(2), 16)
    return m


def addr_of(name, smap):
    if name in smap:
        return smap[name]
    r = re.match(r'^(?:ovl\d_)?(?:func|D|jtbl)_([0-9A-F]+)$', name)
    if r:
        return int(r.group(1), 16)
    return None


def make_unit(draft_path, names):
    src = open(draft_path).read()
    src = re.sub(r'\b\w+ \(\*(\w+)\(([^()]*)\)\)\([^()]*\)', r'void *\1(\2)', src)
    pre, funcs = B.split(src)
    fmap = dict(funcs)
    out_pre = []
    # preamble: keep structs and prototypes of functions we don't define; turn
    # static data definitions into externs (so the real game data is used)
    skip = 0
    lines = pre.splitlines()
    i = 0
    while i < len(lines):
        ln = lines[i]
        if ln.startswith('static ') and '(' not in ln.split('=')[0]:
            decl = re.sub(r'/\*.*?\*/', '', ln)
            while not decl.rstrip().endswith(';'):
                i += 1
                decl += '\n' + re.sub(r'/\*.*?\*/', '', lines[i])
            head = decl.split('=')[0].strip().rstrip(';').replace('static ', 'extern ', 1)
            out_pre.append(head + ';')
            i += 1
            continue
        m = re.match(r'^[A-Za-z_][\w \*\?]*?\b(\w+)\(.*\);', ln)
        if m and m.group(1) in names:
            i += 1
            continue
        out_pre.append(ln)
        i += 1
    used = set(re.findall(r'struct (_struct_\w+?_0x([0-9A-Fa-f]+))\b', src))
    defined = set(re.findall(r'^struct (\w+) \{', src, re.M))
    autostructs = [f'struct {n} {{ char data[0x{sz}]; }};' for n, sz in sorted(used) if n not in defined]
    parts = [DRAFT_HDR] + autostructs + ['\n'.join(out_pre).replace('? ', 'u8 ')]
    for n in names:
        t = fmap[n]
        parts.append(t[:t.index('{')].strip().replace('? ', 'u8 ') + ';')
    for n in names:
        parts.append(fmap[n].replace('? ', 'u8 ').replace('(?)', '(u8)'))
    return '\n'.join(parts)


def link(objs, name, wanted_syms):
    out_dir = os.path.join(ROOT, 'build', 'difftest')
    smap = symbol_map()
    # undefined symbols -> original addresses
    und = set()
    for o in objs:
        r = subprocess.run([BIN + 'nm', '-u', o], capture_output=True, text=True)
        und |= {l.split()[-1] for l in r.stdout.splitlines() if l.strip()}
    defined = set()
    for o in objs:
        r = subprocess.run([BIN + 'nm', '--defined-only', o], capture_output=True, text=True)
        defined |= {l.split()[-1] for l in r.stdout.splitlines() if l.strip()}
    und -= defined
    ld = os.path.join(out_dir, f'{name}.ld')
    missing = []
    with open(ld, 'w') as f:
        f.write(f'SECTIONS {{ . = 0x{CODE_BASE:08X}; .text : {{ *(.text*) }} .rodata : {{ *(.rodata*) }} '
                f'.data : {{ *(.data*) *(.sdata*) }} .bss : {{ *(.bss*) *(.sbss*) *(COMMON) }} '
                f'/DISCARD/ : {{ *(.MIPS.*) *(.reginfo) *(.pdr) *(.comment) *(.note*) *(.gnu.attributes) }} }}\n')
        for s in sorted(und):
            a = addr_of(s, smap)
            if a is None:
                missing.append(s)
            else:
                f.write(f'{s} = 0x{a:08X};\n')
    if missing:
        raise SystemExit(f'no address for: {missing}')
    elf = os.path.join(out_dir, f'{name}.elf')
    r = subprocess.run([BIN + 'ld', '-EB', '-m', 'elf32btsmip', '-T', ld, '-o', elf] + objs, capture_output=True, text=True)
    if r.returncode:
        raise SystemExit('ld failed:\n' + r.stderr)
    binp = os.path.join(out_dir, f'{name}.bin')
    subprocess.run([BIN + 'objcopy', '-O', 'binary', elf, binp], check=True)
    syms = {}
    for l in subprocess.run([BIN + 'nm', elf], capture_output=True, text=True).stdout.splitlines():
        p = l.split()
        if len(p) == 3:
            syms[p[2]] = int(p[0], 16) & 0xFFFFFFFF
    return open(binp, 'rb').read(), syms


def compile_c(path, extra=()):
    o = path + '.mips.o'
    r = subprocess.run(['clang'] + CFLAGS + ['-Wno-everything'] + list(extra) + ['-c', path, '-o', o],
                       capture_output=True, text=True)
    if r.returncode:
        raise RuntimeError(r.stderr[:4000])
    return o


def support_obj():
    os.makedirs(os.path.join(ROOT, 'build', 'difftest'), exist_ok=True)
    p = os.path.join(ROOT, 'build', 'difftest', f'support_min_{os.getpid()}.c')
    # only things the game itself lacks (game code has its own bzero/bcopy etc.)
    open(p, 'w').write('float sqrtf(float x) { float r; __asm__("sqrt.s %0, %1" : "=f"(r) : "f"(x)); return r; }\n'
                       'float fabsf(float x) { return x < 0.0f ? -x : x; }\n')
    return compile_c(p)


def build_drafts(names, draft_path=None, name='drafts'):
    draft_path = draft_path or os.path.join(ROOT, 'src', 'draft', 'game.c')
    os.makedirs(os.path.join(ROOT, 'build', 'difftest'), exist_ok=True)
    unit = make_unit(draft_path, names)
    c = os.path.join(ROOT, 'build', 'difftest', f'{name}.c')
    open(c, 'w').write(unit)
    return link([compile_c(c), support_obj()], name, names)


def build_clean(sources, name='cleanlink'):
    """Clean C sources, linked against the original game for everything else."""
    objs = [compile_c(os.path.join(ROOT, s)) for s in sources]
    return link(objs + [support_obj()], name, [])
