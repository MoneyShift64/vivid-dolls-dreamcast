#!/usr/bin/env python3
"""List and extract the Vivid Dolls asset files.

    python3 tools/assets.py list              -> writes docs/assets.csv
    python3 tools/assets.py sprites           -> writes extracted/sprites/*.png  (needs Pillow)

Photos ("jpeg64") are listed but left in their original compressed form; the
port decodes them at run time with port/vd_jpeg64.c, the same way the game does.
"""
import csv, os, struct, sys

ROM_PATH = 'baserom.z64'
START, END = 0x611F0, 0x23AFE0


def lzss(d, off):
    size = struct.unpack('<I', d[off:off + 4])[0]
    p = off + 4; out = bytearray(); flags = bits = 0
    while len(out) < size:
        if bits == 0:
            flags = d[p]; p += 1; bits = 8
        bits -= 1
        if flags & 0x80:
            b1, b2 = d[p], d[p + 1]; p += 2
            n = (b1 >> 4) + 3; dist = ((b1 & 0xF) << 8 | b2) + 1
            for _ in range(n):
                out.append(out[-dist])
        else:
            out.append(d[p]); p += 1
        flags = (flags << 1) & 0xFF
    return bytes(out[:size])


def assets(rom):
    """Yield (index, rom_offset, compressed, data) for every asset."""
    i = 0
    for o in range(START, END, 4):
        if rom[o:o + 4] == b'Siff':
            n = struct.unpack('>I', rom[o + 4:o + 8])[0]
            yield i, o, False, rom[o:o + 8 + n]; i += 1
        elif rom[o + 4] < 0x10 and rom[o + 5:o + 9] == b'Siff':
            yield i, o, True, lzss(rom, o); i += 1


def chunks(b, o=0, end=None, out=None):
    out = {} if out is None else out
    end = len(b) if end is None else end
    while o + 8 <= end:
        t = b[o:o + 4]; n = struct.unpack('>I', b[o + 4:o + 8])[0]
        out.setdefault(t, b[o + 8:o + 8 + n])
        if t in (b'Siff', b'Sp3d'):
            chunks(b, o + 8, o + 8 + n, out)
        o += 8 + n
    return out


def sprite_quads(c):
    info = struct.unpack('>12h', c[b'info'][:24]); tw, th = info[1], info[2]
    dl = c[b'dlst']; vtx = c.get(b'vrtx')
    pts = [struct.unpack('>hh', vtx[k:k + 4]) for k in range(0, len(vtx), 4)] if vtx else None
    quads = []; base = tex = i = 0
    while i + 2 <= len(dl):
        op = dl[i]
        if pts is None:                       # mode 4: direct coordinates
            if op == 0x80:
                x, y = struct.unpack('>HH', dl[i + 2:i + 6])
                quads.append((x - 0x3E00, -(y - 0x0E00), dl[i + 1])); i += 6; continue
            if op == 0xC0: break
            i += 2; continue
        if op == 0x40: base = struct.unpack('>H', dl[i + 2:i + 4])[0] // 4; i += 4
        elif op == 0x80: tex = dl[i + 1]; i += 2
        elif op == 0xC0: break
        else:
            a, b = struct.unpack('>HH', dl[i:i + 4])
            m = min((a >> 8) & 15, (a >> 4) & 15, a & 15, (b >> 8) & 15, (b >> 4) & 15, b & 15)
            x, y = pts[base + m // 4]; quads.append((x, y, tex)); i += 4
    return tw, th, quads


def render_sprite(c):
    from PIL import Image
    tw, th, quads = sprite_quads(c)
    pal = []
    for k in range(256):
        v = struct.unpack('>H', c[b'pal8'][2 * k:2 * k + 2])[0]
        r, g, b = (v >> 11) & 31, (v >> 6) & 31, (v >> 1) & 31
        pal.append(((r << 3) | (r >> 2), (g << 3) | (g >> 2), (b << 3) | (b >> 2), 255 if v & 1 else 0))
    xs = [x for x, y, t in quads]; ys = [-y for x, y, t in quads]
    x0, y0 = min(xs), min(ys)
    img = Image.new('RGBA', (max(xs) - x0 + tw, max(ys) - y0 + th), (0, 0, 0, 0))
    tiles = c[b'tile']
    for x, y, t in quads:
        tb = tiles[t * tw * th:(t + 1) * tw * th]
        ti = Image.new('RGBA', (tw, th)); ti.putdata([pal[p] for p in tb])
        img.alpha_composite(ti, (x - x0, -y - y0))
    return img


def main():
    cmd = sys.argv[1] if len(sys.argv) > 1 else 'list'
    rom = open(ROM_PATH, 'rb').read()
    if cmd == 'list':
        os.makedirs('docs', exist_ok=True)
        with open('docs/assets.csv', 'w', newline='') as f:
            w = csv.writer(f)
            w.writerow(['index', 'rom_offset', 'game_pointer', 'compressed', 'kind', 'size', 'details'])
            for i, o, comp, d in assets(rom):
                c = chunks(d)
                if b'Sp3d' in c:
                    tw, th, q = sprite_quads(c)
                    kind, det = 'sprite', f'{len(q)} tiles of {tw}x{th}'
                else:
                    xw = struct.unpack('>I', c[b'xw  '])[0]; yw = struct.unpack('>I', c[b'yw  '])[0]
                    kind, det = 'photo (jpeg64)', f'{xw}x{yw}'
                w.writerow([i, f'0x{o:06X}', f'0x{o - START:06X}', comp, kind, len(d), det])
        print('wrote docs/assets.csv')
    elif cmd == 'sprites':
        os.makedirs('extracted/sprites', exist_ok=True)
        n = 0
        for i, o, comp, d in assets(rom):
            c = chunks(d)
            if b'Sp3d' in c:
                render_sprite(c).save(f'extracted/sprites/sprite_{i:03d}_{o:06X}.png'); n += 1
        print(f'wrote {n} sprites to extracted/sprites/')
    else:
        print(__doc__)


if __name__ == '__main__':
    main()
