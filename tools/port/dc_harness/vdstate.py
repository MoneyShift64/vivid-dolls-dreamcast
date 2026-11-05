"""Vivid Dolls game-state readers for the harness (no pixels involved)."""
import struct


def q(dc, sym):
    """OSMesgQueue (VdMesgQueue): mtqueue, fullqueue, validCount, first, msgCount, msg."""
    a = dc.addr(sym)
    mt, full, valid, first, cnt, msg = struct.unpack("<IIiiiI", dc.read(a, 24))
    return "Q(%s valid=%d/%d mt=%x full=%x)" % (sym, valid, cnt, mt, full)


def sched(dc):
    a = dc.addr("D_800C0AC0")
    w = struct.unpack("<9I", dc.read(a + 0x260, 36))
    iq = struct.unpack("<IIiiiI", dc.read(a + 0x40, 24))
    cq = struct.unpack("<IIiiiI", dc.read(a + 0x78, 24))
    return ("sched clients=%x audH=%x gfxH=%x audT=%x gfxT=%x curRSP=%x curRDP=%x frames=%d doAudio=%d "
            "intQ=%d cmdQ=%d" % (w + (iq[2], cq[2])))


def gfxtasks(dc):
    a = dc.addr("D_800B0730")
    out = []
    for i in range(2):
        b = a + i * 0x2180
        nxt, state, flags, fb = struct.unpack("<4I", dc.read(b, 16))
        mq, msg = struct.unpack("<II", dc.read(b + 0x50, 8))
        mt = struct.unpack("<h", dc.read(b + 0x2158, 2))[0]
        out.append("gt%d@%x st=%x fl=%x fb=%x mq=%x msg=%x type=%d" % (i, b, state, flags, fb, mq, msg, mt))
    return " | ".join(out)


def tasks(dc):
    """The game's cooperative task list (head sentinel -> tail)."""
    head = dc.u32("D_800B05A4")
    tail = dc.u32("D_800C53FC")
    out = []
    t = dc.u32(head) if head else 0
    n = 0
    while t and t != tail and n < 64:
        nxt, prev, sp, prio = struct.unpack("<IIIi", dc.read(t, 16))
        pc = dc.u32(sp + 0x3C) if sp else 0
        out.append("%x(p%d pc=%s)" % (t, prio, dc.sym_at(pc)))
        t = nxt
        n += 1
    return "tasks: " + " ".join(out)
