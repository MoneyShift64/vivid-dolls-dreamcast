# Area "audio": the libultra audio library in C, SH4 software mixer, AICA output

Owner files: `port/audio/**`, the audio part of `port/ultra/` (the old `port/ultra/al_stub.c`
was removed and replaced by `port/audio/`), the `port/audio` lines in `port/Makefile.dc`,
this file. `port/ultra/os_sp.c` is **unchanged** (audio tasks still complete at once there).

## Status (2026-11-01)
The port has sound. In headless Flycast: boot logo and title are silent (the game plays no
music there, as on the original), the coin sound plays on the title, song 14 starts at girl
select, song 1 after choosing, song 8 in the round, and sound effects play in the round
(76 in a 27 s round). Soft reset works (music again after the reboot). Audio costs **about
4–4.5 % of the SH4** in a round (emulated; profiler), 2.1 MB of RAM, ~80 ms output latency.

## How it works
The game C (`src/game/audio.c`: Audio_Init, the audio-manager thread AudioMgr_*) is unchanged
and runs as on the N64: every retrace it calls `alAudioFrame`, sends the audio task to the
scheduler, waits for it, and one frame later hands the buffer to `osAiSetNextBuffer`.

| File | What |
|---|---|
| `al_internal.h` | libultra audio structures (ROM layouts: ALCSPlayer 0x7C, ALSndPlayer 0x54, ALCSeq 0xF8, ALVoiceState 0x38, events 0x10, static-asserted), internal API, `vd_audio_stats` |
| `al_bank.c` | heap, links, alCents2Ratio (bit-identical constants to the ROM), alSeqFileNew / alBnkfNew (relocation + BE->native conversion, moved unchanged from al_stub.c) |
| `al_cseq.c` | compact MIDI reader (alCSeqNew/NextEvent, back-references, loops), as the ROM (0x8001D8B0+) |
| `al_csp.c` | compact sequence player, logic taken from the ROM's code (0x8001E370–0x8001F6E0, helpers 0x8002C33C+): voice handler, MIDI (note on with duration, CC 7/10/16/64/91, program, pitch bend), tempo re-timing of note-offs, envelopes, the game's oscillator callbacks, stop/fade |
| `al_sndp.c` | sound player, as the ROM (0x8001F6E0–0x80020150): attack/decay/release, times divided by the pitch (this version does), 16 ms tick |
| `al_evtq.c` | event queue (interrupts off around list operations, like osSetIntMask) |
| `al_synth.c` | synthesizer driver (alInit, alAudioFrame, alSyn*): player callbacks as libultra (all due callbacks at frame start, 16-sample aligned), every alSyn* call becomes a time-stamped update applied at its sample while mixing; voice allocation/stealing/lame list as libultra; then the **software mixer** and the reverb |
| `al_pcm.c` | VADPCM decoder + decoded-sample cache (see below) |
| `vd_ai.c` | osAiSetFrequency/GetLength/SetNextBuffer -> ring buffer -> KOS `snd_stream` (stereo 44100 Hz, 4 KB/channel in sound RAM), polled by a KOS thread `vd_ai` (prio 12, every 4 ms); `vd_audio_stub_reset()` (boot / soft reset hook called by port/sys/main.c) |

Mixer (what the RSP did): per physical voice between its update times, linear-interpolation
resampling by the pitch (ratio capped at 1.99996 as libultra), envelope mixer as libultra
(volume squared, equal-power pan table = round(32767 cos(i pi/254)), dry/wet by the fx mix,
exponential ramps), effect bus = libultra's reverb network for fxType 1 (small room; same
values as the ROM's table) or 6 (the game's table), output saturated to 16 bit.
The audio task still goes to the scheduler (alAudioFrame returns one dummy command), so the
game's audio thread logic (reply wait, osAiSetNextBuffer of the previous buffer) is untouched.

Rate matching: `osAiGetLength` = ring fill above 1536 samples (capped at 768, like a real
DMA buffer; uncapped it made the game's frame size negative). The game then renders 720–832
samples per frame as needed; the AICA clock is the reference, no drift. Output starts (and
restarts after an underrun) once the ring holds 1536 samples.

Decoded-sample cache: at alBnkfNew every distinct wavetable of both banks (48 waves) is
decoded once to PCM (2138 KB, malloc; keyed by ROM address, kept across soft resets; the
wavetable's pad bytes hold the index). The decoder reproduces every loop state of the banks
exactly (checked offline), so a linear decode is exact across loops. If the allocation fails
the mixer decodes on the fly (slower, ~+5 % CPU). Heap top after Audio_Init is 0x8CBD2000,
i.e. ~2.1 MB of the 16 MB stays free after the cache.

### Differences from the N64 (deliberate, for CPU)
* linear interpolation instead of the RSP's 4-tap resampler;
* volume ramps step every 32 samples (RSP: 8), constant-gain runs when no ramp is active;
* voices at pan 64 (all of this game's voices: every sequence pans 64, all samplePan 64)
  are mixed into one centre bus with the mean of the L/R gains (they differ by 0.1 dB);
* the reverb runs at half rate (delays halved, low-pass pole squared = same cutoff, input
  averaged in pairs, output interpolated), processed in 160-sample chunks section by section
  like alFxPull; chorus modulation is not implemented (no game config uses it);
* the stream plays 44100 Hz while the synthesizer is told 44095 (as the N64 AI): 0.01 %.
* ROM quirks kept: `__initFromBank` resets chanState[maxChannels] (one spare entry is
  allocated, so the heap layout moves by 16 bytes); guarded: empty program slots and a sound
  target of -1 (would crash / read out of bounds on the N64; never used by the game).

## Tests
```
make -f port/Makefile.dc && tools/port/make_cdi.sh baserom.z64
python3 -I port/audio/tests/audio_test.py build/dc [--frames 1500]   # ~1.5 min, PASS/FAILED
python3 -I port/audio/tests/sfx_check.py          # uses the capture written by audio_test.py
python3 -I tools/port/dc_harness/boot_test.py build/dc   # still PASS (round 56 fps)
```
`audio_test.py` plays the boot_test path, captures the libretro core's audio (a ctypes
callback installed on the core with `retro_set_audio_sample_batch`; the shared harness is not
modified) to `$DC_HARNESS_DIR/harness/out/audio_test.wav` (+ `.json` with event times), logs the
game's music state / song and the players' play/stop/sound starts per frame, prints loudness
per stage, the `vd_audio_stats` counters and a PC profile of the round. Last results: girl
select -15 dBFS RMS, round -16 dBFS, boot/title silent; 0–2 underruns per run (at start-up /
music changes); a few dozen clipped samples per minute (the game's mix peaks at full scale);
no lost synthesizer updates.
`sfx_check.py`: the coin sound (sound 0, slot pitch 0.5) in the capture vs an independent
Python VADPCM decode of the ROM sample resampled by 0.5: best pitch ratio 1.00, normalised
correlation 0.994, dominant frequency 578.7 Hz both, duration 1.31 s vs 1.27 s, latency 81 ms.

Debug log: `make -f port/Makefile.dc DEBUG=0x41` (category 0x40 = audio: synth init, cache,
sequence play/stop/end, sound starts/ends) -> `port_debug.log` via dcrun.py.

## CPU (emulated, round, avg 4.9 voices, max 17, reverb active 40–65 % of frames)
Mixer kernel 2.5 %, reverb 0.7–1.1 %, frame overhead (players, clear, output) 0.7 %, ring /
stream 0.5 %: ~4–4.5 % total (was 12 % with on-the-fly decoding and the naive mixer).
Flycast ignores cache misses; the PCM is read sequentially, so a real console should be close.

## Open issues / ideas
1. Real-console check not done (GDEMU): listen for crackles (underruns), check latency.
2. Further CPU savings if needed: hand-written SH4 assembly for the two mixing loops (~20
   instructions/sample now), or hardware AICA voices (PCM16 in sound RAM does not fit: 2.1 MB).
3. The music/sfx balance and absolute level follow the N64 maths; not compared with the
   arcade machine.
4. alSndp/alCSP structures are the ROM layouts; `test_audio.py` (difftest) is unaffected
   (game C unchanged).
