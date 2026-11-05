/* Vivid Dolls decompilation - audio: music/sound-effect player, the audio
 * manager thread (a modified N64 SDK audiomgr.c), sample DMA cache, the
 * sequence-player oscillators (SDK playseq oscillator code, modified) and
 * controller start-up.  RAM 0x8000D150-0x8000F3E0.
 *
 * Data in ROM: music bank B1 (.ctl 0x196CC0, .tbl 0x1982B0), sequences S1
 * (0x1D7720), sound-effect bank B1 (.ctl 0x1E2870, .tbl 0x1E3EA0).
 * See docs/notes/audio.md for the globals.
 */
#ifndef AUDIO_H
#define AUDIO_H

#include "vd_common.h"

/* ---- libultra-shaped structures (only the parts the game uses) ---- */

typedef struct ALLink_s {
    struct ALLink_s *next;
    struct ALLink_s *prev;
} ALLink;

typedef struct { u32 w0, w1; } Acmd;            /* one RSP audio command */

/* one 0x600-byte cache buffer for sample DMA (0x14 bytes) */
typedef struct AMDMABuffer_s {
    ALLink node;          /* 0x00 */
    u32    startAddr;     /* 0x08 ROM address of the cached data */
    u32    lastFrame;     /* 0x0C audio frame it was last used in */
    u8    *ptr;           /* 0x10 buffer */
} AMDMABuffer;

typedef struct {
    u8           initialized;   /* 0x00 */
    AMDMABuffer *firstUsed;     /* 0x04 sorted by startAddr */
    AMDMABuffer *firstFree;     /* 0x08 */
} AMDMAState;

/* audio output buffer + its RSP task (0x80 bytes allocated) */
typedef struct {
    s16 *data;            /* 0x00 output samples (0x1000 bytes) */
    s16  frameSamples;    /* 0x04 */
    u8   pad06[2];
    /* OSScTask */
    void *next;           /* 0x08 */
    u32  state;           /* 0x0C */
    u32  flags;           /* 0x10 */
    void *framebuffer;    /* 0x14 */
    /* OSTask */
    u32  type;            /* 0x18 */
    u32  tflags;          /* 0x1C */
    void *ucode_boot;     /* 0x20 */
    u32  ucode_boot_size; /* 0x24 */
    void *ucode;          /* 0x28 */
    u32  ucode_size;      /* 0x2C */
    void *ucode_data;     /* 0x30 */
    u32  ucode_data_size; /* 0x34 */
    void *dram_stack;     /* 0x38 */
    u32  dram_stack_size; /* 0x3C */
    void *output_buff;    /* 0x40 */
    u32  output_buff_size;/* 0x44 */
    void *data_ptr;       /* 0x48 */
    u32  data_size;       /* 0x4C */
    void *yield_data_ptr; /* 0x50 */
    u32  yield_data_size; /* 0x54 */
    void *msgQ;           /* 0x58 */
    void *msg;            /* 0x5C */
    /* AMAudioMsg done message */
    s16  doneType;        /* 0x60 = 2 */
    u8   pad62[2];
    void *doneInfo;       /* 0x64 = this AudioInfo */
} AudioInfo;

typedef struct { u8 data[0x18]; } OSMesgQueueRaw;

/* __am, at 0x800A2390 */
typedef struct {
    Acmd *ACMDList[2];             /* 0x000 */
    AudioInfo *audioInfo[3];       /* 0x008 */
    u8 pad14[4];
    u8 thread[0x1B0];              /* 0x018 OSThread */
    OSMesgQueueRaw audioFrameMsgQ; /* 0x1C8 */
    void *audioFrameMsgBuf[64];    /* 0x1E0 */
    OSMesgQueueRaw audioReplyMsgQ; /* 0x2E0 */
    void *audioReplyMsgBuf[64];    /* 0x2F8 */
    u8 g[1];                       /* 0x3F8 ALGlobals */
} AudioMgr;

/* scheduler messages the audio thread receives */
typedef struct {
    s16 type;             /* 1 = retrace, 4 = pre-NMI; 2 = audio task done */
    u8  pad[2];
    AudioInfo *info;      /* done message: the finished buffer */
} AudioMsg;

/* sound-effect voice slot (12 bytes, 29 of them at 0x800A6018) */
typedef struct {
    f32 pitch;            /* 0x0 */
    s16 vol;              /* 0x4 */
    u8  pan;              /* 0x6 */
    u8  fxMix;            /* 0x7 */
    u8  state;            /* 0x8 SFX_* below */
    u8  pad[3];
} SfxSlot;

enum {
    SFX_IDLE = 0,        /* nothing */
    SFX_PLAYING = 1,     /* playing; goes idle when the voice stops */
    SFX_START = 2,       /* start request (stops it first if still playing) */
    SFX_RESTART = 3,     /* waiting for the voice to stop, then start */
    SFX_STOPPING = 4,    /* stop sent; goes idle when stopped */
    SFX_UPDATE = 5,      /* new pitch/vol/pan/fx to apply */
    SFX_STOP = 6         /* stop request */
};

enum {                   /* D_80038498 */
    MUSIC_IDLE = 0, MUSIC_PLAYING = 1, MUSIC_FADING = 2, MUSIC_START = 3, MUSIC_LOADING = 4
};

/* sequence-player oscillator state (0x14 bytes, 80 at 0x800A61A0) */
typedef struct OscState_s {
    struct OscState_s *next;   /* 0x00 free-list link */
    u8  type;                  /* 0x04 1-4 tremolo, 0x80-0x83 vibrato */
    u8  stateFlags;            /* 0x05 square-wave phase */
    u16 maxCount;              /* 0x06 */
    u16 curCount;              /* 0x08 */
    u8  pad0A[2];
    union {
        struct { u8 a, b, c, pad; } b;          /* tremolo byte values */
        f32 f;                                   /* vibrato: depth cents / ratio */
        s32 i;                                   /* vibrato saw: cents */
    } d0;                      /* 0x0C */
    union { f32 f; s32 i; } d1;/* 0x10 */
} OscState;

/* ---- functions ---- */
void Audio_Nop(void);
void Music_Stop(void);
void Music_Play(u32 song);
s16  Sfx_Play(u16 id);
s16  Sfx_PlayEx(u16 id, f32 pitch, s16 vol, u8 pan, u8 fxMix);
void Sfx_SetSlotParams(u16 slot, f32 pitch, s16 vol, u8 pan, u8 fxMix);
void Sfx_StopSlot(u16 slot);
void Sfx_StopAll(void);
void Sfx_LoadInstrument(u16 inst);
void Sfx_SetWatchSound(u16 slot);
void Audio_Init(void);
void AudioMgr_Create(void *synConfig, s32 pri, u32 *amConfig);
void AudioMgr_Main(void *arg);
s32  AudioMgr_HandleFrame(AudioInfo *info, AudioInfo *lastInfo);
s32  AudioMgr_Dma(s32 addr, s32 len, void *state);
void *AudioMgr_DmaNew(AMDMAState **state);
u32  AudioMgr_ClearDma(void);
f32  Osc_DepthToCents(u8 depth);
s32  Osc_Init(void **oscState, f32 *initVal, u8 oscType, u8 oscRate, u8 oscDepth, u8 oscDelay);
s32  Osc_Update(void *oscState, f32 *updateVal);
void Osc_Stop(void *oscState);
void Cont_Init(void);

#endif
