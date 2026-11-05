/* Vivid Dolls decompilation - audio (RAM 0x8000D150-0x8000F3E0).
 *
 * Music: one compact-sequence player (ALCSPlayer) playing songs from the S1
 * sequence file with the music bank. Sound effects: one ALSndPlayer with 29
 * sounds (the sounds of instrument 0 of the sound-effect bank); the game
 * requests changes through the SfxSlot table and the audio thread applies them
 * once per frame. The audio thread itself is the N64 SDK audio manager
 * (audiomgr.c) with the game's per-frame sound/music handling added.
 */
#include "audio.h"
#include "libm.h"
#include "siff.h"     /* Rom_Read */

/* ---- libultra (unnamed ones by address) ---- */
extern void  alLink(void *ln, void *to);
extern void  alUnlink(void *ln);
extern void *alHeapDBAlloc(u8 *file, s32 line, void *heap, s32 num, s32 size);
extern f32   alCents2Ratio(s32 cents);
extern void  osCreateMesgQueue(void *mq, void *msg, s32 count);
extern void  osCreateThread(void *t, s32 id, void (*entry)(void *), void *arg, void *sp, s32 pri);
extern void  osStartThread(void *t);
extern s32   osRecvMesg(void *mq, void *msg, s32 flag);
extern s32   osSendMesg(void *mq, void *msg, s32 flag);
extern s32   osSetEventMesg(s32 event, void *mq, void *msg);
extern u32   osVirtualToPhysical(void *addr);
extern void  osViBlack(u8 active);

extern void  func_8001D820(void *heap, u8 *base, s32 len);
extern void  func_8001D4A0(void *file, u32 base);
extern void  func_8001D4E0(void *ctl, u32 tbl);
extern void  func_8001D770(void *g, void *config);
extern void  func_8001D79C(void *g);
extern void  func_8001D8B0(void *seq, u8 *data);
extern s16   func_8001E370(void *seqp);
extern s32   func_8001E380(void *seqp);
extern void  func_8001E390(void *seqp);
extern void  func_8001E3C0(void *seqp, void *bank);
extern void  func_8001E3F0(void *seqp, void *seq);
extern void  func_8001E420(void *seqp, s16 vol);
extern void  func_8001E450(void *seqp);
extern void  func_8001E480(void *seqp, void *config);
extern void  func_8001F6E0(void *sndp, void *config);
extern s16   func_8001FDE0(void *sndp, void *sound);
extern void  func_8001FEA0(void *sndp, s16 id);
extern void  func_8001FEF0(void *sndp, s16 id);
extern void  func_8001FF00(void *sndp);
extern s16   func_8001FF60(void *sndp);
extern void  func_8001FF70(void *sndp);
extern s32   func_8001FFC0(void *sndp);
extern void  func_8001FFE0(void *sndp, f32 pitch);
extern void  func_80020030(void *sndp, s16 id, u8 priority);
extern void  func_80020060(void *sndp, s16 vol);
extern void  func_800200B0(void *sndp, u8 pan);
extern void  func_80020100(void *sndp, u8 fxMix);
extern Acmd *func_8002041C(Acmd *cmdList, s32 *cmdLen, s16 *outBuf, s32 outLen);
extern s32   func_80021000(void);
extern s32   func_80021010(u32 freq);
extern s32   func_80021140(void *buf, u32 size);
extern void  func_80025950(void *mb, s32 pri, s32 dir, u32 devAddr, void *vAddr, u32 nbytes, void *mq);
extern s32   func_800222E0(void *mq, u8 *bitpattern, void *status);
extern void  func_8000C6D8(void *sched, void *client, void *mq);   /* osScAddClient */
extern void  func_8000F6EC(void);

#define alHeapInit(h, b, l)        func_8001D820(h, b, l)
#define alHeapAlloc(h, n, s)       alHeapDBAlloc(0, 0, h, n, s)
#define alSeqFileNew(f, b)         func_8001D4A0(f, b)
#define alBnkfNew(c, t)            func_8001D4E0(c, t)
#define alInit(g, c)               func_8001D770(g, c)
#define alClose(g)                 func_8001D79C(g)
#define alCSeqNew(s, d)            func_8001D8B0(s, d)
#define alCSPGetVol(p)             func_8001E370(p)
#define alCSPGetState(p)           func_8001E380(p)
#define alCSPPlay(p)               func_8001E390(p)
#define alCSPSetBank(p, b)         func_8001E3C0(p, b)
#define alCSPSetSeq(p, s)          func_8001E3F0(p, s)
#define alCSPSetVol(p, v)          func_8001E420(p, v)
#define alCSPStop(p)               func_8001E450(p)
#define alCSPNew(p, c)             func_8001E480(p, c)
#define alSndpNew(p, c)            func_8001F6E0(p, c)
#define alSndpAllocate(p, s)       func_8001FDE0(p, s)
#define alSndpDeallocate(p, i)     func_8001FEA0(p, i)
#define alSndpSetSound(p, i)       func_8001FEF0(p, i)
#define alSndpPlay(p)              func_8001FF00(p)
#define alSndpGetSound(p)          func_8001FF60(p)
#define alSndpStop(p)              func_8001FF70(p)
#define alSndpGetState(p)          func_8001FFC0(p)
#define alSndpSetPitch(p, f)       func_8001FFE0(p, f)
#define alSndpSetPriority(p, i, r) func_80020030(p, i, r)
#define alSndpSetVol(p, v)         func_80020060(p, v)
#define alSndpSetPan(p, v)         func_800200B0(p, v)
#define alSndpSetFXMix(p, v)       func_80020100(p, v)
#define alAudioFrame(l, n, o, s)   func_8002041C(l, n, o, s)
#define osAiGetLength()            func_80021000()
#define osAiSetFrequency(f)        func_80021010(f)
#define osAiSetNextBuffer(b, s)    func_80021140(b, s)
#define osPiStartDma(m, p, d, a, v, n, q) func_80025950(m, p, d, a, v, n, q)
#define osContInit(q, b, s)        func_800222E0(q, b, s)
#define osScAddClient(s, c, q)     func_8000C6D8(s, c, q)

/* ---- library structures used through raw offsets ---- */
typedef struct { s16 instCount; u8 flags; u8 pad; s32 sampleRate; void *percussion; void *instArray[1]; } ALBank;
typedef struct { s16 revision; s16 bankCount; ALBank *bankArray[1]; } ALBankFile;
typedef struct { u8 volume, pan, priority, flags; u8 trem[4]; u8 vib[4]; s16 bendRange; s16 soundCount; void *soundArray[1]; } ALInstrument;
typedef struct { u32 offset; s32 len; } ALSeqData;
typedef struct { s16 revision; s16 seqCount; ALSeqData seqArray[1]; } ALSeqFile;

/* ---- ROM data ---- */
#define ROM_MUSIC_CTL 0x196CC0
#define ROM_MUSIC_TBL 0x1982B0
#define ROM_SEQ_FILE  0x1D7720
#define ROM_SFX_CTL   0x1E2870
#define ROM_SFX_TBL   0x1E3EA0

#define AUDIO_HEAP_SIZE   0x5F000
#define DMA_BUFFER_LENGTH 0x600
#define NUM_DMA_BUFFERS   128
#define NUM_OSC_STATES    80
#define NUM_SFX_SLOTS     29

/* ---- globals (.data) ---- */
extern volatile s32 D_80038480;   /* audio thread busy with a frame */
extern volatile s32 D_80038484;   /* pre-NMI received: sound is shut off */
extern volatile u16 D_80038488;   /* sound being watched for its end (0xFFFF none) */
extern volatile u16 D_8003848A;   /* value read by Music_Stop (seqp+0x32) */
extern volatile u16 D_8003848C;   /* music fade: send alCSPStop next frame */
extern u16 D_8003848E;            /* loaded sfx instrument (0xFFFF none) */
extern u32 D_80038490;            /* current song number */
extern volatile s32 D_80038498;   /* music state, MUSIC_* */
extern s32 D_8003849C;            /* sfx sounds allocated */
extern volatile s32 D_800384A0;   /* Music_Play waiting for the fade to end */
extern volatile u32 D_800384A4;   /* audFrameCt */
extern u32 D_800384A8;            /* nextDMA */
extern u32 D_800384AC;            /* curAcmdList */
extern u8  D_800384B0[];          /* per sound id: number of variants (slots) */
extern u8  D_800384CC[];          /* per sound id: first slot */
extern f32 D_800384E8[];          /* per slot: default pitch */
extern s32 D_8003D6B0[26];        /* custom reverb parameters (fxType 6) */
extern u8  D_80033FA0[], D_80034070[], D_8003B790[];  /* rspboot, aspMain text, aspMain data */

/* ---- globals (.bss) ---- */
extern u8   D_80043350[AUDIO_HEAP_SIZE];  /* audio heap memory */
extern void *D_80043330;                   /* OSMesgQueue: music fade finished */
extern void *D_80043348;                   /* its message buffer */
extern void *D_800A2350;                   /* ALCSPlayer * (music) */
extern void *D_800A2354;                   /* ALCSeq * */
extern void *D_800A2358;                   /* ALSndPlayer * (sfx) */
extern void *D_800A235C;                   /* last ALSound * allocated */
extern ALBank *D_800A2360;                 /* sfx bank */
extern ALInstrument *D_800A2364;           /* current sfx instrument */
extern s16 *D_800A2368;                    /* sound id per slot [29] */
extern s16 D_800A236C;                     /* music volume */
extern u8  *D_800A2370;                    /* sequence data buffer (0x2000) */
extern s32 D_800A2374;                     /* sequence file header size */
extern ALSeqFile *D_800A2378;              /* sequence file header */
extern u8  D_800A2380[];                   /* ALHeap */
extern AudioMgr D_800A2390;                /* __am */
extern u8  D_800A47E0[];                   /* top of audio thread stack == dmaState */
extern AMDMABuffer D_800A47F0[NUM_DMA_BUFFERS];
extern u32 D_800A51F0;                     /* minFrameSize */
extern u32 D_800A51F4;                     /* frameSize */
extern u32 D_800A51F8;                     /* maxFrameSize */
extern u8  D_800A5200[][0x18];             /* OSIoMesg audDMAIOMesgBuf[128] */
extern u8  D_800A5E00[];                   /* OSMesgQueue audDMAMessageQ */
extern u8  D_800A5E18[];                   /* its buffer [128] */
extern SfxSlot D_800A6018[NUM_SFX_SLOTS];
extern u8  D_800A6178[];                   /* per sound id: next variant */
extern OscState *D_800A6194;               /* free oscillator states */
extern OscState D_800A61A0[NUM_OSC_STATES];
extern u8  D_800B0508[];                   /* OSMesgQueue: watched sound ended */
extern void *D_800B0568;                   /* scheduler command queue */
extern u8  D_800C0AC0[];                   /* scheduler */
extern u8  D_800C53DC[];

#define dmaState   (*(AMDMAState *)D_800A47E0)
#define audFrameCt D_800384A4

/* @ 0x8000D150 */
/* Empty function. */
void Audio_Nop(void)
{
}

/* @ 0x8000D160 */
/* Start fading out the music if it is playing. */
void Music_Stop(void)
{
    if (D_80038498 == MUSIC_PLAYING && alCSPGetState(D_800A2350) != 0) {
        D_800A236C = 0x7800;
        D_8003848A = alCSPGetVol(D_800A2350);
        D_8003848C = 16;
        D_80038498 = MUSIC_FADING;
    }
}

/* @ 0x8000D1CC */
/* Play song number `song` (0 if out of range): fade out the current one and
 * wait (blocking) for the audio thread to finish it, then load the sequence
 * from ROM and hand it to the player, which starts it next frame. */
void Music_Play(u32 song)
{
    void *msg = 0;
    ALSeqData *sd;

    if (D_80038484 != 0)
        return;
    if (D_80038498 == MUSIC_PLAYING)
        Music_Stop();
    if (D_80038498 == MUSIC_FADING) {
        D_800384A0 = 1;
        if (D_80038498 == MUSIC_FADING)
            osRecvMesg(&D_80043330, &msg, 1);
        D_800384A0 = 0;
    }
    if (song < (u32)(s32)D_800A2378->seqCount)
        D_80038490 = song;
    else
        D_80038490 = 0;
    sd = &D_800A2378->seqArray[D_80038490];
    D_80038498 = MUSIC_LOADING;
    Rom_Read(sd->offset, D_800A2370, sd->len + (sd->len & 1));
    alCSeqNew(D_800A2354, D_800A2370);
    alCSPSetSeq(D_800A2350, D_800A2354);
    alCSPSetVol(D_800A2350, D_800A236C);
    D_80038498 = MUSIC_START;
}

/* @ 0x8000D368 */
/* Play sound effect `id` with its default pitch, volume 0x7F00, centre pan.
 * Each id owns D_800384B0[id] consecutive slots starting at D_800384CC[id],
 * used in rotation. Returns the slot's sound id, or -1. */
s16 Sfx_Play(u16 id)
{
    u32 slot, var;

    if (D_80038484 != 0)
        return -1;
    var = D_800A6178[id];
    slot = (u8)(D_800384CC[id] + var);
    if ((s32)slot >= D_800A2364->soundCount)
        return -1;
    D_800A6018[slot].pitch = D_800384E8[slot];
    D_800A6178[id] = (s32)(var + 1) % (s32)D_800384B0[id];
    D_800A6018[slot].vol = 0x7F00;
    D_800A6018[slot].pan = 0x40;
    D_800A6018[slot].fxMix = 0;
    if (D_800A6018[slot].state != SFX_RESTART)
        D_800A6018[slot].state = SFX_START;
    return D_800A2368[slot];
}

/* @ 0x8000D49C */
/* Like Sfx_Play with explicit pitch, volume, pan and effect mix. */
s16 Sfx_PlayEx(u16 id, f32 pitch, s16 vol, u8 pan, u8 fxMix)
{
    u32 slot, var;

    if (D_80038484 != 0)
        return -1;
    var = D_800A6178[id];
    slot = (u8)(D_800384CC[id] + var);
    if ((s32)slot >= D_800A2364->soundCount)
        return -1;
    D_800A6018[slot].pitch = pitch;
    D_800A6178[id] = (s32)(var + 1) % (s32)D_800384B0[id];
    D_800A6018[slot].vol = vol;
    D_800A6018[slot].pan = pan;
    D_800A6018[slot].fxMix = fxMix;
    if (D_800A6018[slot].state != SFX_RESTART)
        D_800A6018[slot].state = SFX_START;
    return D_800A2368[slot];
}

/* @ 0x8000D5C4 */
/* Change a slot's pitch/volume/pan/fx; applied next frame unless the slot is
 * about to (re)start anyway. */
void Sfx_SetSlotParams(u16 slot, f32 pitch, s16 vol, u8 pan, u8 fxMix)
{
    SfxSlot *s;

    if (D_80038484 != 0 || slot >= D_800A2364->soundCount)
        return;
    s = &D_800A6018[slot];
    s->pitch = pitch;
    s->vol = vol;
    s->pan = pan;
    s->fxMix = fxMix;
    if (s->state != SFX_START && s->state != SFX_RESTART)
        s->state = SFX_UPDATE;
}

/* @ 0x8000D67C */
/* Request a stop of one slot. */
void Sfx_StopSlot(u16 slot)
{
    if (slot < D_800A2364->soundCount)
        D_800A6018[slot].state = SFX_STOP;
}

/* @ 0x8000D6B8 */
/* Request a stop of every slot. */
void Sfx_StopAll(void)
{
    s16 i;

    for (i = 0; i < D_800A2364->soundCount; i++)
        D_800A6018[i].state = SFX_STOP;
}

/* @ 0x8000D720 */
/* Allocate the sounds of sfx-bank instrument `inst` in the sound player,
 * freeing those of the previous one. Quirk kept: only instrument 0 is ever
 * accepted (any other value returns without doing anything). */
void Sfx_LoadInstrument(u16 inst)
{
    u16 cur = D_8003848E;
    u32 i;

    if (inst == cur || inst != 0)
        return;
    if (cur != 0xFFFF) {
        D_800A2364 = D_800A2360->instArray[cur];
        for (i = 0; i < (u32)(s32)D_800A2364->soundCount; i++) {
            alSndpSetSound(D_800A2358, D_800A2368[i]);
            alSndpStop(D_800A2358);
            alSndpDeallocate(D_800A2358, D_800A2368[i]);
        }
    }
    D_800A2364 = D_800A2360->instArray[inst];
    for (i = 0; i < (u32)(s32)D_800A2364->soundCount; i++) {
        D_800A235C = D_800A2364->soundArray[i];
        D_800A2368[i] = alSndpAllocate(D_800A2358, D_800A235C);
    }
    D_8003848E = inst;
    D_8003849C = 1;
}

/* @ 0x8000D890 */
/* Watch a slot: the audio thread posts to D_800B0508 when it stops playing.
 * (The sound player target is switched to it and back, as the original does.) */
void Sfx_SetWatchSound(u16 slot)
{
    s16 old;

    if (slot < D_800A2364->soundCount && slot != 0xFFFF) {
        old = alSndpGetSound(D_800A2358);
        alSndpSetSound(D_800A2358, (s16)slot);
        D_80038488 = slot;
        if (old != -1)
            alSndpSetSound(D_800A2358, old);
    } else {
        D_80038488 = 0xFFFF;
    }
}

/* ALSynConfig as built on the stack */
typedef struct {
    s32 maxVVoices;     /* 0x00 */
    s32 maxPVoices;     /* 0x04 */
    s32 maxUpdates;     /* 0x08 */
    s32 unk0C;
    void *dmaproc;      /* 0x10 */
    void *heap;         /* 0x14 */
    s32 outputRate;     /* 0x18 */
    u8  fxType;         /* 0x1C */
    u8  pad1D[3];
    s32 *params;        /* 0x20 */
} SynConfig;

typedef struct {
    s32 maxVoices;      /* 0x00 */
    s32 maxEvents;      /* 0x04 */
    u8  maxChannels;    /* 0x08 */
    u8  debugFlags;
    u8  pad[2];
    void *heap;         /* 0x0C */
    void *initOsc;      /* 0x10 */
    void *updateOsc;    /* 0x14 */
    void *stopOsc;      /* 0x18 */
} SeqpConfig;

typedef struct {
    s32 maxSounds;
    s32 maxEvents;
    void *heap;
} SndpConfig;

/* @ 0x8000D93C */
/* Set up all audio: clear the 0x5F000-byte audio heap, allocate the players,
 * load both banks' .ctl files and the sequence-file header from ROM, start
 * the audio manager (44.1 kHz) and its thread, and allocate the sfx sounds. */
void Audio_Init(void)
{
    SynConfig c;
    SeqpConfig seqc;
    SndpConfig sndc;
    u32 amc[3];
    u32 i, size;
    ALBankFile *musicCtl, *sfxCtl;

    dmaState.initialized = 0;
    for (i = 0; i <= AUDIO_HEAP_SIZE - 1; i++)
        ((volatile u8 *)D_80043350)[i] = 0;
    alHeapInit(D_800A2380, D_80043350, AUDIO_HEAP_SIZE);
    D_800A2354 = alHeapAlloc(D_800A2380, 1, 0xF8);
    D_800A2350 = alHeapAlloc(D_800A2380, 1, 0x7C);
    D_800A2358 = alHeapAlloc(D_800A2380, 1, 0x54);

    size = ROM_MUSIC_TBL - ROM_MUSIC_CTL;
    musicCtl = alHeapAlloc(D_800A2380, 1, size);
    Rom_Read(ROM_MUSIC_CTL, musicCtl, size);
    size = ROM_SFX_TBL - ROM_SFX_CTL;
    sfxCtl = alHeapAlloc(D_800A2380, 1, size);
    Rom_Read(ROM_SFX_CTL, sfxCtl, size);

    D_800A2378 = alHeapAlloc(D_800A2380, 1, 4);
    Rom_Read(ROM_SEQ_FILE, D_800A2378, 4);
    /* endian: raw ROM header (big-endian); alSeqFileNew has not converted it yet.
     * The port's alSeqFileNew/alBnkfNew must convert the files to native order in
     * place (see port/ENDIAN_AUDIT.md); everything after them reads native fields. */
    D_800A2374 = BE16S((u8 *)D_800A2378 + 2) * 8 + 4;
    D_800A2378 = alHeapAlloc(D_800A2380, 1, D_800A2374);
    Rom_Read(ROM_SEQ_FILE, D_800A2378, D_800A2374);
    D_800A2370 = alHeapAlloc(D_800A2380, 1, 0x2000);

    c.maxVVoices = 40;
    c.maxPVoices = 40;
    c.maxUpdates = 256;
    c.dmaproc = (void *)AudioMgr_DmaNew;
    c.outputRate = osAiSetFrequency(44100);
    c.fxType = 1;
    c.heap = D_800A2380;
    amc[0] = 44100;     /* outputRate */
    amc[1] = 1;         /* framesPerField */
    amc[2] = 0x1800;    /* maxACMDSize */
    AudioMgr_Create(&c, 14, amc);

    alSeqFileNew(D_800A2378, ROM_SEQ_FILE);
    alBnkfNew(musicCtl, ROM_MUSIC_TBL);
    seqc.maxVoices = 40;
    seqc.maxEvents = 256;
    seqc.maxChannels = 16;
    seqc.heap = D_800A2380;
    seqc.initOsc = (void *)Osc_Init;
    seqc.updateOsc = (void *)Osc_Update;
    seqc.stopOsc = (void *)Osc_Stop;
    alCSPNew(D_800A2350, &seqc);
    alCSPSetBank(D_800A2350, musicCtl->bankArray[0]);

    alBnkfNew(sfxCtl, ROM_SFX_TBL);
    D_800A2360 = sfxCtl->bankArray[0];
    D_800A2368 = alHeapAlloc(D_800A2380, 1, NUM_SFX_SLOTS * 2);
    sndc.maxSounds = NUM_SFX_SLOTS;
    sndc.maxEvents = 256;
    sndc.heap = D_800A2380;
    alSndpNew(D_800A2358, &sndc);

    osStartThread(D_800A2390.thread);
    D_800A236C = 0x7800;
    alCSPSetVol(D_800A2350, D_800A236C);
    Sfx_LoadInstrument(0);
}

/* @ 0x8000DCE0 */
/* amCreateAudioMgr: work out the frame sizes, init the synthesizer, the
 * oscillator free list, 128 DMA cache buffers, 2 command lists, 3 output
 * buffers, the message queues and the audio thread. Quirk: `pri` is ignored
 * (the thread always gets priority 14). amConfig = {outputRate,
 * framesPerField, maxACMDSize}. */
void AudioMgr_Create(void *synConfig, s32 pri, u32 *amConfig)
{
    SynConfig *c = synConfig;
    s32 params[26];
    f32 fsize;
    u32 i;
    AudioInfo *info;

    (void)pri;
    fsize = (f32)amConfig[1] * (f32)c->outputRate / 60.0f;
    D_800A51F4 = (s32)fsize;
    if ((f32)D_800A51F4 < fsize)
        D_800A51F4 = D_800A51F4 + 1;
    if (D_800A51F4 & 0xF)
        D_800A51F4 = (D_800A51F4 & ~0xF) + 0x10;
    D_800A51F0 = D_800A51F4 - 16;
    D_800A51F8 = D_800A51F4 + 0x60;

    if (c->fxType == 6) {
        for (i = 0; i < 26; i++)
            params[i] = D_8003D6B0[i];
        c->params = params;
    }
    alInit(D_800A2390.g, c);

    D_800A6194 = &D_800A61A0[0];
    for (i = 0; i < NUM_OSC_STATES - 1; i++)
        D_800A61A0[i].next = &D_800A61A0[i + 1];
    D_800A61A0[i].next = 0;

    D_800A47F0[0].node.prev = 0;
    D_800A47F0[0].node.next = 0;
    for (i = 0; i < NUM_DMA_BUFFERS - 1; i++) {
        alLink(&D_800A47F0[i + 1], &D_800A47F0[i]);
        D_800A47F0[i].ptr = alHeapAlloc(c->heap, 1, DMA_BUFFER_LENGTH);
    }
    D_800A47F0[i].ptr = alHeapAlloc(c->heap, 1, DMA_BUFFER_LENGTH);

    for (i = 0; i < 2; i++)
        D_800A2390.ACMDList[i] = alHeapAlloc(c->heap, 1, amConfig[2] * sizeof(Acmd));
    for (i = 0; i < 3; i++) {
        info = alHeapAlloc(c->heap, 1, 0x80);
        D_800A2390.audioInfo[i] = info;
        info->doneType = 2;
        D_800A2390.audioInfo[i]->doneInfo = D_800A2390.audioInfo[i];
        D_800A2390.audioInfo[i]->data = alHeapAlloc(c->heap, 1, 0x1000);
    }

    osCreateMesgQueue(&D_800A2390.audioReplyMsgQ, D_800A2390.audioReplyMsgBuf, 64);
    osCreateMesgQueue(&D_800A2390.audioFrameMsgQ, D_800A2390.audioFrameMsgBuf, 64);
    osCreateMesgQueue(D_800A5E00, D_800A5E18, 128);
    osCreateMesgQueue(D_800B0508, D_800C53DC, 1);
    osCreateMesgQueue(&D_80043330, &D_80043348, 1);
    osCreateThread(D_800A2390.thread, 3, AudioMgr_Main, 0, D_800A47E0, 14);
}

/* Set pitch, fx mix, pan, volume and priority of the current sound from its slot. */
static void Sfx_ApplyParams(u32 i)
{
    SfxSlot *s = &D_800A6018[i];

    alSndpSetPitch(D_800A2358, s->pitch);
    alSndpSetFXMix(D_800A2358, s->fxMix);
    alSndpSetPan(D_800A2358, s->pan);
    alSndpSetVol(D_800A2358, s->vol);
    alSndpSetPriority(D_800A2358, D_800A2368[i], 10);
}

/* @ 0x8000E0A4 */
/* The audio thread. On every retrace: build and send the audio task, then
 * process the 29 sound slots, start/fade music, and signal the end of a
 * watched sound. On pre-NMI: stop everything, blank the screen and idle. */
void AudioMgr_Main(void *arg)
{
    AudioMsg *msg = 0;
    s32 dummy = 0;
    AudioInfo *lastInfo = 0;
    s32 done = 0;
    s32 watching = 0;
    u8 client[8];
    u8 i;
    s16 j;
    s32 state;

    (void)arg;
    osScAddClient(D_800C0AC0, client, &D_800A2390.audioFrameMsgQ);
    while (!done) {
        osRecvMesg(&D_800A2390.audioFrameMsgQ, &msg, 1);
        switch (msg->type) {
        case 1:     /* retrace */
            if (D_80038480 != 0)
                break;
            if (D_80038484 != 0 && D_8003848A == 0)
                break;
            D_80038480 = 1;
            if (AudioMgr_HandleFrame(D_800A2390.audioInfo[audFrameCt % 3], lastInfo)) {
                osRecvMesg(&D_800A2390.audioReplyMsgQ, &msg, 1);
                lastInfo = msg->info;
            }
            for (i = 0; i < NUM_SFX_SLOTS; i++) {
                alSndpSetSound(D_800A2358, D_800A2368[i]);
                state = alSndpGetState(D_800A2358);
                switch (D_800A6018[i].state) {
                case SFX_STOP:
                    alSndpStop(D_800A2358);
                    D_800A6018[i].state = SFX_STOPPING;
                    break;
                case SFX_PLAYING:
                case SFX_STOPPING:
                    if (state == 0)
                        D_800A6018[i].state = SFX_IDLE;
                    break;
                case SFX_START:
                    if (state != 0) {
                        alSndpStop(D_800A2358);
                        D_800A6018[i].state = SFX_RESTART;
                        break;
                    }
                    Sfx_ApplyParams(i);
                    alSndpPlay(D_800A2358);
                    D_800A6018[i].state = SFX_PLAYING;
                    break;
                case SFX_RESTART:
                    if (state != 0)
                        break;
                    Sfx_ApplyParams(i);
                    alSndpPlay(D_800A2358);
                    D_800A6018[i].state = SFX_PLAYING;
                    break;
                case SFX_UPDATE:
                    Sfx_ApplyParams(i);
                    D_800A6018[i].state = SFX_PLAYING;
                    break;
                default:
                    break;
                }
            }
            if (D_80038498 == MUSIC_START) {
                alCSPPlay(D_800A2350);
                D_80038498 = MUSIC_PLAYING;
            }
            if (D_80038488 != 0xFFFF) {
                if (!watching) {
                    if (alSndpGetState(D_800A2358) == 1)
                        watching = 1;
                } else if (alSndpGetState(D_800A2358) == 0) {
                    watching = 0;
                    D_80038488 = 0xFFFF;
                    osSendMesg(D_800B0508, &dummy, 0);
                }
            }
            if (D_80038498 == MUSIC_FADING) {
                if (D_8003848C != 0) {
                    alCSPStop(D_800A2350);
                    alCSPSetVol(D_800A2350, D_800A236C);
                    D_8003848C = 0;
                } else if (alCSPGetState(D_800A2350) == 0) {
                    if (D_800384A0 == 1) {
                        osSendMesg(&D_80043330, &dummy, 0);
                        D_800384A0 = 0;
                    }
                    D_80038498 = MUSIC_IDLE;
                }
            }
            D_80038480 = 0;
            break;
        case 4:     /* pre-NMI */
            D_80038484 = 1;
            alCSPStop(D_800A2350);
            if (D_8003849C != 0) {
                for (j = 0; j < D_800A2364->soundCount; j++) {
                    alSndpSetSound(D_800A2358, D_800A2368[j]);
                    alSndpStop(D_800A2358);
                }
            }
            osViBlack(1);
            done = 1;
            break;
        }
    }
    alClose(D_800A2390.g);
    for (;;)
        osRecvMesg(&D_800A2390.audioFrameMsgQ, &msg, 1);
}

/* @ 0x8000E740 */
/* __amHandleFrameMsg: free stale DMA buffers, queue the previous output
 * buffer, size this frame, build the command list and send the RSP task to
 * the scheduler. Returns 0 if there were no commands. */
s32 AudioMgr_HandleFrame(AudioInfo *info, AudioInfo *lastInfo)
{
    s16 *audioPtr;
    Acmd *cmdp;
    s32 cmdLen;
    u32 samplesLeft;

    AudioMgr_ClearDma();
    if (lastInfo)
        osAiSetNextBuffer(lastInfo->data, (s32)lastInfo->frameSamples << 2);
    audioPtr = (s16 *)osVirtualToPhysical(info->data);
    samplesLeft = (u32)osAiGetLength() >> 2;
    info->frameSamples = (s16)(((D_800A51F4 - samplesLeft + 0x50) & 0xFFF0) + 0x10);
    if ((u32)(s32)info->frameSamples < D_800A51F0)
        info->frameSamples = (s16)D_800A51F0;
    cmdp = alAudioFrame(D_800A2390.ACMDList[D_800384AC], &cmdLen, audioPtr, info->frameSamples);
    if (cmdLen == 0)
        return 0;
    info->msgQ = &D_800A2390.audioReplyMsgQ;
    info->msg = &info->doneType;
    info->next = 0;
    info->flags = 2;                 /* OS_SC_NEEDS_RSP */
    info->data_ptr = D_800A2390.ACMDList[D_800384AC];
    info->ucode_boot_size = (u32)D_80034070 - (u32)D_80033FA0;
    info->ucode = D_80034070;
    info->ucode_data = D_8003B790;
    info->ucode_size = 0x1000;
    info->type = 2;                  /* M_AUDTASK */
    info->ucode_boot = D_80033FA0;
    info->tflags = 0;
    info->dram_stack = 0;
    info->dram_stack_size = 0;
    info->output_buff = 0;
    info->output_buff_size = 0;
    info->ucode_data_size = 0x800;
    info->yield_data_ptr = 0;
    info->yield_data_size = 0;
    info->data_size = (cmdp - D_800A2390.ACMDList[D_800384AC]) * sizeof(Acmd);
    osSendMesg(D_800B0568, &info->next, 1);
    D_800384AC ^= 1;
    return 1;
}

/* @ 0x8000E904 */
/* __amDMA: synthesizer callback returning the physical address of ROM data
 * at `addr`. Hits in the cached 0x600-byte buffers (sorted list) are reused;
 * otherwise a free buffer is filled by PI DMA from the even address below. */
s32 AudioMgr_Dma(s32 addr, s32 len, void *state)
{
    AMDMABuffer *dmaPtr = dmaState.firstUsed;
    AMDMABuffer *lastDmaPtr = 0;
    s32 addrEnd = (u32)addr + (u32)len;
    u8 *foundBuffer;
    s32 delta;

    (void)state;
    while (dmaPtr) {
        s32 buffEnd = dmaPtr->startAddr + DMA_BUFFER_LENGTH;
        if ((u32)addr < dmaPtr->startAddr)
            break;
        if (addrEnd <= buffEnd) {
            dmaPtr->lastFrame = audFrameCt;
            foundBuffer = dmaPtr->ptr + (u32)addr - dmaPtr->startAddr;
            return osVirtualToPhysical(foundBuffer);
        }
        lastDmaPtr = dmaPtr;
        dmaPtr = (AMDMABuffer *)dmaPtr->node.next;
    }

    dmaPtr = dmaState.firstFree;
    dmaState.firstFree = (AMDMABuffer *)dmaPtr->node.next;
    alUnlink(dmaPtr);
    if (lastDmaPtr) {
        alLink(dmaPtr, lastDmaPtr);
    } else if (dmaState.firstUsed) {
        lastDmaPtr = dmaState.firstUsed;
        dmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = &lastDmaPtr->node;
        dmaPtr->node.prev = 0;
        lastDmaPtr->node.prev = &dmaPtr->node;
    } else {
        dmaState.firstUsed = dmaPtr;
        dmaPtr->node.next = 0;
        dmaPtr->node.prev = 0;
    }
    foundBuffer = dmaPtr->ptr;
    delta = addr & 1;
    addr -= delta;
    dmaPtr->startAddr = addr;
    dmaPtr->lastFrame = audFrameCt;
    osPiStartDma(D_800A5200[D_800384A8++], 0, 0, addr, foundBuffer, DMA_BUFFER_LENGTH, D_800A5E00);
    return osVirtualToPhysical(foundBuffer) + delta;
}

/* @ 0x8000EA88 */
/* __amDmaNew: one shared DMA state; returns the DMA callback. */
void *AudioMgr_DmaNew(AMDMAState **state)
{
    if (!dmaState.initialized) {
        dmaState.firstFree = &D_800A47F0[0];
        dmaState.firstUsed = 0;
        dmaState.initialized = 1;
    }
    *state = &dmaState;
    return (void *)AudioMgr_Dma;
}

/* @ 0x8000EACC */
/* __clearAudioDMA: move buffers unused for more than 2 frames back to the
 * free list, reset the DMA counter, advance and return the frame counter.
 * (Unlike the SDK, the DMA completion messages are never collected.) */
u32 AudioMgr_ClearDma(void)
{
    AMDMABuffer *dmaPtr = dmaState.firstUsed;
    AMDMABuffer *nextPtr;

    while (dmaPtr) {
        nextPtr = (AMDMABuffer *)dmaPtr->node.next;
        if (dmaPtr->lastFrame + 2 < audFrameCt) {
            if (dmaState.firstUsed == dmaPtr)
                dmaState.firstUsed = nextPtr;
            alUnlink(dmaPtr);
            if (dmaState.firstFree) {
                alLink(dmaPtr, dmaState.firstFree);
            } else {
                dmaState.firstFree = dmaPtr;
                dmaPtr->node.next = 0;
                dmaPtr->node.prev = 0;
            }
        }
        dmaPtr = nextPtr;
    }
    D_800384A8 = 0;
    audFrameCt++;
    return audFrameCt;
}

#define OSC_CENTS_BASE 0x1.07ef28p+0f   /* 1.03099298f, 0x3F83F794 */

/* @ 0x8000EB98 */
/* 1.031^depth by repeated squaring (vibrato depth in cents). */
f32 Osc_DepthToCents(u8 depth)
{
    f32 x = OSC_CENTS_BASE;
    f32 cents = 1.0f;

    while (depth) {
        if (depth & 1)
            cents *= x;
        depth >>= 1;
        x *= x;
    }
    return cents;
}

/* @ 0x8000EBDC */
/* initOsc callback of the sequence player: take a free OscState, set it up
 * for the tremolo (1-4) or vibrato (0x80-0x83) type, write the start value.
 * Returns the delay (oscDelay << 14 us), or 0 if no state is free. */
s32 Osc_Init(void **oscState, f32 *initVal, u8 oscType, u8 oscRate, u8 oscDepth, u8 oscDelay)
{
    OscState *s = D_800A6194;
    s32 delay = 0;
    s32 cents;
    u8 half;

    if (s == 0)
        return 0;
    delay = oscDelay << 14;
    D_800A6194 = s->next;
    s->type = oscType;
    *oscState = s;
    switch (oscType) {
    case 1:     /* tremolo sine */
        half = oscDepth >> 1;
        s->maxCount = 259 - oscRate;
        s->d0.b.b = 127 - half;
        s->curCount = 0;
        s->d0.b.a = half;
        *initVal = (f32)(u8)(127 - half);
        break;
    case 2:     /* tremolo square */
        s->maxCount = 256 - oscRate;
        s->curCount = 256 - oscRate;
        s->d0.b.c = 127 - oscDepth;
        s->stateFlags = 0;
        s->d0.b.b = 127;
        s->d0.b.a = 127;
        *initVal = 127.0f;
        break;
    case 3:     /* tremolo descending saw */
        s->maxCount = 256 - oscRate;
        s->curCount = 0;
        s->d0.b.b = oscDepth;
        s->d0.b.a = 127;
        *initVal = 127.0f;
        break;
    case 4:     /* tremolo ascending saw */
        s->maxCount = 256 - oscRate;
        s->d0.b.a = 127 - oscDepth;
        s->curCount = 0;
        s->d0.b.b = oscDepth;
        *initVal = (f32)(u8)(127 - oscDepth);
        break;
    case 0x80:  /* vibrato sine */
        s->d0.f = Osc_DepthToCents(oscDepth);
        s->curCount = 0;
        s->maxCount = 259 - oscRate;
        *initVal = 1.0f;
        break;
    case 0x81:  /* vibrato square */
        s->maxCount = 256 - oscRate;
        s->curCount = 256 - oscRate;
        s->stateFlags = 0;
        cents = (s32)Osc_DepthToCents(oscDepth);
        s->d0.f = alCents2Ratio(-cents);
        s->d1.f = alCents2Ratio(cents);
        *initVal = s->d1.f;
        break;
    case 0x82:  /* vibrato descending saw */
        s->maxCount = 256 - oscRate;
        s->curCount = 256 - oscRate;
        cents = (s32)Osc_DepthToCents(oscDepth);
        s->d0.i = cents;
        s->d1.i = cents * 2;
        *initVal = alCents2Ratio(s->d0.i);
        break;
    case 0x83:  /* vibrato ascending saw */
        s->maxCount = 256 - oscRate;
        s->curCount = 256 - oscRate;
        cents = (s32)Osc_DepthToCents(oscDepth);
        s->d0.i = -cents;
        s->d1.i = cents * 2;
        *initVal = alCents2Ratio(s->d0.i);
        break;
    }
    return delay;
}

/* @ 0x8000EF58 */
/* updateOsc callback: advance the oscillator one step (16000 us; square
 * waves hold for maxCount steps) and write the new volume or pitch ratio.
 * Quirk kept: types 1 and 0x80 wrap at curCount >= maxCount, the saws only
 * at curCount > maxCount. */
s32 Osc_Update(void *oscState, f32 *updateVal)
{
    OscState *s = oscState;
    s32 delta = 16000;
    f32 t;

    switch (s->type) {
    case 1:
        if (++s->curCount >= s->maxCount)
            s->curCount = 0;
        t = (f32)s->curCount / (f32)s->maxCount;
        t = sinf((f32)(t * 6.2831853));
        *updateVal = (f32)s->d0.b.b + t * (f32)s->d0.b.a;
        break;
    case 2:
        if (!s->stateFlags) {
            *updateVal = (f32)s->d0.b.c;
            s->stateFlags = 1;
        } else {
            *updateVal = (f32)s->d0.b.b;
            s->stateFlags = 0;
        }
        delta *= s->maxCount;
        break;
    case 3:
        if (++s->curCount > s->maxCount)
            s->curCount = 0;
        t = (f32)s->curCount / (f32)s->maxCount * (f32)s->d0.b.b;
        *updateVal = (f32)s->d0.b.a - t;
        break;
    case 4:
        if (++s->curCount > s->maxCount)
            s->curCount = 0;
        t = (f32)s->curCount / (f32)s->maxCount * (f32)s->d0.b.b;
        *updateVal = (f32)s->d0.b.a + t;
        break;
    case 0x80:
        if (++s->curCount >= s->maxCount)
            s->curCount = 0;
        t = (f32)s->curCount / (f32)s->maxCount;
        t = sinf((f32)(t * 6.2831853));
        *updateVal = alCents2Ratio((s32)(t * s->d0.f));
        break;
    case 0x81:
        if (!s->stateFlags) {
            *updateVal = s->d0.f;
            s->stateFlags = 1;
        } else {
            *updateVal = s->d1.f;
            s->stateFlags = 0;
        }
        delta *= s->maxCount;
        break;
    case 0x82:
        if (++s->curCount > s->maxCount)
            s->curCount = 0;
        t = (f32)s->curCount / (f32)s->maxCount * (f32)s->d1.i;
        *updateVal = alCents2Ratio((s32)((f32)s->d0.i - t));
        break;
    case 0x83:
        if (++s->curCount > s->maxCount)
            s->curCount = 0;
        t = (f32)s->curCount / (f32)s->maxCount * (f32)s->d1.i;
        *updateVal = alCents2Ratio((s32)(t + (f32)s->d0.i));
        break;
    }
    return delta;
}

/* @ 0x8000F334 */
/* stopOsc callback: return the state to the free list. */
void Osc_Stop(void *oscState)
{
    OscState *s = oscState;

    s->next = D_800A6194;
    D_800A6194 = s;
}

extern u32 D_800AF254;
extern s16 D_800A6840;
extern s16 D_800A6810;          /* SI event message */
extern u8  D_800C30A8[];        /* OSMesgQueue: SI events */
extern u8  D_800B0494[];
extern u8  D_800B0430[];        /* +3: controller bit pattern */
extern u8  D_800A67E8[];        /* OSContStatus[4] */

/* @ 0x8000F350 */
/* Controller start-up: SI message queue + event, osContInit. */
void Cont_Init(void)
{
    D_800AF254 = 0;
    D_800A6840 = 1;
    func_8000F6EC();
    D_800A6810 = 5;
    osCreateMesgQueue(D_800C30A8, D_800B0494, 1);
    osSetEventMesg(5, D_800C30A8, &D_800A6810);
    osContInit(D_800C30A8, D_800B0430 + 3, D_800A67E8);
}
