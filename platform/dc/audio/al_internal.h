/* Vivid Dolls port - libultra 2.0H audio library in C: internal declarations.
 *
 * The game (src/game/audio.c) uses the libultra audio library as it was linked
 * into the ROM (asm/libultra.s 0x8001D4A0-0x8002CB30): the compact sequence
 * player (alCSP*), the sound player (alSndp*), the synthesizer driver (alInit,
 * alAudioFrame) and the bank/sequence loaders. This directory is a C version of
 * those pieces. The players and the sequence reader follow the ROM's code
 * (same structures and event logic); the synthesizer's back end does not build
 * RSP audio command lists: alAudioFrame mixes the voices directly on the SH4
 * (ADPCM decode of the bank samples from the ROM image, pitch resampling,
 * volume/pan ramps, the reverb configured by alSynConfig.fxType) into the
 * output buffer the audio manager passed, and osAiSetNextBuffer then queues
 * that PCM for the AICA (vd_ai.c). See port/notes/audio.md.
 *
 * Structure layouts that the game allocates (ALCSPlayer 0x7C, ALSndPlayer 0x54,
 * ALCSeq 0xF8) match the ROM's layouts, so the game's heap use is unchanged.
 */
#ifndef VD_AL_INTERNAL_H
#define VD_AL_INTERNAL_H

#include <kos.h>
#include "ultra_internal.h"

#ifndef VD_DBG_AUDIO
#define VD_DBG_AUDIO 0x40       /* debug-log category of this area (port/sys/vd_debug.h) */
#endif

/* ------------------------------------------------------------ basics */
typedef s32 ALMicroTime;
typedef u8  ALPan;

typedef struct ALLink_s { struct ALLink_s *next, *prev; } ALLink;
typedef struct { u8 *base; u8 *cur; s32 len; s32 count; } ALHeap;

void  alLink(void *ln, void *to);
void  alUnlink(void *ln);
void *alHeapDBAlloc(u8 *file, s32 line, void *hp, s32 num, s32 size);
#define alHeapAlloc(hp, n, s) alHeapDBAlloc(0, 0, (hp), (n), (s))
f32   alCents2Ratio(s32 cents);

/* ---------------------------------------------------------- bank files */
typedef struct { s32 attackTime, decayTime, releaseTime; u8 attackVolume, decayVolume; } ALEnvelope;
typedef struct { u8 velocityMin, velocityMax, keyMin, keyMax, keyBase; s8 detune; } ALKeyMap;
typedef struct { u32 start, end, count; s16 state[16]; } ALADPCMloop;
typedef struct { u32 start, end, count; } ALRawLoop;
typedef struct { s32 order, npredictors; s16 book[1]; } ALADPCMBook;
typedef struct {
    u8 *base;               /* after alBnkfNew: ROM address of the sample data */
    s32 len;
    u8 type;                /* AL_ADPCM_WAVE 0, AL_RAW16_WAVE 1 */
    u8 flags;
    u8 pad[2];
    union {
        struct { ALADPCMloop *loop; ALADPCMBook *book; } adpcm;
        struct { ALRawLoop *loop; } raw;
    } waveInfo;
} ALWaveTable;
typedef struct {
    ALEnvelope *envelope;
    ALKeyMap *keyMap;
    ALWaveTable *wavetable;
    u8 samplePan, sampleVolume, flags;
} ALSound;
typedef struct {
    u8 volume, pan, priority, flags;
    u8 tremType, tremRate, tremDepth, tremDelay;
    u8 vibType, vibRate, vibDepth, vibDelay;
    s16 bendRange;
    s16 soundCount;
    ALSound *soundArray[1];
} ALInstrument;
typedef struct {
    s16 instCount;
    u8 flags, pad;
    s32 sampleRate;
    ALInstrument *percussion;
    ALInstrument *instArray[1];
} ALBank;
typedef struct { s16 revision, bankCount; ALBank *bankArray[1]; } ALBankFile;
typedef struct { u8 *offset; s32 len; } ALSeqData;
typedef struct { s16 revision, seqCount; ALSeqData seqArray[1]; } ALSeqFile;

#define AL_BANK_VERSION 0x4231   /* 'B1' */
#define AL_ADPCM_WAVE 0
#define AL_RAW16_WAVE 1

#define AL_STOPPED  0
#define AL_PLAYING  1
#define AL_STOPPING 2

#define AL_PAN_CENTER 64
#define AL_PAN_LEFT   0
#define AL_PAN_RIGHT  127
#define AL_VOL_FULL   127

/* ------------------------------------------------------- events, queue */
enum {
    AL_SEQ_REF_EVT, AL_SEQ_MIDI_EVT, AL_SEQP_MIDI_EVT, AL_TEMPO_EVT, AL_SEQ_END_EVT,
    AL_NOTE_END_EVT, AL_SEQP_ENV_EVT, AL_SEQP_META_EVT, AL_SEQP_PROG_EVT, AL_SEQP_API_EVT,
    AL_SEQP_VOL_EVT, AL_SEQP_LOOP_EVT, AL_SEQP_PRIORITY_EVT, AL_SEQP_SEQ_EVT, AL_SEQP_BANK_EVT,
    AL_SEQP_PLAY_EVT, AL_SEQP_STOP_EVT, AL_SEQP_STOPPING_EVT, AL_TRACK_END, AL_CSP_LOOPSTART,
    AL_CSP_LOOPEND, AL_CSP_NOTEOFF_EVT, AL_TREM_OSC_EVT, AL_VIB_OSC_EVT
};

struct ALVoice_s;
struct ALVoiceState_s;

typedef struct {                       /* 0x10 bytes, as in the ROM */
    s16 type;
    union {
        struct { s32 ticks; u8 status, byte1, byte2, pad; u32 duration; } midi;
        struct { s32 ticks; u8 status, type, len, byte1, byte2, byte3; } tempo;
        struct { struct ALVoice_s *voice; s32 delta; u8 vol; } vol;          /* ENV evt */
        struct { struct ALVoice_s *voice; } note;                           /* NOTE_END */
        struct { struct ALVoiceState_s *vs; void *oscState; u8 chan; } osc;
        struct { s16 vol; } spvol;
        struct { u8 chan, priority; } sppriority;
        struct { void *seq; } spseq;
        struct { ALBank *bank; } spbank;
        struct { void *state; union { f32 pitch; s16 vol; u8 pan; u8 fxmix; } v; } snd;
        u32 raw[3];
    } msg;
} ALEvent;

typedef struct {
    ALLink node;
    ALMicroTime delta;
    ALEvent evt;
} ALEventListItem;                     /* 0x1C */

typedef struct {
    ALLink freeList;
    ALLink allocList;
    s32 eventCount;
} ALEventQueue;                        /* 0x14 */

#define AL_EVTQ_END 0x7FFFFFFF

void        alEvtqNew(ALEventQueue *evtq, ALEventListItem *items, s32 itemCount);
ALMicroTime alEvtqNextEvent(ALEventQueue *evtq, ALEvent *evt);
void        alEvtqPostEvent(ALEventQueue *evtq, ALEvent *evt, ALMicroTime delta);
void        alEvtqFlushType(ALEventQueue *evtq, s16 type);

/* ---------------------------------------------------------- synthesizer */
struct PVoice_s;

typedef struct ALPlayer_s {
    struct ALPlayer_s *next;
    void *clientData;
    ALMicroTime (*handler)(void *);
    ALMicroTime callTime;
    s32 samplesLeft;                   /* absolute sample time of the next call */
} ALPlayer;                            /* 0x14 */

typedef struct ALVoice_s {
    ALLink node;
    struct PVoice_s *pvoice;
    ALWaveTable *table;
    void *clientPrivate;
    s16 state;
    s16 priority;
    s16 fxBus;
    s16 unityPitch;
} ALVoice;                             /* 0x1C */

typedef struct { s16 priority; s16 fxBus; u8 unityPitch; } ALVoiceConfig;

typedef void ALSynth;                  /* players keep the pointer only; one synth */

void alSynAddPlayer(ALSynth *drvr, ALPlayer *client);
s32  alSynAllocVoice(ALSynth *drvr, ALVoice *voice, ALVoiceConfig *vc);
void alSynFreeVoice(ALSynth *drvr, ALVoice *voice);
void alSynStartVoice(ALSynth *drvr, ALVoice *voice, ALWaveTable *w);
void alSynStartVoiceParams(ALSynth *drvr, ALVoice *voice, ALWaveTable *w, f32 pitch, s16 vol,
                           ALPan pan, u8 fxmix, ALMicroTime t);
void alSynStopVoice(ALSynth *drvr, ALVoice *voice);
void alSynSetVol(ALSynth *drvr, ALVoice *voice, s16 vol, ALMicroTime t);
void alSynSetPitch(ALSynth *drvr, ALVoice *voice, f32 pitch);
void alSynSetPan(ALSynth *drvr, ALVoice *voice, ALPan pan);
void alSynSetFXMix(ALSynth *drvr, ALVoice *voice, u8 fxmix);
void alSynSetPriority(ALSynth *drvr, ALVoice *voice, s16 priority);

extern void *alGlobals;                /* the ALGlobals block passed to alInit (opaque) */

/* -------------------------------------------- compact sequence (cseq.c) */
typedef struct {
    u8  *base;                         /* 0x00 ALCMidiHdr (big-endian bytes) */
    u32  validTracks;                  /* 0x04 */
    f32  qnpt;                         /* 0x08 */
    u32  lastTicks;                    /* 0x0C */
    u32  lastDeltaTicks;               /* 0x10 */
    u32  deltaFlag;                    /* 0x14 */
    u8  *curLoc[16];                   /* 0x18 */
    u8  *curBUPtr[16];                 /* 0x58 */
    u8   curBULen[16];                 /* 0x98 */
    u8   lastStatus[16];               /* 0xA8 */
    u32  evtDeltaTicks[16];            /* 0xB8 */
} ALCSeq;                              /* 0xF8 */

void alCSeqNew(void *seq, u8 *ptr);
void alCSeqNextEvent(ALCSeq *seq, ALEvent *evt);
s32  __alCSeqNextDelta(ALCSeq *seq, s32 *pDeltaTicks);

/* ------------------------------------------------ compact seq player */
typedef s32  (*ALOscInit)(void **oscState, f32 *initVal, u8 oscType, u8 oscRate, u8 oscDepth, u8 oscDelay);
typedef s32  (*ALOscUpdate)(void *oscState, f32 *updateVal);
typedef void (*ALOscStop)(void *oscState);

typedef struct {
    ALInstrument *instrument;          /* 0x0 */
    s16 bendRange;                     /* 0x4 */
    u8  fxId;                          /* 0x6 */
    u8  pan;                           /* 0x7 */
    u8  priority;                      /* 0x8 */
    u8  vol;                           /* 0x9 */
    u8  fxmix;                         /* 0xA */
    u8  sustain;                       /* 0xB */
    f32 pitchBend;                     /* 0xC */
} ALChanState;                         /* 0x10 */

typedef struct ALVoiceState_s {
    struct ALVoiceState_s *next;       /* 0x00 */
    ALVoice voice;                     /* 0x04 */
    ALSound *sound;                    /* 0x20 */
    ALMicroTime envEndTime;            /* 0x24 */
    f32 pitch;                         /* 0x28 */
    f32 vibrato;                       /* 0x2C */
    u8  envGain;                       /* 0x30 */
    u8  channel;                       /* 0x31 */
    u8  key;                           /* 0x32 */
    u8  velocity;                      /* 0x33 */
    u8  envPhase;                      /* 0x34 */
    u8  phase;                         /* 0x35 */
    u8  tremelo;                       /* 0x36 */
    u8  flags;                         /* 0x37 bit 0 tremolo osc, bit 1 vibrato osc */
} ALVoiceState;                        /* 0x38 */

typedef struct {
    ALPlayer node;                     /* 0x00 */
    ALSynth *drvr;                     /* 0x14 */
    ALCSeq *target;                    /* 0x18 */
    ALMicroTime curTime;               /* 0x1C */
    ALBank *bank;                      /* 0x20 */
    s32 uspt;                          /* 0x24 */
    s32 nextDelta;                     /* 0x28 */
    s32 state;                         /* 0x2C */
    u16 chanMask;                      /* 0x30 */
    s16 vol;                           /* 0x32 */
    u8  maxChannels;                   /* 0x34 */
    u8  debugFlags;                    /* 0x35 */
    u8  pad36[2];
    ALEvent nextEvent;                 /* 0x38 */
    ALEventQueue evtq;                 /* 0x48 */
    ALMicroTime frameTime;             /* 0x5C */
    ALChanState *chanState;            /* 0x60 */
    ALVoiceState *vAllocHead;          /* 0x64 */
    ALVoiceState *vAllocTail;          /* 0x68 */
    ALVoiceState *vFreeList;           /* 0x6C */
    ALOscInit initOsc;                 /* 0x70 */
    ALOscUpdate updateOsc;             /* 0x74 */
    ALOscStop stopOsc;                 /* 0x78 */
} ALCSPlayer;                          /* 0x7C */

typedef struct {
    s32 maxVoices;
    s32 maxEvents;
    u8  maxChannels;
    u8  debugFlags;
    void *heap;
    void *initOsc;
    void *updateOsc;
    void *stopOsc;
} ALSeqpConfig;

/* ---------------------------------------------------------- sound player */
typedef struct {
    ALVoice voice;                     /* 0x00 */
    ALSound *sound;                    /* 0x1C */
    s16 priority;                      /* 0x20 */
    u8  pad22[2];
    f32 pitch;                         /* 0x24 */
    s32 state;                         /* 0x28 */
    s16 vol;                           /* 0x2C */
    ALPan pan;                         /* 0x2E */
    u8  fxMix;                         /* 0x2F */
} ALSoundState;                        /* 0x30 */

typedef struct {
    ALPlayer node;                     /* 0x00 */
    ALEventQueue evtq;                 /* 0x14 */
    ALEvent nextEvent;                 /* 0x28 */
    ALSynth *drvr;                     /* 0x38 */
    s32 target;                        /* 0x3C */
    ALSoundState *sndState;            /* 0x40 */
    s32 maxSounds;                     /* 0x44 */
    ALMicroTime frameTime;             /* 0x48 */
    ALMicroTime nextDelta;             /* 0x4C */
    ALMicroTime curTime;               /* 0x50 */
} ALSndPlayer;                         /* 0x54 */

typedef struct { s32 maxSounds; s32 maxEvents; void *heap; } ALSndpConfig;

/* ------------------------------------------------------- port internals */
/* vd_ai.c: AICA output */
void vd_ai_reset(void);
void vd_ai_init_once(void);

/* al_synth.c, al_bank.c */
void vd_synth_reset(void);
void vd_bank_reset(void);

/* al_pcm.c */
void vd_adpcm_frame(const u8 *in, const s16 *book, s32 npred, s16 *out, s32 *h2p, s32 *h1p);
void vd_pcm_cache_bankfile(void *f);
const s16 *vd_pcm_lookup(const ALWaveTable *w, s32 *nsamp);

/* statistics (read by the harness tests; always compiled in) */
typedef struct {
    u32 frames;                /* alAudioFrame calls that mixed */
    u32 samples;               /* output samples mixed */
    u32 mix_us;                /* time spent in alAudioFrame (handlers + mixing) */
    u32 mix_us_max;
    u32 voices_mixed;          /* sum over frames of active voices */
    u32 voices_max;
    u32 notes;                 /* note-ons started by the sequence player */
    u32 seq_play, seq_stop;    /* sequence player PLAY / STOPPED transitions */
    u32 snd_play, snd_end;     /* sound player starts / ends */
    s32 last_snd_sound;        /* index (alSndpAllocate id) of the last started sound */
    u32 ai_buffers;            /* osAiSetNextBuffer calls */
    u32 ai_samples;            /* samples queued */
    u32 ai_dropped;            /* samples dropped (ring overflow) */
    u32 ai_underruns;          /* stream callbacks that found too little data */
    u32 ai_fill;               /* ring fill after the last osAiSetNextBuffer */
    u32 ai_pulled;             /* samples handed to the AICA */
    u32 params_lost;           /* synthesizer updates dropped (pool empty) */
    u32 peak;                  /* max |sample| of the last 64 frames */
    u32 clipped;               /* output samples clamped */
    u32 fx_frames;             /* frames with the reverb running */
    u32 heap_top;              /* sbrk(0) after Audio_Init (diagnostic: RAM left = 0x8D000000 - this) */
    u32 pcm_kb;                /* decoded-sample cache size */
    u32 pcm_waves;             /* wavetables in the cache */
} VdAudioStats;
extern volatile VdAudioStats vd_audio_stats;

#endif
