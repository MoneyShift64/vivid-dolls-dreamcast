#!/usr/bin/env python3
"""Headless Dreamcast runner (Flycast libretro core + libharness.so).

Build first: tools/port/dc_harness/build.sh. Usage (see port/notes/boot.md):
  python3 -I dcrun.py DISC.cdi [--elf vividdolls.elf] [--frames N] [--input SCRIPT]
         [--watch SYM[:fmt],...] [--every K] [--save-frame F,...] [--serial FILE]

--input SCRIPT : comma list "FRAME:BUTTONS[:DURATION]" e.g. "400:X:6,500:START:6"
                 buttons: A B X Y START UP DOWN LEFT RIGHT L R, joined with +
--watch        : ELF symbols (game globals) printed every --every frames
                 fmt: u8 u16 s16 u32 s32 f32 (default u32), or SYM@N for N u32 words
--save-frame   : frame numbers to save as PPM (ONLY use for frames that cannot show a
                 photo, e.g. the boot logo). Every other frame is checked by hash /
                 pixel statistics only (--stats).
"""
import argparse
import ctypes
import hashlib
import os
import struct
import subprocess
import sys

HERE = os.path.dirname(os.path.abspath(__file__))
# where build.sh put the Flycast core and libharness.so (outside the project tree),
# and where logs / saved frames go
EMU = os.environ.get("DC_HARNESS_DIR", os.path.expanduser("~/vd-emu"))
CORE = os.environ.get("FLYCAST_CORE", os.path.join(EMU, "flycast/build-lr/flycast_libretro.so"))
LIB = os.environ.get("HARNESS_LIB", os.path.join(EMU, "harness/libharness.so"))
OUT = os.environ.get("DC_HARNESS_OUT", os.path.join(EMU, "harness/out"))
NM = "/opt/toolchains/dc/sh-elf/bin/sh-elf-nm"

# RetroPad ids -> Flycast DC mapping (libretro.cpp): B->A, A->B, Y->X, X->Y
RP = {"B": 0, "Y": 1, "SELECT": 2, "START": 3, "UP": 4, "DOWN": 5, "LEFT": 6, "RIGHT": 7,
      "A": 8, "X": 9, "L": 10, "R": 11}
DC2RP = {"A": "B", "B": "A", "X": "Y", "Y": "X"}


def dc_mask(names):
    m = 0
    for n in names:
        n = n.upper()
        n = DC2RP.get(n, n)
        m |= 1 << RP[n]
    return m


class DC:
    def __init__(self, disc, log=os.path.join(OUT, "harness.log"), serial=None, interp=False, options=None,
                 workdir=None):
        os.makedirs(os.path.dirname(os.path.abspath(log)), exist_ok=True)
        if serial:
            if os.path.exists(serial):
                os.remove(serial)
            os.environ["FLYCAST_SERIAL"] = os.path.abspath(serial)
        if interp:
            os.environ["FLYCAST_INTERP"] = "1"
        self.h = ctypes.CDLL(LIB)
        h = self.h
        h.h_ram.restype = ctypes.c_void_p
        h.h_ram_size.restype = ctypes.c_size_t
        h.h_frames.restype = ctypes.c_ulong
        h.h_presented.restype = ctypes.c_ulong
        workdir = workdir or os.path.join(os.path.dirname(LIB), "work")
        for d in ("system", "save"):
            os.makedirs(os.path.join(workdir, d), exist_ok=True)
        if h.h_init(CORE.encode(), os.path.join(workdir, "system").encode(),
                    os.path.join(workdir, "save").encode(), log.encode()) != 0:
            raise RuntimeError("h_init failed (see %s)" % log)
        opts = {
            "reicast_hle_bios": "enabled",
            "reicast_threaded_rendering": "disabled",
            "reicast_internal_resolution": "640x480",
            "reicast_cable_type": "VGA",
            "reicast_boot_to_bios": "disabled",
            "reicast_enable_dsp": "disabled",
            "reicast_gdrom_fast_loading": "enabled",
        }
        if options:
            opts.update(options)
        for k, v in opts.items():
            h.h_set_option(k.encode(), v.encode())
        if h.h_load(os.path.abspath(disc).encode()) != 0:
            raise RuntimeError("h_load failed (see %s)" % log)
        self.ram_ptr = h.h_ram()
        self.ram_size = h.h_ram_size()
        self.syms = {}
        self.frame = 0

    # ---- symbols ----
    def load_syms(self, elf):
        out = subprocess.run([NM, elf], capture_output=True, text=True).stdout
        for line in out.splitlines():
            p = line.split()
            if len(p) == 3:
                name = p[2][1:] if p[2].startswith("_") else p[2]
                self.syms[name] = int(p[0], 16)

    def addr(self, s):
        if isinstance(s, int):
            return s
        if s.startswith("0x"):
            return int(s, 16)
        if "+" in s:
            a, b = s.split("+", 1)
            return self.addr(a) + int(b, 0)
        return self.syms[s]

    # ---- memory ----
    def read(self, addr, n):
        off = addr & 0x00FFFFFF
        if off + n > self.ram_size:
            return b"\0" * n
        return ctypes.string_at(self.ram_ptr + off, n)

    def u32(self, a): return struct.unpack("<I", self.read(self.addr(a), 4))[0]
    def s32(self, a): return struct.unpack("<i", self.read(self.addr(a), 4))[0]
    def u16(self, a): return struct.unpack("<H", self.read(self.addr(a), 2))[0]
    def s16(self, a): return struct.unpack("<h", self.read(self.addr(a), 2))[0]
    def u8(self, a): return self.read(self.addr(a), 1)[0]
    def f32(self, a): return struct.unpack("<f", self.read(self.addr(a), 4))[0]

    def write(self, addr, data):
        off = addr & 0x00FFFFFF
        ctypes.memmove(self.ram_ptr + off, data, len(data))

    def cstr(self, a, n=128):
        b = self.read(self.addr(a), n)
        return b.split(b"\0")[0].decode("latin-1")

    # ---- port debug ring (port/sys/vd_debug.c, DEBUG builds) ----
    def dbg_new(self):
        """Return the debug-log text written since the last call ('' if not a DEBUG build)."""
        if "vd_dbg_head" not in self.syms:
            return ""
        head = self.u32("vd_dbg_head")
        size = 0x10000
        last = getattr(self, "_dbg_last", 0)
        if head - last > size:
            last = head - size
        ring = self.read(self.syms["vd_dbg_ring"], size)
        out = bytearray()
        for i in range(last, head):
            out.append(ring[i % size])
        self._dbg_last = head
        return out.decode("latin-1")

    # ---- cpu ----
    def regs(self):
        buf = (ctypes.c_uint32 * 64)()
        self.h.h_regs(buf)
        r = list(buf)
        return {"r": r[0:16], "pc": r[16], "pr": r[17], "sr": r[18], "gbr": r[19], "vbr": r[20],
                "spc": r[21], "ssr": r[22], "fpscr": r[23], "macl": r[24], "mach": r[25]}

    def sym_at(self, a):
        best, bn = None, None
        for n, v in self.syms.items():
            if v <= a and (best is None or v > best) and a - v < 0x10000:
                best, bn = v, n
        return "%s+0x%x" % (bn, a - best) if bn else "?"

    def regs_str(self):
        g = self.regs()
        s = "pc=%08x (%s) pr=%08x (%s) sr=%08x\n" % (g["pc"], self.sym_at(g["pc"]), g["pr"],
                                                    self.sym_at(g["pr"]), g["sr"])
        s += " ".join("r%d=%08x" % (i, v) for i, v in enumerate(g["r"]))
        return s

    # ---- PC-sampling profiler ----
    def prof_start(self, period=20000):
        self.h.h_prof(period)

    def prof_stop(self):
        """Stop sampling; returns a list of (pc, pr) samples."""
        self.h.h_prof(0)
        buf = (ctypes.c_uint32 * 8000000)()
        n = self.h.h_prof_read(buf, 8000000)
        v = list(buf[:max(n, 0)])
        return list(zip(v[0::2], v[1::2]))

    def func_of(self, a):
        if not hasattr(self, "_sorted"):
            import bisect
            items = sorted((v, k) for k, v in self.syms.items() if 0x8c010000 <= v < 0x8d000000)
            self._sorted = ([v for v, k in items], [k for v, k in items])
        import bisect
        addrs, names = self._sorted
        i = bisect.bisect_right(addrs, a) - 1
        return names[i] if i >= 0 else "?"

    def prof_report(self, samples, top=30):
        from collections import Counter
        c = Counter(self.func_of(pc) for pc, pr in samples)
        n = max(len(samples), 1)
        return "\n".join("%6.2f%% %s" % (100.0 * k / n, f) for f, k in c.most_common(top))

    # ---- run / input ----
    def set_pad(self, port, names):
        self.h.h_set_input(port, dc_mask(names))

    def set_trig(self, port, l, r):
        """Analog L/R triggers, 0..0x7FFF (Dreamcast reads them as 0..255)."""
        self.h.h_set_trig(port, int(l), int(r))

    def run(self, n=1):
        self.h.h_run(n)
        self.frame += n

    # ---- frames ----
    def frame_rgb(self):
        w, h = ctypes.c_int(), ctypes.c_int()
        size = 1280 * 960 * 3
        buf = ctypes.create_string_buffer(size)
        if self.h.h_read_frame(buf, size, ctypes.byref(w), ctypes.byref(h)) != 0:
            return None, 0, 0
        return buf.raw[: w.value * h.value * 3], w.value, h.value

    def frame_stats(self):
        """Automated check only: (sha1-prefix, non-black pixel count, w, h). Pixels never leave memory."""
        rgb, w, h = self.frame_rgb()
        if rgb is None:
            return None
        t = bytes(1 if i >= 8 else 0 for i in range(256))
        n = w * h
        r = int.from_bytes(rgb[0::3].translate(t), "little")
        g = int.from_bytes(rgb[1::3].translate(t), "little")
        b = int.from_bytes(rgb[2::3].translate(t), "little")
        cnt = (r | g | b).to_bytes(n, "little").count(1)
        return hashlib.sha1(rgb).hexdigest()[:12], cnt, w, h

    def save_ppm(self, path):
        rgb, w, h = self.frame_rgb()
        if rgb is None:
            return False
        with open(path, "wb") as f:
            f.write(b"P6\n%d %d\n255\n" % (w, h))
            f.write(rgb)
        return True


def parse_input(spec):
    ev = []
    if not spec:
        return ev
    for item in spec.split(","):
        p = item.split(":")
        f = int(p[0])
        btn = p[1].split("+")
        dur = int(p[2]) if len(p) > 2 else 6
        ev.append((f, btn, dur))
    return ev


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("disc")
    ap.add_argument("--elf", default=os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "..", "..", "build", "dc", "vividdolls.elf"))
    ap.add_argument("--frames", type=int, default=600)
    ap.add_argument("--input", default="")
    ap.add_argument("--watch", default="")
    ap.add_argument("--every", type=int, default=60)
    ap.add_argument("--stats", action="store_true", help="print frame hash / non-black count every --every frames")
    ap.add_argument("--save-frame", default="", help="frames to save as PPM (boot logo only!)")
    ap.add_argument("--outdir", default=OUT)
    ap.add_argument("--serial", default=os.path.join(OUT, "serial.log"))
    ap.add_argument("--log", default=os.path.join(OUT, "harness.log"))
    ap.add_argument("--interp", action="store_true", help="SH4 interpreter instead of the dynarec")
    ap.add_argument("--regs", action="store_true", help="print SH4 registers every --every frames")
    ap.add_argument("--dbg", default=os.path.join(OUT, "port_debug.log"),
                    help="file for the port's debug ring (DEBUG builds)")
    args = ap.parse_args()
    dbgf = open(args.dbg, "w") if args.dbg else None
    os.makedirs(args.outdir, exist_ok=True)

    dc = DC(args.disc, log=args.log, serial=args.serial, interp=args.interp)
    if os.path.exists(args.elf):
        dc.load_syms(args.elf)
    events = parse_input(args.input)
    saves = set(int(x) for x in args.save_frame.split(",") if x)
    watch = [w for w in args.watch.split(",") if w]
    held = {}
    for f in range(1, args.frames + 1):
        for (ef, btn, dur) in events:
            if ef == f:
                held[tuple(btn)] = f + dur
        names = []
        for b, until in list(held.items()):
            if f < until:
                names += list(b)
            else:
                del held[b]
        dc.set_pad(0, names)
        dc.run(1)
        if f in saves:
            p = os.path.join(args.outdir, "frame_%05d.ppm" % f)
            dc.save_ppm(p)
            print("saved", p)
        if args.dbg:
            t = dc.dbg_new()
            if t:
                dbgf.write(t)
                dbgf.flush()
        if f % args.every == 0:
            line = "[f %5d]" % f
            for w in watch:
                fmt = "u32"
                if ":" in w:
                    w, fmt = w.split(":")
                try:
                    if "@" in w:
                        s, n = w.split("@")
                        vals = struct.unpack("<%dI" % int(n), dc.read(dc.addr(s), 4 * int(n)))
                        line += " %s=[%s]" % (s, " ".join("%x" % v for v in vals))
                    else:
                        v = getattr(dc, fmt)(w)
                        line += " %s=%s" % (w, ("%x" % v) if fmt in ("u32", "u16", "u8") else v)
                except KeyError:
                    line += " %s=?" % w
            if args.stats:
                st = dc.frame_stats()
                line += " frame=%s" % (st,)
            print(line, flush=True)
            if args.regs:
                print(dc.regs_str(), flush=True)


if __name__ == "__main__":
    main()
