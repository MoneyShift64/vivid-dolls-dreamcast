# Audio area (0x8000D000–0x8000F3E0)

Source: `src/game/audio.c`, `src/game/audio.h`. Test: `python3 tools/difftest/test_audio.py`
(300 trials per function, about 11 minutes, mostly Audio_Init. Set `AUDIO_TRIALS=50 AUDIO_INIT_TRIALS=5` for a quick run.)

The audio code is the N64 SDK **audio manager** (`audiomgr.c`) and the **playseq oscillator**
code, both changed by the game, plus a small music/sound-effect API. Music: one compact-sequence
player (ALCSPlayer) with the music bank. Sound effects: one ALSndPlayer with 29 sounds (the
sounds of instrument 0 of the sfx bank). The game calls `Sfx_*`, which only write requests into
the `SfxSlot` table. The audio thread applies them once per retrace.

## Functions done (22, all PASS)
| addr | name | what |
|---|---|---|
| 0x8000D150 | Audio_Nop | empty |
| 0x8000D160 | Music_Stop | if playing: volume 0x7800, start fade (state 2) |
| 0x8000D1CC | Music_Play(u32 song) | fade + wait (osRecvMesg on D_80043330), Rom_Read the sequence, alCSeqNew/SetSeq/SetVol, state 3 |
| 0x8000D368 | Sfx_Play(u16 id) → s16 | next variant slot of sound id, default pitch, vol 0x7F00, pan 0x40 |
| 0x8000D49C | Sfx_PlayEx(id, f32 pitch, s16 vol, u8 pan, u8 fx) → s16 | same, with explicit parameters |
| 0x8000D5C4 | Sfx_SetSlotParams(slot, pitch, vol, pan, fx) | change the parameters of a slot (state 5 unless starting) |
| 0x8000D67C | Sfx_StopSlot(u16) | state 6 |
| 0x8000D6B8 | Sfx_StopAll | state 6 on every slot |
| 0x8000D720 | Sfx_LoadInstrument(u16) | free the old instrument's sounds, alSndpAllocate the new ones. **Quirk:** only inst 0 is accepted |
| 0x8000D890 | Sfx_SetWatchSound(u16) | watch a slot; the thread posts to D_800B0508 when it ends |
| 0x8000D93C | Audio_Init | clear the heap, allocate everything, load the .ctl files and sequence header from ROM, start the thread |
| 0x8000DCE0 | AudioMgr_Create | amCreateAudioMgr. **Quirk:** the pri argument is ignored (always 14) |
| 0x8000E0A4 | AudioMgr_Main | __amMain + per-frame sound/music handling (never returns) |
| 0x8000E740 | AudioMgr_HandleFrame | __amHandleFrameMsg (sends the task to the scheduler cmd queue `*D_800B0568`) |
| 0x8000E904 | AudioMgr_Dma | __amDMA (0x600-byte buffers, PI DMA) |
| 0x8000EA88 | AudioMgr_DmaNew | __amDmaNew |
| 0x8000EACC | AudioMgr_ClearDma | __clearAudioDMA (does not collect the DMA messages, unlike the SDK). Returns the new frame count |
| 0x8000EB98 | Osc_DepthToCents | 1.0309929848^depth (the same loop is inlined in Osc_Init) |
| 0x8000EBDC | Osc_Init | initOsc callback (types 1–4 tremolo, 0x80–0x83 vibrato) |
| 0x8000EF58 | Osc_Update | updateOsc callback (uses sinf, alCents2Ratio) |
| 0x8000F334 | Osc_Stop | stopOsc callback (free list) |
| 0x8000F350 | Cont_Init | SI queue + osSetEventMesg(OS_EVENT_SI) + osContInit (calls func_8000F6EC first) |

No functions in this range are left to do.

## libultra functions identified (unnamed in symbol_addrs.txt)
```
alHeapInit = 0x8001D820; // type:func
alSeqFileNew = 0x8001D4A0; // type:func
alBnkfNew = 0x8001D4E0; // type:func
alInit = 0x8001D770; // type:func
alClose = 0x8001D79C; // type:func
alCSeqNew = 0x8001D8B0; // type:func
alCSPGetVol = 0x8001E370; // type:func  (returns s16 at seqp+0x32; maybe another getter)
alCSPGetState = 0x8001E380; // type:func
alCSPPlay = 0x8001E390; // type:func
alCSPSetBank = 0x8001E3C0; // type:func
alCSPSetSeq = 0x8001E3F0; // type:func
alCSPSetVol = 0x8001E420; // type:func
alCSPStop = 0x8001E450; // type:func
alCSPNew = 0x8001E480; // type:func
alSndpNew = 0x8001F6E0; // type:func
alSndpAllocate = 0x8001FDE0; // type:func
alSndpDeallocate = 0x8001FEA0; // type:func
alSndpSetSound = 0x8001FEF0; // type:func
alSndpPlay = 0x8001FF00; // type:func
alSndpGetSound = 0x8001FF60; // type:func
alSndpStop = 0x8001FF70; // type:func
alSndpGetState = 0x8001FFC0; // type:func
alSndpSetPitch = 0x8001FFE0; // type:func
alSndpSetPriority = 0x80020030; // type:func
alSndpSetVol = 0x80020060; // type:func
alSndpSetPan = 0x800200B0; // type:func
alSndpSetFXMix = 0x80020100; // type:func
alAudioFrame = 0x8002041C; // type:func
osAiGetLength = 0x80021000; // type:func
osAiSetFrequency = 0x80021010; // type:func
osAiSetNextBuffer = 0x80021140; // type:func
osPiStartDma = 0x80025950; // type:func
osContInit = 0x800222E0; // type:func
osScAddClient = 0x8000C6D8; // type:func
```

## Data symbols
```
Audio_Nop = 0x8000D150; // type:func
Music_Stop = 0x8000D160; // type:func
Music_Play = 0x8000D1CC; // type:func
Sfx_Play = 0x8000D368; // type:func
Sfx_PlayEx = 0x8000D49C; // type:func
Sfx_SetSlotParams = 0x8000D5C4; // type:func
Sfx_StopSlot = 0x8000D67C; // type:func
Sfx_StopAll = 0x8000D6B8; // type:func
Sfx_LoadInstrument = 0x8000D720; // type:func
Sfx_SetWatchSound = 0x8000D890; // type:func
AudioMgr_Create = 0x8000DCE0; // type:func
AudioMgr_Main = 0x8000E0A4; // type:func
AudioMgr_HandleFrame = 0x8000E740; // type:func
AudioMgr_Dma = 0x8000E904; // type:func
AudioMgr_DmaNew = 0x8000EA88; // type:func
AudioMgr_ClearDma = 0x8000EACC; // type:func
Osc_DepthToCents = 0x8000EB98; // type:func
Osc_Init = 0x8000EBDC; // type:func
Osc_Update = 0x8000EF58; // type:func
Osc_Stop = 0x8000F334; // type:func
Cont_Init = 0x8000F350; // type:func

gAudioBusy = 0x80038480; // type:s32 size:0x4
gAudioNmi = 0x80038484; // type:s32 size:0x4
gSfxWatchSlot = 0x80038488; // type:u16 size:0x2
gMusicSavedVol = 0x8003848A; // type:u16 size:0x2
gMusicFadeReq = 0x8003848C; // type:u16 size:0x2
gSfxInstrument = 0x8003848E; // type:u16 size:0x2
gMusicSong = 0x80038490; // type:u32 size:0x4
gMusicState = 0x80038498; // type:s32 size:0x4
gSfxLoaded = 0x8003849C; // type:s32 size:0x4
gMusicWaiting = 0x800384A0; // type:s32 size:0x4
gAudFrameCt = 0x800384A4; // type:u32 size:0x4
gAudNextDma = 0x800384A8; // type:u32 size:0x4
gAudCurAcmdList = 0x800384AC; // type:u32 size:0x4
gSfxVariantCount = 0x800384B0; // type:u8 size:0x1C
gSfxFirstSlot = 0x800384CC; // type:u8 size:0x1C
gSfxDefaultPitch = 0x800384E8; // type:f32 size:0x74
gAudReverbParams = 0x8003D6B0; // type:s32 size:0x68
gMusicDoneQ = 0x80043330; // type:OSMesgQueue size:0x18
gMusicDoneMsg = 0x80043348; // type:OSMesg size:0x4
gAudioHeapMem = 0x80043350; // type:u8 size:0x5F000
gSeqPlayer = 0x800A2350; // type:ALCSPlayer* size:0x4
gSeq = 0x800A2354; // type:ALCSeq* size:0x4
gSndPlayer = 0x800A2358; // type:ALSndPlayer* size:0x4
gSfxLastSound = 0x800A235C; // type:ALSound* size:0x4
gSfxBank = 0x800A2360; // type:ALBank* size:0x4
gSfxInst = 0x800A2364; // type:ALInstrument* size:0x4
gSfxSoundIds = 0x800A2368; // type:s16* size:0x4
gMusicVolume = 0x800A236C; // type:s16 size:0x2
gSeqBuf = 0x800A2370; // type:u8* size:0x4
gSeqFileSize = 0x800A2374; // type:s32 size:0x4
gSeqFile = 0x800A2378; // type:ALSeqFile* size:0x4
gAudioHeap = 0x800A2380; // type:ALHeap size:0x10
gAudioMgr = 0x800A2390; // type:AMAudioMgr (ALGlobals at +0x3F8)
gAmDmaState = 0x800A47E0; // type:AMDMAState size:0xC
gAmDmaBuffs = 0x800A47F0; // type:AMDMABuffer size:0xA00
gAmMinFrameSize = 0x800A51F0; // type:u32 size:0x4
gAmFrameSize = 0x800A51F4; // type:u32 size:0x4
gAmMaxFrameSize = 0x800A51F8; // type:u32 size:0x4
gAmDmaIoMesgBuf = 0x800A5200; // type:OSIoMesg size:0xC00
gAmDmaMsgQ = 0x800A5E00; // type:OSMesgQueue size:0x18
gAmDmaMsgBuf = 0x800A5E18; // type:OSMesg size:0x200
gSfxSlots = 0x800A6018; // type:SfxSlot size:0x15C
gSfxNextVariant = 0x800A6178; // type:u8 size:0x1C
gOscFreeList = 0x800A6194; // type:OscState* size:0x4
gOscStates = 0x800A61A0; // type:OscState size:0x640
gSfxDoneQ = 0x800B0508; // type:OSMesgQueue size:0x18
gSfxDoneMsg = 0x800C53DC; // type:OSMesg size:0x4
gSchedCmdQ = 0x800B0568; // type:OSMesgQueue* size:0x4
gScheduler = 0x800C0AC0; // type:OSSched
gSiMsgQ = 0x800C30A8; // type:OSMesgQueue size:0x18
gSiMsgBuf = 0x800B0494; // type:OSMesg size:0x4
gSiEventMsg = 0x800A6810; // type:s16 size:0x2
gContStatus = 0x800A67E8; // type:OSContStatus size:0x10
```
(The names are suggestions. The C still uses the `D_` names.)

## Structures and how it works
* **Audio heap**: 0x5F000 bytes at 0x80043350 (it ends exactly at 0x800A2350), ALHeap at 0x800A2380.
  Allocation order in Audio_Init: ALCSeq 0xF8, ALCSPlayer 0x7C, ALSndPlayer 0x54, music .ctl (0x15F0),
  sfx .ctl (0x1630), seq header (4, then 4 + 8·seqCount), sequence buffer 0x2000, then synth/DMA
  buffers (in AudioMgr_Create), the 29 sound ids (0x3A).
* Synth config: 40 voices, 256 updates, fxType 1 (AL_FX_SMALLROOM), output = osAiSetFrequency(44100).
  amConfig = {44100, framesPerField 1, maxACMDSize 0x1800}. frameSize = ceil(44100/60) rounded up to
  16 = 0x2E0. min = frameSize−16, max = frameSize+0x60. With fxType 6 the custom reverb table at
  0x8003D6B0 (26 words) is copied to the stack and used as `params`.
* Seq player config: 40 voices, 256 events, 16 channels, oscillator callbacks Osc_*.
  Sound player: 29 sounds, 256 events.
* **AMAudioMgr** at 0x800A2390: ACMDList[2] (+0), audioInfo[3] (+8), OSThread (+0x18, id 3, pri 14,
  stack top 0x800A47E0), frame queue (+0x1C8, 64 msgs), reply queue (+0x2E0, 64), ALGlobals (+0x3F8).
* **AudioInfo** (0x80 allocated): data (+0, 0x1000 bytes), frameSamples s16 (+4), OSScTask (+8…+0x5F),
  done message {type 2, info} at +0x60. The task uses rspboot 0x80033FA0, aspMain text 0x80034070,
  data 0x8003B790.
* **DMA cache**: 128 buffers of 0x600 bytes (AMDMABuffer: link, startAddr, lastFrame, ptr). The used
  list is sorted by address. A buffer is freed after more than 2 frames without use. A miss DMAs from the even
  address below.
* **SfxSlot** (12 bytes ×29 at 0x800A6018): f32 pitch, s16 vol, u8 pan, u8 fxMix, u8 state.
  States: 0 idle, 1 playing (becomes 0 when the voice stops), 2 start (if the voice is still playing:
  stop it → 3), 3 wait for the stop, then start, 4 stopping, 5 apply new parameters, 6 stop request.
  Starting = SetPitch, SetFXMix, SetPan, SetVol, SetPriority(10), Play.
* Sound ids → slots: sound id n (0..27) uses `gSfxVariantCount[n]` slots from `gSfxFirstSlot[n]`,
  in rotation (`gSfxNextVariant[n]`). Each slot has a default pitch in `gSfxDefaultPitch`.
* **Music state** (0x80038498): 0 idle, 1 playing, 2 fading (the thread sends alCSPStop once, restores the
  volume, then waits for the player to stop. If Music_Play is waiting (0x800384A0) it posts gMusicDoneQ),
  3 start next frame (alCSPPlay), 4 loading.
* Pre-NMI message (type 4): sets 0x80038484, stops the music and all sounds, osViBlack(1), then the
  thread idles. After that, 0x80038484 makes every Sfx/Music call do nothing.
* **OscState** (0x14 ×80): next, type, stateFlags, maxCount, curCount, then data at +0xC/+0x10 (bytes for
  tremolo: +0xC depth/lo, +0xD base, +0xE square low value; f32 or s32 cents for vibrato). It differs a
  little from the SDK sample (square tremolo uses +0xE/+0xD, saw wrap test is `>` not `>=`).
* Update step is 16000 µs. Square waves hold for maxCount steps.

## Test notes
* Stubs record their arguments (and config-struct contents for alInit/alCSPNew/alSndpNew).
  Function pointers are mapped to the original addresses, and stack addresses are compared as `STK`.
* alHeapInit/alHeapDBAlloc/alSeqFileNew/alBnkfNew/alLink/alUnlink/alCents2Ratio/sinf run as the
  original code. Audio_Init uses the real ROM .ctl and sequence data through Rom_Read.
* AudioMgr_Main runs a scripted list of scheduler messages and stops when the script runs out.
  AudioMgr_HandleFrame is stubbed there (it is tested separately).
* Sfx_Play/PlayEx would trap (div by 0) if `gSfxVariantCount[id]` were 0. The tests use non-zero counts.
  AudioMgr_Dma with an empty free list dereferences NULL in the original, so that case is not tested.
