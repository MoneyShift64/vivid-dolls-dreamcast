#!/usr/bin/env python3
"""Checks for the generated data-loader tables (tools/port/gen_dataload.py -> port/gen/).

    python3 tools/port/test_dataload.py [--rom baserom.z64] [--regen]

1. the swap runs of every data block cover each byte exactly once;
2. every relocation sits on a 32-bit item and its target lies inside a block;
3. little-endian host simulation (in Python) of the loader on the real ROM: every item
   read little-endian equals the original read big-endian; every relocated word equals
   the port address of its target, and the ROM word equals the N64 address of it;
7. the special cases of port/ENDIAN_AUDIT.md section D (static Vtx, D_8003B110, ovl2
   animation tables) have the required widths/relocations;
4. every leaf of every C-typed data symbol (its declared type, from the DWARF walk) is
   swapped with its own width, and reads back (as that type, little-endian) as the
   original big-endian value;
5. the game C's data symbols are all placed (none left over), and - if the MIPS
   toolchain is there - a mipsel link of all src/game/*.c with vd_blocks.ld leaves only
   functions undefined;
6. the C loader (port/sys/dataload.c) compiled for the host produces exactly the same
   block images as the Python simulation.
Exit status 0 = all passed.
"""
import argparse, hashlib, json, os, re, struct, subprocess, sys, tempfile

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
GEN = os.path.join(ROOT, 'port', 'gen')
FAILS = []


def fail(msg):
    FAILS.append(msg)
    print('FAIL:', msg)


def parse_blocks_h():
    txt = open(os.path.join(GEN, 'vd_blocks.h')).read()
    blocks = []
    for m in re.finditer(r'X\((\w+), (\d+), 0x([0-9A-F]+)u, (\d+), 0x([0-9A-F]+)u, (\d), (-?\d+)\)', txt):
        blocks.append(dict(name=m.group(1), id=int(m.group(2)), size=int(m.group(3), 16), align=int(m.group(4)),
                           n64=int(m.group(5), 16), data=int(m.group(6)), ovl=int(m.group(7))))
    ids = {('VD_BLK_' + b['name'].upper()): b['id'] for b in blocks}
    return blocks, ids


def parse_tables(ids):
    txt = open(os.path.join(GEN, 'vd_dataload_tables.c')).read()
    if re.search(r'"[^"]*"', re.sub(r'#include "[^"]*"', '', txt)):
        fail('tables contain a string literal (ROM text?)')
    runs, relocs = {}, {}
    for m in re.finditer(r'static const vd_swap_run swap_(\w+)\[(\d+)\] = \{(.*?)\};', txt, re.S):
        items = [(int(a, 16), int(b), int(c)) for a, b, c, _ in
                 re.findall(r'\{0x([0-9A-F]+),(\d+),(\d+),(\d+)\}', m.group(3))]
        if len(items) != int(m.group(2)):
            fail(f'swap_{m.group(1)}: {len(items)} entries parsed, {m.group(2)} declared')
        runs[m.group(1)] = items
    for m in re.finditer(r'static const vd_reloc reloc_(\w+)\[(\d+)\] = \{(.*?)\};', txt, re.S):
        items = [(int(a, 16), ids[b], int(c, 16)) for a, b, c in
                 re.findall(r'\{0x([0-9A-F]+),VD_RT\((VD_BLK_\w+),0x([0-9A-F]+)\)\}', m.group(3))]
        if len(items) != int(m.group(2)):
            fail(f'reloc_{m.group(1)}: {len(items)} entries parsed, {m.group(2)} declared')
        relocs[m.group(1)] = items
    table = []
    for m in re.finditer(r'\{ (VD_BLK_\w+), 0x([0-9A-F]+)u, 0x([0-9A-F]+)u, swap_(\w+), (\d+), (\w+), (\d+) \}', txt):
        table.append(dict(block=ids[m.group(1)], rom=int(m.group(2), 16), size=int(m.group(3), 16), name=m.group(4),
                          nruns=int(m.group(5)), nrelocs=int(m.group(7))))
    return runs, relocs, table


def fake_bases(blocks):
    """32-bit 'port' addresses for the simulation (like a DC link: blocks packed from 0x8C100000)."""
    a, out = 0x8C100000, {}
    for b in blocks:
        al = max(b['align'], 4)
        a = (a + al - 1) // al * al
        out[b['id']] = a
        a += max(b['size'], 1)
    return out


def simulate(rom, blocks, runs, relocs, table, addr):
    """Python model of vd_dataload_apply() for a little-endian host."""
    img = {}
    for d in table:
        buf = bytearray(rom[d['rom']:d['rom'] + d['size']])
        for (o, c, w) in runs[d['name']]:
            if w > 1:
                for i in range(c):
                    p = o + i * w
                    buf[p:p + w] = buf[p:p + w][::-1]
        for (o, tb, to) in relocs.get(d['name'], []):
            buf[o:o + 4] = struct.pack('<I', (addr[tb] + to) & 0xFFFFFFFF)
        img[d['block']] = bytes(buf)
    return img


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--rom', default=os.path.join(ROOT, 'baserom.z64'))
    ap.add_argument('--regen', action='store_true', help='run gen_dataload.py first')
    ap.add_argument('--no-c', action='store_true', help='skip the C loader / link checks')
    args = ap.parse_args()
    if args.regen:
        subprocess.run([sys.executable, os.path.join(ROOT, 'tools', 'port', 'gen_dataload.py')], check=True)

    blocks, ids = parse_blocks_h()
    bid = {b['id']: b for b in blocks}
    runs, relocs, table = parse_tables(ids)
    J = json.load(open(os.path.join(GEN, 'vd_dataload_types.json')))
    print(f'{len(blocks)} blocks, {len(table)} data blocks, '
          f'{sum(len(v) for v in runs.values())} swap runs, {sum(len(v) for v in relocs.values())} relocations')

    # ---- 1. coverage ----------------------------------------------------------------------
    widths = {}
    for d in table:
        b = bid[d['block']]
        if d['size'] != b['size']:
            fail(f'{d["name"]}: table size 0x{d["size"]:X} != block size 0x{b["size"]:X}')
        cov = bytearray(d['size'])
        W = {}
        for (o, c, w) in runs[d['name']]:
            if w not in (1, 2, 4, 8):
                fail(f'{d["name"]}+0x{o:X}: width {w}')
            if o % w:
                fail(f'{d["name"]}+0x{o:X}: width-{w} run not aligned')
            for i in range(c):
                p = o + i * w
                W[p] = w
                for j in range(w):
                    if p + j >= d['size']:
                        fail(f'{d["name"]}: run at 0x{o:X} runs past the end')
                        break
                    cov[p + j] += 1
        bad = [i for i, v in enumerate(cov) if v != 1]
        if bad:
            fail(f'{d["name"]}: {len(bad)} bytes not covered exactly once (first at 0x{bad[0]:X}, count {cov[bad[0]]})')
        else:
            print(f'  [1] {d["name"]}: 0x{d["size"]:X} bytes covered exactly once by {len(runs[d["name"]])} runs')
        widths[d['name']] = W

    # ---- 2. relocations ---------------------------------------------------------------------
    nrel = 0
    for d in table:
        for (o, tb, to) in relocs.get(d['name'], []):
            nrel += 1
            if widths[d['name']].get(o) != 4:
                fail(f'{d["name"]}+0x{o:X}: relocation not on a 32-bit item')
            if tb not in bid:
                fail(f'{d["name"]}+0x{o:X}: bad target block {tb}')
            elif not (0 <= to < bid[tb]['size'] or (to == bid[tb]['size'] and bid[tb]['size'])):
                fail(f'{d["name"]}+0x{o:X}: target {bid[tb]["name"]}+0x{to:X} outside the block')
            elif to == bid[tb]['size']:
                print(f'  [2] note: {d["name"]}+0x{o:X} points one past the end of {bid[tb]["name"]}')
    print(f'  [2] {nrel} relocations checked')

    # ---- 3. little-endian simulation on the real ROM -----------------------------------------
    if not os.path.exists(args.rom):
        fail(f'no ROM at {args.rom}; ROM checks skipped')
        return finish()
    rom = open(args.rom, 'rb').read()
    sha = open(os.path.join(ROOT, 'vividdolls.sha1')).read().split()[0]
    if hashlib.sha1(rom).hexdigest() != sha:
        fail('ROM SHA-1 does not match vividdolls.sha1')
    addr = fake_bases(blocks)
    img = simulate(rom, blocks, runs, relocs, table, addr)
    for d in table:
        buf, base = img[d['block']], d['rom']
        rel = {o: (tb, to) for (o, tb, to) in relocs.get(d['name'], [])}
        nbad = 0
        for (o, c, w) in runs[d['name']]:
            for i in range(c):
                p = o + i * w
                le = int.from_bytes(buf[p:p + w], 'little')
                be = int.from_bytes(rom[base + p:base + p + w], 'big')
                if p in rel:
                    tb, to = rel[p]
                    if be != bid[tb]['n64'] + to:
                        fail(f'{d["name"]}+0x{p:X}: ROM word 0x{be:08X} != N64 address of '
                             f'{bid[tb]["name"]}+0x{to:X}')
                    if le != addr[tb] + to:
                        fail(f'{d["name"]}+0x{p:X}: relocated word wrong')
                elif le != be:
                    nbad += 1
        if nbad:
            fail(f'{d["name"]}: {nbad} items differ between LE load and BE original')
        else:
            print(f'  [3] {d["name"]}: every item reads back LE == BE original, {len(rel)} relocations OK')

    # ---- 4. declared C types ------------------------------------------------------------------
    nleaf = nmis = 0
    mism = []
    for d in table:
        buf, base, W = img[d['block']], d['rom'], widths[d['name']]
        rel = {o for (o, tb, to) in relocs.get(d['name'], [])}
        for ent in J['ctyped'].get(d['name'], []):
            for (o, w, k) in ent['leaves']:
                nleaf += 1
                if W.get(o) != w:
                    nmis += 1
                    mism.append(f'{ent["sym"]} @{d["name"]}+0x{o:X}: declared width {w}, swapped as {W.get(o)}')
                    continue
                if o in rel:
                    continue
                fmt = {1: 'B', 2: 'H', 4: 'I', 8: 'Q'}[w]
                if k == 'float':
                    fmt = {4: 'f', 8: 'd'}[w]
                le = struct.unpack('<' + fmt, buf[o:o + w])[0]
                be = struct.unpack('>' + fmt, rom[base + o:base + o + w])[0]
                if le != be and not (le != le and be != be):      # NaN == NaN here
                    fail(f'{ent["sym"]} @{d["name"]}+0x{o:X}: {k}{w * 8} LE {le} != BE {be}')
    print(f'  [4] {nleaf} C-typed leaves checked, {nmis} swapped with a different width')
    for m in mism:
        print('      ' + m)
    # the only accepted kind of mismatch: leaves of an extern whose declared extent overlaps
    # another, more specific object (reported by the generator as a conflict); none expected
    if nmis:
        fail(f'{nmis} C-typed leaves swapped with a width different from their declared type')

    # ---- 7. cases from port/ENDIAN_AUDIT.md section D ---------------------------------------------
    audit_checks(J, bid, ids, table, widths, relocs, rom)

    # ---- 5. placement ------------------------------------------------------------------------
    if J['unplaced']:
        for s, why in J['unplaced']:
            fail(f'symbol {s} not placed: {why}')
    else:
        print(f'  [5] all {len(J["placed"])} data symbols the game C references are placed '
              f'(+{len(J["absolute"])} address constants)')
    ld = open(os.path.join(GEN, 'vd_blocks.ld')).read()
    for s in J['c_data_refs']:
        if not re.search(r'^' + re.escape(s) + r' = ', ld, re.M):
            fail(f'{s} referenced by the game C but missing from vd_blocks.ld')
    if not args.no_c:
        link_check(J)
        c_loader_check(rom, blocks, img, addr, table)
    return finish()


VTX = [(0, 2), (2, 2), (4, 2), (6, 2), (8, 2), (10, 2), (12, 1), (13, 1), (14, 1), (15, 1)]


def audit_checks(J, bid, ids, table, widths, relocs, rom):
    """Static data whose real use differs from splat's directives / the C declaration."""
    syms = J['placed']
    byname = {d['name']: d for d in table}
    n0 = len(FAILS)

    def want(sym, extra, layout, what):
        bn, off = syms[sym]
        W = widths[bn]
        for (o, w) in layout:
            if W.get(off + extra + o) != w:
                fail(f'audit: {sym}+0x{extra + o:X} ({what}) swapped as {W.get(off + extra + o)}, want {w}')
                return

    # static vertices: per field (6 x 16-bit, 4 colour bytes), not splat's .byte/.short/.word
    for sym in ('D_800386D0', 'D_80038710', 'D_80038750', 'D_80038790', 'gTextQuadVtx'):
        for i in range(4):
            want(sym if sym in syms else 'D_80038AD0', i * 16, VTX, f'Vtx[{i}]')
    sym = 'gObjDefaultDL' if 'gObjDefaultDL' in syms else 'D_80038F20'
    if 'D_80038F20' in syms:
        sym = 'D_80038F20'
    want(sym, 0, [(0, 4), (4, 4)], 'Gfx')
    for i in range(4):
        want(sym, 8 + i * 16, VTX, f'Vtx[{i}]')
    # the initial OSViMode: 20 x u32
    want('D_8003B110', 0, [(4 * i, 4) for i in range(20)], 'OSViMode word')
    # ovl2 animation tables: pointer arrays / EnemyAnims, every non-NULL word relocated,
    # and every target typed from the C struct (EffFrame / AnimFrame), not guessed
    tables = {'ovl2_D_800F8E1C': 0x30, 'ovl2_D_800F8E4C': 0x18, 'ovl2_D_800F922C': 0x14, 'ovl2_D_800F9318': 0x14,
              'ovl2_D_800F93B4': 0xC, 'ovl2_D_800F9488': 0xC, 'ovl2_D_800F95D4': 0x40, 'ovl2_D_800F9438': 0x18,
              'ovl2_D_800F9874': 0xC, 'ovl2_D_800F97B0': 0xC, 'ovl2_D_800F96DC': 0xC}
    typed_at = {}
    for bn, ents in J['ctyped'].items():
        for e in ents:
            for (o, w, k) in e['leaves']:
                typed_at[(bn, o)] = (w, e['sym'])
    ntgt = 0
    for sym, size in tables.items():
        bn, off = syms[sym]
        d = byname[bn]
        rel = {o: (tb, to) for (o, tb, to) in relocs[bn]}
        for o in range(off, off + size, 4):
            if widths[bn].get(o) != 4 or (bn, o) not in typed_at:
                fail(f'audit: {sym}+0x{o - off:X} not a C-typed 32-bit word')
            v = int.from_bytes(rom[d['rom'] + o:d['rom'] + o + 4], 'big')
            if v and o not in rel:
                fail(f'audit: {sym}+0x{o - off:X} holds 0x{v:08X} but is not relocated')
            if o in rel:
                tb, to = rel[o]
                tname = bid[tb]['name']
                for k in (0, 4):           # {pointer, s32} (EffFrame/AnimFrame) or a nested table
                    if typed_at.get((tname, to + k), (None,))[0] != 4:
                        fail(f'audit: target of {sym}+0x{o - off:X} ({tname}+0x{to + k:X}) not C-typed')
                        break
                ntgt += 1
    # nothing guessed in the overlays except all-zero words
    nz = 0
    for bn, rngs in J['guessed'].items():
        if not bn.startswith('ovl'):
            continue
        d = byname[bn]
        for (a, b) in rngs:
            if any(rom[d['rom'] + a:d['rom'] + b]):
                nz += b - a
    if nz:
        fail(f'audit: {nz} non-zero guessed bytes left in the overlays')
    if len(FAILS) == n0:
        print(f'  [7] ENDIAN_AUDIT D: 5 static Vtx sets per field, D_80038F20 Gfx+Vtx, D_8003B110 as 20 u32, '
              f'{len(tables)} ovl2 animation tables ({ntgt} typed targets); overlays: no non-zero guessed bytes')


def link_check(J):
    ld_bin = os.environ.get('MIPS_PREFIX', 'mips-linux-gnu-') + 'ld'
    if not os.path.exists(ld_bin):
        print('  [5] (no MIPS ld: link check skipped)')
        return
    inc = os.path.join(ROOT, 'tools', 'difftest', 'include')
    with tempfile.TemporaryDirectory() as td:
        objs = []
        for f in sorted(os.listdir(os.path.join(ROOT, 'src', 'game'))):
            if not f.endswith('.c'):
                continue
            o = os.path.join(td, f + '.o')
            r = subprocess.run(['clang', '--target=mipsel-linux-gnu', '-march=mips32', '-mabi=32', '-O2', '-fno-pic',
                                '-mno-abicalls', '-G0', '-ffreestanding', '-fno-builtin', '-w', '-nostdlibinc',
                                '-isystem', inc, '-I', os.path.join(ROOT, 'src', 'game'), '-I', os.path.join(ROOT, 'port'),
                                '-DVD_MIPS_TEST', '-c', os.path.join(ROOT, 'src', 'game', f), '-o', o],
                               capture_output=True, text=True)
            if r.returncode:
                fail(f'mipsel compile of {f} failed: {r.stderr[:300]}')
                return
            objs.append(o)
        shim = os.path.join(td, 'inc')
        os.makedirs(shim)
        open(os.path.join(shim, 'string.h'), 'w').write(
            '#include <stddef.h>\nvoid *memcpy(void *, const void *, size_t);\nvoid *memset(void *, int, size_t);\n')
        for src in ('port/sys/dataload.c', 'port/gen/vd_dataload_tables.c'):
            o = os.path.join(td, os.path.basename(src) + '.o')
            r = subprocess.run(['clang', '--target=mipsel-linux-gnu', '-march=mips32', '-mabi=32', '-O2', '-fno-pic',
                                '-mno-abicalls', '-G0', '-std=c99', '-Wall', '-Wextra', '-pedantic', '-Werror',
                                '-ffreestanding', '-nostdlibinc', '-isystem', shim,
                                '-I', os.path.join(ROOT, 'port', 'sys'), '-I', GEN, '-c', os.path.join(ROOT, src), '-o', o],
                               capture_output=True, text=True)
            if r.returncode:
                fail(f'mipsel compile of {src} failed:\n{r.stderr[:2000]}')
                return
            objs.append(o)
        script = os.path.join(td, 'link.ld')
        open(script, 'w').write(
            'SECTIONS { . = 0x80010000; .text : { *(.text*) } .rodata : { *(.rodata*) } .data : { *(.data*) } '
            '.bss : { *(.bss*) *(COMMON) } /DISCARD/ : { *(.MIPS.*) *(.reginfo) *(.pdr) *(.comment) *(.note*) '
            '*(.gnu.attributes) } }\n'
            f'INCLUDE {os.path.join(GEN, "vd_blocks.ld")}\nINCLUDE {os.path.join(GEN, "vd_func_aliases.ld")}\n')
        r = subprocess.run([ld_bin, '-EL', '-m', 'elf32ltsmip', '-T', script, '-o', os.path.join(td, 'a.elf')] + objs,
                           capture_output=True, text=True)
        undef = sorted(set(re.findall(r"undefined reference to `([^']+)'", r.stderr)))
        expected = set(J['func_missing']) | {'memcpy', 'memset'}
        data_undef = [u for u in undef if u not in expected]
        if data_undef:
            fail('mipsel link: undefined non-function symbols: ' + ' '.join(data_undef))
        else:
            print(f'  [5] mipsel link of the game C + loader: only {len(undef)} functions left undefined '
                  f'(libultra/asm/libc, provided by the port layer)')
        other = [l for l in r.stderr.splitlines() if 'undefined reference' not in l and 'in function' not in l
                 and l.strip() and 'more undefined references' not in l]
        if other:
            print('      ld also said: ' + ' | '.join(other[:5]))


HARNESS = r'''
#include <stdio.h>
#include <stdlib.h>
#include "dataload.h"
int main(int argc, char **argv) {
    FILE *f = fopen(argv[1], "rb"); long n; uint8_t *rom; uint32_t addr[VD_BLK_COUNT], bad = 0; unsigned i;
    if (!f) return 2;
    fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET);
    rom = malloc(n); if (fread(rom, 1, n, f) != (size_t)n) return 2; fclose(f);
    for (i = 0; i < VD_BLK_COUNT; i++) addr[i] = (uint32_t)strtoul(argv[3 + i], 0, 0);
    /* dirty every block first, so BSS zeroing is checked too */
    for (i = 0; i < VD_BLK_COUNT; i++) if (vd_block_size[i]) memset(vd_block_base[i], 0xA5, vd_block_size[i]);
    i = vd_dataload_apply(rom, n, vd_block_base, addr, 1, 1, &bad);
    if (i) { printf("error %d at 0x%X\n", (int)i, (unsigned)bad); return 1; }
    f = fopen(argv[2], "wb");
    for (i = 0; i < VD_BLK_COUNT; i++) fwrite(vd_block_base[i], 1, vd_block_size[i], f);
    fclose(f);
    /* a truncated ROM and a corrupted pointer must be refused */
    if (vd_dataload_apply(rom, 0x40000, vd_block_base, addr, 1, 0, &bad) != VD_DL_ERR_ROM_SIZE) return 3;
    for (i = 0; i < vd_data_block_count; i++) if (vd_data_blocks[i].nrelocs) {
        uint32_t o = vd_data_blocks[i].rom_offset + vd_data_blocks[i].relocs[0].offset;
        rom[o + 3] ^= 0x10;
        if (vd_dataload_apply(rom, n, vd_block_base, addr, 1, 0, &bad) != VD_DL_ERR_RELOC || bad != o) return 4;
        rom[o + 3] ^= 0x10;
        break;
    }
    return 0;
}
'''


def c_loader_check(rom, blocks, img, addr, table):
    cc = 'cc'
    with tempfile.TemporaryDirectory() as td:
        h = os.path.join(td, 'harness.c')
        open(h, 'w').write(HARNESS.replace('#include <stdlib.h>', '#include <stdlib.h>\n#include <string.h>'))
        exe = os.path.join(td, 'harness')
        r = subprocess.run([cc, '-std=c99', '-O1', '-Wall', '-Wextra', '-I', os.path.join(ROOT, 'port', 'sys'),
                            '-I', GEN, h, os.path.join(ROOT, 'port', 'sys', 'dataload.c'),
                            os.path.join(GEN, 'vd_dataload_tables.c'), '-o', exe], capture_output=True, text=True)
        if r.returncode:
            fail('host build of the C loader failed:\n' + r.stderr[:2000])
            return
        out = os.path.join(td, 'dump.bin')
        romf = os.path.join(td, 'rom.z64')
        open(romf, 'wb').write(rom)
        r = subprocess.run([exe, romf, out] + [hex(addr[b['id']]) for b in blocks], capture_output=True, text=True)
        if r.returncode:
            fail(f'C loader harness failed (exit {r.returncode}) {r.stdout}')
            return
        dump = open(out, 'rb').read()
        p = 0
        nbad = 0
        for b in blocks:
            chunk = dump[p:p + b['size']]
            p += b['size']
            want = img.get(b['id'], bytes(b['size']))
            if chunk != want:
                nbad += 1
                fail(f'C loader: block {b["name"]} differs from the Python simulation')
        if not nbad:
            print(f'  [6] C loader (host build, little-endian) == Python simulation for all {len(blocks)} blocks; '
                  f'BSS zeroed; short ROM and corrupted pointer refused')


def finish():
    if FAILS:
        print(f'\n{len(FAILS)} FAILURE(S)')
        return 1
    print('\nALL PASSED')
    return 0


if __name__ == '__main__':
    sys.exit(main())
