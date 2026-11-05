"""Compile clean C to big-endian MIPS (clang) and link it at 0x80700000 so it can
run inside the emulator right next to the original game code.

    blob, syms = build(['src/game/heap.c'])
    emu.write(CODE_BASE, blob); emu.call(syms['Heap_Alloc'], ...)

Game globals the C code refers to are given their original addresses through
`extra_syms` (or tools/difftest/game_syms.ld), so both versions share memory.
"""
import os, subprocess, tempfile

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
BIN = os.environ.get('MIPS_PREFIX', 'mips-linux-gnu-')   # binutils prefix (on PATH)
CODE_BASE = 0x80700000
CFLAGS = ['--target=mips-linux-gnu', '-march=mips32', '-mabi=32', '-mfp32', '-mhard-float', '-O2',
          '-fno-pic', '-mno-abicalls', '-G0', '-ffreestanding', '-fno-builtin', '-fno-math-errno',
          '-ffp-contract=off', '-fno-strict-aliasing', '-fwrapv', '-Wall', '-Wno-unused-function',
          '-nostdlibinc', '-isystem', os.path.join(os.path.dirname(os.path.abspath(__file__)), 'include'),
          '-I', os.path.join(ROOT, 'src', 'game'), '-I', os.path.join(ROOT, 'port'), '-DVD_MIPS_TEST']

SUPPORT_C = r'''
typedef unsigned long size_t;
void *memset(void *d, int c, size_t n) { unsigned char *p = d; while (n--) *p++ = (unsigned char)c; return d; }
void *memcpy(void *d, const void *s, size_t n) { unsigned char *p = d; const unsigned char *q = s; while (n--) *p++ = *q++; return d; }
void *memmove(void *d, const void *s, size_t n) { unsigned char *p = d; const unsigned char *q = s;
    if (p < q) while (n--) *p++ = *q++; else { p += n; q += n; while (n--) *--p = *--q; } return d; }
float sqrtf(float x) { float r; __asm__("sqrt.s %0, %1" : "=f"(r) : "f"(x)); return r; }
float fabsf(float x) { return x < 0.0f ? -x : x; }
'''


def build(sources, extra_syms=None, name='clean'):
    out_dir = os.path.join(ROOT, 'build', 'difftest')
    os.makedirs(out_dir, exist_ok=True)
    objs = []
    sup = os.path.join(out_dir, 'mips_support.c')
    open(sup, 'w').write(SUPPORT_C)
    for src in list(sources) + [sup]:
        o = os.path.join(out_dir, os.path.basename(src) + '.mips.o')
        r = subprocess.run(['clang'] + CFLAGS + ['-c', src, '-o', o], capture_output=True, text=True)
        if r.returncode:
            raise SystemExit(f'clang failed for {src}:\n{r.stderr}')
        objs.append(o)
    ld = os.path.join(out_dir, f'{name}.ld')
    with open(ld, 'w') as f:
        f.write(f'SECTIONS {{ . = 0x{CODE_BASE:08X}; .text : {{ *(.text*) }} .rodata : {{ *(.rodata*) }} '
                f'.data : {{ *(.data*) *(.sdata*) }} .bss : {{ *(.bss*) *(.sbss*) *(COMMON) }} /DISCARD/ : {{ *(.MIPS.*) *(.reginfo) *(.pdr) *(.comment) *(.note*) *(.gnu.attributes) }} }}\n')
        for k, v in (extra_syms or {}).items():
            f.write(f'{k} = 0x{v:08X};\n')
        gs = os.path.join(os.path.dirname(__file__), 'game_syms.ld')
        if os.path.exists(gs):
            f.write(open(gs).read())
    elf = os.path.join(out_dir, f'{name}.elf')
    r = subprocess.run([BIN + 'ld', '-EB', '-m', 'elf32btsmip', '-T', ld, '-o', elf] + objs, capture_output=True, text=True)
    if r.returncode:
        raise SystemExit(f'ld failed:\n{r.stderr}')
    binp = os.path.join(out_dir, f'{name}.bin')
    subprocess.run([BIN + 'objcopy', '-O', 'binary', elf, binp], check=True)
    syms = {}
    for line in subprocess.run([BIN + 'nm', elf], capture_output=True, text=True).stdout.splitlines():
        p = line.split()
        if len(p) == 3:
            syms[p[2]] = int(p[0], 16) & 0xFFFFFFFF
    return open(binp, 'rb').read(), syms
