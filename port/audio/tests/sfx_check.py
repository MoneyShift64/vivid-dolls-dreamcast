#!/usr/bin/env python3
"""Pitch / duration check of a sound effect in a capture made by audio_test.py.

  python3 -I port/audio/tests/sfx_check.py [capture.wav] [baserom.z64]

Takes the first sound effect that starts while no music plays (the coin sound
on the title screen), decodes the same sample offline from the ROM (VADPCM,
the bank's book; independent Python decoder), resamples it by the slot's pitch
and compares with the capture: best pitch ratio by normalised cross-correlation
(search 0.90..1.10), dominant frequency, and the sound's duration (time above
-40 dB of its peak). Only audio is analysed.
"""
import json
import os
import struct
import sys
import wave

import numpy as np

HERE = os.path.dirname(os.path.abspath(__file__))
ROOT = os.path.abspath(os.path.join(HERE, "..", "..", ".."))
WAV = sys.argv[1] if len(sys.argv) > 1 else os.path.join(os.environ.get("DC_HARNESS_DIR", os.path.expanduser("~/vd-emu")), "harness", "out", "audio_test.wav")
ROM = sys.argv[2] if len(sys.argv) > 2 else os.path.join(ROOT, "baserom.z64")
rom = open(ROM, "rb").read()
SFX_CTL, SFX_TBL = 0x1E2870, 0x1E3EA0


def u32(o): return struct.unpack(">I", rom[o:o + 4])[0]
def s32(o): return struct.unpack(">i", rom[o:o + 4])[0]


def sfx_sound(i):
    b = SFX_CTL + u32(SFX_CTL + 4)
    inst = SFX_CTL + u32(b + 12)
    so = SFX_CTL + u32(inst + 16 + 4 * i)
    wt = SFX_CTL + u32(so + 8)
    env = SFX_CTL + u32(so)
    bo = SFX_CTL + u32(wt + 16)
    order, npred = s32(bo), s32(bo + 4)
    book = struct.unpack(">%dh" % (order * npred * 8), rom[bo + 8:bo + 8 + order * npred * 16])
    return dict(base=SFX_TBL + u32(wt), len=s32(wt + 4), book=book, loop=u32(wt + 12),
                decay=s32(env + 4), release=s32(env + 8))


def decode(s):
    data = rom[s["base"]:s["base"] + s["len"]]
    bk, h, out = s["book"], [0, 0], []
    for f in range(len(data) // 9):
        fr = data[9 * f:9 * f + 9]
        sc, p = min(fr[0] >> 4, 12), fr[0] & 15
        c1, c2 = bk[p * 16:p * 16 + 8], bk[p * 16 + 8:p * 16 + 16]
        nib = []
        for b in fr[1:]:
            for v in (b >> 4, b & 15):
                nib.append((v - 16 if v >= 8 else v) << sc)
        for g in range(2):
            x = nib[8 * g:8 * g + 8]
            o = []
            for i in range(8):
                acc = (x[i] << 11) + c1[i] * h[0] + c2[i] * h[1] + sum(c2[j] * x[i - 1 - j] for j in range(i))
                o.append(max(-32768, min(32767, acc >> 11)))
            h = [o[6], o[7]]
            out += o
    return np.array(out, dtype=np.float64)


def resample(x, ratio):
    n = int(len(x) / ratio)
    t = np.arange(n) * ratio
    return np.interp(t, np.arange(len(x)), x)


w = wave.open(WAV)
cap = np.frombuffer(w.readframes(w.getnframes()), dtype="<i2").reshape(-1, 2).astype(np.float64).mean(axis=1)
meta = json.load(open(WAV + ".json"))
music_start = min([b for lab, b in meta["marks"] if lab == "girl select"] or [len(cap) * 4]) // 4
cands = [e for e in meta["sfx"] if e[0] < music_start]
if not cands:
    print("no sound effect before the music started")
    sys.exit(1)
off, sid, pitch, frame = cands[0]
s = sfx_sound(sid)
ref = resample(decode(s), pitch)
seg = cap[max(off - 2000, 0):min(off + 44100 * 3, music_start)]
print("sound %d (slot pitch %.3f) started at frame %d; reference %d samples (%.3f s)" %
      (sid, pitch, frame, len(ref), len(ref) / 44100.0))


def best_corr(ratio):
    r = resample(decode_cache, pitch * ratio)[:8192]
    c = np.correlate(seg[:len(r) + 8000], r, mode="valid")
    e = np.sqrt(np.convolve(seg[:len(r) + 8000] ** 2, np.ones(len(r)), mode="valid")) * np.sqrt((r ** 2).sum())
    k = int(np.argmax(c / np.maximum(e, 1)))
    return (c / np.maximum(e, 1))[k], k


decode_cache = decode(s)
res = [(best_corr(r), r) for r in np.arange(0.90, 1.1001, 0.01)]
(cbest, lag), rbest = max(res, key=lambda x: x[0][0])
print("best pitch ratio vs expected: %.2f (normalised correlation %.3f, latency %.1f ms)" %
      (rbest, cbest, (lag + max(off - 2000, 0) - off) / 44.1))


def dom_freq(x):
    x = x[:16384] * np.hanning(min(len(x), 16384))
    sp = np.abs(np.fft.rfft(x))
    return np.argmax(sp[5:]) * 44100.0 / len(x) * 1.0 + 5 * 44100.0 / len(x)


start = lag
cs = seg[start:start + len(ref)]
print("dominant frequency: capture %.1f Hz, reference %.1f Hz" % (dom_freq(cs), dom_freq(ref)))


def duration(x):
    env = np.convolve(np.abs(x), np.ones(441) / 441, mode="same")
    th = env.max() * 0.01
    idx = np.nonzero(env > th)[0]
    return (idx[-1] - idx[0]) / 44100.0 if len(idx) else 0.0


dc_, dr = duration(seg[start:]), duration(ref)
print("duration above -40 dB: capture %.3f s, reference %.3f s" % (dc_, dr))
ok = abs(rbest - 1.0) < 0.015 and cbest > 0.8 and abs(dc_ - dr) < 0.1 + 0.1 * dr
print("PASS" if ok else "FAILED")
sys.exit(0 if ok else 1)
