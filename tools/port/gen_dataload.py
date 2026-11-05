#!/usr/bin/env python3
"""Generate the Dreamcast port's memory-block layout and boot-time data-loader tables.

    python3 tools/port/gen_dataload.py            # writes port/gen/*
    python3 tools/port/test_dataload.py         # checks the result (needs baserom.z64)

Inputs (all from the repository, see port/ARCHITECTURE.md section 2):
  vividdolls.yaml                 segment layout (main + 4 overlays)
  asm/data/*.s                    splat data/rodata/bss disassembly (widths, pointers)
  symbol_addrs.txt, undefined_syms_auto.txt, absolute_syms.txt
  src/game/*.c                    the game C; every extern it references is typed by
                                  compiling a probe TU with clang (32-bit MIPS) and -g and
                                  reading the DWARF (llvm-dwarfdump)

Outputs (port/gen/):
  vd_blocks.h             block ids, N64 ranges, sizes, alignments
  vd_blocks.ld            symbol = block_base + offset   (no symbol prefix: mipsel/host builds)
  vd_blocks_sh.ld         same with the leading '_' sh-elf GCC adds to C symbols
  vd_func_aliases.ld      game-C call names (func_XXXXXXXX / ovlN_func_...) -> C function names
  vd_dataload_tables.c    per data block: ROM offset/size, swap runs, relocations
  vd_dataload_types.json  leaf layout of every C-typed data symbol (used by the test)
  dataload_report.txt     guessed widths, conflicts, unused code pointers, unplaced symbols

The generated files contain offsets, sizes and types only - never ROM bytes. The ROM is
read only with --check (and by tools/port/test_dataload.py).
"""
import argparse, collections, glob, json, os, re, subprocess, sys, tempfile

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
GEN = os.path.join(ROOT, 'port', 'gen')
CLANG = 'clang'
DWARFDUMP = 'llvm-dwarfdump-18' if subprocess.run(['sh', '-c', 'command -v llvm-dwarfdump-18'],
                                                 capture_output=True).returncode == 0 else 'llvm-dwarfdump'
NM = os.environ.get('MIPS_PREFIX', 'mips-linux-gnu-') + 'nm'
if not os.path.exists(NM):
    NM = 'llvm-nm'
CFLAGS = ['--target=mips-linux-gnu', '-march=mips32', '-mabi=32', '-mfp32', '-mhard-float', '-O0', '-g',
          '-fno-pic', '-mno-abicalls', '-G0', '-ffreestanding', '-fno-builtin', '-w',
          '-nostdlibinc', '-isystem', os.path.join(ROOT, 'tools', 'difftest', 'include'),
          '-I', os.path.join(ROOT, 'src', 'game'), '-I', os.path.join(ROOT, 'port'), '-DVD_MIPS_TEST']


# ----------------------------------------------------------------------------- blocks
class Block:
    def __init__(self, name, lo, hi, kind, rom=None, ovl=None, align=16, note=''):
        self.name, self.lo, self.hi, self.kind, self.rom, self.ovl = name, lo, hi, kind, rom, ovl
        self.align, self.note = align, note
        self.size = hi - lo
        self.stops = []

    def __contains__(self, a):
        return self.lo <= a < self.hi


def load_blocks():
    """Data/BSS blocks from the splat yaml, plus the fixed RAM areas outside every segment."""
    import yaml
    y = yaml.safe_load(open(os.path.join(ROOT, 'vividdolls.yaml')))
    segs = [s for s in y['segments'] if isinstance(s, dict) and s.get('type') == 'code']
    starts = sorted(s['start'] for s in y['segments'] if isinstance(s, dict)) + [0x800000]
    blocks = []
    for s in segs:
        name, vram, start = s['name'], s['vram'], s['start']
        seg_end = min(x for x in starts if x > start)
        subs = []
        for sub in s['subsegments']:
            if isinstance(sub, dict):
                subs.append((sub['start'], sub['type']))
            else:
                subs.append((sub[0], sub[1]))
        data_start = None
        bss = None
        stops = []
        for (off, typ), nxt in zip(subs, [x[0] for x in subs[1:]] + [seg_end]):
            if typ in ('data', 'rodata'):
                stops.append(off)
                if data_start is None:
                    data_start = off
            if typ == 'bss':
                bss = off
        data_end = bss if bss is not None else seg_end
        ovl = int(name[3:]) if name.startswith('ovl') else None
        pfx = 'main' if name == 'main' else name
        if data_start is not None:
            blocks.append(Block(f'{pfx}_data', vram + data_start - start, vram + data_end - start, 'data',
                                rom=data_start, ovl=ovl))
            blocks[-1].stops = [o - data_start for o in stops]   # .data/.rodata section starts
        bss_lo = vram + data_end - start
        bss_size = s.get('bss_size', 0)
        blocks.append(Block(f'{pfx}_bss', bss_lo, bss_lo + bss_size, 'bss', ovl=ovl))
    # RSP microcode text lives at the end of the libultra code segment. The port never runs
    # it, but the game C takes its addresses (ucode_boot / ucode pointers and their sizes).
    blocks.append(Block('ucode', 0x80033FA0, 0x80038150, 'zero',
                        note='RSP microcode text (rspboot, aspMain, 3 gfx ucodes); addresses only, not loaded'))
    blocks.append(Block('zbuf', 0x80115000, 0x8013A800, 'zero', align=32, note='Z buffer (gZBuffer)'))
    blocks.append(Block('rcpbuf', 0x8013A800, 0x80173600, 'zero',
                        note='RSP yield data, DRAM stack, RDP output FIFO (D_8013A800..D_80173510)'))
    blocks.append(Block('heap', 0x80173600, 0x8069E600, 'zero', note='gGameHeap: Heap header + 0x52B000 bytes'))
    blocks.append(Block('taskpool', 0x8069E600, 0x807B2600, 'zero', note='gTaskPoolMem / D_8069E600, 0x114000 bytes'))
    blocks.append(Block('fb0', 0x807B2600, 0x807D7E00, 'zero', align=32, note='gFrameBuffer0'))
    blocks.append(Block('fb1', 0x807D7E00, 0x807FD600, 'zero', align=32, note='gFrameBuffer1'))
    return blocks


def find_block(blocks, addr, ovl=None, allow_end=False):
    """Block holding N64 address `addr`. Overlays share 0x800E0000+, so `ovl` picks one."""
    cands = [b for b in blocks if b.lo <= addr < b.hi or (allow_end and addr == b.hi and b.size)]
    if ovl is not None:
        c2 = [b for b in cands if b.ovl == ovl]
        if c2:
            cands = c2
    cands = [b for b in cands if b.ovl is None or ovl is None or b.ovl == ovl]
    if len(cands) > 1:
        exact = [b for b in cands if b.lo <= addr < b.hi]
        if len(exact) == 1:
            return exact[0]
        return 'ambiguous'
    return cands[0] if cands else None


# ----------------------------------------------------------------------------- symbols
def load_symbols():
    syms = {}
    for fn in ('symbol_addrs.txt', 'undefined_syms_auto.txt', 'absolute_syms.txt'):
        p = os.path.join(ROOT, fn)
        if not os.path.exists(p):
            continue
        for line in open(p):
            m = re.match(r'\s*(\w+)\s*=\s*(0x[0-9A-Fa-f]+|\d+)\s*;(.*)', line)
            if not m:
                continue
            attrs = dict(re.findall(r'(\w+):(\S+)', m.group(3)))
            syms.setdefault(m.group(1), (int(m.group(2), 0), attrs, fn))
    return syms


def symbol_address(name, syms):
    """(address, overlay or None, attrs) for a symbol name, or None."""
    m = re.match(r'(?:ovl(\d)_)?D_([0-9A-Fa-f]{1,8})$', name)
    if name in syms:
        a, attrs, _ = syms[name]
        seg = attrs.get('segment', '')
        ovl = int(seg[3:]) if seg.startswith('ovl') else None
        if m and m.group(1):
            ovl = int(m.group(1))
        return a, ovl, attrs
    if m:
        return int(m.group(2), 16), (int(m.group(1)) if m.group(1) else None), {}
    m = re.match(r'(?:ovl(\d)_)?func_([0-9A-Fa-f]{8})$', name)
    if m:
        return int(m.group(2), 16), (int(m.group(1)) if m.group(1) else None), {'type': 'func'}
    return None


# ----------------------------------------------------------------------------- DWARF
class PtrKind(str):
    """Leaf kind 'ptr' that remembers the pointee type (for propagating types along relocations)."""
    def __new__(cls, dw, die):
        o = str.__new__(cls, 'ptr')
        o.target = (dw, die)
        return o


class Dwarf:
    """Minimal parser for `llvm-dwarfdump --debug-info` text output."""
    def __init__(self, text):
        self.dies = {}
        self.vars = {}
        stack = []
        cur = None
        for line in text.splitlines():
            m = re.match(r'^0x([0-9a-f]+):(\s+)(DW_TAG_\w+|NULL)', line)
            if m:
                off, depth, tag = int(m.group(1), 16), len(m.group(2)), m.group(3)
                while stack and stack[-1][0] >= depth:
                    stack.pop()
                if tag == 'NULL':
                    cur = None
                    continue
                cur = {'off': off, 'tag': tag, 'attrs': {}, 'children': []}
                self.dies[off] = cur
                if stack:
                    stack[-1][1]['children'].append(cur)
                stack.append((depth, cur))
                continue
            m = re.match(r'^\s+(DW_AT_\w+)\s+\((.*)\)\s*$', line)
            if m and cur is not None:
                cur['attrs'][m.group(1)] = m.group(2)
        for d in self.dies.values():
            if d['tag'] == 'DW_TAG_variable' and 'DW_AT_name' in d['attrs']:
                self.vars[self.sval(d, 'DW_AT_name')] = d

    @staticmethod
    def sval(d, k):
        v = d['attrs'].get(k)
        if v is None:
            return None
        m = re.match(r'^"(.*)"$', v)
        return m.group(1) if m else v

    @staticmethod
    def ival(d, k):
        v = d['attrs'].get(k)
        if v is None:
            return None
        m = re.match(r'^(0x[0-9a-fA-F]+|\d+)', v)
        return int(m.group(1), 0) if m else None

    def tref(self, d):
        v = d['attrs'].get('DW_AT_type')
        if v is None:
            return None
        return self.dies[int(v.split()[0], 16)]

    def tname(self, d):
        v = d['attrs'].get('DW_AT_type')
        m = re.search(r'"(.*)"', v or '')
        return m.group(1) if m else 'void'

    def strip(self, t):
        while t is not None and t['tag'] in ('DW_TAG_typedef', 'DW_TAG_const_type', 'DW_TAG_volatile_type',
                                              'DW_TAG_restrict_type', 'DW_TAG_atomic_type'):
            t = self.tref(t)
        return t

    def size(self, t):
        t = self.strip(t)
        if t is None:
            return None
        tag = t['tag']
        if tag == 'DW_TAG_pointer_type':
            return 4
        if tag == 'DW_TAG_array_type':
            n = 1
            for sr in t['children']:
                if sr['tag'] != 'DW_TAG_subrange_type':
                    continue
                c = self.ival(sr, 'DW_AT_count')
                if c is None:
                    ub = self.ival(sr, 'DW_AT_upper_bound')
                    c = ub + 1 if ub is not None else None
                if c is None:
                    return None
                n *= c
            es = self.size(self.tref(t))
            return None if es is None else n * es
        bs = self.ival(t, 'DW_AT_byte_size')
        return bs

    def leaves(self, t, base, out, notes, path):
        """Append (offset, width, kind, path) scalar leaves of type t at `base`."""
        t = self.strip(t)
        if t is None:
            notes.append(f'{path}: void type')
            return
        tag = t['tag']
        if tag == 'DW_TAG_base_type':
            enc = t['attrs'].get('DW_AT_encoding', '')
            kind = 'float' if 'float' in enc else 'int'
            out.append((base, self.ival(t, 'DW_AT_byte_size'), kind, path))
        elif tag == 'DW_TAG_enumeration_type':
            out.append((base, self.ival(t, 'DW_AT_byte_size'), 'int', path))
        elif tag == 'DW_TAG_pointer_type':
            out.append((base, 4, PtrKind(self, self.tref(t)), path))
        elif tag == 'DW_TAG_array_type':
            dims = []
            for sr in t['children']:
                if sr['tag'] == 'DW_TAG_subrange_type':
                    c = self.ival(sr, 'DW_AT_count')
                    if c is None:
                        ub = self.ival(sr, 'DW_AT_upper_bound')
                        c = ub + 1 if ub is not None else None
                    dims.append(c)
            et = self.tref(t)
            es = self.size(et)
            n = 1
            for c in dims:
                n *= (c if c is not None else 1)
            if any(c is None for c in dims):
                notes.append(f'{path}: inner open array')
            sub = []
            self.leaves(et, 0, sub, notes, path + '[]')
            for i in range(n):
                for (o, w, k, p) in sub:
                    out.append((base + i * es + o, w, k, p))
        elif tag == 'DW_TAG_structure_type':
            for m in t['children']:
                if m['tag'] != 'DW_TAG_member':
                    continue
                mname = self.sval(m, 'DW_AT_name') or '?'
                loc = self.ival(m, 'DW_AT_data_member_location')
                bsz = self.ival(m, 'DW_AT_bit_size')
                mt = self.tref(m)
                if bsz is not None:
                    unit = self.size(mt)
                    bo = self.ival(m, 'DW_AT_data_bit_offset') or 0
                    if loc is None:
                        loc = (bo // (8 * unit)) * unit
                    leaf = (base + loc, unit, 'int', path + '.' + mname)
                    if leaf not in out:
                        out.append(leaf)
                    continue
                self.leaves(mt, base + (loc or 0), out, notes, path + '.' + mname)
        elif tag == 'DW_TAG_union_type':
            mems = [m for m in t['children'] if m['tag'] == 'DW_TAG_member']
            variants = []
            for m in mems:
                sub = []
                self.leaves(self.tref(m), base, sub, notes, path + '.' + (self.sval(m, 'DW_AT_name') or '?'))
                variants.append(sub)
            # the first member's widths (C initialisation rule); note if the members disagree
            ws = {tuple((o, w) for (o, w, k, p) in v) for v in variants}
            if len(ws) > 1:
                notes.append(f'{path}: union members with different widths, using the first')
            out.extend(variants[0] if variants else [])
        elif tag == 'DW_TAG_subroutine_type':
            notes.append(f'{path}: function type')
        else:
            notes.append(f'{path}: unhandled {tag}')

    def typedef_index(self):
        """name -> type DIE for every named typedef/struct (for symbol_addrs `type:` fallback)."""
        idx = {}
        for d in self.dies.values():
            if d['tag'] in ('DW_TAG_typedef', 'DW_TAG_structure_type', 'DW_TAG_base_type') and 'DW_AT_name' in d['attrs']:
                idx.setdefault(self.sval(d, 'DW_AT_name'), d)
        return idx


# Consumer types for data the game C declares only as an opaque `u8 x[]` (from
# port/ENDIAN_AUDIT.md section D). symbol -> (translation unit that knows the type, C type).
# They replace the u8[] declaration; open arrays `T[]` extend over the symbol's extent as usual.
TYPE_OVERRIDES = {
    'D_8003B110':      ('gfx',    'u32[20]'),        # OSViMode, bcopy'd into u32 D_800A6850[20]
    'ovl2_D_800F8E1C': ('ovl2_d', 'EffFrame *[]'),   # Item_Task
    'ovl2_D_800F8E4C': ('ovl2_d', 'EffFrame *[]'),   # Clock task
    'ovl2_D_800F922C': ('ovl2_d', 'EffFrame *[]'),   # player effects
    'ovl2_D_800F9318': ('ovl2_d', 'EffFrame *[]'),
    'ovl2_D_800F93B4': ('ovl2_d', 'EffFrame *[]'),   # Crawler
    'ovl2_D_800F9488': ('ovl2_d', 'EffFrame *[]'),   # SpinBoss_Task
    'ovl2_D_800F95D4': ('ovl2_c', 'AnimFrame *[]'),  # Snake_Task
    'ovl2_D_800F9438': ('ovl2_c', 'AnimFrame *[]'),  # Boss_Task
    'ovl2_D_800F9874': ('ovl2_c', 'AnimFrame *[]'),  # Minion_Task
    'ovl2_D_800F97B0': ('ovl2_b', 'EnemyAnims'),     # Enemy_WanderTask
    'ovl2_D_800F96DC': ('ovl2_b', 'EnemyAnims'),     # Enemy_BounceTask
}


def compile_and_probe(cache_dir):
    """Compile each src/game/*.c; return {file: (undef_syms, defined_syms, Dwarf)}."""
    res = {}
    for src in sorted(glob.glob(os.path.join(ROOT, 'src', 'game', '*.c'))):
        base = os.path.basename(src)[:-2]
        obj = os.path.join(cache_dir, base + '.o')
        r = subprocess.run([CLANG] + CFLAGS + ['-c', src, '-o', obj], capture_output=True, text=True)
        if r.returncode:
            sys.exit(f'clang failed on {src}:\n{r.stderr}')
        undef, defined = set(), set()
        for line in subprocess.run([NM, obj], capture_output=True, text=True).stdout.splitlines():
            p = line.split()
            if len(p) == 2 and p[0] == 'U':
                undef.add(p[1])
            elif len(p) == 3 and p[1] in 'TtDdBbRr' and not p[2].startswith('.'):
                defined.add((p[2], p[1]))
        probe = os.path.join(cache_dir, base + '_probe.c')
        with open(probe, 'w') as f:
            f.write(f'#include "{src}"\n')
            for s in sorted(undef):
                f.write(f'__typeof__({s}) *__vdprobe_{s} = 0;\n')
            for s, (tu, ty) in sorted(TYPE_OVERRIDES.items()):
                if tu == base:
                    f.write(f'__typeof__({ty}) *__vdprobe_ovr_{s} = 0;\n')
        pobj = os.path.join(cache_dir, base + '_probe.o')
        r = subprocess.run([CLANG] + CFLAGS + ['-c', probe, '-o', pobj], capture_output=True, text=True)
        if r.returncode:
            sys.exit(f'clang failed on probe for {src}:\n{r.stderr}')
        txt = subprocess.run([DWARFDUMP, '--debug-info', pobj], capture_output=True, text=True).stdout
        res[base] = (undef, defined, Dwarf(txt))
    return res


# ----------------------------------------------------------------------------- splat .s
LINE_RE = re.compile(r'^\s*/\*\s*([0-9A-F]+)\s+([0-9A-F]{8})(?:\s+([0-9A-F]+))?\s*\*/\s*\.(\w+)\s*(.*?)\s*$')
BSS_RE = re.compile(r'^\s*/\*\s*([0-9A-F]{8})\s*\*/\s*\.space\s+(\S+)')


def parse_data_s(path):
    """Items (vram, rom, directive, args, rawvalue) and labels [(name, vram_start, vram_end)]."""
    items, labels = [], []
    open_lbl = {}
    pending = []
    for line in open(path):
        m = LINE_RE.match(line)
        if m:
            rom, vram, raw, d, args = int(m.group(1), 16), int(m.group(2), 16), m.group(3), m.group(4), m.group(5)
            items.append((vram, rom, d, args, raw))
            for nm in pending:
                open_lbl[nm] = vram
            pending = []
            continue
        m = BSS_RE.match(line)
        if m:
            vram = int(m.group(1), 16)
            items.append((vram, None, 'space', m.group(2), None))
            for nm in pending:
                open_lbl[nm] = vram
            pending = []
            continue
        m = re.match(r'^\s*dlabel\s+(\w+)', line)
        if m:
            pending.append(m.group(1))
            continue
        m = re.match(r'^\s*enddlabel\s+(\w+)', line)
        if m and m.group(1) in open_lbl:
            labels.append([m.group(1), open_lbl.pop(m.group(1)), None])
    return items, labels


# ----------------------------------------------------------------------------- main generator
class Gen:
    def __init__(self):
        self.blocks = load_blocks()
        self.bmap = {b.name: b for b in self.blocks}
        self.syms = load_symbols()
        self.report = collections.defaultdict(list)

    def run(self):
        with tempfile.TemporaryDirectory() as td:
            self.files = compile_and_probe(td)
        self.collect_c_symbols()
        self.parse_asm()
        self.type_blocks()
        self.place_symbols()

    # --- game C symbols ------------------------------------------------------------------
    def collect_c_symbols(self):
        self.cdefined = {}           # C function name -> file
        for f, (undef, defined, dw) in self.files.items():
            for n, t in defined:
                if t in 'Tt':
                    self.cdefined[n] = f
        self.cdata = {}              # name -> list of (file, leaves, size, open_elem, typestr, notes)
        self.cfuncs = set()
        self.overridden = {}
        for f, (undef, defined, dw) in self.files.items():
            ovr = [('__vdprobe_ovr_' + s, s) for s, (tu, ty) in TYPE_OVERRIDES.items() if tu == f]
            for pname, s in [('__vdprobe_' + s, s) for s in sorted(undef)] + ovr:
                v = dw.vars.get(pname)
                if v is None:
                    self.report['probe'].append(f'{f}: no DWARF for {s}')
                    continue
                pt = dw.tref(v)                      # pointer to the symbol's type
                t = dw.tref(pt) if pt is not None else None
                st = dw.strip(t)
                if st is not None and st['tag'] == 'DW_TAG_subroutine_type':
                    self.cfuncs.add(s)
                    continue
                typestr = dw.tname(pt)[:-4] if dw.tname(pt).endswith(' (*)') else dw.tname(pt)
                leaves, notes = [], []
                size = dw.size(t)
                open_elem = None
                if st is not None and st['tag'] == 'DW_TAG_array_type' and size is None:
                    # incomplete array T x[]: one element's leaves, repeated later
                    et = dw.tref(st)
                    es = dw.size(et)
                    sub = []
                    dw.leaves(et, 0, sub, notes, s + '[]')
                    open_elem = (es, sub)
                else:
                    dw.leaves(t, 0, leaves, notes, s)
                self.cdata.setdefault(s, []).append(dict(file=f, leaves=leaves, size=size, open_elem=open_elem,
                                                         type=typestr, notes=notes,
                                                         override=pname.startswith('__vdprobe_ovr_')))
        # pick one declaration per symbol
        self.cbest = {}
        for s, decls in self.cdata.items():
            def score(d):
                ws = {w for (_, w, _, _) in (d['leaves'] or (d['open_elem'][1] if d['open_elem'] else []))}
                blob = ws <= {1} and d['type'].split('[')[0].strip() in ('u8', 's8', 'char', 'unsigned char', 'const u8')
                return (2 if d['override'] else 0 if blob else 1, d['size'] or 0)
            best = max(decls, key=score)
            self.cbest[s] = best
            if best['override']:
                self.report['override'].append(
                    f'{s}: declared ' + ' / '.join(sorted({d['type'] for d in decls if not d['override']})) +
                    f' in the game C; typed as {best["type"]} (TYPE_OVERRIDES, ENDIAN_AUDIT.md D)')
                continue
            sigs = set()
            for d in decls:
                sigs.add(d['type'])
            if len(sigs) > 1:
                self.report['decl_conflict'].append(
                    f'{s}: declared as ' + ' / '.join(sorted(f"{d['type']} ({d['file']})" for d in decls)) +
                    f' -> using {best["type"]}')
            for n in best['notes']:
                self.report['type_notes'].append(f'{s} ({best["file"]}): {n}')
        # the typedef index for symbol_addrs `type:` fallbacks
        self.tindex = {}
        for f, (_, _, dw) in self.files.items():
            for k, d in dw.typedef_index().items():
                self.tindex.setdefault(k, (dw, d))

    # --- splat output --------------------------------------------------------------------
    def parse_asm(self):
        self.items = {b.name: [] for b in self.blocks}
        self.labels = {b.name: [] for b in self.blocks}
        for p in sorted(glob.glob(os.path.join(ROOT, 'asm', 'data', '*.s'))):
            items, labels = parse_data_s(p)
            if not items:
                continue
            rom0 = int(os.path.basename(p).split('.')[0], 16)
            # data files are named by ROM offset; bss files by the ROM offset of the segment end
            if '.bss.' in p:
                blk = next(b for b in self.blocks if b.kind == 'bss' and items[0][0] in b)
            else:
                blk = next(b for b in self.blocks if b.kind == 'data' and b.rom <= rom0 < b.rom + b.size)
            self.items[blk.name].extend(items)
            self.labels[blk.name].extend(labels)
        for bn, its in self.items.items():
            its.sort()
            labs = sorted(self.labels[bn], key=lambda l: l[1])
            b = self.bmap[bn]
            for i, l in enumerate(labs):
                l[2] = labs[i + 1][1] if i + 1 < len(labs) else b.hi
            self.labels[bn] = labs

    # --- widths / relocations -------------------------------------------------------------
    def type_blocks(self):
        """Give every byte of every data block a swap width. Priority (first wins):
        relocations and code pointers (4) > C types of the externs the game C uses >
        symbol_addrs `type:` > types propagated along relocations from C pointer fields >
        explicit splat directives (.byte/.short/.float/.double/.asciz) > C byte blobs
        (`u8 x[]` / `char x[]`, limited to their splat label) > other splat `.word` (guessed)
        > padding (bytes)."""
        self.width, self.src = {}, {}
        self.relocs = {}          # block -> list of (off, target_block, target_off, sym)
        self.unused_ptr = []
        self.ctyped = {}          # block -> list of (sym, off, leaves) actually applied (for the test)
        self.ptr_fields_unrelocated = []
        self.puts = {}
        dblocks = [b for b in self.blocks if b.kind == 'data']
        for b in dblocks:
            self.width[b.name] = [0] * b.size
            self.src[b.name] = [None] * b.size
            self.relocs[b.name] = []
            self.ctyped[b.name] = []
            self.puts[b.name] = self.make_put(b)
        # (1) relocations / code pointers: `.word SYMBOL [+ n]` in the splat output
        code_ranges = [(0x80000400, 0x80033FA0), (0x800E0000, 0x800F6390)]   # main text, overlay text
        for b in dblocks:
            put = self.puts[b.name]
            self.valuewords = getattr(self, 'valuewords', {})
            for (vram, rom, d, args, raw) in self.items[b.name]:
                if d != 'word' or not re.match(r'[A-Za-z_]', args):
                    continue
                off = vram - b.lo
                val = int(raw, 16)
                sym = args.split()[0]
                tb = find_block(self.blocks, val, b.ovl, allow_end=True)
                if tb is not None and tb != 'ambiguous' and tb.name != 'ucode':
                    self.relocs[b.name].append((off, tb.name, val - tb.lo, args))
                    put(off, 4, 'reloc')
                elif sym.startswith(('L', 'func_', 'ovl')) and any(lo <= val < hi for lo, hi in code_ranges):
                    kind = 'jump table' if sym.startswith('L') else 'code pointer'
                    self.unused_ptr.append((b.name, off, val, args, kind))
                    put(off, 4, 'codeptr')
                else:
                    # splat turned a plain number into a symbol (D_80000000, D_611F0 ...)
                    self.valuewords.setdefault(b.name, []).append(off)
        # (2) C types of the externs the game C references
        self.blobs = []
        for b in dblocks:
            put = self.puts[b.name]
            S = self.src[b.name]
            csyms = []
            for s, d in self.cbest.items():
                sa = symbol_address(s, self.syms)
                if sa is None:
                    continue
                a, ovl, attrs = sa
                if find_block(self.blocks, a, ovl) is not b:
                    continue
                csyms.append((a - b.lo, s, d, attrs))
            csyms.sort()
            starts = sorted({o for o, _, _, _ in csyms})
            for (off, s, d, attrs) in csyms:
                leaves = d['leaves']
                if d['open_elem']:
                    es, sub = d['open_elem']
                    if es == 1 and all(w == 1 for (_, w, _, _) in sub):
                        self.blobs.append((b, s, off, d))      # applied later, low priority
                        continue
                    limit = self.open_limit(b, off, starts, attrs, (es, sub))
                    n = max(1, (limit - off) // es) if es else 0
                    if es and (limit - off) % es:
                        self.report['open_array'].append(
                            f'{s}: {d["type"]} element size {es} does not divide 0x{limit - off:X} (using {n})')
                    leaves = [(i * es + o, w, k, p) for i in range(n) for (o, w, k, p) in sub]
                    self.report['open_array_extent'].append(f'{s} ({d["type"]}): {n} elements, 0x{n * es:X} bytes')
                elif leaves and d['size'] and 'size' in attrs and int(attrs['size'], 0) > d['size']:
                    # struct with a variable-length trailing array (e.g. Jpeg64Huff.huffval[1]):
                    # symbol_addrs knows the real size; continue the last member's width
                    full = int(attrs['size'], 0)
                    lo, lw, lk, lp = max(leaves)
                    leaves = list(leaves) + [(o, lw, lk, lp + '+') for o in range(lo + lw, full - lw + 1, lw)]
                    self.report['tail_extend'].append(
                        f'{s} ({d["type"]}): sizeof 0x{d["size"]:X}, symbol_addrs size 0x{full:X}: '
                        f'tail continued as width {lw}')
                self.apply_leaves(b, s, off, leaves, 'C')
        # (3) symbol_addrs `type:` for data symbols the game C doesn't declare
        for b in dblocks:
            for name, (a, attrs, fn) in self.syms.items():
                if 'type' not in attrs or attrs['type'] == 'func' or name in self.cbest:
                    continue
                sa = symbol_address(name, self.syms)
                if find_block(self.blocks, a, sa[1]) is not b:
                    continue
                self.apply_named_type(b, self.puts[b.name], name, a - b.lo, attrs)
        # (4) propagate pointee types along relocations from typed pointer fields
        self.propagate()
        # (5) explicit splat directives
        for b in dblocks:
            put = self.puts[b.name]
            items = self.items[b.name]
            for i, (vram, rom, d, args, raw) in enumerate(items):
                off = vram - b.lo
                nxt = items[i + 1][0] - b.lo if i + 1 < len(items) else b.size
                if d == 'byte':
                    put(off, 1, 's.byte')
                elif d == 'short':
                    put(off, 2, 's.short')
                elif d == 'float':
                    put(off, 4, 's.float')
                elif d == 'double':
                    put(off, 8, 's.double')
                elif d == 'asciz':
                    for j in range(off, nxt):
                        put(j, 1, 's.asciz')
        # (6) C byte blobs: u8/char x[] (RSP data, strings, opaque task data), over their splat label
        for (b, s, off, d) in self.blobs:
            put = self.puts[b.name]
            lim = self.label_end(b, off)
            nb = 0
            for j in range(off, lim):
                if put(j, 1, 'C.blob'):
                    nb += 1
            self.ctyped[b.name].append((s, off, []))
            self.report['blob'].append(f'{s} ({d["type"]}) @{b.name}+0x{off:X}: 0x{lim - off:X} bytes, '
                                       f'{nb} as bytes, {lim - off - nb} already typed (pointers etc.)')
        # (7) remaining .word: default 32-bit (guessed); (8) holes: bytes
        for b in dblocks:
            put = self.puts[b.name]
            S = self.src[b.name]
            for (vram, rom, d, args, raw) in self.items[b.name]:
                if d == 'word':
                    put(vram - b.lo, 4, 'guess.word')
            for j in range(b.size):
                if S[j] is None:
                    put(j, 1, 'pad')
        # C pointer fields that hold an address but got no relocation
        for b in dblocks:
            for (s, off, applied) in self.ctyped[b.name]:
                for (o, w, k, p) in applied:
                    if k != 'ptr':
                        continue
                    if any(r[0] == o for r in self.relocs[b.name]):
                        continue
                    self.ptr_fields_unrelocated.append((b.name, o, p))

    def make_put(self, b):
        W, S = self.width[b.name], self.src[b.name]

        def put(off, w, src, why=''):
            if off < 0 or off + w > b.size:
                self.report['conflict'].append(f'{b.name}+0x{off:X}: {src} width {w} runs past the block {why}')
                return False
            if any(S[off + i] is not None for i in range(w)):
                return W[off] == w and all(W[off + i] == 0 for i in range(1, w))
            W[off] = w
            for i in range(w):
                S[off + i] = src
            return True
        return put

    def apply_leaves(self, b, s, off, leaves, src):
        put, S = self.puts[b.name], self.src[b.name]
        applied = []
        for (o, w, k, p) in leaves:
            if off + o + w > b.size:
                self.report['conflict'].append(f'{s}: leaf {p} at +0x{o:X} lies outside {b.name}')
                continue
            if not put(off + o, w, src):
                have = self.cell_width(b.name, off + o)
                if have != w:
                    self.report['conflict'].append(
                        f'{s}{p[len(s):]} @{b.name}+0x{off + o:X}: {src} width {w} ({k}), '
                        f'already {have} from {S[off + o]}')
                continue
            applied.append((off + o, w, k, p))
        self.ctyped[b.name].append((s, off, applied))
        return applied

    def propagate(self):
        """A relocation stored in a C pointer field `T *` says the target holds T(s)."""
        done = set()
        for _ in range(10):
            new = 0
            for bn in list(self.ctyped):
                for (s, off, applied) in list(self.ctyped[bn]):
                    for (o, w, k, p) in applied:
                        if k != 'ptr' or not hasattr(k, 'target'):
                            continue
                        rel = [r for r in self.relocs[bn] if r[0] == o]
                        if not rel:
                            continue
                        _, tbn, toff, sym = rel[0]
                        tb = self.bmap[tbn]
                        if tb.kind != 'data' or (tbn, toff) in done:
                            continue
                        dw, die = k.target
                        st = dw.strip(die)
                        if st is None or st['tag'] in ('DW_TAG_subroutine_type',):
                            continue
                        es = dw.size(die)
                        if not es:
                            continue
                        sub, notes = [], []
                        dw.leaves(die, 0, sub, notes, f'*({p})')
                        if all(x[1] == 1 for x in sub):
                            continue                 # u8 * / char *: no information
                        done.add((tbn, toff))
                        lim = self.label_end(tb, toff)
                        n = max(1, (lim - toff) // es)
                        leaves = [(i * es + lo, lw, lk, lp) for i in range(n) for (lo, lw, lk, lp) in sub]
                        nm = f'{self.label_at(tbn, toff)}(via {p})'
                        self.apply_leaves(tb, nm, toff, leaves, 'C.ptr')
                        new += 1
            if not new:
                break

    def cell_width(self, bn, off):
        W = self.width[bn]
        j = off
        while j > 0 and W[j] == 0:
            j -= 1
        return W[j] if j + W[j] > off else None

    def label_end(self, b, off):
        """End (block offset) of the splat label containing `off`, capped at section starts."""
        end = b.size
        for (n, lo, hi) in self.labels[b.name]:
            if lo - b.lo <= off < hi - b.lo:
                end = hi - b.lo
                break
        for st in b.stops:
            if off < st < end:
                end = st
        return end

    def open_limit(self, b, off, starts, attrs, elem=None):
        """Extent of an incomplete array: symbol_addrs size, else up to the next C-declared
        symbol, splat label that is a relocation target, or .data/.rodata section start."""
        if 'size' in attrs:
            return off + int(attrs['size'], 0)
        nxt = [s for s in starts if s > off]
        lim = nxt[0] if nxt else b.size
        tgts = {r[2] for bl in self.relocs.values() for r in bl if r[1] == b.name}
        for (name, lo, hi) in self.labels[b.name]:
            o = lo - b.lo
            if off < o < lim and o in tgts:
                lim = o
        for st in b.stops:
            if off < st < lim:
                lim = st
        if elem is not None:
            # do not run into a following splat label whose explicit directives disagree with
            # the element layout (e.g. a Gfx list followed by a .short table)
            es, sub = elem
            wmap = {o: w for (o, w, _, _) in sub}
            first = self.label_end(b, off)
            for (name, lo, hi) in self.labels[b.name]:
                o = lo - b.lo
                if o < first or o >= lim:
                    continue
                bad = False
                for (vram, rom, d, args, raw) in self.items_by_label(b, lo, hi):
                    ew = {'byte': 1, 'short': 2, 'double': 8, 'asciz': 1}.get(d)
                    if ew is None:
                        continue
                    if wmap.get((vram - b.lo - off) % es) != ew:
                        bad = True
                        break
                if bad:
                    lim = o
                    break
        return lim

    def items_by_label(self, b, lo, hi):
        return [it for it in self.items[b.name] if lo <= it[0] < hi]

    def apply_named_type(self, b, put, name, off, attrs):
        tn = attrs['type'].rstrip('*')
        size = int(attrs['size'], 0) if 'size' in attrs else None
        if attrs['type'].endswith('*'):
            w, n = 4, (size // 4 if size else 1)
            for i in range(n):
                put(off + 4 * i, 4, 'symtype')
            return
        prim = {'u8': 1, 's8': 1, 'char': 1, 'u16': 2, 's16': 2, 'u32': 4, 's32': 4, 'f32': 4,
                'f64': 8, 'u64': 8, 's64': 8}
        if tn in prim:
            w = prim[tn]
            n = (size // w) if size else 1
            for i in range(n):
                put(off + w * i, w, 'symtype')
            return
        if tn in self.tindex:
            dw, d = self.tindex[tn]
            es = dw.size(d)
            sub, notes = [], []
            dw.leaves(d, 0, sub, notes, name)
            n = (size // es) if (size and es) else 1
            for i in range(n):
                for (o, w, k, p) in sub:
                    put(off + i * es + o, w, 'symtype')
            return
        self.report['symtype_unknown'].append(f'{name}: type {attrs["type"]} unknown')

    # --- linker symbols --------------------------------------------------------------------
    def place_symbols(self):
        self.placed = {}          # name -> (block, offset)
        self.absolute = {}        # name -> value
        self.unplaced = []
        wanted = set()
        for f, (undef, defined, dw) in self.files.items():
            wanted |= {s for s in undef if s not in self.cfuncs}
        wanted |= {'gGameHeap', 'gFrameBuffer0', 'gFrameBuffer1', 'gZBuffer', 'gTaskPoolMem'}
        for s in sorted(wanted):
            sa = symbol_address(s, self.syms)
            if sa is None:
                self.unplaced.append((s, 'no address known'))
                continue
            a, ovl, attrs = sa
            if a < 0x80000000:
                self.absolute[s] = a            # address constants (ROM offsets)
                continue
            b = find_block(self.blocks, a, ovl, allow_end=True)
            if b is None:
                self.unplaced.append((s, f'0x{a:08X} is in no block'))
            elif b == 'ambiguous':
                self.unplaced.append((s, f'0x{a:08X}: overlay unknown'))
            else:
                self.placed[s] = (b.name, a - b.lo)
        # function call names that differ from the C definitions
        self.func_alias = {}
        self.func_missing = []
        by_addr = collections.defaultdict(list)
        for n, (a, attrs, fn) in self.syms.items():
            if attrs.get('type') == 'func' and n in self.cdefined:
                seg = attrs.get('segment', '')
                by_addr[(a, int(seg[3:]) if seg.startswith('ovl') else None)].append(n)
        for s in sorted(self.cfuncs):
            if s in self.cdefined:
                continue
            sa = symbol_address(s, self.syms)
            tgt = None
            if sa:
                a, ovl, _ = sa
                c = by_addr.get((a, ovl)) or (by_addr.get((a, None)) if ovl is None else None)
                if c:
                    tgt = c[0]
            if tgt:
                self.func_alias[s] = tgt
            else:
                self.func_missing.append(s)

    # --- output ------------------------------------------------------------------------------
    def runs(self, bn):
        W, out = self.width[bn], []
        j, n = 0, len(W)
        while j < n:
            w = W[j]
            assert w, (bn, j)
            k = j
            cnt = 0
            while k < n and W[k] == w and cnt < 0xFFFF:
                k += w
                cnt += 1
            out.append((j, cnt, w))
            j = k
        return out

    def write(self):
        os.makedirs(GEN, exist_ok=True)
        hdr = ['/* Generated by tools/port/gen_dataload.py - do not edit. Offsets and sizes only. */',
               '#ifndef VD_BLOCKS_H', '#define VD_BLOCKS_H', '',
               '/* Memory blocks of the port (see port/ARCHITECTURE.md section 2).',
               ' * kind: DATA = filled from the ROM at boot, BSS/ZERO = zero-filled. */',
               'enum {']
        for i, b in enumerate(self.blocks):
            hdr.append(f'    VD_BLK_{b.name.upper()} = {i},')
        hdr += [f'    VD_BLK_COUNT = {len(self.blocks)}', '};', '']
        for b in self.blocks:
            u = b.name.upper()
            hdr.append(f'#define VD_BLK_{u}_N64     0x{b.lo:08X}u  /* .. 0x{b.hi:08X}{"  " + b.note if b.note else ""} */')
            hdr.append(f'#define VD_BLK_{u}_SIZE    0x{b.size:X}u')
            hdr.append(f'#define VD_BLK_{u}_ALIGN   {b.align}')
            if b.rom is not None:
                hdr.append(f'#define VD_BLK_{u}_ROM     0x{b.rom:X}u')
        hdr += ['', '/* X-macro: X(name, id, size, align, n64_base, is_data, overlay or -1) */', '#define VD_BLOCK_LIST(X) \\']
        for i, b in enumerate(self.blocks):
            hdr.append(f'    X({b.name}, {i}, 0x{b.size:X}u, {b.align}, 0x{b.lo:08X}u, {1 if b.kind == "data" else 0}, '
                       f'{-1 if b.ovl is None else b.ovl}) \\')
        hdr += ['', '#endif', '']
        open(os.path.join(GEN, 'vd_blocks.h'), 'w').write('\n'.join(hdr))

        for fname, pfx in (('vd_blocks.ld', ''), ('vd_blocks_sh.ld', '_')):
            L = ['/* Generated by tools/port/gen_dataload.py - do not edit.',
                 ' * Every data/BSS symbol the game C references, as block base + offset.',
                 f' * Block storage (vd_blk_<name>) is defined in port/sys/dataload.c.'
                 f'{" Symbol prefix: _ (sh-elf GCC)." if pfx else ""} */', '']
            for s, (bn, off) in sorted(self.placed.items(), key=lambda kv: (kv[1][0], kv[1][1], kv[0])):
                L.append(f'{pfx}{s} = {pfx}vd_blk_{bn} + 0x{off:X};')
            L.append('')
            L.append('/* address constants (ROM offsets), not memory */')
            for s, v in sorted(self.absolute.items()):
                L.append(f'{pfx}{s} = 0x{v:X};')
            open(os.path.join(GEN, fname), 'w').write('\n'.join(L) + '\n')
        for fname, pfx in (('vd_func_aliases.ld', ''), ('vd_func_aliases_sh.ld', '_')):
            L = ['/* Generated by tools/port/gen_dataload.py: call names used by the game C for',
                 ' * functions that the C defines under another name. */']
            for s, t in sorted(self.func_alias.items()):
                L.append(f'{pfx}{s} = {pfx}{t};')
            open(os.path.join(GEN, fname), 'w').write('\n'.join(L) + '\n')

        T = ['/* Generated by tools/port/gen_dataload.py - do not edit.',
             ' * Boot-time data loader tables: ROM offsets, swap runs and relocations only;',
             ' * no ROM bytes. Applied by port/sys/dataload.c. */',
             '#include "dataload.h"', '']
        datab = [b for b in self.blocks if b.kind == 'data']
        for b in datab:
            rs = self.runs(b.name)
            T.append(f'static const vd_swap_run swap_{b.name}[{len(rs)}] = {{')
            line = []
            for (o, c, w) in rs:
                line.append(f'{{0x{o:X},{c},{w},0}}')
                if len(line) == 8:
                    T.append('    ' + ','.join(line) + ',')
                    line = []
            if line:
                T.append('    ' + ','.join(line) + ',')
            T.append('};')
            rl = sorted(self.relocs[b.name])
            if rl:
                T.append(f'static const vd_reloc reloc_{b.name}[{len(rl)}] = {{')
                line = []
                for (o, tb, to, sym) in rl:
                    line.append(f'{{0x{o:X},VD_RT(VD_BLK_{tb.upper()},0x{to:X})}}')
                    if len(line) == 6:
                        T.append('    ' + ','.join(line) + ',')
                        line = []
                if line:
                    T.append('    ' + ','.join(line) + ',')
                T.append('};')
            T.append('')
        T.append(f'const vd_data_block vd_data_blocks[{len(datab)}] = {{')
        for b in datab:
            rl = self.relocs[b.name]
            T.append(f'    {{ VD_BLK_{b.name.upper()}, 0x{b.rom:X}u, 0x{b.size:X}u, swap_{b.name}, '
                     f'{len(self.runs(b.name))}, {"reloc_" + b.name if rl else "0"}, {len(rl)} }},')
        T.append('};')
        T.append(f'const unsigned vd_data_block_count = {len(datab)};')
        T.append('')
        open(os.path.join(GEN, 'vd_dataload_tables.c'), 'w').write('\n'.join(T))

        J = {'blocks': [dict(name=b.name, lo=b.lo, hi=b.hi, kind=b.kind, rom=b.rom, ovl=b.ovl) for b in self.blocks],
             'ctyped': {bn: [dict(sym=s, off=o, leaves=[(lo, w, k) for (lo, w, k, p) in lv]) for (s, o, lv) in v]
                        for bn, v in self.ctyped.items()},
             'relocs': {bn: [dict(off=o, block=tb, toff=to, sym=sym) for (o, tb, to, sym) in v]
                        for bn, v in self.relocs.items()},
             'unused_ptr': [dict(block=a, off=b_, value=c, sym=d, kind=e) for (a, b_, c, d, e) in self.unused_ptr],
             'placed': {s: list(v) for s, v in self.placed.items()},
             'absolute': self.absolute,
             'unplaced': self.unplaced,
             'guessed': {bn: self.ranges_with_src(bn, 'guess.word') for bn in self.src},
             'c_data_refs': sorted(set(self.cbest)),
             'func_missing': self.func_missing}
        json.dump(J, open(os.path.join(GEN, 'vd_dataload_types.json'), 'w'), indent=0)
        self.write_report()

    def write_report(self):
        R = ['Data loader generation report (tools/port/gen_dataload.py)', '=' * 60, '']
        R.append('Blocks:')
        for b in self.blocks:
            R.append(f'  {b.name:10s} 0x{b.lo:08X}-0x{b.hi:08X} size 0x{b.size:06X} {b.kind:5s} '
                     f'{"rom 0x%X" % b.rom if b.rom is not None else ""} {b.note}')
        R.append('')
        for b in self.blocks:
            if b.kind != 'data':
                continue
            S = self.src[b.name]
            cnt = collections.Counter(S)
            R.append(f'{b.name}: width sources (bytes): ' + ', '.join(f'{k}={v}' for k, v in sorted(cnt.items())))
        R.append('')
        R.append('Guessed ranges (splat `.word` with no C type, swapped as 32-bit words).')
        R.append('"zero" = every word is 0 in the ROM, so the width does not matter.')
        rom = self.rom
        for b in self.blocks:
            if b.kind != 'data':
                continue
            S = self.src[b.name]
            j = 0
            while j < b.size:
                if S[j] != 'guess.word':
                    j += 1
                    continue
                k = j
                while k < b.size and S[k] == 'guess.word':
                    k += 1
                lbls = [l[0] for l in self.labels[b.name] if l[1] - b.lo < k and l[2] - b.lo > j]
                z = ''
                if rom is not None:
                    z = ' zero' if not any(rom[b.rom + j:b.rom + k]) else ''
                R.append(f'  {b.name}+0x{j:04X}..0x{k:04X} (0x{k - j:X} bytes, N64 0x{b.lo + j:08X}){z}  '
                         f'{" ".join(lbls[:6])}{" ..." if len(lbls) > 6 else ""}')
                j = k
        R.append('')
        R.append('Code pointers in data (not relocated; the port does not use these tables):')
        for (bn, off, val, sym, kind) in self.unused_ptr:
            lbl = self.label_at(bn, off)
            R.append(f'  {bn}+0x{off:04X} = 0x{val:08X} ({sym}; {kind}) in {lbl}')
        R.append('')
        R.append('C pointer fields with no relocation whose value looks like an address:')
        for (bn, off, p) in self.ptr_fields_unrelocated:
            v = None
            if rom is not None:
                b = self.bmap[bn]
                v = int.from_bytes(rom[b.rom + off:b.rom + off + 4], 'big')
                if find_block(self.blocks, v, b.ovl) is None and not (0x80000400 <= v < 0x80038150):
                    continue        # NULL, or a number (ROM offset, Gfx word) in a void * field
            R.append(f'  {bn}+0x{off:04X} {p}' + (' (ROM not checked)' if v is None else ''))
        R.append('')
        R.append('Raw words that look like addresses inside a block but are not relocations:')
        for line in self.raw_pointer_candidates():
            R.append('  ' + line)
        R.append('')
        R.append('Data symbols referenced by the game C that could not be placed:')
        for s, why in self.unplaced:
            R.append(f'  {s}: {why}')
        if not self.unplaced:
            R.append('  (none)')
        R.append('')
        R.append('Functions the game C calls that it does not define and that have no C alias')
        R.append('(libultra / assembly routines the port layer must provide):')
        R.append('  ' + ' '.join(self.func_missing))
        R.append('')
        for k in ('override', 'decl_conflict', 'conflict', 'blob', 'tail_extend', 'type_notes', 'open_array', 'open_array_extent', 'symtype_unknown', 'probe'):
            R.append(f'[{k}] ({len(self.report[k])})')
            for x in self.report[k]:
                R.append('  ' + x)
            R.append('')
        open(os.path.join(GEN, 'dataload_report.txt'), 'w').write('\n'.join(R) + '\n')

    def ranges_with_src(self, bn, src):
        S, out, j = self.src[bn], [], 0
        while j < len(S):
            if S[j] != src:
                j += 1
                continue
            k = j
            while k < len(S) and S[k] == src:
                k += 1
            out.append([j, k])
            j = k
        return out

    def label_at(self, bn, off):
        b = self.bmap[bn]
        for (n, lo, hi) in self.labels[bn]:
            if lo - b.lo <= off < hi - b.lo:
                return n
        return '?'

    def raw_pointer_candidates(self):
        out = []
        for b in self.blocks:
            if b.kind != 'data':
                continue
            for (vram, rom, d, args, raw) in self.items[b.name]:
                if d != 'word' or not args.startswith('0x'):
                    continue
                v = int(args, 16)
                tb = find_block(self.blocks, v, b.ovl, allow_end=True)
                if tb and tb != 'ambiguous' and tb.name != 'ucode':
                    out.append(f'{b.name}+0x{vram - b.lo:04X} = 0x{v:08X} (would be {tb.name}+0x{v - tb.lo:X}) '
                               f'in {self.label_at(b.name, vram - b.lo)}')
                elif 0x80000000 <= v < 0x80800000 and v not in (0x80008000, 0x80000040):
                    out.append(f'{b.name}+0x{vram - b.lo:04X} = 0x{v:08X} (code or outside blocks) '
                               f'in {self.label_at(b.name, vram - b.lo)}')
        return out


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('--rom', default=os.path.join(ROOT, 'baserom.z64'),
                    help='ROM, only read to mark all-zero guessed ranges in the report')
    args = ap.parse_args()
    g = Gen()
    g.rom = open(args.rom, 'rb').read() if os.path.exists(args.rom) else None
    g.run()
    g.write()
    nrel = sum(len(v) for v in g.relocs.values())
    print(f'wrote {GEN}: {len(g.placed)} symbols placed, {len(g.absolute)} absolute, {len(g.unplaced)} unplaced, '
          f'{nrel} relocations, {len(g.unused_ptr)} unused code pointers, {len(g.func_alias)} function aliases')
    for b in g.blocks:
        if b.kind == 'data':
            c = collections.Counter(g.src[b.name])
            print(f'  {b.name}: 0x{b.size:X} bytes, {len(g.runs(b.name))} swap runs, {len(g.relocs[b.name])} relocs, '
                  f'guessed {c.get("guess.word", 0)} bytes')


if __name__ == '__main__':
    main()
