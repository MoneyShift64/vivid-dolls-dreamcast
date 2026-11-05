#!/usr/bin/env python3
"""Audio test for the Vivid Dolls Dreamcast port in headless Flycast.

  python3 -I port/audio/tests/audio_test.py [build/dc] [--frames N] [--wav out.wav]

Plays the boot path of tools/port/dc_harness/boot_test.py (boot logo -> title ->
coin -> start -> girl select -> stage intro -> round, random-ish input in the
round) while
  * capturing the libretro core's audio output (the AICA's mix) to a WAV file,
    through a ctypes callback installed on the core with
    retro_set_audio_sample_batch (the harness itself is not changed),
  * logging every music-state change of the game (D_80038498 / song D_80038490),
    sequence-player play/stop, sound-player starts, from emulator RAM,
  * reading the port's audio counters (vd_audio_stats: CPU time in alAudioFrame,
    voices, ring fill, underruns, clipping).
At the end it prints per-stage loudness (RMS / peak, dBFS) and pass/fail checks:
music audible on title / girl select / round, sound effects started, no clipping
storm, few underruns, audio CPU share.
Photo rule: no video frame is looked at or saved (only game state is read).
"""
import ctypes
import math
import os
import struct
import sys
import wave

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
sys.path.insert(0, os.path.join(ROOT, "tools", "port", "dc_harness"))
from dcrun import DC, OUT, CORE  # noqa: E402

args = [a for a in sys.argv[1:]]
BUILD = args[0] if args and not args[0].startswith("-") else os.path.join(ROOT, "build", "dc")


def opt(name, default):
    if name in args:
        return type(default)(args[args.index(name) + 1])
    return default


ROUND_FRAMES = opt("--frames", 1500)
WAV = opt("--wav", os.path.join(OUT, "audio_test.wav"))

STATS = ["frames", "samples", "mix_us", "mix_us_max", "voices_mixed", "voices_max", "notes",
         "seq_play", "seq_stop", "snd_play", "snd_end", "last_snd_sound", "ai_buffers", "ai_samples",
         "ai_dropped", "ai_underruns", "ai_fill", "ai_pulled", "params_lost", "peak", "clipped",
         "fx_frames", "heap_top", "pcm_kb", "pcm_waves"]

dc = DC(os.path.join(BUILD, "vividdolls.cdi"), serial=os.path.join(OUT, "serial.log"))
dc.load_syms(os.path.join(BUILD, "vividdolls.elf"))

# ---- audio capture: install our own batch callback on the core ----
pcm = bytearray()
BATCH = ctypes.CFUNCTYPE(ctypes.c_size_t, ctypes.POINTER(ctypes.c_int16), ctypes.c_size_t)
SAMPLE = ctypes.CFUNCTYPE(None, ctypes.c_int16, ctypes.c_int16)


def _batch(data, frames):
    pcm.extend(ctypes.string_at(data, frames * 4))
    return frames


def _sample(l, r):
    pcm.extend(struct.pack("<hh", l, r))


_cb_batch, _cb_sample = BATCH(_batch), SAMPLE(_sample)
core = ctypes.CDLL(CORE)
core.retro_set_audio_sample_batch(_cb_batch)
core.retro_set_audio_sample(_cb_sample)


def stats():
    base = dc.addr("vd_audio_stats")
    v = struct.unpack("<%dI" % len(STATS), dc.read(base, 4 * len(STATS)))
    return dict(zip(STATS, v))


marks = []          # (label, pcm byte offset)
events = []
sfx_starts = []   # (pcm sample offset, sound id, slot pitch, frame)
last = {}


def poll():
    """log game music state + player counters changes"""
    st = dc.s32("D_80038498")
    song = dc.u32("D_80038490")
    s = stats()
    cur = {"music": (st, song), "seq_play": s["seq_play"], "seq_stop": s["seq_stop"],
           "snd_play": s["snd_play"]}
    t = len(pcm) / 4 / 44100.0
    if cur["music"] != last.get("music"):
        events.append("  t=%6.2fs frame %5d  music state %d song %d" % (t, dc.frame, st, song))
    if cur["seq_play"] != last.get("seq_play", 0):
        events.append("  t=%6.2fs frame %5d  sequence player: PLAY (#%d)" % (t, dc.frame, cur["seq_play"]))
    if cur["seq_stop"] != last.get("seq_stop", 0):
        events.append("  t=%6.2fs frame %5d  sequence player: STOPPED (#%d)" % (t, dc.frame, cur["seq_stop"]))
    if cur["snd_play"] != last.get("snd_play", 0):
        sid = s["last_snd_sound"]
        pitch = dc.f32(dc.addr("D_800A6018") + 12 * sid) if 0 <= sid < 29 else 0.0
        sfx_starts.append((len(pcm) // 4, sid, pitch, dc.frame))
        events.append("  t=%6.2fs frame %5d  sound player: %d start(s), last sound %d" %
                      (t, dc.frame, cur["snd_play"] - last.get("snd_play", 0), s["last_snd_sound"]))
    last.update(cur)


def task_funcs():
    head = dc.u32("D_800B05A4")
    tail = dc.u32("D_800C53FC")
    names, t, n = [], (dc.u32(head) if head else 0), 0
    while t and t != tail and n < 64:
        sp = dc.u32(t + 8)
        names.append(dc.func_of(dc.u32(sp + 0x3C)) if sp else "?")
        t = dc.u32(t)
        n += 1
    return names


def step(n, pad=()):
    for _ in range(n):
        dc.set_pad(0, list(pad))
        dc.run(1)
        poll()


def wait_for(desc, names, limit, pad=()):
    for _ in range(limit):
        if any(f in names for f in task_funcs()):
            print("  reached %-28s frame %5d" % (desc, dc.frame), flush=True)
            return True
        step(1, pad)
    print("  FAILED to reach", desc, flush=True)
    return False


def mark(label):
    marks.append((label, len(pcm)))


def prof_share(samples, prefixes):
    n = max(len(samples), 1)
    k = 0
    for pc, pr in samples:
        f = dc.func_of(pc)
        if any(f.startswith(p) for p in prefixes):
            k += 1
    return 100.0 * k / n


AUDIO_FUNCS = ("alAudioFrame", "voice_", "mix_", "src_read", "adpcm_frame", "fx_process", "apply_param",
               "env_", "__CSP", "alCSP", "alSnd", "_sndp", "_handleEvent", "alEvtq", "alSyn", "param_",
               "osAi", "ai_", "snd_stream", "snd_pcm16", "spu_", "g2_", "__alCSeq", "alCSeq",
               "__getTrackByte", "__readVarLen", "__vs", "__lookup", "__map", "__unmap", "time_to",
               "next_client", "Osc_", "AudioMgr_", "Sfx_", "alCents2Ratio", "powf", "__ieee754_powf",
               "set_pitch", "pv_", "env_targets", "env_ramp", "csp_post", "post_state_evt", "_removeEvents",
               "_DivS32ByF32", "__seqp", "__voiceNeedsNoteKill", "__setInst", "__resetPerf", "__initFromBank")

ok = True
print("Vivid Dolls audio test:", BUILD)
mark("boot")
ok &= wait_for("boot logo", ("Boot_Logo",), 300)
ok &= wait_for("title", ("Title_Main", "title_slide"), 900)
mark("title")
step(300)
mark("title-end")
step(8, ["X"])
step(30)
step(8, ["START"])
ok &= wait_for("girl select", ("GirlSelect_Task",), 600)
mark("girl select")
step(240)
mark("girl-select-end")
step(8, ["A"])
ok &= wait_for("stage intro", ("StageIntro_Main",), 600)
mark("stage intro")
ok &= wait_for("round", ("Game_Main",), 900)
mark("round")
s0 = stats()
vi0 = dc.u32("vd_vi_retrace_count")
dc.prof_start(20000)
pads = [["RIGHT", "A"], ["DOWN", "A"], ["LEFT", "B"], ["UP", "A"], ["RIGHT"], ["DOWN", "B"]]
for i in range(ROUND_FRAMES // 30):
    step(30, pads[i % len(pads)])
samples = dc.prof_stop()
s1 = stats()
vi1 = dc.u32("vd_vi_retrace_count")
mark("end")

# ---- report ----
print("\nevents:")
for e in events:
    print(e)

with wave.open(WAV, "wb") as w:
    w.setnchannels(2)
    w.setsampwidth(2)
    w.setframerate(44100)
    w.writeframes(bytes(pcm))
print("\nwrote %s (%.1f s)" % (WAV, len(pcm) / 4 / 44100.0))
import json  # noqa: E402
with open(WAV + ".json", "w") as jf:
    json.dump({"marks": marks, "sfx": sfx_starts}, jf)


def loud(a, b):
    n = (b - a) // 4
    if n <= 0:
        return (-999.0, -999.0, 0)
    v = struct.unpack("<%dh" % (2 * n), bytes(pcm[a:a + 4 * n]))
    ss = sum(x * x for x in v)
    rms = math.sqrt(ss / len(v))
    pk = max(abs(x) for x in v)
    clip = sum(1 for x in v if x >= 32767 or x <= -32768)
    db = lambda x: 20 * math.log10(x / 32768.0) if x > 0 else -999.0
    return (db(rms), db(pk), clip)


print("\nloudness per stage (RMS dBFS / peak dBFS / clipped samples):")
loud_by = {}
for (lab, a), (_, b) in zip(marks, marks[1:]):
    r = loud(a, b)
    loud_by[lab] = r
    print("  %-16s %7.1f %7.1f %6d   (%.1f s)" % (lab, r[0], r[1], r[2], (b - a) / 4 / 44100.0))

d = {k: s1[k] - s0[k] for k in STATS}
frames = max(d["frames"], 1)
vis = max(vi1 - vi0, 1)
mix_ms_per_s = d["mix_us"] / 1000.0 / (vis / 60.0)
print("\nround (%d vblanks): alAudioFrame %.2f ms per frame avg (max %d us), %.1f ms CPU per s = %.1f %%" %
      (vis, d["mix_us"] / 1000.0 / frames, s1["mix_us_max"], mix_ms_per_s, mix_ms_per_s / 10.0))
print("  voices avg %.1f (max %d), notes %d, sounds started %d, ended %d, fx frames %d" %
      (d["voices_mixed"] / frames, s1["voices_max"], d["notes"], d["snd_play"], d["snd_end"], d["fx_frames"]))
print("  AI: %d buffers %d samples, pulled %d, ring fill now %d, underruns %d, dropped %d, params lost %d, "
      "clipped %d" % (d["ai_buffers"], d["ai_samples"], d["ai_pulled"], s1["ai_fill"], d["ai_underruns"],
                      d["ai_dropped"], s1["params_lost"], d["clipped"]))
share = prof_share(samples, AUDIO_FUNCS)
print("  decoded-sample cache: %d waves, %d KB; heap top after Audio_Init %08x (%d KB left below 16 MB)" %
      (s1["pcm_waves"], s1["pcm_kb"], s1["heap_top"], (0x8D000000 - s1["heap_top"]) // 1024))
print("  profiler: %d samples, audio code %.1f %% of SH4 time" % (len(samples), share))
print(dc.prof_report(samples, 25))

checks = [
    ("stages reached", ok),
    ("boot logo / title silent (the game plays no music there)", loud_by.get("title", (0,))[0] < -80),
    ("girl select music audible", loud_by.get("girl select", (-999,))[0] > -45),
    ("round music audible", loud_by.get("round", (-999,))[0] > -45),
    ("sequence player started", s1["seq_play"] >= 2),
    ("sound effects started", s1["snd_play"] >= 1),
    ("no clipping storm", s1["clipped"] < 0.001 * max(s1["samples"], 1) * 2),
    ("few underruns in the round", d["ai_underruns"] <= 2),
    ("no lost synth updates", s1["params_lost"] == 0),
]
print()
for name, res in checks:
    print("  %s  %s" % ("OK  " if res else "FAIL", name))
allok = all(r for _, r in checks)
print("PASS" if allok else "FAILED")
sys.exit(0 if allok else 1)
