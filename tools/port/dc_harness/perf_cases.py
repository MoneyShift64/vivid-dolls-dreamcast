#!/usr/bin/env python3
"""Frame-time measurement of three heavy cases (headless Flycast), using the L+R test cheat.

  python3 -I tools/port/dc_harness/perf_cases.py [build/dc] [wide] [tall] [ending] [proj] [--max-rounds N]

wide   : clears photos with the L+R cheat until a 640-wide photo (ovl2_D_800FBA70 == 640),
         then measures its stage-clear pan (Game_Main stage_clear mode 1, 640 frames).
tall   : same for a 480-tall photo (mode 2), for comparison.
ending : marks the other 7 girls done (D_800C178E) and the photo as the third one
         (D_800B04CC = 2), clears with the cheat -> final texts / stage score / ending;
         measures frame times and audio underruns (vd_audio_stats) there.
proj   : stays in rounds (no cheat), forces the boss to attack often, measures frames
         with many live effects (gEffects pool, ovl2_D_800FBA90: 64 x 0x4C bytes).
Per case: fps = 60 * frames built / vblanks, per-frame vblank gaps (a 'hitch' = a frame
that took 2+ vblanks), F3D time per gfx task and textures uploaded per task (vd_perf_ring),
and audio underruns. Photo rule: no frame is saved or looked at; only RAM and counters.
"""
import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from dcrun import DC, OUT  # noqa: E402

args = sys.argv[1:]
BUILD = args[0] if args and not args[0].startswith("-") and args[0] not in ("wide", "tall", "ending", "proj") else "build/dc"
CASES = [a for a in args if a in ("wide", "tall", "ending", "proj")] or ["wide"]
MAXR = int(args[args.index("--max-rounds") + 1]) if "--max-rounds" in args else 30
PROF = "--prof" in args
ALLB = "--all-bosses" in args      # proj: measure the first 3 rounds whatever the enemy set
FR = int(args[args.index("--frames") + 1]) if "--frames" in args else 1500
HASHES = args[args.index("--hashes") + 1] if "--hashes" in args else None

dc = DC(os.path.join(BUILD, "vividdolls.cdi"), serial=os.path.join(OUT, "serial_cases.log"), log=os.devnull)
dc.load_syms(os.path.join(BUILD, "vividdolls.elf"))
hf = open(HASHES, "w") if HASHES else None


def task_funcs():
    head = dc.u32("D_800B05A4")
    tail = dc.u32("D_800C53FC")
    names = []
    t = dc.u32(head) if head else 0
    n = 0
    while t and t != tail and n < 64:
        sp = dc.u32(t + 8)
        names.append(dc.func_of(dc.u32(sp + 0x3C)) if sp else "?")
        t = dc.u32(t)
        n += 1
    return names


def has(*names):
    return lambda: any(f in names for f in task_funcs())


def counters():
    return dc.u32("vd_vi_retrace_count"), dc.u32("D_800C1650")


def step(n, pad=(), trig=None):
    for _ in range(n):
        dc.set_pad(0, list(pad))
        if trig is not None:
            dc.set_trig(0, trig, trig)
        dc.run(1)
        if hf:
            st = dc.frame_stats()
            hf.write("%d %s\n" % (dc.frame, st[0] if st else "-"))


def wait_for(desc, cond, limit, pad=()):
    for _ in range(limit):
        if cond():
            return True
        step(1, pad)
    print("  FAIL waiting for %s (frame %d, tasks %s)" % (desc, dc.frame, ",".join(sorted(set(task_funcs())))))
    return False


AUD = ["frames", "samples", "mix_us", "mix_us_max", "voices_mixed", "voices_max", "notes",
       "seq_play", "seq_stop", "snd_play", "snd_end", "last_snd_sound", "ai_buffers", "ai_samples",
       "ai_dropped", "ai_underruns", "ai_fill", "ai_pulled"]


def aud():
    if "vd_audio_stats" not in dc.syms:
        return {}
    v = struct.unpack("<%dI" % len(AUD), dc.read(dc.addr("vd_audio_stats"), 4 * len(AUD)))
    return dict(zip(AUD, v))


def pacc():
    if "vd_pvr_acc" not in dc.syms:
        return None
    return struct.unpack("<9I", dc.read(dc.addr("vd_pvr_acc"), 36))


F3DS = ["cmds", "vtx", "tris_in", "tris_culled", "tris_out", "rects", "tex_decodes", "unknown", "last_unknown",
        "loadblocks", "loadtiles", "loadtluts", "load_bytes", "hash_bytes", "tex_lookups", "tex_direct"]


def f3ds():
    if "vd_perf_f3d_stats" not in dc.syms:
        return None
    return dict(zip(F3DS, struct.unpack("<16I", dc.read(dc.addr("vd_perf_f3d_stats"), 64))))


def ring():
    """vd_perf_ring: [256][3] = (retrace when queued, F3D us, textures uploaded)"""
    raw = dc.read(dc.addr("vd_perf_ring"), 256 * 12)
    return [struct.unpack_from("<3I", raw, 12 * i) for i in range(256)]


def effects_live():
    raw = dc.read(dc.addr("ovl2_D_800FBA90"), 64 * 0x4C)
    return sum(1 for i in range(64) if struct.unpack_from(">i" if False else "<i", raw, i * 0x4C)[0] != 0)


class Meter:
    """Collects per-presented-frame data while the caller steps."""

    def __init__(self):
        self.vi0, self.b0 = counters()
        self.t0 = dc.u32("vd_perf_tasks")
        self.a0 = aud()
        self.p0 = pacc()
        self.s0 = f3ds()
        if self.p0:
            dc.write(dc.addr("vd_pvr_acc") + 20, b"\0" * 8)      # reset the maxima [5], [6]
            dc.write(dc.addr("vd_pvr_acc") + 32, b"\0" * 4)      # [8]
        self.gaps = []
        self.f3d = []
        self.up = []
        self.last_b = self.b0
        self.last_vi = self.vi0
        self.last_t = self.t0

    def sample(self):
        vi, b = counters()
        if b != self.last_b:
            # vblanks per built frame
            self.gaps.append((vi - self.last_vi) / max(b - self.last_b, 1))
            self.last_b, self.last_vi = b, vi
        t = dc.u32("vd_perf_tasks")
        if t != self.last_t:
            r = ring()
            for k in range(self.last_t + 1, t + 1):
                e = r[k & 255]
                self.f3d.append(e[1])
                self.up.append(e[2])
            self.last_t = t

    def report(self, name, partial=False):
        vi, b = counters()
        a1 = aud()
        fps = 60.0 * (b - self.b0) / max(vi - self.vi0, 1)
        if partial:
            # only the sampled frames count: fps from the per-frame vblank gaps
            fps = 60.0 * len(self.gaps) / max(sum(self.gaps), 1)
            a1 = None
        hitches = sum(1 for g in self.gaps if g >= 2)
        f = sorted(self.f3d) or [0]
        n = max(len(self.f3d), 1)
        print("  %-10s %.1f fps over %d vblanks; frames needing 2+ vblanks: %d of %d" %
              (name, fps, sum(self.gaps) if partial else vi - self.vi0, hitches, len(self.gaps)))
        print("             F3D per task: avg %.2f ms, median %.2f, p90 %.2f, max %.2f; textures uploaded per task avg %.1f max %d" %
              (sum(self.f3d) / n / 1000.0, f[len(f) // 2] / 1000.0, f[int(len(f) * 0.9)] / 1000.0, f[-1] / 1000.0,
               sum(self.up) / n, max(self.up or [0])))
        p1 = pacc()
        if p1 and self.p0 and not partial:
            nf = max(p1[0] - self.p0[0], 1)
            print("             PVR per frame: %.0f tris (max %d), %.0f headers, opaque %.0f kpx, blended %.0f kpx (max %.0f kpx, max %d blended polys), uploads %.1f KB" %
                  ((p1[1] - self.p0[1]) / nf, p1[6], (p1[2] - self.p0[2]) / nf, (p1[3] - self.p0[3]) / nf / 1000.0,
                   (p1[4] - self.p0[4]) / nf / 1000.0, p1[5] / 1000.0, p1[8], (p1[7] - self.p0[7]) / nf / 1024.0))
        s1 = f3ds()
        if s1 and self.s0 and not partial:
            nt = max(len(self.f3d), 1)
            print("             F3D per task: " + ", ".join("%s %.0f" % (k, (s1[k] - self.s0[k]) / nt) for k in F3DS
                                                         if k not in ("unknown", "last_unknown")))
        if a1 and self.a0:
            d = {k: a1[k] - self.a0[k] for k in ("frames", "ai_buffers", "ai_underruns", "ai_dropped", "mix_us")}
            print("             audio: %d frames mixed, %d buffers, underruns %d, dropped %d, mix avg %.2f ms, ring fill now %d" %
                  (d["frames"], d["ai_buffers"], d["ai_underruns"], d["ai_dropped"],
                   d["mix_us"] / max(d["frames"], 1) / 1000.0, a1["ai_fill"]))
        return fps


def dump_samples(smp):
    # raw PCs (code addresses only) for addr2line, see --prof
    with open(os.path.join(OUT, "prof_pcs.txt"), "w") as f:
        for pc, pr in smp:
            f.write("%08x %08x\n" % (pc, pr))


def run_measured(n, m, pad=(), cond=None):
    for _ in range(n):
        if cond is not None and not cond():
            break
        step(1, pad)
        m.sample()


def boot_to_round():
    wait_for("boot logo", has("Boot_Logo"), 300)
    wait_for("title", has("Title_Main", "title_slide"), 900)
    step(60)
    step(8, ["X"])
    wait_for("coin", lambda: dc.s32("gCredits") >= 1, 120)
    step(30)
    step(8, ["START"])
    wait_for("girl select", has("GirlSelect_Task"), 600)
    step(120)
    step(8, ["A"])
    return wait_for("round", has("Game_Main"), 2500)


def cheat_clear():
    for _ in range(10):
        dc.set_trig(0, 0x7FFF, 0x7FFF)
        dc.set_pad(0, [])
        dc.run(1)
    dc.set_trig(0, 0, 0)


def leaving():
    return dc.s32("ovl2_D_800FD2D8") == 1


def advance_to_next_round(limit=6000):
    """From a cleared round to the next Game_Main: presses A now and then (girl select,
    bonus stage intro), taps button 0x10 (B) in the bonus stage."""
    k = 0
    while k < limit:
        f = task_funcs()
        if "Game_Main" in f and not leaving():
            return True
        if "Bonus_Task" in f:
            step(2, ["B"])
            step(2)
            k += 4
            continue
        if "GirlSelect_Task" in f or "BonusIntro_Main" in f:
            step(60)
            step(8, ["A"])
            k += 68
            continue
        step(10)
        k += 10
    return False


def round_info():
    for _ in range(300):
        if dc.s32("ovl2_D_800FBA70") != 0 and has("Game_Main")() and not leaving():
            break
        step(1)
    return (dc.s32("D_800B0608"), dc.s32("D_800B04CC"), dc.s32("ovl2_D_800FBA70"), dc.s32("ovl2_D_800FBA74"))


def case_photo(kind):
    want = 640 if kind == "wide" else 480
    for r in range(MAXR):
        girl, photo, w, h = round_info()
        print("  round %d: girl %d photo %d board %dx%d (frame %d)" % (r, girl, photo, w, h, dc.frame), flush=True)
        step(60)
        hit = (w == 640) if kind == "wide" else (h == 480)
        if hit:
            m0 = Meter()
            run_measured(120, m0)
            m0.report("round")
        if not hit and kind == "wide" and r == 0:
            # the only 640x240 photo of the game (jpeg64 xw/yw headers): girl 6, photo 3;
            # make this clear lead there (photo index is incremented after the clear)
            dc.write(dc.addr("D_800B0608"), struct.pack("<i", 6))
            dc.write(dc.addr("D_800B04CC"), struct.pack("<i", 1))
        cheat_clear()
        if hit:
            wait_for("stage clear", leaving, 600)
            step(3)
            m = Meter()
            if PROF:
                dc.prof_start(20000)
            # pan: 640 (wide) / 480 (tall) frames of the stage-clear loop
            run_measured(want + 40, m)
            m.report("pan")
            if PROF:
                smp = dc.prof_stop()
                print("profile samples:", len(smp))
                print(dc.prof_report(smp, 30))
                dump_samples(smp)
            return True
        if not advance_to_next_round():
            print("  could not reach the next round")
            return False
    return False


def case_ending():
    girl = dc.s32("D_800B0608")
    for i in range(8):
        if i != girl:
            dc.write(dc.addr("D_800C178E") + i, b"\x01")
    dc.write(dc.addr("D_800B04CC"), struct.pack("<i", 2))
    print("  girl %d, others marked done, photo 3; board %dx%d" % (girl, dc.s32("ovl2_D_800FBA70"), dc.s32("ovl2_D_800FBA74")))
    step(30)
    cheat_clear()
    wait_for("stage clear", leaving, 600)
    m = Meter()
    run_measured(640, m)                 # stage-clear pan / sparkles + perfect bonus
    m.report("clear")
    m = Meter()
    run_measured(1400, m, cond=has("Game_Main"))   # final texts, stage score, icons, fade
    m.report("ending")
    print("  after ending: tasks %s" % ",".join(sorted(set(task_funcs()))))
    return True


def case_proj():
    """Round with the spinning boss (SpinBoss_Task) made to attack whenever it can
    (its 1-in-n chance table ovl2_D_800F9D00 set to 1): fast-bullet spins and salvos of
    8 homing missiles with smoke trails fill the effect pool. Lives are topped up."""
    for r in range(MAXR):
        girl, photo, w, h = round_info()
        boss = any(f in ("SpinBoss_Task", "spinboss_frame", "ovl2_func_800F00F4") for f in task_funcs())
        print("  round %d: girl %d photo %d board %dx%d, spin boss %s" % (r, girl, photo, w, h, boss), flush=True)
        if not boss and not ALLB:
            cheat_clear()
            if not advance_to_next_round():
                return False
            continue
        print("    tasks:", ",".join(sorted(set(task_funcs()))))
        for i in range(16):
            dc.write(dc.addr("ovl2_D_800F9D00") + 4 * i, struct.pack("<i", 1))
        if PROF:
            dc.prof_start(20000)
        m = Meter()
        moves = (["RIGHT"], ["DOWN"], ["LEFT"], ["UP"])
        buckets = {}
        import ctypes
        from collections import Counter
        pbuf = (ctypes.c_uint32 * 400000)()
        pfun = {}
        if PROF:
            dc.h.h_prof_read(pbuf, 400000)      # drop the samples so far
        last_t = dc.u32("vd_perf_tasks")
        last_s = f3ds()
        last_vi, last_b = counters()
        for k in range(FR):
            if not has("Game_Main")() or leaving():
                break
            dc.write(dc.addr("D_800B04D4"), struct.pack("<i", 3))
            n0 = effects_live()
            step(1, moves[(k // 40) % 4])
            m.sample()
            if PROF:
                ns = dc.h.h_prof_read(pbuf, 400000)
                c = pfun.setdefault(min(n0 // 8, 5), Counter())
                for q in range(0, ns, 2):
                    c[pbuf[q]] += 1
            t = dc.u32("vd_perf_tasks")
            vi, b = counters()
            if t != last_t:
                rg = ring()
                sc = f3ds()
                bk = buckets.setdefault(min(n0 // 8, 5), {"n": 0, "us": 0, "max": 0, "vi": 0, "fr": 0,
                                                          "s": dict.fromkeys(F3DS, 0)})
                for q in range(last_t + 1, t + 1):
                    bk["n"] += 1
                    bk["us"] += rg[q & 255][1]
                    bk["max"] = max(bk["max"], rg[q & 255][1])
                if sc and last_s:
                    for key in F3DS:
                        bk["s"][key] += sc[key] - last_s[key]
                bk["vi"] += vi - last_vi
                bk["fr"] += b - last_b
                last_t, last_s, last_vi, last_b = t, sc, vi, b
        m.report("round")
        print("    by live effects: tasks, fps (frames built / vblanks), F3D avg / max ms, per task: cmds vtx tris_out lookups hash_bytes")
        for key in sorted(buckets):
            bk = buckets[key]
            n = max(bk["n"], 1)
            print("    %2d-%2d fx: %4d  %5.1f fps  %5.2f / %5.2f ms   %5.0f %4.0f %4.0f %4.0f %6.0f" %
                  (8 * key, 8 * key + 7 if key < 5 else 64, bk["n"], 60.0 * bk["fr"] / max(bk["vi"], 1),
                   bk["us"] / n / 1000.0, bk["max"] / 1000.0, bk["s"]["cmds"] / n, bk["s"]["vtx"] / n,
                   bk["s"]["tris_out"] / n, bk["s"]["tex_lookups"] / n, bk["s"]["hash_bytes"] / n))
        if PROF:
            dc.prof_stop()
            byf = {}
            for key, c in pfun.items():
                f = Counter()
                for pc, n in c.items():
                    f[dc.func_of(pc)] += n
                byf[key] = f
            lo = byf.get(0, Counter())
            nlo = max(buckets.get(0, {}).get("n", 1), 1)
            for key in sorted(byf):
                if key == 0:
                    continue
                f = byf[key]
                nk = max(buckets.get(key, {}).get("n", 1), 1)
                tot = sum(f.values()) / nk
                print("    profile, %d-%d fx vs 0-7 fx: samples per frame %.0f vs %.0f (idle %.0f vs %.0f); growth by function:" %
                      (8 * key, 8 * key + 7, tot, sum(lo.values()) / nlo, f["Main_IdleThread"] / nk, lo["Main_IdleThread"] / nlo))
                diff = Counter({fn: f[fn] / nk - lo[fn] / nlo for fn in set(f) | set(lo)})
                print("      " + ", ".join("%s %+.1f" % (fn, v) for fn, v in diff.most_common(14)))
        if not ALLB or r >= 2:
            return True
        cheat_clear()
        if not advance_to_next_round():
            return False
    return False


print("Vivid Dolls perf cases:", BUILD, CASES)
if not boot_to_round():
    sys.exit(1)
step(60)
for c in CASES:
    print("case", c, flush=True)
    if c in ("wide", "tall"):
        case_photo(c)
    elif c == "ending":
        case_ending()
    elif c == "proj":
        case_proj()
    if not has("Game_Main")():
        advance_to_next_round()
if hf:
    hf.close()
