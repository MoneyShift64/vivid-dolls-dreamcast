#!/usr/bin/env python3
"""Disassemble the static F3D display lists in Vivid Dolls' main data segment.

Analysis tool only: reads the player's ROM, prints a symbolic listing to stdout.
Nothing is written to disk (the brief forbids putting ROM bytes into files).

usage: f3d_disasm.py baserom.z64 [data.s] [symbol ...]
  data.s defaults to asm/data/38D50.data.s (used for symbol names / ranges).
  With no symbols, every label whose name ends in DL (or D_8003856x-D_800388D8)
  is decoded.
"""
import re
import struct
import sys

MAIN_ROM_DELTA = 0x7FFFF400          # vram - rom for the main segment

CC_A = ["COMBINED", "TEXEL0", "TEXEL1", "PRIMITIVE", "SHADE", "ENVIRONMENT", "1", "NOISE"] + ["0"] * 8
CC_B = ["COMBINED", "TEXEL0", "TEXEL1", "PRIMITIVE", "SHADE", "ENVIRONMENT", "CENTER", "K4"] + ["0"] * 8
CC_C = ["COMBINED", "TEXEL0", "TEXEL1", "PRIMITIVE", "SHADE", "ENVIRONMENT", "SCALE", "COMBINED_ALPHA",
        "TEXEL0_ALPHA", "TEXEL1_ALPHA", "PRIMITIVE_ALPHA", "SHADE_ALPHA", "ENV_ALPHA", "LOD_FRACTION",
        "PRIM_LOD_FRAC", "K5"] + ["0"] * 16
CC_D = ["COMBINED", "TEXEL0", "TEXEL1", "PRIMITIVE", "SHADE", "ENVIRONMENT", "1", "0"]
AC_ABD = ["COMBINED", "TEXEL0", "TEXEL1", "PRIMITIVE", "SHADE", "ENVIRONMENT", "1", "0"]
AC_C = ["LOD_FRACTION", "TEXEL0", "TEXEL1", "PRIMITIVE", "SHADE", "ENVIRONMENT", "PRIM_LOD_FRAC", "0"]

FMT = ["RGBA", "YUV", "CI", "IA", "I", "?5", "?6", "?7"]
SIZ = ["4b", "8b", "16b", "32b"]

GEOM = [(0x1, "ZBUFFER"), (0x2, "TEXTURE_ENABLE"), (0x4, "SHADE"), (0x200, "SHADING_SMOOTH"),
        (0x1000, "CULL_FRONT"), (0x2000, "CULL_BACK"), (0x10000, "FOG"), (0x20000, "LIGHTING"),
        (0x40000, "TEXTURE_GEN"), (0x80000, "TEXTURE_GEN_LINEAR"), (0x100000, "LOD")]

OMH = {4: "ALPHADITHER", 6: "RGBDITHER", 8: "COMBKEY", 9: "TEXTCONV", 12: "TEXTFILT", 14: "TEXTLUT",
       16: "TEXTLOD", 17: "TEXTDETAIL", 19: "TEXTPERSP", 20: "CYCLETYPE", 22: "COLORDITHER", 23: "PIPELINE"}
OML = {0: "ALPHACOMPARE", 2: "ZSRCSEL", 3: "RENDERMODE"}

MOVEMEM = {0x80: "VIEWPORT", 0x82: "LOOKATY", 0x84: "LOOKATX", 0x86: "L0", 0x88: "L1", 0x8A: "L2",
           0x8C: "L3", 0x8E: "L4", 0x90: "L5", 0x92: "L6", 0x94: "L7", 0x96: "TXTATT", 0x98: "MATRIX_1",
           0x9A: "MATRIX_2", 0x9C: "MATRIX_3", 0x9E: "MATRIX_4"}
MOVEWORD = {0: "MATRIX", 2: "NUMLIGHT", 4: "CLIP", 6: "SEGMENT", 8: "FOG", 0xA: "LIGHTCOL", 0xC: "POINTS",
            0xE: "PERSPNORM"}


def othermode_h_value(shift, val):
    v = (val >> shift)
    if shift == 20:
        return ["1CYCLE", "2CYCLE", "COPY", "FILL"][v & 3]
    if shift == 12:
        return {0: "POINT", 2: "BILERP", 3: "AVERAGE"}.get(v & 3, hex(v))
    if shift == 14:
        return {0: "NONE", 2: "RGBA16", 3: "IA16"}.get(v & 3, hex(v))
    if shift == 19:
        return ["NONE", "PERSP"][v & 1]
    if shift == 23:
        return ["NPRIMITIVE", "1PRIMITIVE"][v & 1]
    if shift == 9:
        return {0: "CONV", 5: "FILTCONV", 6: "FILT"}.get(v & 7, hex(v))
    if shift == 16:
        return ["TILE", "LOD"][v & 1]
    if shift == 17:
        return {0: "CLAMP", 1: "SHARPEN", 2: "DETAIL"}.get(v & 3, hex(v))
    if shift == 6:
        return ["MAGICSQ", "BAYER", "NOISE", "DISABLE"][v & 3]
    if shift == 4:
        return ["PATTERN", "NOTPATTERN", "NOISE", "DISABLE"][v & 3]
    if shift == 8:
        return ["NONE", "KEY"][v & 1]
    return hex(v)


def render_mode(v):
    flags = []
    for bit, n in [(0x8, "AA_EN"), (0x10, "Z_CMP"), (0x20, "Z_UPD"), (0x40, "IM_RD"), (0x80, "CLR_ON_CVG"),
                   (0x1000, "CVG_X_ALPHA"), (0x2000, "ALPHA_CVG_SEL"), (0x4000, "FORCE_BL")]:
        if v & bit:
            flags.append(n)
    flags.append("CVG_DST_" + ["CLAMP", "WRAP", "FULL", "SAVE"][(v >> 8) & 3])
    flags.append("ZMODE_" + ["OPA", "INTER", "XLU", "DEC"][(v >> 10) & 3])
    P = ["IN", "MEM", "BL", "FOG"]
    A = ["IN_A", "FOG_A", "SHADE_A", "0"]
    B = ["1MA", "MEM_A", "1", "0"]
    c1 = "(%s*%s + %s*%s)" % (P[(v >> 30) & 3], A[(v >> 26) & 3], P[(v >> 22) & 3], B[(v >> 18) & 3])
    c2 = "(%s*%s + %s*%s)" % (P[(v >> 28) & 3], A[(v >> 24) & 3], P[(v >> 20) & 3], B[(v >> 16) & 3])
    return " | ".join(flags) + " blend c1=" + c1 + " c2=" + c2


def combine(w0, w1):
    a0 = (w0 >> 20) & 0xF; c0 = (w0 >> 15) & 0x1F; Aa0 = (w0 >> 12) & 7; Ac0 = (w0 >> 9) & 7
    a1 = (w0 >> 5) & 0xF; c1 = w0 & 0x1F
    b0 = (w1 >> 28) & 0xF; b1 = (w1 >> 24) & 0xF; Aa1 = (w1 >> 21) & 7; Ac1 = (w1 >> 18) & 7
    d0 = (w1 >> 15) & 7; Ab0 = (w1 >> 12) & 7; Ad0 = (w1 >> 9) & 7
    d1 = (w1 >> 6) & 7; Ab1 = (w1 >> 3) & 7; Ad1 = w1 & 7
    def cyc(a, b, c, d, Aa, Ab, Ac, Ad):
        return "(%s-%s)*%s+%s / (%s-%s)*%s+%s" % (CC_A[a], CC_B[b], CC_C[c], CC_D[d],
                                                    AC_ABD[Aa], AC_ABD[Ab], AC_C[Ac], AC_ABD[Ad])
    return "c1 " + cyc(a0, b0, c0, d0, Aa0, Ab0, Ac0, Ad0) + " ; c2 " + cyc(a1, b1, c1, d1, Aa1, Ab1, Ac1, Ad1)


def settile(w0, w1):
    return ("fmt=%s siz=%s line=%d tmem=0x%X tile=%d pal=%d cmt=%d maskt=%d shiftt=%d cms=%d masks=%d shifts=%d" %
            (FMT[(w0 >> 21) & 7], SIZ[(w0 >> 19) & 3], (w0 >> 9) & 0x1FF, w0 & 0x1FF, (w1 >> 24) & 7,
             (w1 >> 20) & 0xF, (w1 >> 18) & 3, (w1 >> 14) & 0xF, (w1 >> 10) & 0xF, (w1 >> 8) & 3,
             (w1 >> 4) & 0xF, w1 & 0xF))


def disasm(w0, w1, sym):
    op = w0 >> 24
    a = lambda x: sym.get(x, "0x%08X" % x)
    if op == 0x00:
        return "G_SPNOOP" if (w0 | w1) == 0 else "G_SPNOOP? %08X %08X" % (w0, w1)
    if op == 0x01:
        p = (w0 >> 16) & 0xFF
        return "G_MTX %s %s %s len=%d %s" % ("PROJECTION" if p & 1 else "MODELVIEW", "LOAD" if p & 2 else "MUL",
                                              "PUSH" if p & 4 else "NOPUSH", w0 & 0xFFFF, a(w1))
    if op == 0x03:
        return "G_MOVEMEM %s len=%d %s" % (MOVEMEM.get((w0 >> 16) & 0xFF, hex((w0 >> 16) & 0xFF)), w0 & 0xFFFF, a(w1))
    if op == 0x04:
        return "G_VTX n=%d v0=%d len=%d %s" % (((w0 >> 20) & 0xF) + 1, (w0 >> 16) & 0xF, w0 & 0xFFFF, a(w1))
    if op == 0x06:
        return "G_DL %s %s" % ("BRANCH" if (w0 >> 16) & 0xFF else "CALL", a(w1))
    if op == 0xBF:
        return "G_TRI1 %d %d %d flag=%d" % (((w1 >> 16) & 0xFF) // 10, ((w1 >> 8) & 0xFF) // 10, (w1 & 0xFF) // 10, w1 >> 24)
    if op == 0xBE:
        return "G_CULLDL %d..%d" % ((w0 & 0xFFFF) // 40, (w1 & 0xFFFF) // 40)
    if op == 0xBD:
        return "G_POPMTX %d" % w1
    if op == 0xBC:
        idx = w0 & 0xFF
        return "G_MOVEWORD %s off=0x%X data=0x%08X" % (MOVEWORD.get(idx, hex(idx)), (w0 >> 8) & 0xFFFF, w1)
    if op == 0xBB:
        return "G_TEXTURE on=%d tile=%d level=%d s=0x%04X t=0x%04X" % (w0 & 0xFF, (w0 >> 8) & 7, (w0 >> 11) & 7,
                                                                       w1 >> 16, w1 & 0xFFFF)
    if op == 0xBA:
        sh, ln = (w0 >> 8) & 0xFF, w0 & 0xFF
        return "G_SETOTHERMODE_H %s(len %d) = %s" % (OMH.get(sh, str(sh)), ln, othermode_h_value(sh, w1))
    if op == 0xB9:
        sh, ln = (w0 >> 8) & 0xFF, w0 & 0xFF
        if sh == 3:
            return "G_SETOTHERMODE_L RENDERMODE = " + render_mode(w1)
        return "G_SETOTHERMODE_L %s(len %d) = 0x%X" % (OML.get(sh, str(sh)), ln, w1)
    if op == 0xB8:
        return "G_ENDDL"
    if op in (0xB7, 0xB6):
        names = [n for b, n in GEOM if w1 & b]
        return "%s %s" % ("G_SETGEOMETRYMODE" if op == 0xB7 else "G_CLEARGEOMETRYMODE", "|".join(names) or hex(w1))
    if op == 0xB4:
        return "G_RDPHALF_1 0x%08X" % w1
    if op == 0xB3:
        return "G_RDPHALF_2 0x%08X" % w1
    if op == 0xE4 or op == 0xE5:
        return "%s xh=%.2f yh=%.2f xl=%.2f yl=%.2f tile=%d" % ("G_TEXRECT" if op == 0xE4 else "G_TEXRECTFLIP",
               ((w0 >> 12) & 0xFFF) / 4, (w0 & 0xFFF) / 4, ((w1 >> 12) & 0xFFF) / 4, (w1 & 0xFFF) / 4, (w1 >> 24) & 7)
    if op == 0xE6: return "G_RDPLOADSYNC"
    if op == 0xE7: return "G_RDPPIPESYNC"
    if op == 0xE8: return "G_RDPTILESYNC"
    if op == 0xE9: return "G_RDPFULLSYNC"
    if op == 0xED:
        return "G_SETSCISSOR mode=%d %.2f,%.2f - %.2f,%.2f" % ((w1 >> 24) & 3, ((w0 >> 12) & 0xFFF) / 4, (w0 & 0xFFF) / 4,
                                                             ((w1 >> 12) & 0xFFF) / 4, (w1 & 0xFFF) / 4)
    if op == 0xEE:
        return "G_SETPRIMDEPTH z=0x%04X dz=0x%04X" % (w1 >> 16, w1 & 0xFFFF)
    if op == 0xEF:
        return "G_RDPSETOTHERMODE H=0x%06X L: %s" % (w0 & 0xFFFFFF, render_mode(w1))
    if op == 0xF0:
        return "G_LOADTLUT tile=%d count=%d (sl=%d sh=%d)" % ((w1 >> 24) & 7, (((w1 >> 14) & 0x3FF) - ((w0 >> 14) & 0x3FF)) + 1,
                                                             (w0 >> 14) & 0x3FF, (w1 >> 14) & 0x3FF)
    if op == 0xF2:
        return "G_SETTILESIZE tile=%d uls=%.2f ult=%.2f lrs=%.2f lrt=%.2f" % ((w1 >> 24) & 7, ((w0 >> 12) & 0xFFF) / 4,
               (w0 & 0xFFF) / 4, ((w1 >> 12) & 0xFFF) / 4, (w1 & 0xFFF) / 4)
    if op == 0xF3:
        return "G_LOADBLOCK tile=%d uls=%d ult=%d lrs=%d dxt=0x%X" % ((w1 >> 24) & 7, (w0 >> 12) & 0xFFF, w0 & 0xFFF,
                                                                     (w1 >> 12) & 0xFFF, w1 & 0xFFF)
    if op == 0xF4:
        return "G_LOADTILE tile=%d uls=%.2f ult=%.2f lrs=%.2f lrt=%.2f" % ((w1 >> 24) & 7, ((w0 >> 12) & 0xFFF) / 4,
               (w0 & 0xFFF) / 4, ((w1 >> 12) & 0xFFF) / 4, (w1 & 0xFFF) / 4)
    if op == 0xF5:
        return "G_SETTILE " + settile(w0, w1)
    if op == 0xF6:
        return "G_FILLRECT %d,%d - %d,%d" % (((w1 >> 12) & 0xFFF) >> 2, (w1 & 0xFFF) >> 2, ((w0 >> 12) & 0xFFF) >> 2, (w0 & 0xFFF) >> 2)
    if op == 0xF7: return "G_SETFILLCOLOR 0x%08X" % w1
    if op == 0xF8: return "G_SETFOGCOLOR 0x%08X" % w1
    if op == 0xF9: return "G_SETBLENDCOLOR 0x%08X" % w1
    if op == 0xFA: return "G_SETPRIMCOLOR minlevel=%d lodfrac=%d rgba=0x%08X" % ((w0 >> 8) & 0xFF, w0 & 0xFF, w1)
    if op == 0xFB: return "G_SETENVCOLOR 0x%08X" % w1
    if op == 0xFC: return "G_SETCOMBINE " + combine(w0 & 0xFFFFFF, w1)
    if op in (0xFD, 0xFF):
        return "%s fmt=%s siz=%s width=%d %s" % ("G_SETTIMG" if op == 0xFD else "G_SETCIMG", FMT[(w0 >> 21) & 7],
                                                SIZ[(w0 >> 19) & 3], (w0 & 0xFFF) + 1, a(w1))
    if op == 0xFE: return "G_SETZIMG " + a(w1)
    return "??? %08X %08X" % (w0, w1)


def load_labels(path):
    labels = []
    cur = None
    for line in open(path):
        m = re.match(r"dlabel (\w+)", line)
        if m:
            cur = [m.group(1), None, None]
            labels.append(cur)
            continue
        m = re.match(r"\s*/\* ([0-9A-F]+) ([0-9A-F]{8}) ", line)
        if m and cur is not None:
            va = int(m.group(2), 16)
            if cur[1] is None:
                cur[1] = va
            cur[2] = va
    out = []
    for n, s, e in labels:
        if s is not None:
            out.append((n, s, e + 4))
    return out


def main():
    rom = open(sys.argv[1], "rb").read()
    datas = sys.argv[2] if len(sys.argv) > 2 and sys.argv[2].endswith(".s") else "asm/data/38D50.data.s"
    want = [x for x in sys.argv[2:] if not x.endswith(".s")]
    labels = load_labels(datas)
    sym = {s: n for n, s, e in labels}
    for n, s, e in labels:
        if want:
            if n not in want:
                continue
        elif not (n.endswith("DL") or n in ("D_80038560", "D_80038648", "D_800386A0", "D_800387D0", "D_80038828",
                                             "D_80038880", "D_800388D8")):
            continue
        print("%s @ 0x%08X (%d bytes)" % (n, s, e - s))
        for va in range(s, e, 8):
            off = va - MAIN_ROM_DELTA
            w0, w1 = struct.unpack(">II", rom[off:off + 8])
            print("  +%03X  %s" % (va - s, disasm(w0, w1, sym)))
        print()


if __name__ == "__main__":
    main()
