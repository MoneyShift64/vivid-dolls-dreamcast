/* Warning: missing "jr $ra" in last block of func_8000AE90 (initial). */

/* Warning: missing "jr $ra" in last block of func_8000AEAC (.L8000AF90). */

/* Warning: missing "jr $ra" in last block of func_8000B06C (initial). */

/* Warning: missing "jr $ra" in last block of __osDispatchThread (.L80026BCC). */

struct _struct_D_80038410_0xC {
    /* 0x0 */ s32 unk0;                             /* inferred */
    /* 0x4 */ s32 unk4;                             /* inferred */
    /* 0x8 */ s32 unk8;                             /* inferred */
};                                                  /* size = 0xC */

struct _struct_D_8003A12B_0xC {
    /* 0x0 */ u8 unk0;                              /* inferred */
    /* 0x1 */ char pad1[0xB];                       /* maybe part of unk0[0xC]? */
};                                                  /* size = 0xC */

struct _struct_D_800A4800_0x14 {
    /* 0x00 */ u8 *unk0;                            /* inferred */
    /* 0x04 */ char pad4[0x10];                     /* maybe part of unk0[5]? */
};                                                  /* size = 0x14 */

struct _struct_D_800A6020_0xC {
    /* 0x0 */ u8 unk0;                              /* inferred */
    /* 0x1 */ char pad1[0xB];                       /* maybe part of unk0[0xC]? */
};                                                  /* size = 0xC */

struct _struct_D_800A67F8_0x6 {
    /* 0x0 */ u16 unk0;                             /* inferred */
    /* 0x2 */ s8 unk2;                              /* inferred */
    /* 0x3 */ s8 unk3;                              /* inferred */
    /* 0x4 */ u8 unk4;                              /* inferred */
    /* 0x5 */ char pad5[1];
};                                                  /* size = 0x6 */

struct _struct_D_800A691E_0x10 {
    /* 0x00 */ s16 unk0;                            /* inferred */
    /* 0x02 */ char pad2[0xE];                      /* maybe part of unk0[8]? */
};                                                  /* size = 0x10 */

struct _struct_D_800A6990_0x68 {
    /* 0x00 */ s32 unk0;                            /* inferred */
    /* 0x04 */ s16 *(*unk4)[];                      /* inferred */
    /* 0x08 */ s32 unk8;                            /* inferred */
    /* 0x0C */ char padC[0x20];                     /* maybe part of unk8[9]? */
    /* 0x2C */ u8 unk2C;                            /* inferred */
    /* 0x2D */ char pad2D[0x1F];                    /* maybe part of unk2C[0x20]? */
    /* 0x4C */ s32 unk4C;                           /* inferred */
    /* 0x50 */ s32 unk50;                           /* inferred */
    /* 0x54 */ s32 unk54;                           /* inferred */
    /* 0x58 */ s32 unk58;                           /* inferred */
    /* 0x5C */ s32 unk5C;                           /* inferred */
    /* 0x60 */ s32 unk60;                           /* inferred */
    /* 0x64 */ u8 unk64;                            /* inferred */
    /* 0x65 */ u8 unk65;                            /* inferred */
    /* 0x66 */ char pad66[2];                       /* maybe part of unk65[3]? */
};                                                  /* size = 0x68 */

struct _struct_D_800AF198_0x8 {
    /* 0x0 */ u32 *unk0;                            /* inferred */
    /* 0x4 */ u32 unk4;                             /* inferred */
};                                                  /* size = 0x8 */

struct _struct_D_800C51E0_0x8 {
    /* 0x0 */ s16 *(*unk0)[];                       /* inferred */
    /* 0x4 */ s16 *unk4;                            /* inferred */
};                                                  /* size = 0x8 */

struct _struct_gOverlayTable_0x10 {
    /* 0x00 */ u8 *unk0;                            /* inferred */
    /* 0x04 */ char pad4[4];
    /* 0x08 */ M2C_UNK unk8;                        /* inferred */
    /* 0x08 */ char pad8[4];
    /* 0x0C */ M2C_UNK unkC;                        /* inferred */
    /* 0x0C */ char padC[4];
};                                                  /* size = 0x10 */

struct _struct_osViModeTable_0x50 {
    /* 0x00 */ s32 unk0;                            /* inferred */
    /* 0x04 */ s32 unk4;                            /* inferred */
    /* 0x08 */ s32 unk8;                            /* inferred */
    /* 0x0C */ s32 unkC;                            /* inferred */
    /* 0x10 */ char pad10[0xC];                     /* maybe part of unkC[4]? */
    /* 0x1C */ s32 unk1C;                           /* inferred */
    /* 0x20 */ char pad20[0x10];                    /* maybe part of unk1C[5]? */
    /* 0x30 */ s32 unk30;                           /* inferred */
    /* 0x34 */ char pad34[0x10];                    /* maybe part of unk30[5]? */
    /* 0x44 */ s32 unk44;                           /* inferred */
    /* 0x48 */ char pad48[8];                       /* maybe part of unk44[3]? */
};                                                  /* size = 0x50 */

M2C_UNK *Asset_LoadLzss(M2C_UNK *arg0);             /* static */
void Audio_Init();                                  /* static */
M2C_UNK D_80033FA0();                               /* static */
M2C_UNK D_80034070();                               /* static */
M2C_UNK D_80034E90();                               /* static */
M2C_UNK D_80035E10();                               /* static */
M2C_UNK D_80037210();                               /* static */
void Heap_AddRegion(void *arg1, u32 arg2);          /* static */
M2C_UNK *Heap_Alloc(M2C_UNK *arg0, s32 arg1, s32 *, s32); /* static */
void *Heap_Free(M2C_UNK *arg0, M2C_UNK *arg1, s32); /* static */
void Jpeg64_DecodeAC(s16 *arg0, s16 *arg1, M2C_UNK *arg2, void *arg3, void *arg4, void *arg5); /* static */
s32 Jpeg64_DecodeDC(M2C_UNK *arg0, void *arg1, void *arg2, void *arg3); /* static */
void Jpeg64_DecodeMacroblock(M2C_UNK *arg0, M2C_UNK *arg1); /* static */
void Jpeg64_Dequant(s16 *arg0, M2C_UNK *arg1, f32 arg2); /* static */
s32 Jpeg64_GetBit();                                /* static */
s16 Jpeg64_GetBits(u16 arg0);                       /* static */
void Jpeg64_Idct(M2C_UNK *arg0, s16 *arg1);         /* static */
void Jpeg64_Init(u8 *arg0);                         /* static */
void Jpeg64_PutBits(u16 arg0, u16 arg1);            /* static */
void Jpeg64_Upsample(s16 *arg0, M2C_UNK *arg1);     /* static */
void Jpeg64_YCbCrToRgba5551(u16 *arg0, s16 *arg1);  /* static */
void Main_GameLoop();                               /* static */
void Mtx_Identity(f32 *arg0);                       /* static */
void PhotoView_Build(M2C_UNK *arg0, s32 arg1, s32 arg2); /* static */
void Sprite_DrawScreen(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4); /* static */
s32 Sprite_Parse(M2C_UNK *arg0, M2C_UNK *arg1, s32 arg2, s32 arg3); /* static */
s32 Sprite_ParseScreen(M2C_UNK *arg0, M2C_UNK *arg1); /* static */
void _Ldtob(M2C_UNK *arg0, s32 arg1);               /* static */
void _Litob(M2C_UNK *arg0, s32 arg1, u8 *, s32);    /* static */
s32 _Printf(s32 (*arg0)(s32, u8 *, s32), s32 arg1, u8 *arg2, s32 *arg3); /* static */
s32 __alSeqNextDelta(void *arg0, s32 *arg1);        /* static */
s32 __divdi3(s32, u32, s32, u32);                   /* static */
void __initChanState(u8 *arg0);                     /* static */
void __initFromBank(void *arg0, M2C_UNK *arg1);     /* static */
void *__lookupSoundQuick(void *arg0, s8 arg1, s8 arg2, s8 arg3); /* static */
M2C_UNK *__lookupVoice(void *arg0, u8 arg1, u8 arg2); /* static */
M2C_UNK *__mapVoice(void *arg0, s8 arg1, s8 arg2, s8 arg3); /* static */
s32 __osAiDeviceBusy();                             /* static */
u32 __osCheckId(struct _struct_D_800A6990_0x68 *arg0, s32); /* static */
s32 __osContAddressCrc(s32 arg0);                   /* static */
s32 __osContDataCrc(u8 *arg0, s32, s8 *, M2C_UNK *); /* static */
u32 __osContRamRead(s16 *(*arg0)[], s32 arg1, u16 arg2, u8 *arg3); /* static */
u32 __osContRamWrite(s16 *(*arg0)[], s32 arg1, u16 arg2, u8 *arg3, s32 arg4); /* static */
void __osDequeueThread(s16 **arg0, s16 **arg1);     /* static */
void __osDevMgrMain(s16 **arg0);                    /* static */
struct _struct_osViModeTable_0x50 *__osDisableInt(M2C_UNK, s32); /* static */
void __osDispatchThread(s16 **);                    /* static */
s32 __osDpDeviceBusy();                             /* static */
u32 __osEepStatus(s16 **arg0, u16 *arg1);           /* static */
void __osEnqueueAndYield(s16 *(*arg0)[], M2C_UNK);  /* static */
void __osEnqueueThread(s16 **arg0, s16 **arg1);     /* static */
M2C_UNK __osExceptionPreamble();                    /* static */
s16 **__osGetActiveQueue();                         /* static */
s32 __osGetCause();                                 /* static */
u32 __osGetId(struct _struct_D_800A6990_0x68 *arg0); /* static */
s32 __osGetSR();                                    /* static */
s32 __osIdCheckSum(u8 *arg0, u16 *arg1, u16 *arg2); /* static */
u64 __osInsertTimer(M2C_UNK *arg0, s32);            /* static */
u32 __osPfsGetStatus(s16 *(*arg0)[], s32 arg1);     /* static */
u32 __osPfsRWInode(struct _struct_D_800A6990_0x68 *arg0, u16 *arg1, s32 arg2, u32 arg3); /* static */
u32 __osPfsSelectBank(struct _struct_D_800A6990_0x68 *arg0, u8 *); /* static */
void __osPiCreateAccessQueue();                     /* static */
void __osPiGetAccess();                             /* static */
void __osPiRelAccess();                             /* static */
s16 **__osPopThread(s16 **arg0, s16 **);            /* static */
u32 __osProbeTLB(s32 arg0);                         /* static */
u32 __osRdbSend(u8 *arg0, u32 arg1, s32 arg2);      /* static */
void __osResetGlobalIntMask(s32 arg0);              /* static */
void __osRestoreInt(struct _struct_osViModeTable_0x50 *arg0, struct _struct_osViModeTable_0x50 *); /* static */
void __osSetCompare(s32, u32);                      /* static */
s32 __osSetFpcCsr(M2C_UNK);                         /* static */
void __osSetGlobalIntMask(s32 arg0);                /* static */
void __osSetSR(s32);                                /* static */
void __osSetTimerIntr(u64 arg0, u32 arg1, M2C_UNK, u32); /* static */
void __osSiCreateAccessQueue();                     /* static */
s32 __osSiDeviceBusy();                             /* static */
void __osSiGetAccess(u32);                          /* static */
s32 __osSiRawReadIo(s32 arg0, s32 *arg1);           /* static */
u32 __osSiRawStartDma(s32 arg0, s8 *arg1, s32 *);   /* static */
s32 __osSiRawWriteIo(s32 arg0, s32 arg1);           /* static */
void __osSiRelAccess(s32);                          /* static */
s32 __osSpDeviceBusy(s32);                          /* static */
u32 __osSpGetStatus();                              /* static */
s32 __osSpRawReadIo(u32 *arg0, s32 *arg1);          /* static */
s32 __osSpRawStartDma(s32 arg0, s32 arg1, s16 *arg2, s32 arg3); /* static */
s32 __osSpRawWriteIo(s32 arg0, u32 arg1);           /* static */
s32 __osSpSetPc(s32 arg0);                          /* static */
void __osSpSetStatus(s32 arg0);                     /* static */
s32 __osSumcalc(u8 *arg0, s32 arg1);                /* static */
void __osTimerInterrupt();                          /* static */
void __osTimerServicesInit();                       /* static */
u16 *__osViGetCurrentContext();                     /* static */
void __osViInit();                                  /* static */
void __osViSwapContext(M2C_UNK);                    /* static */
s16 __rmonExecute(M2C_UNK *arg0);                   /* static */
u32 __rmonGetBranchTarget(s32 arg0, s32 arg1, u32 *arg2); /* static */
void __rmonGetExceptionStatus(s16 ***arg0);         /* static */
s16 **__rmonGetTCB(s32 arg0);                       /* static */
void __rmonHitBreak();                              /* static */
void __rmonHitCpuFault();                           /* static */
void __rmonHitSpBreak();                            /* static */
void __rmonIdleRCP();                               /* static */
s32 __rmonRCPrunning();                             /* static */
void __rmonRunRCP();                                /* static */
void __rmonSendData(u32 arg0, u32 arg1);            /* static */
void __rmonSendHeader(s16 ***arg0, u32 arg1, s8 arg2); /* static */
void __rmonSendReply(s16 ***arg0, s16 **arg1, s8 arg2, s16 **); /* static */
s32 __rmonSetSingleStep(s32 arg0, u32 *arg1);       /* static */
M2C_UNK __rmonStepRCP();                            /* static */
void __seqpReleaseVoice(void *arg0, M2C_UNK *arg1, s32 arg2); /* static */
void __seqpStopOsc(void *arg0, M2C_UNK *arg1);      /* static */
void __setInstChanState(void *arg0, void *arg1, u8 arg2); /* static */
s32 __udivdi3(s32 arg2, u32, M2C_UNK, s32);         /* static */
u64 __umoddi3(s32 arg2, u32, M2C_UNK, s32);         /* static */
void __unmapVoice(void *arg0, M2C_UNK *arg1);       /* static */
s32 __voiceNeedsNoteKill(void *arg0, M2C_UNK *arg1, s32 arg2); /* static */
s32 __vsDelta(M2C_UNK *arg0, s32 arg1);             /* static */
s32 __vsPan(M2C_UNK *arg0, void *arg1);             /* static */
s16 __vsVol(M2C_UNK *arg0, void *arg1);             /* static */
void *alAdpcmPull(void *arg0, s16 *arg1, s32 arg2, void *arg4, void *arg4); /* static */
void alAuxBusNew(u8 *arg0, u8 *arg1, s32 arg2);     /* static */
void alAuxBusParam(u8 *arg0, s32 arg1, M2C_UNK *arg2); /* static */
void *alAuxBusPull(void *arg0, s16 *arg1, s32 arg2, void *arg4, void *arg4); /* static */
f32 alCents2Ratio(s32 arg0, u32, s32);              /* static */
void alCopy(M2C_UNK *arg0, u8 *arg1, s32 arg2, M2C_UNK *); /* static */
void alEnvmixerNew(M2C_UNK *arg0, M2C_UNK *arg1);   /* static */
void alEnvmixerParam(M2C_UNK *arg0, s32 arg1, M2C_UNK *arg2); /* static */
void *alEnvmixerPull(void *arg0, s16 *arg1, s32 arg2, void *arg4, void *arg4); /* static */
void alEvtqFlushType(M2C_UNK *arg0, s16 arg1);      /* static */
void alEvtqNew(M2C_UNK *arg0, u8 *arg1, s32 arg2);  /* static */
s32 alEvtqNextEvent(M2C_UNK *arg0, u8 *arg1);       /* static */
void alEvtqPostEvent(M2C_UNK *arg0, s16 *arg1, s32 arg2); /* static */
void alFilterNew(void *(*arg0)(void *, s16 *, s32, void *, void *), void (*arg1)(M2C_UNK *, s32, M2C_UNK *), s32 arg2, s32 arg3); /* static */
void alFxNew(void *arg0, void *arg1, M2C_UNK *arg2); /* static */
void alFxParam(M2C_UNK *arg0, s32 arg1, M2C_UNK *arg2); /* static */
s32 alFxParamHdl(void *arg0, s32 arg1, s32 *arg2);  /* static */
void *alFxPull(void *arg0, s32 arg2, s32 arg4);     /* static */
u8 *alHeapDBAlloc(void *arg2, s32 arg3, M2C_UNK *arg4, u8, s32); /* static */
void alLink(M2C_UNK *arg0, M2C_UNK *arg1, M2C_UNK *); /* static */
void alLoadNew(M2C_UNK *arg0, s32 (*arg1)(void *), M2C_UNK *arg2); /* static */
void alLoadParam(M2C_UNK *arg0, s32 arg1, M2C_UNK *arg2); /* static */
void alMainBusNew(u8 *arg0, u8 *arg1, s32 arg2);    /* static */
void alMainBusParam(u8 *arg0, s32 arg1, u8 *arg2);  /* static */
void *alMainBusPull(void *arg0, s16 *arg1, s32 arg2, void *arg4, void *arg4); /* static */
void alResampleNew(M2C_UNK *arg0, M2C_UNK *arg1);   /* static */
void alResampleParam(M2C_UNK *arg0, s32 arg1, M2C_UNK *arg2); /* static */
void *alResamplePull(void *arg0, s16 *arg1, s32 arg2, void *arg4, void *arg4); /* static */
void alSaveNew(u8 *arg0);                           /* static */
void alSaveParam(u8 *arg0, s32 arg1, u8 *arg2);     /* static */
void *alSavePull(void *arg0, s16 *arg1, s32 arg2, void *arg4, void *arg4); /* static */
s32 alSeqGetTicks(void *arg0);                      /* static */
void alSeqSetLoc(void *arg0, void *arg1);           /* static */
void alSynAddPlayer(u8 **arg0, u8 *arg1);           /* static */
s32 alSynAllocFX(void *arg0, s16 arg1, void *arg2, M2C_UNK *arg3); /* static */
s32 alSynAllocVoice(u8 **arg0, M2C_UNK *arg1, u16 *arg2); /* static */
void alSynDelete(s32 *arg0);                        /* static */
void alSynFreeVoice(u8 **arg0, M2C_UNK *arg1);      /* static */
void alSynNew(void *arg0, void *arg1);              /* static */
void alSynSetFXMix(u8 **arg0, M2C_UNK *arg1, u8 arg2); /* static */
void alSynSetPan(u8 **arg0, M2C_UNK *arg1, s32 arg2); /* static */
void alSynSetPitch(u8 **arg0, M2C_UNK *arg1, f32 arg2); /* static */
void alSynSetPriority(u8 **arg1, M2C_UNK *arg2, M2C_UNK); /* static */
void alSynSetVol(u8 **arg0, M2C_UNK *arg1, s16 arg2, s32 arg3); /* static */
void alSynStartVoice(u8 **arg0, M2C_UNK *arg1, s32 arg2, void *); /* static */
void alSynStartVoiceParams(u8 **arg0, M2C_UNK *arg1, s32 arg2, f32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7); /* static */
void alSynStopVoice(u8 **arg0, M2C_UNK *arg1);      /* static */
void alUnlink(M2C_UNK *arg0, s32);                  /* static */
s32 bcopy(s16 *arg0, struct _struct_osViModeTable_0x50 *arg1, s32 arg2, M2C_UNK *); /* static */
void bzero(M2C_UNK *arg0, s32 arg1, M2C_UNK, M2C_UNK); /* static */
s32 func_8000306C(f32 *arg0, void *arg1, f32 *arg2); /* static */
void func_80007420(f32 *arg0, f32 *arg1);           /* static */
s32 func_80007558(f32 *arg0, s32 arg1);             /* static */
void func_80007D28(f32 *arg0, void *arg2, void *);  /* static */
void *func_8000ADA4(s32 arg1, s32 *);               /* static */
void func_8000AE90();                               /* static */
M2C_UNK func_8000AF94();                            /* static */
void func_8000B150(M2C_UNK *arg0, M2C_UNK **arg1, s32 arg2, f32 arg3); /* static */
void func_8000B2D0(s16 **arg0);                     /* static */
void func_8000BD28(s16 **arg0);                     /* static */
void func_8000C2A8();                               /* static */
void func_8000C424(s16 **arg0);                     /* static */
void func_8000C510(s16 **arg0);                     /* static */
void func_8000C590(s16 **arg0, s16 **arg1, s32 arg2, s32 arg3, u8 arg4); /* static */
void func_8000C6D8(s16 **arg0, M2C_UNK *arg1, s16 *(*arg2)[]); /* static */
s16 **func_8000C7C0(s16 **arg0);                    /* static */
void func_8000C7C8(s16 **arg0);                     /* static */
void func_8000C9D8(s16 **arg0);                     /* static */
void func_8000CADC(s16 **arg0);                     /* static */
void func_8000CBCC(s16 **arg0);                     /* static */
s32 func_8000CCB0(s16 **arg1, s16 *);               /* static */
void func_8000CD8C(s16 **arg0, s16 *arg1);          /* static */
void func_8000CDE4(s16 **arg0, M2C_UNK *arg1, s32 arg2); /* static */
void func_8000CEF8(s16 **arg0);                     /* static */
s32 func_8000CF30(s16 **arg0, M2C_UNK **arg1, s32 *arg2, s32 arg3); /* static */
void func_8000D150(s32);                            /* static */
void func_8000DCE0(s32 *arg0, M2C_UNK *arg2, s32 *); /* static */
void func_8000E0A4(s16 **arg0);                     /* static */
s32 func_8000E740(void *arg0, void *arg1);          /* static */
u32 (*func_8000EA88(u8 **arg0))(u32, s32);          /* static */
u32 func_8000EACC();                                /* static */
s32 func_8000EBDC(M2C_UNK **arg0, f32 *arg1, s8 arg2, s32 arg3, u32 arg4, u8 arg5); /* static */
s32 func_8000EF58(void *arg0, f32 *arg1);           /* static */
void func_8000F334(M2C_UNK *arg0);                  /* static */
void func_8000F350();                               /* static */
void func_8000F3E0(s32, s16 *, s16 *, s16 *);       /* static */
void func_8000F6EC();                               /* static */
u16 func_8000F750(s16 *(*)[]);                      /* static */
void func_8000F890();                               /* static */
void func_8000F8F4();                               /* static */
void func_8000FA34();                               /* static */
void func_8000FA4C();                               /* static */
void func_8000FDB0();                               /* static */
void func_8000FF64(s16 **arg0);                     /* static */
void *func_800107D0(s32 *arg0, s32 *, M2C_UNK);     /* static */
void func_80010BF4();                               /* static */
void func_80010D28(s32, s32, M2C_UNK *, struct _struct_osViModeTable_0x50 *); /* static */
void func_80010DB0();                               /* static */
void func_800113E0(u8 *arg0);                       /* static */
void func_80011550();                               /* static */
M2C_UNK *func_800116B8(s32 arg0);                   /* static */
void func_80011868(M2C_UNK *arg0, f32 *arg1);       /* static */
void func_80011C20(M2C_UNK *arg0);                  /* static */
s32 *func_80011C88(s32 *arg0, M2C_UNK *arg1, M2C_UNK, M2C_UNK); /* static */
void func_80011E60();                               /* static */
void func_80011FB0(f32 *arg0);                      /* static */
void func_80012090(M2C_UNK *arg0);                  /* static */
void func_80012180(M2C_UNK *arg0, f32 arg1, f32 arg2, f32 arg3); /* static */
void func_80012250(M2C_UNK *arg0, f32 arg1, f32 arg2, f32 arg3); /* static */
void func_80012320(M2C_UNK *arg0, f32 *arg1);       /* static */
void func_80012760(M2C_UNK *arg0, M2C_UNK *arg1);   /* static */
s32 func_80014E90();                                /* static */
u32 func_8001547C(u32 arg0, struct _struct_D_800A6990_0x68 *arg1, M2C_UNK *); /* static */
s32 func_800154D0();                                /* static */
void func_80015988(f32 *arg0, f32 *arg1, f32 *arg2); /* static */
void func_80015EA4(M2C_UNK *arg0, s32 arg1, s32 arg2, u16 arg3, s32 arg4); /* static */
void func_80016298(void *arg0);                     /* static */
void func_8001650C(void *arg0, s16 arg1, s16 arg2, f32 arg3, f32 arg4); /* static */
M2C_UNK *func_800169B4(u8 *arg0, M2C_UNK *arg1, u8 arg2, s32 arg3, u8 arg4); /* static */
s32 func_80016B00(s32 arg0, u8 *arg1, M2C_UNK *arg2, u8 arg3, u8 arg4, u8 arg5); /* static */
void func_80016C3C(M2C_UNK **arg0, u8 *arg1, s16 arg2, u8 arg3, s32 arg4, s32 arg5); /* static */
void func_800177B0(M2C_UNK **arg0, s32 *arg1);      /* static */
void func_80017878(M2C_UNK *arg0, M2C_UNK **arg1);  /* static */
s32 func_80017A64(M2C_UNK *arg0, M2C_UNK *arg1, s32 arg2); /* static */
M2C_UNK *func_800182E4(M2C_UNK *arg0, M2C_UNK *arg1, s32 arg2); /* static */
s32 func_8001831C(u32 arg0, s32);                   /* static */
void func_80018DBC();                               /* static */
void func_80019040();                               /* static */
void *func_80019174(void *arg0, s16 arg1);          /* static */
void func_8001933C(M2C_UNK *arg0);                  /* static */
void func_800193C4(M2C_UNK *arg0);                  /* static */
void func_800193F0(void *arg0);                     /* static */
void func_80019520(void *arg0);                     /* static */
void func_800195F4(void *arg0);                     /* static */
void func_80019688(void *arg0);                     /* static */
void func_800196BC(M2C_UNK *arg0);                  /* static */
void func_800197C8(M2C_UNK *arg0);                  /* static */
void func_8001B4D0(s16 arg0, M2C_UNK *arg1, M2C_UNK *arg2, u16); /* static */
void func_8001B55C(s16 arg0, s32 arg1, M2C_UNK *arg2, M2C_UNK *arg3); /* static */
s16 func_8001BB18(s32 arg0);                        /* static */
void func_8001BE80();                               /* static */
void func_8001BE88();                               /* static */
void func_8001BE90(s16 *(*)[]);                     /* static */
void func_8001BE98(M2C_UNK);                        /* static */
void func_8001BEA0();                               /* static */
void func_8001BF08();                               /* static */
f32 func_8001C450(f32 arg0);                        /* static */
f32 func_8001C5A0(f32 arg0);                        /* static */
void func_8001C990(s32, s32 *);                     /* static */
void func_8001CAC0();                               /* static */
s32 func_8001D340();                                /* static */
void func_8001D350(u8 *arg0, u8 arg1);              /* static */
void func_8001D4A0(u8 *arg0, u8 *arg1);             /* static */
void func_8001D4E0(u8 *arg0, M2C_UNK *arg1);        /* static */
void func_8001D574(void *arg0, u8 *arg1, M2C_UNK *arg2); /* static */
void func_8001D620(void *arg0, u8 *arg1, M2C_UNK *arg2); /* static */
void func_8001D6B4(void *arg0, u8 *arg1, M2C_UNK *); /* static */
void func_8001D70C(void *arg0, u8 *arg1, void *arg2); /* static */
void func_8001D770(u8 **arg0, s32 *, M2C_UNK *, M2C_UNK *); /* static */
void func_8001D79C(u8 **);                          /* static */
void func_8001D820(M2C_UNK *arg0, s8 *arg1, s32 arg2); /* static */
void func_8001D8B0(u8 *arg0, u8 *arg1);             /* static */
s32 func_8001DB30(u8 *arg0, u32 arg1, s16 *arg2, u8 *); /* static */
u8 func_8001E244(u8 *arg0, u32 arg1, u8);           /* static */
s32 func_8001E2FC(u8 *arg0, u32 arg1, s32);         /* static */
u16 func_8001E370(u8 *arg0);                        /* static */
s32 func_8001E380(u8 *arg0);                        /* static */
void func_8001E390(u8 *arg0);                       /* static */
void func_8001E3C0(u8 *arg0, s32 arg1);             /* static */
void func_8001E3F0(u8 *arg0, u8 *arg1);             /* static */
void func_8001E420(u8 *arg0, s16 arg1);             /* static */
void func_8001E450(u8 *arg0);                       /* static */
void func_8001E480(u8 *arg0, s32 *arg1);            /* static */
s32 func_8001E5F0(void *arg0);                      /* static */
void func_8001EAB8(void *arg0);                     /* static */
void func_8001EB2C(void *arg0);                     /* static */
void func_8001EC2C(void *arg0, s16 *arg1);          /* static */
void func_8001F488(void *arg0, s16 *arg1);          /* static */
void func_8001F618(void *arg0, M2C_UNK *arg1);      /* static */
void func_8001F6A8(void *arg0, f32 arg1);           /* static */
void func_8001F6E0(u8 *arg0, u32 *arg1);            /* static */
s32 func_8001F808(void *arg0);                      /* static */
void func_8001F88C(void *arg0, void *arg1);         /* static */
void func_8001FCE4(M2C_UNK *arg0, M2C_UNK *arg1);   /* static */
s32 func_8001FD8C(s32 arg0, f32 arg1);              /* static */
s16 func_8001FDE0(u8 *arg0, void *arg1);            /* static */
void func_8001FEA0(u8 *arg0, s16 arg1);             /* static */
void func_8001FEF0(u8 *arg0, s16 arg1);             /* static */
void func_8001FF00(u8 *arg0);                       /* static */
s16 func_8001FF60(u8 *arg0);                        /* static */
void func_8001FF70(u8 *arg0);                       /* static */
s32 func_8001FFC0(u8 *arg0);                        /* static */
void func_8001FFE0(u8 *arg0, M2C_UNK32 arg1);       /* static */
void func_80020030(u8 *arg0, s16 arg1, s32 arg2);   /* static */
void func_80020060(u8 *arg0, s16 arg1);             /* static */
void func_800200B0(u8 *arg0, u8 arg1);              /* static */
void func_80020100(u8 *arg0, u8 arg1);              /* static */
u8 *func_8002041C(u8 *arg0, s32 *arg1, u32 arg2, s16 arg3); /* static */
void func_80020610(u8 **arg0);                      /* static */
s32 func_800206A4(u8 **arg0, s32 arg1);             /* static */
s32 func_80020738(u8 **arg0, u8 **arg1, u8 *);      /* static */
s32 func_800209B8(M2C_UNK **arg0, s16 arg1, s16 arg2); /* static */
u32 func_80021000();                                /* static */
s32 func_80021010(s32 arg0);                        /* static */
s32 func_80021140(u8 *arg0, s32 arg1);              /* static */
s32 func_800211E0(u8 *arg0, M2C_UNK arg2, s32 arg3); /* static */
void func_80021280(void *arg0);                     /* static */
void func_8002148C(void *);                         /* static */
void func_80021748(s16 **arg0);                     /* static */
void func_80021990(s32 arg0);                       /* static */
void func_80021B00(s32 arg0);                       /* static */
void func_80021EA0(s32 arg0, s32 arg1);             /* static */
u32 func_80021FC0(s16 *(*arg0)[]);                  /* static */
void func_80022048(struct _struct_D_800A67F8_0x6 (*arg0)[]); /* static */
void func_800220E4();                               /* static */
u32 func_800221A0(s16 *(*arg0)[], s16 *arg1);       /* static */
void func_800222A0();                               /* static */
u32 func_800222E0(s16 **arg0, s8 *arg1, s16 *arg2); /* static */
void func_8002247C(s8 *arg0, s16 *arg1);            /* static */
void func_80022530(s8 arg0);                        /* static */
void func_80022824(s32 arg0, s32 arg1);             /* static */
s32 func_80022E0C(struct _struct_D_800A6990_0x68 *arg0, u16 *arg1); /* static */
u32 func_80022F64(struct _struct_D_800A6990_0x68 *arg0, s32 arg1, u16 *arg2); /* static */
u32 func_800230D0(struct _struct_D_800A6990_0x68 *arg0, u16 arg1, s32 arg2, u8 *arg3, u8 *arg4, s32 arg5, u32 *arg6); /* static */
u32 func_80023460(struct _struct_D_800A6990_0x68 *arg0, u16 *arg1, s32 arg2, s32 *arg3, u32 arg4, s32 *arg5, s32 *arg6); /* static */
u32 func_8002362C(struct _struct_D_800A6990_0x68 *arg0, s32 arg1, u8 *arg2, u8 arg3); /* static */
u32 func_800236C0(struct _struct_D_800A6990_0x68 *arg0, s32 arg1, s32 arg2, u8 *arg3, u8 *arg4); /* static */
u32 func_800238EC(struct _struct_D_800A6990_0x68 *arg0, u16 *arg1, u8 arg2, u16 *arg3, u32 arg4, u16 *arg5, s32 arg6); /* static */
u32 func_80023A8C(struct _struct_D_800A6990_0x68 *arg0, s32 arg1, u16 *arg2, u8 arg3); /* static */
u32 func_80023B60(struct _struct_D_800A6990_0x68 *arg0, u32 arg1, s32 arg2, s32 arg3, s32 arg4, u8 *arg5); /* static */
u32 func_80023F90(struct _struct_D_800A6990_0x68 *arg0, u32 arg1, void *arg2); /* static */
u32 func_80024360(s16 **arg0, u8 *arg1);            /* static */
void func_800244DC(u8 arg0);                        /* static */
void func_80024580(s8 *arg0, s16 *arg1);            /* static */
u32 func_80024750(struct _struct_D_800A6990_0x68 *arg0, s32 *arg1, s32 *arg2); /* static */
s32 func_80024850(s16 **arg0);                      /* static */
u32 func_800248D0(s16 **arg0, s32 arg1, u8 *arg2, s32 arg3); /* static */
u32 func_800249C0(s16 **arg0, s32 arg1, u8 *arg2, s32 arg3); /* static */
u32 func_80024A30(s16 **arg0, struct _struct_D_800A6990_0x68 *arg1, s32 arg2); /* static */
void func_80024CE0(s32 arg0);                       /* static */
void func_80024D70(s32 arg0, M2C_UNK *arg1);        /* static */
void func_8002500C(s16 **arg0);                     /* static */
s32 func_80025950(struct _struct_D_800A5200_0x18 *arg0, s8 arg1, s32 arg2, u8 *arg3, u8 *arg4, s32 arg5, s16 *(*arg6)[]); /* static */
s32 func_80025DC0(s32 arg0, u8 *arg1, s32 arg2, s32 arg3); /* static */
s32 func_80025E18(s32 arg0, u8 *arg2, s32);         /* static */
void func_80026E30(s16 **arg0, s64, s64, s64);      /* static */
void *func_80028DFC(void *arg0, void *arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5, s32 arg6); /* static */
void *func_80029664(void *arg0, u16 *arg1, u16 *arg2, s32 arg3, s32 arg4, void *arg5); /* static */
u16 func_80029A3C(f64 arg0, f64 arg1, s32 arg4, s32 arg5, void *); /* static */
f32 func_80029CBC(f32 arg0, s32 arg1, s16 arg2, u16 arg3); /* static */
void *func_8002AA48(void *arg0, void *arg1, s16 arg2, s32 arg3, void *arg4); /* static */
void *func_8002AC60(void *arg0, u32 arg1, s16 arg2, s32 arg3, void *arg4); /* static */
void *func_8002ADD0(void *arg0, u32 arg1, s16 arg2, s32 arg3, void *arg4); /* static */
void *func_8002AF40(void *arg0, s16 arg1, s32 arg2, void *arg3); /* static */
f32 func_8002AFDC(void *arg0, s32 arg1);            /* static */
s32 func_8002B304(void *arg0);                      /* static */
void func_8002B7D8(void *arg0);                     /* static */
void func_8002B8A8(void *arg0);                     /* static */
void func_8002BAA8(void *arg0, s16 *arg1);          /* static */
void func_8002C2D0(void *arg0, s16 *arg1);          /* static */
void func_8002C8BC(void *arg0, u8 arg1);            /* static */
void func_8002CAE0(void *arg0, f32 arg1, void *);   /* static */
void func_8002D210(s32 arg0, s32 arg1, u8 *arg2);   /* static */
u32 func_8002D42C(struct _struct_D_800A6990_0x68 *arg0, u8 *arg1, u8 *arg2); /* static */
u32 func_8002D70C(struct _struct_D_800A6990_0x68 *arg0, u8 *arg1, u16, u16 *); /* static */
void func_8002DFD4(s8 arg0);                        /* static */
void func_8002E218(s8 arg0, u8 *arg1);              /* static */
void func_8002ED0C(u32, M2C_UNK);                   /* static */
void func_8002EDDC(M2C_UNK *, s32, void *);         /* static */
void func_8002F640(M2C_UNK *arg0, s32 **arg1, u8 arg2, u8 *arg3); /* static */
void func_8002FC08(void *arg0, s16 *arg1);          /* static */
u8 func_80030024(void *arg0, u8);                   /* static */
s16 func_80030038(void *arg0);                      /* static */
s32 func_80030068(void *arg0);                      /* static */
s32 func_800300B8(void *arg0);                      /* static */
s16 func_80030190(M2C_UNK *arg0);                   /* static */
s16 func_80030280(M2C_UNK *arg0);                   /* static */
s16 func_800303F0(M2C_UNK *arg0);                   /* static */
s16 func_80030614(M2C_UNK *arg0);                   /* static */
s16 func_8003066C(M2C_UNK *arg0);                   /* static */
s16 func_80030674(M2C_UNK *arg0);                   /* static */
s16 func_80030730(M2C_UNK *arg0);                   /* static */
s16 func_80030784(M2C_UNK *arg0);                   /* static */
s16 func_80030910(M2C_UNK *arg0);                   /* static */
s16 func_80030A60(M2C_UNK *arg0);                   /* static */
s16 func_80030B38(M2C_UNK *arg0);                   /* static */
s16 func_80030C28(M2C_UNK *arg0);                   /* static */
s16 func_80030CB4(M2C_UNK *arg0);                   /* static */
s16 func_80030E14(M2C_UNK *arg0);                   /* static */
s16 func_80030FAC(M2C_UNK *arg0);                   /* static */
s16 func_80031118(M2C_UNK *arg0);                   /* static */
s16 func_800312F0(M2C_UNK *arg0);                   /* static */
s16 func_800316D4(M2C_UNK *arg0);                   /* static */
s16 func_800319A8(M2C_UNK *arg0);                   /* static */
s16 func_80031A00(M2C_UNK *arg0);                   /* static */
s16 func_80031BB4(M2C_UNK *arg0);                   /* static */
s16 func_80032820(s16 *arg0, M2C_UNK *arg1);        /* static */
void func_800328B8(M2C_UNK *arg0, s8 arg1, u8 *arg2, s16 arg3, s32 arg4); /* static */
s16 func_80033070(M2C_UNK *arg0);                   /* static */
s16 func_800331E8(M2C_UNK *arg0);                   /* static */
s16 func_800331F0(M2C_UNK *arg0);                   /* static */
void func_80033808();                               /* static */
s32 *ldiv(s32 *arg0, s32 arg1, s32 arg2);           /* static */
s32 *lldiv(s32 *arg0, s32 arg2, s32 arg3, s32 arg4, u32 arg5); /* static */
u8 *memcpy(u8 *arg0, u8 *arg1, s32 arg2, s32);      /* static */
void osCreateMesgQueue(s16 *(*arg0)[], s16 **arg1, s16 *arg2); /* static */
void osCreatePiManager(s32 arg0, s16 *(*arg1)[], s16 **arg2, s16 *arg3); /* static */
void osCreateThread(s16 **arg0, s32 arg1, void (*arg2)(s16 **), s16 **arg3, s16 **arg4, s32 arg5); /* static */
void osCreateViManager(s32 arg0);                   /* static */
s32 osEPiRawReadIo(M2C_UNK *arg0, s32 arg1, s32 *arg2); /* static */
s32 osEPiRawStartDma(M2C_UNK *arg0, u32 arg1, s32 arg2, u8 *arg3, s32 arg4); /* static */
s32 osEPiRawWriteIo(M2C_UNK *arg0, s32 arg1, s32 arg2); /* static */
u32 osEepromRead(s16 **arg0, s32 arg1, u8 *arg2);   /* static */
u32 osEepromWrite(s16 **arg0, s32 arg1, u8 *arg2);  /* static */
u32 osGetCount();                                   /* static */
s32 osGetThreadPri(s16 **arg0);                     /* static */
u64 osGetTime();                                    /* static */
void osInvalDCache(u8 *arg0, s32 arg1);             /* static */
void osInvalICache(u32 arg0, s32 arg1);             /* static */
s32 osJamMesg(s16 **arg0, s16 **arg1, s32 arg2);    /* static */
void osMapTLBRdb();                                 /* static */
u32 osPfsChecker(struct _struct_D_800A6990_0x68 *arg0); /* static */
u32 osPfsFindFile(struct _struct_D_800A6990_0x68 *arg0, u16 arg1, s32 arg2, u8 *arg3, u8 *arg4, u32 *arg5); /* static */
u32 osPfsFreeBlocks(struct _struct_D_800A6990_0x68 *arg0, s32 *arg1); /* static */
s16 **osPiGetCmdQueue(u8 *);                        /* static */
s32 osPiRawReadIo(s32 arg0, u32 *arg1);             /* static */
s32 osPiRawStartDma(s32 arg0, s32 arg1, u8 *arg2, s32 arg3); /* static */
s32 osRecvMesg(s16 *(*arg0)[], s16 **arg1, s32 arg2); /* static */
s32 osSendMesg(s16 **arg0, s16 **arg1, s32 arg2, u32); /* static */
void osSetEventMesg(s32 arg0, s16 *(*arg1)[], s16 *arg2); /* static */
s32 osSetIntMask(s32 arg0, s32);                    /* static */
void osSetThreadPri(s16 **arg0, s32 arg1);          /* static */
s32 osSetTimer(M2C_UNK *arg0, s16 *(*arg2)[], s16 **arg3, s16 *(*arg4)[], s16 **arg5, s32 arg6, s32 arg7); /* static */
void osSpTaskYield(void *);                         /* static */
s32 osSpTaskYielded(void *arg0);                    /* static */
void osStartThread(s16 **arg0);                     /* static */
void osStopThread(s16 **arg0);                      /* static */
void osViBlack(s32 arg0);                           /* static */
s32 osViGetCurrentFramebuffer();                    /* static */
s32 osViGetNextFramebuffer();                       /* static */
void osViSetEvent(s16 *(*arg0)[], s32 arg1, u8 arg2); /* static */
void osViSetMode(struct _struct_osViModeTable_0x50 *arg0, s32, M2C_UNK *, struct _struct_osViModeTable_0x50 *); /* static */
u32 osVirtualToPhysical(u8 *arg0, u32, u8, s32);    /* static */
void osWritebackDCache(s16 *arg0, s32 arg1, M2C_UNK); /* static */
void osYieldThread();                               /* static */
f32 sqrtf(f32 arg0, f32, s32, s32, f32 *);          /* static */
extern M2C_UNK D_196710;
extern u8 D_196CC0;
extern M2C_UNK D_1982B0;
extern u8 D_1D7720;
extern u8 D_1E2870;
extern M2C_UNK D_1E3EA0;
extern M2C_UNK D_611F0;
extern s32 D_80000000;
extern s32 D_80000180;
extern s32 D_80000184;
extern s32 D_80000188;
extern s32 D_80000194;
extern s32 D_80000198;
extern s32 D_8000019C;
extern M2C_UNK D_80001;
extern u8 D_80115000;
extern s32 D_8013A800;
extern s32 D_8013A804;
extern M2C_UNK D_8013A810;
extern M2C_UNK D_8013B110;
extern s32 D_8013B510;
extern M2C_UNK D_80173510;
extern M2C_UNK D_8069E600;
extern s16 D_807B2600;
extern s16 D_807D7E00;
extern M2C_UNK D_A0000000;
extern s32 D_A4040000;
extern s32 D_A4040010;
extern s32 D_A4080000;
extern u32 D_A4100000;
extern s32 D_A430000C;
extern s32 D_A4400000;
extern u32 D_A4500000;
extern u32 D_A4600000;
extern s32 D_BFF00000;
extern s32 D_BFF00004;
extern s32 func_800E0000;
extern M2C_UNK gGameHeap;
extern M2C_UNK osAppNMIBuffer;
extern s32 osResetType;
extern s32 osRomBase;
extern s32 osTvType;
static M2C_UNK D_80038150;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038250;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038350;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038360;                          /* unable to generate initializer: unknown type */
static u32 D_80038370[0xC] = {
    0x3B9ACA00,
    0x05F5E100,
    0x989680,
    0xF4240,
    0x186A0,
    0x2710,
    0x3E8,
    0x64,
    0xA,
    1,
    0,
    0,
};
static u8 D_800383A0[0x60] = {
    0,
    1,
    2,
    3,
    0,
    1,
    3,
    2,
    0,
    2,
    1,
    3,
    0,
    2,
    3,
    1,
    0,
    3,
    1,
    2,
    0,
    3,
    2,
    1,
    1,
    0,
    2,
    3,
    1,
    0,
    3,
    2,
    1,
    2,
    0,
    3,
    1,
    2,
    3,
    0,
    1,
    3,
    0,
    2,
    1,
    3,
    2,
    0,
    2,
    0,
    1,
    3,
    2,
    0,
    3,
    1,
    2,
    1,
    0,
    3,
    2,
    1,
    3,
    0,
    2,
    3,
    0,
    1,
    2,
    3,
    1,
    0,
    3,
    0,
    1,
    2,
    3,
    0,
    2,
    1,
    3,
    1,
    0,
    2,
    3,
    1,
    2,
    0,
    3,
    2,
    0,
    1,
    3,
    2,
    1,
    0,
};
static M2C_UNK D_80038400;                          /* unable to generate initializer: unknown type */
static struct _struct_D_80038410_0xC D_80038410[8] = {
    { 0, 0x1000, 0 },
    { 1, 0x4000, 0x6000 },
    { 2, 0x10000, 0x1E000 },
    { 3, 0x40000, 0x7E000 },
    { 4, 0x100000, 0x1FE000 },
    { 5, 0x400000, 0x7FE000 },
    { 6, 0x01000000, 0x01FFE000 },
    { 0, 0, 0 },
};
static s32 D_80038470 = 0;
static s32 D_80038474 = 0;
static s32 D_80038478 = 1;
static s32 D_80038480 = 0;
static s32 D_80038484 = 0;
static s16 D_80038488 = -1;
static u16 D_8003848A = 0;
static u16 D_8003848C = 0;
static u16 D_8003848E = 0xFFFF;
static u32 D_80038490 = 0;
static s32 D_80038498 = 0;
static s32 D_8003849C = 0;
static s32 D_800384A0 = 0;
static u32 D_800384A4 = 0;
static s32 D_800384A8 = 0;
static s32 D_800384AC = 0;
static M2C_UNK D_800384B0;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_800384CC;                          /* unable to generate initializer: unknown type */
static M2C_UNK32 D_800384E8[0x1E] = {
    0x3F000000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F000000,
    0x3F000000,
    0x3F000000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    0x3F800000,
    1,
};
static M2C_UNK D_80038560;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038648;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_800386A0;                          /* unable to generate initializer: unknown type */
static s32 D_800386C0 = 0;
static s32 D_800386C4 = 0;
static s32 D_800386C8 = 1;
static M2C_UNK D_800386DF;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003871F;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003875C;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003875D;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003875E;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003875F;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003879C;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003879D;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003879E;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003879F;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_800387D0;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038828;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038880;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_800388D8;                          /* unable to generate initializer: unknown type */
static M2C_UNK gSpriteSetupPlainDL;                 /* unable to generate initializer: unknown type */
static M2C_UNK gSpriteSetupDL;                      /* unable to generate initializer: unknown type */
static M2C_UNK gSpriteFinishDL;                     /* unable to generate initializer: unknown type */
static M2C_UNK gSpriteRectSetupDL;                  /* unable to generate initializer: unknown type */
static M2C_UNK gSpriteRectFinishDL;                 /* unable to generate initializer: unknown type */
static M2C_UNK gPhotoSetupDL;                       /* unable to generate initializer: unknown type */
static M2C_UNK gPhotoFinishDL;                      /* unable to generate initializer: unknown type */
static s16 D_80038A78[8] = { 1, 2, 4, 8, 0x10, 0x20, 0x40, 0 };
static f32 D_80038A88[0xA] = { 1.0f, 4.0f, 16.0f, 64.0f, 256.0f, 1024.0f, 4096.0f, 0.0f, 0.0f, 0.0f };
static u8 D_80038AB0[0x10] = { 0x2B, 0x1E, 0x2F, 0x25, 0x22, 0x26, 0x22, 0x2D, 0, 0, 0, 0, 0, 0, 0, 0 };
static u8 D_80038AC0 = 0;
static M2C_UNK D_80038AD0;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038B10;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038B58;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038B88;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038BC0;                          /* unable to generate initializer: unknown type */
static f32 D_80038BD8[4] = { 0.0f, 0.3f, 0.5f, 1.0f };
static f32 D_80038BE8[6] = { 0.5f, 0.3f, 0.1f, 0.0f, 0.0f, 0.0f };
static s32 *D_80038C00[4] = { NULL, NULL, NULL, NULL };
static M2C_UNK D_80038C10;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038C80;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038CC0;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038D00;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038D50;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038DA0;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038DE0;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038E60;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038EA0;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038EE0;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038F20;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038F68;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80038FA8;                          /* unable to generate initializer: unknown type */
static M2C_UNK gJpeg64Zigzag;                       /* unable to generate initializer: unknown type */
static M2C_UNK gJpeg64QuantLuma;                    /* unable to generate initializer: unknown type */
static M2C_UNK gJpeg64QuantChroma;                  /* unable to generate initializer: unknown type */
static M2C_UNK D_800392D0;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_800392F0;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80039310;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80039330;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80039350;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80039550;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80039750;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_80039950;                          /* unable to generate initializer: unknown type */
static M2C_UNK gJpeg64HuffDcLuma;                   /* unable to generate initializer: unknown type */
static M2C_UNK gJpeg64HuffDcChroma;                 /* unable to generate initializer: unknown type */
static M2C_UNK gJpeg64HuffAcLuma;                   /* unable to generate initializer: unknown type */
static M2C_UNK gJpeg64HuffAcChroma;                 /* unable to generate initializer: unknown type */
static M2C_UNK D_8003A014;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003A120;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003A124;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003A128;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003A12A;                          /* unable to generate initializer: unknown type */
static struct _struct_D_8003A12B_0xC D_8003A12B[0x17]; /* unable to generate initializer: non-zero padding */
static u32 D_8003A240[0x10] = {
    0x180000,
    0x3F000000,
    0x7FFF4000,
    0x180000,
    0x3F000000,
    0x3FFF4000,
    8,
    0xD,
    0xA,
    0xC,
    7,
    9,
    0xB,
    6,
    0xE,
    0,
};
static struct _struct_gOverlayTable_0x10 gOverlayTable[4]; /* unable to generate initializer: confusing struct layout */
static M2C_UNK D_8003A2C0;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003A2C4;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003A2C8;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003A2CC;                          /* unable to generate initializer: unknown type */
static s32 D_8003A340[4] = { 4, 0, 8, 0xC };
static s32 D_8003A350 = 0;
static u32 D_8003A354 = 0x03B9ACA0;
static s32 osViClock = 0x02E6D354;
static u32 __OSGlobalIntMask = 0x3FFF01;
static s16 *__osThreadTail[2] = { NULL, (s16 *)0xFFFFFFFF };
static s16 *__osRunQueue[1] = { (s16 *) __osThreadTail };
static s16 **__osActiveQueue = __osThreadTail;
static s16 **__osRunningThread = NULL;
static s32 D_8003A384 = 0;
static s32 D_8003A390 = 0;
static u8 **alGlobals = NULL;
static u8 D_8003A3B0 = 0;
static s16 *D_8003A3C0 = NULL;
static s16 **D_8003A3C4 = NULL;
static s16 *(*D_8003A3C8)[] = NULL;
static s16 *(*D_8003A3CC)[] = NULL;
static s32 D_8003A3D0 = 0;
static s32 D_8003A3D4 = 0;
static s32 D_8003A3D8 = 0;
static struct _struct_osViModeTable_0x50 osViModeTable[0x2A]; /* unable to generate initializer: non-zero padding */
static s32 D_8003B100 = 0;
static s16 D_8003B110[0x28] = {
    0,
    0,
    0,
    0x320E,
    0,
    0x140,
    0x3E5,
    0x2239,
    0,
    0x20D,
    0,
    0xC15,
    0xC15,
    0xC15,
    0x6C,
    0x2EC,
    0,
    0x200,
    0,
    0,
    0,
    0x280,
    0,
    0x400,
    0x25,
    0x1FF,
    0xE,
    0x204,
    0,
    2,
    0,
    0x280,
    0,
    0x400,
    0x25,
    0x1FF,
    0xE,
    0x204,
    0,
    2,
};
static s16 *D_8003B160 = NULL;
static s16 **D_8003B164 = NULL;
static s16 *(*D_8003B168)[] = NULL;
static s16 *(*D_8003B16C)[] = NULL;
static s16 *(*D_8003B170)[] = NULL;
static s32 (*D_8003B174)(s32, s32, u8 *, s32) = NULL;
static s32 (*D_8003B178)(M2C_UNK *, u32, s32, u8 *, s32) = NULL;
static M2C_UNK *__osPiTable = NULL;
static M2C_UNK *__osCurrentHandle[4] = { &D_800C5340, &D_800C30F8, NULL, NULL };
static s32 __osPiAccessQueueEnabled = 0;
static u32 D_8003B1C0 = 0;
static M2C_UNK *D_8003B1D0 = (M2C_UNK *) &D_800C1690;
static s32 D_8003B1E0 = 1;
static s32 D_8003B1F0 = 0;
static M2C_UNK D_8003B200;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003B268;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003B2F0;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003B318;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003B340;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003B368;                          /* unable to generate initializer: unknown type */
static u16 D_8003B390[0x90] = {
    0x7FFF,
    0x7FFC,
    0x7FF5,
    0x7FE8,
    0x7FD7,
    0x7FC0,
    0x7FA5,
    0x7F84,
    0x7F5F,
    0x7F34,
    0x7F05,
    0x7ED0,
    0x7E97,
    0x7E58,
    0x7E15,
    0x7DCD,
    0x7D7F,
    0x7D2D,
    0x7CD6,
    0x7C7A,
    0x7C1A,
    0x7BB4,
    0x7B49,
    0x7ADA,
    0x7A66,
    0x79ED,
    0x796F,
    0x78ED,
    0x7866,
    0x77DA,
    0x7749,
    0x76B4,
    0x761A,
    0x757B,
    0x74D8,
    0x7430,
    0x7384,
    0x72D3,
    0x721E,
    0x7164,
    0x70A6,
    0x6FE3,
    0x6F1C,
    0x6E51,
    0x6D81,
    0x6CAD,
    0x6BD5,
    0x6AF9,
    0x6A18,
    0x6933,
    0x684A,
    0x675D,
    0x666C,
    0x6577,
    0x647E,
    0x6381,
    0x6280,
    0x617C,
    0x6073,
    0x5F67,
    0x5E57,
    0x5D43,
    0x5C2C,
    0x5B11,
    0x59F2,
    0x58D0,
    0x57AA,
    0x5681,
    0x5555,
    0x5425,
    0x52F2,
    0x51BC,
    0x5082,
    0x4F46,
    0x4E06,
    0x4CC3,
    0x4B7D,
    0x4A35,
    0x48E9,
    0x479B,
    0x4649,
    0x44F5,
    0x439E,
    0x4245,
    0x40E9,
    0x3F8A,
    0x3E29,
    0x3CC6,
    0x3B60,
    0x39F8,
    0x388D,
    0x3721,
    0x35B2,
    0x3441,
    0x32CE,
    0x3159,
    0x2FE2,
    0x2E69,
    0x2CEF,
    0x2B72,
    0x29F4,
    0x2875,
    0x26F3,
    0x2570,
    0x23EC,
    0x2266,
    0x20DF,
    0x1F57,
    0x1DCE,
    0x1C43,
    0x1AB7,
    0x192A,
    0x179C,
    0x160E,
    0x147E,
    0x12ED,
    0x115C,
    0xFCA,
    0xE38,
    0xCA5,
    0xB11,
    0x97D,
    0x7E9,
    0x654,
    0x4C0,
    0x32A,
    0x195,
    0,
    0,
    0x10,
    0,
    0x10,
    0,
    0x20,
    0,
    0,
    0xC120,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
};
static u16 D_8003B4B0 = 0;
static s16 D_8003B4B2 = 0;
static s32 D_8003B4B4[0xB] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static u16 D_8003B4E0 = 0;
static s16 D_8003B4E2 = 0;
static s32 D_8003B4E4 = 0;
static M2C_UNK *D_8003B4E8[0xA] = { NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL };
static u16 *__osViCurr = &D_8003B4B0;
static u16 *__osViNext = &D_8003B4E0;
static s32 D_8003B520 = 0;
static M2C_UNK osViModeNtscLan1;                    /* unable to generate initializer: unknown type */
static M2C_UNK osViModePalLan1;                     /* unable to generate initializer: unknown type */
static M2C_UNK osViModeMpalLan1;                    /* unable to generate initializer: unknown type */
static u8 D_8003B620[0x24] = "                                ";
static u8 D_8003B644[0x2C] = "00000000000000000000000000000000";
static s16 (*D_8003B670[0x38])(M2C_UNK *) = {
    func_8003066C,
    func_80030614,
    func_80030674,
    func_800316D4,
    func_800319A8,
    func_80030190,
    func_80031A00,
    func_80031BB4,
    func_80030190,
    func_80030190,
    func_80030910,
    func_80030190,
    func_80030730,
    func_80030784,
    func_80030A60,
    func_80030B38,
    func_80030C28,
    func_80030CB4,
    func_80030280,
    func_800303F0,
    func_80033070,
    func_800331F0,
    func_800331E8,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    (s16 (*)(M2C_UNK *))0x80030A24,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030190,
    func_80030E14,
    func_80030FAC,
    func_80031118,
    func_800312F0,
    func_80030190,
    NULL,
    NULL,
};
static s32 D_8003B750 = 0;
static M2C_UNK D_8003B760;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003B774;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003B790;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003BA50;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003C250;                          /* unable to generate initializer: unknown type */
static M2C_UNK D_8003CA50;                          /* unable to generate initializer: unknown type */
static f64 D_8003D520 = 1.5707963267948966;
static f64 D_8003D528 = -1.5707963267948966;
static f64 D_8003D530 = 3.141592653589793;
static f64 D_8003D538 = 3.141592653589793;
static f64 D_8003D540 = 0.5;
static s32 D_8003E130;
static s32 D_8003E134;
static s16 *D_8003E138[0x6C];
static s16 *D_8003E2E8[0x6C];
static s16 *D_8003E498[0x6C];
static s16 *D_8003E648[0x26E];
static s16 *D_8003F000[0x200];
static s16 *D_8003F800[0x600];
static s16 *D_80041000[0x200];
static s16 *D_80041800[0x40];
static s16 *D_80041900[6];
static s16 *D_80041918[6];
static s16 *D_80041930;
static s16 *D_80041938[6];
static s16 *D_80041950;
static s16 *D_80041958[0x66E];
static s16 *D_80043310;
static u32 D_80043314;
static s32 D_80043320;
static s16 *D_80043330[6];
static s16 *D_80043348;
static s8 D_80043350[0x5F000];
static u8 *D_800A2350;
static u8 *D_800A2354;
static u8 *D_800A2358;
static void *D_800A235C;
static s32 D_800A2360;
static void *D_800A2364;
static u8 *D_800A2368;
static u16 D_800A236C;
static u8 *D_800A2370;
static s32 D_800A2374;
static u8 *D_800A2378;
static M2C_UNK D_800A2380;
static u8 *D_800A2390;
static u8 *D_800A2398;
static s16 *D_800A23A8[0x6C];
static s16 *D_800A2558[0x46];
static s16 *D_800A2670[0x46];
static u8 *D_800A2788[0x816];
static u8 D_800A47E0;
static M2C_UNK *D_800A47E4;
static M2C_UNK *D_800A47E8;
static M2C_UNK *D_800A47F0;
static M2C_UNK *D_800A47F4;
static struct _struct_D_800A4800_0x14 D_800A4800[0x7F];
static u32 D_800A51F0;
static s32 D_800A51F4;
static s32 D_800A51F8;
static struct _struct_D_800A5200_0x18 D_800A5200[0x80];
static s16 *D_800A5E00[6];
static s16 *D_800A5E18[0x80];
static M2C_UNK D_800A6018;
static M2C_UNK D_800A601C;
static M2C_UNK D_800A601E;
static M2C_UNK D_800A601F;
static struct _struct_D_800A6020_0xC D_800A6020[0x1C];
static M2C_UNK D_800A6178;
static M2C_UNK *D_800A6194;
static M2C_UNK *D_800A61A0[0x190];
static s32 D_800A67E0;
static s32 D_800A67E4;
static s16 D_800A67E8;
static struct _struct_D_800A67F8_0x6 D_800A67F8[4];
static s16 D_800A6810[0x10];
static u16 D_800A6830[4];
static u16 D_800A6838[4];
static u16 D_800A6840;
static s32 D_800A6844;
static struct _struct_osViModeTable_0x50 D_800A6850; /* type too large by 0x48 */
static s32 D_800A6858[6];
static s32 D_800A6870;
static s32 D_800A6878[5];
static s32 D_800A688C[5];
static struct _struct_osViModeTable_0x50 *D_800A68A0;
static M2C_UNK D_800A68A8;
static u32 D_800A68C4[5];
static u32 D_800A68D8[5];
static u32 D_800A68EC;
static s16 D_800A68F8;
static s16 D_800A68FA;
static s16 D_800A68FC;
static s16 D_800A68FE;
static s16 D_800A6900;
static s16 D_800A6902;
static s16 D_800A6904;
static s16 D_800A6906;
static s32 D_800A6908;
static M2C_UNK D_800A6910;
static M2C_UNK D_800A6912;
static M2C_UNK D_800A6914;
static M2C_UNK D_800A6916;
static M2C_UNK D_800A6918;
static M2C_UNK D_800A691A;
static M2C_UNK D_800A691C;
static struct _struct_D_800A691E_0x10 D_800A691E[7];
static struct _struct_D_800A6990_0x68 D_800A6990[4];
static M2C_UNK *D_800A6B30;
static M2C_UNK D_800A6B34;
static s32 D_800A6BE0;
static M2C_UNK *D_800A6BE4[0x2B];
static s32 D_800A6C90;
static u8 *D_800A6C94;
static f32 D_800A6C98[7];
static M2C_UNK D_800A6CB4;
static f32 D_800A6CB8[0x38];
static f32 D_800A6D98[7];
static M2C_UNK D_800A6DB4;
static u16 D_800A6E98[0x104];
static s16 D_800A70A0[0x300];
static M2C_UNK D_800A76A0;
static s16 D_800A79A0[0x180];
static M2C_UNK D_800A7CA0;
static M2C_UNK D_800A7FA0;
static s16 D_800A7FAA;
static s16 *D_800A7FB0[6];
static s16 *D_800A7FC8;
static s16 D_800A7FD0;
static s32 D_800A7FD4;
static u8 *D_800A7FE0;
static u8 *D_800A7FE8;
static s32 D_800A7FEC;
static u8 *D_800A7FF0;
static u8 *D_800A7FF8;
static u8 *D_800A7FFC;
static u8 *D_800A8000;
static u8 *D_800A8008;
static s32 D_800A800C;
static u16 D_800A8010;
static s16 *D_800A8018[0x46E];
static s16 *D_800A91D0[6];
static s16 *D_800A91E8[6];
static s16 D_800A9200;
static s8 D_800A9202;
static s32 D_800A9204[5];
static s16 D_800A9218;
static s8 D_800A921A;
static s32 D_800A921C[5];
static s16 *D_800A9230[0x46C];
static s16 *D_800AA3E0[0x16C];
static s16 *D_800AA990[6];
static s16 *D_800AA9A8;
static s16 *D_800AA9B0[6];
static s16 *D_800AA9C8;
static s16 *D_800AA9D0[6];
static s16 *D_800AA9E8;
static s16 *D_800AA9F0;
static u8 D_800AAA00;
static u8 D_800AAA01;
static u8 D_800AAA02;
static u8 D_800AAA03;
static u8 D_800AAA04;
static u8 D_800AAA05;
static u8 D_800AAA06;
static u8 D_800AAA07;
static u8 D_800AAA08;
static s16 *D_800AAA10[6];
static s16 *D_800AAA28;
static s16 *D_800AAA30;
static s16 *D_800AAA40[6];
static s16 *D_800AAA58;
static s16 *D_800AAA60[0x6C];
static s16 *D_800AAC10[0x1008];
static s16 *D_800AEC30[8];
static s16 *D_800AEC50[8];
static u32 D_800AEC70;
static u32 D_800AEC74;
static u32 D_800AEC78;
static u32 D_800AEC7C;
static u32 D_800AEC80;
static u32 D_800AEC84;
static s32 D_800AEC90;
static M2C_UNK D_800AECA0;
static s8 D_800AF100;
static s8 D_800AF101;
static s32 D_800AF104;
static M2C_UNK *D_800AF108;
static struct _struct_D_800AF198_0x8 D_800AF110[];  /* type too large by 4 */
static u32 D_800AF114[0x1F];
static u32 *D_800AF190;
static u32 D_800AF194;
static struct _struct_D_800AF198_0x8 D_800AF198[0x11];
static s16 D_800AF220;
static u16 D_800AF222;
static u16 D_800AF224;
static void *D_800AF22C;
static s32 D_800AF230;
static M2C_UNK *D_800AF234;
static u16 D_800AF238;
static u16 D_800AF23A;
static M2C_UNK *D_800AF240;
static M2C_UNK *D_800AF244;
static s32 D_800AF254;
static u16 D_800AF258;
static u16 D_800AF25A;
static u32 __osBaseCounter;
static s32 D_800AF260;
static s32 D_800AF264;
static s32 D_800AF268;
static s32 D_800B0270;
static u16 D_800B0274;
static u16 D_800B0276;
static s32 D_800B0428;
static s32 D_800B042C;
static s32 D_800B0430;
static s32 D_800B0434;
static u16 D_800B0438;
static u16 D_800B043A;
static u16 D_800B043C;
static f32 D_800B0444;
static f32 D_800B0448;
static f32 D_800B044C;
static s8 D_800B0450[0x38];
static s32 D_800B0488;
static s32 D_800B048C;
static u32 D_800B0490;
static s16 *D_800B0494;
static u16 D_800B0498;
static u16 D_800B049A;
static u32 D_800B049C;
static M2C_UNK *D_800B04A0;
static u16 D_800B04A4[4];
static s16 D_800B04AC[4];
static s32 D_800B04B4;
static f32 D_800B04B8;
static f32 D_800B04BC;
static f32 D_800B04C0;
static s32 D_800B04C4;
static s32 D_800B04C8;
static s32 D_800B04CC;
static s32 D_800B04D0;
static M2C_UNK D_800B04D4;
static s32 D_800B04D8;
static s16 *D_800B04E0[6];
static u32 D_800B04F8;
static M2C_UNK *D_800B0500;
static s32 D_800B0504;
static s16 *D_800B0508[6];
static s16 *D_800B0520[6];
static s32 D_800B0538;
static s32 *D_800B053C;
static s32 D_800B0540;
static s32 D_800B0544;
static s32 D_800B0548;
static M2C_UNK D_800B054C;
static M2C_UNK *D_800B0554;
static s32 D_800B0558;
static s16 **D_800B0568;
static s32 D_800B056C;
static s32 D_800B0570;
static u16 D_800B0574;
static u16 D_800B0576;
static M2C_UNK *D_800B0578;
static u32 D_800B0588;
static u16 D_800B058C;
static u16 D_800B058E;
static u8 D_800B0594;
static u16 D_800B0596;
static u16 D_800B0598;
static s32 D_800B05A0;
static M2C_UNK **D_800B05A4[5];
static f32 D_800B05B8[0x10];
static u8 D_800B05F8;
static s32 D_800B0600;
static u32 D_800B0604;
static s32 D_800B0608;
static M2C_UNK D_800B060C;
static s16 D_800B060E;
static s16 D_800B0610[4];
static u8 D_800B0618;
static M2C_UNK *D_800B061C;
static s16 *D_800B0620[0x40];
static s32 D_800B0720;
static u8 D_800B0724;
static s32 D_800B0728;
static M2C_UNK D_800B0730;
static s16 D_800B2888[0x10];
static s16 *D_800B28A8[0x858];
static s16 D_800B4A08[0x10];
static s16 *D_800B4A28;
static M2C_UNK D_800B4A30;
static s16 D_800B4A38[4];
static s8 D_800B4A40[0x3C];
static s32 D_800B4A7C;
static u16 D_800B4A80;
static u16 D_800B4A82;
static s16 *D_800B4A88[6];
static s32 D_800B4AA0;
static s32 D_800B4AA4;
static M2C_UNK D_800B4AB0;
static u16 D_800C0AB0;
static M2C_UNK *D_800C0AB4;
static s32 D_800C0AB8;
static s16 *D_800C0AC0[0xA2];
static u16 D_800C0D48;
static u16 D_800C0D4A;
static u16 D_800C0D4C;
static s16 **D_800C0D50;
static u8 D_800C0D54;
static s16 D_800C0D56;
static s32 D_800C0D5C;
static s32 D_800C0D60;
static s32 D_800C0D64;
static s32 D_800C0D68;
static s16 D_800C0D6C;
static s32 D_800C0D70;
static s32 D_800C0D74;
static s32 D_800C0D78;
static s16 D_800C0D7C;
static s32 D_800C0D80;
static s32 D_800C0D84;
static s32 D_800C0D88;
static s16 D_800C0D8C;
static s32 D_800C0D90;
static s32 D_800C0D94;
static s32 D_800C0D98;
static s16 D_800C0D9C;
static s32 D_800C0DA0;
static s32 D_800C0DA4;
static s32 D_800C0DA8;
static s16 D_800C0DAC;
static s32 D_800C0DB0;
static s32 D_800C0DB4;
static s32 D_800C0DB8;
static s16 D_800C0DBC;
static s32 D_800C0DC0[0x24];
static M2C_UNK D_800C0E50;
static s32 D_800C1650;
static M2C_UNK D_800C1654;
static u8 D_800C1658;
static M2C_UNK D_800C165C;
static f32 D_800C1664;
static f32 D_800C1668;
static f32 D_800C166C;
static s16 *D_800C1670;
static s32 D_800C1674;
static u16 D_800C1678;
static u16 D_800C167A[0xB];
static M2C_UNK D_800C1690;
static s32 D_800C16B4;
static M2C_UNK D_800C16B8;
static u8 D_800C16C0[0x20];
static u16 D_800C16E0;
static u16 D_800C16E2;
static f32 D_800C16E8[0x10];
static u32 D_800C1728;
static s32 D_800C172C;
static M2C_UNK D_800C1730;
static M2C_UNK D_800C1778;
static s32 D_800C1788;
static u16 D_800C178C;
static s8 D_800C1795;
static s16 *D_800C1798[0x40];
static u16 D_800C1898[0xC04];
static s16 D_800C30A0[4];
static s16 *D_800C30A8[6];
static M2C_UNK *D_800C30C0;
static u16 D_800C30C4;
static u32 D_800C30C8;
static s32 gScreenWidth;
static s16 D_800C30D4[6];
static s16 *D_800C30E0[6];
static M2C_UNK D_800C30F8;
static M2C_UNK D_800C30FC;
static s8 D_800C30FD;
static s8 D_800C30FE;
static s8 D_800C30FF;
static s8 D_800C3100;
static s8 D_800C3101;
static s32 D_800C3104;
static s32 D_800C3108[0x19];
static M2C_UNK *D_800C316C;
static u32 D_800C3170;
static s32 D_800C3174;
static M2C_UNK D_800C3178;
static M2C_UNK *D_800C3190[8];
static u16 D_800C31B0[0x1008];
static s16 *D_800C51C0;
static s32 D_800C51C4;
static s32 D_800C51C8;
static u32 D_800C51CC;
static s32 D_800C51D0;
static f32 D_800C51D4;
static s32 D_800C51D8;
static M2C_UNK *D_800C51DC;
static struct _struct_D_800C51E0_0x8 D_800C51E0[8];
static s16 **D_800C5220;
static s32 D_800C5224[0x1D];
static s16 D_800C5298[4];
static u16 D_800C52A0;
static s32 D_800C52A4;
static u16 D_800C52A8;
static u16 D_800C52AA;
static s8 D_800C52B0[0x3C];
static s32 D_800C52EC;
static M2C_UNK D_800C52F0;
static u32 D_800C52F4;
static s16 D_800C5304[6];
static s16 *D_800C5310[6];
static s16 *D_800C5328;
static u16 D_800C532C;
static u16 D_800C532E;
static s32 D_800C5330;
static u16 D_800C5334;
static u16 D_800C5336;
static M2C_UNK D_800C5340;
static s8 D_800C5344;
static s8 D_800C5345;
static s8 D_800C5346;
static s8 D_800C5347;
static s8 D_800C5348;
static s8 D_800C5349;
static M2C_UNK D_800C534C;
static s32 D_800C5350[0x19];
static u8 D_800C53B4;
static s16 *D_800C53B8[6];
static s32 D_800C53D0;
static u32 D_800C53D4;
static u16 D_800C53D8;
static u16 D_800C53DA;
static s16 *D_800C53DC;
static s16 *D_800C53E0[6];
static u32 D_800C53F8;
static void *D_800C53FC;
static s32 D_8003D5E8 = 0x3FF00000;                 /* const */
static M2C_UNK D_8003D6B0;                          /* unable to generate initializer: unknown type; const */
static M2C_UNK D_8003D780;                          /* unable to generate initializer: unknown type; const */
static M2C_UNK D_8003D788;                          /* unable to generate initializer: unknown type; const */
static u16 __osRcpImTable[0x40] = {
    0x555,
    0x556,
    0x559,
    0x55A,
    0x565,
    0x566,
    0x569,
    0x56A,
    0x595,
    0x596,
    0x599,
    0x59A,
    0x5A5,
    0x5A6,
    0x5A9,
    0x5AA,
    0x655,
    0x656,
    0x659,
    0x65A,
    0x665,
    0x666,
    0x669,
    0x66A,
    0x695,
    0x696,
    0x699,
    0x69A,
    0x6A5,
    0x6A6,
    0x6A9,
    0x6AA,
    0x955,
    0x956,
    0x959,
    0x95A,
    0x965,
    0x966,
    0x969,
    0x96A,
    0x995,
    0x996,
    0x999,
    0x99A,
    0x9A5,
    0x9A6,
    0x9A9,
    0x9AA,
    0xA55,
    0xA56,
    0xA59,
    0xA5A,
    0xA65,
    0xA66,
    0xA69,
    0xA6A,
    0xA95,
    0xA96,
    0xA99,
    0xA9A,
    0xAA5,
    0xAA6,
    0xAA9,
    0xAAA,
}; /* const */
static M2C_UNK D_8003DD30;                          /* unable to generate initializer: unknown type; const */
static s32 D_8003DEA8[6] = { 1, 2, 4, 8, 0x10, 0 }; /* const */
static u8 D_8003E050[8] = { 0x69, 0x6D, 0x65, 0x6D, 0, 0, 0, 0 }; /* const */
static u8 D_8003E058[8] = { 0x72, 0x6D, 0x6F, 0x6E, 0, 0, 0, 0 }; /* const */
static f64 D_8003E060[9] = { 10.0, 100.0, 10000.0, 100000000.0, 1e+16, 1e+32, 1e+64, 1e+128, 1e+256 }; /* const */
static M2C_UNK D_8003E0A8;                          /* unable to generate initializer: unknown type; const */
static M2C_UNK D_8003E0AC;                          /* unable to generate initializer: unknown type; const */

f32 Math_Atan2f(f32 arg0, f32 arg1) {
    f32 temp_f0;
    f32 temp_f2;
    f32 temp_f8;
    f32 var_f0;
    f32 var_f4;
    f32 var_f4_2;
    f32 var_f6;
    f64 temp_f0_2;
    f64 var_f0_2;
    s32 var_a0;
    s32 var_v0;
    s32 var_v1;
    s32 var_v1_2;

    var_f0 = 0.0f;
    var_v0 = 1;
    if (arg1 != 0.0f) {
        var_v0 = 0;
    }
    var_v1 = 1;
    if (arg0 != 0.0f) {
        var_v1 = 0;
    }
    if (!(var_v0 & var_v1)) {
        if (arg1 != 0.0f) {
            temp_f8 = arg0 / arg1;
            var_f4_2 = temp_f8;
            if (!(fabsf(temp_f8) <= 1.0f)) {
                var_f4_2 = 1.0f / temp_f8;
            }
            var_f6 = 0.0f;
            var_v1_2 = 0xE;
            var_a0 = 0x1D;
            do {
                temp_f2 = (f32) (var_v1_2 * var_v1_2) * var_f4_2 * var_f4_2;
                temp_f0 = (f32) var_a0 + var_f6;
                var_a0 -= 2;
                var_v1_2 -= 1;
                var_f6 = temp_f2 / temp_f0;
            } while (var_v1_2 > 0);
            var_f4 = var_f4_2 / (var_f6 + 1.0f);
            if (temp_f8 > 1.0f) {
                var_f4 = (f32) (D_8003D520 - (f64) var_f4);
            }
            if (temp_f8 < -1.0f) {
                var_f4 = (f32) (D_8003D528 - (f64) var_f4);
            }
            if (arg1 > 0.0f) {
                return var_f4;
            }
            temp_f0_2 = (f64) var_f4;
            if (!(arg0 > 0.0f)) {
                var_f0_2 = temp_f0_2 - D_8003D530;
            } else {
                var_f0_2 = temp_f0_2 + D_8003D538;
            }
            var_f0 = (f32) var_f0_2;
            /* Duplicate return node #24. Try simplifying control flow for better match */
            return var_f0;
        }
        var_f0 = -1.5707964f;
        if (arg0 > 0.0f) {
            return 1.5707964f;
        }
        /* Duplicate return node #24. Try simplifying control flow for better match */
        return var_f0;
    }
    return var_f0;
}

void Math_Atanf(void) {
    Math_Atan2f(1.0f);
}

s32 Math_VecToAngle(f32 arg0, f32 arg1) {
    f32 var_f12;
    f32 var_f14;
    s32 temp_v1;
    s32 var_s0;
    s32 var_v0;
    s32 var_v0_2;

    var_f12 = arg0;
    var_f14 = arg1;
    var_s0 = 0;
    if (var_f12 == 0.0f) {
        if (var_f14 == 0.0f) {
            return 0;
        }
        var_v0_2 = 0x100;
        if (var_f14 < 0.0f) {
            return 0x300;
        }
        /* Duplicate return node #23. Try simplifying control flow for better match */
        return var_v0_2;
    }
    if (var_f14 == 0.0f) {
        var_v0_2 = 0;
        if (var_f12 < 0.0f) {
            return 0x200;
        }
        /* Duplicate return node #23. Try simplifying control flow for better match */
        return var_v0_2;
    }
    if (var_f12 < 0.0f) {
        var_s0 = 1;
        var_f12 = -var_f12;
    }
    if (var_f14 < 0.0f) {
        var_s0 |= 2;
        var_f14 = -var_f14;
    }
    temp_v1 = (s32) ((f64) ((Math_Atan2f(var_f14 / var_f12, 1.0f) / 3.1415927f) * 512.0f) + D_8003D540);
    var_v0 = 0x200;
    if (var_s0 != 1) {
        if (var_s0 >= 2) {
            var_v0 = 0x400;
            switch (var_s0) {                       /* irregular */
            case 3:
                var_v0_2 = (temp_v1 + 0x200) & 0x3FF;
                /* Duplicate return node #23. Try simplifying control flow for better match */
                return var_v0_2;
            default:
                return 1;
            }
        } else {
            var_v0_2 = temp_v1 & 0x3FF;
            if (var_s0 != 0) {
                return 1;
            }
            return var_v0_2;
        }
    } else {
    case 2:
        return (var_v0 - temp_v1) & 0x3FF;
    }
}

s32 func_800007A0(void **arg0, u8 *arg1, s32 arg2) {
    s32 temp_v0;
    s32 temp_v1;
    s32 var_t0;
    u8 *var_a1;
    void *var_a3;

    var_a1 = arg1;
    var_a3 = *arg0;
    var_t0 = 0;
    if (*var_a1 != 0) {
loop_1:
        temp_v1 = *var_a1 & 0xFF;
        var_a1 += 1;
        if (temp_v1 != 0) {
            temp_v0 = M2C_FIELD(var_a3, u8 *, 0) & 0xFF;
            var_a3 += 1;
            if ((temp_v1 != temp_v0) && (temp_v1 != *(arg2 + temp_v0))) {
                var_a1 += 1;
                if (*var_a1 != 0) {
                    do {
                        var_a1 += 1;
                    } while (*var_a1 != 0);
                }
                goto block_10;
            }
            goto loop_1;
        }
        if ((*(arg2 + M2C_FIELD(var_a3, u8 *, 0)) == 0) || (*(arg2 + M2C_FIELD(var_a3, u8 *, -1)) == 0)) {
            *arg0 = var_a3;
            return var_t0;
        }
block_10:
        var_t0 += 1;
        if (*var_a1 != 0) {
            var_a3 = *arg0;
            goto loop_1;
        }
        goto block_12;
    }
block_12:
    return -1;
}

s32 func_80000858(void **arg0, u8 *arg1) {
    s32 temp_v0;
    s32 temp_v1;
    s32 var_a3;
    u8 *var_a1;
    void *var_a2;

    var_a1 = arg1;
    var_a2 = *arg0;
    var_a3 = 0;
    if (*var_a1 != 0) {
loop_1:
        temp_v1 = *var_a1 & 0xFF;
        var_a1 += 1;
        if (temp_v1 != 0) {
            temp_v0 = M2C_FIELD(var_a2, u8 *, 0) & 0xFF;
            var_a2 += 1;
            if ((temp_v1 != temp_v0) && (temp_v1 != *(&D_80038150 + temp_v0))) {
                var_a1 += 1;
                if (*var_a1 != 0) {
                    do {
                        var_a1 += 1;
                    } while (*var_a1 != 0);
                }
                goto block_10;
            }
            goto loop_1;
        }
        if ((*(&D_80038150 + M2C_FIELD(var_a2, u8 *, 0)) == 0) || (*(&D_80038150 + M2C_FIELD(var_a2, u8 *, -1)) == 0)) {
            *arg0 = var_a2;
            return var_a3;
        }
block_10:
        var_a3 += 1;
        if (*var_a1 != 0) {
            var_a2 = *arg0;
            goto loop_1;
        }
        goto block_12;
    }
block_12:
    return -1;
}

s32 func_80000918(void **arg0, u8 *arg1) {
    s32 temp_v0;
    s32 temp_v1;
    s32 var_a3;
    u8 *var_a1;
    void *var_a2;

    var_a1 = arg1;
    var_a2 = *arg0;
    var_a3 = 0;
    if (*var_a1 != 0) {
loop_1:
        temp_v1 = *var_a1 & 0xFF;
        var_a1 += 1;
        if (temp_v1 != 0) {
            temp_v0 = M2C_FIELD(var_a2, u8 *, 0) & 0xFF;
            var_a2 += 1;
            if ((temp_v1 != temp_v0) && (temp_v1 != *(&D_80038250 + temp_v0))) {
                var_a1 += 1;
                if (*var_a1 != 0) {
                    do {
                        var_a1 += 1;
                    } while (*var_a1 != 0);
                }
                goto block_10;
            }
            goto loop_1;
        }
        if ((*(&D_80038250 + M2C_FIELD(var_a2, u8 *, 0)) == 0) || (*(&D_80038250 + M2C_FIELD(var_a2, u8 *, -1)) == 0)) {
            *arg0 = var_a2;
            return var_a3;
        }
block_10:
        var_a3 += 1;
        if (*var_a1 != 0) {
            var_a2 = *arg0;
            goto loop_1;
        }
        goto block_12;
    }
block_12:
    return -1;
}

s32 func_800009D8(u8 **arg0, u8 *arg1) {
    s32 temp_a2;
    s32 var_a3;
    u8 *var_a1;
    u8 *var_v1;

    var_a1 = arg1;
    var_v1 = *arg0;
    var_a3 = 0;
    if (*var_a1 != 0) {
loop_1:
        temp_a2 = *var_a1 & 0xFF;
        var_a1 += 1;
        if (temp_a2 == 0) {
            *arg0 = var_v1;
            return var_a3;
        }
        var_v1 += 1;
        if (temp_a2 != *var_v1) {
            do {
                var_a1 += 1;
            } while (*var_a1 != 0);
            var_a3 += 1;
            if (*var_a1 != 0) {
                var_v1 = *arg0;
                goto loop_1;
            }
            goto block_7;
        }
        goto loop_1;
    }
block_7:
    return -1;
}

void *func_80000A40(s32 *arg0, void *arg1, u8 *arg2) {
    s32 temp_v0;
    s32 temp_v1;
    s32 var_a3;
    s32 var_v0;
    u8 *var_a2;
    void *var_a1;

    var_a1 = arg1;
    var_a2 = arg2;
    var_a3 = 0;
    if (*var_a2 != 0) {
loop_1:
        temp_v1 = *var_a2 & 0xFF;
        var_a2 += 1;
        if (temp_v1 != 0) {
            temp_v0 = M2C_FIELD(var_a1, u8 *, 0) & 0xFF;
            var_a1 += 1;
            if ((temp_v1 != temp_v0) && (temp_v1 != *(&D_80038150 + temp_v0))) {
                var_a2 += 1;
                if (*var_a2 != 0) {
                    do {
                        var_a2 += 1;
                    } while (*var_a2 != 0);
                }
                goto block_10;
            }
            goto loop_1;
        }
        if ((*(&D_80038150 + M2C_FIELD(var_a1, u8 *, 0)) == 0) || (*(&D_80038150 + M2C_FIELD(var_a1, u8 *, -1)) == 0)) {
            arg1 = var_a1;
            var_v0 = var_a3;
        } else {
block_10:
            var_a3 += 1;
            if (*var_a2 != 0) {
                var_a1 = arg1;
                goto loop_1;
            }
            goto block_12;
        }
    } else {
block_12:
        var_v0 = -1;
    }
    *arg0 = var_v0;
    return arg1;
}

void *func_80000B04(s32 *arg0, void *arg1, u8 *arg2) {
    s32 temp_v0;
    s32 temp_v1;
    s32 var_a3;
    s32 var_v0;
    u8 *var_a2;
    void *var_a1;

    var_a1 = arg1;
    var_a2 = arg2;
    var_a3 = 0;
    if (*var_a2 != 0) {
loop_1:
        temp_v1 = *var_a2 & 0xFF;
        var_a2 += 1;
        if (temp_v1 != 0) {
            temp_v0 = M2C_FIELD(var_a1, u8 *, 0) & 0xFF;
            var_a1 += 1;
            if ((temp_v1 != temp_v0) && (temp_v1 != *(&D_80038250 + temp_v0))) {
                var_a2 += 1;
                if (*var_a2 != 0) {
                    do {
                        var_a2 += 1;
                    } while (*var_a2 != 0);
                }
                goto block_10;
            }
            goto loop_1;
        }
        if ((*(&D_80038250 + M2C_FIELD(var_a1, u8 *, 0)) == 0) || (*(&D_80038250 + M2C_FIELD(var_a1, u8 *, -1)) == 0)) {
            arg1 = var_a1;
            var_v0 = var_a3;
        } else {
block_10:
            var_a3 += 1;
            if (*var_a2 != 0) {
                var_a1 = arg1;
                goto loop_1;
            }
            goto block_12;
        }
    } else {
block_12:
        var_v0 = -1;
    }
    *arg0 = var_v0;
    return arg1;
}

u8 *func_80000BC8(s32 *arg0, u8 *arg1, u8 *arg2) {
    s32 temp_v1;
    s32 var_a3;
    s32 var_v0;
    u8 *var_a1;
    u8 *var_a2;

    var_a1 = arg1;
    var_a2 = arg2;
    var_a3 = 0;
    if (*var_a2 != 0) {
loop_1:
        temp_v1 = *var_a2 & 0xFF;
        var_a2 += 1;
        if (temp_v1 == 0) {
            arg1 = var_a1;
            var_v0 = var_a3;
        } else {
            var_a1 += 1;
            if (temp_v1 != *var_a1) {
                do {
                    var_a2 += 1;
                } while (*var_a2 != 0);
                var_a3 += 1;
                if (*var_a2 != 0) {
                    var_a1 = arg1;
                    goto loop_1;
                }
                goto block_7;
            }
            goto loop_1;
        }
    } else {
block_7:
        var_v0 = -1;
    }
    *arg0 = var_v0;
    return arg1;
}

s32 func_80000C40(u8 *arg0, s32 arg1) {
    s32 var_a1;
    s32 var_a2;
    s32 var_v0;
    s32 var_v1;
    u8 *var_a0;
    u8 temp_v0;

    var_a0 = arg0;
    var_a1 = arg1;
    var_v1 = 0xFFFF;
    do {
        temp_v0 = *var_a0;
        var_a0 += 1;
        var_a2 = 7;
        var_v1 ^= temp_v0 << 8;
        var_v0 = var_v1 & 0x8000;
loop_2:
        if (var_v0 != 0) {
            var_v1 = 0x1021 ^ (var_v1 * 2);
        } else {
            var_v1 *= 2;
        }
        var_a2 -= 1;
        var_v0 = var_v1 & 0x8000;
        if (var_a2 >= 0) {
            goto loop_2;
        }
        var_a1 -= 1;
    } while (var_a1 != 0);
    return ~var_v1 & 0xFFFF;
}

void func_80000CA0(void) {

}

u8 *func_80000CB0(u8 *arg0, u8 *arg1, void *arg2) {
    s32 temp_t6;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v1;
    s32 var_a3_2;
    s32 var_a3_3;
    s32 var_a3_4;
    s32 var_t1_2;
    s32 var_t2_2;
    s32 var_t3;
    s32 var_t6;
    s32 var_t7;
    s32 var_t8;
    s32 var_v0_3;
    s32 var_v1_3;
    s32 var_v1_4;
    s32 var_v1_5;
    s32 var_v1_6;
    s32 var_v1_7;
    s8 *temp_v0_5;
    s8 *var_v1_2;
    u32 *var_t1;
    u32 temp_lo;
    u32 var_t0_2;
    u8 *temp_a1;
    u8 *temp_t0;
    u8 *temp_t1;
    u8 *temp_t1_2;
    u8 *temp_t1_3;
    u8 *var_a0;
    u8 *var_a0_2;
    u8 *var_a1;
    u8 *var_t0;
    u8 *var_t0_3;
    u8 *var_t2;
    u8 *var_t4;
    u8 *var_t5;
    u8 *var_v0_2;
    u8 *var_v0_4;
    u8 *var_v0_5;
    u8 *var_v1;
    u8 temp_a3;
    u8 temp_v0;
    u8 temp_v0_6;
    u8 temp_v0_7;
    u8 var_a3;
    u8 var_t0_4;
    void *var_a2;
    void *var_v0;

    var_a0 = arg0;
    var_a2 = arg2;
    var_a3 = *arg1;
    var_a1 = arg1 + 1;
    if (var_a3 != 0) {
        do {
            if ((var_a3 == 0x5C) && (temp_v0 = *var_a1, (temp_v0 == 0x25))) {
                *var_a0 = temp_v0;
                var_a1 += 1;
                var_a0 += 1;
            } else {
                temp_t0 = var_a1;
                if (var_a3 != 0x25) {
                    *var_a0 = var_a3;
                    var_a0 += 1;
                } else {
                    var_t8 = 0;
                    var_t6 = 0;
                    var_t7 = 0;
loop_8:
                    temp_a3 = *var_a1;
                    var_a1 += 1;
                    if (temp_a3 == 0x30) {
                        var_t7 |= 1;
                        goto loop_8;
                    }
                    if (temp_a3 == 0x2D) {
                        var_t7 |= 2;
                        goto loop_8;
                    }
                    temp_a1 = var_a1 - 1;
                    var_a3_2 = *temp_a1 - 0x30;
                    var_a1 = temp_a1 + 1;
                    if (var_a3_2 < 0xA) {
                        do {
                            var_t6 = (var_t6 * 0xA) + var_a3_2;
                            var_a3_2 = *var_a1 - 0x30;
                            var_a1 += 1;
                        } while (var_a3_2 < 0xA);
                    }
                    var_t5 = sp;
                    switch (var_a3_2) {
                    case 0x43:
                        var_t5 = M2C_FIELD(var_a2, u8 **, 0);
                        var_a2 += 4;
                        var_v1 = var_t5 + 1;
                        if (*var_t5 != 0) {
                            do {
                                var_v1 += 1;
                            } while (*var_v1 != 0);
                        }
                        var_v1_2 = var_v1 - 1;
                        break;
                    case 0x48:
                        var_t0 = sp;
                        temp_t1 = M2C_FIELD(var_a2, u8 **, 0);
                        var_a2 += 4;
                        var_a3_3 = 0;
                        var_v1_3 = 0x1C;
                        do {
                            temp_v0_2 = ((u32) temp_t1 >> var_v1_3) & 0xF;
                            if (temp_v0_2 != 0) {
                                var_a3_3 = -1;
                            }
                            var_v1_3 -= 4;
                            *var_t0 = *(&D_80038350 + temp_v0_2);
                            var_t0 -= var_a3_3;
                        } while (var_v1_3 >= 0);
                        var_v0 = var_a3_3 + var_t0;
block_31:
                        M2C_FIELD(var_v0, s8 *, 1) = 0;
                        var_v1_2 = var_t0 + (var_a3_3 + 1);
                        break;
                    case 0x28:
                        var_t0 = sp;
                        temp_t1_2 = M2C_FIELD(var_a2, u8 **, 0);
                        var_a2 += 4;
                        var_a3_3 = 0;
                        var_v1_4 = 0x1C;
                        do {
                            temp_v0_3 = ((u32) temp_t1_2 >> var_v1_4) & 0xF;
                            if (temp_v0_3 != 0) {
                                var_a3_3 = -1;
                            }
                            var_v1_4 -= 4;
                            *var_t0 = *(&D_80038360 + temp_v0_3);
                            var_t0 -= var_a3_3;
                        } while (var_v1_4 >= 0);
                        var_v0 = var_a3_3 + var_t0;
                        goto block_31;
                    case 0x34:
                        var_v0_2 = M2C_FIELD(var_a2, u8 **, 0);
                        var_a2 += 4;
                        if ((s32) var_v0_2 < 0) {
                            var_v0_2 = (u8 *) -(s32) var_v0_2;
                            var_t8 = 1;
                        }
                        var_t4 = sp;
                        var_t2 = var_v0_2;
                        var_t0_2 = *D_80038370;
                        var_t3 = 0;
                        var_t1 = &D_80038370[1];
                        if (var_t0_2 != 0) {
                            do {
                                temp_lo = (u32) var_t2 / var_t0_2;
                                if (temp_lo != 0) {
                                    var_t3 = -1;
                                }
                                temp_v1 = temp_lo * var_t0_2;
                                *var_t4 = temp_lo + 0x30;
                                var_t4 -= var_t3;
                                var_t0_2 = *var_t1;
                                var_t1 += 4;
                                var_t2 -= temp_v1;
                            } while (var_t0_2 != 0);
                        }
                        M2C_FIELD((var_t3 + var_t4), s8 *, 1) = 0;
                        var_v1_2 = var_t4 + (var_t3 + 1);
                        break;
                    case 0x32:
                        var_t0_3 = sp;
                        temp_t1_3 = M2C_FIELD(var_a2, u8 **, 0);
                        var_a2 += 4;
                        var_a3_4 = 0;
                        var_v1_5 = 0x1F;
                        do {
                            temp_v0_4 = ((u32) temp_t1_3 >> var_v1_5) & 1;
                            var_a3_4 |= temp_v0_4;
                            *var_t0_3 = temp_v0_4 + 0x30;
                            var_v1_5 -= 1;
                            var_t0_3 += var_a3_4;
                        } while (var_v1_5 >= 0);
                        temp_v0_5 = var_t0_3 - (var_a3_4 - 1);
                        *temp_v0_5 = 0;
                        var_v1_2 = temp_v0_5;
                        break;
                    case 0x33:
                        temp_v0_6 = M2C_FIELD(var_a2, u8 *, 3);
                        var_a2 += 4;
                        var_v1_2 = sp + 1;
                        M2C_FIELD(sp, s8 *, 1) = 0;
                        M2C_FIELD(sp, u8 *, 0) = temp_v0_6;
                        break;
                    default:
                        var_v1_2 = NULL;
                        var_a1 = temp_t0;
                        *var_a0 = 0x25;
                        var_a0 += 1;
                        break;
                    }
                    var_t0_4 = 0x20;
                    if (var_v1_2 != NULL) {
                        var_t2_2 = 0;
                        temp_t6 = var_t6 - ((var_v1_2 - var_t5) + var_t8);
                        var_t1_2 = 0;
                        if (temp_t6 > 0) {
                            if (var_t7 != 0) {
                                if (!(var_t7 & 2)) {
                                    var_t1_2 = temp_t6;
                                    var_t0_4 = 0x30;
                                } else {
                                    var_t2_2 = temp_t6;
                                }
                            } else {
                                var_v0_3 = temp_t6 - 1;
                                if (var_v0_3 != -1) {
                                    do {
                                        *var_a0 = 0x20;
                                        var_v0_3 -= 1;
                                        var_a0 += 1;
                                    } while (var_v0_3 != -1);
                                }
                            }
                        }
                        *var_a0 = 0x2D;
                        var_v1_6 = var_t1_2 - 1;
                        var_v0_4 = var_a0 + var_t8;
                        if (var_v1_6 != -1) {
                            do {
                                *var_v0_4 = var_t0_4;
                                var_v1_6 -= 1;
                                var_v0_4 += 1;
                            } while (var_v1_6 != -1);
                        }
                        var_a0_2 = var_v0_4;
                        do {
                            temp_v0_7 = *var_t5;
                            var_t5 += 1;
                            *var_a0_2 = temp_v0_7;
                            var_a0_2 += 1;
                        } while (temp_v0_7 != 0);
                        var_v1_7 = var_t2_2 - 1;
                        var_v0_5 = var_a0_2 - 1;
                        if (var_v1_7 != -1) {
                            do {
                                *var_v0_5 = var_t0_4;
                                var_v1_7 -= 1;
                                var_v0_5 += 1;
                            } while (var_v1_7 != -1);
                        }
                        var_a0 = var_v0_5;
                    }
                }
            }
            var_a3 = *var_a1;
            var_a1 += 1;
        } while (var_a3 != 0);
    }
    *var_a0 = 0;
    return var_a0;
}

void func_80001068(s8 *arg1, M2C_UNK *arg2, s32 arg3) {
    func_80000CB0((u8 *) &arg2);
}

void Plane_FromPoints(f32 *arg0, f32 *arg1, M2C_UNK *arg2, M2C_UNK *arg3) {
    f32 temp_f0;
    f32 temp_f10;
    f32 temp_f14;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;
    f32 temp_f6;
    f32 temp_f6_2;
    f32 temp_f6_3;
    f32 temp_f8;

    temp_f6 = M2C_FIELD(arg1, f32 *, 4);
    temp_f2 = M2C_FIELD(arg1, f32 *, 8);
    temp_f10 = M2C_FIELD(arg2, f32 *, 4) - temp_f6;
    temp_f8 = M2C_FIELD(arg3, f32 *, 8) - temp_f2;
    temp_f0 = M2C_FIELD(arg2, f32 *, 8) - temp_f2;
    temp_f4 = M2C_FIELD(arg3, f32 *, 4) - temp_f6;
    temp_f14 = M2C_FIELD(arg1, f32 *, 0);
    temp_f2_2 = M2C_FIELD(arg3, f32 *, 0) - temp_f14;
    temp_f6_2 = M2C_FIELD(arg2, f32 *, 0) - temp_f14;
    temp_f6_3 = (temp_f6_2 * temp_f4) - (temp_f10 * temp_f2_2);
    M2C_FIELD(arg0, f32 *, 0) = (temp_f10 * temp_f8) - (temp_f0 * temp_f4);
    M2C_FIELD(arg0, f32 *, 4) = (f32) ((temp_f0 * temp_f2_2) - (temp_f6_2 * temp_f8));
    M2C_FIELD(arg0, f32 *, 8) = temp_f6_3;
    M2C_FIELD(arg0, f32 *, 0xC) = (f32) -((M2C_FIELD(arg2, f32 *, 0) * M2C_FIELD(arg0, f32 *, 0)) + (M2C_FIELD(arg2, f32 *, 4) * M2C_FIELD(arg0, f32 *, 4)) + (M2C_FIELD(arg2, f32 *, 8) * temp_f6_3));
}

s32 Plane_FromTriangle(void *arg0, void *arg1) {
    f32 temp_f0;
    f32 temp_f10;
    f32 temp_f14;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;
    f32 temp_f6;
    f32 temp_f6_2;
    f32 temp_f6_3;
    f32 temp_f8;

    temp_f6 = M2C_FIELD(arg1, f32 *, 4);
    temp_f2 = M2C_FIELD(arg1, f32 *, 8);
    temp_f10 = M2C_FIELD(arg1, f32 *, 0x10) - temp_f6;
    temp_f8 = M2C_FIELD(arg1, f32 *, 0x20) - temp_f2;
    temp_f0 = M2C_FIELD(arg1, f32 *, 0x14) - temp_f2;
    temp_f4 = M2C_FIELD(arg1, f32 *, 0x1C) - temp_f6;
    temp_f14 = M2C_FIELD(arg1, f32 *, 0);
    temp_f2_2 = M2C_FIELD(arg1, f32 *, 0x18) - temp_f14;
    temp_f6_2 = M2C_FIELD(arg1, f32 *, 0xC) - temp_f14;
    temp_f6_3 = (temp_f6_2 * temp_f4) - (temp_f10 * temp_f2_2);
    M2C_FIELD(arg0, f32 *, 0) = (f32) ((temp_f10 * temp_f8) - (temp_f0 * temp_f4));
    M2C_FIELD(arg0, f32 *, 4) = (f32) ((temp_f0 * temp_f2_2) - (temp_f6_2 * temp_f8));
    M2C_FIELD(arg0, f32 *, 8) = temp_f6_3;
    M2C_FIELD(arg0, f32 *, 0xC) = (f32) -((M2C_FIELD(arg1, f32 *, 0xC) * M2C_FIELD(arg0, f32 *, 0)) + (M2C_FIELD(arg1, f32 *, 0x10) * M2C_FIELD(arg0, f32 *, 4)) + (M2C_FIELD(arg1, f32 *, 0x14) * temp_f6_3));
    return 0;
}

s32 Plane_IntersectLine(void *arg0, void *arg1, void *arg2) {
    f32 temp_f10;
    f32 temp_f12;
    f32 temp_f14;
    f32 temp_f16;
    f32 temp_f4;
    f32 temp_f6;
    f32 temp_f8;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v1;

    temp_f14 = M2C_FIELD(arg2, f32 *, 0);
    temp_f16 = M2C_FIELD(arg1, f32 *, 0xC);
    temp_f12 = M2C_FIELD(arg2, f32 *, 4);
    temp_f10 = M2C_FIELD(arg2, f32 *, 8);
    temp_f8 = (temp_f14 * temp_f16) + (temp_f12 * M2C_FIELD(arg1, f32 *, 0x10)) + (temp_f10 * M2C_FIELD(arg1, f32 *, 0x14));
    var_v0_2 = 1;
    if (!(temp_f8 > -1e-16f)) {
        var_v0_2 = 0;
    }
    var_v1 = 1;
    if (!(temp_f8 < 1e-16f)) {
        var_v1 = 0;
    }
    var_v0 = 0;
    if (!(var_v0_2 & var_v1)) {
        temp_f6 = M2C_FIELD(arg1, f32 *, 0);
        temp_f4 = -((temp_f14 * temp_f6) + (temp_f12 * M2C_FIELD(arg1, f32 *, 4)) + (temp_f10 * M2C_FIELD(arg1, f32 *, 8)) + M2C_FIELD(arg2, f32 *, 0xC)) / temp_f8;
        M2C_FIELD(arg0, f32 *, 0) = (f32) (temp_f6 + (temp_f16 * temp_f4));
        M2C_FIELD(arg0, f32 *, 4) = (f32) (M2C_FIELD(arg1, f32 *, 4) + (M2C_FIELD(arg1, f32 *, 0x10) * temp_f4));
        var_v0 = 1;
        M2C_FIELD(arg0, f32 *, 8) = (f32) (M2C_FIELD(arg1, f32 *, 8) + (M2C_FIELD(arg1, f32 *, 0x14) * temp_f4));
    }
    return var_v0;
}

s32 Line3_FromPlanes(void *arg0, void *arg1, void *arg2) {
    f32 temp_f0;
    f32 temp_f10;
    f32 temp_f12;
    f32 temp_f14;
    f32 temp_f14_2;
    f32 temp_f16;
    f32 temp_f18;
    f32 temp_f20;
    f32 temp_f20_2;
    f32 temp_f4;
    f32 temp_f8;
    f32 var_f18;
    f32 var_f2;
    f32 var_f4;

    M2C_FIELD(arg0, f32 *, 0xC) = (f32) ((M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg2, f32 *, 8)) - (M2C_FIELD(arg2, f32 *, 4) * M2C_FIELD(arg1, f32 *, 8)));
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) ((M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg2, f32 *, 0)) - (M2C_FIELD(arg2, f32 *, 8) * M2C_FIELD(arg1, f32 *, 0)));
    temp_f14 = M2C_FIELD(arg0, f32 *, 0x10);
    temp_f0 = M2C_FIELD(arg0, f32 *, 0xC);
    temp_f18 = (M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg2, f32 *, 4)) - (M2C_FIELD(arg2, f32 *, 0) * M2C_FIELD(arg1, f32 *, 4));
    temp_f20 = (temp_f0 * temp_f0) + (temp_f14 * temp_f14) + (temp_f18 * temp_f18);
    M2C_FIELD(arg0, f32 *, 0x14) = temp_f18;
    if (temp_f20 < 1e-16f) {
        return 0;
    }
    temp_f16 = M2C_FIELD(arg1, f32 *, 0xC);
    temp_f4 = M2C_FIELD(arg2, f32 *, 0xC);
    temp_f20_2 = 1.0f / temp_f20;
    temp_f12 = (temp_f16 * M2C_FIELD(arg2, f32 *, 4)) - (M2C_FIELD(arg1, f32 *, 4) * temp_f4);
    temp_f10 = (temp_f16 * M2C_FIELD(arg2, f32 *, 8)) - (M2C_FIELD(arg1, f32 *, 8) * temp_f4);
    var_f18 = sqrtf(temp_f20_2);
    temp_f8 = (M2C_FIELD(arg1, f32 *, 0) * temp_f4) - (M2C_FIELD(arg2, f32 *, 0) * temp_f16);
    temp_f14_2 = ((temp_f14 * temp_f10) - (temp_f18 * temp_f12)) * temp_f20_2;
    M2C_FIELD(arg0, f32 *, 0) = temp_f14_2;
    M2C_FIELD(arg0, f32 *, 8) = (f32) (((M2C_FIELD(arg0, f32 *, 0xC) * temp_f12) + (M2C_FIELD(arg0, f32 *, 0x10) * temp_f8)) * temp_f20_2);
    M2C_FIELD(arg0, f32 *, 4) = (f32) (-((M2C_FIELD(arg0, f32 *, 0xC) * temp_f10) + (M2C_FIELD(arg0, f32 *, 0x14) * temp_f8)) * temp_f20_2);
    if (var_f18 != var_f18) {
        var_f18 = sqrtf(temp_f20_2, temp_f14_2);
    }
    var_f4 = sqrtf(temp_f20_2);
    M2C_FIELD(arg0, f32 *, 0xC) = (f32) (M2C_FIELD(arg0, f32 *, 0xC) * var_f18);
    if (var_f4 != var_f4) {
        var_f4 = sqrtf(temp_f20_2);
    }
    var_f2 = sqrtf(temp_f20_2);
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) (M2C_FIELD(arg0, f32 *, 0x10) * var_f4);
    if (var_f2 != var_f2) {
        var_f2 = sqrtf(temp_f20_2);
    }
    M2C_FIELD(arg0, f32 *, 0x14) = (f32) (M2C_FIELD(arg0, f32 *, 0x14) * var_f2);
    return 1;
}

s32 Line3_FromPoints(void *arg0, void *arg1, void *arg2) {
    M2C_FIELD(arg0, f32 *, 0) = (f32) M2C_FIELD(arg1, f32 *, 0);
    M2C_FIELD(arg0, f32 *, 4) = (f32) M2C_FIELD(arg1, f32 *, 4);
    M2C_FIELD(arg0, f32 *, 8) = (f32) M2C_FIELD(arg1, f32 *, 8);
    M2C_FIELD(arg0, f32 *, 0xC) = (f32) (M2C_FIELD(arg2, f32 *, 0) - M2C_FIELD(arg1, f32 *, 0));
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) (M2C_FIELD(arg2, f32 *, 4) - M2C_FIELD(arg1, f32 *, 4));
    M2C_FIELD(arg0, f32 *, 0x14) = (f32) (M2C_FIELD(arg2, f32 *, 8) - M2C_FIELD(arg1, f32 *, 8));
    return 1;
}

f32 Line3_DistanceToPoint(void *arg0, void *arg1) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f10;
    f32 temp_f12;
    f32 temp_f14;
    f32 temp_f16;
    f32 temp_f18;
    f32 temp_f20;
    f32 temp_f2;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 temp_f6;
    f32 temp_f6_2;
    f32 temp_f8;
    f32 var_f0;

    temp_f16 = M2C_FIELD(arg1, f32 *, 0xC);
    temp_f18 = M2C_FIELD(arg1, f32 *, 0x10);
    temp_f14 = M2C_FIELD(arg1, f32 *, 0x14);
    temp_f20 = (temp_f16 * temp_f16) + (temp_f18 * temp_f18) + (temp_f14 * temp_f14);
    if (temp_f20 < 1e-16f) {
        return 0.0f;
    }
    temp_f6 = M2C_FIELD(arg0, f32 *, 4) - M2C_FIELD(arg1, f32 *, 4);
    temp_f0 = M2C_FIELD(arg0, f32 *, 0) - M2C_FIELD(arg1, f32 *, 0);
    temp_f4 = M2C_FIELD(arg0, f32 *, 8) - M2C_FIELD(arg1, f32 *, 8);
    temp_f10 = (temp_f16 * temp_f6) - (temp_f18 * temp_f0);
    temp_f8 = (temp_f16 * temp_f4) - (temp_f14 * temp_f0);
    temp_f4_2 = (temp_f18 * temp_f4) - (temp_f14 * temp_f6);
    temp_f2 = (temp_f18 * temp_f10) + (temp_f14 * temp_f8);
    temp_f6_2 = (temp_f14 * temp_f4_2) - (temp_f16 * temp_f10);
    temp_f0_2 = (-temp_f16 * temp_f8) - (temp_f18 * temp_f4_2);
    temp_f12 = (temp_f2 * temp_f2) + (temp_f6_2 * temp_f6_2) + (temp_f0_2 * temp_f0_2);
    var_f0 = sqrtf(temp_f12);
    if (var_f0 != var_f0) {
        var_f0 = sqrtf(temp_f12, temp_f14);
    }
    return var_f0 / temp_f20;
}

f32 Vec3_Distance(void *arg0, void *arg1) {
    f32 temp_f0;
    f32 temp_f2;
    f32 temp_f4;

    temp_f2 = M2C_FIELD(arg1, f32 *, 0) - M2C_FIELD(arg0, f32 *, 0);
    temp_f4 = M2C_FIELD(arg1, f32 *, 4) - M2C_FIELD(arg0, f32 *, 4);
    temp_f0 = M2C_FIELD(arg1, f32 *, 8) - M2C_FIELD(arg0, f32 *, 8);
    return sqrtf((temp_f2 * temp_f2) + (temp_f4 * temp_f4) + (temp_f0 * temp_f0));
}

f32 Vec3_Length(void *arg0) {
    f32 temp_f0;
    f32 temp_f2;
    f32 temp_f4;

    temp_f4 = M2C_FIELD(arg0, f32 *, 0);
    temp_f2 = M2C_FIELD(arg0, f32 *, 4);
    temp_f0 = M2C_FIELD(arg0, f32 *, 8);
    return sqrtf((temp_f4 * temp_f4) + (temp_f2 * temp_f2) + (temp_f0 * temp_f0));
}

f32 Vec3_Dot(f32 *arg0, M2C_UNK *arg1) {
    return (M2C_FIELD(arg0, f32 *, 0) * M2C_FIELD(arg1, f32 *, 0)) + (M2C_FIELD(arg0, f32 *, 4) * M2C_FIELD(arg1, f32 *, 4)) + (M2C_FIELD(arg0, f32 *, 8) * M2C_FIELD(arg1, f32 *, 8));
}

f32 Vec3_CosAngle(f32 *arg0, M2C_UNK *arg1) {
    f32 temp_f10;
    f32 temp_f12;
    f32 temp_f14;
    f32 temp_f16;
    f32 temp_f18;
    f32 temp_f20;
    f32 temp_f8;
    f32 var_f6;

    temp_f20 = M2C_FIELD(arg0, f32 *, 0);
    temp_f18 = M2C_FIELD(arg0, f32 *, 4);
    temp_f16 = M2C_FIELD(arg1, f32 *, 0);
    temp_f14 = M2C_FIELD(arg1, f32 *, 4);
    temp_f10 = M2C_FIELD(arg0, f32 *, 8);
    temp_f8 = M2C_FIELD(arg1, f32 *, 8);
    temp_f12 = ((temp_f20 * temp_f20) + (temp_f18 * temp_f18) + (temp_f10 * temp_f10)) * ((temp_f16 * temp_f16) + (temp_f14 * temp_f14) + (temp_f8 * temp_f8));
    if (!(temp_f12 < 1e-16f)) {
        var_f6 = sqrtf(temp_f12);
        if (var_f6 != var_f6) {
            var_f6 = sqrtf(temp_f12, temp_f14);
        }
        return ((temp_f20 * temp_f16) + (temp_f18 * temp_f14) + (temp_f10 * temp_f8)) / var_f6;
    }
    return 0.0f;
}

s32 Vec3_TriangleNormal(M2C_UNK *arg0, f32 *arg1, f32 *arg2, f32 *arg3) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f10;
    f32 temp_f12;
    f32 temp_f12_2;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 temp_f8;

    temp_f2 = M2C_FIELD(arg2, f32 *, 4);
    temp_f12 = temp_f2 - M2C_FIELD(arg1, f32 *, 4);
    temp_f4 = M2C_FIELD(arg2, f32 *, 8);
    temp_f10 = M2C_FIELD(arg3, f32 *, 8) - temp_f4;
    temp_f4_2 = temp_f4 - M2C_FIELD(arg1, f32 *, 8);
    temp_f8 = M2C_FIELD(arg3, f32 *, 4) - temp_f2;
    temp_f0 = M2C_FIELD(arg2, f32 *, 0);
    temp_f2_2 = M2C_FIELD(arg3, f32 *, 0) - temp_f0;
    temp_f0_2 = temp_f0 - M2C_FIELD(arg1, f32 *, 0);
    temp_f24 = (temp_f12 * temp_f10) - (temp_f4_2 * temp_f8);
    temp_f22 = (temp_f4_2 * temp_f2_2) - (temp_f0_2 * temp_f10);
    temp_f20 = (temp_f0_2 * temp_f8) - (temp_f12 * temp_f2_2);
    temp_f12_2 = (temp_f24 * temp_f24) + (temp_f22 * temp_f22) + (temp_f20 * temp_f20);
    if (temp_f12_2 < 1e-16f) {
        return 0;
    }
    temp_f0_3 = 1.0f / sqrtf(temp_f12_2);
    M2C_FIELD(arg0, f32 *, 0) = (f32) (temp_f24 * temp_f0_3);
    M2C_FIELD(arg0, f32 *, 4) = (f32) (temp_f22 * temp_f0_3);
    M2C_FIELD(arg0, f32 *, 8) = (f32) (temp_f20 * temp_f0_3);
    return 1;
}

void Vec3_NormalizeXYZ(f32 *arg0, f32 *arg1, f32 *arg2) {
    f32 temp_f0;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;

    temp_f4 = *arg0;
    temp_f2 = *arg1;
    temp_f0 = *arg2;
    temp_f2_2 = 1.0f / sqrtf((temp_f4 * temp_f4) + (temp_f2 * temp_f2) + (temp_f0 * temp_f0));
    *arg0 *= temp_f2_2;
    *arg1 *= temp_f2_2;
    *arg2 *= temp_f2_2;
}

void Vec3_Normalize(M2C_UNK *arg0, void *arg1) {
    f32 temp_f0;
    f32 temp_f12;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;
    f32 var_f0;

    temp_f4 = M2C_FIELD(arg1, f32 *, 0);
    temp_f2 = M2C_FIELD(arg1, f32 *, 4);
    temp_f0 = M2C_FIELD(arg1, f32 *, 8);
    temp_f12 = (temp_f4 * temp_f4) + (temp_f2 * temp_f2) + (temp_f0 * temp_f0);
    var_f0 = sqrtf(temp_f12);
    if (var_f0 != var_f0) {
        var_f0 = sqrtf(temp_f12);
    }
    temp_f2_2 = 1.0f / var_f0;
    M2C_FIELD(arg0, f32 *, 0) = (f32) (M2C_FIELD(arg1, f32 *, 0) * temp_f2_2);
    M2C_FIELD(arg0, f32 *, 4) = (f32) (M2C_FIELD(arg1, f32 *, 4) * temp_f2_2);
    M2C_FIELD(arg0, f32 *, 8) = (f32) (M2C_FIELD(arg1, f32 *, 8) * temp_f2_2);
}

s32 Line2_FromPoints(void *arg0, void *arg1, void *arg2) {
    f32 temp_f12;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f6;

    temp_f22 = M2C_FIELD(arg2, f32 *, 0) - M2C_FIELD(arg1, f32 *, 0);
    temp_f20 = M2C_FIELD(arg2, f32 *, 4) - M2C_FIELD(arg1, f32 *, 4);
    temp_f12 = (temp_f22 * temp_f22) + (temp_f20 * temp_f20);
    if (temp_f12 < 1e-16f) {
        return 0;
    }
    temp_f6 = 1.0f / sqrtf(temp_f12);
    M2C_FIELD(arg0, f32 *, 0) = (f32) (-temp_f20 * temp_f6);
    M2C_FIELD(arg0, f32 *, 4) = (f32) (temp_f22 * temp_f6);
    M2C_FIELD(arg0, f32 *, 8) = (f32) (((M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg2, f32 *, 4)) - (M2C_FIELD(arg2, f32 *, 0) * M2C_FIELD(arg1, f32 *, 4))) * temp_f6);
    return 1;
}

s32 Line2_Intersect(void *arg0, void *arg1, void *arg2) {
    f32 temp_f10;
    f32 temp_f2;
    f32 temp_f6;
    f32 temp_f8;
    f32 var_f4;
    s32 var_v0;

    temp_f10 = M2C_FIELD(arg2, f32 *, 4);
    temp_f8 = M2C_FIELD(arg1, f32 *, 4);
    temp_f2 = (M2C_FIELD(arg1, f32 *, 0) * temp_f10) - (M2C_FIELD(arg2, f32 *, 0) * temp_f8);
    var_f4 = temp_f2;
    if (temp_f2 < 0.0f) {
        var_f4 = -temp_f2;
    }
    var_v0 = 0;
    if (!(var_f4 < 1e-16f)) {
        temp_f6 = 1.0f / temp_f2;
        M2C_FIELD(arg0, f32 *, 0) = (f32) (((temp_f8 * M2C_FIELD(arg2, f32 *, 8)) - (temp_f10 * M2C_FIELD(arg1, f32 *, 8))) * temp_f6);
        var_v0 = 1;
        M2C_FIELD(arg0, f32 *, 4) = (f32) (((M2C_FIELD(arg2, f32 *, 0) * M2C_FIELD(arg1, f32 *, 8)) - (M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg2, f32 *, 8))) * temp_f6);
    }
    return var_v0;
}

f32 Math_Cross2(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    return ((arg2 - arg0) * (arg5 - arg1)) - ((arg4 - arg0) * (arg3 - arg1));
}

s32 Ray2_IntersectCircle(void *arg0, void *arg1, void *arg2, f32 arg3) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f10;
    f32 temp_f12;
    f32 temp_f14;
    f32 temp_f16;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 temp_f6;
    f32 temp_f6_2;
    f32 temp_f8;

    temp_f8 = M2C_FIELD(arg1, f32 *, 8);
    temp_f6 = M2C_FIELD(arg1, f32 *, 0xC);
    temp_f22 = (temp_f8 * temp_f8) + (temp_f6 * temp_f6);
    if (!(temp_f22 < 1e-16f)) {
        temp_f14 = M2C_FIELD(arg2, f32 *, 4) - M2C_FIELD(arg1, f32 *, 4);
        temp_f16 = M2C_FIELD(arg1, f32 *, 0);
        temp_f10 = M2C_FIELD(arg2, f32 *, 0) - temp_f16;
        temp_f0 = (temp_f8 * temp_f14) - (temp_f6 * temp_f10);
        temp_f12 = (arg3 * arg3 * temp_f22) - (temp_f0 * temp_f0);
        if (!(temp_f12 < -1e-16f)) {
            temp_f20 = (temp_f8 * temp_f10) + (temp_f6 * temp_f14);
            if (temp_f12 < 1e-16f) {
                temp_f2 = temp_f20 / temp_f22;
                temp_f0_2 = temp_f16 + (temp_f8 * temp_f2);
                M2C_FIELD(arg0, f32 *, 8) = temp_f0_2;
                M2C_FIELD(arg0, f32 *, 0) = temp_f0_2;
                temp_f2_2 = M2C_FIELD(arg1, f32 *, 4) + (M2C_FIELD(arg1, f32 *, 0xC) * temp_f2);
                M2C_FIELD(arg0, f32 *, 0xC) = temp_f2_2;
                M2C_FIELD(arg0, f32 *, 4) = temp_f2_2;
                return 1;
            }
            temp_f0_3 = sqrtf(temp_f12);
            temp_f6_2 = 1.0f / temp_f22;
            temp_f4 = (temp_f20 - temp_f0_3) * temp_f6_2;
            M2C_FIELD(arg0, f32 *, 0) = (f32) (M2C_FIELD(arg1, f32 *, 0) + (M2C_FIELD(arg1, f32 *, 8) * temp_f4));
            temp_f4_2 = (temp_f20 + temp_f0_3) * temp_f6_2;
            M2C_FIELD(arg0, f32 *, 4) = (f32) (M2C_FIELD(arg1, f32 *, 4) + (M2C_FIELD(arg1, f32 *, 0xC) * temp_f4));
            M2C_FIELD(arg0, f32 *, 8) = (f32) (M2C_FIELD(arg1, f32 *, 0) + (M2C_FIELD(arg1, f32 *, 8) * temp_f4_2));
            M2C_FIELD(arg0, f32 *, 0xC) = (f32) (M2C_FIELD(arg1, f32 *, 4) + (M2C_FIELD(arg1, f32 *, 0xC) * temp_f4_2));
            return 2;
        }
        goto block_4;
    }
block_4:
    return 0;
}

s32 Ray2_PerpendicularPoints(void *arg0, void *arg1, f32 arg2) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;
    f32 temp_f6;
    f32 var_f0;

    temp_f22 = M2C_FIELD(arg1, f32 *, 0xC);
    temp_f24 = -M2C_FIELD(arg1, f32 *, 8);
    temp_f20 = (temp_f22 * temp_f22) + (temp_f24 * temp_f24);
    if (temp_f20 < 1e-16f) {
        temp_f0 = M2C_FIELD(arg1, f32 *, 0);
        M2C_FIELD(arg0, f32 *, 8) = temp_f0;
        M2C_FIELD(arg0, f32 *, 0) = temp_f0;
        temp_f0_2 = M2C_FIELD(arg1, f32 *, 4);
        M2C_FIELD(arg0, f32 *, 0xC) = temp_f0_2;
        M2C_FIELD(arg0, f32 *, 4) = temp_f0_2;
        return 0;
    }
    var_f0 = sqrtf(temp_f20);
    if (var_f0 != var_f0) {
        var_f0 = sqrtf(temp_f20);
    }
    temp_f4 = 1.0f / temp_f20;
    temp_f2 = arg2 * var_f0;
    temp_f6 = temp_f2 * temp_f4;
    temp_f2_2 = -temp_f2 * temp_f4;
    M2C_FIELD(arg0, f32 *, 0) = (f32) (M2C_FIELD(arg1, f32 *, 0) + (temp_f22 * temp_f6));
    M2C_FIELD(arg0, f32 *, 4) = (f32) (M2C_FIELD(arg1, f32 *, 4) + (temp_f24 * temp_f6));
    M2C_FIELD(arg0, f32 *, 8) = (f32) (M2C_FIELD(arg1, f32 *, 0) + (temp_f22 * temp_f2_2));
    M2C_FIELD(arg0, f32 *, 0xC) = (f32) (M2C_FIELD(arg1, f32 *, 4) + (temp_f24 * temp_f2_2));
    return 2;
}

s32 Segment2_Offset(void *arg0, void *arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6) {
    f32 temp_f0;
    f32 temp_f12;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f2;
    f32 temp_f4;
    f32 var_f8;

    temp_f22 = arg4 - arg2;
    temp_f20 = arg5 - arg3;
    temp_f12 = 1.0f / ((temp_f22 * temp_f22) + (temp_f20 * temp_f20));
    var_f8 = sqrtf(temp_f12);
    if (var_f8 != var_f8) {
        var_f8 = sqrtf(temp_f12);
    }
    temp_f0 = -arg6;
    M2C_FIELD(arg0, f32 *, 0) = (f32) ((arg6 * temp_f20 * var_f8) + arg2);
    M2C_FIELD(arg0, f32 *, 4) = (f32) ((temp_f0 * temp_f22 * var_f8) + arg3);
    M2C_FIELD(arg1, f32 *, 0) = (f32) ((temp_f0 * temp_f20 * var_f8) + arg2);
    M2C_FIELD(arg1, f32 *, 4) = (f32) ((arg6 * temp_f22 * var_f8) + arg3);
    temp_f4 = M2C_FIELD(arg0, f32 *, 0);
    if (((temp_f22 * (M2C_FIELD(arg0, f32 *, 4) - arg3)) - ((temp_f4 - arg2) * temp_f20)) < 0.0f) {
        M2C_FIELD(arg0, f32 *, 0) = (f32) M2C_FIELD(arg1, f32 *, 0);
        M2C_FIELD(arg1, f32 *, 0) = temp_f4;
        temp_f2 = M2C_FIELD(arg0, f32 *, 4);
        M2C_FIELD(arg0, f32 *, 4) = (f32) M2C_FIELD(arg1, f32 *, 4);
        M2C_FIELD(arg1, f32 *, 4) = temp_f2;
    }
    return 1;
}

f32 Ray2_DistanceSqToPoint(void *arg0, f32 arg1, f32 arg2) {
    f32 temp_f0;
    f32 temp_f12;
    f32 temp_f14;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 temp_f6;
    f32 temp_f8;
    f32 temp_f8_2;

    temp_f8 = M2C_FIELD(arg0, f32 *, 8);
    temp_f14 = temp_f8 * temp_f8;
    temp_f2_2 = M2C_FIELD(arg0, f32 *, 0xC);
    temp_f4_2 = temp_f2_2 * temp_f2_2;
    temp_f12 = temp_f14 + temp_f4_2;
    if (!(temp_f12 < 1e-16f)) {
        temp_f8_2 = temp_f8 * temp_f2_2;
        temp_f6 = arg1 - M2C_FIELD(arg0, f32 *, 0);
        temp_f2_3 = arg2 - M2C_FIELD(arg0, f32 *, 4);
        temp_f4 = 1.0f / temp_f12;
        temp_f0 = (temp_f4_2 * temp_f6) - (temp_f8_2 * temp_f2_3);
        temp_f2 = (temp_f14 * temp_f2_3) - (temp_f8_2 * temp_f6);
        return ((temp_f0 * temp_f0) + (temp_f2 * temp_f2)) * (temp_f4 * temp_f4);
    }
    return 0.0f;
}

f32 Vec2_CosAngle(void *arg0, void *arg1) {
    f32 temp_f10;
    f32 temp_f12;
    f32 temp_f14;
    f32 temp_f2;
    f32 temp_f4;
    f32 temp_f8;
    f32 var_f0;

    temp_f12 = M2C_FIELD(arg0, f32 *, 0);
    temp_f8 = M2C_FIELD(arg0, f32 *, 4);
    temp_f14 = temp_f8 * temp_f8;
    temp_f4 = M2C_FIELD(arg1, f32 *, 0);
    temp_f2 = M2C_FIELD(arg1, f32 *, 4);
    temp_f10 = ((temp_f12 * temp_f12) + temp_f14) * ((temp_f4 * temp_f4) + (temp_f2 * temp_f2));
    var_f0 = sqrtf(temp_f10);
    if (var_f0 != var_f0) {
        var_f0 = sqrtf(temp_f10, temp_f14);
    }
    return ((temp_f12 * temp_f4) + (temp_f8 * temp_f2)) / var_f0;
}

void Line2_FromPointNormal(void *arg0, void *arg1, void *arg2) {
    M2C_FIELD(arg0, f32 *, 0) = (f32) M2C_FIELD(arg2, f32 *, 0);
    M2C_FIELD(arg0, f32 *, 4) = (f32) M2C_FIELD(arg2, f32 *, 4);
    M2C_FIELD(arg0, f32 *, 8) = (f32) -((M2C_FIELD(arg2, f32 *, 0) * M2C_FIELD(arg1, f32 *, 0)) + (M2C_FIELD(arg2, f32 *, 4) * M2C_FIELD(arg1, f32 *, 4)));
}

s32 func_80002200(void *arg0, void *arg1, void *arg2) {
    s32 temp_a0;
    s32 temp_t0;

    temp_t0 = M2C_FIELD(arg0, s32 *, 0);
    temp_a0 = M2C_FIELD(arg0, s32 *, 4);
    return ((M2C_FIELD(arg1, s32 *, 0) - temp_t0) * (M2C_FIELD(arg2, s32 *, 4) - temp_a0)) - ((M2C_FIELD(arg2, s32 *, 0) - temp_t0) * (M2C_FIELD(arg1, s32 *, 4) - temp_a0));
}

u32 func_80002248(void *arg0, void *arg1, void *arg2, void *arg3) {
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_a0_3;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_a1_3;
    s32 temp_a2;
    s32 temp_a2_2;
    s32 temp_s0;
    s32 temp_s1;
    s32 temp_t0;
    s32 temp_t1;
    s32 temp_t2;
    s32 temp_t3;
    s32 temp_t4;
    s32 temp_t5;
    s32 temp_t6;
    s32 temp_v0;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_3;
    s32 var_a0;
    s32 var_a1;
    s32 var_t0;
    s32 var_t1;
    s32 var_v0_2;
    u32 var_v0;

    temp_s1 = M2C_FIELD(arg1, s32 *, 0);
    temp_t5 = M2C_FIELD(arg0, s32 *, 0);
    temp_t6 = M2C_FIELD(arg2, s32 *, 4);
    temp_t4 = M2C_FIELD(arg0, s32 *, 4);
    temp_t1 = temp_s1 - temp_t5;
    temp_t3 = M2C_FIELD(arg2, s32 *, 0);
    temp_s0 = M2C_FIELD(arg1, s32 *, 4);
    temp_t0 = temp_s0 - temp_t4;
    temp_a2 = M2C_FIELD(arg3, s32 *, 4);
    temp_t2 = M2C_FIELD(arg3, s32 *, 0);
    temp_a1_2 = (temp_t1 * (temp_t6 - temp_t4)) - ((temp_t3 - temp_t5) * temp_t0);
    temp_a0_2 = (temp_t1 * (temp_a2 - temp_t4)) - ((temp_t2 - temp_t5) * temp_t0);
    if ((temp_a1_2 | temp_a0_2) == 0) {
        temp_v1 = temp_t2 - temp_t3;
        temp_v0 = temp_a2 - temp_t6;
        temp_a1_3 = (((temp_t1 * temp_t1) + (temp_v1 * temp_v1)) < ((temp_t0 * temp_t0) + (temp_v0 * temp_v0))) * 4;
        var_t0 = *(temp_a1_3 + arg0);
        var_t1 = *(temp_a1_3 + arg1);
        temp_v1_2 = var_t0;
        if (var_t1 < var_t0) {
            var_t0 = var_t1;
            var_t1 = temp_v1_2;
        }
        var_a0 = *(temp_a1_3 + arg2);
        var_a1 = *(temp_a1_3 + arg3);
        temp_v1_3 = var_a0;
        if (var_a1 < var_a0) {
            var_a0 = var_a1;
            var_a1 = temp_v1_3;
        }
        var_v0 = 2;
        if (((var_t0 == var_a0) | (var_t1 == var_a1)) == 0) {
            var_v0_2 = var_t0 < var_a1;
            if (var_a0 >= var_t0) {
                var_v0_2 = var_a0 < var_t1;
            }
            if (var_v0_2 != 0) {
                return 2U;
            }
            goto block_14;
        }
        /* Duplicate return node #15. Try simplifying control flow for better match */
        return var_v0;
    }
    var_v0 = 0;
    if (((temp_a1_2 == 0) | (temp_a0_2 == 0)) == 0) {
        var_v0 = 0;
        if ((temp_a1_2 ^ temp_a0_2) < 0) {
            temp_a0_3 = temp_t2 - temp_t3;
            temp_a2_2 = temp_a2 - temp_t6;
            temp_a1 = (temp_a0_3 * (temp_t4 - temp_t6)) - ((temp_t5 - temp_t3) * temp_a2_2);
            temp_a0 = (temp_a0_3 * (temp_s0 - temp_t6)) - ((temp_s1 - temp_t3) * temp_a2_2);
            if (((temp_a1 == 0) | (temp_a0 == 0)) == 0) {
                return (u32) (temp_a1 ^ temp_a0) >> 0x1F;
            }
block_14:
            var_v0 = 0;
            /* Duplicate return node #15. Try simplifying control flow for better match */
            return var_v0;
        }
    }
    return var_v0;
}

s32 func_80002434(void *arg0, void *arg1) {
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_a2;
    s32 temp_a3;
    s32 temp_a3_2;
    s32 temp_t0;
    s32 temp_t1;
    s32 temp_t2;
    s32 temp_t3;
    s32 temp_t5;
    s32 temp_t6;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v1;
    s32 temp_v1_2;

    temp_t3 = M2C_FIELD(arg1, s32 *, 0x10);
    temp_v0_2 = M2C_FIELD(arg1, s32 *, 4);
    temp_t6 = M2C_FIELD(arg1, s32 *, 8);
    temp_t1 = temp_t3 - temp_v0_2;
    temp_t0 = M2C_FIELD(arg1, s32 *, 0x20) - temp_t6;
    temp_t2 = M2C_FIELD(arg1, s32 *, 0x14);
    temp_a3_2 = temp_t2 - temp_t6;
    temp_a2 = M2C_FIELD(arg1, s32 *, 0x1C) - temp_v0_2;
    temp_v1 = M2C_FIELD(arg1, s32 *, 0);
    temp_a0_2 = M2C_FIELD(arg1, s32 *, 0x18) - temp_v1;
    temp_v0 = M2C_FIELD(arg1, s32 *, 0xC);
    temp_v1_2 = temp_v0 - temp_v1;
    temp_t5 = (temp_t1 * temp_t0) - (temp_a3_2 * temp_a2);
    temp_a3 = (temp_a3_2 * temp_a0_2) - (temp_v1_2 * temp_t0);
    temp_a0 = (temp_v1_2 * temp_a2) - (temp_t1 * temp_a0_2);
    if (temp_a0 == 0) {
        return temp_t6;
    }
    return (s32) -((temp_t5 * M2C_FIELD(arg0, s32 *, 0)) + (temp_a3 * M2C_FIELD(arg0, s32 *, 4)) + -((temp_v0 * temp_t5) + (temp_t3 * temp_a3) + (temp_t2 * temp_a0))) / temp_a0;
}

s32 func_80002558(s32 *arg0, void *arg1, s32 *arg2, s32 arg3, s32 arg4) {
    s32 *var_a0;
    s32 *var_a2;
    s32 *var_a2_2;
    s32 *var_a3;
    s32 *var_t0_2;
    s32 *var_t1;
    s32 *var_t1_2;
    s32 *var_t3;
    s32 *var_t4;
    s32 temp_a0_2;
    s32 temp_a0_5;
    s32 temp_a0_6;
    s32 temp_a1_2;
    s32 temp_a1_3;
    s32 temp_a1_4;
    s32 temp_a2;
    s32 temp_a2_2;
    s32 temp_a2_3;
    s32 temp_a3;
    s32 temp_a3_2;
    s32 temp_a3_3;
    s32 temp_t1;
    s32 temp_t6;
    s32 temp_t7;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 temp_v1;
    s32 var_a1;
    s32 var_a1_2;
    s32 var_t0;
    s32 var_t1_3;
    s32 var_t3_2;
    s32 var_v0;
    s32 var_v0_2;
    void *temp_a0;
    void *temp_a0_3;
    void *temp_a0_4;
    void *temp_a1;
    void *temp_v0;
    void *temp_v0_2;
    void *temp_v0_3;

    if (arg4 > 0) {
        var_t3 = arg2;
loop_2:
        temp_a0 = (M2C_FIELD(var_t3, s32 *, 0) * 0x14) + arg3;
        temp_a1 = (M2C_FIELD(var_t3, s32 *, 4) * 0x14) + arg3;
        temp_v0 = (M2C_FIELD(var_t3, s32 *, 8) * 0x14) + arg3;
        temp_a3 = M2C_FIELD(temp_a0, s32 *, 0);
        temp_a0_2 = M2C_FIELD(temp_a0, s32 *, 4);
        temp_a1_2 = ((M2C_FIELD(temp_a1, s32 *, 0) - temp_a3) * (M2C_FIELD(temp_v0, s32 *, 4) - temp_a0_2)) - ((M2C_FIELD(temp_v0, s32 *, 0) - temp_a3) * (M2C_FIELD(temp_a1, s32 *, 4) - temp_a0_2));
        if (temp_a1_2 == 0) {
            var_t3 += 0xC;
            goto block_33;
        }
        var_t0 = 0;
        if (temp_a1_2 > 0) {
            var_t1 = var_t3;
loop_7:
            var_v0 = 0;
            if (var_t0 != 2) {
                var_v0 = var_t0 + 1;
            }
            temp_a3_2 = M2C_FIELD(arg1, s32 *, 0);
            temp_a2 = M2C_FIELD(arg1, s32 *, 4);
            temp_a0_3 = (*var_t1 * 0x14) + arg3;
            temp_v0_2 = (*((var_v0 * 4) + var_t3) * 0x14) + arg3;
            if ((((M2C_FIELD(temp_a0_3, s32 *, 0) - temp_a3_2) * (M2C_FIELD(temp_v0_2, s32 *, 4) - temp_a2)) - ((M2C_FIELD(temp_v0_2, s32 *, 0) - temp_a3_2) * (M2C_FIELD(temp_a0_3, s32 *, 4) - temp_a2))) >= 0) {
                var_t0 += 1;
                var_t1 += 4;
                if (var_t0 >= 3) {

                } else {
                    goto loop_7;
                }
            }
        } else {
            var_t1_2 = var_t3;
loop_13:
            var_v0_2 = 0;
            if (var_t0 != 2) {
                var_v0_2 = var_t0 + 1;
            }
            temp_a3_3 = M2C_FIELD(arg1, s32 *, 0);
            temp_a2_2 = M2C_FIELD(arg1, s32 *, 4);
            temp_a0_4 = (*var_t1_2 * 0x14) + arg3;
            temp_v0_3 = (*((var_v0_2 * 4) + var_t3) * 0x14) + arg3;
            if ((((M2C_FIELD(temp_a0_4, s32 *, 0) - temp_a3_3) * (M2C_FIELD(temp_v0_3, s32 *, 4) - temp_a2_2)) - ((M2C_FIELD(temp_v0_3, s32 *, 0) - temp_a3_3) * (M2C_FIELD(temp_a0_4, s32 *, 4) - temp_a2_2))) <= 0) {
                var_t0 += 1;
                var_t1_2 += 4;
                if (var_t0 >= 3) {

                } else {
                    goto loop_13;
                }
            }
        }
        if (var_t0 != 3) {
            var_t3 += 0xC;
block_33:
            if ((s32) var_t3 >= (s32) ((arg4 * 0xC) + arg2)) {
                goto block_34;
            }
            goto loop_2;
        }
        var_t1_3 = 0;
        var_t0_2 = var_t3;
        var_a3 = sp;
        do {
            var_a1 = 0;
            var_a2 = var_a3;
loop_23:
            temp_v0_4 = *((var_a1 * 4) + ((*var_t0_2 * 0x14) + arg3));
            var_a1 += 1;
            *var_a2 = temp_v0_4;
            var_a2 += 4;
            if (var_a1 < 2) {
                goto loop_23;
            }
            var_t0_2 += 4;
            var_t1_3 += 1;
            var_a3 += 0xC;
        } while (var_t1_3 < 3);
        temp_t7 = spC - sp0;
        temp_t6 = sp18 - sp0;
        var_t4 = arg0;
        var_t3_2 = 0;
        do {
            var_a1_2 = 0;
            var_a2_2 = var_t3;
            var_a0 = sp;
loop_27:
            temp_v0_5 = M2C_FIELD((var_t3_2 + ((*var_a2_2 * 0x14) + arg3)), s32 *, 8);
            var_a2_2 += 4;
            var_a1_2 += 1;
            M2C_FIELD(var_a0, s32 *, 8) = temp_v0_5;
            var_a0 += 0xC;
            if (var_a1_2 < 3) {
                goto loop_27;
            }
            temp_a2_3 = sp10 - sp4;
            temp_a0_5 = sp20 - sp8;
            temp_a1_3 = sp14 - sp8;
            temp_v1 = sp1C - sp4;
            temp_t1 = (temp_a2_3 * temp_a0_5) - (temp_a1_3 * temp_v1);
            temp_a1_4 = (temp_a1_3 * temp_t6) - (temp_t7 * temp_a0_5);
            temp_a0_6 = (temp_t7 * temp_v1) - (temp_a2_3 * temp_t6);
            if (temp_a0_6 != 0) {
                *var_t4 = (s32) -((temp_t1 * M2C_FIELD(arg1, s32 *, 0)) + (temp_a1_4 * M2C_FIELD(arg1, s32 *, 4)) + -((spC * temp_t1) + (sp10 * temp_a1_4) + (sp14 * temp_a0_6))) / temp_a0_6;
            } else {
                *var_t4 = sp8;
            }
            var_t3_2 += 4;
            var_t4 += 4;
        } while (var_t3_2 < 0xC);
        return 1;
    }
block_34:
    return 0;
}

s32 func_80002990(s32 arg0, void *arg1, s32 arg2) {
    s32 sp14;
    M2C_UNK *sp1C;
    s32 sp24;
    s32 sp2C;
    s32 sp34;
    M2C_UNK *sp3C;
    void *sp68;
    s32 sp6C;
    void *sp70;
    s32 sp74;
    s32 sp78;
    M2C_UNK *sp7C;
    s32 sp80;
    s32 sp84;
    s32 sp8C;
    M2C_UNK *temp_s5;
    M2C_UNK *temp_t2;
    M2C_UNK *temp_t3;
    M2C_UNK *temp_t6;
    M2C_UNK *var_a1;
    M2C_UNK *var_a3;
    M2C_UNK *var_s0;
    M2C_UNK *var_s1_2;
    M2C_UNK *var_s2_2;
    M2C_UNK *var_s4_2;
    M2C_UNK *var_s7;
    M2C_UNK *var_t0_2;
    M2C_UNK *var_t2_2;
    M2C_UNK *var_t3_2;
    M2C_UNK *var_t5;
    M2C_UNK *var_v1;
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_s3;
    s32 temp_s3_2;
    s32 temp_s7;
    s32 temp_t4;
    s32 temp_v0;
    s32 var_a2;
    s32 var_s1;
    s32 var_s1_3;
    s32 var_s2;
    s32 var_s4;
    s32 var_s5;
    s32 var_t1;
    s32 var_t3;
    s32 var_t4;
    s32 var_t6;
    s32 var_t6_2;
    s32 var_t6_3;
    s32 var_t8;
    u32 temp_v0_2;
    void *temp_s0;
    void *temp_t1;
    void *temp_t5;
    void *temp_t7;
    void *temp_v0_3;
    void *temp_v1;
    void *var_s3;
    void *var_t0;
    void *var_t2;

    sp14 = arg0;
    sp24 = 0;
    sp6C = 0;
    sp84 = arg2;
    var_t8 = arg2;
    sp3C = Heap_Alloc(&gGameHeap, ((arg2 * 3) - 6) * 8);
    var_t5 = sp3C;
    var_t6 = var_t8 - 1;
    var_t1 = 0;
loop_1:
    var_t3 = 0x7FFFFFFF;
    var_s5 = 0;
    if (var_t6 > 0) {
        var_t2 = arg1;
        do {
            var_s2 = var_s5 + 1;
            if (var_s2 < var_t8) {
                var_t4 = var_s5 * 8;
                var_t0 = var_t2;
                var_s4 = var_s2 * 8;
                var_s3 = var_s4 + arg1;
                do {
                    temp_a0 = M2C_FIELD(var_s3, s32 *, 0) - M2C_FIELD(var_t0, s32 *, 0);
                    temp_v0 = M2C_FIELD(var_s3, s32 *, 4) - M2C_FIELD(var_t0, s32 *, 4);
                    temp_s7 = (temp_a0 * temp_a0) + (temp_v0 * temp_v0);
                    if (temp_s7 >= var_t3) {
                        var_s4 += 8;
                    } else {
                        var_s1 = 0;
                        if (var_t1 > 0) {
                            var_s0 = sp3C;
loop_10:
                            sp68 = var_t0;
                            sp6C = var_t1;
                            sp70 = var_t2;
                            sp74 = var_t3;
                            sp78 = var_t4;
                            sp7C = var_t5;
                            sp80 = var_t6;
                            sp84 = var_t8;
                            temp_v0_2 = func_80002248(arg1 + var_t4, arg1 + var_s4, arg1 + ((s32) M2C_FIELD(var_s0, M2C_UNK **, 0) * 8), arg1 + (var_s0[1] * 8));
                            if (temp_v0_2 == 0) {
                                var_s1 += 1;
                                var_s0 += 8;
                                if (var_s1 < var_t1) {
                                    goto loop_10;
                                }
                            }
                        }
                        var_s4 += 8;
                        if (var_s1 == var_t1) {
                            var_t3 = temp_s7;
                            sp2C = var_s5;
                            sp34 = var_s2;
                        }
                    }
                    var_s2 += 1;
                    var_s3 += 8;
                } while (var_s2 < var_t8);
            }
            var_s5 += 1;
            var_t2 += 8;
        } while (var_s5 < var_t6);
    }
    if (var_t3 != 0x7FFFFFFF) {
        M2C_FIELD(var_t5, M2C_UNK **, 0) = (M2C_UNK *) sp2C;
        var_t1 += 1;
        var_t5[1] = sp34;
        var_t5 += 8;
        goto loop_1;
    }
    sp6C = var_t1;
    sp84 = var_t8;
    sp1C = Heap_Alloc(&gGameHeap, var_t1 * 0x18);
    var_s1_2 = sp1C;
    var_t6_2 = 0;
    temp_s5 = Heap_Alloc(&gGameHeap, var_t8 * 0x18);
    if (var_t8 > 0) {
        var_v1 = temp_s5;
        do {
            var_v1[1] = NULL;
            var_v1[3] = NULL;
            M2C_FIELD(var_v1, M2C_UNK **, 0) = var_v1 + 0xC;
            var_v1[4] = var_v1;
            var_t6_2 += 1;
            var_v1 += 0x18;
        } while (var_t6_2 < var_t8);
    }
    var_a2 = 0;
    if (var_t1 > 0) {
        var_a1 = sp3C;
        do {
            temp_t6 = M2C_FIELD(var_a1, M2C_UNK **, 0);
            temp_s3 = var_a1[1];
            temp_t3 = var_s1_2;
            temp_v0_3 = ((s32) (((s32) temp_t6 * 2) + temp_t6) * 8) + temp_s5;
            temp_t3[2] = temp_s3;
            temp_t3[5] = temp_t6;
            *M2C_FIELD(temp_v0_3, M2C_UNK **, 0x10) = temp_t3;
            temp_t2 = temp_t3 + 0xC;
            M2C_FIELD(temp_t3, M2C_UNK **, 0) = temp_v0_3 + 0xC;
            temp_v1 = (temp_s3 * 0x18) + temp_s5;
            temp_t3[1] = M2C_FIELD(temp_v0_3, M2C_UNK **, 0x10);
            M2C_FIELD(temp_v0_3, M2C_UNK **, 0x10) = temp_t3;
            var_s1_2 = temp_t3 + 0x18;
            var_a1 += 8;
            *M2C_FIELD(temp_v1, M2C_UNK **, 0x10) = temp_t2;
            var_a2 += 1;
            temp_t3[3] = temp_v1 + 0xC;
            temp_t3[4] = M2C_FIELD(temp_v1, M2C_UNK **, 0x10);
            M2C_FIELD(temp_v1, M2C_UNK **, 0x10) = temp_t2;
        } while (var_a2 < var_t1);
    }
    var_t6_3 = 0;
    if (var_t8 > 0) {
        var_s7 = temp_s5;
        do {
            var_t3_2 = *var_s7;
            var_s4_2 = *var_t3_2;
            if (var_s4_2 != NULL) {
                sp8C = var_t6_3 * 8;
                do {
                    temp_s3_2 = var_t3_2[2];
                    var_t2_2 = *((temp_s3_2 * 0x18) + temp_s5);
                    var_s2_2 = *var_t2_2;
                    var_s1_3 = 0;
                    if (var_s2_2 != NULL) {
loop_30:
                        temp_t4 = M2C_FIELD(var_t2_2, s32 *, 8);
                        var_a3 = *((temp_t4 * 0x18) + temp_s5);
                        var_t0_2 = *var_a3;
                        if (var_t0_2 != NULL) {
                            temp_s0 = sp8C + arg1;
                            temp_t5 = (temp_t4 * 8) + arg1;
                            temp_t7 = (temp_s3_2 * 8) + arg1;
                            temp_t1 = (sp24 * 0xC) + sp14;
loop_32:
                            if (M2C_FIELD(var_a3, s32 *, 8) == var_t6_3) {
                                var_a2 = M2C_FIELD(temp_s0, s32 *, 0);
                                temp_a1 = M2C_FIELD(temp_s0, s32 *, 4);
                                if ((((M2C_FIELD(temp_t7, s32 *, 0) - var_a2) * (M2C_FIELD(temp_t5, s32 *, 4) - temp_a1)) - ((M2C_FIELD(temp_t5, s32 *, 0) - var_a2) * (M2C_FIELD(temp_t7, s32 *, 4) - temp_a1))) >= 0) {
                                    M2C_FIELD(temp_t1, s32 *, 0) = var_t6_3;
                                    M2C_FIELD(temp_t1, s32 *, 4) = temp_s3_2;
                                    M2C_FIELD(temp_t1, s32 *, 8) = temp_t4;
                                    *var_t3_2[1] = M2C_FIELD(var_t3_2, M2C_UNK **, 0);
                                    M2C_FIELD(var_t3_2, M2C_UNK **, 0)[1] = var_t3_2[1];
                                    *M2C_FIELD(var_t2_2, void ***, 4) = M2C_FIELD(var_t2_2, M2C_UNK **, 0);
                                    M2C_FIELD(M2C_FIELD(var_t2_2, M2C_UNK **, 0), M2C_UNK ***, 4) = (M2C_UNK **) M2C_FIELD(var_t2_2, M2C_UNK ***, 4);
                                    *M2C_FIELD(var_a3, void ***, 4) = M2C_FIELD(var_a3, M2C_UNK **, 0);
                                    var_s1_3 = 1;
                                    sp24 += 1;
                                    M2C_FIELD(M2C_FIELD(var_a3, M2C_UNK **, 0), M2C_UNK ***, 4) = (M2C_UNK **) M2C_FIELD(var_a3, M2C_UNK ***, 4);
                                    goto block_37;
                                }
                            }
                            if (var_s1_3 == 0) {
                                var_a3 = var_t0_2;
                                var_t0_2 = *var_t0_2;
                                if (var_t0_2 == NULL) {
                                    goto block_37;
                                }
                                goto loop_32;
                            }
                        } else {
block_37:
                            if (var_s1_3 == 0) {
                                var_t2_2 = var_s2_2;
                                var_s2_2 = *var_s2_2;
                                if (var_s2_2 != NULL) {
                                    goto loop_30;
                                }
                            }
                        }
                    }
                    var_t3_2 = var_s4_2;
                    var_s4_2 = *var_s4_2;
                } while (var_s4_2 != NULL);
            }
            var_t6_3 += 1;
            var_s7 += 0x18;
        } while (var_t6_3 < var_t8);
    }
    Heap_Free(&gGameHeap, temp_s5, var_a2);
    Heap_Free(&gGameHeap, sp1C);
    Heap_Free(&gGameHeap, sp3C);
    return sp24;
}

s32 func_80002E70(void *arg0, void *arg1, f32 *arg2) {
    f32 sp10;
    f32 temp_f16;
    f32 temp_f4;
    f32 temp_f6;
    f32 temp_f8;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v1;

    Plane_FromPoints(&sp10, arg2, arg2 + 0xC, arg2 + 0x18);
    temp_f16 = M2C_FIELD(arg1, f32 *, 0xC);
    temp_f8 = (sp10 * temp_f16) + (sp14 * M2C_FIELD(arg1, f32 *, 0x10)) + (sp18 * M2C_FIELD(arg1, f32 *, 0x14));
    var_v0_2 = 1;
    if (!(temp_f8 > -1e-16f)) {
        var_v0_2 = 0;
    }
    var_v1 = 1;
    if (!(temp_f8 < 1e-16f)) {
        var_v1 = 0;
    }
    var_v0 = 0;
    if (!(var_v0_2 & var_v1)) {
        temp_f6 = M2C_FIELD(arg1, f32 *, 0);
        temp_f4 = -((sp10 * temp_f6) + (sp14 * M2C_FIELD(arg1, f32 *, 4)) + (sp18 * M2C_FIELD(arg1, f32 *, 8)) + sp1C) / temp_f8;
        M2C_FIELD(arg0, f32 *, 0) = (f32) (temp_f6 + (temp_f16 * temp_f4));
        M2C_FIELD(arg0, f32 *, 4) = (f32) (M2C_FIELD(arg1, f32 *, 4) + (M2C_FIELD(arg1, f32 *, 0x10) * temp_f4));
        var_v0 = 1;
        M2C_FIELD(arg0, f32 *, 8) = (f32) (M2C_FIELD(arg1, f32 *, 8) + (M2C_FIELD(arg1, f32 *, 0x14) * temp_f4));
    }
    return var_v0;
}

s32 func_80002FA0(f32 *arg0, void *arg1, f32 *arg2) {
    f32 sp10;
    f32 *var_a0;
    f32 *var_a0_2;
    f32 *var_a0_3;
    f32 *var_a1;
    f32 *var_a1_2;
    f32 *var_a2;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    s32 var_v0;
    s32 var_v1;
    s32 var_v1_2;
    s32 var_v1_3;

    var_v0 = 1;
    if (func_8000306C() == 0) {
        var_v1 = 0;
        var_a1 = &sp10;
        var_a0 = arg2;
        do {
            temp_f0 = M2C_FIELD(var_a0, f32 *, 0x18);
            var_a0 += 4;
            var_v1 += 1;
            *var_a1 = temp_f0;
            var_a1 += 4;
        } while (var_v1 < 3);
        var_v1_2 = 0;
        var_a1_2 = arg2;
        var_a0_2 = &sp10;
        do {
            temp_f0_2 = M2C_FIELD(var_a1_2, f32 *, 0x24);
            var_a1_2 += 4;
            var_v1_2 += 1;
            M2C_FIELD(var_a0_2, f32 *, 0xC) = temp_f0_2;
            var_a0_2 += 4;
        } while (var_v1_2 < 3);
        var_v1_3 = 0;
        var_a2 = arg2;
        var_a0_3 = &sp10;
        do {
            temp_f0_3 = *var_a2;
            var_a2 += 4;
            var_v1_3 += 1;
            M2C_FIELD(var_a0_3, f32 *, 0x18) = temp_f0_3;
            var_a0_3 += 4;
        } while (var_v1_3 < 3);
        var_v0 = func_8000306C(arg0, arg1, &sp10);
    }
    return var_v0;
}

s32 func_8000306C(f32 *arg0, void *arg1, f32 *arg2) {
    f32 sp10;
    M2C_UNK sp20;
    f32 sp30;
    M2C_UNK sp40;
    f32 sp50;
    f32 *temp_s0;
    f32 *temp_s2;
    f32 *var_a0;
    f32 *var_a1;
    f32 *var_a2;
    f32 *var_v1_3;
    f32 temp_f16;
    f32 temp_f2;
    f32 temp_f4;
    f32 temp_f6;
    f32 temp_f8;
    s32 temp_s1;
    s32 temp_s3;
    s32 var_a3;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;
    s32 var_v1;
    s32 var_v1_2;

    Plane_FromPoints(&sp50, arg2, arg2 + 0xC, arg2 + 0x18);
    temp_f16 = M2C_FIELD(arg1, f32 *, 0xC);
    temp_f8 = (sp50 * temp_f16) + (sp54 * M2C_FIELD(arg1, f32 *, 0x10)) + (sp58 * M2C_FIELD(arg1, f32 *, 0x14));
    var_v0_2 = 1;
    if (!(temp_f8 > -1e-16f)) {
        var_v0_2 = 0;
    }
    var_v1 = 1;
    if (!(temp_f8 < 1e-16f)) {
        var_v1 = 0;
    }
    var_v0_3 = 0;
    if (!(var_v0_2 & var_v1)) {
        temp_f6 = M2C_FIELD(arg1, f32 *, 0);
        temp_f4 = -((sp50 * temp_f6) + (sp54 * M2C_FIELD(arg1, f32 *, 4)) + (sp58 * M2C_FIELD(arg1, f32 *, 8)) + sp5C) / temp_f8;
        M2C_FIELD(arg0, f32 *, 0) = temp_f6 + (temp_f16 * temp_f4);
        M2C_FIELD(arg0, f32 *, 4) = (f32) (M2C_FIELD(arg1, f32 *, 4) + (M2C_FIELD(arg1, f32 *, 0x10) * temp_f4));
        var_v0_3 = 1;
        M2C_FIELD(arg0, f32 *, 8) = (f32) (M2C_FIELD(arg1, f32 *, 8) + (M2C_FIELD(arg1, f32 *, 0x14) * temp_f4));
    }
    if (var_v0_3 == 0) {
        return 0;
    }
    Vec3_TriangleNormal(M2C_BITWISE(M2C_UNK *, sp54), M2C_BITWISE(f32 *, sp50), &sp10, arg2, arg2 + 0xC, arg2 + 0x18);
    var_v1_2 = 0;
loop_9:
    temp_s2 = arg2 + (var_v1_2 * 0xC);
    temp_s1 = var_v1_2 + 1;
    temp_s3 = temp_s1 < 3;
    temp_s0 = arg2 + ((temp_s1 & -temp_s3) * 0xC);
    if (Vec3_TriangleNormal(&sp20, temp_s2, temp_s0, arg0) == 0) {
        var_a3 = 0;
        var_a0 = arg0;
        var_a2 = temp_s0;
        var_a1 = temp_s2;
        var_v1_3 = &sp10;
        do {
            M2C_FIELD(var_v1_3, f32 *, 0x20) = (f32) (*var_a1 - *var_a0);
            var_a1 += 4;
            var_a3 += 1;
            temp_f2 = *var_a2 - *var_a0;
            var_a0 += 4;
            var_a2 += 4;
            M2C_FIELD(var_v1_3, f32 *, 0x30) = temp_f2;
            var_v1_3 += 4;
        } while (var_a3 < 3);
        var_v0 = 0;
        if (Vec3_CosAngle(&sp30, &sp40, var_a2, var_a3) <= 0.0f) {
            return 1;
        }
        /* Duplicate return node #17. Try simplifying control flow for better match */
        return var_v0;
    }
    var_v0 = 0;
    if (!(Vec3_Dot(&sp10, &sp20) < 0.0f)) {
        var_v1_2 = temp_s1;
        if (temp_s3 == 0) {
            var_v0 = 1;
            /* Duplicate return node #17. Try simplifying control flow for better match */
            return var_v0;
        }
        goto loop_9;
    }
    return var_v0;
}

void Heap_Init(M2C_UNK *arg0, s32 arg1) {
    M2C_FIELD(arg0, M2C_UNK **, 0x10) = arg0;
    M2C_FIELD(arg0, M2C_UNK **, 0) = arg0;
    M2C_FIELD(arg0, s32 *, 4) = 0;
    Heap_AddRegion(arg0 + 0x20, arg1 - 0x20);
}

void Heap_AddRegion(void *arg1, u32 arg2) {
    u32 temp_a2;

    temp_a2 = arg2 >> 4;
    M2C_FIELD(arg1, s32 *, 0) = (s32) ~arg1;
    M2C_FIELD(arg1, u32 *, 4) = temp_a2;
    Heap_Free(arg1 + 0x10, (M2C_UNK *) temp_a2);
}

M2C_UNK *Heap_Alloc(M2C_UNK *arg0, s32 arg1) {
    M2C_UNK *var_a2;
    M2C_UNK *var_a3;
    u32 temp_a1;
    u32 temp_v0;
    u32 temp_v1;

    var_a3 = M2C_FIELD(arg0, M2C_UNK **, 0x10);
    temp_a1 = ((u32) (arg1 + 0xF) >> 4) + 1;
    var_a2 = *var_a3;
loop_1:
    temp_v1 = M2C_FIELD(var_a2, u32 *, 4);
    if (temp_v1 >= temp_a1) {
        temp_v0 = temp_v1 - temp_a1;
        if (temp_v1 == temp_a1) {
            *var_a3 = M2C_FIELD(var_a2, M2C_UNK **, 0);
        } else {
            M2C_FIELD(var_a2, u32 *, 4) = temp_v0;
            var_a2 += temp_v0 * 0x10;
            M2C_FIELD(var_a2, u32 *, 4) = temp_a1;
        }
        *var_a2 = ~var_a2;
        M2C_FIELD(arg0, M2C_UNK **, 0x10) = var_a3;
        return var_a2 + 0x10;
    }
    var_a3 = var_a2;
    if (var_a2 != M2C_FIELD(arg0, M2C_UNK **, 0x10)) {
        var_a2 = M2C_FIELD(var_a2, M2C_UNK **, 0);
        goto loop_1;
    }
    return NULL;
}

void *Heap_Free(M2C_UNK *arg0, M2C_UNK *arg1) {
    s32 temp_a3;
    s32 temp_v1_3;
    s32 var_v0_2;
    u32 temp_v1;
    u32 temp_v1_2;
    u32 var_a2;
    u32 var_v0;
    void *temp_a1;

    var_v0 = 0;
    if (arg1 != NULL) {
        temp_a1 = arg1 - 0x10;
        var_v0 = 0;
        if (~M2C_FIELD(arg1, s32 *, -0x10) == temp_a1) {
            var_a2 = M2C_FIELD(arg0, u32 *, 0x10);
            var_v0_2 = var_a2 < (u32) temp_a1;
loop_7:
            if ((var_v0_2 == 0) || ((u32) temp_a1 >= (u32) M2C_FIELD(var_a2, u32 *, 0))) {
                temp_v1 = M2C_FIELD(var_a2, u32 *, 0);
                if ((var_a2 < temp_v1) || ((var_a2 >= (u32) temp_a1) && ((u32) temp_a1 >= temp_v1))) {
                    var_a2 = temp_v1;
                    var_v0_2 = var_a2 < (u32) temp_a1;
                    goto loop_7;
                }
            }
            temp_a3 = M2C_FIELD(temp_a1, s32 *, 4);
            temp_v1_2 = M2C_FIELD(var_a2, u32 *, 0);
            if ((temp_a1 + (temp_a3 * 0x10)) != temp_v1_2) {
                M2C_FIELD(temp_a1, u32 *, 0) = temp_v1_2;
            } else {
                M2C_FIELD(temp_a1, s32 *, 4) = (s32) (temp_a3 + M2C_FIELD(temp_v1_2, s32 *, 4));
                M2C_FIELD(temp_a1, u32 *, 0) = (u32) M2C_FIELD(M2C_FIELD(var_a2, u32 *, 0), u32 *, 0);
            }
            temp_v1_3 = M2C_FIELD(var_a2, s32 *, 4);
            if ((var_a2 + (temp_v1_3 * 0x10)) != temp_a1) {
                M2C_FIELD(var_a2, u32 *, 0) = (u32) temp_a1;
            } else {
                M2C_FIELD(var_a2, s32 *, 4) = (s32) (temp_v1_3 + M2C_FIELD(temp_a1, s32 *, 4));
                M2C_FIELD(var_a2, u32 *, 0) = (u32) M2C_FIELD(temp_a1, u32 *, 0);
            }
            M2C_FIELD(arg0, u32 *, 0x10) = var_a2;
            var_v0 = var_a2;
        }
    }
    return (void *) var_v0;
}

u8 *Heap_AllocFront(M2C_UNK *arg0, s32 arg1) {
    M2C_UNK *temp_v1_2;
    M2C_UNK *var_a2;
    M2C_UNK *var_a3;
    u32 temp_a1;
    u32 temp_v1;

    var_a3 = M2C_FIELD(arg0, M2C_UNK **, 0x10);
    temp_a1 = ((u32) (arg1 + 0xF) >> 4) + 1;
    var_a2 = *var_a3;
loop_1:
    temp_v1 = M2C_FIELD(var_a2, u32 *, 4);
    if (temp_v1 >= temp_a1) {
        if (temp_v1 == temp_a1) {
            *var_a3 = M2C_FIELD(var_a2, M2C_UNK **, 0);
        } else {
            temp_v1_2 = var_a2 + (temp_a1 * 0x10);
            M2C_FIELD(var_a2, u32 *, 4) = temp_a1;
            M2C_FIELD(temp_v1_2, s32 *, 4) = (s32) (temp_v1 - temp_a1);
            M2C_FIELD(temp_v1_2, M2C_UNK **, 0) = M2C_FIELD(var_a2, M2C_UNK **, 0);
            *var_a3 = temp_v1_2;
        }
        M2C_FIELD(var_a2, M2C_UNK **, 0) = (M2C_UNK *) ~var_a2;
        M2C_FIELD(arg0, M2C_UNK **, 0x10) = var_a3;
        return var_a2 + 0x10;
    }
    var_a3 = var_a2;
    if (var_a2 != M2C_FIELD(arg0, M2C_UNK **, 0x10)) {
        var_a2 = M2C_FIELD(var_a2, M2C_UNK **, 0);
        goto loop_1;
    }
    return NULL;
}

s32 Heap_TotalFree(void *arg0) {
    M2C_UNK **temp_v0;
    M2C_UNK *var_v1;
    s32 var_a0;

    temp_v0 = M2C_FIELD(arg0, M2C_UNK ***, 0x10);
    var_v1 = *temp_v0;
    var_a0 = M2C_FIELD(var_v1, s32 *, 4);
    if (var_v1 != temp_v0) {
        do {
            var_v1 = *var_v1;
            var_a0 += M2C_FIELD(var_v1, s32 *, 4);
        } while (var_v1 != temp_v0);
    }
    return var_a0 * 0x10;
}

s32 Heap_LargestFree(void *arg0) {
    u32 temp_a1;
    u32 var_a2;
    void *var_v1;

    var_a2 = 0;
    var_v1 = *M2C_FIELD(arg0, void ***, 0x10);
loop_1:
    temp_a1 = M2C_FIELD(var_v1, u32 *, 4);
    if (var_a2 < temp_a1) {
        var_a2 = temp_a1;
    }
    if (var_v1 != M2C_FIELD(arg0, void ***, 0x10)) {
        var_v1 = M2C_FIELD(var_v1, void **, 0);
        goto loop_1;
    }
    return var_a2 * 0x10;
}

s32 Math_Log2Ceil(u32 arg0) {
    s32 var_v1;

    var_v1 = 0;
    if (arg0 > 1U) {
        do {
            var_v1 += 1;
        } while ((u32) (1 << var_v1) < arg0);
    }
    return var_v1;
}

s32 Math_Log2Floor(u32 arg0) {
    s32 var_v1;

    var_v1 = 0;
    if (arg0 > 1U) {
        do {
            var_v1 += 1;
        } while ((u32) (1 << var_v1) < arg0);
    }
    return var_v1 - ((1 << var_v1) != arg0);
}

s32 Math_Log2Exact(u32 arg0) {
    s32 var_v1;

    var_v1 = 0;
    if (arg0 > 1U) {
        do {
            var_v1 += 1;
        } while ((u32) (1 << var_v1) < arg0);
    }
    if ((1 << var_v1) != arg0) {
        var_v1 = -1;
    }
    return var_v1;
}

void func_80003650(f32 *arg0, void *arg1, void *arg2, f32 arg3, f32 arg4, f32 arg5) {
    f32 *var_a3;
    f32 *var_v1;
    f32 temp_f12;
    f32 temp_f14;
    f32 temp_f14_2;
    f32 temp_f16;
    f32 temp_f20;
    f32 temp_f20_2;
    f32 temp_f22;
    f32 temp_f22_2;
    f32 temp_f24;
    f32 temp_f24_2;
    f32 temp_f26;
    f32 temp_f26_2;
    f32 temp_f28;
    f32 temp_f28_2;
    f32 temp_f2;
    f32 temp_f30;
    f32 temp_f30_2;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 temp_f6;
    f32 temp_f6_2;
    f32 temp_f6_3;
    f32 var_f2;
    f64 temp_f0;
    s32 var_a0;
    s32 var_a1;

    var_a1 = 0;
    var_a3 = arg0;
    do {
        var_a0 = 0;
        var_v1 = var_a3;
loop_2:
        if (var_a1 != var_a0) {
            *var_v1 = 0.0f;
        } else {
            *var_v1 = 1.0f;
        }
        var_a0 += 1;
        var_v1 += 4;
        if (var_a0 < 4) {
            goto loop_2;
        }
        var_a1 += 1;
        var_a3 += 0x10;
    } while (var_a1 < 4);
    temp_f2 = M2C_FIELD(arg2, f32 *, 0) - M2C_FIELD(arg1, f32 *, 0);
    temp_f4 = M2C_FIELD(arg2, f32 *, 4) - M2C_FIELD(arg1, f32 *, 4);
    temp_f4_2 = M2C_FIELD(arg2, f32 *, 8) - M2C_FIELD(arg1, f32 *, 8);
    temp_f12 = (temp_f2 * temp_f2) + (temp_f4 * temp_f4) + (temp_f4_2 * temp_f4_2);
    var_f2 = sqrtf(temp_f12);
    if (var_f2 != var_f2) {
        var_f2 = sqrtf(temp_f12, temp_f4, var_a0, var_a1, var_a3);
    }
    temp_f6 = (f32) (-1.0 / (f64) var_f2);
    temp_f16 = temp_f4_2 * temp_f6;
    temp_f14 = temp_f2 * temp_f6;
    temp_f14_2 = temp_f4 * temp_f6;
    temp_f20 = (arg4 * temp_f16) - (arg5 * temp_f14_2);
    temp_f22 = (arg5 * temp_f14) - (arg3 * temp_f16);
    temp_f24 = (arg3 * temp_f14_2) - (arg4 * temp_f14);
    temp_f6_2 = (f32) (M2C_BITWISE(f64, D_8003D5E8) / (f64) sqrtf((temp_f20 * temp_f20) + (temp_f22 * temp_f22) + (temp_f24 * temp_f24)));
    temp_f20_2 = temp_f20 * temp_f6_2;
    temp_f24_2 = temp_f24 * temp_f6_2;
    temp_f22_2 = temp_f22 * temp_f6_2;
    temp_f28 = (temp_f14_2 * temp_f24_2) - (temp_f16 * temp_f22_2);
    temp_f30 = (temp_f16 * temp_f20_2) - (temp_f14 * temp_f24_2);
    temp_f26 = (temp_f14 * temp_f22_2) - (temp_f14_2 * temp_f20_2);
    temp_f0 = M2C_BITWISE(f64, D_8003D5E8) / (f64) sqrtf((temp_f28 * temp_f28) + (temp_f30 * temp_f30) + (temp_f26 * temp_f26));
    M2C_FIELD(arg0, f32 *, 0) = temp_f20_2;
    M2C_FIELD(arg0, f32 *, 0x10) = temp_f22_2;
    M2C_FIELD(arg0, f32 *, 0x20) = temp_f24_2;
    temp_f6_3 = (f32) temp_f0;
    temp_f28_2 = temp_f28 * temp_f6_3;
    temp_f30_2 = temp_f30 * temp_f6_3;
    temp_f26_2 = temp_f26 * temp_f6_3;
    M2C_FIELD(arg0, f32 *, 4) = temp_f28_2;
    M2C_FIELD(arg0, f32 *, 0x14) = temp_f30_2;
    M2C_FIELD(arg0, f32 *, 0x24) = temp_f26_2;
    M2C_FIELD(arg0, f32 *, 0x30) = (f32) -((M2C_FIELD(arg1, f32 *, 0) * temp_f20_2) + (M2C_FIELD(arg1, f32 *, 4) * temp_f22_2) + (M2C_FIELD(arg1, f32 *, 8) * temp_f24_2));
    M2C_FIELD(arg0, f32 *, 8) = temp_f14;
    M2C_FIELD(arg0, f32 *, 0x18) = temp_f14_2;
    M2C_FIELD(arg0, f32 *, 0x28) = temp_f16;
    M2C_FIELD(arg0, f32 *, 0x34) = (f32) -((M2C_FIELD(arg1, f32 *, 0) * temp_f28_2) + (M2C_FIELD(arg1, f32 *, 4) * temp_f30_2) + (M2C_FIELD(arg1, f32 *, 8) * temp_f26_2));
    M2C_FIELD(arg0, s32 *, 0xC) = 0;
    M2C_FIELD(arg0, s32 *, 0x1C) = 0;
    M2C_FIELD(arg0, s32 *, 0x2C) = 0;
    M2C_FIELD(arg0, f32 *, 0x3C) = 1.0f;
    M2C_FIELD(arg0, f32 *, 0x38) = (f32) -((M2C_FIELD(arg1, f32 *, 0) * temp_f14) + (M2C_FIELD(arg1, f32 *, 4) * temp_f14_2) + (M2C_FIELD(arg1, f32 *, 8) * temp_f16));
}

void func_800039F0(M2C_UNK *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    M2C_UNK **temp_v0_4;
    M2C_UNK **temp_v0_5;
    M2C_UNK *temp_a0;
    M2C_UNK *temp_a3;
    M2C_UNK *temp_a3_2;
    M2C_UNK *temp_v0;
    M2C_UNK *temp_v0_2;
    M2C_UNK *temp_v0_6;
    M2C_UNK *temp_v0_7;
    M2C_UNK *var_a0;
    M2C_UNK *var_a0_2;
    M2C_UNK *var_a0_3;
    M2C_UNK *var_a0_4;
    M2C_UNK *var_a3_2;
    M2C_UNK *var_s2;
    M2C_UNK *var_t1;
    M2C_UNK *var_t2;
    M2C_UNK *var_t3;
    M2C_UNK *var_v0;
    M2C_UNK *var_v0_2;
    M2C_UNK *var_v1;
    M2C_UNK *var_v1_2;
    s32 temp_s1;
    s32 var_a1;
    s32 var_a2;
    s32 var_a2_2;
    s32 var_a2_3;
    s32 var_a2_4;
    s32 var_s1;
    s32 var_t0;
    s32 var_t0_2;
    s32 var_t0_3;
    u8 *var_a3;
    u8 temp_v0_3;

    temp_s1 = arg3 << 0xA;
    temp_v0 = Heap_Alloc(&gGameHeap, temp_s1);
    var_s2 = temp_v0;
    temp_v0_2 = Heap_Alloc(&gGameHeap, arg1 * 4);
    var_t3 = temp_v0_2;
    var_s1 = temp_s1 >> 2;
    var_v0 = temp_v0;
    if (var_s1 != 0) {
        do {
            *var_v0 = NULL;
            var_s1 -= 1;
            var_v0 += 4;
        } while (var_s1 != 0);
    }
    var_t2 = arg0;
    var_t0 = 0;
    if (arg1 > 0) {
        var_t1 = arg0;
        do {
            var_a2 = 0;
            var_a3 = *var_t1 + arg2;
            if (arg3 > 0) {
                var_a0 = temp_v0;
                do {
                    temp_v0_3 = *var_a3;
                    var_a3 += 1;
                    temp_v0_4 = &var_a0[temp_v0_3];
                    var_a2 += 1;
                    *temp_v0_4 += 1;
                    var_a0 += 0x400;
                } while (var_a2 < arg3);
            }
            var_t0 += 1;
            var_t1 += 4;
        } while (var_t0 < arg1);
    }
    var_t0_2 = 0;
    if (arg3 > 0) {
        do {
            var_a2_2 = 0xFF;
            if (arg4 == 0) {
                var_a0_2 = NULL;
                var_a2_3 = 0;
                var_v1 = var_s2;
                do {
                    temp_a3 = *var_v1;
                    *var_v1 = var_a0_2;
                    var_v1 += 4;
                    var_a2_3 += 1;
                    var_a0_2 += temp_a3;
                } while (var_a2_3 < 0x100);
                var_t0_2 += 1;
            } else {
                var_a0_3 = NULL;
                var_v0_2 = var_s2 + 0x3FC;
                do {
                    temp_a3_2 = *var_v0_2;
                    *var_v0_2 = var_a0_3;
                    var_v0_2 -= 4;
                    var_a2_2 -= 1;
                    var_a0_3 += temp_a3_2;
                } while (var_a2_2 >= 0);
                var_t0_2 += 1;
            }
            var_s2 += 0x400;
        } while (var_t0_2 < arg3);
    }
    var_t0_3 = (arg2 + arg3) - 1;
    if (var_t0_3 >= arg2) {
        do {
            var_s2 -= 0x400;
            var_a2_4 = 0;
            if (arg1 > 0) {
                var_a3_2 = var_t2;
                do {
                    temp_v0_5 = &var_s2[*(*var_a3_2 + var_t0_3)];
                    temp_a0 = *temp_v0_5;
                    var_a2_4 += 1;
                    *temp_v0_5 = temp_a0 + 1;
                    var_t3[(s32) temp_a0] = *var_a3_2;
                    var_a3_2 += 4;
                } while (var_a2_4 < arg1);
            }
            temp_v0_6 = var_t2;
            var_t2 = var_t3;
            var_t3 = temp_v0_6;
            var_t0_3 -= 1;
        } while (var_t0_3 >= arg2);
    }
    var_a1 = arg1 * 4;
    if (var_t2 != arg0) {
        var_v1_2 = temp_v0_2;
        if (var_a1 != 0) {
            var_a0_4 = arg0;
            do {
                temp_v0_7 = *var_v1_2;
                var_v1_2 += 4;
                var_a1 -= 4;
                *var_a0_4 = temp_v0_7;
                var_a0_4 += 4;
            } while (var_a1 > 0);
        }
    }
    Heap_Free(&gGameHeap, temp_v0_2);
    Heap_Free(&gGameHeap, temp_v0);
}

void func_80003C4C(M2C_UNK *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    M2C_UNK *temp_v0;
    M2C_UNK *temp_v0_2;
    M2C_UNK *temp_v0_3;
    M2C_UNK *temp_v0_5;
    M2C_UNK *var_a0;
    M2C_UNK *var_a0_2;
    M2C_UNK *var_a1;
    M2C_UNK *var_a1_2;
    M2C_UNK *var_a1_3;
    M2C_UNK *var_a3;
    M2C_UNK *var_s2;
    M2C_UNK *var_s2_2;
    M2C_UNK *var_v1;
    s32 var_a2;
    s32 var_a2_2;
    s32 var_s1;
    s32 var_v1_2;
    s32 var_v1_3;
    u8 temp_v0_4;

    var_s1 = arg1 * arg2;
    temp_v0 = Heap_Alloc(&gGameHeap, var_s1);
    var_s2 = temp_v0;
    temp_v0_2 = Heap_Alloc(&gGameHeap, arg1 * 4);
    var_a1 = temp_v0_2;
    if (var_s1 != 0) {
        var_a0 = arg0;
        var_v1 = temp_v0;
        do {
            temp_v0_3 = *var_a0;
            var_a0 += 4;
            var_s1 -= 4;
            *var_v1 = temp_v0_3;
            var_v1 += 4;
        } while (var_s1 > 0);
    }
    var_a2 = 0;
    if (arg1 > 0) {
        do {
            *var_a1 = var_s2;
            var_a1 += 4;
            var_a2 += 1;
            var_s2 += arg2;
        } while (var_a2 < arg1);
    }
    func_800039F0(temp_v0_2, arg1, arg3, arg4, arg5);
    var_a0_2 = arg0;
    if (arg2 & 3) {
        var_a2_2 = 0;
        if (arg1 > 0) {
            var_a1_2 = temp_v0_2;
            do {
                var_s2_2 = *var_a1_2;
                var_v1_2 = 0;
                if (arg2 > 0) {
                    do {
                        temp_v0_4 = *var_s2_2;
                        var_s2_2 += 1;
                        var_v1_2 += 1;
                        *var_a0_2 = temp_v0_4;
                        var_a0_2 += 1;
                    } while (var_v1_2 < arg2);
                }
                var_a2_2 += 1;
                var_a1_2 += 4;
            } while (var_a2_2 < arg1);
        }
    } else {
        var_a2_2 = 0;
        if (arg1 > 0) {
            var_a3 = temp_v0_2;
            do {
                var_a1_3 = *var_a3;
                var_v1_3 = 0;
                if (arg2 > 0) {
                    do {
                        temp_v0_5 = *var_a1_3;
                        var_a1_3 += 4;
                        var_v1_3 += 4;
                        *var_a0_2 = temp_v0_5;
                        var_a0_2 += 4;
                    } while (var_v1_3 < arg2);
                }
                var_a2_2 += 1;
                var_a3 += 4;
            } while (var_a2_2 < arg1);
        }
    }
    Heap_Free(&gGameHeap, temp_v0_2, var_a2_2);
    Heap_Free(&gGameHeap, temp_v0);
}

s32 func_80003E10(s32 arg0) {
    s32 var_v0;

    var_v0 = arg0;
    if (var_v0 < 0) {
        var_v0 = -var_v0;
    }
    return var_v0;
}

f32 func_80003E24(f32 arg0) {
    f32 var_f0;

    var_f0 = arg0;
    if (!(arg0 >= 0.0f)) {
        var_f0 = -arg0;
    }
    return var_f0;
}

f32 func_80003E4C(f32 arg0, s32 *arg1) {
    s32 temp_v1;

    *arg1 = 0;
    temp_v1 = (M2C_BITWISE(u32, arg0) >> 0x17) & 0xFF;
    if (temp_v1 == 0xFF) {
        if (!(M2C_BITWISE(s32, arg0) & 0x7FFFFF)) {
            *arg1 = 0x401;
            goto block_5;
        }
    } else if (temp_v1 != 0) {
        *arg1 = temp_v1 - 0x7E;
block_5:
        arg0 = M2C_BITWISE(f32, ((M2C_BITWISE(s32, arg0) & 0x807FFFFF) | 0x3F000000));
    } else {
        arg0 = 0.0f;
    }
    return arg0;
}

void func_80003EBC(f32 arg0, s32 arg1) {
    s32 var_a1;

    var_a1 = arg1;
    if (var_a1 >= 0) {

    } else {
        var_a1 = -var_a1;
    }
    if (var_a1 != 0) {
        do {
            if (var_a1 & 1) {

            }
            var_a1 = var_a1 >> 1;
        } while (var_a1 != 0);
    }
}

void func_80003F08(f32 arg0) {
    f32 temp_f0;
    f32 var_f0;
    f32 var_f12;
    f32 var_f2;
    f32 var_f4;
    s32 var_a0;
    s32 var_v0;
    s32 var_v1;

    var_f0 = -0.5f;
    if (arg0 >= 0.0f) {
        var_f0 = 0.5f;
    }
    var_v1 = (s32) ((arg0 / 0.6931472f) + var_f0);
    var_f12 = arg0 - ((f32) var_v1 * 0.6931472f);
    var_a0 = 0;
    if (!(var_f12 > 0.0f)) {
        var_a0 = 1;
        var_f12 = -var_f12;
    }
    var_f4 = var_f12;
    var_v0 = 2;
    var_f2 = var_f12 + 1.0f;
    do {
        var_f4 *= var_f12 / (f32) var_v0;
        temp_f0 = var_f2;
        var_f2 += var_f4;
        var_v0 += 1;
    } while (var_f2 != temp_f0);
    if (var_a0 != 0) {

    }
    if (var_v1 >= 0) {

    } else {
        var_v1 = -var_v1;
    }
    if (var_v1 != 0) {
        do {
            if (var_v1 & 1) {

            }
            var_v1 = var_v1 >> 1;
        } while (var_v1 != 0);
    }
}

void func_80004034(f32 arg0) {
    f32 temp_f0;
    f32 temp_f12;
    f32 temp_f4;
    f32 var_f0;
    s32 temp_f2;
    s32 var_v1;
    s32 var_v1_2;
    f32 phi_f2;

    var_f0 = -0.5f;
    if (arg0 >= 0.0f) {
        var_f0 = 0.5f;
    }
    temp_f2 = (s32) ((arg0 / 0.6931472f) + var_f0);
    temp_f12 = arg0 - ((f32) temp_f2 * 0.6931472f);
    temp_f4 = temp_f12 * temp_f12;
    var_v1 = 0x12;
    phi_f2 = temp_f4 / (f32) 0x16;
    if (0x12 >= 6) {
        do {
            temp_f0 = phi_f2 + (f32) var_v1;
            var_v1 -= 4;
            phi_f2 = temp_f4 / temp_f0;
        } while (var_v1 >= 6);
    }
    var_v1_2 = temp_f2;
    if (var_v1_2 < 0) {
        var_v1_2 = -var_v1_2;
    }
    if (var_v1_2 != 0) {
        do {
            if (var_v1_2 & 1) {

            }
            var_v1_2 = var_v1_2 >> 1;
        } while (var_v1_2 != 0);
    }
}

f32 func_80004130(f32 arg0) {
    s32 sp0;
    f32 sp8;                                        /* compiler-managed */
    f32 temp_f12;
    f32 temp_f2;
    f32 temp_f6;
    f32 var_f0;
    f32 var_f12;
    f32 var_f2;
    f32 var_f4;
    s32 temp_v1;
    s32 var_v0;
    s32 var_v1;
    s32 var_v1_2;

    if (arg0 <= 0.0f) {
        return 0.0f;
    }
    sp8 = arg0 / 1.4142135f;
    temp_v1 = (M2C_BITWISE(u32, sp8) >> 0x17) & 0xFF;
    sp0 = 0;
    if (temp_v1 == 0xFF) {
        if (!(M2C_BITWISE(s32, sp8) & 0x7FFFFF)) {
            var_v1 = 0x401;
            goto block_7;
        }
    } else if (temp_v1 != 0) {
        var_v1 = temp_v1 - 0x7E;
block_7:
        sp0 = var_v1;
        sp8 = (M2C_BITWISE(s32, sp8) & 0x807FFFFF) | 0x3F000000;
    } else {
        sp8 = 0.0f;
    }
    var_v1_2 = sp0;
    var_f2 = 1.0f;
    if (var_v1_2 < 0) {
        var_v1_2 = -var_v1_2;
        var_f0 = 0.5f;
    } else {
        var_f0 = 2.0f;
    }
    if (var_v1_2 != 0) {
        do {
            if (var_v1_2 & 1) {
                var_f2 *= var_f0;
            }
            var_v1_2 = var_v1_2 >> 1;
            var_f0 *= var_f0;
        } while (var_v1_2 != 0);
    }
    temp_f12 = arg0 / var_f2;
    var_f12 = (temp_f12 - 1.0f) / (temp_f12 + 1.0f);
    var_v0 = 1;
    var_f4 = var_f12;
    temp_f6 = var_f12 * var_f12;
    do {
        var_f12 *= temp_f6;
        var_v0 += 2;
        temp_f2 = var_f4;
        var_f4 += var_f12 / (f32) var_v0;
    } while (temp_f2 != var_f4);
    return ((f32) sp0 * 0.6931472f) + (2.0f * var_f4);
}

f32 func_800042AC(f32 arg0) {
    s32 sp0;
    f32 sp8;                                        /* compiler-managed */
    f32 temp_f0;
    f32 temp_f12;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 var_f0;
    f32 var_f2;
    f32 var_f4;
    s32 temp_v1;
    s32 var_v0;
    s32 var_v1;
    s32 var_v1_2;

    if (arg0 <= 0.0f) {
        return 0.0f;
    }
    sp8 = arg0 / 1.4142135f;
    temp_v1 = (M2C_BITWISE(u32, sp8) >> 0x17) & 0xFF;
    sp0 = 0;
    if (temp_v1 == 0xFF) {
        if (!(M2C_BITWISE(s32, sp8) & 0x7FFFFF)) {
            var_v1 = 0x401;
            goto block_7;
        }
    } else if (temp_v1 != 0) {
        var_v1 = temp_v1 - 0x7E;
block_7:
        sp0 = var_v1;
        sp8 = (M2C_BITWISE(s32, sp8) & 0x807FFFFF) | 0x3F000000;
    } else {
        sp8 = 0.0f;
    }
    var_v1_2 = sp0;
    var_f2 = 1.0f;
    if (var_v1_2 < 0) {
        var_v1_2 = -var_v1_2;
        var_f0 = 0.5f;
    } else {
        var_f0 = 2.0f;
    }
    if (var_v1_2 != 0) {
        do {
            if (var_v1_2 & 1) {
                var_f2 *= var_f0;
            }
            var_v1_2 = var_v1_2 >> 1;
            var_f0 *= var_f0;
        } while (var_v1_2 != 0);
    }
    var_v0 = 9;
    var_f4 = 0.0f;
    temp_f12 = (arg0 / var_f2) - 1.0f;
    if (9 > 0) {
        do {
            temp_f2 = (f32) var_v0;
            temp_f2_2 = temp_f2 * temp_f12;
            temp_f0 = (temp_f2_2 / ((2.0f * temp_f2) + 1.0f + var_f4)) + 2.0f;
            var_v0 -= 1;
            var_f4 = temp_f2_2 / temp_f0;
        } while (var_v0 > 0);
    }
    return ((f32) sp0 * 0.6931472f) + (temp_f12 / (var_f4 + 1.0f));
}

f32 func_80004434(f32 arg0, s32 arg1) {
    f32 var_f12;
    f32 var_f2;
    s32 var_v1;

    var_f12 = arg0;
    var_v1 = arg1;
    if (arg1 < 0) {
        var_v1 = -arg1;
    }
    var_f2 = 1.0f;
    if (var_v1 != 0) {
        do {
            if (var_v1 & 1) {
                var_f2 *= var_f12;
            }
            var_v1 = var_v1 >> 1;
            var_f12 *= var_f12;
        } while (var_v1 != 0);
    }
    if (arg1 < 0) {
        var_f2 = 1.0f / var_f2;
    }
    return var_f2;
}

f32 func_80004488(f32 arg0, f32 arg1) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f14;
    f32 temp_f14_2;
    f32 temp_f4;
    f32 var_f0;
    f32 var_f0_2;
    f32 var_f12;
    f32 var_f2;
    f32 var_f2_2;
    f32 var_f4;
    s32 temp_f2;
    s32 var_a0;
    s32 var_v0;
    s32 var_v1;
    s32 var_v1_2;
    s32 var_v1_3;
    s32 var_v1_4;

    var_f12 = arg0;
    var_a0 = arg0;
    var_v0 = 1;
    if (!(arg1 <= 2.1474836e9f)) {
        var_v0 = 0;
    }
    var_v1 = 1;
    if (!(arg1 >= -2.1474836e9f)) {
        var_v1 = 0;
    }
    if (var_v0 & var_v1) {
        var_v1_2 = (s32) arg1;
        var_a0 = var_v1_2;
        if (arg1 == (f32) var_v1_2) {
            if (var_a0 < 0) {
                var_v1_2 = -var_a0;
            }
            var_f2 = 1.0f;
            if (var_v1_2 != 0) {
                do {
                    if (var_v1_2 & 1) {
                        var_f2 *= var_f12;
                    }
                    var_v1_2 = var_v1_2 >> 1;
                    var_f12 *= var_f12;
                } while (var_v1_2 != 0);
            }
            if (var_a0 >= 0) {
                return var_f2;
            }
            return 1.0f / var_f2;
        }
    }
    var_f0 = 0.0f;
    if (var_f12 > 0.0f) {
        temp_f14 = arg1 * func_800042AC(M2C_BITWISE(f32, var_a0));
        var_f0_2 = -0.5f;
        if (temp_f14 >= 0.0f) {
            var_f0_2 = 0.5f;
        }
        temp_f2 = (s32) ((temp_f14 / 0.6931472f) + var_f0_2);
        temp_f14_2 = temp_f14 - ((f32) temp_f2 * 0.6931472f);
        temp_f4 = temp_f14_2 * temp_f14_2;
        var_v1_3 = 0x12;
        var_f2_2 = temp_f4 / (f32) 0x16;
        if (0x12 >= 6) {
            do {
                temp_f0 = var_f2_2 + (f32) var_v1_3;
                var_v1_3 -= 4;
                var_f2_2 = temp_f4 / temp_f0;
            } while (var_v1_3 >= 6);
        }
        var_f4 = 2.0f;
        temp_f0_2 = var_f2_2 + 2.0f;
        var_v1_4 = temp_f2;
        var_f0 = (temp_f0_2 + temp_f14_2) / (temp_f0_2 - temp_f14_2);
        if (var_v1_4 < 0) {
            var_f4 = 0.5f;
            var_v1_4 = -var_v1_4;
        }
        if (var_v1_4 != 0) {
            do {
                if (var_v1_4 & 1) {
                    var_f0 *= var_f4;
                }
                var_v1_4 = var_v1_4 >> 1;
                var_f4 *= var_f4;
            } while (var_v1_4 != 0);
        }
    }
    return var_f0;
}

f32 func_8000468C(f32 arg0, f32 arg1) {
    f32 var_f4;
    f32 var_f6;
    f32 var_f8;
    s32 var_a0;
    s32 var_v1;

    var_a0 = 1;
    var_v1 = 1;
    var_f4 = 1.0f;
    var_f8 = 1.0f;
    var_f6 = 1.0f;
    do {
        var_f6 *= arg0 - 1.0f;
        var_f8 *= (arg1 - (f32) var_v1) + 1.0f;
        var_a0 *= var_v1;
        var_v1 += 1;
        var_f4 += (var_f8 * var_f6) / (f32) var_a0;
    } while (var_v1 < 0x16);
    return var_f4;
}

f32 func_80004708(f32 arg0) {
    f32 temp_f0;
    f32 temp_f2;
    f32 var_f0;
    f32 var_f12;
    f32 var_f2;
    f32 var_f4;
    s32 var_a0;
    s32 var_v1;

    var_f12 = arg0;
    var_a0 = 1;
    if (var_f12 > 1.0f) {
        var_f12 = 1.0f / var_f12;
    } else {
        var_a0 = -1;
        if (var_f12 < -1.0f) {
            var_f12 = 1.0f / var_f12;
        } else {
            var_a0 = 0;
        }
    }
    var_f4 = 0.0f;
    var_v1 = 0x16;
    if (0x16 > 0) {
        do {
            temp_f2 = (f32) (var_v1 * var_v1) * var_f12 * var_f12;
            temp_f0 = (2.0f * (f32) var_v1) + 1.0f + var_f4;
            var_v1 -= 1;
            var_f4 = temp_f2 / temp_f0;
        } while (var_v1 > 0);
    }
    if (var_a0 > 0) {
        var_f2 = 1.5707964f;
        var_f0 = var_f12 / (var_f4 + 1.0f);
        goto block_13;
    }
    if (var_a0 >= 0) {
        return var_f12 / (var_f4 + 1.0f);
    }
    var_f0 = var_f12 / (var_f4 + 1.0f);
    var_f2 = -1.5707964f;
block_13:
    return var_f2 - var_f0;
}

f32 func_80004838(f32 arg0, f32 arg1) {
    f32 temp_f0;
    f32 temp_f2;
    f32 var_f0;
    f32 var_f2;
    f32 var_f4;
    f32 var_f6;
    s32 var_a0;
    s32 var_v1;

    if (arg1 == 0.0f) {
        var_f0 = 0.0f;
        if (arg0 != 0.0f) {
            var_f0 = -1.5707964f;
            if (arg0 > 0.0f) {
                return 1.5707964f;
            }
        }
        /* Duplicate return node #26. Try simplifying control flow for better match */
        return var_f0;
    }
    if (arg0 != 0.0f) {
        var_f6 = arg0 / arg1;
        var_a0 = 1;
        if (var_f6 > 1.0f) {
            var_f6 = 1.0f / var_f6;
        } else {
            var_a0 = -1;
            if (var_f6 < -1.0f) {
                var_f6 = 1.0f / var_f6;
            } else {
                var_a0 = 0;
            }
        }
        var_f4 = 0.0f;
        var_v1 = 0x16;
        if (0x16 > 0) {
            do {
                temp_f2 = (f32) (var_v1 * var_v1) * var_f6 * var_f6;
                temp_f0 = (2.0f * (f32) var_v1) + 1.0f + var_f4;
                var_v1 -= 1;
                var_f4 = temp_f2 / temp_f0;
            } while (var_v1 > 0);
        }
        if (var_a0 > 0) {
            var_f2 = 1.5707964f - (var_f6 / (var_f4 + 1.0f));
        } else if (var_a0 < 0) {
            var_f2 = -1.5707964f - (var_f6 / (var_f4 + 1.0f));
        } else {
            var_f2 = var_f6 / (var_f4 + 1.0f);
        }
        if (arg1 >= 0.0f) {
            var_f0 = var_f2;
            if (!(arg0 >= 0.0f)) {
                return var_f2 + 6.2831855f;
            }
            /* Duplicate return node #26. Try simplifying control flow for better match */
            return var_f0;
        }
        var_f0 = var_f2 + 3.1415927f;
        /* Duplicate return node #26. Try simplifying control flow for better match */
        return var_f0;
    }
    var_f0 = 3.1415927f;
    if (arg1 > 0.0f) {
        return 0.0f;
    }
    return var_f0;
}

f32 func_80004A50(f32 arg0, s32 *arg1) {
    f32 temp_f0;
    f32 temp_f12;
    f32 temp_f2;
    f32 var_f0;
    f32 var_f2;
    s32 var_v1;

    var_f0 = -0.5f;
    if (arg0 >= 0.0f) {
        var_f0 = 0.5f;
    }
    *arg1 = (s32) ((arg0 / 1.5707964f) + var_f0);
    temp_f2 = (f32) *arg1;
    temp_f12 = (arg0 - (temp_f2 * 1.5708008f)) + (temp_f2 * 0.000004454455f);
    var_v1 = 0x13;
    var_f2 = 0.0f;
    if (0x13 >= 3) {
        do {
            temp_f0 = (f32) var_v1 - var_f2;
            var_v1 -= 2;
            var_f2 = (temp_f12 * temp_f12) / temp_f0;
        } while (var_v1 >= 3);
    }
    return temp_f12 / (1.0f - var_f2);
}

f32 func_80004B14(f32 arg0) {
    s32 sp0;
    f32 temp_f0;
    f32 temp_f12;
    f32 temp_f12_2;
    f32 temp_f2;
    f32 var_f0;
    f32 var_f2;
    s32 var_v1;

    var_f0 = -0.5f;
    if (arg0 >= 0.0f) {
        var_f0 = 0.5f;
    }
    sp0 = (s32) ((arg0 / 1.5707964f) + var_f0);
    temp_f2 = (f32) *sp;
    temp_f12_2 = (arg0 - (temp_f2 * 1.5708008f)) + (temp_f2 * 0.000004454455f);
    var_v1 = 0x13;
    var_f2 = 0.0f;
    if (0x13 >= 3) {
        do {
            temp_f0 = (f32) var_v1 - var_f2;
            var_v1 -= 2;
            var_f2 = (temp_f12_2 * temp_f12_2) / temp_f0;
        } while (var_v1 >= 3);
    }
    temp_f12 = temp_f12_2 / (1.0f - var_f2);
    if (!(sp0 & 1)) {
        return temp_f12;
    }
    if (temp_f12 == 0.0f) {
        return 1e16f;
    }
    return -1.0f / temp_f12;
}

f32 func_80004C3C(f32 arg0) {
    s32 sp0;
    f32 temp_f0;
    f32 temp_f12;
    f32 temp_f12_2;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 var_f0;
    f32 var_f2;
    s32 var_v1;

    temp_f12 = arg0 / 2.0f;
    var_f0 = -0.5f;
    if (temp_f12 >= 0.0f) {
        var_f0 = 0.5f;
    }
    sp0 = (s32) ((temp_f12 / 1.5707964f) + var_f0);
    temp_f2_2 = (f32) *sp;
    temp_f12_2 = (temp_f12 - (temp_f2_2 * 1.5708008f)) + (temp_f2_2 * 0.000004454455f);
    var_v1 = 0x13;
    var_f2 = 0.0f;
    if (0x13 >= 3) {
        do {
            temp_f0 = (f32) var_v1 - var_f2;
            var_v1 -= 2;
            var_f2 = (temp_f12_2 * temp_f12_2) / temp_f0;
        } while (var_v1 >= 3);
    }
    temp_f2_3 = temp_f12_2 / (1.0f - var_f2);
    temp_f2 = (2.0f * temp_f2_3) / ((temp_f2_3 * temp_f2_3) + 1.0f);
    if (sp0 & 1) {
        return -temp_f2;
    }
    return temp_f2;
}

void func_80004D44(f32 arg0) {
    f32 var_f0;

    if (arg0 >= 0.0f) {
        var_f0 = arg0;
    } else {
        var_f0 = -arg0;
    }
    func_8001C5A0(1.5707964f - var_f0);
}

f32 func_80004D90(f32 arg0) {
    f32 temp_f4;
    f32 var_f0;
    f32 var_f2;

    var_f0 = 0.0f;
    if (arg0 > 0.0f) {
        var_f2 = 1.0f;
        if (arg0 > 1.0f) {
            var_f2 = arg0;
        }
        do {
            temp_f4 = var_f2;
            var_f2 = ((arg0 / var_f2) + var_f2) / 2.0f;
        } while (var_f2 < temp_f4);
        var_f0 = temp_f4;
    }
    return var_f0;
}

f32 func_80004E04(f32 arg0) {
    f32 temp_f4;
    f32 var_f0;
    f32 var_f0_2;
    f32 var_f12;
    s32 var_v0;

    var_f12 = arg0;
    var_f0 = 0.0f;
    if (var_f12 != 0.0f) {
        var_v0 = 1;
        if (!(var_f12 > 0.0f)) {
            var_v0 = 0;
            var_f12 = -var_f12;
        }
        var_f0_2 = 1.0f;
        if (var_f12 > 1.0f) {
            var_f0_2 = var_f12;
        }
        do {
            temp_f4 = var_f0_2;
            var_f0_2 = ((var_f12 / (temp_f4 * temp_f4)) + (2.0f * temp_f4)) / 3.0f;
        } while (var_f0_2 < temp_f4);
        var_f0 = temp_f4;
        if (var_v0 == 0) {
            var_f0 = -temp_f4;
        }
    }
    return var_f0;
}

s32 func_80004EA4(f32 arg0, f32 arg1, void *arg2) {
    f32 temp_f12;
    f32 temp_f12_2;
    f32 temp_f2;
    f32 var_f0;
    s32 var_v0;

    temp_f12 = (arg0 * arg0) + (arg1 * arg1);
    var_f0 = sqrtf(temp_f12);
    if (var_f0 != var_f0) {
        var_f0 = sqrtf(temp_f12);
    }
    temp_f12_2 = arg0 + var_f0;
    var_v0 = 0;
    if (temp_f12_2 != 0.0f) {
        temp_f2 = sqrtf(temp_f12_2);
        M2C_FIELD(arg2, f32 *, 0) = (f32) (temp_f2 / 1.4142135f);
        M2C_FIELD(arg2, f32 *, 4) = (f32) (arg1 / (temp_f2 * 1.4142135f));
        var_v0 = 1;
        M2C_FIELD(arg2, f32 *, 8) = (f32) -M2C_FIELD(arg2, f32 *, 0);
        M2C_FIELD(arg2, f32 *, 0xC) = (f32) -M2C_FIELD(arg2, f32 *, 4);
    }
    return var_v0;
}

s32 func_80004F80(f32 arg0, f32 arg1, void *arg2) {
    f32 temp_f12;
    f32 temp_f20;
    f32 temp_f28;
    f32 temp_f4;
    f32 var_f0;
    f32 var_f22;
    f32 var_f2;
    f32 var_f6;
    s32 var_s1;
    s32 var_v0;
    void *var_s0;

    var_s0 = arg2;
    temp_f12 = (arg0 * arg0) + (arg1 * arg1);
    var_f2 = sqrtf(temp_f12);
    temp_f28 = func_80004838(arg1, arg0);
    if (var_f2 != var_f2) {
        var_f2 = sqrtf(temp_f12);
    }
    var_f6 = var_f2;
    if (var_f2 == 0.0f) {
        var_f22 = 0.0f;
    } else {
        var_v0 = 1;
        if (!(var_f2 > 0.0f)) {
            var_v0 = 0;
            var_f6 = -var_f2;
        }
        var_f0 = 1.0f;
        if (var_f6 > 1.0f) {
            var_f0 = var_f6;
        }
        do {
            temp_f4 = var_f0;
            var_f0 = ((var_f6 / (temp_f4 * temp_f4)) + (2.0f * temp_f4)) / 3.0f;
        } while (var_f0 < temp_f4);
        var_f22 = temp_f4;
        if (var_v0 == 0) {
            var_f22 = -temp_f4;
        }
    }
    var_s1 = 0;
    do {
        temp_f20 = (((f32) var_s1 * 6.2831855f) + temp_f28) / 3.0f;
        M2C_FIELD(var_s0, f32 *, 0) = (f32) (var_f22 * func_8001C450(temp_f20));
        var_s1 += 1;
        M2C_FIELD(var_s0, f32 *, 4) = (f32) (var_f22 * func_8001C5A0(temp_f20));
        var_s0 += 8;
    } while (var_s1 < 3);
    return 1;
}

s32 func_8000512C(f32 arg0, f32 arg1, f32 arg2, f32 arg3, f32 *arg4, f32 *arg5) {
    f32 temp_f12;
    f32 temp_f12_2;
    f32 temp_f20;
    f32 temp_f20_2;
    f32 temp_f22;
    f32 var_f0;
    f32 var_f20;
    s32 var_v0;

    temp_f12 = (arg0 * arg0) + (arg1 * arg1);
    var_f20 = sqrtf(temp_f12);
    if (var_f20 != var_f20) {
        var_f20 = sqrtf(temp_f12);
    }
    temp_f12_2 = (arg2 * arg2) + (arg3 * arg3);
    var_f0 = sqrtf(temp_f12_2);
    if (var_f0 != var_f0) {
        var_f0 = sqrtf(temp_f12_2);
    }
    temp_f22 = var_f20 / var_f0;
    var_v0 = 0;
    if (!(temp_f22 <= 1e-16f)) {
        temp_f20 = func_80004838(arg1, arg0);
        temp_f20_2 = temp_f20 - func_80004838(arg3, arg2);
        *arg4 = temp_f22 * func_8001C450(temp_f20_2);
        var_v0 = 1;
        *arg5 = temp_f22 * func_8001C5A0(temp_f20_2);
    }
    return var_v0;
}

void func_80005270(f32 *arg0, s32 arg1, s32 arg2, s32 arg3) {
    f32 *var_a0;
    f32 *var_a2_2;
    f32 *var_t0;
    f32 *var_v1;
    f32 *var_v1_2;
    f32 *var_v1_3;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    s32 var_a2;
    s32 var_a3;
    s32 var_t1;
    s32 var_t1_2;

    var_a2 = arg2;
    var_a3 = arg3;
    if (arg1 != 0) {
        var_t1 = 0;
        if (var_a2 == 0) {
            var_a2_2 = (arg1 * 0xC) + arg0;
            var_v1 = arg0;
            do {
                temp_f2 = *var_v1;
                var_t1 += 1;
                *var_v1 = *var_a2_2;
                *var_a2_2 = temp_f2;
                var_a2_2 += 4;
                var_v1 += 4;
            } while (var_t1 < 3);
            var_a2 = arg1;
        } else {
            var_t0 = (arg1 * 0xC) + arg0;
            var_v1_2 = (var_a3 * 0xC) + arg0;
            do {
                temp_f2_2 = *var_v1_2;
                var_t1 += 1;
                *var_v1_2 = *var_t0;
                *var_t0 = temp_f2_2;
                var_t0 += 4;
                var_v1_2 += 4;
            } while (var_t1 < 3);
            var_a3 = arg1;
        }
    }
    if (var_a2 != 1) {
        var_t1_2 = 0;
        var_v1_3 = (var_a2 * 0xC) + arg0;
        var_a0 = (var_a3 * 0xC) + arg0;
        do {
            temp_f2_3 = *var_a0;
            var_t1_2 += 1;
            *var_a0 = *var_v1_3;
            *var_v1_3 = temp_f2_3;
            var_v1_3 += 4;
            var_a0 += 4;
        } while (var_t1_2 < 3);
    }
}

s32 func_80005360(void *arg0, f32 *arg1, s32 arg2) {
    f32 *temp_a0_2;
    f32 *temp_v0_2;
    f32 *temp_v0_3;
    f32 *temp_v0_6;
    f32 *temp_v0_7;
    f32 *temp_v0_8;
    f32 *temp_v0_9;
    f32 *temp_v1;
    f32 *temp_v1_3;
    f32 *var_t0;
    f32 *var_t0_2;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 temp_f4_3;
    f32 temp_f4_4;
    f32 var_f0;
    f32 var_f2;
    f64 temp_f2;
    f64 temp_f2_2;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_a2;
    s32 temp_v0;
    s32 temp_v0_4;
    s32 temp_v0_5;
    s32 var_a3;
    s32 var_t1;
    s32 var_t1_2;
    s32 var_t3;
    s32 var_v0;
    s32 var_v1;
    void *temp_a0;
    void *temp_v1_2;
    void *var_a0;
    void *var_a2;

    if (arg2 == 0) {
        var_a3 = 1;
        var_v1 = 2;
    } else {
        var_a3 = 0;
        if (arg2 == 1) {
            var_v1 = 2;
        } else {
            var_v1 = 1;
        }
    }
    var_f2 = M2C_FIELD(((var_a3 * 0xC) + arg0), f32 *, 4);
    if (var_f2 < 0.0f) {
        var_f2 = -var_f2;
    }
    var_f0 = M2C_FIELD(((var_v1 * 0xC) + arg0), f32 *, 4);
    if (var_f0 < 0.0f) {
        var_f0 = -var_f0;
    }
    var_t3 = var_v1;
    if (var_f0 < var_f2) {
        var_t3 = var_a3;
    }
    if (var_t3 == var_a3) {
        var_a3 = var_v1;
    }
    temp_f2 = (f64) M2C_FIELD(((var_t3 * 0xC) + arg0), f32 *, 4);
    if (!(temp_f2 < 1e-08) || (var_v0 = 0, !(temp_f2 > -1e-08))) {
        temp_v0 = var_t3 * 0xC;
        temp_a0 = temp_v0 + arg0;
        temp_f4 = 1.0f / M2C_FIELD(temp_a0, f32 *, 4);
        temp_f0 = M2C_FIELD(temp_a0, f32 *, 8) * temp_f4;
        temp_v1 = (var_t3 * 0x10) + arg1;
        temp_a1 = arg2 * 4;
        temp_v0_2 = temp_a1 + (temp_v0 + arg1);
        M2C_FIELD(temp_a0, f32 *, 4) = 1.0f;
        M2C_FIELD(temp_a0, f32 *, 8) = temp_f0;
        *temp_v1 = temp_f4;
        var_t1 = 0;
        var_t0 = arg1;
        var_a2 = arg0;
        *temp_v0_2 = *temp_v0_2 * temp_f4;
        do {
            if (var_t1 != var_t3) {
                temp_f0_2 = M2C_FIELD(var_a2, f32 *, 4);
                M2C_FIELD(var_a2, f32 *, 4) = 0.0f;
                temp_f4_2 = -temp_f0_2;
                M2C_FIELD(var_a2, f32 *, 8) = (f32) (M2C_FIELD(var_a2, f32 *, 8) + (temp_f4_2 * M2C_FIELD(temp_a0, f32 *, 8)));
                *((var_t3 * 4) + var_t0) = temp_f4_2 * *temp_v1;
                temp_v0_3 = temp_a1 + var_t0;
                *temp_v0_3 += temp_f4_2 * *temp_v0_2;
            }
            var_t0 += 0xC;
            var_t1 += 1;
            var_a2 += 0xC;
        } while (var_t1 < 3);
        temp_f2_2 = (f64) M2C_FIELD(((var_a3 * 0xC) + arg0), f32 *, 8);
        if (!(temp_f2_2 < 1e-08) || (var_v0 = 0, !(temp_f2_2 > -1e-08))) {
            temp_v0_4 = var_a3 * 0xC;
            temp_v1_2 = temp_v0_4 + arg0;
            temp_f4_3 = 1.0f / M2C_FIELD(temp_v1_2, f32 *, 8);
            temp_a0_2 = (var_a3 * 0x10) + arg1;
            temp_a2 = arg2 * 4;
            temp_v0_5 = temp_v0_4 + arg1;
            M2C_FIELD(temp_v1_2, f32 *, 8) = 1.0f;
            temp_v1_3 = temp_a2 + temp_v0_5;
            *temp_a0_2 = temp_f4_3;
            temp_a1_2 = var_t3 * 4;
            temp_v0_6 = temp_a1_2 + temp_v0_5;
            *temp_v1_3 *= temp_f4_3;
            var_t1_2 = 0;
            var_t0_2 = arg1;
            var_a0 = arg0;
            *temp_v0_6 = *temp_v0_6 * temp_f4_3;
            do {
                temp_v0_7 = temp_a2 + var_t0_2;
                if (var_t1_2 != var_a3) {
                    temp_f0_3 = M2C_FIELD(var_a0, f32 *, 8);
                    M2C_FIELD(var_a0, f32 *, 8) = 0.0f;
                    temp_f4_4 = -temp_f0_3;
                    *temp_v0_7 += temp_f4_4 * *temp_v1_3;
                    temp_v0_8 = temp_a1_2 + var_t0_2;
                    *temp_v0_8 += temp_f4_4 * *temp_v0_6;
                    temp_v0_9 = (var_a3 * 4) + var_t0_2;
                    *temp_v0_9 += temp_f4_4 * *temp_a0_2;
                }
                var_t0_2 += 0xC;
                var_t1_2 += 1;
                var_a0 += 0xC;
            } while (var_t1_2 < 3);
            func_80005270(arg1, arg2, var_t3, var_a3);
            var_v0 = 1;
        }
    }
    return var_v0;
}

s32 func_80005684(M2C_UNK *arg0, M2C_UNK *arg1) {
    M2C_UNK *var_t1;
    M2C_UNK *var_v1;
    f32 *temp_v0;
    f32 *temp_v0_2;
    f32 temp_f0;
    f32 temp_f6;
    f32 temp_f6_2;
    f32 var_f0;
    f32 var_f2;
    f32 var_f4;
    f64 temp_f2;
    s32 var_a2;
    s32 var_t0;
    s32 var_v0;

    M2C_FIELD(arg1, f32 *, 0x1C) = 0.0f;
    M2C_FIELD(arg1, f32 *, 0x18) = 0.0f;
    M2C_FIELD(arg1, f32 *, 0x14) = 0.0f;
    M2C_FIELD(arg1, f32 *, 0xC) = 0.0f;
    M2C_FIELD(arg1, f32 *, 8) = 0.0f;
    M2C_FIELD(arg1, f32 *, 4) = 0.0f;
    M2C_FIELD(arg1, f32 *, 0x20) = 1.0f;
    M2C_FIELD(arg1, f32 *, 0x10) = 1.0f;
    M2C_FIELD(arg1, f32 *, 0) = 1.0f;
    var_f4 = M2C_FIELD(arg0, f32 *, 0);
    if (var_f4 < 0.0f) {
        var_f4 = -var_f4;
    }
    var_f2 = M2C_FIELD(arg0, f32 *, 0xC);
    if (var_f2 < 0.0f) {
        var_f2 = -var_f2;
    }
    var_f0 = M2C_FIELD(arg0, f32 *, 0x18);
    if (var_f0 < 0.0f) {
        var_f0 = -var_f0;
    }
    if (var_f2 < var_f4) {
        var_a2 = 2;
        if (var_f0 < var_f4) {
            var_a2 = 0;
        }
    } else {
        var_a2 = 2;
        if (var_f0 < var_f2) {
            var_a2 = 1;
        }
    }
    temp_f2 = (f64) *((var_a2 * 0xC) + arg0);
    if (!(temp_f2 < 1e-08) || (var_v0 = 0, !(temp_f2 > -1e-08))) {
        temp_v0 = (var_a2 * 0xC) + arg0;
        temp_f6 = 1.0f / M2C_FIELD(temp_v0, f32 *, 0);
        var_t0 = 0;
        temp_v0_2 = (var_a2 * 0x10) + arg1;
        var_t1 = arg1;
        var_v1 = arg0;
        M2C_FIELD(temp_v0, f32 *, 0) = 1.0f;
        M2C_FIELD(temp_v0, f32 *, 4) = (f32) (M2C_FIELD(temp_v0, f32 *, 4) * temp_f6);
        M2C_FIELD(temp_v0, f32 *, 8) = (f32) (M2C_FIELD(temp_v0, f32 *, 8) * temp_f6);
        *temp_v0_2 = temp_f6;
        do {
            if (var_t0 != var_a2) {
                temp_f0 = M2C_FIELD(var_v1, f32 *, 0);
                M2C_FIELD(var_v1, f32 *, 0) = 0.0f;
                temp_f6_2 = -temp_f0;
                M2C_FIELD(var_v1, f32 *, 4) = (f32) (M2C_FIELD(var_v1, f32 *, 4) + (temp_f6_2 * M2C_FIELD(temp_v0, f32 *, 4)));
                M2C_FIELD(var_v1, f32 *, 8) = (f32) (M2C_FIELD(var_v1, f32 *, 8) + (temp_f6_2 * M2C_FIELD(temp_v0, f32 *, 8)));
                *((var_a2 * 4) + var_t1) = temp_f6_2 * *temp_v0_2;
            }
            var_t1 += 0xC;
            var_t0 += 1;
            var_v1 += 0xC;
        } while (var_t0 < 3);
        var_v0 = func_80005360((void *) var_a2, temp_v0) != 0;
    }
    return var_v0;
}

s32 func_80005870(f32 *arg0, f32 *arg1, f32 *arg2, s32 *arg3) {
    f32 *var_a0;
    f32 *var_t0_2;
    f32 *var_v1;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 var_f0;
    f32 var_f4;
    s32 temp_v1;
    s32 var_t0;
    s32 var_t0_3;
    s32 var_t1;

    var_a0 = arg0;
    *arg3 = -1;
    var_f0 = M2C_FIELD(var_a0, f32 *, 0x3C);
    var_f4 = 0.0f;
    if (!(var_f0 >= 0.0f)) {
        var_f0 = -var_f0;
    }
    var_t0 = 0;
    if ((f64) var_f0 < 1e-08) {
        var_v1 = var_a0;
        do {
            temp_f2 = M2C_FIELD(var_v1, f32 *, 0xC);
            if (!(var_f4 < temp_f2)) {
                if (temp_f2 < -var_f4) {
                    *arg3 = var_t0;
                    var_f4 = -M2C_FIELD(var_v1, f32 *, 0xC);
                }
                var_t0 += 1;
            } else {
                *arg3 = var_t0;
                var_f4 = M2C_FIELD(var_v1, f32 *, 0xC);
                var_t0 += 1;
            }
            var_v1 += 0x10;
        } while (var_t0 < 4);
        var_t1 = 0;
        if (*arg3 < 0) {
            return 0;
        }
        var_t0_2 = var_a0;
        do {
            temp_v1 = var_t1 * 4;
            temp_f2_2 = M2C_FIELD(var_t0_2, f32 *, 0x30);
            M2C_FIELD(var_t0_2, f32 *, 0x30) = (f32) *(temp_v1 + ((*arg3 * 0x10) + var_a0));
            var_t1 += 1;
            var_t0_2 += 4;
            *(temp_v1 + ((*arg3 * 0x10) + var_a0)) = temp_f2_2;
        } while (var_t1 < 4);
        goto block_16;
    }
block_16:
    M2C_FIELD(arg1, f32 *, 0) = -M2C_FIELD(var_a0, f32 *, 0xC);
    M2C_FIELD(arg1, f32 *, 4) = (f32) -M2C_FIELD(var_a0, f32 *, 0x1C);
    M2C_FIELD(arg1, f32 *, 8) = (f32) -M2C_FIELD(var_a0, f32 *, 0x2C);
    *arg2 = 1.0f / M2C_FIELD(var_a0, f32 *, 0x3C);
    M2C_FIELD(var_a0, f32 *, 0x2C) = 0.0f;
    M2C_FIELD(var_a0, f32 *, 0x1C) = 0.0f;
    M2C_FIELD(var_a0, f32 *, 0xC) = 0.0f;
    M2C_FIELD(var_a0, f32 *, 0x3C) = 1.0f;
    M2C_FIELD(var_a0, f32 *, 0x30) = (f32) (M2C_FIELD(var_a0, f32 *, 0x30) * *arg2);
    M2C_FIELD(var_a0, f32 *, 0x34) = (f32) (M2C_FIELD(var_a0, f32 *, 0x34) * *arg2);
    var_t0_3 = 0;
    M2C_FIELD(var_a0, f32 *, 0x38) = (f32) (M2C_FIELD(var_a0, f32 *, 0x38) * *arg2);
    do {
        M2C_FIELD(var_a0, f32 *, 0) += M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(var_a0, f32 *, 0x30);
        M2C_FIELD(var_a0, f32 *, 0x10) = (f32) (M2C_FIELD(var_a0, f32 *, 0x10) + (M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(var_a0, f32 *, 0x30)));
        var_t0_3 += 1;
        M2C_FIELD(var_a0, f32 *, 0x20) = (f32) (M2C_FIELD(var_a0, f32 *, 0x20) + (M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(var_a0, f32 *, 0x30)));
        var_a0 += 4;
    } while (var_t0_3 < 3);
    return 1;
}

s32 func_80005A58(f32 *arg0, f32 *arg1) {
    f32 sp10;
    M2C_UNK sp50;
    M2C_UNK sp78;
    f32 spA0;
    f32 spB0;
    f32 spB4;
    f32 spB8;
    f32 spC0;
    s32 spC4;
    f32 *temp_v0;
    f32 *var_a0;
    f32 *var_a0_2;
    f32 *var_a0_3;
    f32 *var_a0_4;
    f32 *var_a0_5;
    f32 *var_a1;
    f32 *var_a1_2;
    f32 *var_a1_3;
    f32 *var_a1_4;
    f32 *var_a2;
    f32 *var_a2_2;
    f32 *var_a2_3;
    f32 *var_a3;
    f32 *var_a3_2;
    f32 *var_a3_3;
    f32 *var_a3_4;
    f32 *var_t1;
    f32 *var_t2;
    f32 *var_v1_5;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f2;
    f32 temp_f2_2;
    s32 var_s1;
    s32 var_t0;
    s32 var_t0_2;
    s32 var_t0_3;
    s32 var_t0_4;
    s32 var_t0_5;
    s32 var_t0_6;
    s32 var_v1;
    s32 var_v1_2;
    s32 var_v1_3;
    s32 var_v1_4;

    var_a1 = arg1;
    var_t0 = 0;
    var_t2 = arg0;
    var_t1 = &sp10;
    do {
        var_v1 = 0;
        var_a3 = var_t2;
        var_a2 = var_t1;
        var_a0 = var_a1;
loop_2:
        temp_f0 = *var_a0;
        var_a0 += 4;
        var_v1 += 1;
        *var_a2 = temp_f0;
        *var_a3 = 0.0f;
        var_a3 += 4;
        var_a2 += 4;
        if (var_v1 < 4) {
            goto loop_2;
        }
        var_t2 += 0x10;
        var_a1 += 0x10;
        var_t0 += 1;
        var_t1 += 0x10;
    } while (var_t0 < 4);
    M2C_FIELD(arg0, f32 *, 0) = 1.0f;
    M2C_FIELD(arg0, f32 *, 0x14) = 1.0f;
    M2C_FIELD(arg0, f32 *, 0x28) = 1.0f;
    M2C_FIELD(arg0, f32 *, 0x3C) = 1.0f;
    var_s1 = 0;
    if ((sp1C == 0.0f) && (sp2C == 0.0f) && (sp3C == 0.0f) && (sp4C == 1.0f)) {
        var_s1 = 1;
    }
    var_t0_2 = 0;
    if ((var_s1 != 0) || (var_t0_2 = 0, (func_80005870(&sp10, &spA0, &spC0, &spC4) != 0))) {
        var_a2_2 = &sp10;
        var_a3_2 = var_a2_2;
        spB0 = sp40;
        spB4 = sp44;
        spB8 = sp48;
        do {
            var_v1_2 = 0;
            var_a1_2 = var_a3_2 + 0x40;
            var_a0_2 = var_a2_2;
loop_13:
            temp_f0_2 = *var_a0_2;
            var_a0_2 += 4;
            var_v1_2 += 1;
            *var_a1_2 = temp_f0_2;
            var_a1_2 += 4;
            if (var_v1_2 < 3) {
                goto loop_13;
            }
            var_a2_2 += 0x10;
            var_t0_2 += 1;
            var_a3_2 += 0xC;
        } while (var_t0_2 < 3);
        var_t0_3 = 0;
        if (func_80005684(&sp50, &sp78, var_a2_2, var_a3_2) == 0) {
            goto block_16;
        }
        var_a3_3 = &sp10;
        var_a2_3 = arg0;
        do {
            var_v1_3 = 0;
            var_a1_3 = var_a2_3;
            var_a0_3 = var_a3_3 + 0x68;
loop_19:
            temp_f0_3 = *var_a0_3;
            var_a0_3 += 4;
            var_v1_3 += 1;
            *var_a1_3 = temp_f0_3;
            var_a1_3 += 4;
            if (var_v1_3 < 3) {
                goto loop_19;
            }
            var_a3_3 += 0xC;
            var_t0_3 += 1;
            var_a2_3 += 0x10;
        } while (var_t0_3 < 3);
        var_t0_4 = 0;
        var_a3_4 = arg0;
        do {
            var_v1_4 = 0;
            var_a0_4 = &sp10;
            var_a1_4 = var_a0_4;
loop_23:
            temp_f2 = M2C_FIELD(var_a1_4, f32 *, 0xA0) * M2C_FIELD(&var_a0_4[var_t0_4], f32 *, 0x68);
            var_a0_4 += 0xC;
            var_a1_4 += 4;
            var_v1_4 += 1;
            M2C_FIELD(var_a3_4, f32 *, 0x30) = (f32) (M2C_FIELD(var_a3_4, f32 *, 0x30) - temp_f2);
            if (var_v1_4 < 3) {
                goto loop_23;
            }
            var_t0_4 += 1;
            var_a3_4 += 4;
        } while (var_t0_4 < 3);
        if (var_s1 == 0) {
            var_t0_5 = 0;
            var_v1_5 = arg0;
            do {
                var_t0_5 += 1;
                M2C_FIELD(var_v1_5, f32 *, 0xC) = (f32) ((M2C_FIELD(var_v1_5, f32 *, 0xC) + ((spA0 * M2C_FIELD(var_v1_5, f32 *, 0)) + (spA4 * M2C_FIELD(var_v1_5, f32 *, 4)) + (spA8 * M2C_FIELD(var_v1_5, f32 *, 8)))) * spC0);
                var_v1_5 += 0x10;
            } while (var_t0_5 < 4);
            if (spC4 >= 0) {
                var_t0_6 = 0;
                var_a0_5 = arg0;
                do {
                    temp_f2_2 = M2C_FIELD(var_a0_5, f32 *, 0xC);
                    temp_v0 = (spC4 * 4) + var_a0_5;
                    var_t0_6 += 1;
                    M2C_FIELD(var_a0_5, f32 *, 0xC) = (f32) *temp_v0;
                    *temp_v0 = temp_f2_2;
                    var_a0_5 += 0x10;
                } while (var_t0_6 < 4);
            }
        }
        return 1;
    }
block_16:
    return 0;
}

u8 *func_80005D40(u8 *arg0, u8 *arg1, s32 arg2) {
    s32 var_a2;
    u32 temp_a3;
    u8 *var_a0;
    u8 *var_a1;
    u8 temp_v0;
    u8 var_t0;

    var_a0 = arg0;
    var_a1 = arg1;
    var_a2 = arg2;
    if (var_a2 != 0) {
        do {
            var_t0 = *var_a1;
            temp_a3 = var_t0 & 0xFF;
            var_a1 += 1;
            if (temp_a3 < 0x80U) {
                if (((temp_a3 < 0x20U) | (temp_a3 == 0x7F)) != 0) {
                    var_t0 = 0x2E;
                }
                goto block_10;
            }
            if ((var_a2 != 1) && ((u32) ((var_t0 + 0x60) & 0xFF) < 0x5FU) && ((u32) ((*var_a1 + 0x60) & 0xFF) < 0x5FU)) {
                M2C_FIELD(var_a0, u8 *, 0) = var_t0;
                temp_v0 = *var_a1;
                var_a1 += 1;
                var_a2 -= 2;
                M2C_FIELD(var_a0, u8 *, 1) = temp_v0;
                var_a0 = var_a0 + 1 + 1;
            } else {
                var_t0 = 0x2E;
block_10:
                M2C_FIELD(var_a0, u8 *, 0) = var_t0;
                var_a0 += 1;
                var_a2 -= 1;
            }
        } while (var_a2 != 0);
    }
    *var_a0 = 0;
    return var_a0;
}

void func_80005DF4(s8 *arg0, u8 *arg1, s32 arg2, s32 arg3) {
    s32 var_s0;
    s32 var_s0_2;
    s32 var_s1;
    s8 *var_v0;
    u8 *var_s2;
    u8 temp_a2;

    var_s1 = arg2;
    var_s2 = arg1;
    var_v0 = arg0;
    if (var_s1 > 0) {
        do {
            var_s0 = arg3;
            if (var_s0 == 0) {
                var_s0 = 1;
            }
            var_s0_2 = var_s0 - 1;
            if (var_s0_2 != -1) {
                do {
                    temp_a2 = *var_s2;
                    var_s2 += 1;
                    var_s1 -= 1;
                    var_s0_2 -= 1;
                    var_v0 = func_80001068(var_v0, "%02x", (s32) temp_a2);
                } while (var_s0_2 != -1);
            }
            var_v0 = func_80001068(var_v0, " ");
        } while (var_s1 > 0);
    }
    *var_v0 = 0;
}

void func_80005EB0(void) {

}

void func_80005EC0(u8 *arg0, u8 *arg1, s32 arg2) {
    s32 var_a2;
    u8 *var_a0;
    u8 *var_a1;
    u8 var_v1;

    var_a0 = arg0;
    var_a1 = arg1;
    var_a2 = arg2;
    if (var_a2 != 0) {
        do {
            var_v1 = *var_a1;
            var_a1 += 1;
            if ((u32) (var_v1 - 0x20) >= 0x5FU) {
                var_v1 = 0x2E;
            }
            *var_a0 = var_v1;
            var_a2 -= 1;
            var_a0 += 1;
        } while (var_a2 != 0);
    }
    *var_a0 = 0;
}

void func_80005EF8(s8 *arg0, u8 *arg1, s32 arg2) {
    s32 var_s0;
    s8 *var_v0;
    u8 *var_s1;
    u8 temp_a2;

    var_s1 = arg1;
    var_v0 = arg0;
    var_s0 = arg2;
    if (var_s0 > 0) {
        do {
            temp_a2 = *var_s1;
            var_s1 += 1;
            var_s0 -= 1;
            var_v0 = func_80001068(var_v0, "%02x ", (s32) temp_a2);
        } while (var_s0 > 0);
    }
}

void func_80005F50(s8 *arg0, s32 *arg1, s32 arg2) {
    s32 *var_s1;
    s32 temp_a2;
    s32 var_s0;
    s8 *var_v0;

    var_s1 = arg1;
    var_v0 = arg0;
    var_s0 = arg2;
    if (var_s0 > 0) {
        do {
            temp_a2 = *var_s1;
            var_s1 += 4;
            var_s0 -= 4;
            var_v0 = func_80001068(var_v0, "%08x ", temp_a2);
        } while (var_s0 > 0);
    }
}

void func_80005FA8(void) {

}

void func_80005FB0(void) {

}

void func_80005FC0(f32 *arg0, f32 *arg1, s32 arg2) {
    f32 *var_a0;
    f32 *var_a1;
    f32 *var_a3;
    f32 *var_t2;
    f32 temp_f2;
    f32 var_f4;
    s32 var_t0;
    s32 var_t1;
    s32 var_t4;
    s32 var_v1;

    var_a0 = arg0;
    var_a1 = arg1;
    var_t4 = 0;
    do {
        var_t2 = var_a0;
        var_t1 = 0;
loop_2:
        var_f4 = 0.0f;
        var_t0 = 0;
        var_a3 = var_a1;
        var_v1 = arg2;
loop_3:
        temp_f2 = *var_a3 * *(var_t1 + var_v1);
        var_a3 += 4;
        var_v1 += 0x10;
        var_t0 += 1;
        var_f4 += temp_f2;
        if (var_t0 < 4) {
            goto loop_3;
        }
        *var_t2 = var_f4;
        var_t1 += 4;
        var_t2 += 4;
        if (var_t1 < 0x10) {
            goto loop_2;
        }
        var_a0 += 0x10;
        var_t4 += 1;
        var_a1 += 0x10;
    } while (var_t4 < 4);
}

void Mtx_Mul(s32 arg0, s32 arg1, s32 arg2) {
    f32 sp0;
    f32 sp4;
    f32 sp8;
    f32 spC;
    f32 sp10;
    f32 sp14;
    f32 sp18;
    f32 sp1C;
    f32 sp20;
    f32 sp24;
    f32 sp28;
    f32 sp2C;
    f32 sp30;
    f32 sp34;
    f32 sp38;
    f32 sp3C;
    f32 temp_f2;
    f32 temp_f2_10;
    f32 temp_f2_11;
    f32 temp_f2_12;
    f32 temp_f2_13;
    f32 temp_f2_14;
    f32 temp_f2_15;
    f32 temp_f2_16;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 temp_f2_4;
    f32 temp_f2_5;
    f32 temp_f2_6;
    f32 temp_f2_7;
    f32 temp_f2_8;
    f32 temp_f2_9;
    f32 temp_f6;
    f32 temp_f6_2;
    f32 temp_f6_3;
    f32 temp_f6_4;
    f32 temp_f6_5;
    f32 temp_f6_6;
    f32 temp_f6_7;
    f32 temp_f6_8;
    f32 temp_f8;
    f32 temp_f8_2;
    f32 temp_f8_3;
    f32 temp_f8_4;
    f32 temp_f8_5;
    f32 temp_f8_6;
    f32 temp_f8_7;
    f32 temp_f8_8;

    if ((arg0 != arg2) & (arg0 != arg1)) {
        temp_f2 = M2C_FIELD(arg1, f32 *, 0);
        temp_f8 = M2C_FIELD(arg1, f32 *, 8);
        temp_f2_2 = M2C_FIELD(arg1, f32 *, 4);
        temp_f6 = M2C_FIELD(arg1, f32 *, 0xC);
        M2C_FIELD(arg0, f32 *, 0) = (f32) ((temp_f2 * M2C_FIELD(arg2, f32 *, 0)) + (temp_f2_2 * M2C_FIELD(arg2, f32 *, 0x10)) + (temp_f8 * M2C_FIELD(arg2, f32 *, 0x20)) + (temp_f6 * M2C_FIELD(arg2, f32 *, 0x30)));
        M2C_FIELD(arg0, f32 *, 4) = (f32) ((temp_f2 * M2C_FIELD(arg2, f32 *, 4)) + (temp_f2_2 * M2C_FIELD(arg2, f32 *, 0x14)) + (temp_f8 * M2C_FIELD(arg2, f32 *, 0x24)) + (temp_f6 * M2C_FIELD(arg2, f32 *, 0x34)));
        M2C_FIELD(arg0, f32 *, 8) = (f32) ((temp_f2 * M2C_FIELD(arg2, f32 *, 8)) + (temp_f2_2 * M2C_FIELD(arg2, f32 *, 0x18)) + (temp_f8 * M2C_FIELD(arg2, f32 *, 0x28)) + (temp_f6 * M2C_FIELD(arg2, f32 *, 0x38)));
        M2C_FIELD(arg0, f32 *, 0xC) = (f32) ((temp_f2 * M2C_FIELD(arg2, f32 *, 0xC)) + (temp_f2_2 * M2C_FIELD(arg2, f32 *, 0x1C)) + (temp_f8 * M2C_FIELD(arg2, f32 *, 0x2C)) + (temp_f6 * M2C_FIELD(arg2, f32 *, 0x3C)));
        temp_f2_3 = M2C_FIELD(arg1, f32 *, 0x10);
        temp_f8_2 = M2C_FIELD(arg1, f32 *, 0x18);
        temp_f2_4 = M2C_FIELD(arg1, f32 *, 0x14);
        temp_f6_2 = M2C_FIELD(arg1, f32 *, 0x1C);
        M2C_FIELD(arg0, f32 *, 0x10) = (f32) ((temp_f2_3 * M2C_FIELD(arg2, f32 *, 0)) + (temp_f2_4 * M2C_FIELD(arg2, f32 *, 0x10)) + (temp_f8_2 * M2C_FIELD(arg2, f32 *, 0x20)) + (temp_f6_2 * M2C_FIELD(arg2, f32 *, 0x30)));
        M2C_FIELD(arg0, f32 *, 0x14) = (f32) ((temp_f2_3 * M2C_FIELD(arg2, f32 *, 4)) + (temp_f2_4 * M2C_FIELD(arg2, f32 *, 0x14)) + (temp_f8_2 * M2C_FIELD(arg2, f32 *, 0x24)) + (temp_f6_2 * M2C_FIELD(arg2, f32 *, 0x34)));
        M2C_FIELD(arg0, f32 *, 0x18) = (f32) ((temp_f2_3 * M2C_FIELD(arg2, f32 *, 8)) + (temp_f2_4 * M2C_FIELD(arg2, f32 *, 0x18)) + (temp_f8_2 * M2C_FIELD(arg2, f32 *, 0x28)) + (temp_f6_2 * M2C_FIELD(arg2, f32 *, 0x38)));
        M2C_FIELD(arg0, f32 *, 0x1C) = (f32) ((temp_f2_3 * M2C_FIELD(arg2, f32 *, 0xC)) + (temp_f2_4 * M2C_FIELD(arg2, f32 *, 0x1C)) + (temp_f8_2 * M2C_FIELD(arg2, f32 *, 0x2C)) + (temp_f6_2 * M2C_FIELD(arg2, f32 *, 0x3C)));
        temp_f2_5 = M2C_FIELD(arg1, f32 *, 0x20);
        temp_f8_3 = M2C_FIELD(arg1, f32 *, 0x28);
        temp_f2_6 = M2C_FIELD(arg1, f32 *, 0x24);
        temp_f6_3 = M2C_FIELD(arg1, f32 *, 0x2C);
        M2C_FIELD(arg0, f32 *, 0x20) = (f32) ((temp_f2_5 * M2C_FIELD(arg2, f32 *, 0)) + (temp_f2_6 * M2C_FIELD(arg2, f32 *, 0x10)) + (temp_f8_3 * M2C_FIELD(arg2, f32 *, 0x20)) + (temp_f6_3 * M2C_FIELD(arg2, f32 *, 0x30)));
        M2C_FIELD(arg0, f32 *, 0x24) = (f32) ((temp_f2_5 * M2C_FIELD(arg2, f32 *, 4)) + (temp_f2_6 * M2C_FIELD(arg2, f32 *, 0x14)) + (temp_f8_3 * M2C_FIELD(arg2, f32 *, 0x24)) + (temp_f6_3 * M2C_FIELD(arg2, f32 *, 0x34)));
        M2C_FIELD(arg0, f32 *, 0x28) = (f32) ((temp_f2_5 * M2C_FIELD(arg2, f32 *, 8)) + (temp_f2_6 * M2C_FIELD(arg2, f32 *, 0x18)) + (temp_f8_3 * M2C_FIELD(arg2, f32 *, 0x28)) + (temp_f6_3 * M2C_FIELD(arg2, f32 *, 0x38)));
        M2C_FIELD(arg0, f32 *, 0x2C) = (f32) ((temp_f2_5 * M2C_FIELD(arg2, f32 *, 0xC)) + (temp_f2_6 * M2C_FIELD(arg2, f32 *, 0x1C)) + (temp_f8_3 * M2C_FIELD(arg2, f32 *, 0x2C)) + (temp_f6_3 * M2C_FIELD(arg2, f32 *, 0x3C)));
        temp_f2_7 = M2C_FIELD(arg1, f32 *, 0x30);
        temp_f8_4 = M2C_FIELD(arg1, f32 *, 0x38);
        temp_f2_8 = M2C_FIELD(arg1, f32 *, 0x34);
        temp_f6_4 = M2C_FIELD(arg1, f32 *, 0x3C);
        M2C_FIELD(arg0, f32 *, 0x30) = (f32) ((temp_f2_7 * M2C_FIELD(arg2, f32 *, 0)) + (temp_f2_8 * M2C_FIELD(arg2, f32 *, 0x10)) + (temp_f8_4 * M2C_FIELD(arg2, f32 *, 0x20)) + (temp_f6_4 * M2C_FIELD(arg2, f32 *, 0x30)));
        M2C_FIELD(arg0, f32 *, 0x34) = (f32) ((temp_f2_7 * M2C_FIELD(arg2, f32 *, 4)) + (temp_f2_8 * M2C_FIELD(arg2, f32 *, 0x14)) + (temp_f8_4 * M2C_FIELD(arg2, f32 *, 0x24)) + (temp_f6_4 * M2C_FIELD(arg2, f32 *, 0x34)));
        M2C_FIELD(arg0, f32 *, 0x38) = (f32) ((temp_f2_7 * M2C_FIELD(arg2, f32 *, 8)) + (temp_f2_8 * M2C_FIELD(arg2, f32 *, 0x18)) + (temp_f8_4 * M2C_FIELD(arg2, f32 *, 0x28)) + (temp_f6_4 * M2C_FIELD(arg2, f32 *, 0x38)));
        M2C_FIELD(arg0, f32 *, 0x3C) = (f32) ((temp_f2_7 * M2C_FIELD(arg2, f32 *, 0xC)) + (temp_f2_8 * M2C_FIELD(arg2, f32 *, 0x1C)) + (temp_f8_4 * M2C_FIELD(arg2, f32 *, 0x2C)) + (temp_f6_4 * M2C_FIELD(arg2, f32 *, 0x3C)));
        return;
    }
    temp_f2_9 = M2C_FIELD(arg1, f32 *, 0);
    temp_f8_5 = M2C_FIELD(arg1, f32 *, 8);
    temp_f2_10 = M2C_FIELD(arg1, f32 *, 4);
    temp_f6_5 = M2C_FIELD(arg1, f32 *, 0xC);
    sp0 = (temp_f2_9 * M2C_FIELD(arg2, f32 *, 0)) + (temp_f2_10 * M2C_FIELD(arg2, f32 *, 0x10)) + (temp_f8_5 * M2C_FIELD(arg2, f32 *, 0x20)) + (temp_f6_5 * M2C_FIELD(arg2, f32 *, 0x30));
    sp4 = (temp_f2_9 * M2C_FIELD(arg2, f32 *, 4)) + (temp_f2_10 * M2C_FIELD(arg2, f32 *, 0x14)) + (temp_f8_5 * M2C_FIELD(arg2, f32 *, 0x24)) + (temp_f6_5 * M2C_FIELD(arg2, f32 *, 0x34));
    sp8 = (temp_f2_9 * M2C_FIELD(arg2, f32 *, 8)) + (temp_f2_10 * M2C_FIELD(arg2, f32 *, 0x18)) + (temp_f8_5 * M2C_FIELD(arg2, f32 *, 0x28)) + (temp_f6_5 * M2C_FIELD(arg2, f32 *, 0x38));
    spC = (temp_f2_9 * M2C_FIELD(arg2, f32 *, 0xC)) + (temp_f2_10 * M2C_FIELD(arg2, f32 *, 0x1C)) + (temp_f8_5 * M2C_FIELD(arg2, f32 *, 0x2C)) + (temp_f6_5 * M2C_FIELD(arg2, f32 *, 0x3C));
    temp_f2_11 = M2C_FIELD(arg1, f32 *, 0x10);
    temp_f8_6 = M2C_FIELD(arg1, f32 *, 0x18);
    temp_f2_12 = M2C_FIELD(arg1, f32 *, 0x14);
    temp_f6_6 = M2C_FIELD(arg1, f32 *, 0x1C);
    sp10 = (temp_f2_11 * M2C_FIELD(arg2, f32 *, 0)) + (temp_f2_12 * M2C_FIELD(arg2, f32 *, 0x10)) + (temp_f8_6 * M2C_FIELD(arg2, f32 *, 0x20)) + (temp_f6_6 * M2C_FIELD(arg2, f32 *, 0x30));
    sp14 = (temp_f2_11 * M2C_FIELD(arg2, f32 *, 4)) + (temp_f2_12 * M2C_FIELD(arg2, f32 *, 0x14)) + (temp_f8_6 * M2C_FIELD(arg2, f32 *, 0x24)) + (temp_f6_6 * M2C_FIELD(arg2, f32 *, 0x34));
    sp18 = (temp_f2_11 * M2C_FIELD(arg2, f32 *, 8)) + (temp_f2_12 * M2C_FIELD(arg2, f32 *, 0x18)) + (temp_f8_6 * M2C_FIELD(arg2, f32 *, 0x28)) + (temp_f6_6 * M2C_FIELD(arg2, f32 *, 0x38));
    sp1C = (temp_f2_11 * M2C_FIELD(arg2, f32 *, 0xC)) + (temp_f2_12 * M2C_FIELD(arg2, f32 *, 0x1C)) + (temp_f8_6 * M2C_FIELD(arg2, f32 *, 0x2C)) + (temp_f6_6 * M2C_FIELD(arg2, f32 *, 0x3C));
    temp_f2_13 = M2C_FIELD(arg1, f32 *, 0x20);
    temp_f8_7 = M2C_FIELD(arg1, f32 *, 0x28);
    temp_f2_14 = M2C_FIELD(arg1, f32 *, 0x24);
    temp_f6_7 = M2C_FIELD(arg1, f32 *, 0x2C);
    sp20 = (temp_f2_13 * M2C_FIELD(arg2, f32 *, 0)) + (temp_f2_14 * M2C_FIELD(arg2, f32 *, 0x10)) + (temp_f8_7 * M2C_FIELD(arg2, f32 *, 0x20)) + (temp_f6_7 * M2C_FIELD(arg2, f32 *, 0x30));
    sp24 = (temp_f2_13 * M2C_FIELD(arg2, f32 *, 4)) + (temp_f2_14 * M2C_FIELD(arg2, f32 *, 0x14)) + (temp_f8_7 * M2C_FIELD(arg2, f32 *, 0x24)) + (temp_f6_7 * M2C_FIELD(arg2, f32 *, 0x34));
    sp28 = (temp_f2_13 * M2C_FIELD(arg2, f32 *, 8)) + (temp_f2_14 * M2C_FIELD(arg2, f32 *, 0x18)) + (temp_f8_7 * M2C_FIELD(arg2, f32 *, 0x28)) + (temp_f6_7 * M2C_FIELD(arg2, f32 *, 0x38));
    sp2C = (temp_f2_13 * M2C_FIELD(arg2, f32 *, 0xC)) + (temp_f2_14 * M2C_FIELD(arg2, f32 *, 0x1C)) + (temp_f8_7 * M2C_FIELD(arg2, f32 *, 0x2C)) + (temp_f6_7 * M2C_FIELD(arg2, f32 *, 0x3C));
    temp_f2_15 = M2C_FIELD(arg1, f32 *, 0x30);
    temp_f8_8 = M2C_FIELD(arg1, f32 *, 0x38);
    temp_f2_16 = M2C_FIELD(arg1, f32 *, 0x34);
    temp_f6_8 = M2C_FIELD(arg1, f32 *, 0x3C);
    sp30 = (temp_f2_15 * M2C_FIELD(arg2, f32 *, 0)) + (temp_f2_16 * M2C_FIELD(arg2, f32 *, 0x10)) + (temp_f8_8 * M2C_FIELD(arg2, f32 *, 0x20)) + (temp_f6_8 * M2C_FIELD(arg2, f32 *, 0x30));
    sp34 = (temp_f2_15 * M2C_FIELD(arg2, f32 *, 4)) + (temp_f2_16 * M2C_FIELD(arg2, f32 *, 0x14)) + (temp_f8_8 * M2C_FIELD(arg2, f32 *, 0x24)) + (temp_f6_8 * M2C_FIELD(arg2, f32 *, 0x34));
    sp38 = (temp_f2_15 * M2C_FIELD(arg2, f32 *, 8)) + (temp_f2_16 * M2C_FIELD(arg2, f32 *, 0x18)) + (temp_f8_8 * M2C_FIELD(arg2, f32 *, 0x28)) + (temp_f6_8 * M2C_FIELD(arg2, f32 *, 0x38));
    sp3C = (temp_f2_15 * M2C_FIELD(arg2, f32 *, 0xC)) + (temp_f2_16 * M2C_FIELD(arg2, f32 *, 0x1C)) + (temp_f8_8 * M2C_FIELD(arg2, f32 *, 0x2C)) + (temp_f6_8 * M2C_FIELD(arg2, f32 *, 0x3C));
    M2C_FIELD(arg0, f32 *, 0) = sp0;
    M2C_FIELD(arg0, f32 *, 4) = sp4;
    M2C_FIELD(arg0, f32 *, 8) = sp8;
    M2C_FIELD(arg0, f32 *, 0xC) = spC;
    M2C_FIELD(arg0, f32 *, 0x10) = sp10;
    M2C_FIELD(arg0, f32 *, 0x14) = sp14;
    M2C_FIELD(arg0, f32 *, 0x18) = sp18;
    M2C_FIELD(arg0, f32 *, 0x1C) = sp1C;
    M2C_FIELD(arg0, f32 *, 0x20) = sp20;
    M2C_FIELD(arg0, f32 *, 0x24) = sp24;
    M2C_FIELD(arg0, f32 *, 0x28) = sp28;
    M2C_FIELD(arg0, f32 *, 0x2C) = sp2C;
    M2C_FIELD(arg0, f32 *, 0x30) = sp30;
    M2C_FIELD(arg0, f32 *, 0x34) = sp34;
    M2C_FIELD(arg0, f32 *, 0x38) = sp38;
    M2C_FIELD(arg0, f32 *, 0x3C) = sp3C;
}

void Mtx_MulAffine(s32 arg0, s32 arg1, s32 arg2) {
    f32 *var_a3_2;
    f32 *var_t3_2;
    f32 *var_t3_3;
    f32 *var_v1_4;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;
    f32 temp_f4_2;
    s32 var_a0;
    s32 var_a1;
    s32 var_a2;
    s32 var_a3;
    s32 var_a3_3;
    s32 var_t0;
    s32 var_t0_2;
    s32 var_t0_3;
    s32 var_t1;
    s32 var_t1_2;
    s32 var_t1_3;
    s32 var_t1_4;
    s32 var_t1_5;
    s32 var_t1_6;
    s32 var_t2;
    s32 var_t3;
    s32 var_t4;
    s32 var_t4_2;
    s32 var_v1;
    s32 var_v1_2;
    s32 var_v1_3;
    void *var_v0;

    var_a0 = arg0;
    var_a1 = arg1;
    var_a2 = arg2;
    if ((var_a0 != var_a2) & (var_a0 != var_a1)) {
        var_t1 = 0;
        var_t4 = var_a1;
        var_t3 = var_a0;
        do {
            var_t0 = 0;
            var_a3 = var_t3;
            var_v1 = var_a2;
loop_3:
            temp_f4 = (M2C_FIELD(var_t4, f32 *, 0) * M2C_FIELD(var_v1, f32 *, 0)) + (M2C_FIELD(var_t4, f32 *, 4) * M2C_FIELD(var_v1, f32 *, 0x10)) + (M2C_FIELD(var_t4, f32 *, 8) * M2C_FIELD(var_v1, f32 *, 0x20));
            var_t0 += 1;
            var_v1 += 4;
            *var_a3 = temp_f4;
            var_a3 += 4;
            if (var_t0 < 3) {
                goto loop_3;
            }
            var_t4 += 0x10;
            var_t1 += 1;
            var_t3 += 0x10;
        } while (var_t1 < 3);
        var_t1_2 = 0;
        var_v1_2 = var_a0;
        do {
            temp_f2 = M2C_FIELD(var_a2, f32 *, 0x30);
            var_a2 += 4;
            temp_f0 = M2C_FIELD(var_a1, f32 *, 0x30) + temp_f2;
            var_a1 += 4;
            var_t1_2 += 1;
            M2C_FIELD(var_v1_2, f32 *, 0x30) = temp_f0;
            var_v1_2 += 4;
        } while (var_t1_2 < 3);
        var_t1_3 = 2;
        var_v0 = var_a0 + 0x20;
        do {
            M2C_FIELD(var_v0, s32 *, 0xC) = 0;
            var_t1_3 -= 1;
            var_v0 -= 0x10;
        } while (var_t1_3 >= 0);
        M2C_FIELD(var_a0, f32 *, 0x3C) = 1.0f;
        return;
    }
    var_t1_4 = 0;
    var_t4_2 = var_a1;
    var_t3_2 = sp;
    do {
        var_t0_2 = 0;
        var_a3_2 = var_t3_2;
        var_v1_3 = var_a2;
loop_12:
        temp_f4_2 = (M2C_FIELD(var_t4_2, f32 *, 0) * M2C_FIELD(var_v1_3, f32 *, 0)) + (M2C_FIELD(var_t4_2, f32 *, 4) * M2C_FIELD(var_v1_3, f32 *, 0x10)) + (M2C_FIELD(var_t4_2, f32 *, 8) * M2C_FIELD(var_v1_3, f32 *, 0x20));
        var_t0_2 += 1;
        var_v1_3 += 4;
        *var_a3_2 = temp_f4_2;
        var_a3_2 += 4;
        if (var_t0_2 < 3) {
            goto loop_12;
        }
        var_t4_2 += 0x10;
        var_t1_4 += 1;
        var_t3_2 += 0x10;
    } while (var_t1_4 < 3);
    var_t1_5 = 0;
    var_t3_3 = sp;
    var_t2 = var_a0;
    do {
        var_t0_3 = 0;
        var_a3_3 = var_t2;
        var_v1_4 = var_t3_3;
loop_16:
        temp_f0_2 = *var_v1_4;
        var_v1_4 += 4;
        var_t0_3 += 1;
        *var_a3_3 = temp_f0_2;
        var_a3_3 += 4;
        if (var_t0_3 < 3) {
            goto loop_16;
        }
        var_t3_3 += 0x10;
        var_t1_5 += 1;
        var_t2 += 0x10;
    } while (var_t1_5 < 3);
    var_t1_6 = 0;
    do {
        temp_f2_2 = M2C_FIELD(var_a2, f32 *, 0x30);
        var_a2 += 4;
        temp_f0_3 = M2C_FIELD(var_a1, f32 *, 0x30) + temp_f2_2;
        var_a1 += 4;
        var_t1_6 += 1;
        M2C_FIELD(var_a0, f32 *, 0x30) = temp_f0_3;
        var_a0 += 4;
    } while (var_t1_6 < 3);
}

void Mtx_TransformPoint(void *arg0, void *arg1, void *arg2) {
    f32 temp_f16;
    f32 temp_f2;
    f32 temp_f4;

    temp_f2 = M2C_FIELD(arg1, f32 *, 0);
    temp_f16 = M2C_FIELD(arg1, f32 *, 4);
    temp_f4 = M2C_FIELD(arg1, f32 *, 8);
    M2C_FIELD(arg0, f32 *, 0) = (f32) ((temp_f2 * M2C_FIELD(arg2, f32 *, 0)) + (temp_f16 * M2C_FIELD(arg2, f32 *, 0x10)) + (temp_f4 * M2C_FIELD(arg2, f32 *, 0x20)) + M2C_FIELD(arg2, f32 *, 0x30));
    M2C_FIELD(arg0, f32 *, 4) = (f32) ((temp_f2 * M2C_FIELD(arg2, f32 *, 4)) + (temp_f16 * M2C_FIELD(arg2, f32 *, 0x14)) + (temp_f4 * M2C_FIELD(arg2, f32 *, 0x24)) + M2C_FIELD(arg2, f32 *, 0x34));
    M2C_FIELD(arg0, f32 *, 8) = (f32) ((temp_f2 * M2C_FIELD(arg2, f32 *, 8)) + (temp_f16 * M2C_FIELD(arg2, f32 *, 0x18)) + (temp_f4 * M2C_FIELD(arg2, f32 *, 0x28)) + M2C_FIELD(arg2, f32 *, 0x38));
}

void Mtx_TransformVector(f32 *arg0, f32 *arg1, f32 *arg2) {
    f32 temp_f16;
    f32 temp_f2;
    f32 temp_f4;

    temp_f2 = M2C_FIELD(arg1, f32 *, 0);
    temp_f16 = M2C_FIELD(arg1, f32 *, 4);
    temp_f4 = M2C_FIELD(arg1, f32 *, 8);
    M2C_FIELD(arg0, f32 *, 0) = (temp_f2 * M2C_FIELD(arg2, f32 *, 0)) + (temp_f16 * arg2[4]) + (temp_f4 * arg2[8]);
    M2C_FIELD(arg0, f32 *, 4) = (f32) ((temp_f2 * arg2[1]) + (temp_f16 * arg2[5]) + (temp_f4 * arg2[9]));
    M2C_FIELD(arg0, f32 *, 8) = (f32) ((temp_f2 * arg2[2]) + (temp_f16 * arg2[6]) + (temp_f4 * arg2[0xA]));
}

s32 Mtx_ProjectOntoPlane(void *arg0, void *arg1) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 temp_f2_4;
    f32 temp_f2_5;
    f32 temp_f6;
    f32 temp_f6_2;
    f32 temp_f8;
    s32 var_v0;

    temp_f0 = M2C_FIELD(arg1, f32 *, 0);
    temp_f2 = M2C_FIELD(arg1, f32 *, 4);
    temp_f8 = temp_f2 * temp_f2;
    temp_f2_2 = M2C_FIELD(arg1, f32 *, 8);
    temp_f2_3 = temp_f2_2 * temp_f2_2;
    temp_f6 = (temp_f0 * temp_f0) + temp_f8 + temp_f2_3;
    var_v0 = 0;
    if (temp_f6 != 0.0f) {
        temp_f6_2 = 1.0f / temp_f6;
        M2C_FIELD(arg0, f32 *, 0) = (f32) ((temp_f8 + temp_f2_3) * temp_f6_2);
        M2C_FIELD(arg0, f32 *, 0x10) = (f32) (-M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg1, f32 *, 4) * temp_f6_2);
        M2C_FIELD(arg0, f32 *, 0x20) = (f32) (-M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg1, f32 *, 8) * temp_f6_2);
        M2C_FIELD(arg0, f32 *, 0x30) = (f32) (-M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg1, f32 *, 0xC) * temp_f6_2);
        M2C_FIELD(arg0, f32 *, 4) = (f32) (-M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg1, f32 *, 4) * temp_f6_2);
        temp_f2_4 = M2C_FIELD(arg1, f32 *, 0);
        temp_f0_2 = M2C_FIELD(arg1, f32 *, 8);
        M2C_FIELD(arg0, f32 *, 0x14) = (f32) (((temp_f2_4 * temp_f2_4) + (temp_f0_2 * temp_f0_2)) * temp_f6_2);
        M2C_FIELD(arg0, f32 *, 0x24) = (f32) (-M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg1, f32 *, 8) * temp_f6_2);
        M2C_FIELD(arg0, f32 *, 0x34) = (f32) (-M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg1, f32 *, 0xC) * temp_f6_2);
        M2C_FIELD(arg0, f32 *, 8) = (f32) (-M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg1, f32 *, 8) * temp_f6_2);
        M2C_FIELD(arg0, f32 *, 0x18) = (f32) (-M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg1, f32 *, 8) * temp_f6_2);
        temp_f2_5 = M2C_FIELD(arg1, f32 *, 0);
        temp_f0_3 = M2C_FIELD(arg1, f32 *, 4);
        M2C_FIELD(arg0, f32 *, 0x28) = (f32) (((temp_f2_5 * temp_f2_5) + (temp_f0_3 * temp_f0_3)) * temp_f6_2);
        var_v0 = 1;
        M2C_FIELD(arg0, f32 *, 0x38) = (f32) (-M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg1, f32 *, 0xC) * temp_f6_2);
        M2C_FIELD(arg0, f32 *, 0xC) = 0.0f;
        M2C_FIELD(arg0, f32 *, 0x1C) = 0.0f;
        M2C_FIELD(arg0, f32 *, 0x2C) = 0.0f;
        M2C_FIELD(arg0, f32 *, 0x3C) = 1.0f;
    }
    return var_v0;
}

void func_80006C10(void *arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7, f32 arg8, f32 arg9) {
    f32 sp14;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f10;
    f32 temp_f12;
    f32 temp_f12_2;
    f32 temp_f12_3;
    f32 temp_f18;
    f32 temp_f20;
    f32 temp_f20_2;
    f32 temp_f22;
    f32 temp_f22_2;
    f32 temp_f24;
    f32 temp_f24_2;
    f32 temp_f26;
    f32 temp_f26_2;
    f32 temp_f28;
    f32 temp_f28_2;
    f32 temp_f30;
    f32 temp_f30_2;
    f32 temp_f8;
    f32 var_f4;
    f32 var_f6;

    temp_f30 = arg4 - arg1;
    temp_f26 = arg5 - arg2;
    temp_f28 = arg6 - arg3;
    temp_f12 = (temp_f30 * temp_f30) + (temp_f26 * temp_f26) + (temp_f28 * temp_f28);
    var_f6 = sqrtf(temp_f12);
    if (var_f6 != var_f6) {
        var_f6 = sqrtf(temp_f12);
    }
    temp_f12_2 = (arg7 * arg7) + (arg8 * arg8) + (arg9 * arg9);
    var_f4 = sqrtf(temp_f12_2);
    temp_f0 = (f32) (-1.0 / (f64) var_f6);
    temp_f30_2 = temp_f30 * temp_f0;
    temp_f26_2 = temp_f26 * temp_f0;
    temp_f28_2 = temp_f28 * temp_f0;
    if (var_f4 != var_f4) {
        var_f4 = sqrtf(temp_f12_2);
    }
    temp_f0_2 = (f32) (1.0 / (f64) var_f4);
    temp_f24 = arg7 * temp_f0_2;
    temp_f20 = arg8 * temp_f0_2;
    temp_f22 = arg9 * temp_f0_2;
    temp_f12_3 = (temp_f24 * temp_f26_2) - (temp_f20 * temp_f30_2);
    temp_f8 = (temp_f22 * temp_f30_2) - (temp_f24 * temp_f28_2);
    temp_f10 = (temp_f20 * temp_f28_2) - (temp_f22 * temp_f26_2);
    temp_f18 = arg3 * temp_f12_3;
    temp_f24_2 = (temp_f26_2 * temp_f12_3) - (temp_f28_2 * temp_f8);
    sp14 = temp_f18;
    temp_f20_2 = (temp_f28_2 * temp_f10) - (temp_f30_2 * temp_f12_3);
    temp_f22_2 = (temp_f30_2 * temp_f8) - (temp_f26_2 * temp_f10);
    M2C_FIELD(arg0, f32 *, 8) = temp_f30_2;
    M2C_FIELD(arg0, f32 *, 0x18) = temp_f26_2;
    M2C_FIELD(arg0, f32 *, 0x28) = temp_f28_2;
    M2C_FIELD(arg0, s32 *, 0xC) = 0;
    M2C_FIELD(arg0, s32 *, 0x1C) = 0;
    M2C_FIELD(arg0, s32 *, 0x2C) = 0;
    M2C_FIELD(arg0, f32 *, 0) = temp_f10;
    M2C_FIELD(arg0, f32 *, 0x10) = temp_f8;
    M2C_FIELD(arg0, f32 *, 0x20) = temp_f12_3;
    M2C_FIELD(arg0, f32 *, 4) = temp_f24_2;
    M2C_FIELD(arg0, f32 *, 0x14) = temp_f20_2;
    M2C_FIELD(arg0, f32 *, 0x24) = temp_f22_2;
    M2C_FIELD(arg0, f32 *, 0x30) = (f32) -((arg1 * temp_f10) + (arg2 * temp_f8) + temp_f18);
    M2C_FIELD(arg0, f32 *, 0x3C) = 1.0f;
    M2C_FIELD(arg0, f32 *, 0x38) = (f32) -((arg1 * temp_f30_2) + (arg2 * temp_f26_2) + (arg3 * temp_f28_2));
    M2C_FIELD(arg0, f32 *, 0x34) = (f32) -((arg1 * temp_f24_2) + (arg2 * temp_f20_2) + (arg3 * temp_f22_2));
}

void func_80006EA8(void *arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    f32 temp_f0;
    f32 temp_f22;
    f32 temp_f22_2;
    f32 temp_f22_3;
    f32 temp_f26;

    Mtx_Identity();
    temp_f22 = (arg1 * 0.017453292f) / 2.0f;
    temp_f22_2 = func_8001C450(temp_f22);
    temp_f22_3 = temp_f22_2 / func_8001C5A0(temp_f22);
    temp_f0 = arg3 - arg4;
    temp_f26 = temp_f22_3 / arg2;
    M2C_FIELD(arg0, s32 *, 0x3C) = 0;
    M2C_FIELD(arg0, f32 *, 0x2C) = -1.0f;
    M2C_FIELD(arg0, f32 *, 0x14) = temp_f22_3;
    M2C_FIELD(arg0, f32 *, 0x28) = (f32) ((arg3 + arg4) / temp_f0);
    M2C_FIELD(arg0, f32 *, 0x38) = (f32) ((2.0f * arg3 * arg4) / temp_f0);
    M2C_FIELD(arg0, f32 *, 0) = temp_f26;
}

void func_80006F74(f32 *arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5, f32 arg6, f32 arg7) {
    f32 *var_s0;
    f32 *var_v1;
    f32 temp_f0;
    f32 temp_f6;
    f32 temp_f8;
    s32 var_a0;
    s32 var_a1;

    var_s0 = arg0;
    Mtx_Identity();
    temp_f0 = arg2 - arg1;
    temp_f6 = arg4 - arg3;
    temp_f8 = arg6 - arg5;
    var_a1 = 0;
    M2C_FIELD(var_s0, f32 *, 0x3C) = 1.0f;
    M2C_FIELD(var_s0, f32 *, 0) = 2.0f / temp_f0;
    M2C_FIELD(var_s0, f32 *, 0x30) = (f32) (-(arg2 + arg1) / temp_f0);
    M2C_FIELD(var_s0, f32 *, 0x14) = (f32) (2.0f / temp_f6);
    M2C_FIELD(var_s0, f32 *, 0x28) = (f32) (-2.0f / temp_f8);
    M2C_FIELD(var_s0, f32 *, 0x34) = (f32) (-(arg4 + arg3) / temp_f6);
    M2C_FIELD(var_s0, f32 *, 0x38) = (f32) (-(arg6 + arg5) / temp_f8);
    do {
        var_a0 = 0;
        var_v1 = var_s0;
loop_2:
        var_a0 += 1;
        *var_v1 *= arg7;
        var_v1 += 4;
        if (var_a0 < 4) {
            goto loop_2;
        }
        var_a1 += 1;
        var_s0 += 0x10;
    } while (var_a1 < 4);
}

void func_800070A4(void *arg0, void *arg1, f32 arg2, f32 arg3) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f10;
    f32 temp_f10_2;
    f32 temp_f10_3;
    f32 temp_f10_4;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 temp_f8;

    Mtx_Identity();
    temp_f0 = M2C_FIELD(arg1, f32 *, 0);
    temp_f2 = M2C_FIELD(arg1, f32 *, 4);
    temp_f4 = M2C_FIELD(arg1, f32 *, 8);
    temp_f10 = 1.0f - arg3;
    temp_f8 = temp_f0 * temp_f2 * temp_f10;
    temp_f2_2 = temp_f2 * temp_f4 * temp_f10;
    temp_f4_2 = temp_f4 * temp_f0 * temp_f10;
    temp_f10_2 = temp_f0 * temp_f0;
    M2C_FIELD(arg0, f32 *, 0) = (f32) (temp_f10_2 + (arg3 * (1.0f - temp_f10_2)));
    M2C_FIELD(arg0, f32 *, 0x24) = (f32) (temp_f2_2 - (M2C_FIELD(arg1, f32 *, 0) * arg2));
    M2C_FIELD(arg0, f32 *, 0x18) = (f32) (temp_f2_2 + (M2C_FIELD(arg1, f32 *, 0) * arg2));
    temp_f0_2 = M2C_FIELD(arg1, f32 *, 4);
    temp_f10_3 = temp_f0_2 * temp_f0_2;
    M2C_FIELD(arg0, f32 *, 0x14) = (f32) (temp_f10_3 + (arg3 * (1.0f - temp_f10_3)));
    M2C_FIELD(arg0, f32 *, 0x20) = (f32) (temp_f4_2 + (M2C_FIELD(arg1, f32 *, 4) * arg2));
    M2C_FIELD(arg0, f32 *, 8) = (f32) (temp_f4_2 - (M2C_FIELD(arg1, f32 *, 4) * arg2));
    temp_f0_3 = M2C_FIELD(arg1, f32 *, 8);
    temp_f10_4 = temp_f0_3 * temp_f0_3;
    M2C_FIELD(arg0, f32 *, 0x28) = (f32) (temp_f10_4 + (arg3 * (1.0f - temp_f10_4)));
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) (temp_f8 - (M2C_FIELD(arg1, f32 *, 8) * arg2));
    M2C_FIELD(arg0, f32 *, 4) = (f32) (temp_f8 + (M2C_FIELD(arg1, f32 *, 8) * arg2));
}

void func_800071CC(M2C_UNK *arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    f32 temp_f0;
    f32 temp_f10;
    f32 temp_f16;
    f32 temp_f20;
    f32 temp_f20_2;
    f32 temp_f22;
    f32 temp_f22_2;
    f32 temp_f22_3;
    f32 temp_f22_4;
    f32 temp_f2;
    f32 temp_f4;
    f32 temp_f6;
    f32 temp_f8;

    Mtx_Identity();
    Vec3_NormalizeXYZ(&arg2, &arg3, &arg4);
    temp_f20 = arg1 * 0.017453292f;
    temp_f20_2 = func_8001C5A0(temp_f20);
    temp_f0 = func_8001C450(temp_f20);
    temp_f22 = 1.0f - temp_f0;
    temp_f16 = arg2 * arg3 * temp_f22;
    temp_f10 = arg3 * arg4 * temp_f22;
    temp_f8 = arg4 * arg2 * temp_f22;
    temp_f22_2 = arg2 * arg2;
    M2C_FIELD(arg0, f32 *, 0) = (f32) (temp_f22_2 + (temp_f0 * (1.0f - temp_f22_2)));
    temp_f22_3 = arg3 * arg3;
    temp_f4 = arg3 * temp_f20_2;
    temp_f6 = arg2 * temp_f20_2;
    M2C_FIELD(arg0, f32 *, 0x20) = (f32) (temp_f8 + temp_f4);
    temp_f22_4 = arg4 * arg4;
    temp_f2 = arg4 * temp_f20_2;
    M2C_FIELD(arg0, f32 *, 0x24) = (f32) (temp_f10 - temp_f6);
    M2C_FIELD(arg0, f32 *, 0x18) = (f32) (temp_f10 + temp_f6);
    M2C_FIELD(arg0, f32 *, 8) = (f32) (temp_f8 - temp_f4);
    M2C_FIELD(arg0, f32 *, 0x14) = (f32) (temp_f22_3 + (temp_f0 * (1.0f - temp_f22_3)));
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) (temp_f16 - temp_f2);
    M2C_FIELD(arg0, f32 *, 4) = (f32) (temp_f16 + temp_f2);
    M2C_FIELD(arg0, f32 *, 0x28) = (f32) (temp_f22_4 + (temp_f0 * (1.0f - temp_f22_4)));
}

void Mtx_Translate(f32 *arg0, f32 arg1, f32 arg2, M2C_UNK32 arg3) {
    Mtx_Identity();
    M2C_FIELD(arg0, f32 *, 0x30) = arg1;
    M2C_FIELD(arg0, f32 *, 0x34) = arg2;
    M2C_FIELD(arg0, M2C_UNK32 *, 0x38) = arg3;
}

void Mtx_Scale(M2C_UNK *arg0, M2C_UNK32 arg1, M2C_UNK32 arg2, M2C_UNK32 arg3) {
    Mtx_Identity();
    M2C_FIELD(arg0, M2C_UNK32 *, 0) = arg1;
    M2C_FIELD(arg0, M2C_UNK32 *, 0x14) = arg2;
    M2C_FIELD(arg0, M2C_UNK32 *, 0x28) = arg3;
}

s32 func_800073B0(f32 *arg1) {
    f32 *var_a2;
    s32 var_v1;

    var_v1 = 0;
    var_a2 = arg1;
loop_1:
    if (*var_a2 != 0.0f) {
        var_v1 += 1;
        var_a2 += 0x14;
        if (var_v1 >= 4) {

        } else {
            goto loop_1;
        }
    }
    if (var_v1 != 4) {
        return func_80007558(var_a2);
    }
    func_80007420(var_a2);
    return 1;
}

void func_80007420(f32 *arg0, f32 *arg1) {
    f32 *temp_v0;
    f32 *var_a1;
    f32 *var_a1_2;
    f32 *var_a3;
    f32 *var_a3_2;
    f32 *var_t1;
    f32 *var_t1_2;
    f32 *var_t3;
    f32 *var_v1;
    f32 *var_v1_2;
    f32 *var_v1_3;
    f32 temp_f0;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;
    s32 var_a2;
    s32 var_a2_2;
    s32 var_a2_3;
    s32 var_t0;
    s32 var_t0_2;
    s32 var_t2;
    s32 var_t6;

    var_a1 = arg1;
    var_t1 = arg0;
    if (arg0 != var_a1) {
        var_t0 = 0;
        do {
            var_a2 = 0;
            var_a3 = var_t1;
            var_v1 = var_a1;
loop_3:
            temp_f0 = *var_v1;
            var_v1 += 4;
            var_a2 += 1;
            *var_a3 = temp_f0;
            var_a3 += 4;
            if (var_a2 < 4) {
                goto loop_3;
            }
            var_a1 += 0x10;
            var_t0 += 1;
            var_t1 += 0x10;
        } while (var_t0 < 4);
    }
    var_t2 = 0;
    var_t1_2 = arg0;
    var_t6 = 0;
    var_t3 = var_t1_2;
    do {
        temp_f2 = *var_t3;
        *var_t3 = 1.0f;
        if (temp_f2 != 0.0f) {
            var_a2_2 = 0;
            var_v1_2 = var_t1_2;
            do {
                var_a2_2 += 1;
                *var_v1_2 *= (f32) (1.0 / (f64) temp_f2);
                var_v1_2 += 4;
            } while (var_a2_2 < 4);
        }
        var_t0_2 = 0;
        var_a3_2 = arg0;
loop_10:
        temp_v0 = var_t6 + var_a3_2;
        if (var_t0_2 != var_t2) {
            var_a2_3 = 0;
            temp_f4 = *temp_v0;
            var_v1_3 = var_a3_2;
            var_a1_2 = var_t1_2;
            *temp_v0 = 0.0f;
            do {
                temp_f2_2 = *var_v1_3 - (temp_f4 * *var_a1_2);
                var_a1_2 += 4;
                var_a2_3 += 1;
                *var_v1_3 = temp_f2_2;
                var_v1_3 += 4;
            } while (var_a2_3 < 4);
        }
        var_t0_2 += 1;
        var_a3_2 += 0x10;
        if (var_t0_2 < 4) {
            goto loop_10;
        }
        var_t1_2 += 0x10;
        var_t6 += 4;
        var_t2 += 1;
        var_t3 += 0x14;
    } while (var_t2 < 4);
}

s32 func_80007558(f32 *arg0, s32 arg1) {
    M2C_UNK sp20;
    M2C_UNK *var_t5_3;
    f32 *var_a1;
    f32 *var_a1_2;
    f32 *var_a1_3;
    f32 *var_a3_3;
    f32 *var_t0_2;
    f32 *var_t0_3;
    f32 *var_t0_4;
    f32 *var_t1_2;
    f32 *var_t4_3;
    f32 *var_t5;
    f32 *var_t5_2;
    f32 *var_v1_2;
    f32 *var_v1_3;
    f32 *var_v1_4;
    f32 *var_v1_5;
    f32 *var_v1_6;
    f32 *var_v1_7;
    f32 temp_f0;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 temp_f4;
    f32 var_f0;
    s32 *var_a3_2;
    s32 *var_t3;
    s32 *var_t4;
    s32 *var_t4_2;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 var_a2;
    s32 var_a2_2;
    s32 var_a2_3;
    s32 var_a2_4;
    s32 var_a2_5;
    s32 var_a2_6;
    s32 var_a2_7;
    s32 var_a2_8;
    s32 var_a2_9;
    s32 var_a3_5;
    s32 var_t1;
    s32 var_t2;
    s32 var_t2_2;
    s32 var_t2_3;
    s32 var_t3_2;
    s32 var_t3_3;
    u8 *var_a3;
    u8 *var_t0;
    u8 *var_v1;
    u8 temp_v0;
    void *temp_v1;
    void *temp_v1_2;
    void *var_a3_4;

    var_t2 = 0;
    var_t0 = D_800383A0;
loop_1:
    var_a2 = 0;
    var_a3 = var_t0;
loop_2:
    if (*((var_a2 * 4) + ((*var_a3 * 0x10) + arg1)) != 0.0f) {
        var_a2 += 1;
        var_a3 += 1;
        if (var_a2 >= 4) {

        } else {
            goto loop_2;
        }
    }
    if (var_a2 != 4) {
        var_t2 += 1;
        var_t0 += 4;
        if (var_t2 >= 0x18) {

        } else {
            goto loop_1;
        }
    } else {
        var_a2_2 = 0;
        var_a3_2 = sp;
        var_v1 = var_t0;
        do {
            temp_v0 = *var_v1;
            var_v1 += 1;
            var_a2_2 += 1;
            *var_a3_2 = (s32) temp_v0;
            var_a3_2 += 4;
        } while (var_a2_2 < 4);
    }
    var_t2_2 = 0;
    if (var_t2 == 0x18) {
        return 0;
    }
    var_t5 = arg0;
    var_t4 = sp;
    var_t3 = sp;
    do {
        var_a2_3 = 0;
        var_t0_2 = var_t5;
        var_a3_3 = var_t3 + 0x10;
loop_17:
        *var_a3_3 = *((var_a2_3 * 4) + ((*var_t4 * 0x10) + arg1));
        var_f0 = 0.0f;
        if (*var_t4 == var_a2_3) {
            var_f0 = 1.0f;
        }
        *var_t0_2 = var_f0;
        var_t0_2 += 4;
        var_a2_3 += 1;
        var_a3_3 += 4;
        if (var_a2_3 < 4) {
            goto loop_17;
        }
        var_t5 += 0x10;
        var_t4 += 4;
        var_t2_2 += 1;
        var_t3 += 0x10;
    } while (var_t2_2 < 4);
    var_t3_2 = 0;
    var_t5_2 = arg0;
    var_t4_2 = sp;
    var_t1 = 0;
    do {
        temp_v1 = var_t4_2 + 0x10;
        var_a2_4 = var_t3_2 + 1;
        temp_f2 = (f32) (1.0 / (f64) *(var_t1 + temp_v1));
        if (var_a2_4 < 4) {
            var_v1_2 = (var_a2_4 * 4) + temp_v1;
            do {
                var_a2_4 += 1;
                *var_v1_2 *= temp_f2;
                var_v1_2 += 4;
            } while (var_a2_4 < 4);
        }
        var_a2_5 = 0;
        var_v1_3 = var_t5_2;
loop_26:
        var_a2_5 += 1;
        *var_v1_3 *= temp_f2;
        var_v1_3 += 4;
        if (var_a2_5 < 4) {
            goto loop_26;
        }
        var_t2_3 = var_t3_2 + 1;
        temp_v0_2 = var_t2_3 * 0x10;
        if (var_t2_3 < 4) {
            var_t0_3 = temp_v0_2 + arg0;
            var_a3_4 = temp_v0_2 + sp;
            do {
                var_a2_6 = var_t3_2 + 1;
                temp_v1_2 = var_a3_4 + 0x10;
                temp_f4 = *(var_t1 + temp_v1_2);
                temp_v0_3 = var_a2_6 * 4;
                if (var_a2_6 < 4) {
                    var_v1_4 = temp_v0_3 + temp_v1_2;
                    var_a1 = temp_v0_3 + (var_t4_2 + 0x10);
                    do {
                        temp_f2_2 = *var_v1_4 - (temp_f4 * *var_a1);
                        var_a1 += 4;
                        var_a2_6 += 1;
                        *var_v1_4 = temp_f2_2;
                        var_v1_4 += 4;
                    } while (var_a2_6 < 4);
                }
                var_a2_7 = 0;
                var_v1_5 = var_t0_3;
                var_a1_2 = var_t5_2;
loop_33:
                temp_f2_3 = *var_v1_5 - (temp_f4 * *var_a1_2);
                var_a1_2 += 4;
                var_a2_7 += 1;
                *var_v1_5 = temp_f2_3;
                var_v1_5 += 4;
                if (var_a2_7 < 4) {
                    goto loop_33;
                }
                var_t0_3 += 0x10;
                var_t2_3 += 1;
                var_a3_4 += 0x10;
            } while (var_t2_3 < 4);
        }
        var_t5_2 += 0x10;
        var_t4_2 += 0x10;
        var_t3_2 += 1;
        var_t1 += 4;
    } while (var_t3_2 < 3);
    var_a2_8 = 0;
    var_v1_6 = arg0;
    do {
        var_a2_8 += 1;
        M2C_FIELD(var_v1_6, f32 *, 0x30) = (f32) (M2C_FIELD(var_v1_6, f32 *, 0x30) * (f32) (1.0 / (f64) sp4C));
        var_v1_6 += 4;
    } while (var_a2_8 < 4);
    var_t3_3 = 2;
    var_t5_3 = &sp20;
    var_t4_3 = arg0 + 0x20;
    do {
        var_a2_9 = var_t3_3 + 1;
        if (var_a2_9 < 4) {
            var_t1_2 = (var_a2_9 * 0x10) + arg0;
            var_t0_4 = (var_a2_9 * 4) + (var_t5_3 + 0x10);
            do {
                var_a3_5 = 0;
                var_v1_7 = var_t4_3;
                var_a1_3 = var_t1_2;
loop_42:
                temp_f0 = *var_v1_7 - (*var_t0_4 * *var_a1_3);
                var_a1_3 += 4;
                var_a3_5 += 1;
                *var_v1_7 = temp_f0;
                var_v1_7 += 4;
                if (var_a3_5 < 4) {
                    goto loop_42;
                }
                var_t1_2 += 0x10;
                var_a2_9 += 1;
                var_t0_4 += 4;
            } while (var_a2_9 < 4);
        }
        var_t5_3 -= 0x10;
        var_t3_3 -= 1;
        var_t4_3 -= 0x10;
    } while (var_t3_3 >= 0);
    return 1;
}

void Mtx_Identity(f32 *arg0) {
    arg0[0xE] = 0.0f;
    arg0[0xD] = 0.0f;
    arg0[0xC] = 0.0f;
    arg0[0xB] = 0.0f;
    arg0[9] = 0.0f;
    arg0[8] = 0.0f;
    arg0[7] = 0.0f;
    arg0[6] = 0.0f;
    arg0[4] = 0.0f;
    arg0[3] = 0.0f;
    arg0[2] = 0.0f;
    arg0[1] = 0.0f;
    arg0[0xF] = 1.0f;
    arg0[0xA] = 1.0f;
    arg0[5] = 1.0f;
    M2C_FIELD(arg0, f32 *, 0) = 1.0f;
}

void Mtx_Copy(f32 *arg0, f32 *arg1) {
    f32 *var_a0;
    f32 *var_a1;
    f32 temp_f0;
    s32 var_v0;

    var_a0 = arg0;
    var_a1 = arg1;
    var_v0 = 0x10;
    do {
        temp_f0 = *var_a1;
        var_a1 += 4;
        var_v0 -= 1;
        *var_a0 = temp_f0;
        var_a0 += 4;
    } while (var_v0 != 0);
}

void func_8000793C(void *arg0, void *arg1, void *arg2) {
    f32 sp10;
    f32 temp_f16;
    f32 temp_f2;
    f32 temp_f4;

    func_80007D28(&sp10, arg2, arg1);
    temp_f2 = M2C_FIELD(arg1, f32 *, 0);
    temp_f16 = M2C_FIELD(arg1, f32 *, 4);
    temp_f4 = M2C_FIELD(arg1, f32 *, 8);
    M2C_FIELD(arg0, f32 *, 0) = (f32) ((temp_f2 * sp10) + (temp_f16 * sp20) + (temp_f4 * sp30) + sp40);
    M2C_FIELD(arg0, f32 *, 4) = (f32) ((temp_f2 * sp14) + (temp_f16 * sp24) + (temp_f4 * sp34) + sp44);
    M2C_FIELD(arg0, f32 *, 8) = (f32) ((temp_f2 * sp18) + (temp_f16 * sp28) + (temp_f4 * sp38) + sp48);
}

s32 func_80007A08(f32 *arg0, M2C_UNK *arg1, M2C_UNK *arg2) {
    f32 sp10;
    f32 sp14;
    f32 sp18;
    f32 *var_a0;
    f32 *var_v1;
    f32 temp_f0;
    f32 temp_f12;
    f32 temp_f12_2;
    f32 temp_f12_3;
    f32 temp_f2;
    f32 temp_f4;
    f32 temp_f8;
    f32 var_f20;
    f32 var_f4;
    f32 var_f6;
    s32 var_a1;

    temp_f8 = (M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg2, f32 *, 8)) - (M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg2, f32 *, 4));
    sp10 = temp_f8;
    temp_f4 = (M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg2, f32 *, 0)) - (M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg2, f32 *, 8));
    sp14 = temp_f4;
    temp_f2 = (M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg2, f32 *, 4)) - (M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg2, f32 *, 0));
    temp_f12 = (temp_f8 * temp_f8) + (temp_f4 * temp_f4) + (temp_f2 * temp_f2);
    sp18 = temp_f2;
    if (temp_f12 < 1e-16f) {
        return 0;
    }
    temp_f12_2 = 1.0f / sqrtf(temp_f12);
    sp10 *= temp_f12_2;
    sp14 *= temp_f12_2;
    sp18 *= temp_f12_2;
    var_f20 = (M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg2, f32 *, 0)) + (M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg2, f32 *, 4)) + (M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg2, f32 *, 8));
    if (!(var_f20 <= -1.0f)) {
        temp_f12_3 = 1.0f - (var_f20 * var_f20);
        temp_f0 = sqrtf(temp_f12_3);
        var_f6 = temp_f0;
        if (temp_f0 != temp_f0) {
            var_f6 = sqrtf(temp_f12_3);
        }
        var_f4 = 1.0f - var_f20;
    } else {
        var_f4 = 2.0f;
        var_f6 = 0.0f;
        var_f20 = -1.0f;
    }
    M2C_FIELD(arg0, f32 *, 0) = (sp10 * sp10 * var_f4) + var_f20;
    M2C_FIELD(arg0, f32 *, 4) = (f32) ((sp10 * sp14 * var_f4) + (sp18 * var_f6));
    M2C_FIELD(arg0, f32 *, 8) = (f32) ((sp10 * sp18 * var_f4) - (sp14 * var_f6));
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) ((sp14 * sp10 * var_f4) - (sp18 * var_f6));
    M2C_FIELD(arg0, f32 *, 0x14) = (f32) ((sp14 * sp14 * var_f4) + var_f20);
    M2C_FIELD(arg0, f32 *, 0x18) = (f32) ((sp14 * sp18 * var_f4) + (sp10 * var_f6));
    M2C_FIELD(arg0, f32 *, 0x20) = (f32) ((sp18 * sp10 * var_f4) + (sp14 * var_f6));
    M2C_FIELD(arg0, f32 *, 0x24) = (f32) ((sp18 * sp14 * var_f4) - (sp10 * var_f6));
    var_a1 = 0;
    var_a0 = arg0;
    var_v1 = arg0;
    M2C_FIELD(arg0, f32 *, 0x28) = (f32) ((sp18 * sp18 * var_f4) + var_f20);
    do {
        M2C_FIELD(var_a0, f32 *, 0x30) = 0.0f;
        var_a0 += 4;
        M2C_FIELD(var_v1, f32 *, 0xC) = 0.0f;
        var_a1 += 1;
        var_v1 += 0x10;
    } while (var_a1 < 3);
    M2C_FIELD(arg0, f32 *, 0x3C) = 1.0f;
    return 1;
}

void func_80007D28(f32 *arg0, void *arg2) {
    M2C_UNK sp10;
    M2C_UNK sp20;

    Vec3_Normalize(&sp10);
    Vec3_Normalize(&sp20, arg2);
    func_80007A08(arg0, &sp10, &sp20);
}

s32 func_80007D84(void *arg0, void *arg1, void *arg2, void *arg3, void *arg4) {
    f32 sp4;
    f32 spC;
    f32 sp14;
    f32 sp2C;
    f32 sp3C;
    f32 temp_f0;
    f32 temp_f10;
    f32 temp_f10_2;
    f32 temp_f12;
    f32 temp_f14;
    f32 temp_f14_2;
    f32 temp_f14_3;
    f32 temp_f16;
    f32 temp_f16_2;
    f32 temp_f16_3;
    f32 temp_f18;
    f32 temp_f18_2;
    f32 temp_f20;
    f32 temp_f20_2;
    f32 temp_f20_3;
    f32 temp_f22;
    f32 temp_f22_2;
    f32 temp_f24;
    f32 temp_f24_2;
    f32 temp_f24_3;
    f32 temp_f24_4;
    f32 temp_f24_5;
    f32 temp_f26;
    f32 temp_f28;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 temp_f30;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 temp_f6;
    f32 temp_f6_2;
    f32 temp_f6_3;
    f32 temp_f8;
    f32 temp_f8_2;
    f32 temp_v0;
    f32 var_f28;

    temp_f16 = M2C_FIELD(arg3, f32 *, 8);
    temp_f8 = M2C_FIELD(arg4, f32 *, 4);
    temp_f6 = M2C_FIELD(arg3, f32 *, 4);
    temp_f14 = M2C_FIELD(arg4, f32 *, 8);
    temp_f2 = M2C_FIELD(arg3, f32 *, 0);
    temp_f4 = M2C_FIELD(arg4, f32 *, 0);
    temp_f0 = temp_f4 * temp_f6;
    temp_f10 = temp_f8 * temp_f2;
    temp_f20 = M2C_FIELD(arg2, f32 *, 8);
    temp_v0 = M2C_FIELD(arg1, f32 *, 0);
    temp_f18 = (temp_f14 * temp_f2) - (temp_f4 * temp_f16);
    temp_f30 = (temp_f8 * temp_f16) - (temp_f14 * temp_f6);
    temp_f22 = temp_f0 - temp_f10;
    temp_f2_2 = M2C_FIELD(arg2, f32 *, 0);
    temp_f24 = temp_f2_2 * M2C_FIELD(arg1, f32 *, 8);
    sp14 = temp_f24;
    temp_f24_2 = (M2C_FIELD(arg2, f32 *, 4) * M2C_FIELD(arg1, f32 *, 8)) - (temp_f20 * M2C_FIELD(arg1, f32 *, 4));
    temp_f24_3 = (temp_f20 * temp_v0) - temp_f24;
    temp_f28 = (((((temp_f0 * temp_f22) - (temp_f10 * temp_f22)) + (temp_f2 * temp_f18 * temp_f14)) - (temp_f30 * temp_f6 * temp_f14)) - (temp_f4 * temp_f18 * temp_f16)) + (temp_f30 * temp_f8 * temp_f16);
    temp_f24_4 = (temp_f2_2 * M2C_FIELD(arg1, f32 *, 4)) - (M2C_FIELD(arg2, f32 *, 4) * temp_v0);
    if (temp_f28 >= 1e-16f) {
        var_f28 = 1.0f / temp_f28;
    } else {
        var_f28 = 0.0f;
    }
    temp_f20_2 = M2C_FIELD(arg4, f32 *, 4);
    temp_f14_2 = M2C_FIELD(arg3, f32 *, 8);
    temp_f4_2 = M2C_FIELD(arg3, f32 *, 4);
    temp_f6_2 = M2C_FIELD(arg4, f32 *, 8);
    temp_f16_2 = M2C_FIELD(arg3, f32 *, 0);
    temp_f26 = M2C_FIELD(arg4, f32 *, 0);
    temp_f8_2 = ((temp_f20_2 * temp_f14_2) - (temp_f4_2 * temp_f6_2)) * var_f28;
    temp_f10_2 = ((temp_f16_2 * temp_f6_2) - (temp_f26 * temp_f14_2)) * var_f28;
    temp_f12 = ((temp_f26 * temp_f4_2) - (temp_f16_2 * temp_f20_2)) * var_f28;
    temp_f2_3 = (temp_f4_2 * temp_f22) - (temp_f18 * temp_f14_2);
    sp2C = temp_f2_3;
    temp_f22_2 = temp_f2_3 * var_f28;
    temp_f14_3 = ((temp_f30 * temp_f14_2) - (temp_f16_2 * temp_f22)) * var_f28;
    sp4 = temp_f22_2;
    temp_f6_3 = (temp_f26 * temp_f22) - (temp_f30 * temp_f6_2);
    sp3C = temp_f6_3;
    temp_f24_5 = temp_f6_3 * var_f28;
    temp_f18_2 = ((temp_f18 * temp_f6_2) - (temp_f20_2 * temp_f22)) * var_f28;
    spC = temp_f24_5;
    temp_f16_3 = ((temp_f16_2 * temp_f18) - (temp_f30 * temp_f4_2)) * var_f28;
    M2C_FIELD(arg0, f32 *, 0) = (f32) ((temp_f24_2 * temp_f8_2) + (M2C_FIELD(arg2, f32 *, 0) * sp4) + (M2C_FIELD(arg1, f32 *, 0) * temp_f18_2));
    temp_f20_3 = ((temp_f30 * temp_f20_2) - (temp_f26 * temp_f18)) * var_f28;
    M2C_FIELD(arg0, f32 *, 4) = (f32) ((temp_f24_2 * temp_f10_2) + (M2C_FIELD(arg2, f32 *, 0) * temp_f14_3) + (M2C_FIELD(arg1, f32 *, 0) * spC));
    M2C_FIELD(arg0, f32 *, 8) = (f32) ((temp_f24_2 * temp_f12) + (M2C_FIELD(arg2, f32 *, 0) * temp_f16_3) + (M2C_FIELD(arg1, f32 *, 0) * temp_f20_3));
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) ((temp_f24_3 * temp_f8_2) + (M2C_FIELD(arg2, f32 *, 4) * temp_f22_2) + (M2C_FIELD(arg1, f32 *, 4) * temp_f18_2));
    M2C_FIELD(arg0, f32 *, 0x14) = (f32) ((temp_f24_3 * temp_f10_2) + (M2C_FIELD(arg2, f32 *, 4) * temp_f14_3) + (M2C_FIELD(arg1, f32 *, 4) * temp_f24_5));
    M2C_FIELD(arg0, f32 *, 0x18) = (f32) ((temp_f24_3 * temp_f12) + (M2C_FIELD(arg2, f32 *, 4) * temp_f16_3) + (M2C_FIELD(arg1, f32 *, 4) * temp_f20_3));
    M2C_FIELD(arg0, f32 *, 0x20) = (f32) ((temp_f24_4 * temp_f8_2) + (M2C_FIELD(arg2, f32 *, 8) * temp_f22_2) + (M2C_FIELD(arg1, f32 *, 8) * temp_f18_2));
    M2C_FIELD(arg0, f32 *, 0x24) = (f32) ((temp_f24_4 * temp_f10_2) + (M2C_FIELD(arg2, f32 *, 8) * temp_f14_3) + (M2C_FIELD(arg1, f32 *, 8) * temp_f24_5));
    M2C_FIELD(arg0, f32 *, 0x28) = (f32) ((temp_f24_4 * temp_f12) + (M2C_FIELD(arg2, f32 *, 8) * temp_f16_3) + (M2C_FIELD(arg1, f32 *, 8) * temp_f20_3));
    M2C_FIELD(arg0, s32 *, 0xC) = 0;
    M2C_FIELD(arg0, s32 *, 0x1C) = 0;
    M2C_FIELD(arg0, s32 *, 0x2C) = 0;
    M2C_FIELD(arg0, s32 *, 0x30) = 0;
    M2C_FIELD(arg0, s32 *, 0x34) = 0;
    M2C_FIELD(arg0, s32 *, 0x38) = 0;
    M2C_FIELD(arg0, f32 *, 0x3C) = 1.0f;
    return 1;
}

void func_80008190(void *arg0, void *arg1, f32 arg2) {
    f32 temp_f0;
    f32 temp_f10;
    f32 temp_f10_2;
    f32 temp_f14;
    f32 temp_f14_2;
    f32 temp_f16;
    f32 temp_f20;
    f32 temp_f22;
    f32 temp_f24;
    f32 temp_f24_2;
    f32 temp_f26;
    f32 temp_f26_2;
    f32 temp_f28;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 temp_f30;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 temp_f8;
    f32 temp_f8_2;

    temp_f20 = func_8001C5A0(arg2);
    temp_f0 = func_8001C450(arg2);
    temp_f10 = M2C_FIELD(arg1, f32 *, 0);
    temp_f26 = temp_f10 * temp_f10;
    temp_f2 = M2C_FIELD(arg1, f32 *, 4);
    temp_f24 = temp_f2 * temp_f2;
    temp_f4 = M2C_FIELD(arg1, f32 *, 8);
    temp_f22 = temp_f4 * temp_f20;
    temp_f28 = temp_f2 * temp_f20;
    temp_f10_2 = temp_f10 * temp_f20;
    temp_f8 = 1.0f - temp_f0;
    temp_f16 = temp_f10 * temp_f2 * temp_f8;
    temp_f14 = temp_f10 * temp_f4 * temp_f8;
    temp_f2_2 = temp_f2 * temp_f4 * temp_f8;
    temp_f30 = temp_f14 - temp_f28;
    temp_f8_2 = temp_f2_2 + temp_f10_2;
    temp_f14_2 = temp_f14 + temp_f28;
    temp_f2_3 = temp_f2_2 - temp_f10_2;
    temp_f26_2 = temp_f26 + ((1.0f - temp_f26) * temp_f0);
    temp_f4_2 = temp_f4 * temp_f4;
    temp_f24_2 = temp_f24 + ((1.0f - temp_f24) * temp_f0);
    M2C_FIELD(arg0, s32 *, 0xC) = 0;
    M2C_FIELD(arg0, s32 *, 0x1C) = 0;
    M2C_FIELD(arg0, s32 *, 0x2C) = 0;
    M2C_FIELD(arg0, s32 *, 0x30) = 0;
    M2C_FIELD(arg0, s32 *, 0x34) = 0;
    M2C_FIELD(arg0, s32 *, 0x38) = 0;
    M2C_FIELD(arg0, f32 *, 0x3C) = 1.0f;
    M2C_FIELD(arg0, f32 *, 8) = temp_f30;
    M2C_FIELD(arg0, f32 *, 0x18) = temp_f8_2;
    M2C_FIELD(arg0, f32 *, 0x20) = temp_f14_2;
    M2C_FIELD(arg0, f32 *, 0x24) = temp_f2_3;
    M2C_FIELD(arg0, f32 *, 4) = (f32) (temp_f16 + temp_f22);
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) (temp_f16 - temp_f22);
    M2C_FIELD(arg0, f32 *, 0) = temp_f26_2;
    M2C_FIELD(arg0, f32 *, 0x14) = temp_f24_2;
    M2C_FIELD(arg0, f32 *, 0x28) = (f32) (temp_f4_2 + ((1.0f - temp_f4_2) * temp_f0));
}

void func_800082DC(M2C_UNK *arg0, M2C_UNK *arg1, f32 arg2) {
    f32 temp_f20;
    f32 temp_f20_2;
    f32 temp_f4;
    s16 temp_v0;
    s16 var_a0;
    s32 temp_v1;
    void *temp_v0_2;
    void *temp_v1_2;

    temp_f20 = arg2 * 0.017453292f;
    temp_f20_2 = func_8001C5A0(temp_f20);
    temp_f4 = func_8001C450(temp_f20);
    var_a0 = 0;
    do {
        temp_v1 = (s32) (var_a0 << 0x10) >> 0xE;
        temp_v0_2 = temp_v1 + arg1;
        temp_v1_2 = temp_v1 + arg0;
        M2C_FIELD(temp_v1_2, f32 *, 0) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0);
        M2C_FIELD(temp_v1_2, f32 *, 0x10) = (f32) ((M2C_FIELD(temp_v0_2, f32 *, 0x10) * temp_f4) + (M2C_FIELD(temp_v0_2, f32 *, 0x20) * temp_f20_2));
        M2C_FIELD(temp_v1_2, f32 *, 0x20) = (f32) ((M2C_FIELD(temp_v0_2, f32 *, 0x20) * temp_f4) - (M2C_FIELD(temp_v0_2, f32 *, 0x10) * temp_f20_2));
        temp_v0 = var_a0 + 1;
        var_a0 = temp_v0;
        M2C_FIELD(temp_v1_2, f32 *, 0x30) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x30);
    } while (temp_v0 < 4);
}

void func_800083A8(M2C_UNK *arg0, s32 arg1, f32 arg2) {
    f32 temp_f20;
    f32 temp_f20_2;
    f32 temp_f4;
    s16 temp_v0;
    s16 var_a0;
    s32 temp_v1;
    void *temp_v0_2;
    void *temp_v1_2;

    temp_f20 = arg2 * 0.017453292f;
    temp_f20_2 = func_8001C5A0(temp_f20);
    temp_f4 = func_8001C450(temp_f20);
    var_a0 = 0;
    do {
        temp_v1 = (s32) (var_a0 << 0x10) >> 0xE;
        temp_v0_2 = temp_v1 + arg1;
        temp_v1_2 = temp_v1 + arg0;
        M2C_FIELD(temp_v1_2, f32 *, 0) = (f32) ((M2C_FIELD(temp_v0_2, f32 *, 0) * temp_f4) - (M2C_FIELD(temp_v0_2, f32 *, 0x20) * temp_f20_2));
        M2C_FIELD(temp_v1_2, f32 *, 0x10) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x10);
        M2C_FIELD(temp_v1_2, f32 *, 0x20) = (f32) ((M2C_FIELD(temp_v0_2, f32 *, 0) * temp_f20_2) + (M2C_FIELD(temp_v0_2, f32 *, 0x20) * temp_f4));
        temp_v0 = var_a0 + 1;
        var_a0 = temp_v0;
        M2C_FIELD(temp_v1_2, f32 *, 0x30) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x30);
    } while (temp_v0 < 4);
}

void func_80008474(f32 *arg0, M2C_UNK *arg1, f32 arg2) {
    f32 temp_f20;
    f32 temp_f20_2;
    f32 temp_f4;
    s16 temp_v0;
    s16 var_a0;
    s32 temp_v1;
    void *temp_v0_2;
    void *temp_v1_2;

    temp_f20 = arg2 * 0.017453292f;
    temp_f20_2 = func_8001C5A0(temp_f20);
    temp_f4 = func_8001C450(temp_f20);
    var_a0 = 0;
    do {
        temp_v1 = (s32) (var_a0 << 0x10) >> 0xE;
        temp_v0_2 = temp_v1 + arg1;
        temp_v1_2 = temp_v1 + arg0;
        M2C_FIELD(temp_v1_2, f32 *, 0) = (f32) ((M2C_FIELD(temp_v0_2, f32 *, 0) * temp_f4) + (M2C_FIELD(temp_v0_2, f32 *, 0x10) * temp_f20_2));
        M2C_FIELD(temp_v1_2, f32 *, 0x10) = (f32) ((M2C_FIELD(temp_v0_2, f32 *, 0x10) * temp_f4) - (M2C_FIELD(temp_v0_2, f32 *, 0) * temp_f20_2));
        M2C_FIELD(temp_v1_2, f32 *, 0x20) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x20);
        temp_v0 = var_a0 + 1;
        var_a0 = temp_v0;
        M2C_FIELD(temp_v1_2, f32 *, 0x30) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x30);
    } while (temp_v0 < 4);
}

void func_80008540(f32 *arg0, f32 arg3, f32 arg4) {
    M2C_UNK sp10;
    M2C_UNK sp50;

    func_800083A8(&sp10);
    func_800082DC(&sp50, &sp10, arg3);
    func_80008474(arg0, &sp50, arg4);
}

void func_800085B4(void *arg0, f32 *arg1, f32 arg2, f32 arg3, f32 arg4) {
    f32 *var_a1;
    f32 temp_f2;
    s32 var_v1;
    void *var_a0;

    var_a0 = arg0;
    var_a1 = arg1;
    var_v1 = 0;
    do {
        M2C_FIELD(var_a0, f32 *, 0) = (f32) M2C_FIELD(var_a1, f32 *, 0);
        M2C_FIELD(var_a0, f32 *, 0x10) = (f32) M2C_FIELD(var_a1, f32 *, 0x10);
        M2C_FIELD(var_a0, f32 *, 0x20) = (f32) M2C_FIELD(var_a1, f32 *, 0x20);
        temp_f2 = (M2C_FIELD(var_a1, f32 *, 0) * arg2) + (M2C_FIELD(var_a1, f32 *, 0x10) * arg3) + (M2C_FIELD(var_a1, f32 *, 0x20) * arg4) + M2C_FIELD(var_a1, f32 *, 0x30);
        var_v1 += 1;
        var_a1 += 4;
        M2C_FIELD(var_a0, f32 *, 0x30) = temp_f2;
        var_a0 += 4;
    } while (var_v1 < 4);
}

void func_80008624(s32 arg0, s32 arg1, f32 arg2) {
    s16 temp_v0;
    s16 var_a2;
    s32 temp_v1;
    void *temp_v0_2;
    void *temp_v1_2;

    var_a2 = 0;
    do {
        temp_v1 = (s32) (var_a2 << 0x10) >> 0xE;
        temp_v0_2 = temp_v1 + arg1;
        temp_v1_2 = temp_v1 + arg0;
        M2C_FIELD(temp_v1_2, f32 *, 0) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0);
        M2C_FIELD(temp_v1_2, f32 *, 0x10) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x10);
        M2C_FIELD(temp_v1_2, f32 *, 0x20) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x20);
        temp_v0 = var_a2 + 1;
        var_a2 = temp_v0;
        M2C_FIELD(temp_v1_2, f32 *, 0x30) = (f32) ((M2C_FIELD(temp_v0_2, f32 *, 0) * arg2) + M2C_FIELD(temp_v0_2, f32 *, 0x30));
    } while (temp_v0 < 4);
}

void func_80008688(s32 arg0, s32 arg1, f32 arg2) {
    s16 temp_v0;
    s16 var_a2;
    s32 temp_v1;
    void *temp_v0_2;
    void *temp_v1_2;

    var_a2 = 0;
    do {
        temp_v1 = (s32) (var_a2 << 0x10) >> 0xE;
        temp_v0_2 = temp_v1 + arg1;
        temp_v1_2 = temp_v1 + arg0;
        M2C_FIELD(temp_v1_2, f32 *, 0) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0);
        M2C_FIELD(temp_v1_2, f32 *, 0x10) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x10);
        M2C_FIELD(temp_v1_2, f32 *, 0x20) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x20);
        temp_v0 = var_a2 + 1;
        var_a2 = temp_v0;
        M2C_FIELD(temp_v1_2, f32 *, 0x30) = (f32) ((M2C_FIELD(temp_v0_2, f32 *, 0x10) * arg2) + M2C_FIELD(temp_v0_2, f32 *, 0x30));
    } while (temp_v0 < 4);
}

void func_800086EC(s32 arg0, s32 arg1, f32 arg2) {
    s16 temp_v0;
    s16 var_a2;
    s32 temp_v1;
    void *temp_v0_2;
    void *temp_v1_2;

    var_a2 = 0;
    do {
        temp_v1 = (s32) (var_a2 << 0x10) >> 0xE;
        temp_v0_2 = temp_v1 + arg1;
        temp_v1_2 = temp_v1 + arg0;
        M2C_FIELD(temp_v1_2, f32 *, 0) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0);
        M2C_FIELD(temp_v1_2, f32 *, 0x10) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x10);
        M2C_FIELD(temp_v1_2, f32 *, 0x20) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x20);
        temp_v0 = var_a2 + 1;
        var_a2 = temp_v0;
        M2C_FIELD(temp_v1_2, f32 *, 0x30) = (f32) ((M2C_FIELD(temp_v0_2, f32 *, 0x20) * arg2) + M2C_FIELD(temp_v0_2, f32 *, 0x30));
    } while (temp_v0 < 4);
}

void func_80008750(f32 *arg0, f32 *arg1, f32 arg2, f32 arg3, f32 arg4) {
    s16 temp_v0;
    s16 var_a2;
    s32 temp_v1;
    void *temp_v0_2;
    void *temp_v1_2;

    var_a2 = 0;
    do {
        temp_v1 = (s32) (var_a2 << 0x10) >> 0xE;
        temp_v0_2 = temp_v1 + arg1;
        temp_v1_2 = temp_v1 + arg0;
        M2C_FIELD(temp_v1_2, f32 *, 0) = (f32) (M2C_FIELD(temp_v0_2, f32 *, 0) * arg2);
        M2C_FIELD(temp_v1_2, f32 *, 0x10) = (f32) (M2C_FIELD(temp_v0_2, f32 *, 0x10) * arg3);
        M2C_FIELD(temp_v1_2, f32 *, 0x20) = (f32) (M2C_FIELD(temp_v0_2, f32 *, 0x20) * arg4);
        temp_v0 = var_a2 + 1;
        var_a2 = temp_v0;
        M2C_FIELD(temp_v1_2, f32 *, 0x30) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x30);
    } while (temp_v0 < 4);
}

void func_800087BC(s32 arg0, s32 arg1, f32 arg2) {
    s16 temp_v0;
    s16 var_a2;
    s32 temp_v1;
    void *temp_v0_2;
    void *temp_v1_2;

    var_a2 = 0;
    do {
        temp_v1 = (s32) (var_a2 << 0x10) >> 0xE;
        temp_v0_2 = temp_v1 + arg1;
        temp_v1_2 = temp_v1 + arg0;
        M2C_FIELD(temp_v1_2, f32 *, 0) = (f32) (M2C_FIELD(temp_v0_2, f32 *, 0) * arg2);
        M2C_FIELD(temp_v1_2, f32 *, 0x10) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x10);
        M2C_FIELD(temp_v1_2, f32 *, 0x20) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x20);
        temp_v0 = var_a2 + 1;
        var_a2 = temp_v0;
        M2C_FIELD(temp_v1_2, f32 *, 0x30) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x30);
    } while (temp_v0 < 4);
}

void func_80008818(s32 arg0, s32 arg1, f32 arg2) {
    s16 temp_v0;
    s16 var_a2;
    s32 temp_v1;
    void *temp_v0_2;
    void *temp_v1_2;

    var_a2 = 0;
    do {
        temp_v1 = (s32) (var_a2 << 0x10) >> 0xE;
        temp_v0_2 = temp_v1 + arg1;
        temp_v1_2 = temp_v1 + arg0;
        M2C_FIELD(temp_v1_2, f32 *, 0) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0);
        M2C_FIELD(temp_v1_2, f32 *, 0x10) = (f32) (M2C_FIELD(temp_v0_2, f32 *, 0x10) * arg2);
        M2C_FIELD(temp_v1_2, f32 *, 0x20) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x20);
        temp_v0 = var_a2 + 1;
        var_a2 = temp_v0;
        M2C_FIELD(temp_v1_2, f32 *, 0x30) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x30);
    } while (temp_v0 < 4);
}

void func_80008874(s32 arg0, s32 arg1, f32 arg2) {
    s16 temp_v0;
    s16 var_a2;
    s32 temp_v1;
    void *temp_v0_2;
    void *temp_v1_2;

    var_a2 = 0;
    do {
        temp_v1 = (s32) (var_a2 << 0x10) >> 0xE;
        temp_v0_2 = temp_v1 + arg1;
        temp_v1_2 = temp_v1 + arg0;
        M2C_FIELD(temp_v1_2, f32 *, 0) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0);
        M2C_FIELD(temp_v1_2, f32 *, 0x10) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x10);
        M2C_FIELD(temp_v1_2, f32 *, 0x20) = (f32) (M2C_FIELD(temp_v0_2, f32 *, 0x20) * arg2);
        temp_v0 = var_a2 + 1;
        var_a2 = temp_v0;
        M2C_FIELD(temp_v1_2, f32 *, 0x30) = (f32) M2C_FIELD(temp_v0_2, f32 *, 0x30);
    } while (temp_v0 < 4);
}

void func_800088D0(void) {

}

void func_800088D8(void) {

}

void func_800088E0(f32 *arg0, f32 *arg1) {
    f32 *var_a0;
    f32 *var_a0_2;
    f32 *var_a1;
    f32 *var_a2;
    f32 *var_s0;
    f32 *var_s1;
    f32 *var_v1;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 var_f0;
    f32 var_f12;
    s32 var_a3;
    s32 var_s2;
    s32 var_v1_2;
    s32 var_v1_3;

    var_a1 = arg1;
    var_a3 = 0;
    var_a2 = arg0;
    var_v1 = arg0;
    do {
        M2C_FIELD(var_v1, s32 *, 0x30) = 0;
        M2C_FIELD(var_a2, s32 *, 0xC) = 0;
        var_a2 += 0x10;
        var_a3 += 1;
        var_v1 += 4;
    } while (var_a3 < 3);
    var_s0 = var_a1;
    var_s1 = arg0;
    var_s2 = 0;
    M2C_FIELD(var_s1, f32 *, 0x3C) = 1.0f;
    do {
        var_f12 = 0.0f;
        var_v1_2 = 0;
        var_a0 = var_s0;
loop_4:
        temp_f0 = *var_a0;
        temp_f0_2 = temp_f0 * temp_f0;
        var_a0 += 4;
        var_v1_2 += 1;
        var_f12 += temp_f0_2;
        if (var_v1_2 < 3) {
            goto loop_4;
        }
        if (var_f12 != 0.0f) {
            var_f0 = sqrtf(var_f12);
            if (var_f0 != var_f0) {
                var_f0 = sqrtf(var_f12, M2C_BITWISE(f32, var_a0), (s32) var_a1);
            }
            var_f12 = 1.0f / var_f0;
        }
        var_v1_3 = 0;
        var_a1 = var_s1;
        var_a0_2 = var_s0;
loop_10:
        temp_f0_3 = *var_a0_2 * var_f12;
        var_a0_2 += 4;
        var_v1_3 += 1;
        *var_a1 = temp_f0_3;
        var_a1 += 4;
        if (var_v1_3 < 3) {
            goto loop_10;
        }
        var_s0 += 0x10;
        var_s2 += 0x10;
        var_s1 += 0x10;
    } while (var_s2 < 0x30);
}

s32 func_80008A10(u16 *arg0, M2C_UNK **arg1, s32 *arg2, s32 arg3) {
    M2C_UNK *sp18;
    s32 sp1C;
    M2C_UNK *sp30;
    M2C_UNK *sp34;                                  /* compiler-managed */
    M2C_UNK *sp48;
    u16 *sp308C;
    M2C_UNK **sp3094;
    M2C_UNK *sp309C;
    M2C_UNK *sp30A4;
    s32 sp30AC;
    M2C_UNK **temp_v0_3;
    M2C_UNK **temp_v1_3;
    M2C_UNK **temp_v1_6;
    M2C_UNK **var_a0_10;
    M2C_UNK **var_a0_8;
    M2C_UNK **var_a1_4;
    M2C_UNK **var_a1_7;
    M2C_UNK **var_t2;
    M2C_UNK **var_t2_8;
    M2C_UNK *temp_a1;
    M2C_UNK *temp_fp;
    M2C_UNK *temp_s1_2;
    M2C_UNK *temp_s5;
    M2C_UNK *temp_v0_6;
    M2C_UNK *temp_v1_4;
    M2C_UNK *temp_v1_5;
    M2C_UNK *var_a0;
    M2C_UNK *var_a0_2;
    M2C_UNK *var_a0_3;
    M2C_UNK *var_a0_4;
    M2C_UNK *var_a0_5;
    M2C_UNK *var_a2_5;
    M2C_UNK *var_a3_8;
    M2C_UNK *var_t0;
    M2C_UNK *var_t0_10;
    M2C_UNK *var_t0_2;
    M2C_UNK *var_t0_3;
    M2C_UNK *var_t0_4;
    M2C_UNK *var_t0_5;
    M2C_UNK *var_t0_6;
    M2C_UNK *var_t0_7;
    M2C_UNK *var_t0_8;
    M2C_UNK *var_t0_9;
    M2C_UNK *var_t1;
    M2C_UNK *var_t1_2;
    M2C_UNK *var_t1_3;
    M2C_UNK *var_t1_4;
    M2C_UNK *var_t1_5;
    M2C_UNK *var_t1_6;
    M2C_UNK *var_t1_7;
    M2C_UNK *var_t2_5;
    M2C_UNK *var_t3_3;
    M2C_UNK *var_t7;
    M2C_UNK *var_v1_12;
    M2C_UNK *var_v1_15;
    M2C_UNK *var_v1_3;
    M2C_UNK *var_v1_4;
    M2C_UNK *var_v1_5;
    M2C_UNK *var_v1_8;
    s16 var_a2_11;
    s16 var_t2_7;
    s16 var_v1_14;
    s32 *temp_s7;
    s32 *var_a1;
    s32 *var_a1_2;
    s32 *var_a2;
    s32 *var_a2_2;
    s32 temp_a0;
    s32 temp_s0;
    s32 temp_s1;
    s32 temp_s2;
    s32 temp_s4;
    s32 temp_s4_2;
    s32 temp_t4;
    s32 temp_v0_2;
    s32 temp_v0_4;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_a0_6;
    s32 var_a0_7;
    s32 var_a0_9;
    s32 var_a1_3;
    s32 var_a1_5;
    s32 var_a1_6;
    s32 var_a1_8;
    s32 var_a2_10;
    s32 var_a2_12;
    s32 var_a2_3;
    s32 var_a2_4;
    s32 var_a2_6;
    s32 var_a2_7;
    s32 var_a2_8;
    s32 var_a2_9;
    s32 var_a3;
    s32 var_a3_10;
    s32 var_a3_11;
    s32 var_a3_12;
    s32 var_a3_2;
    s32 var_a3_3;
    s32 var_a3_4;
    s32 var_a3_5;
    s32 var_a3_6;
    s32 var_a3_7;
    s32 var_a3_9;
    s32 var_fp;
    s32 var_s0;
    s32 var_s0_2;
    s32 var_s2;
    s32 var_s3;
    s32 var_s6;
    s32 var_s6_2;
    s32 var_s7;
    s32 var_t2_2;
    s32 var_t2_3;
    s32 var_t2_4;
    s32 var_t2_6;
    s32 var_t3;
    s32 var_t3_2;
    s32 var_t4;
    s32 var_t5_2;
    s32 var_t5_3;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;
    s32 var_v0_4;
    s32 var_v0_5;
    s32 var_v0_6;
    s32 var_v0_7;
    s32 var_v0_8;
    s32 var_v0_9;
    s32 var_v1;
    s32 var_v1_10;
    s32 var_v1_13;
    s32 var_v1_2;
    s32 var_v1_6;
    s32 var_v1_7;
    s32 var_v1_9;
    s8 var_v1_11;
    u16 *temp_a0_3;
    u16 *temp_a0_4;
    u16 *var_a0_11;
    u16 temp_v0;
    u16 temp_v0_5;
    void *temp_a0_2;
    void *temp_t4_2;
    void *temp_t4_3;
    void *var_t5;

    var_s6 = saved_reg_s6;
    var_fp = saved_reg_fp;
    var_a2 = arg2;
    var_a3 = arg3;
    temp_s7 = var_a2;
    temp_s4 = var_a3;
    var_s3 = 0;
    var_s2 = 0;
    sp308C = arg0;
    sp3094 = arg1;
    sp30AC = 0;
    if (temp_s4 > 0) {
        var_a3 = (temp_s4 * 0x10) + temp_s7;
        do {
            temp_v1 = M2C_FIELD(var_a2, s32 *, 0xC);
            if (sp30AC < temp_v1) {
                sp30AC = temp_v1;
            }
            var_v1 = 0;
            var_a1 = var_a2;
loop_5:
            temp_a0 = *var_a1;
            if (var_s2 < temp_a0) {
                var_s2 = temp_a0;
            }
            var_v1 += 1;
            var_a1 += 4;
            if (var_v1 < 3) {
                goto loop_5;
            }
            var_a2 += 0x10;
        } while ((s32) var_a2 < var_a3);
    }
    temp_s2 = var_s2 + 1;
    temp_s1 = temp_s2 * 4;
    sp30AC += 1;
    sp30A4 = Heap_Alloc(&gGameHeap, temp_s1, var_a2, var_a3);
    temp_s5 = Heap_Alloc(&gGameHeap, temp_s1);
    sp309C = Heap_Alloc(&gGameHeap, temp_s4 * 0x18);
    var_t0 = sp309C;
    var_a3_2 = 0;
    sp1C = 0;
    sp30 = NULL;
    sp18 = &sp30;
    sp34 = &sp18;
    if (temp_s4 > 0) {
        var_a2_2 = temp_s7;
        do {
            var_v1_2 = 0;
            var_a1_2 = var_a2_2;
            var_a0 = var_t0;
loop_12:
            temp_v0 = M2C_FIELD(var_a1_2, u16 *, 2);
            var_a1_2 += 4;
            var_v1_2 += 1;
            var_a0[2] = (M2C_UNK *) temp_v0;
            var_a0 += 4;
            if (var_v1_2 < 4) {
                goto loop_12;
            }
            *sp34 = (M2C_UNK *) var_t0;
            var_a2_2 += 0x10;
            var_a3_2 += 1;
            M2C_FIELD(var_t0, M2C_UNK **, 0) = (M2C_UNK *) &sp30;
            var_t0[1] = sp34;
            sp34 = var_t0;
            var_t0 += 0x18;
        } while (var_a3_2 < temp_s4);
    }
loop_15:
    var_a1_3 = 0;
    var_a2_3 = 0;
    if (sp30AC > 0) {
loop_16:
        var_a3_3 = 0;
        if (temp_s2 > 0) {
            var_v1_3 = temp_s5;
            do {
                *var_v1_3 = NULL;
                var_a3_3 += 1;
                var_v1_3 += 4;
            } while (var_a3_3 < temp_s2);
        }
        var_t0_2 = sp18;
        var_t1 = *var_t0_2;
        if (var_t1 != NULL) {
loop_20:
            if (M2C_FIELD(var_t0_2, s32 *, 0x14) == var_a1_3) {
                var_a3_4 = 0;
                var_a0_2 = var_t0_2;
loop_22:
                temp_v1_2 = M2C_FIELD(var_a0_2, s32 *, 8);
                if (temp_v1_2 < temp_s2) {
                    temp_v1_3 = &temp_s5[temp_v1_2];
                    var_a3_4 += 1;
                    *temp_v1_3 += 1;
                    var_a0_2 += 4;
                    if (var_a3_4 >= 3) {
                        goto block_24;
                    }
                    goto loop_22;
                }
                goto block_69;
            }
block_24:
            var_t0_2 = var_t1;
            var_t1 = *var_t1;
            if (var_t1 == NULL) {
                goto block_25;
            }
            goto loop_20;
        }
block_25:
        var_a3_2 = 0;
        if (temp_s2 > 0) {
            var_a0_3 = temp_s5;
            do {
                temp_v1_4 = *var_a0_3;
                if (var_a2_3 < (s32) temp_v1_4) {
                    var_a2_3 = (s32) temp_v1_4;
                    var_fp = var_a1_3;
                    var_s6 = var_a3_2;
                }
                var_a3_2 += 1;
                var_a0_3 += 4;
            } while (var_a3_2 < temp_s2);
        }
        var_a1_3 += 1;
        if (var_a1_3 >= sp30AC) {
            goto block_31;
        }
        goto loop_16;
    }
block_31:
    if (var_a2_3 != 0) {
        temp_v0_2 = var_s3 * 0x30;
        var_t5 = temp_v0_2 + &sp18;
        var_s0 = temp_v0_2;
        var_t7 = var_s0 + &sp48;
loop_33:
        var_a2_4 = 0;
        var_a3_5 = 0;
        if (temp_s2 > 0) {
            var_v1_4 = temp_s5;
            do {
                *var_v1_4 = NULL;
                var_a3_5 += 1;
                var_v1_4 += 4;
            } while (var_a3_5 < temp_s2);
        }
        var_t0_3 = sp18;
        var_t1_2 = *var_t0_3;
        if (var_t1_2 != NULL) {
            do {
                if (M2C_FIELD(var_t0_3, s32 *, 0x14) == var_fp) {
                    var_a3_6 = 0;
                    var_a0_4 = var_t0_3;
                    do {
                        temp_v0_3 = &temp_s5[M2C_FIELD(var_a0_4, s32 *, 8)];
                        var_a3_6 += 1;
                        *temp_v0_3 += 1;
                        var_a0_4 += 4;
                    } while (var_a3_6 < 3);
                }
                var_t0_3 = var_t1_2;
                var_t1_2 = *var_t1_2;
            } while (var_t1_2 != NULL);
        }
        var_a3_2 = 0;
        if (temp_s2 > 0) {
            var_a0_5 = temp_s5;
            do {
                temp_v1_5 = *var_a0_5;
                if (var_a2_4 < (s32) temp_v1_5) {
                    var_a2_4 = (s32) temp_v1_5;
                    var_s6 = var_a3_2;
                }
                var_a3_2 += 1;
                var_a0_5 += 4;
            } while (var_a3_2 < temp_s2);
        }
        if (var_a2_4 != 0) {
            M2C_FIELD(var_t5, s32 *, 0x34) = 0;
            M2C_FIELD(var_t5, s32 *, 0x48) = 0;
            M2C_FIELD(var_t5, void **, 0x30) = (void *) (var_t7 + 0x18);
            M2C_FIELD(var_t5, M2C_UNK **, 0x4C) = var_t7;
            var_t0_4 = sp18;
            var_t1_3 = *var_t0_4;
            var_t3 = 0;
            if (var_t1_3 != NULL) {
loop_49:
                var_v0 = var_t3 < 0x10;
                if (M2C_FIELD(var_t0_4, s32 *, 0x14) == var_fp) {
                    var_a3_7 = 0;
                    var_v1_5 = var_t0_4;
loop_51:
                    if (M2C_FIELD(var_v1_5, s32 *, 8) != var_s6) {
                        var_a3_7 += 1;
                        var_v1_5 += 4;
                        if (var_a3_7 >= 3) {
                            goto block_65;
                        }
                        goto loop_51;
                    }
                    var_v1_6 = 0;
                    var_a2_5 = var_t0_4;
                    var_t2 = &(&sp18)[var_t3];
                    do {
                        var_a0_6 = 0;
                        if (var_t3 > 0) {
                            var_a1_4 = &sp18;
loop_57:
                            if (M2C_FIELD(var_a2_5, s32 *, 8) != M2C_FIELD(var_a1_4, s32 *, 0x3030)) {
                                var_a0_6 += 1;
                                var_a1_4 += 4;
                                if (var_a0_6 < var_t3) {
                                    goto loop_57;
                                }
                            }
                        }
                        var_v1_6 += 1;
                        if (var_a0_6 == var_t3) {
                            var_t3 += 1;
                            M2C_FIELD(var_t2, s32 *, 0x3030) = (s32) M2C_FIELD(var_a2_5, s32 *, 8);
                            var_t2 += 4;
                        }
                        var_a2_5 += 4;
                    } while (var_v1_6 < 3);
                    var_v0 = var_t3 < 0x10;
                    if (var_t3 < 0x11) {
                        *M2C_FIELD(var_t0_4, M2C_UNK **, 4) = M2C_FIELD(var_t0_4, M2C_UNK **, 0);
                        M2C_FIELD(M2C_FIELD(var_t0_4, M2C_UNK **, 0), M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(var_t0_4, M2C_UNK **, 4);
                        *M2C_FIELD(var_t5, M2C_UNK **, 0x4C) = var_t0_4;
                        M2C_FIELD(var_t0_4, M2C_UNK **, 0) = &sp48 + var_s0 + 0x18;
                        M2C_FIELD(var_t0_4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(var_t5, M2C_UNK **, 0x4C);
                        M2C_FIELD(var_t5, M2C_UNK **, 0x4C) = var_t0_4;
block_65:
                        var_v0 = var_t3 < 0x10;
                    }
                }
                if (var_v0 != 0) {
                    var_t0_4 = var_t1_3;
                    var_t1_3 = *var_t1_3;
                    if (var_t1_3 != NULL) {
                        goto loop_49;
                    }
                }
            }
            var_t5 += 0x30;
            var_s0 += 0x30;
            var_s3 += 1;
            var_t7 += 0x30;
            if (var_s3 >= 0x100) {
block_69:
                return 0;
            }
            goto loop_33;
        }
        goto loop_15;
    }
    temp_s0 = (var_s3 + 3) & ~3;
    temp_s4_2 = temp_s0 + 4;
    var_a3_8 = NULL;
    temp_s1_2 = Heap_Alloc(&gGameHeap, temp_s4_2 * temp_s2, (s32 *) var_a2_3, var_a3_2);
    if (temp_s2 > 0) {
        var_a0_7 = 0;
        do {
            var_v0_2 = var_a0_7;
            if (var_a0_7 < 0) {
                var_v0_2 = var_a0_7 + 3;
            }
            temp_s1_2[var_v0_2 >> 2] = var_a3_8;
            var_v1_7 = 0;
            if (temp_s0 > 0) {
                var_v0_3 = var_a0_7;
                do {
                    M2C_FIELD((var_v0_3 + temp_s1_2), s8 *, 4) = 0xFE;
                    var_v1_7 += 1;
                    var_v0_3 = var_v1_7 + var_a0_7;
                } while (var_v1_7 < temp_s0);
            }
            var_a3_8 += 1;
            var_a0_7 += temp_s4_2;
        } while ((s32) var_a3_8 < temp_s2);
    }
    var_a2_6 = 0;
    if (var_s3 > 0) {
        var_a0_8 = &sp48;
        do {
            var_t0_5 = *var_a0_8;
            var_t1_4 = *var_t0_5;
            if (var_t1_4 != NULL) {
                do {
                    var_a3_8 = NULL;
                    var_v1_8 = var_t0_5;
loop_82:
                    var_a3_8 += 1;
                    M2C_FIELD((var_a2_6 + (M2C_FIELD(var_v1_8, s32 *, 8) * temp_s4_2) + temp_s1_2), s8 *, 4) = 1;
                    var_v1_8 += 4;
                    if ((s32) var_a3_8 < 3) {
                        goto loop_82;
                    }
                    var_t0_5 = var_t1_4;
                    var_t1_4 = *var_t1_4;
                } while (var_t1_4 != NULL);
            }
            var_a2_6 += 1;
            var_a0_8 += 0x30;
        } while (var_a2_6 < var_s3);
    }
    var_t2_2 = 0;
    if (temp_s2 > 0) {
        var_a2_6 = 0;
        do {
            var_v1_9 = 0;
            var_a3_8 = NULL;
            if (temp_s0 > 0) {
                var_v0_4 = var_a2_6;
                do {
                    temp_a0_2 = var_v0_4 + temp_s1_2;
                    if (M2C_FIELD(temp_a0_2, u8 *, 4) != 0xFE) {
                        var_v1_9 = 1;
                        goto block_96;
                    }
                    if (var_v1_9 != 0) {
                        M2C_FIELD(temp_a0_2, u8 *, 4) = 0U;
block_96:
                        var_a3_8 += 1;
                    } else {
                        var_a3_8 += 1;
                    }
                    var_v0_4 = (s32) (var_a3_8 + var_a2_6);
                } while ((s32) var_a3_8 < temp_s0);
            }
            var_t2_2 += 1;
            var_a2_6 += temp_s4_2;
        } while (var_t2_2 < temp_s2);
    }
    var_s6_2 = 0;
    var_t5_2 = 0;
    temp_fp = Heap_Alloc(&gGameHeap, (var_s3 + 1) * 4, (s32 *) var_a2_6, (s32) var_a3_8);
    if (var_s3 > 0) {
        var_t0_6 = temp_fp;
        do {
            var_t4 = var_t5_2;
            var_t3_2 = 0;
loop_102:
            var_t2_3 = 0;
            if (temp_s2 > 0) {
                var_a1_5 = 0;
                do {
                    var_v1_10 = 0;
                    var_a2_7 = var_t5_2;
                    if (var_t4 >= var_t5_2) {
                        var_v0_5 = var_a2_7 + var_a1_5;
                        do {
                            var_a2_7 += 1;
                            var_v1_10 |= M2C_FIELD((var_v0_5 + temp_s1_2), u8 *, 4);
                            var_v0_5 = var_a2_7 + var_a1_5;
                        } while (var_t4 >= var_a2_7);
                    }
                    var_t3_2 += var_v1_10 & 1;
                    var_t2_3 += 1;
                    var_a1_5 += temp_s4_2;
                } while (var_t2_3 < temp_s2);
            }
            if (var_t3_2 < 0x11) {
                var_t4 += 1;
                var_t3_2 = 0;
                if (var_t4 < var_s3) {
                    goto loop_102;
                }
            }
            temp_t4 = var_t4 - 1;
            *var_t0_6 = (temp_t4 - var_t5_2) + 1;
            var_t0_6 += 4;
            var_t5_2 = temp_t4 + 1;
            var_s6_2 += 1;
        } while (var_t5_2 < var_s3);
        var_t5_2 = 0;
    }
    var_a3_9 = 0;
    if (var_s6_2 > 0) {
        var_t0_7 = temp_fp;
        do {
            var_t2_4 = 0;
            if (temp_s2 > 0) {
                var_a1_6 = 0;
                do {
                    var_v1_11 = 0;
                    temp_t4_2 = (var_t5_2 + *var_t0_7) - 1;
                    var_a2_8 = var_t5_2;
                    if ((s32) temp_t4_2 >= var_t5_2) {
                        var_v0_6 = var_a2_8 + var_a1_6;
                        do {
                            var_a2_8 += 1;
                            var_v1_11 |= M2C_FIELD((var_v0_6 + temp_s1_2), u8 *, 4);
                            var_v0_6 = var_a2_8 + var_a1_6;
                        } while ((s32) temp_t4_2 >= var_a2_8);
                    }
                    if (var_v1_11 & 1) {
                        var_v1_11 = 1;
                    }
                    var_a2_9 = var_t5_2;
                    if ((s32) temp_t4_2 >= var_t5_2) {
                        var_v0_7 = var_a2_9 + var_a1_6;
                        do {
                            M2C_FIELD((var_v0_7 + temp_s1_2), s8 *, 4) = var_v1_11;
                            var_a2_9 += 1;
                            var_v0_7 = var_a2_9 + var_a1_6;
                        } while ((s32) temp_t4_2 >= var_a2_9);
                    }
                    var_t2_4 += 1;
                    var_a1_6 += temp_s4_2;
                } while (var_t2_4 < temp_s2);
            }
            var_a3_9 += 1;
            var_t5_2 = (s32) (var_t5_2 + *var_t0_7);
            var_t0_7 += 4;
        } while (var_a3_9 < var_s6_2);
    }
    func_80003C4C(temp_s1_2, temp_s2, temp_s4_2, 4, temp_s0, 0);
    var_t2_5 = NULL;
    if (temp_s2 > 0) {
        var_a1_7 = sp3094;
        var_a0_9 = 0;
        do {
            var_v0_8 = var_a0_9;
            if (var_a0_9 < 0) {
                var_v0_8 = var_a0_9 + 3;
            }
            temp_v1_6 = &temp_s1_2[var_v0_8 >> 2];
            *var_a1_7 = *temp_v1_6;
            var_a1_7 += 4;
            sp30A4[(s32) *temp_v1_6] = var_t2_5;
            var_t2_5 += 1;
            var_a0_9 += temp_s4_2;
        } while ((s32) var_t2_5 < temp_s2);
    }
    var_a2_10 = 0;
    if (var_s3 > 0) {
        var_a0_10 = &sp48;
        do {
            var_t0_8 = *var_a0_10;
            var_t1_5 = *var_t0_8;
            if (var_t1_5 != NULL) {
                do {
                    var_a3_10 = 0;
                    var_v1_12 = var_t0_8;
loop_135:
                    var_a3_10 += 1;
                    M2C_FIELD(var_v1_12, s32 *, 8) = (s32) sp30A4[M2C_FIELD(var_v1_12, s32 *, 8)];
                    var_v1_12 += 4;
                    if (var_a3_10 < 3) {
                        goto loop_135;
                    }
                    var_t0_8 = var_t1_5;
                    var_t1_5 = *var_t1_5;
                } while (var_t1_5 != NULL);
            }
            var_a2_10 += 1;
            var_a0_10 += 0x30;
        } while (var_a2_10 < var_s3);
    }
    var_t2_6 = 0;
    var_s7 = 0;
    if (temp_s2 > 0) {
        var_a1_8 = 0;
        do {
            var_v1_13 = 0;
            var_a2_10 = 0;
            if (var_s3 > 0) {
                var_v0_9 = var_a1_8;
                do {
                    var_a2_10 += 1;
                    var_v1_13 |= M2C_FIELD((var_v0_9 + temp_s1_2), u8 *, 4);
                    var_v0_9 = var_a2_10 + var_a1_8;
                } while (var_a2_10 < var_s3);
            }
            var_s7 += var_v1_13 & 1;
            var_t2_6 += 1;
            var_a1_8 += temp_s4_2;
        } while (var_t2_6 < temp_s2);
    }
    var_s0_2 = -1;
    var_a0_11 = sp308C;
    var_t5_3 = 0;
    temp_a1 = Heap_Alloc(&gGameHeap, temp_s2 * 4, (s32 *) var_a2_10);
    if (var_s6_2 > 0) {
        var_t3_3 = temp_fp;
        var_t1_6 = NULL;
        do {
            var_a2_11 = 0;
            var_v1_14 = -1;
            var_t2_7 = 0;
            temp_t4_3 = (var_t5_3 + *var_t3_3) - 1;
            if (temp_s2 >= 0) {
                var_t0_9 = temp_a1;
                var_a3_11 = var_t5_3;
                do {
                    if ((var_t2_7 != temp_s2) && (M2C_FIELD((var_a3_11 + temp_s1_2), u8 *, 4) & 1)) {
                        *var_t0_9 = var_t1_6;
                        var_t1_6 += 1;
                        if (var_v1_14 >= 0) {
                            var_a2_11 += 1;
                        } else {
                            var_v1_14 = var_t2_7;
                            var_a2_11 = 1;
                        }
                    } else if (var_v1_14 >= 0) {
                        M2C_FIELD(var_a0_11, u16 *, 0) = 2;
                        temp_a0_3 = var_a0_11 + 2;
                        M2C_FIELD(var_a0_11, u16 *, 2) = (u16) M2C_FIELD(&temp_a1[var_v1_14], u16 *, 2);
                        temp_a0_4 = temp_a0_3 + 2;
                        M2C_FIELD(temp_a0_3, s16 *, 2) = var_v1_14;
                        M2C_FIELD(temp_a0_4, s16 *, 2) = var_a2_11;
                        var_a0_11 = temp_a0_4 + 2 + 2;
                        var_v1_14 = -1;
                    }
                    var_t0_9 += 4;
                    var_t2_7 += 1;
                    var_a3_11 += temp_s4_2;
                } while (temp_s2 >= var_t2_7);
            }
            var_a2_12 = var_t5_3;
            if ((s32) temp_t4_3 >= var_t5_3) {
                var_t2_8 = (var_t5_3 * 0x30) + &sp48;
                do {
                    var_t0_10 = *var_t2_8;
                    var_t1_7 = *var_t0_10;
                    if (var_t1_7 != NULL) {
                        do {
                            temp_v0_4 = M2C_FIELD(var_t0_10, s32 *, 0x14);
                            if (var_s0_2 != temp_v0_4) {
                                var_s0_2 = temp_v0_4;
                                M2C_FIELD(var_a0_11, u16 *, 0) = 1;
                                M2C_FIELD(var_a0_11, s16 *, 2) = (s16) var_s0_2;
                                var_a0_11 = var_a0_11 + 2 + 2;
                            }
                            *var_a0_11 = 3;
                            var_a0_11 += 2;
                            var_a3_12 = 0;
                            var_v1_15 = var_t0_10;
loop_162:
                            temp_v0_5 = M2C_FIELD(&temp_a1[M2C_FIELD(var_v1_15, s32 *, 8)], u16 *, 2);
                            var_v1_15 += 4;
                            var_a3_12 += 1;
                            *var_a0_11 = temp_v0_5;
                            var_a0_11 += 2;
                            if (var_a3_12 < 3) {
                                goto loop_162;
                            }
                            var_t0_10 = var_t1_7;
                            var_t1_7 = *var_t1_7;
                        } while (var_t1_7 != NULL);
                    }
                    var_a2_12 += 1;
                    var_t2_8 += 0x30;
                } while ((s32) temp_t4_3 >= var_a2_12);
            }
            temp_v0_6 = *var_t3_3;
            var_t3_3 += 4;
            var_t5_3 = (s32) (var_t5_3 + temp_v0_6);
            var_t1_6 = NULL;
        } while ((s32) var_t3_3 < (s32) &temp_fp[var_s6_2]);
    }
    *var_a0_11 = 0;
    Heap_Free(&gGameHeap, temp_a1);
    Heap_Free(&gGameHeap, temp_fp);
    Heap_Free(&gGameHeap, temp_s1_2);
    Heap_Free(&gGameHeap, sp309C);
    Heap_Free(&gGameHeap, temp_s5);
    Heap_Free(&gGameHeap, sp30A4);
    return var_s7;
}

void func_800094D0(void *arg0, s32 arg1, s32 arg2) {
    s32 *var_a3;
    s32 temp_a2;
    s32 var_a1;
    s32 var_v1;
    u32 temp_lo;

    temp_a2 = (arg2 + 7) & ~7;
    temp_lo = (u32) (arg1 - 0xC) / (u32) (temp_a2 + 4);
    var_a3 = arg0 + 8;
    M2C_FIELD(arg0, s32 **, 0) = var_a3;
    var_v1 = var_a3 + (((temp_lo * 4) + 0xB) & ~7);
    var_a1 = temp_lo - 1;
    M2C_FIELD(arg0, s32 **, 4) = var_a3;
    if (var_a1 != -1) {
        do {
            *var_a3 = var_v1;
            var_a3 += 4;
            var_a1 -= 1;
            var_v1 += temp_a2;
        } while (var_a1 != -1);
    }
    *var_a3 = 0;
}

s32 func_8000953C(s32 **arg0) {
    s32 *temp_v1;
    s32 var_v0;

    temp_v1 = *arg0;
    *arg0 = temp_v1 + 4;
    var_v0 = *temp_v1;
    if (var_v0 == 0) {
        *arg0 = temp_v1;
        var_v0 = 0;
    }
    return var_v0;
}

s32 func_80009564(s32 **arg0) {
    s32 *temp_a0;
    s32 *var_a0;
    s32 var_v1;

    temp_a0 = *arg0;
    var_v1 = 0;
    var_a0 = temp_a0 + 4;
    if (*temp_a0 != 0) {
        do {
            var_v1 += 1;
            var_a0 += 4;
        } while (*var_a0 != 0);
    }
    return var_v1;
}

void func_80009590(void **arg0, s32 arg1) {
    void *temp_v1;

    temp_v1 = *arg0;
    *arg0 = temp_v1 - 4;
    M2C_FIELD(temp_v1, s32 *, -4) = arg1;
}

void func_800095B0(f32 *arg0, void *arg1, f32 *arg2, f32 *arg3, f32 arg4, f32 arg5, f32 arg6) {
    f32 temp_f12;
    f32 temp_f14;
    f32 temp_f14_2;
    f32 temp_f16;
    f32 temp_f20;
    f32 temp_f20_2;
    f32 temp_f22;
    f32 temp_f22_2;
    f32 temp_f24;
    f32 temp_f24_2;
    f32 temp_f26;
    f32 temp_f26_2;
    f32 temp_f28;
    f32 temp_f28_2;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 temp_f2_4;
    f32 temp_f2_5;
    f32 temp_f2_6;
    f32 temp_f2_7;
    f32 temp_f2_8;
    f32 temp_f2_9;
    f32 temp_f30;
    f32 temp_f30_2;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 temp_f4_3;
    f32 var_f4;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;
    s32 var_v0_4;
    s32 var_v0_5;

    temp_f2 = M2C_FIELD(arg3, f32 *, 0) - M2C_FIELD(arg2, f32 *, 0);
    temp_f4 = M2C_FIELD(arg3, f32 *, 4) - M2C_FIELD(arg2, f32 *, 4);
    temp_f4_2 = M2C_FIELD(arg3, f32 *, 8) - M2C_FIELD(arg2, f32 *, 8);
    temp_f12 = (temp_f2 * temp_f2) + (temp_f4 * temp_f4) + (temp_f4_2 * temp_f4_2);
    var_f4 = sqrtf(temp_f12);
    if (var_f4 != var_f4) {
        var_f4 = sqrtf(temp_f12, temp_f4);
    }
    temp_f2_2 = -1.0f / var_f4;
    temp_f16 = temp_f4_2 * temp_f2_2;
    temp_f14 = temp_f2 * temp_f2_2;
    temp_f14_2 = temp_f4 * temp_f2_2;
    temp_f26 = (arg5 * temp_f16) - (arg6 * temp_f14_2);
    temp_f28 = (arg6 * temp_f14) - (arg4 * temp_f16);
    temp_f30 = (arg4 * temp_f14_2) - (arg5 * temp_f14);
    temp_f2_3 = 1.0f / sqrtf((temp_f26 * temp_f26) + (temp_f28 * temp_f28) + (temp_f30 * temp_f30));
    temp_f26_2 = temp_f26 * temp_f2_3;
    temp_f30_2 = temp_f30 * temp_f2_3;
    temp_f28_2 = temp_f28 * temp_f2_3;
    temp_f24 = (temp_f14_2 * temp_f30_2) - (temp_f16 * temp_f28_2);
    temp_f22 = (temp_f16 * temp_f26_2) - (temp_f14 * temp_f30_2);
    temp_f20 = (temp_f14 * temp_f28_2) - (temp_f14_2 * temp_f26_2);
    temp_f2_4 = 1.0f / sqrtf((temp_f24 * temp_f24) + (temp_f22 * temp_f22) + (temp_f20 * temp_f20));
    temp_f4_3 = temp_f26_2 * 128.0f;
    temp_f24_2 = temp_f24 * temp_f2_4;
    temp_f22_2 = temp_f22 * temp_f2_4;
    temp_f20_2 = temp_f20 * temp_f2_4;
    if (temp_f4_3 < 127.0f) {
        var_v0 = (s32) temp_f4_3;
    } else {
        var_v0 = 0x7F;
    }
    temp_f2_5 = temp_f28_2 * 128.0f;
    M2C_FIELD(arg1, s8 *, 8) = (s8) var_v0;
    if (temp_f2_5 < 127.0f) {
        var_v0_2 = (s32) temp_f2_5;
    } else {
        var_v0_2 = 0x7F;
    }
    temp_f2_6 = temp_f30_2 * 128.0f;
    M2C_FIELD(arg1, s8 *, 9) = (s8) var_v0_2;
    if (temp_f2_6 < 127.0f) {
        var_v0_3 = (s32) temp_f2_6;
    } else {
        var_v0_3 = 0x7F;
    }
    temp_f2_7 = temp_f24_2 * 128.0f;
    M2C_FIELD(arg1, s8 *, 0xA) = (s8) var_v0_3;
    if (temp_f2_7 < 127.0f) {
        var_v0_4 = (s32) temp_f2_7;
    } else {
        var_v0_4 = 0x7F;
    }
    temp_f2_8 = temp_f22_2 * 128.0f;
    M2C_FIELD(arg1, s8 *, 0x18) = (s8) var_v0_4;
    if (temp_f2_8 < 127.0f) {
        var_v0_5 = (s32) temp_f2_8;
    } else {
        var_v0_5 = 0x7F;
    }
    temp_f2_9 = temp_f20_2 * 128.0f;
    M2C_FIELD(arg1, s8 *, 0x19) = (s8) var_v0_5;
    if (temp_f2_9 < 127.0f) {
        M2C_FIELD(arg1, s8 *, 0x1A) = (s8) (s32) temp_f2_9;
    } else {
        M2C_FIELD(arg1, s8 *, 0x1A) = 0x7F;
    }
    M2C_FIELD(arg1, s8 *, 0) = 0;
    M2C_FIELD(arg1, s8 *, 1) = 0;
    M2C_FIELD(arg1, s8 *, 2) = 0;
    M2C_FIELD(arg1, s8 *, 3) = 0;
    M2C_FIELD(arg1, s8 *, 4) = 0;
    M2C_FIELD(arg1, s8 *, 5) = 0;
    M2C_FIELD(arg1, s8 *, 6) = 0;
    M2C_FIELD(arg1, s8 *, 7) = 0;
    M2C_FIELD(arg1, s8 *, 0x10) = 0;
    M2C_FIELD(arg1, s8 *, 0x11) = 0x80;
    M2C_FIELD(arg1, s8 *, 0x12) = 0;
    M2C_FIELD(arg1, s8 *, 0x13) = 0;
    M2C_FIELD(arg1, s8 *, 0x14) = 0;
    M2C_FIELD(arg1, s8 *, 0x15) = 0x80;
    M2C_FIELD(arg1, s8 *, 0x16) = 0;
    M2C_FIELD(arg1, s8 *, 0x17) = 0;
    M2C_FIELD(arg0, f32 *, 0) = temp_f26_2;
    arg0[4] = temp_f28_2;
    arg0[8] = temp_f30_2;
    arg0[1] = temp_f24_2;
    arg0[5] = temp_f22_2;
    arg0[9] = temp_f20_2;
    arg0[0xC] = -((M2C_FIELD(arg2, f32 *, 0) * temp_f26_2) + (M2C_FIELD(arg2, f32 *, 4) * temp_f28_2) + (M2C_FIELD(arg2, f32 *, 8) * temp_f30_2));
    arg0[2] = temp_f14;
    arg0[6] = temp_f14_2;
    arg0[0xA] = temp_f16;
    arg0[0xD] = -((M2C_FIELD(arg2, f32 *, 0) * temp_f24_2) + (M2C_FIELD(arg2, f32 *, 4) * temp_f22_2) + (M2C_FIELD(arg2, f32 *, 8) * temp_f20_2));
    arg0[0xE] = -((M2C_FIELD(arg2, f32 *, 0) * temp_f14) + (M2C_FIELD(arg2, f32 *, 4) * temp_f14_2) + (M2C_FIELD(arg2, f32 *, 8) * temp_f16));
    arg0[3] = 0.0f;
    arg0[7] = 0.0f;
    arg0[0xB] = 0.0f;
    arg0[0xF] = 1.0f;
}

s32 func_80009AD4(f32 *arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    f32 *var_a0;
    f32 *var_a0_2;
    f32 *var_a1;
    f32 *var_a1_2;
    f32 temp_f20;
    f32 temp_f20_2;
    f32 temp_f20_3;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    s32 var_a2;
    s32 var_a2_2;
    s32 var_a3;
    s32 var_v0;
    s32 var_v1;
    s32 var_v1_2;

    temp_f20 = (f32) ((f64) arg1 * 0.017453292519943295) / 2.0f;
    temp_f20_2 = func_8001C450(temp_f20);
    var_a2 = 0;
    var_a1 = arg0;
    temp_f20_3 = temp_f20_2 / func_8001C5A0(temp_f20);
    do {
        var_v1 = 0;
        var_a0 = var_a1;
loop_2:
        if (var_a2 != var_v1) {
            *var_a0 = 0.0f;
        } else {
            *var_a0 = 1.0f;
        }
        var_v1 += 1;
        var_a0 += 4;
        if (var_v1 < 4) {
            goto loop_2;
        }
        var_a2 += 1;
        var_a1 += 0x10;
    } while (var_a2 < 4);
    temp_f2 = arg3 - arg4;
    var_a2_2 = 0;
    var_a0_2 = arg0;
    var_a0_2[5] = temp_f20_3;
    var_a0_2[0xF] = 0.0f;
    var_a0_2[0xB] = -1.0f;
    M2C_FIELD(var_a0_2, f32 *, 0) = temp_f20_3 / arg2;
    var_a0_2[0xA] = (arg3 + arg4) / temp_f2;
    var_a0_2[0xE] = (2.0f * arg3 * arg4) / temp_f2;
    do {
        var_v1_2 = 0;
        var_a1_2 = var_a0_2;
loop_10:
        var_v1_2 += 1;
        *var_a1_2 *= arg5;
        var_a1_2 += 4;
        if (var_v1_2 < 4) {
            goto loop_10;
        }
        var_a2_2 += 1;
        var_a0_2 += 0x10;
    } while (var_a2_2 < 4);
    temp_f2_2 = arg3 + arg4;
    var_a3 = 0xFFFF;
    if (!(temp_f2_2 <= 2.0f)) {
        temp_f2_3 = 131072.0f / temp_f2_2;
        if (temp_f2_3 >= 2.1474836e9f) {
            var_a3 = (s32) (temp_f2_3 - 2.1474836e9f) | 0x80000000;
            var_v0 = var_a3 & 0xFFFF;
        } else {
            var_a3 = (s32) temp_f2_3;
            var_v0 = var_a3 & 0xFFFF;
        }
        if (var_v0 == 0) {
            var_a3 = 1;
        }
    }
    return var_a3 & 0xFFFF;
}

s32 func_80009CB0(s32 *arg0) {
    s32 var_v1;

    var_v1 = (*arg0 * 0x41C64E6D) + 0x3039;
    *arg0 = var_v1;
    if (var_v1 < 0) {
        var_v1 += 0xFFFF;
    }
    return (var_v1 >> 0x10) & 0x7FFF;
}

u32 func_80009CE8(s32 *arg0, s32 arg1) {
    s32 var_v1;

    var_v1 = (*arg0 * 0x41C64E6D) + 0x3039;
    *arg0 = var_v1;
    if (var_v1 < 0) {
        var_v1 += 0xFFFF;
    }
    return (u32) (((var_v1 >> 0x10) & 0x7FFF) * arg1) >> 0xF;
}

void func_80009D40(M2C_UNK *arg0, s32 arg1, s32 arg2) {
    s32 *var_a3;
    s32 temp_a2;
    s32 var_a1;
    s32 var_v1;
    u32 temp_lo;

    temp_a2 = (arg2 + 7) & ~7;
    temp_lo = (u32) (arg1 - 0x14) / (u32) (temp_a2 + 4);
    var_a3 = arg0 + 0x10;
    M2C_FIELD(arg0, s32 **, 0xC) = var_a3;
    M2C_FIELD(arg0, s32 **, 4) = var_a3;
    M2C_FIELD(arg0, s32 **, 0) = var_a3;
    var_v1 = var_a3 + (((temp_lo * 4) + 0xB) & ~7);
    var_a1 = temp_lo - 1;
    M2C_FIELD(arg0, s32 *, 8) = 0;
    if (var_a1 != -1) {
        do {
            *var_a3 = var_v1;
            var_a3 += 4;
            var_a1 -= 1;
            var_v1 += temp_a2;
        } while (var_a1 != -1);
    }
    *var_a3 = 0;
}

M2C_UNK *func_80009DB4(M2C_UNK *arg0) {
    M2C_UNK **temp_v1;
    M2C_UNK **temp_v1_2;
    M2C_UNK *var_v0;

    temp_v1 = M2C_FIELD(arg0, M2C_UNK ***, 4);
    if (temp_v1 == M2C_FIELD(arg0, s32 *, 8)) {
        return NULL;
    }
    M2C_FIELD(arg0, M2C_UNK ***, 4) = (M2C_UNK **) (temp_v1 + 4);
    var_v0 = *temp_v1;
    if (var_v0 == NULL) {
        temp_v1_2 = M2C_FIELD(arg0, M2C_UNK ***, 0);
        M2C_FIELD(arg0, M2C_UNK ***, 4) = (M2C_UNK **) (temp_v1_2 + 4);
        var_v0 = *temp_v1_2;
    }
    return var_v0;
}

s32 func_80009DF4(M2C_UNK *arg0) {
    s32 *var_v1;
    s32 temp_v0;
    s32 var_a1;

    var_v1 = M2C_FIELD(arg0, s32 **, 4);
    temp_v0 = M2C_FIELD(arg0, s32 *, 0xC);
    var_a1 = 0;
    if (var_v1 != temp_v0) {
        do {
            var_v1 += 4;
            if (*var_v1 == 0) {
                var_v1 = M2C_FIELD(arg0, s32 **, 0);
            }
            var_a1 += 1;
        } while (var_v1 != temp_v0);
    }
    return var_a1;
}

void func_80009E2C(M2C_UNK *arg0, M2C_UNK *arg1) {
    M2C_UNK **temp_v1;

    temp_v1 = M2C_FIELD(arg0, M2C_UNK ***, 0xC);
    M2C_FIELD(arg0, M2C_UNK ***, 0xC) = (M2C_UNK **) (temp_v1 + 4);
    *temp_v1 = arg1;
    if (*M2C_FIELD(arg0, M2C_UNK ***, 0xC) == NULL) {
        M2C_FIELD(arg0, M2C_UNK ***, 0xC) = (M2C_UNK **) M2C_FIELD(arg0, M2C_UNK ***, 0);
    }
    if (M2C_FIELD(arg0, M2C_UNK ***, 0xC) == M2C_FIELD(arg0, s32 *, 4)) {
        M2C_FIELD(arg0, s32 *, 8) = 0;
    }
}

void func_80009E6C(M2C_UNK *arg0) {
    s32 temp_v1;

    temp_v1 = M2C_FIELD(arg0, s32 *, 0xC);
    if (M2C_FIELD(arg0, s32 *, 4) != temp_v1) {
        M2C_FIELD(arg0, s32 *, 8) = temp_v1;
    }
}

void func_80009E90(void) {

}

void func_80009E98(void) {

}

void func_80009EA0(s32 arg0, M2C_UNK arg1, M2C_UNK arg2, M2C_UNK arg3) {

}

void SiffIter_Init(s32 *arg0) {
    s32 *var_v1;
    s32 var_v0;

    var_v1 = arg0;
    var_v0 = 0x41;
    do {
        *var_v1 = 0;
        var_v0 -= 1;
        var_v1 += 4;
    } while (var_v0 != 0);
    *arg0 = -1;
}

void SiffWriter_Begin(s32 *arg0, s32 *arg1, s32 arg2) {
    *arg0 += 1;
    *arg1 = arg2;
    M2C_FIELD((arg0 + (*arg0 * 0x10)), void **, 4) = (void *) (arg1 + 8);
}

void *SiffWriter_End(s32 *arg0, s32 arg1) {
    s32 temp_v1;
    s32 var_a1;
    s32 var_v0;
    void *temp_a2;

    var_a1 = arg1;
    temp_a2 = M2C_FIELD((arg0 + (*arg0 * 0x10)), void **, 4);
    temp_v1 = (var_a1 + 3) & ~3;
    var_v0 = var_a1 - temp_a2;
    if (var_a1 < temp_v1) {
        do {
            *var_a1 = 0;
            var_a1 += 1;
        } while (var_a1 < temp_v1);
        var_v0 = var_a1 - temp_a2;
    }
    M2C_FIELD(temp_a2, s32 *, -4) = var_v0;
    *arg0 -= 1;
    return temp_a2 + var_v0;
}

void SiffIter_Enter(s32 *arg0, M2C_UNK *arg1) {
    s32 temp_v1;
    void *temp_a0;

    temp_v1 = *arg0 + 1;
    *arg0 = temp_v1;
    M2C_FIELD((arg0 + (temp_v1 * 0x10)), s32 *, 8) = (s32) M2C_FIELD(arg1, M2C_UNK **, 0);
    M2C_FIELD((arg0 + (*arg0 * 0x10)), s32 *, 0xC) = (s32) arg1[1];
    temp_a0 = arg0 + (*arg0 * 0x10);
    M2C_FIELD(temp_a0, void **, 0x10) = (void *) (arg1 + 8 + M2C_FIELD(temp_a0, s32 *, 0xC));
}

M2C_UNK *SiffIter_Leave(s32 *arg0) {
    s32 temp_v1;

    temp_v1 = *arg0;
    *arg0 = temp_v1 - 1;
    return M2C_FIELD((arg0 + (temp_v1 * 0x10)), M2C_UNK **, 0x10);
}

u8 *Siff_WriteString(void *arg0, s32 arg1, u8 *arg2) {
    s32 temp_v1;
    s32 var_v0;
    u8 *var_a0;
    u8 *var_a2;
    u8 temp_v0;
    void *temp_a0;

    var_a2 = arg2;
    M2C_FIELD(arg0, s32 *, 0) = arg1;
    temp_a0 = arg0 + 4;
    var_a0 = temp_a0 + 4;
    do {
        temp_v0 = *var_a2;
        var_a2 += 1;
        *var_a0 = temp_v0;
        var_a0 += 1;
    } while (temp_v0 != 0);
    temp_v1 = (s32) (var_a0 + 3) & ~3;
    var_v0 = var_a0 - temp_a0;
    if ((s32) var_a0 < temp_v1) {
        do {
            *var_a0 = 0;
            var_a0 += 1;
        } while ((s32) var_a0 < temp_v1);
        var_v0 = var_a0 - temp_a0;
    }
    M2C_FIELD(arg0, s32 *, 4) = (s32) (var_v0 - 4);
    return var_a0;
}

void *Siff_WriteU32(void *arg0, s32 arg1, s32 arg2) {
    M2C_FIELD(arg0, s32 *, 4) = 4;
    M2C_FIELD(arg0, s32 *, 0) = arg1;
    M2C_FIELD(arg0, s32 *, 8) = arg2;
    return arg0 + 0xC;
}

s32 *Siff_WriteData(void *arg0, s32 arg1, s32 *arg2, s32 arg3) {
    s32 *var_a0;
    s32 *var_a2;
    s32 temp_v0;
    s32 var_a3;

    var_a2 = arg2;
    var_a3 = (arg3 + 3) & ~3;
    M2C_FIELD(arg0, s32 *, 0) = arg1;
    M2C_FIELD(arg0, s32 *, 4) = var_a3;
    var_a0 = arg0 + 4 + 4;
    do {
        temp_v0 = *var_a2;
        var_a2 += 4;
        var_a3 -= 4;
        *var_a0 = temp_v0;
        var_a0 += 4;
    } while (var_a3 != 0);
    return var_a0;
}

void func_8000A090(void) {

}

void func_8000A0A0(f32 *arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    f32 *var_a2;
    f32 *var_v1;
    f32 temp_f0;
    f32 temp_f14;
    f32 temp_f18;
    f32 temp_f18_2;
    f32 temp_f18_3;
    f32 temp_f18_4;
    f32 temp_f2;
    f32 temp_f4;
    f32 temp_f6;
    f32 temp_f8;
    s32 var_a0;
    s32 var_a1;

    var_a1 = 0;
    var_a2 = arg0;
    do {
        var_a0 = 0;
        var_v1 = var_a2;
loop_2:
        if (var_a1 != var_a0) {
            *var_v1 = 0.0f;
        } else {
            *var_v1 = 1.0f;
        }
        var_a0 += 1;
        var_v1 += 4;
        if (var_a0 < 4) {
            goto loop_2;
        }
        var_a1 += 1;
        var_a2 += 0x10;
    } while (var_a1 < 4);
    Vec3_NormalizeXYZ(&arg3, &arg4, &arg5);
    temp_f18 = 1.0f - arg2;
    temp_f14 = arg3 * arg4 * temp_f18;
    temp_f8 = arg4 * arg5 * temp_f18;
    temp_f6 = arg5 * arg3 * temp_f18;
    temp_f18_2 = arg3 * arg3;
    M2C_FIELD(arg0, f32 *, 0) = temp_f18_2 + (arg2 * (1.0f - temp_f18_2));
    temp_f18_3 = arg4 * arg4;
    temp_f2 = arg4 * arg1;
    temp_f4 = arg3 * arg1;
    M2C_FIELD(arg0, f32 *, 0x20) = (f32) (temp_f6 + temp_f2);
    temp_f18_4 = arg5 * arg5;
    temp_f0 = arg5 * arg1;
    M2C_FIELD(arg0, f32 *, 0x24) = (f32) (temp_f8 - temp_f4);
    M2C_FIELD(arg0, f32 *, 0x18) = (f32) (temp_f8 + temp_f4);
    M2C_FIELD(arg0, f32 *, 8) = (f32) (temp_f6 - temp_f2);
    M2C_FIELD(arg0, f32 *, 0x14) = (f32) (temp_f18_3 + (arg2 * (1.0f - temp_f18_3)));
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) (temp_f14 - temp_f0);
    M2C_FIELD(arg0, f32 *, 4) = (f32) (temp_f14 + temp_f0);
    M2C_FIELD(arg0, f32 *, 0x28) = (f32) (temp_f18_4 + (arg2 * (1.0f - temp_f18_4)));
}

void func_8000A1F4(f32 *arg0, f32 *arg1, M2C_UNK *arg2) {
    f32 sp10;
    f32 sp2C;
    f32 sp30;
    f32 sp34;
    f32 *var_a2;
    f32 *var_v1;
    f32 temp_f0;
    f32 temp_f12;
    f32 temp_f14;
    f32 temp_f18;
    f32 temp_f18_2;
    f32 temp_f18_3;
    f32 temp_f18_4;
    f32 temp_f20;
    f32 temp_f2;
    f32 temp_f4;
    f32 temp_f6;
    f32 temp_f8;
    f32 var_f0;
    s32 var_a0;
    s32 var_a1;

    temp_f20 = -Vec3_CosAngle(arg1, arg2);
    temp_f12 = 1.0f - (temp_f20 * temp_f20);
    var_f0 = sqrtf(temp_f12);
    if (var_f0 != var_f0) {
        var_f0 = sqrtf(temp_f12, M2C_BITWISE(f32, &sp10));
    }
    Plane_FromPoints(&sp10, arg1, arg2, &D_80038400);
    var_a1 = 0;
    var_a2 = arg0;
    sp2C = sp10;
    sp30 = sp14;
    sp34 = sp18;
    do {
        var_a0 = 0;
        var_v1 = var_a2;
loop_4:
        if (var_a1 != var_a0) {
            *var_v1 = 0.0f;
        } else {
            *var_v1 = 1.0f;
        }
        var_a0 += 1;
        var_v1 += 4;
        if (var_a0 < 4) {
            goto loop_4;
        }
        var_a1 += 1;
        var_a2 += 0x10;
    } while (var_a1 < 4);
    Vec3_NormalizeXYZ(&sp2C, &sp30, &sp34);
    temp_f18 = 1.0f - temp_f20;
    temp_f14 = sp2C * sp30 * temp_f18;
    temp_f8 = sp30 * sp34 * temp_f18;
    temp_f6 = sp34 * sp2C * temp_f18;
    temp_f18_2 = sp2C * sp2C;
    M2C_FIELD(arg0, f32 *, 0) = temp_f18_2 + (temp_f20 * (1.0f - temp_f18_2));
    temp_f18_3 = sp30 * sp30;
    temp_f2 = sp30 * var_f0;
    temp_f4 = sp2C * var_f0;
    M2C_FIELD(arg0, f32 *, 0x20) = (f32) (temp_f6 + temp_f2);
    temp_f18_4 = sp34 * sp34;
    temp_f0 = sp34 * var_f0;
    M2C_FIELD(arg0, f32 *, 0x24) = (f32) (temp_f8 - temp_f4);
    M2C_FIELD(arg0, f32 *, 0x18) = (f32) (temp_f8 + temp_f4);
    M2C_FIELD(arg0, f32 *, 8) = (f32) (temp_f6 - temp_f2);
    M2C_FIELD(arg0, f32 *, 0x14) = (f32) (temp_f18_3 + (temp_f20 * (1.0f - temp_f18_3)));
    M2C_FIELD(arg0, f32 *, 0x10) = (f32) (temp_f14 - temp_f0);
    M2C_FIELD(arg0, f32 *, 4) = (f32) (temp_f14 + temp_f0);
    M2C_FIELD(arg0, f32 *, 0x28) = (f32) (temp_f18_4 + (temp_f20 * (1.0f - temp_f18_4)));
}

f32 func_8000A3D0(f32 arg0, f32 arg1, f32 arg2, f32 arg3) {
    f32 temp_f2;

    temp_f2 = 1.0f - arg0;
    return (((temp_f2 * temp_f2) / 2.0f) * arg1) + (((temp_f2 * arg0) + 0.5f) * arg2) + (((arg0 * arg0) / 2.0f) * arg3);
}

s32 func_8000A440(void *arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f12;
    f32 temp_f20;
    f32 temp_f22;
    f32 var_f2;
    s32 var_v0;

    temp_f20 = arg3 - arg2;
    temp_f22 = (arg2 - (2.0f * arg3)) + arg4;
    temp_f0 = (arg2 + arg3) - (arg1 + arg1);
    if (temp_f22 == 0.0f) {
        if (temp_f20 != 0.0f) {
            M2C_FIELD(arg0, f32 *, 0) = (f32) (-temp_f0 / temp_f20);
        } else {
            M2C_FIELD(arg0, f32 *, 0) = 0.0f;
        }
        return 1;
    }
    temp_f12 = (temp_f20 * temp_f20) - (temp_f22 * temp_f0);
    var_v0 = 0;
    if (!(temp_f12 < 0.0f)) {
        if (temp_f12 == 0.0f) {
            M2C_FIELD(arg0, f32 *, 0) = (f32) (-temp_f20 / temp_f22);
            return 1;
        }
        var_f2 = sqrtf(temp_f12);
        if (var_f2 != var_f2) {
            var_f2 = sqrtf(temp_f12);
        }
        temp_f0_2 = -temp_f20;
        var_v0 = 2;
        M2C_FIELD(arg0, f32 *, 0) = (f32) ((temp_f0_2 - var_f2) / temp_f22);
        M2C_FIELD(arg0, f32 *, 4) = (f32) ((temp_f0_2 + var_f2) / temp_f22);
        /* Duplicate return node #13. Try simplifying control flow for better match */
        return var_v0;
    }
    return var_v0;
}

f32 func_8000A574(f32 arg0, f32 arg1, f32 arg2, f32 arg3) {
    return (((arg1 - (2.0f * arg2)) + arg3) * arg0) + (arg2 - arg1);
}

void func_8000A5A0(void) {
    s32 sp10;
    s32 sp14;
    s32 sp18;
    s32 sp1C;
    M2C_UNK *var_s1;
    s32 var_s0;

    var_s1 = &D_80001;
    var_s0 = 0;
    sp14 = 0;
    sp18 = 0;
    sp1C = 0x2000;
    do {
        sp10 = (((s32) var_s1 / 2) << 0xD) | 0;
        func_8000ADA4(var_s0, &sp10);
        var_s0 += 1;
    } while (var_s0 < 0x20);
}

s32 func_8000A628(s32 arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    s32 sp10;
    s32 sp14;
    s32 sp18;
    s32 sp1C;
    s32 temp_v1;
    struct _struct_D_80038410_0xC *temp_a0;

    temp_a0 = &D_80038410[arg3];
    if (temp_a0->unk0 == arg3) {
        temp_v1 = ((-(arg4 != 0) & 3) | 2) * 8;
        sp10 = arg1;
        sp14 = (arg2 >> 6) | temp_v1 | 7;
        sp18 = ((s32) (arg2 + temp_a0->unk4) >> 6) | temp_v1 | 7;
        sp1C = temp_a0->unk8;
        func_8000ADA4(arg0, &sp10);
        return 1;
    }
    return 0;
}

void func_8000A6D0(M2C_UNK *arg0, s32 arg1, s32 arg2) {
    M2C_UNK **temp_v1;
    s32 *var_a3;
    s32 var_a1;
    s32 var_v1;
    u32 temp_lo;
    void *temp_v0;

    temp_lo = (u32) (arg1 - 0x30) / (u32) (arg2 + 4);
    var_a3 = arg0 + 0x2C;
    D_800C51DC = arg0;
    M2C_FIELD(arg0, s32 *, 0) = arg2;
    var_v1 = (var_a3 + ((temp_lo + 1) * 4) + 0x10) & ~0xF;
    var_a1 = temp_lo - 1;
    M2C_FIELD(arg0, s32 **, 8) = var_a3;
    if (var_a1 != -1) {
        do {
            *var_a3 = var_v1;
            var_a3 += 4;
            var_a1 -= 1;
            var_v1 += arg2;
        } while (var_a1 != -1);
    }
    temp_v1 = arg0 + 0xC;
    temp_v0 = arg0 + 0x1C;
    *var_a3 = 0;
    *D_800B05A4 = temp_v1;
    D_800C53FC = temp_v0;
    M2C_FIELD(arg0, s32 *, 0x10) = 0;
    M2C_FIELD(arg0, s32 *, 0x1C) = 0;
    M2C_FIELD(arg0, void **, 0xC) = temp_v0;
    M2C_FIELD(arg0, M2C_UNK ***, 0x20) = temp_v1;
}

M2C_UNK *func_8000A76C(void (*arg0)(), s32 arg1, s32 arg2, M2C_UNK *arg3) {
    M2C_UNK **temp_s0;
    M2C_UNK **temp_v1;
    M2C_UNK **var_a0;
    M2C_UNK **var_a1;
    M2C_UNK **var_a1_2;
    M2C_UNK *temp_a0;
    M2C_UNK *temp_s0_2;
    M2C_UNK *temp_s1;
    M2C_UNK *temp_v0;
    M2C_UNK *temp_v0_2;
    M2C_UNK *var_a0_2;
    M2C_UNK *var_v1;
    s32 var_v1_2;
    s32 var_v1_3;
    s32 var_v1_4;
    s32 var_v1_5;

    temp_v1 = M2C_FIELD(D_800C51DC, M2C_UNK ***, 8);
    M2C_FIELD(D_800C51DC, M2C_UNK ***, 8) = (M2C_UNK **) (temp_v1 + 4);
    temp_s1 = *temp_v1;
    if (temp_s1 != NULL) {
        M2C_FIELD(temp_s1, s32 *, 0xC) = arg1;
        var_v1 = **D_800B05A4;
        if (*var_v1 != NULL) {
loop_2:
            temp_a0 = M2C_FIELD(var_v1, M2C_UNK **, 0);
            if (arg1 >= M2C_FIELD(var_v1, s32 *, 0xC)) {
                var_v1 = temp_a0;
                if (*temp_a0 != NULL) {
                    goto loop_2;
                }
            }
        }
        *M2C_FIELD(var_v1, M2C_UNK **, 4) = temp_s1;
        M2C_FIELD(temp_s1, M2C_UNK **, 0) = var_v1;
        M2C_FIELD(temp_s1, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(var_v1, M2C_UNK **, 4);
        M2C_FIELD(var_v1, M2C_UNK **, 4) = temp_s1;
        var_v1_2 = arg2;
        var_a1 = &arg3;
        temp_s0 = (temp_s1 + M2C_FIELD(D_800C51DC, s32 *, 0)) - var_v1_2;
        var_a0 = temp_s0;
        if (var_v1_2 < 0) {
            var_v1_2 += 3;
        }
        var_v1_3 = (var_v1_2 >> 2) - 1;
        if (var_v1_3 != -1) {
            do {
                temp_v0 = *var_a1;
                var_a1 += 4;
                var_v1_3 -= 1;
                *var_a0 = temp_v0;
                var_a0 += 4;
            } while (var_v1_3 != -1);
        }
        temp_s0_2 = temp_s0 - 0xC0;
        func_8000B150(temp_s0_2, var_a1, -1);
        var_a1_2 = &arg3;
        var_v1_4 = arg2;
        var_a0_2 = temp_s0_2;
        M2C_FIELD(temp_s0_2, void (**)(), 0x38) = func_8000AE90;
        M2C_FIELD(temp_s0_2, void (**)(), 0x3C) = arg0;
        if (var_v1_4 < 0) {
            var_v1_4 += 3;
        }
        var_v1_5 = (var_v1_4 >> 2) - 1;
        if (var_v1_5 != -1) {
            do {
                temp_v0_2 = *var_a1_2;
                var_a1_2 += 4;
                var_v1_5 -= 1;
                *var_a0_2 = temp_v0_2;
                var_a0_2 += 4;
            } while (var_v1_5 != -1);
        }
        M2C_FIELD(temp_s1, M2C_UNK **, 8) = temp_s0_2;
    } else {
        M2C_FIELD(D_800C51DC, M2C_UNK ***, 8) = temp_v1;
    }
    return temp_s1;
}

void func_8000A8D0(void *arg0) {
    void *temp_a1;

    temp_a1 = M2C_FIELD(D_800C51DC, void **, 8);
    M2C_FIELD(D_800C51DC, void **, 8) = (void *) (temp_a1 - 4);
    M2C_FIELD(temp_a1, void **, -4) = arg0;
    *M2C_FIELD(arg0, void ***, 4) = M2C_FIELD(arg0, void **, 0);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0), void ***, 4) = (void **) M2C_FIELD(arg0, void ***, 4);
}

void func_8000A910(s32 arg0, M2C_UNK arg1, M2C_UNK arg2, M2C_UNK arg3) {

}

void func_8000A924(void *arg0, f32 *arg1) {
    f32 *var_a1;
    f32 temp_f0;
    f32 temp_f2_2;
    s32 temp_f2;
    s32 temp_f2_3;
    s32 temp_f4;
    s32 temp_f4_2;
    s32 var_t2;
    void *var_a0;
    void *var_t0;

    var_a0 = arg0;
    var_a1 = arg1;
    var_t0 = var_a0 + 0x20;
    var_t2 = 0;
    do {
        temp_f4 = (s32) (M2C_FIELD(var_a1, f32 *, 0) * 65536.0f);
        temp_f2 = (s32) (var_a1[1] * 65536.0f);
        M2C_FIELD(var_a0, s32 *, 0) = (s32) ((temp_f4 & 0xFFFF0000) | ((u32) temp_f2 >> 0x10));
        M2C_FIELD(var_t0, s32 *, 0) = (s32) (((temp_f4 << 0x10) & 0xFFFF0000) | (temp_f2 & 0xFFFF));
        temp_f2_2 = var_a1[2] * 65536.0f;
        temp_f0 = var_a1[3] * 65536.0f;
        var_t2 += 1;
        var_a1 += 0x10;
        temp_f4_2 = (s32) temp_f2_2;
        temp_f2_3 = (s32) temp_f0;
        M2C_FIELD(var_a0, s32 *, 4) = (s32) ((temp_f4_2 & 0xFFFF0000) | ((u32) temp_f2_3 >> 0x10));
        var_a0 = var_a0 + 4 + 4;
        M2C_FIELD(var_t0, s32 *, 4) = (s32) (((temp_f4_2 << 0x10) & 0xFFFF0000) | (temp_f2_3 & 0xFFFF));
        var_t0 = var_t0 + 4 + 4;
    } while (var_t2 < 4);
}

void func_8000A9E8(void *arg0, void *arg1) {
    s32 sp0;
    s32 sp4;
    f32 temp_f0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_a3;
    void *temp_a2;
    void *var_a0;
    void *var_a1;
    void *var_a2;

    var_a0 = arg0;
    var_a1 = arg1;
    var_a2 = var_a1 + 0x20;
    var_a3 = 0;
    do {
        temp_v0 = (M2C_FIELD(var_a1, s32 *, 0) & 0xFFFF0000) | M2C_FIELD(var_a2, u16 *, 0);
        sp0 = temp_v0;
        temp_v1 = ((M2C_FIELD(var_a1, s32 *, 0) << 0x10) & 0xFFFF0000) | M2C_FIELD(var_a2, u16 *, 2);
        temp_a2 = var_a2 + 4;
        sp4 = temp_v1;
        M2C_FIELD(var_a0, f32 *, 0) = (f32) ((f32) temp_v0 * 0.000015258789f);
        M2C_FIELD(var_a0, f32 *, 4) = (f32) ((f32) temp_v1 * 0.000015258789f);
        temp_v0_2 = (M2C_FIELD(var_a1, s32 *, 4) & 0xFFFF0000) | M2C_FIELD(var_a2, u16 *, 4);
        sp0 = temp_v0_2;
        temp_v1_2 = ((M2C_FIELD(var_a1, s32 *, 4) << 0x10) & 0xFFFF0000) | M2C_FIELD(temp_a2, u16 *, 2);
        temp_f0 = (f32) temp_v1_2 * 0.000015258789f;
        var_a3 += 1;
        var_a2 = temp_a2 + 4;
        var_a1 = var_a1 + 4 + 4;
        sp4 = temp_v1_2;
        M2C_FIELD(var_a0, f32 *, 8) = (f32) ((f32) temp_v0_2 * 0.000015258789f);
        M2C_FIELD(var_a0, f32 *, 0xC) = temp_f0;
        var_a0 += 0x10;
    } while (var_a3 < 4);
}

f32 func_8000AAE0(f32 arg0) {
    return fabsf(arg0);
}

void func_8000AAF0(void) {
    s32 var_at;
    s32 var_at_2;
    s32 var_at_3;
    s32 var_at_4;
    s32 var_at_5;
    s32 var_at_6;
    s32 var_at_7;

    *(s16 *)0x02000044 = 0xB0B;
    var_at = 0x780;
    do {
        var_at -= 1;
    } while (var_at != 0);
    *(s16 *)0x02000042 = 0;
    var_at_2 = 0x780;
    do {
        var_at_2 -= 1;
    } while (var_at_2 != 0);
    *(s16 *)0x02000042 = 0;
    var_at_3 = 0x780;
    do {
        var_at_3 -= 1;
    } while (var_at_3 != 0);
    *(s16 *)0x02000042 = 0;
    var_at_4 = 0x780;
    do {
        var_at_4 -= 1;
    } while (var_at_4 != 0);
    *(s16 *)0x02000042 = 0x4040;
    var_at_5 = 0x780;
    do {
        var_at_5 -= 1;
    } while (var_at_5 != 0);
    *(s16 *)0x02000042 = 0x4E;
    var_at_6 = 0x780;
    do {
        var_at_6 -= 1;
    } while (var_at_6 != 0);
    *(s16 *)0x02000042 = 0x25;
    *(s16 *)0x02000044 = 0xF04;
    var_at_7 = 0x780;
    do {
        var_at_7 -= 1;
    } while (var_at_7 != 0);
}

s32 func_8000ABA0(u16 arg0) {
    s32 sp0;
    s32 var_at;
    s32 var_at_2;
    s32 var_at_3;
    s32 var_s0;
    s32 var_v0;

    sp0 = M2C_ERROR(/* Read from unset register $at */);
    var_s0 = 0x3E8;
loop_1:
    var_at = 0xF0;
    if (*(u16 *)0x02000042 & 1) {
        do {
            var_at -= 1;
        } while (var_at != 0);
        var_v0 = 0;
        *(u16 *)0x02000040 = arg0;
    } else {
        var_at_2 = 0x3C;
        do {
            var_at_2 -= 1;
        } while (var_at_2 != 0);
        var_s0 -= 1;
        if (var_s0 == 0) {
            var_v0 = -1;
        } else {
            goto loop_1;
        }
    }
    var_at_3 = 0x3C;
    do {
        var_at_3 -= 1;
    } while (var_at_3 != 0);
    return var_v0;
}

s32 func_8000AC24(void) {
    s32 sp0;
    s32 var_at;
    s32 var_at_2;
    s32 var_v1;

    sp0 = M2C_ERROR(/* Read from unset register $t0 */);
    var_v1 = 0x3E8;
loop_1:
    var_at = 0x1E;
    if (*(u16 *)0x02000042 & 2) {
        do {
            var_at -= 1;
        } while (var_at != 0);
        return *(u16 *)0x02000040 & 0xFF;
    }
    var_at_2 = 0x1E;
    do {
        var_at_2 -= 1;
    } while (var_at_2 != 0);
    var_v1 -= 1;
    if (var_v1 == 0) {
        return -1;
    }
    goto loop_1;
}

s32 func_8000AC88(void) {
    return *(u16 *)0x02000040 & 0xFF;
}

u16 func_8000AC98(void) {
    return *(u16 *)0x02000042;
}

/*
Decompilation failure in function func_8000ACA4:

Cannot find branch target func_8000ACA4
*/

void *func_8000ACD0(void *arg1) {
    M2C_ERROR(/* mtc0 $a0, $0 */);
    M2C_ERROR(/* unknown instruction: tlbr */);
    M2C_FIELD(arg1, s32 *, 0) = (s32) M2C_ERROR(/* mfc0 $10 */);
    M2C_FIELD(arg1, s32 *, 4) = (s32) M2C_ERROR(/* mfc0 $2 */);
    M2C_FIELD(arg1, s32 *, 8) = (s32) M2C_ERROR(/* mfc0 $3 */);
    M2C_FIELD(arg1, s32 *, 0xC) = (s32) M2C_ERROR(/* mfc0 $5 */);
    M2C_ERROR(/* mtc0 $t7, $0 */);
    return arg1;
}

void *func_8000ADA4(void *arg1) {
    M2C_ERROR(/* mtc0 $a0, $0 */);
    M2C_ERROR(/* mtc0 $t0, $10 */);
    M2C_ERROR(/* mtc0 $t0, $2 */);
    M2C_ERROR(/* mtc0 $t0, $3 */);
    M2C_ERROR(/* mtc0 $t0, $5 */);
    M2C_ERROR(/* unknown instruction: tlbwi */);
    M2C_ERROR(/* mtc0 $t7, $0 */);
    return arg1;
}

void func_8000AE90(void) {
    func_8000A8D0(D_800AF22C);
    func_8000AF94();
}

/* Warning: Gap in callee-saved word stack region.
 * Saved: [0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x90, 0x94, 0x98, 0x9c, 0xa0, 0xa4, 0xa8, 0xac, 0xb0, 0xb4, 0xb8, 0xbc], gap at: 0x3c. */
void func_8000AEAC(s32 arg0, f32 arg1, s32 arg2, f32 arg3) {
    unksp40 = M2C_ERROR(/* Read from unset register $f0 */);
    unksp44 = M2C_ERROR(/* Read from unset register $f1 */);
    unksp48 = M2C_ERROR(/* Read from unset register $f2 */);
    unksp4C = M2C_ERROR(/* Read from unset register $f3 */);
    unksp50 = M2C_ERROR(/* Read from unset register $f4 */);
    unksp54 = M2C_ERROR(/* Read from unset register $f5 */);
    unksp58 = M2C_ERROR(/* Read from unset register $f6 */);
    unksp5C = M2C_ERROR(/* Read from unset register $f7 */);
    unksp60 = M2C_ERROR(/* Read from unset register $f8 */);
    unksp64 = M2C_ERROR(/* Read from unset register $f9 */);
    unksp68 = M2C_ERROR(/* Read from unset register $f10 */);
    unksp6C = M2C_ERROR(/* Read from unset register $f11 */);
    unksp70 = arg0;
    unksp74 = arg1;
    unksp78 = arg1;
    unksp7C = arg3;
    unksp80 = M2C_ERROR(/* Read from unset register $f16 */);
    unksp84 = M2C_ERROR(/* Read from unset register $f17 */);
    unksp88 = M2C_ERROR(/* Read from unset register $f18 */);
    unksp8C = M2C_ERROR(/* Read from unset register $f19 */);
    if (M2C_FIELD(D_800AF22C, s32 *, 0) == 0) {
        return;
    }
    M2C_FIELD(D_800AF22C, void **, 8) = (void *) sp;
}

/*
Decompilation failure in function func_8000AF94:

Unable to determine jump table for jr instruction at game.s line 12704.

There must be a read of a variable before the instruction
which has a name starting with with "jtbl"/"jpt_"/"lbl_"/"jumptable_".
*/

/* Warning: Gap in callee-saved word stack region.
 * Saved: [0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c, 0x30, 0x34, 0x38, 0x90, 0x94, 0x98, 0x9c, 0xa0, 0xa4, 0xa8, 0xac, 0xb0, 0xb4, 0xb8, 0xbc], gap at: 0x3c. */
void func_8000B06C(s32 arg0, f32 arg1, s32 arg2, f32 arg3) {
    unksp40 = M2C_ERROR(/* Read from unset register $f0 */);
    unksp44 = M2C_ERROR(/* Read from unset register $f1 */);
    unksp48 = M2C_ERROR(/* Read from unset register $f2 */);
    unksp4C = M2C_ERROR(/* Read from unset register $f3 */);
    unksp50 = M2C_ERROR(/* Read from unset register $f4 */);
    unksp54 = M2C_ERROR(/* Read from unset register $f5 */);
    unksp58 = M2C_ERROR(/* Read from unset register $f6 */);
    unksp5C = M2C_ERROR(/* Read from unset register $f7 */);
    unksp60 = M2C_ERROR(/* Read from unset register $f8 */);
    unksp64 = M2C_ERROR(/* Read from unset register $f9 */);
    unksp68 = M2C_ERROR(/* Read from unset register $f10 */);
    unksp6C = M2C_ERROR(/* Read from unset register $f11 */);
    unksp70 = arg0;
    unksp74 = arg1;
    unksp78 = arg1;
    unksp7C = arg3;
    unksp80 = M2C_ERROR(/* Read from unset register $f16 */);
    unksp84 = M2C_ERROR(/* Read from unset register $f17 */);
    unksp88 = M2C_ERROR(/* Read from unset register $f18 */);
    unksp8C = M2C_ERROR(/* Read from unset register $f19 */);
    M2C_FIELD(D_800C53FC, void **, 8) = (void *) sp;
    func_8000AF94();
}

void func_8000B150(M2C_UNK *arg0, f32 arg1, s32 arg2, f32 arg3) {
    M2C_FIELD(arg0, M2C_UNK **, 0) = arg0;
    M2C_FIELD(arg0, f32 *, 4) = arg1;
    M2C_FIELD(arg0, s32 *, 8) = arg2;
    M2C_FIELD(arg0, f32 *, 0xC) = arg3;
    M2C_FIELD(arg0, s32 *, 0x10) = (s32) saved_reg_s0;
    M2C_FIELD(arg0, s32 *, 0x14) = (s32) saved_reg_s1;
    M2C_FIELD(arg0, s32 *, 0x18) = (s32) saved_reg_s2;
    M2C_FIELD(arg0, s32 *, 0x1C) = (s32) saved_reg_s3;
    M2C_FIELD(arg0, s32 *, 0x20) = (s32) saved_reg_s4;
    M2C_FIELD(arg0, s32 *, 0x24) = (s32) saved_reg_s5;
    M2C_FIELD(arg0, s32 *, 0x28) = (s32) saved_reg_s6;
    M2C_FIELD(arg0, s32 *, 0x2C) = (s32) saved_reg_s7;
    M2C_FIELD(arg0, s32 *, 0x30) = (s32) saved_reg_gp;
    M2C_FIELD(arg0, s32 *, 0x34) = (s32) saved_reg_fp;
    M2C_FIELD(arg0, s32 *, 0x38) = (s32) saved_reg_ra;
    M2C_FIELD(arg0, f32 *, 0x40) = (f32) M2C_ERROR(/* Read from unset register $f0 */);
    M2C_FIELD(arg0, f32 *, 0x44) = (f32) M2C_ERROR(/* Read from unset register $f1 */);
    M2C_FIELD(arg0, f32 *, 0x48) = (f32) M2C_ERROR(/* Read from unset register $f2 */);
    M2C_FIELD(arg0, f32 *, 0x4C) = (f32) M2C_ERROR(/* Read from unset register $f3 */);
    M2C_FIELD(arg0, f32 *, 0x50) = (f32) M2C_ERROR(/* Read from unset register $f4 */);
    M2C_FIELD(arg0, f32 *, 0x54) = (f32) M2C_ERROR(/* Read from unset register $f5 */);
    M2C_FIELD(arg0, f32 *, 0x58) = (f32) M2C_ERROR(/* Read from unset register $f6 */);
    M2C_FIELD(arg0, f32 *, 0x5C) = (f32) M2C_ERROR(/* Read from unset register $f7 */);
    M2C_FIELD(arg0, f32 *, 0x60) = (f32) M2C_ERROR(/* Read from unset register $f8 */);
    M2C_FIELD(arg0, f32 *, 0x64) = (f32) M2C_ERROR(/* Read from unset register $f9 */);
    M2C_FIELD(arg0, f32 *, 0x68) = (f32) M2C_ERROR(/* Read from unset register $f10 */);
    M2C_FIELD(arg0, f32 *, 0x6C) = (f32) M2C_ERROR(/* Read from unset register $f11 */);
    M2C_FIELD(arg0, f32 *, 0x70) = arg0;
    M2C_FIELD(arg0, f32 *, 0x74) = arg1;
    M2C_FIELD(arg0, f32 *, 0x78) = arg1;
    M2C_FIELD(arg0, f32 *, 0x7C) = arg3;
    M2C_FIELD(arg0, f32 *, 0x80) = (f32) M2C_ERROR(/* Read from unset register $f16 */);
    M2C_FIELD(arg0, f32 *, 0x84) = (f32) M2C_ERROR(/* Read from unset register $f17 */);
    M2C_FIELD(arg0, f32 *, 0x88) = (f32) M2C_ERROR(/* Read from unset register $f18 */);
    M2C_FIELD(arg0, f32 *, 0x8C) = (f32) M2C_ERROR(/* Read from unset register $f19 */);
    M2C_FIELD(arg0, f32 *, 0x90) = (f32) saved_reg_f20;
    M2C_FIELD(arg0, f32 *, 0x94) = (f32) saved_reg_f21;
    M2C_FIELD(arg0, f32 *, 0x98) = (f32) saved_reg_f22;
    M2C_FIELD(arg0, f32 *, 0x9C) = (f32) saved_reg_f23;
    M2C_FIELD(arg0, f32 *, 0xA0) = (f32) saved_reg_f24;
    M2C_FIELD(arg0, f32 *, 0xA4) = (f32) saved_reg_f25;
    M2C_FIELD(arg0, f32 *, 0xA8) = (f32) saved_reg_f26;
    M2C_FIELD(arg0, f32 *, 0xAC) = (f32) saved_reg_f27;
    M2C_FIELD(arg0, f32 *, 0xB0) = (f32) saved_reg_f28;
    M2C_FIELD(arg0, f32 *, 0xB4) = (f32) saved_reg_f29;
    M2C_FIELD(arg0, f32 *, 0xB8) = (f32) saved_reg_f30;
    M2C_FIELD(arg0, f32 *, 0xBC) = (f32) saved_reg_f31;
}

void Main_Boot(s16 **arg0) {
    func_8001CAC0();
    func_8000A5A0();
    func_8000A628(1, 0x01000000, 0xC0000000, 5, 0);
    func_8000A628(2, 0x02000000, 0xC0800000, 0, 0);
    func_8000A628(3, 0x02010000, 0xC0C00000, 0, 0);
    func_800113E0(NULL);
    osCreateThread(D_8003E648, 1, func_8000B2D0, arg0, D_80041800, 0xA);
    osStartThread(D_8003E648);
}

void func_8000B2D0(s16 **arg0) {
    osCreatePiManager(0x96, (s16 *(*)[]) D_80041900, D_80041800, (s16 *)0x40);
    osCreateMesgQueue((s16 *(*)[]) D_80041938, &D_80041950, (s16 *)1);
    osCreateThread(D_80041958, 5, func_8000C510, NULL, &D_80043310, 0xFA);
    osStartThread(D_80041958);
    osCreateThread(D_8003E498, 6, func_8000BD28, arg0, D_80041000, 0xB);
    if (D_800B04C8 == 0) {
        osStartThread(D_8003E498);
    }
    osSetThreadPri(NULL, 0);
loop_3:
    goto loop_3;
}

void func_8000B3C8(void) {
    s16 *sp10;
    s16 temp_a0_12;
    s16 temp_s0;
    s16 temp_v1_36;
    s16 var_v0_41;
    s16 var_v0_59;
    s32 temp_a0;
    s32 temp_a0_10;
    s32 temp_a0_11;
    s32 temp_a0_2;
    s32 temp_a0_3;
    s32 temp_a0_4;
    s32 temp_a0_5;
    s32 temp_a0_7;
    s32 temp_a0_8;
    s32 temp_a0_9;
    s32 temp_v1;
    s32 temp_v1_10;
    s32 temp_v1_11;
    s32 temp_v1_13;
    s32 temp_v1_14;
    s32 temp_v1_15;
    s32 temp_v1_16;
    s32 temp_v1_17;
    s32 temp_v1_19;
    s32 temp_v1_20;
    s32 temp_v1_21;
    s32 temp_v1_22;
    s32 temp_v1_23;
    s32 temp_v1_25;
    s32 temp_v1_26;
    s32 temp_v1_27;
    s32 temp_v1_28;
    s32 temp_v1_29;
    s32 temp_v1_2;
    s32 temp_v1_31;
    s32 temp_v1_32;
    s32 temp_v1_33;
    s32 temp_v1_34;
    s32 temp_v1_35;
    s32 temp_v1_37;
    s32 temp_v1_38;
    s32 temp_v1_39;
    s32 temp_v1_3;
    s32 temp_v1_40;
    s32 temp_v1_41;
    s32 temp_v1_43;
    s32 temp_v1_44;
    s32 temp_v1_45;
    s32 temp_v1_46;
    s32 temp_v1_47;
    s32 temp_v1_49;
    s32 temp_v1_4;
    s32 temp_v1_50;
    s32 temp_v1_51;
    s32 temp_v1_52;
    s32 temp_v1_53;
    s32 temp_v1_55;
    s32 temp_v1_56;
    s32 temp_v1_57;
    s32 temp_v1_58;
    s32 temp_v1_59;
    s32 temp_v1_5;
    s32 temp_v1_7;
    s32 temp_v1_8;
    s32 temp_v1_9;
    s32 var_v0;
    s32 var_v0_10;
    s32 var_v0_11;
    s32 var_v0_13;
    s32 var_v0_14;
    s32 var_v0_15;
    s32 var_v0_16;
    s32 var_v0_17;
    s32 var_v0_19;
    s32 var_v0_20;
    s32 var_v0_21;
    s32 var_v0_22;
    s32 var_v0_23;
    s32 var_v0_25;
    s32 var_v0_26;
    s32 var_v0_27;
    s32 var_v0_28;
    s32 var_v0_29;
    s32 var_v0_2;
    s32 var_v0_31;
    s32 var_v0_32;
    s32 var_v0_33;
    s32 var_v0_34;
    s32 var_v0_35;
    s32 var_v0_36;
    s32 var_v0_37;
    s32 var_v0_38;
    s32 var_v0_39;
    s32 var_v0_3;
    s32 var_v0_40;
    s32 var_v0_42;
    s32 var_v0_43;
    s32 var_v0_44;
    s32 var_v0_45;
    s32 var_v0_46;
    s32 var_v0_48;
    s32 var_v0_49;
    s32 var_v0_4;
    s32 var_v0_50;
    s32 var_v0_51;
    s32 var_v0_52;
    s32 var_v0_54;
    s32 var_v0_55;
    s32 var_v0_56;
    s32 var_v0_57;
    s32 var_v0_58;
    s32 var_v0_5;
    s32 var_v0_60;
    s32 var_v0_61;
    s32 var_v0_62;
    s32 var_v0_63;
    s32 var_v0_64;
    s32 var_v0_66;
    s32 var_v0_67;
    s32 var_v0_68;
    s32 var_v0_69;
    s32 var_v0_70;
    s32 var_v0_7;
    s32 var_v0_8;
    s32 var_v0_9;
    u16 temp_a0_6;
    u16 temp_v1_12;
    u16 temp_v1_18;
    u16 temp_v1_24;
    u16 temp_v1_30;
    u16 temp_v1_42;
    u16 temp_v1_48;
    u16 temp_v1_54;
    u16 temp_v1_60;
    u16 temp_v1_6;
    u16 var_v0_12;
    u16 var_v0_18;
    u16 var_v0_24;
    u16 var_v0_30;
    u16 var_v0_47;
    u16 var_v0_53;
    u16 var_v0_65;
    u16 var_v0_6;
    u16 var_v0_71;

    sp10 = NULL;
loop_1:
    osRecvMesg((s16 *(*)[]) D_800B04E0, &sp10, 1);
    temp_s0 = *sp10;
    if (temp_s0 == 1) {
        if ((func_8000F750() != 0) && (D_80043310 != NULL)) {
            func_8000F6EC();
            D_80043310 = NULL;
            if (D_800B04D0 != 0) {
                func_8000F3E0();
            }
            if (D_800B0430 != 0) {
                D_800B04D0 = 0;
                D_800B4AA4 = (s32) temp_s0;
                func_80021FC0((s16 *(*)[]) D_800C30A8);
            }
        }
        func_8000FA4C();
        func_8000FA34();
        func_8000F8F4();
        switch (D_800B0720) {                       /* irregular */
        case 0:
            M2C_FIELD(&D_800B060C, u16 *, 0) = (u16) D_800B0498;
            M2C_FIELD(&D_800B060C, u16 *, 2) = (u16) D_800B049A;
            D_800C0D48 = D_800C532C;
            D_800C0D4A = D_800C532E;
            D_800AF258 = D_800B4A80;
            D_800AF25A = D_800B4A82;
            break;
        case 1:
            M2C_FIELD(&D_800B060C, u16 *, 0) = 0U;
            M2C_FIELD(&D_800B060C, u16 *, 2) = 0U;
            D_800C0D48 = 0;
            D_800C0D4A = 0;
            D_800AF258 = 0;
            D_800AF25A = 0;
            break;
        case 2:
            if (((D_800B058C & 0xC00) == 0xC00) || ((D_800B058C & 0x300) == 0x300)) {
                temp_v1 = ((u16) D_800C5334 >> 0xB) & 1;
                var_v0 = temp_v1 | 2;
                if (!(D_800C5334 & 0x400)) {
                    var_v0 = temp_v1;
                }
                temp_v1_2 = var_v0 & 0xFFFF;
                var_v0_2 = temp_v1_2 | 4;
                if (!(D_800C5334 & 0x200)) {
                    var_v0_2 = temp_v1_2;
                }
                temp_v1_3 = var_v0_2 & 0xFFFF;
                var_v0_3 = temp_v1_3 | 8;
                if (!(D_800C5334 & 0x100)) {
                    var_v0_3 = temp_v1_3;
                }
                temp_v1_4 = var_v0_3 & 0xFFFF;
                var_v0_4 = temp_v1_4 | 0x10;
                if (!(D_800C5334 & 0x8000)) {
                    var_v0_4 = temp_v1_4;
                }
                temp_v1_5 = var_v0_4 & 0xFFFF;
                var_v0_5 = temp_v1_5 | 0x20;
                if (!(D_800C5334 & 0x4000)) {
                    var_v0_5 = temp_v1_5;
                }
                temp_v1_6 = var_v0_5 & 0xFFFF;
                var_v0_6 = temp_v1_6 | 0x100;
                if (!(D_800C5334 & 0x1000)) {
                    var_v0_6 = temp_v1_6;
                }
                M2C_FIELD(&D_800B060C, u16 *, 0) = var_v0_6;
                temp_v1_7 = ((u16) D_800B0596 >> 0xB) & 1;
                var_v0_7 = temp_v1_7 | 2;
                if (!(D_800B0596 & 0x400)) {
                    var_v0_7 = temp_v1_7;
                }
                temp_v1_8 = var_v0_7 & 0xFFFF;
                var_v0_8 = temp_v1_8 | 4;
                if (!(D_800B0596 & 0x200)) {
                    var_v0_8 = temp_v1_8;
                }
                temp_v1_9 = var_v0_8 & 0xFFFF;
                var_v0_9 = temp_v1_9 | 8;
                if (!(D_800B0596 & 0x100)) {
                    var_v0_9 = temp_v1_9;
                }
                temp_v1_10 = var_v0_9 & 0xFFFF;
                var_v0_10 = temp_v1_10 | 0x10;
                if (!(D_800B0596 & 0x8000)) {
                    var_v0_10 = temp_v1_10;
                }
                temp_v1_11 = var_v0_10 & 0xFFFF;
                var_v0_11 = temp_v1_11 | 0x20;
                if (!(D_800B0596 & 0x4000)) {
                    var_v0_11 = temp_v1_11;
                }
                temp_v1_12 = var_v0_11 & 0xFFFF;
                var_v0_12 = temp_v1_12 | 0x100;
                if (!(D_800B0596 & 0x1000)) {
                    var_v0_12 = temp_v1_12;
                }
                D_800C0D48 = var_v0_12;
                temp_v1_13 = ((u16) D_800B043A >> 0xB) & 1;
                var_v0_13 = temp_v1_13 | 2;
                if (!(D_800B043A & 0x400)) {
                    var_v0_13 = temp_v1_13;
                }
                temp_v1_14 = var_v0_13 & 0xFFFF;
                var_v0_14 = temp_v1_14 | 4;
                if (!(D_800B043A & 0x200)) {
                    var_v0_14 = temp_v1_14;
                }
                temp_v1_15 = var_v0_14 & 0xFFFF;
                var_v0_15 = temp_v1_15 | 8;
                if (!(D_800B043A & 0x100)) {
                    var_v0_15 = temp_v1_15;
                }
                temp_v1_16 = var_v0_15 & 0xFFFF;
                var_v0_16 = temp_v1_16 | 0x10;
                if (!(D_800B043A & 0x8000)) {
                    var_v0_16 = temp_v1_16;
                }
                temp_v1_17 = var_v0_16 & 0xFFFF;
                var_v0_17 = temp_v1_17 | 0x20;
                if (!(D_800B043A & 0x4000)) {
                    var_v0_17 = temp_v1_17;
                }
                temp_v1_18 = var_v0_17 & 0xFFFF;
                var_v0_18 = temp_v1_18 | 0x100;
                if (!(D_800B043A & 0x1000)) {
                    var_v0_18 = temp_v1_18;
                }
            } else {
                temp_a0 = ((u16) D_800B058C >> 0xB) & 1;
                var_v0_19 = temp_a0 | 2;
                if (!(D_800B058C & 0x400)) {
                    var_v0_19 = temp_a0;
                }
                temp_a0_2 = var_v0_19 & 0xFFFF;
                var_v0_20 = temp_a0_2 | 4;
                if (!(D_800B058C & 0x200)) {
                    var_v0_20 = temp_a0_2;
                }
                temp_a0_3 = var_v0_20 & 0xFFFF;
                var_v0_21 = temp_a0_3 | 8;
                if (!(D_800B058C & 0x100)) {
                    var_v0_21 = temp_a0_3;
                }
                temp_a0_4 = var_v0_21 & 0xFFFF;
                var_v0_22 = temp_a0_4 | 0x10;
                if (!(D_800B058C & 0x8000)) {
                    var_v0_22 = temp_a0_4;
                }
                temp_a0_5 = var_v0_22 & 0xFFFF;
                var_v0_23 = temp_a0_5 | 0x20;
                if (!(D_800B058C & 0x4000)) {
                    var_v0_23 = temp_a0_5;
                }
                temp_a0_6 = var_v0_23 & 0xFFFF;
                var_v0_24 = temp_a0_6 | 0x100;
                if (!(D_800B058C & 0x1000)) {
                    var_v0_24 = temp_a0_6;
                }
                M2C_FIELD(&D_800B060C, u16 *, 0) = var_v0_24;
                temp_v1_19 = ((u16) D_800AF222 >> 0xB) & 1;
                var_v0_25 = temp_v1_19 | 2;
                if (!(D_800AF222 & 0x400)) {
                    var_v0_25 = temp_v1_19;
                }
                temp_v1_20 = var_v0_25 & 0xFFFF;
                var_v0_26 = temp_v1_20 | 4;
                if (!(D_800AF222 & 0x200)) {
                    var_v0_26 = temp_v1_20;
                }
                temp_v1_21 = var_v0_26 & 0xFFFF;
                var_v0_27 = temp_v1_21 | 8;
                if (!(D_800AF222 & 0x100)) {
                    var_v0_27 = temp_v1_21;
                }
                temp_v1_22 = var_v0_27 & 0xFFFF;
                var_v0_28 = temp_v1_22 | 0x10;
                if (!(D_800AF222 & 0x8000)) {
                    var_v0_28 = temp_v1_22;
                }
                temp_v1_23 = var_v0_28 & 0xFFFF;
                var_v0_29 = temp_v1_23 | 0x20;
                if (!(D_800AF222 & 0x4000)) {
                    var_v0_29 = temp_v1_23;
                }
                temp_v1_24 = var_v0_29 & 0xFFFF;
                var_v0_30 = temp_v1_24 | 0x100;
                if (!(D_800AF222 & 0x1000)) {
                    var_v0_30 = temp_v1_24;
                }
                D_800C0D48 = var_v0_30;
                temp_v1_25 = ((u16) D_800AF238 >> 0xB) & 1;
                var_v0_31 = temp_v1_25 | 2;
                if (!(D_800AF238 & 0x400)) {
                    var_v0_31 = temp_v1_25;
                }
                temp_v1_26 = var_v0_31 & 0xFFFF;
                var_v0_32 = temp_v1_26 | 4;
                if (!(D_800AF238 & 0x200)) {
                    var_v0_32 = temp_v1_26;
                }
                temp_v1_27 = var_v0_32 & 0xFFFF;
                var_v0_33 = temp_v1_27 | 8;
                if (!(D_800AF238 & 0x100)) {
                    var_v0_33 = temp_v1_27;
                }
                temp_v1_28 = var_v0_33 & 0xFFFF;
                var_v0_34 = temp_v1_28 | 0x10;
                if (!(D_800AF238 & 0x8000)) {
                    var_v0_34 = temp_v1_28;
                }
                temp_v1_29 = var_v0_34 & 0xFFFF;
                var_v0_35 = temp_v1_29 | 0x20;
                if (!(D_800AF238 & 0x4000)) {
                    var_v0_35 = temp_v1_29;
                }
                temp_v1_30 = var_v0_35 & 0xFFFF;
                var_v0_18 = temp_v1_30 | 0x100;
                if (!(D_800AF238 & 0x1000)) {
                    var_v0_18 = temp_v1_30;
                }
            }
            D_800AF258 = var_v0_18;
            if (((D_800B058E & 0xC00) == 0xC00) || ((D_800B058E & 0x300) == 0x300)) {
                temp_v1_31 = ((u16) D_800C5336 >> 0xB) & 1;
                var_v0_36 = temp_v1_31 | 2;
                if (!(D_800C5336 & 0x400)) {
                    var_v0_36 = temp_v1_31;
                }
                temp_v1_32 = var_v0_36 & 0xFFFF;
                var_v0_37 = temp_v1_32 | 4;
                if (!(D_800C5336 & 0x200)) {
                    var_v0_37 = temp_v1_32;
                }
                temp_v1_33 = var_v0_37 & 0xFFFF;
                var_v0_38 = temp_v1_33 | 8;
                if (!(D_800C5336 & 0x100)) {
                    var_v0_38 = temp_v1_33;
                }
                temp_v1_34 = var_v0_38 & 0xFFFF;
                var_v0_39 = temp_v1_34 | 0x10;
                if (!(D_800C5336 & 0x8000)) {
                    var_v0_39 = temp_v1_34;
                }
                temp_v1_35 = var_v0_39 & 0xFFFF;
                var_v0_40 = temp_v1_35 | 0x20;
                if (!(D_800C5336 & 0x4000)) {
                    var_v0_40 = temp_v1_35;
                }
                temp_v1_36 = var_v0_40 & 0xFFFF;
                var_v0_41 = temp_v1_36 | 0x100;
                if (!(D_800C5336 & 0x1000)) {
                    var_v0_41 = temp_v1_36;
                }
                D_800B060E = var_v0_41;
                temp_v1_37 = ((u16) D_800B0598 >> 0xB) & 1;
                var_v0_42 = temp_v1_37 | 2;
                if (!(D_800B0598 & 0x400)) {
                    var_v0_42 = temp_v1_37;
                }
                temp_v1_38 = var_v0_42 & 0xFFFF;
                var_v0_43 = temp_v1_38 | 4;
                if (!(D_800B0598 & 0x200)) {
                    var_v0_43 = temp_v1_38;
                }
                temp_v1_39 = var_v0_43 & 0xFFFF;
                var_v0_44 = temp_v1_39 | 8;
                if (!(D_800B0598 & 0x100)) {
                    var_v0_44 = temp_v1_39;
                }
                temp_v1_40 = var_v0_44 & 0xFFFF;
                var_v0_45 = temp_v1_40 | 0x10;
                if (!(D_800B0598 & 0x8000)) {
                    var_v0_45 = temp_v1_40;
                }
                temp_v1_41 = var_v0_45 & 0xFFFF;
                var_v0_46 = temp_v1_41 | 0x20;
                if (!(D_800B0598 & 0x4000)) {
                    var_v0_46 = temp_v1_41;
                }
                temp_v1_42 = var_v0_46 & 0xFFFF;
                var_v0_47 = temp_v1_42 | 0x100;
                if (!(D_800B0598 & 0x1000)) {
                    var_v0_47 = temp_v1_42;
                }
                D_800C0D4A = var_v0_47;
                temp_v1_43 = ((u16) D_800B043C >> 0xB) & 1;
                var_v0_48 = temp_v1_43 | 2;
                if (!(D_800B043C & 0x400)) {
                    var_v0_48 = temp_v1_43;
                }
                temp_v1_44 = var_v0_48 & 0xFFFF;
                var_v0_49 = temp_v1_44 | 4;
                if (!(D_800B043C & 0x200)) {
                    var_v0_49 = temp_v1_44;
                }
                temp_v1_45 = var_v0_49 & 0xFFFF;
                var_v0_50 = temp_v1_45 | 8;
                if (!(D_800B043C & 0x100)) {
                    var_v0_50 = temp_v1_45;
                }
                temp_v1_46 = var_v0_50 & 0xFFFF;
                var_v0_51 = temp_v1_46 | 0x10;
                if (!(D_800B043C & 0x8000)) {
                    var_v0_51 = temp_v1_46;
                }
                temp_v1_47 = var_v0_51 & 0xFFFF;
                var_v0_52 = temp_v1_47 | 0x20;
                if (!(D_800B043C & 0x4000)) {
                    var_v0_52 = temp_v1_47;
                }
                temp_v1_48 = var_v0_52 & 0xFFFF;
                var_v0_53 = temp_v1_48 | 0x100;
                if (!(D_800B043C & 0x1000)) {
                    var_v0_53 = temp_v1_48;
                }
            } else {
                temp_a0_7 = ((u16) D_800B058E >> 0xB) & 1;
                var_v0_54 = temp_a0_7 | 2;
                if (!(D_800B058E & 0x400)) {
                    var_v0_54 = temp_a0_7;
                }
                temp_a0_8 = var_v0_54 & 0xFFFF;
                var_v0_55 = temp_a0_8 | 4;
                if (!(D_800B058E & 0x200)) {
                    var_v0_55 = temp_a0_8;
                }
                temp_a0_9 = var_v0_55 & 0xFFFF;
                var_v0_56 = temp_a0_9 | 8;
                if (!(D_800B058E & 0x100)) {
                    var_v0_56 = temp_a0_9;
                }
                temp_a0_10 = var_v0_56 & 0xFFFF;
                var_v0_57 = temp_a0_10 | 0x10;
                if (!(D_800B058E & 0x8000)) {
                    var_v0_57 = temp_a0_10;
                }
                temp_a0_11 = var_v0_57 & 0xFFFF;
                var_v0_58 = temp_a0_11 | 0x20;
                if (!(D_800B058E & 0x4000)) {
                    var_v0_58 = temp_a0_11;
                }
                temp_a0_12 = var_v0_58 & 0xFFFF;
                var_v0_59 = temp_a0_12 | 0x100;
                if (!(D_800B058E & 0x1000)) {
                    var_v0_59 = temp_a0_12;
                }
                D_800B060E = var_v0_59;
                temp_v1_49 = ((u16) D_800AF224 >> 0xB) & 1;
                var_v0_60 = temp_v1_49 | 2;
                if (!(D_800AF224 & 0x400)) {
                    var_v0_60 = temp_v1_49;
                }
                temp_v1_50 = var_v0_60 & 0xFFFF;
                var_v0_61 = temp_v1_50 | 4;
                if (!(D_800AF224 & 0x200)) {
                    var_v0_61 = temp_v1_50;
                }
                temp_v1_51 = var_v0_61 & 0xFFFF;
                var_v0_62 = temp_v1_51 | 8;
                if (!(D_800AF224 & 0x100)) {
                    var_v0_62 = temp_v1_51;
                }
                temp_v1_52 = var_v0_62 & 0xFFFF;
                var_v0_63 = temp_v1_52 | 0x10;
                if (!(D_800AF224 & 0x8000)) {
                    var_v0_63 = temp_v1_52;
                }
                temp_v1_53 = var_v0_63 & 0xFFFF;
                var_v0_64 = temp_v1_53 | 0x20;
                if (!(D_800AF224 & 0x4000)) {
                    var_v0_64 = temp_v1_53;
                }
                temp_v1_54 = var_v0_64 & 0xFFFF;
                var_v0_65 = temp_v1_54 | 0x100;
                if (!(D_800AF224 & 0x1000)) {
                    var_v0_65 = temp_v1_54;
                }
                D_800C0D4A = var_v0_65;
                temp_v1_55 = ((u16) D_800AF23A >> 0xB) & 1;
                var_v0_66 = temp_v1_55 | 2;
                if (!(D_800AF23A & 0x400)) {
                    var_v0_66 = temp_v1_55;
                }
                temp_v1_56 = var_v0_66 & 0xFFFF;
                var_v0_67 = temp_v1_56 | 4;
                if (!(D_800AF23A & 0x200)) {
                    var_v0_67 = temp_v1_56;
                }
                temp_v1_57 = var_v0_67 & 0xFFFF;
                var_v0_68 = temp_v1_57 | 8;
                if (!(D_800AF23A & 0x100)) {
                    var_v0_68 = temp_v1_57;
                }
                temp_v1_58 = var_v0_68 & 0xFFFF;
                var_v0_69 = temp_v1_58 | 0x10;
                if (!(D_800AF23A & 0x8000)) {
                    var_v0_69 = temp_v1_58;
                }
                temp_v1_59 = var_v0_69 & 0xFFFF;
                var_v0_70 = temp_v1_59 | 0x20;
                if (!(D_800AF23A & 0x4000)) {
                    var_v0_70 = temp_v1_59;
                }
                temp_v1_60 = var_v0_70 & 0xFFFF;
                var_v0_53 = temp_v1_60 | 0x100;
                if (!(D_800AF23A & 0x1000)) {
                    var_v0_53 = temp_v1_60;
                }
            }
            D_800AF25A = var_v0_53;
            break;
        }
        if (D_800C1674 == 0) {
            if ((D_800C53D8 != 0) || (D_800C53DA != 0) || (*D_800C1898 != 0)) {
                func_8001BB18(0);
                if (*D_800C1898 != 0) {
                    D_800C30C8 += 1;
                }
            }
            if ((u32) D_800C30C8 < 0x63U) {
                var_v0_71 = 0xFFFF;
            } else {
                D_800C30C8 = 0x63;
                var_v0_71 = 0xFFF3;
            }
            D_800C1678 = var_v0_71;
        }
        if ((D_800B0574 != 0) || (D_800B0576 != 0)) {
            D_800AF260 = 1;
        }
        D_80043314 += 1;
        goto loop_1;
    }
    if (temp_s0 != 4) {
        goto loop_1;
    }
    D_800C53F8 += 2;
    goto loop_1;
}

void func_8000BD28(s16 **arg0) {
    s16 *sp18;
    s16 temp_a0;
    u32 temp_v0;

    D_80043314 = 2;
    sp18 = NULL;
    D_800C51CC = 1;
    D_800C0AB8 = 0;
    func_8000C2A8();
    func_80018DBC();
    D_80043310 = NULL;
    osCreateThread(D_8003E138, 7, func_8000C424, arg0, D_8003F000, 0xC);
    osStartThread(D_8003E138);
    osCreateThread(D_8003E2E8, 8, func_8000B3C8, arg0, D_8003F800, 0xD);
    osStartThread(D_8003E2E8);
loop_1:
    osRecvMesg((s16 *(*)[]) D_800C30E0, &sp18, 1);
    temp_a0 = *sp18;
    if (temp_a0 == 1) {
        if (((u32) D_800C53F8 < 2U) && ((u32) D_80043314 >= (u32) D_800C51CC)) {
            D_80043310 = (s16 *) temp_a0;
            D_80043314 = 0;
            func_8000FF64((D_800C0AB8 * 0x2180) + &D_800B0730);
            func_8001BE98(0xC1);
            temp_v0 = D_800C53F8;
            if (temp_v0 == 0) {
                func_8001BE98(0x82);
                func_80009E6C(&D_800B4AB0);
                func_80017878(&gGameHeap, &D_800B0578);
                func_80011C20(D_800C316C);
                func_80011C20(D_800C30C0);
                func_80011C20(D_800C0AB4);
                func_80011C20(D_800AF234);
                func_8001BE98(0xC2);
                func_8001BEA0();
            }
            D_800C53F8 = temp_v0 + 1;
            D_800C0AB8 ^= 1;
        }
        goto loop_1;
    }
    if (temp_a0 != 2) {
        goto loop_1;
    }
    if ((u32) D_800C53F8 >= 2U) {
        func_8001BE98(0x82);
        func_80009E6C(&D_800B4AB0);
        func_80017878(&gGameHeap, &D_800B0578);
        func_80011C20(D_800C316C);
        func_80011C20(D_800C30C0);
        func_80011C20(D_800C0AB4);
        func_80011C20(D_800AF234);
        func_8001BE98(0xC2);
        func_8001BEA0();
    }
    if (D_800C53F8 == 0) {
        goto loop_1;
    }
    D_800C53F8 -= 1;
    goto loop_1;
}

void Rom_Read(u8 *arg0, u8 *arg1, s32 arg2) {
    struct _struct_D_800A5200_0x18 sp20;
    s16 *sp38;
    s32 var_s0;
    u8 *var_s1;
    u8 *var_s2;

    var_s2 = arg0;
    var_s1 = arg1;
    var_s0 = arg2;
    osInvalDCache(var_s1, var_s0);
    if (var_s0 >= 0x400) {
        do {
            func_80025950(&sp20, 0, 0, var_s2, var_s1, 0x400, (s16 *(*)[]) D_80041918);
            osRecvMesg((s16 *(*)[]) D_80041918, &sp38, 1);
            var_s2 += 0x400;
            var_s0 -= 0x400;
            var_s1 += 0x400;
        } while (var_s0 >= 0x400);
    }
    if (var_s0 != 0) {
        func_80025950(&sp20, 0, 0, var_s2, var_s1, var_s0, (s16 *(*)[]) D_80041918);
        osRecvMesg((s16 *(*)[]) D_80041918, &sp38, 1);
    }
}

void Overlay_Load(s32 arg0) {
    struct _struct_D_800A5200_0x18 sp20;
    s16 *sp38;
    s32 *var_a1;
    s32 temp_a0;
    s32 var_s0;
    s32 var_v1;
    u8 *var_s1;
    u8 *var_s2;

    temp_a0 = arg0 * 0x10;
    var_s1 = *(&gOverlayTable->unk8 + temp_a0);
    var_s2 = gOverlayTable[arg0].unk0;
    var_a1 = &func_800E0000;
    var_v1 = 0x13400;
    var_s0 = *(&gOverlayTable->unkC + temp_a0) - var_s1;
    do {
        *var_a1 = 0;
        var_v1 -= 1;
    } while (var_v1 != 0);
    func_8001C990(temp_a0, var_a1);
    osInvalDCache((u8 *)0x800E0000, 0x4D000);
    osInvalICache(0x800E0000U, 0x4D000);
    if (var_s0 >= 0x400) {
        do {
            func_80025950(&sp20, 0, 0, var_s1, var_s2, 0x400, (s16 *(*)[]) D_80041918);
            osRecvMesg((s16 *(*)[]) D_80041918, &sp38, 1);
            var_s1 += 0x400;
            var_s0 -= 0x400;
            var_s2 += 0x400;
        } while (var_s0 >= 0x400);
    }
    if (var_s0 != 0) {
        func_80025950(&sp20, 0, 0, var_s1, var_s2, var_s0, (s16 *(*)[]) D_80041918);
        osRecvMesg((s16 *(*)[]) D_80041918, &sp38, 1);
    }
}

void func_8000C218(void) {
    func_80025DC0(0, NULL);
    __osContRamRead(NULL, 0, 0U, NULL);
    func_8001D350(NULL, 0U);
    func_80021EA0(0, 0);
    func_80024750(NULL, NULL, NULL);
    func_80023F90(NULL, 0U, NULL);
    func_800221A0(NULL, NULL);
    func_800236C0(NULL, 0, 0, NULL, NULL);
}

void func_8000C2A8(void) {
    osCreateMesgQueue((s16 *(*)[]) D_80041918, &D_80041930, (s16 *)1);
    osCreateMesgQueue((s16 *(*)[]) D_800B04E0, D_800C1798, (s16 *)0x40);
    osCreateMesgQueue((s16 *(*)[]) D_800C30E0, D_800B0620, (s16 *)0x40);
    func_80011E60();
    func_80012090(&D_800C1730);
    Heap_Init(&gGameHeap, 0x52B000);
    func_800177B0(&D_800B0578, &D_800B0558);
    func_80011550();
    func_8000C590(D_800C0AC0, &D_800C51C0, 0xF, 2, 1U);
    D_800B0568 = func_8000C7C0(D_800C0AC0);
    func_8000F890();
    D_800C16E0 = 1;
    func_8000F350();
    func_80014E90();
    func_800154D0();
    func_8001BE80();
    func_8000FDB0();
    Audio_Init();
    func_8000C6D8(D_800C0AC0, &D_800B054C, (s16 *(*)[]) D_800C30E0);
    func_8000C6D8(D_800C0AC0, &D_800C165C, (s16 *(*)[]) D_800B04E0);
    func_8000A6D0(&D_8069E600, 0x114000, 0x4000);
    func_8001BF08();
}

void func_8000C424(void) {
    s16 *sp10;

    sp10 = NULL;
loop_1:
loop_2:
    D_800C51D0 = 1;
    osRecvMesg((s16 *(*)[]) D_800C30A8, &sp10, 1);
    D_800C51D0 = 0;
    if (*sp10 != 5) {
        goto loop_1;
    }
    D_800B4AA4 = 0;
    if (func_8000F750((s16 *(*)[]) D_800C30A8) != 0) {
        D_800B04D0 = 1;
        if (D_800AF254 != 0) {
            __osContRamRead((s16 *(*)[]) D_800C30A8, 0, 0U, D_800C16C0);
        }
        goto loop_2;
    }
    D_800C51D0 = 2;
    osRecvMesg((s16 *(*)[]) D_800B0520, &sp10, 1);
    D_800C51D0 = 0;
    goto loop_1;
}

void func_8000C510(void) {
    s16 *sp10;
    s32 temp_v0;

    sp10 = NULL;
    osSetEventMesg(0xC, (s16 *(*)[]) D_80041938, (s16 *)0x10);
    D_8003E134 = 0;
    osRecvMesg((s16 *(*)[]) D_80041938, &sp10, 1);
    temp_v0 = func_8001D340();
    D_8003E130 = temp_v0;
    if (temp_v0 != 0) {
        func_8000D150(temp_v0);
    }
loop_2:
    goto loop_2;
}

void func_8000C590(s16 **arg0, s16 **arg1, s32 arg2, s32 arg3, u8 arg4) {
    s16 *(*temp_s2)[];
    s16 **temp_s1;

    temp_s2 = arg0 + 0x40;
    M2C_FIELD(arg0, s16 *, 0) = 1;
    arg0[0x9D] = NULL;
    arg0[0x9E] = NULL;
    arg0[0x98] = NULL;
    arg0[0x9F] = NULL;
    arg0[0x99] = NULL;
    arg0[0x9A] = NULL;
    arg0[0x9B] = NULL;
    arg0[0x9C] = NULL;
    M2C_FIELD(arg0, s16 *, 0x20) = 4;
    osCreateMesgQueue(temp_s2, arg0 + 0x58, (s16 *)8);
    osCreateMesgQueue(arg0 + 0x78, arg0 + 0x90, (s16 *)8);
    osCreateViManager(0xFE);
    osViSetMode(&osViModeTable[arg3 & 0xFF]);
    osViBlack(1);
    osSetEventMesg(4, temp_s2, (s16 *)0x29B);
    osSetEventMesg(9, temp_s2, (s16 *)0x29C);
    osSetEventMesg(0xE, temp_s2, (s16 *)0x29D);
    osViSetEvent(temp_s2, 0x29A, arg4);
    temp_s1 = arg0 + 0xB0;
    osCreateThread(temp_s1, 4, func_8000C7C8, arg0, arg1, arg2);
    osStartThread(temp_s1);
}

void func_8000C6D8(s16 **arg0, M2C_UNK *arg1, s16 *(*arg2)[]) {
    s32 temp_v0;

    temp_v0 = osSetIntMask(1);
    M2C_FIELD(arg1, s16 *(**)[], 4) = arg2;
    M2C_FIELD(arg1, M2C_UNK **, 0) = (M2C_UNK *) arg0[0x98];
    arg0[0x98] = arg1;
    osSetIntMask(temp_v0);
}

void func_8000C730(void *arg0, s32 arg1) {
    M2C_UNK *var_s0;
    M2C_UNK *var_s1;
    s32 temp_a0;

    var_s0 = M2C_FIELD(arg0, M2C_UNK **, 0x260);
    var_s1 = NULL;
    temp_a0 = osSetIntMask(1);
    if (var_s0 != NULL) {
loop_1:
        if (var_s0 != arg1) {
            var_s1 = var_s0;
            var_s0 = *var_s0;
            if (var_s0 != NULL) {
                goto loop_1;
            }
        } else if (var_s1 != NULL) {
            *var_s1 = *var_s0;
        } else {
            M2C_FIELD(arg0, M2C_UNK **, 0x260) = (M2C_UNK *) *var_s0;
        }
    }
    osSetIntMask(temp_a0);
}

s16 **func_8000C7C0(s16 **arg0) {
    return arg0 + 0x78;
}

void func_8000C7C8(s16 **arg0) {
    s16 *sp10;
    s16 *(*var_a0)[];
    s16 *(*var_a0_2)[];
    s16 *temp_a0;
    s16 *var_s0;
    s16 *var_s1;
    s32 temp_lo;
    s32 temp_v1;
    s32 var_s2;
    s32 var_s3;
    s32 var_s4;

    sp10 = NULL;
    var_a0 = arg0 + 0x40;
loop_1:
    osRecvMesg(var_a0, &sp10, 1);
    if (sp10 == (s16 *)0x29B) {
        func_8000CADC(arg0);
        var_a0 = (s16 *(*)[]) &arg0[0x10];
        goto loop_1;
    }
    if ((s32) sp10 < 0x29C) {
        var_a0 = (s16 *(*)[]) &arg0[0x10];
        if (sp10 != (s16 *)0x29A) {

        } else {
            func_8001BE90(var_a0);
            func_8001BE88();
            func_8001BE98(0x81);
            func_8000C9D8(arg0);
            func_8001BE98(0xC1);
            var_a0 = (s16 *(*)[]) &arg0[0x10];
        }
        goto loop_1;
    }
    if (sp10 == (s16 *)0x29C) {
        func_8000CBCC(arg0);
        var_a0 = (s16 *(*)[]) &arg0[0x10];
        goto loop_1;
    }
    var_s2 = 8;
    if (sp10 != (s16 *)0x29D) {
        var_a0 = arg0 + 0x40;
        goto loop_1;
    }
    var_a0_2 = arg0 + 0x40;
    do {
        osRecvMesg(var_a0_2, &sp10, 1);
        var_s2 -= (s32) sp10 == 0x29A;
        var_a0_2 = arg0 + 0x40;
    } while (var_s2 != 0);
    do {
        osRecvMesg(var_a0_2, &sp10, 1);
        if (sp10 == (s16 *)0x29A) {
            var_s3 = 0xEF;
            do {
                temp_v1 = gScreenWidth;
                var_s4 = var_s2;
                if (var_s2 < temp_v1) {
                    var_s0 = &(&D_807D7E00)[var_s2];
                    var_s1 = &(&D_807B2600)[var_s2];
                    do {
                        temp_lo = var_s3 * temp_v1;
                        temp_a0 = &var_s1[temp_lo];
                        *temp_a0 = 0;
                        var_s0[temp_lo] = 0;
                        osWritebackDCache(temp_a0, 2);
                        var_s4 += 8;
                        osWritebackDCache(&var_s0[var_s3 * gScreenWidth], 2);
                        var_s1 += 0x10;
                        var_s0 += 0x10;
                    } while (var_s4 < gScreenWidth);
                }
                var_s3 -= 1;
            } while (var_s3 >= 0);
        }
        var_s2 += 1;
        var_a0_2 = arg0 + 0x40;
    } while (var_s2 < 8);
loop_23:
    osRecvMesg(var_a0_2, &sp10, 1);
    var_a0_2 = arg0 + 0x40;
    goto loop_23;
}

void func_8000C9D8(s16 **arg0) {
    s16 *sp10;
    M2C_UNK *sp14;
    s32 sp18;
    s16 *(*var_a0)[];
    s16 *var_s0_2;
    s32 var_s0;

    sp10 = NULL;
    sp14 = NULL;
    sp18 = 0;
    arg0[0x9F] += 1;
    var_a0 = arg0 + 0x78;
loop_1:
    if (osRecvMesg(var_a0, &sp10, 0) != -1) {
        func_8000CD8C(arg0, sp10);
        var_a0 = (s16 *(*)[]) &arg0[0x1E];
        goto loop_1;
    }
    if (arg0[0xA0] != NULL) {
        if (arg0[0x9D] != NULL) {
            func_8000CEF8(arg0);
        } else {
            goto block_7;
        }
    } else {
block_7:
        var_s0 = (arg0[0x9D] == NULL) * 2;
        if (arg0[0x9E] == NULL) {
            var_s0 |= 1;
        }
        if (func_8000CF30(arg0, &sp14, &sp18, var_s0) != var_s0) {
            func_8000CDE4(arg0, sp14, sp18);
        }
    }
    var_s0_2 = arg0[0x98];
    if (var_s0_2 != NULL) {
        do {
            osSendMesg(M2C_FIELD(var_s0_2, s16 ***, 4), arg0, 0);
            var_s0_2 = M2C_FIELD(var_s0_2, s16 **, 0);
        } while (var_s0_2 != NULL);
    }
}

void func_8000CADC(s16 **arg0) {
    M2C_UNK *sp10;
    s32 sp14;
    s16 *temp_s0;
    s32 var_s0;

    temp_s0 = arg0[0x9D];
    arg0[0x9D] = NULL;
    sp10 = NULL;
    sp14 = 0;
    if ((M2C_FIELD(temp_s0, s32 *, 4) & 0x10) && (osSpTaskYielded(temp_s0 + 0x10) != 0)) {
        M2C_FIELD(temp_s0, s32 *, 4) = (s32) (M2C_FIELD(temp_s0, s32 *, 4) | 0x20);
        if ((M2C_FIELD(temp_s0, s32 *, 8) & 7) == 3) {
            M2C_FIELD(temp_s0, s16 **, 0) = (s16 *) arg0[0x9A];
            arg0[0x9A] = temp_s0;
            if (arg0[0x9C] == NULL) {
                arg0[0x9C] = temp_s0;
            }
        }
    } else {
        M2C_FIELD(temp_s0, s32 *, 4) = (s32) (M2C_FIELD(temp_s0, s32 *, 4) & ~2);
        func_8000CCB0(arg0, temp_s0);
    }
    var_s0 = (arg0[0x9D] == NULL) * 2;
    if (arg0[0x9E] == NULL) {
        var_s0 |= 1;
    }
    if (func_8000CF30(arg0, &sp10, &sp14, var_s0) != var_s0) {
        func_8000CDE4(arg0, sp10, sp14);
    }
}

void func_8000CBCC(s16 **arg0) {
    M2C_UNK *sp10;
    s32 sp14;
    s16 **temp_a1;
    s32 var_s0;

    temp_a1 = arg0[0x9E];
    sp10 = NULL;
    sp14 = 0;
    arg0[0x9E] = NULL;
    temp_a1[1] &= ~1;
    func_8000CCB0(temp_a1);
    var_s0 = (arg0[0x9D] == NULL) * 2;
    if (arg0[0x9E] == NULL) {
        var_s0 |= 1;
    }
    if (func_8000CF30(arg0, &sp10, &sp14, var_s0) != var_s0) {
        func_8000CDE4(arg0, sp10, sp14);
    }
}

s32 func_8000CC5C(s32 arg0) {
    s32 temp_s0;

    if (arg0 != 0) {
        temp_s0 = osViGetCurrentFramebuffer();
        return arg0 & -(temp_s0 == osViGetNextFramebuffer());
    }
    return 0;
}

s32 func_8000CCB0(s16 **arg1) {
    M2C_UNK var_a0;

    if (!(arg1[1] & 3)) {
        osSendMesg(arg1[0x14], arg1[0x15], 1);
        var_a0 = 0x48;
        if (arg1[4] == (s16 *)1) {
            if ((arg1[2] & 0x60) == 0x60) {
                if (D_80038478 != 0) {
                    osViBlack(0);
                    D_80038478 = 0;
                }
                func_80021B00(arg1[3]);
            }
            if (D_80043320 != 0) {
                D_80043320 -= 1;
            }
            var_a0 = 0x4A;
        }
        func_8001BE98(var_a0);
        return 1;
    }
    if (D_80043320 != 0) {
        D_80043320 -= 1;
    }
    func_8001BE98(0x49);
    return 0;
}

void func_8000CD8C(s16 **arg0, s16 *arg1) {
    s16 *temp_v0;
    s16 *temp_v0_2;

    if (M2C_FIELD(arg1, s32 *, 0x10) == 2) {
        temp_v0 = arg0[0x9B];
        if (temp_v0 == NULL) {
            arg0[0x99] = arg1;
        } else {
            *temp_v0 = arg1;
        }
        arg0[0x9B] = arg1;
        arg0[0xA0] = (s16 *)1;
    } else {
        temp_v0_2 = arg0[0x9C];
        if (temp_v0_2 == NULL) {
            arg0[0x9A] = arg1;
        } else {
            *temp_v0_2 = arg1;
        }
        arg0[0x9C] = arg1;
    }
    M2C_FIELD(arg1, s32 *, 0) = 0;
    M2C_FIELD(arg1, s32 *, 4) = (s32) (M2C_FIELD(arg1, s32 *, 8) & 3);
}

void func_8000CDE4(s16 **arg0, M2C_UNK *arg1, s32 arg2) {
    M2C_UNK var_a0;
    void *temp_s0;

    if (arg1 != NULL) {
        func_8001C990();
        if (M2C_FIELD(arg1, s32 *, 0x10) == 2) {
            var_a0 = 0x4A;
            switch (D_80043320) {                   /* irregular */
            case 2:
                var_a0 = 0x49;
                /* fallthrough */
            case 1:
                func_8001BE98(var_a0);
                break;
            }
            func_8001BE98(8);
        } else {
            func_8001BE98(9);
            D_80043320 = 2;
        }
        temp_s0 = arg1 + 0x10;
        M2C_FIELD(arg1, s32 *, 4) = (s32) (M2C_FIELD(arg1, s32 *, 4) & ~0x30);
        func_80021280(temp_s0);
        func_8002148C(temp_s0);
        arg0[0x9D] = arg1;
        if (arg1 == arg2) {
            arg0[0x9E] = arg2;
        }
    }
    if ((arg2 != 0) & (arg2 != (s32) arg1)) {
        func_800211E0(M2C_FIELD(arg2, u8 **, 0x38));
        D_80038470 = 1;
        D_80038474 = 0;
        arg0[0x9E] = arg2;
    }
}

void func_8000CEF8(s16 **arg0) {
    s16 *temp_a0;

    temp_a0 = arg0[0x9D];
    if (M2C_FIELD(temp_a0, s32 *, 0x10) == 1) {
        M2C_FIELD(temp_a0, s32 *, 4) = (s32) (M2C_FIELD(temp_a0, s32 *, 4) | 0x10);
        osSpTaskYield(temp_a0);
    }
}

s32 func_8000CF30(s16 **arg0, M2C_UNK **arg1, s32 *arg2, s32 arg3) {
    M2C_UNK *temp_s1;
    M2C_UNK *temp_v0_2;
    M2C_UNK *temp_v0_3;
    M2C_UNK *temp_v1;
    M2C_UNK *temp_v1_3;
    s32 temp_s0;
    s32 temp_s6;
    s32 temp_v0;
    s32 temp_v1_2;
    s32 var_s2;
    s32 var_v0;

    var_s2 = arg3;
    temp_s1 = arg0[0x9A];
    temp_s6 = var_s2;
    if ((arg0[0xA0] != NULL) && (var_s2 & 2)) {
        if ((temp_s1 == NULL) || !(M2C_FIELD(temp_s1, s32 *, 8) & 0x10)) {
            *arg1 = arg0[0x99];
            arg0[0xA0] = NULL;
            temp_v1 = *arg0[0x99];
            var_s2 &= ~2;
            arg0[0x99] = temp_v1;
            if (temp_v1 == NULL) {
                arg0[0x9B] = NULL;
            }
        } else {
            goto block_24;
        }
    } else {
        if (temp_s1 == NULL) {
            var_v0 = 0;
        } else {
            temp_s0 = osViGetCurrentFramebuffer();
            var_v0 = (s32) temp_s1 & -(temp_s0 == osViGetNextFramebuffer());
        }
        if (var_v0 != 0) {
            temp_v0 = M2C_FIELD(temp_s1, s32 *, 8) & 7;
            switch (temp_v0) {
            case 1:
            case 4:
            case 5:
                break;
            case 3:
                if (M2C_FIELD(temp_s1, s32 *, 4) & 0x20) {
                    if (var_s2 & 2) {
                        *arg1 = temp_s1;
                        var_s2 &= ~2;
                        if (M2C_FIELD(temp_s1, s32 *, 4) & 1) {
                            *arg2 = temp_s1;
                            var_s2 &= ~1;
                        }
                        temp_v0_2 = M2C_FIELD(arg0[0x9A], M2C_UNK **, 0);
                        arg0[0x9A] = temp_v0_2;
                        if (temp_v0_2 == NULL) {
                            arg0[0x9C] = NULL;
                        }
                    }
                } else if (var_s2 == 3) {
                    *arg2 = temp_s1;
                    *arg1 = temp_s1;
                    temp_v0_3 = M2C_FIELD(arg0[0x9A], M2C_UNK **, 0);
                    var_s2 = 0;
                    arg0[0x9A] = temp_v0_3;
                    if (temp_v0_3 == NULL) {
                        arg0[0x9C] = NULL;
                    }
                }
                break;
            case 2:
            case 6:
            case 7:
                temp_v1_2 = M2C_FIELD(temp_s1, s32 *, 4);
                if (temp_v1_2 & 2) {
                    if (var_s2 & 2) {
block_24:
                        *arg1 = temp_s1;
                        var_s2 &= ~2;
                    }
                } else if ((temp_v1_2 & 1) && (var_s2 & 1)) {
                    *arg2 = temp_s1;
                    temp_v1_3 = M2C_FIELD(arg0[0x9A], M2C_UNK **, 0);
                    var_s2 &= ~1;
                    arg0[0x9A] = temp_v1_3;
                    if (temp_v1_3 == NULL) {
                        arg0[0x9C] = NULL;
                    }
                }
                break;
            }
        }
    }
    if (var_s2 != temp_s6) {
        var_s2 = func_8000CF30(arg0, arg1, arg2, var_s2);
    }
    return var_s2;
}

void func_8000D150(void) {

}

void func_8000D160(void) {
    if ((D_80038498 == 1) && (func_8001E380(D_800A2350) != 0)) {
        D_800A236C = 0x7800;
        D_8003848A = func_8001E370(D_800A2350);
        D_8003848C = 0x10;
        D_80038498 = 2;
    }
}

void func_8000D1CC(u32 arg0) {
    s16 *sp10;
    s32 temp_a3;
    void *temp_v1;

    sp10 = NULL;
    if (D_80038484 == 0) {
        if ((D_80038498 == 1) && (D_80038498 == D_80038498) && (func_8001E380(D_800A2350) != 0)) {
            D_800A236C = 0x7800;
            D_8003848A = func_8001E370(D_800A2350);
            D_8003848C = 0x10;
            D_80038498 = 2;
        }
        if (D_80038498 == 2) {
            D_800384A0 = 1;
            if (D_80038498 == D_80038498) {
                osRecvMesg((s16 *(*)[]) D_80043330, &sp10, 1);
            }
            D_800384A0 = 0;
        }
        if (arg0 >= (u32) M2C_FIELD(D_800A2378, s16 *, 2)) {
            D_80038490 = 0;
        } else {
            D_80038490 = arg0;
        }
        temp_v1 = D_800A2378 + (D_80038490 * 8);
        temp_a3 = M2C_FIELD(temp_v1, s32 *, 8);
        D_80038498 = 4;
        Rom_Read(M2C_FIELD(temp_v1, u8 **, 4), D_800A2370, temp_a3 + (temp_a3 & 1), temp_a3);
        func_8001D8B0(D_800A2354, D_800A2370);
        func_8001E3F0(D_800A2350, D_800A2354);
        func_8001E420(D_800A2350, (s16) D_800A236C);
        D_80038498 = 3;
    }
}

s16 func_8000D368(s32 arg0) {
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_a1_2;
    u16 var_v1;
    u8 temp_a2;

    var_v1 = -1U;
    if (D_80038484 == 0) {
        temp_a0 = arg0 & 0xFFFF;
        temp_a2 = *(&D_800A6178 + temp_a0);
        temp_a1 = (*(&D_800384CC + temp_a0) + temp_a2) & 0xFF;
        if (temp_a1 >= M2C_FIELD(D_800A2364, s16 *, 0xE)) {
            return -1;
        }
        temp_a1_2 = temp_a1 * 0xC;
        *(&D_800A6018 + temp_a1_2) = D_800384E8[temp_a1];
        *(&D_800A6178 + temp_a0) = (s8) ((s32) (temp_a2 + 1) % (s32) *(&D_800384B0 + temp_a0));
        *(&D_800A601C + temp_a1_2) = 0x7F00;
        *(&D_800A601E + temp_a1_2) = 0x40;
        *(&D_800A601F + temp_a1_2) = 0;
        if ((D_800A6020[temp_a1].unk0 & 0xFF) != 3) {
            D_800A6020[temp_a1].unk0 = 2;
        }
        var_v1 = *((temp_a1 * 2) + D_800A2368);
        goto block_6;
    }
block_6:
    return (s16) var_v1;
}

s16 func_8000D49C(u16 arg0, f32 arg1, s16 arg2, u8 arg3, u8 arg4) {
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_a1_2;
    u16 var_v1;
    u8 temp_t0;

    var_v1 = -1U;
    if (D_80038484 == 0) {
        temp_a0 = arg0 & 0xFFFF;
        temp_t0 = *(&D_800A6178 + temp_a0);
        temp_a1 = (*(&D_800384CC + temp_a0) + temp_t0) & 0xFF;
        if (temp_a1 >= M2C_FIELD(D_800A2364, s16 *, 0xE)) {
            return -1;
        }
        temp_a1_2 = temp_a1 * 0xC;
        *(&D_800A6018 + temp_a1_2) = arg1;
        *(&D_800A6178 + temp_a0) = (s8) ((s32) (temp_t0 + 1) % (s32) *(&D_800384B0 + temp_a0));
        *(&D_800A601C + temp_a1_2) = arg2;
        *(&D_800A601E + temp_a1_2) = arg3;
        *(&D_800A601F + temp_a1_2) = arg4;
        if ((D_800A6020[temp_a1].unk0 & 0xFF) != 3) {
            D_800A6020[temp_a1].unk0 = 2;
        }
        var_v1 = *((temp_a1 * 2) + D_800A2368);
        goto block_6;
    }
block_6:
    return (s16) var_v1;
}

void func_8000D5C4(s32 arg0, f32 arg1, s16 arg2, s8 arg3, u8 arg4) {
    s32 temp_a0;
    s32 temp_a0_2;

    temp_a0 = arg0 & 0xFFFF;
    if ((D_80038484 == 0) && (temp_a0 < M2C_FIELD(D_800A2364, s16 *, 0xE))) {
        temp_a0_2 = temp_a0 * 0xC;
        *(&D_800A6018 + temp_a0_2) = arg1;
        *(&D_800A601C + temp_a0_2) = arg2;
        *(&D_800A601E + temp_a0_2) = arg3;
        *(&D_800A601F + temp_a0_2) = arg4;
        if (((D_800A6020[temp_a0].unk0 & 0xFF) != 2) && ((D_800A6020[temp_a0].unk0 & 0xFF) != 3)) {
            D_800A6020[temp_a0].unk0 = 5;
        }
    }
}

void func_8000D67C(s32 arg0) {
    s32 temp_a0;

    temp_a0 = arg0 & 0xFFFF;
    if (temp_a0 < M2C_FIELD(D_800A2364, s16 *, 0xE)) {
        D_800A6020[temp_a0].unk0 = 6;
    }
}

void func_8000D6B8(void) {
    s16 temp_v0;
    s16 var_a0;
    s32 var_v1;

    var_a0 = 0;
    if (M2C_FIELD(D_800A2364, s16 *, 0xE) > 0) {
        var_v1 = 0 << 0x10;
        do {
            D_800A6020[var_v1 >> 0x10].unk0 = 6;
            temp_v0 = var_a0 + 1;
            var_a0 = temp_v0;
            var_v1 = var_a0 << 0x10;
        } while (temp_v0 < M2C_FIELD(D_800A2364, s16 *, 0xE));
    }
}

void func_8000D720(u16 arg0) {
    s16 temp_a1;
    s32 temp_s0;
    s32 temp_v0;
    u32 var_s1;
    u32 var_s1_2;
    void *temp_a1_2;
    void *temp_v0_2;
    void *temp_v0_3;

    temp_v0 = arg0 & 0xFFFF;
    if ((temp_v0 != D_8003848E) && (temp_v0 == 0)) {
        if (D_8003848E != 0xFFFF) {
            temp_v0_2 = M2C_FIELD(((D_8003848E * 4) + D_800A2360), void **, 0xC);
            D_800A2364 = temp_v0_2;
            var_s1 = 0;
            if (M2C_FIELD(temp_v0_2, s16 *, 0xE) != 0) {
                do {
                    temp_s0 = var_s1 * 2;
                    temp_a1 = *(temp_s0 + D_800A2368);
                    var_s1 += 1;
                    func_8001FEF0(D_800A2358, temp_a1);
                    func_8001FF70(D_800A2358);
                    func_8001FEA0(D_800A2358, D_800A2368[temp_s0]);
                } while (var_s1 < (u32) M2C_FIELD(D_800A2364, s16 *, 0xE));
            }
        }
        temp_v0_3 = M2C_FIELD((((arg0 & 0xFFFF) * 4) + D_800A2360), void **, 0xC);
        D_800A2364 = temp_v0_3;
        var_s1_2 = 0;
        if (M2C_FIELD(temp_v0_3, s16 *, 0xE) != 0) {
            do {
                temp_a1_2 = M2C_FIELD(((var_s1_2 * 4) + D_800A2364), void **, 0x10);
                D_800A235C = temp_a1_2;
                *((var_s1_2 * 2) + D_800A2368) = func_8001FDE0(D_800A2358, temp_a1_2);
                var_s1_2 += 1;
            } while (var_s1_2 < (u32) M2C_FIELD(D_800A2364, s16 *, 0xE));
        }
        D_8003848E = arg0;
        D_8003849C = 1;
    }
}

void func_8000D890(s16 arg0) {
    s16 temp_s0;
    s32 temp_v0;

    temp_v0 = arg0 & 0xFFFF;
    if ((temp_v0 < M2C_FIELD(D_800A2364, s16 *, 0xE)) & (temp_v0 != 0xFFFF)) {
        temp_s0 = func_8001FF60(D_800A2358);
        func_8001FEF0(D_800A2358, arg0);
        D_80038488 = arg0;
        if (temp_s0 != -1) {
            func_8001FEF0(D_800A2358, temp_s0);
        }
    } else {
        D_80038488 = -1;
    }
}

void Audio_Init(void) {
    s32 sp18;
    s32 sp1C;
    s32 sp20;
    u32 (*(*sp28)(u8 **))(u32, s32);
    M2C_UNK *sp2C;
    s32 sp30;
    s8 sp34;
    s32 sp40;
    s32 sp44;
    s8 sp48;
    M2C_UNK *sp4C;
    s32 (*sp50)(M2C_UNK **, f32 *, s8, s32, u32, u8);
    s32 (*sp54)(void *, f32 *);
    void (*sp58)(M2C_UNK *);
    u32 sp60;
    s32 sp64;
    M2C_UNK *sp68;
    s32 sp70;
    s32 sp74;
    s32 sp78;
    s32 temp_s2;
    s32 temp_s2_2;
    s32 temp_v0_4;
    s8 *var_a0;
    u32 var_v1;
    u8 *temp_v0;
    u8 *temp_v0_2;
    u8 *temp_v0_3;
    u8 *temp_v0_5;

    var_a0 = D_80043350;
    var_v1 = 0;
    D_800A47E0 = 0;
    do {
        *var_a0 = 0;
        var_v1 += 1;
        var_a0 += 1;
    } while (var_v1 <= 0x5EFFFU);
    func_8001D820(&D_800A2380, D_80043350, 0x5F000);
    D_800A2354 = alHeapDBAlloc(NULL, 0, &D_800A2380, 1U, 0xF8);
    D_800A2350 = alHeapDBAlloc(NULL, 0, &D_800A2380, 1U, 0x7C);
    temp_s2 = &D_1982B0 - &D_196CC0;
    D_800A2358 = alHeapDBAlloc(NULL, 0, &D_800A2380, 1U, 0x54);
    temp_v0 = alHeapDBAlloc(NULL, 0, &D_800A2380, 1U, temp_s2);
    Rom_Read(&D_196CC0, temp_v0, temp_s2);
    temp_s2_2 = &D_1E3EA0 - &D_1E2870;
    temp_v0_2 = alHeapDBAlloc(NULL, 0, &D_800A2380, 1U, temp_s2_2);
    Rom_Read(&D_1E2870, temp_v0_2, temp_s2_2);
    temp_v0_3 = alHeapDBAlloc(NULL, 0, &D_800A2380, 1U, 4);
    D_800A2378 = temp_v0_3;
    Rom_Read(&D_1D7720, temp_v0_3, 4);
    temp_v0_4 = (M2C_FIELD(D_800A2378, s16 *, 2) * 8) + 4;
    D_800A2374 = temp_v0_4;
    temp_v0_5 = alHeapDBAlloc(NULL, 0, &D_800A2380, 1U, temp_v0_4);
    D_800A2378 = temp_v0_5;
    Rom_Read(&D_1D7720, temp_v0_5, D_800A2374);
    D_800A2370 = alHeapDBAlloc(NULL, 0, &D_800A2380, 1U, 0x2000);
    sp18 = 0x28;
    sp1C = 0x28;
    sp20 = 0x100;
    sp28 = func_8000EA88;
    sp30 = func_80021010(0xAC44);
    sp34 = 1;
    sp70 = 0xAC44;
    sp74 = 1;
    sp2C = &D_800A2380;
    sp78 = 0x1800;
    func_8000DCE0(&sp18, (M2C_UNK *)0xE, &sp70);
    func_8001D4A0(D_800A2378, &D_1D7720);
    sp48 = 0x10;
    sp50 = func_8000EBDC;
    sp54 = func_8000EF58;
    sp40 = 0x28;
    sp44 = 0x100;
    sp4C = &D_800A2380;
    sp58 = func_8000F334;
    func_8001D4E0(temp_v0, &D_1982B0);
    func_8001E480(D_800A2350, &sp40);
    func_8001E3C0(D_800A2350, M2C_FIELD(temp_v0, s32 *, 4));
    func_8001D4E0(temp_v0_2, &D_1E3EA0);
    D_800A2360 = M2C_FIELD(temp_v0_2, s32 *, 4);
    D_800A2368 = alHeapDBAlloc(NULL, 0, &D_800A2380, 1U, 0x3A);
    sp60 = 0x1D;
    sp64 = 0x100;
    sp68 = &D_800A2380;
    func_8001F6E0(D_800A2358, &sp60);
    osStartThread(D_800A23A8);
    D_800A236C = 0x7800;
    func_8001E420(D_800A2350, (s16) D_800A236C);
    func_8000D720(0U);
}

void func_8000DCE0(s32 *arg0, M2C_UNK *arg2) {
    M2C_UNK sp18;
    M2C_UNK *temp_s5;
    M2C_UNK *var_a0;
    M2C_UNK *var_a2;
    M2C_UNK *var_a3;
    M2C_UNK *var_s2;
    M2C_UNK *var_s3;
    M2C_UNK *var_v1;
    f32 temp_f4;
    f64 var_f2;
    f64 var_f2_2;
    s32 temp_f0;
    s32 temp_v0;
    s32 var_s1;
    u32 var_s0;
    u32 var_s0_2;
    u32 var_s0_3;
    u32 var_s0_4;
    u8 **var_s1_2;
    u8 **var_s1_3;
    u8 *temp_v0_2;
    u8 *temp_v0_3;

    var_a2 = arg2;
    temp_s5 = var_a2;
    temp_v0 = M2C_FIELD(temp_s5, s32 *, 4);
    var_f2 = (f64) temp_v0;
    if (temp_v0 < 0) {
        var_f2 += 4294967296.0;
    }
    temp_f4 = ((f32) var_f2 * (f32) M2C_FIELD(arg0, s32 *, 0x18)) / 60.0f;
    temp_f0 = (s32) temp_f4;
    var_f2_2 = (f64) temp_f0;
    D_800A51F4 = temp_f0;
    if (temp_f0 < 0) {
        var_f2_2 += 4294967296.0;
    }
    if ((f32) var_f2_2 < temp_f4) {
        D_800A51F4 = temp_f0 + 1;
    }
    if (D_800A51F4 & 0xF) {
        D_800A51F4 = (D_800A51F4 & ~0xF) + 0x10;
    }
    D_800A51F0 = D_800A51F4 - 0x10;
    D_800A51F8 = D_800A51F4 + 0x60;
    var_a3 = &sp18;
    if (M2C_FIELD(arg0, u8 *, 0x1C) == 6) {
        var_a2 = &D_8003D6B0;
        do {
            M2C_FIELD(var_a3, s32 *, 0) = (s32) M2C_FIELD(var_a2, s32 *, 0);
            M2C_FIELD(var_a3, s32 *, 4) = (s32) M2C_FIELD(var_a2, s32 *, 4);
            M2C_FIELD(var_a3, s32 *, 8) = (s32) M2C_FIELD(var_a2, s32 *, 8);
            M2C_FIELD(var_a3, s32 *, 0xC) = (s32) M2C_FIELD(var_a2, s32 *, 0xC);
            var_a2 += 0x10;
            var_a3 += 0x10;
        } while (var_a2 != (&D_8003D6B0 + 0x60));
        M2C_FIELD(var_a3, s32 *, 0) = (s32) M2C_FIELD(var_a2, s32 *, 0);
        M2C_FIELD(var_a3, s32 *, 4) = (s32) M2C_FIELD(var_a2, s32 *, 4);
        M2C_FIELD(arg0, M2C_UNK **, 0x20) = &sp18;
    }
    func_8001D770(D_800A2788, arg0, var_a2, var_a3);
    D_800A6194 = D_800A61A0;
    var_a0 = D_800A61A0;
    var_s0 = 0;
    var_v1 = &D_800A61A0[5];
    do {
        *var_a0 = var_v1;
        var_a0 = var_v1;
        var_s0 += 1;
        var_v1 = &var_a0[5];
    } while (var_s0 < 0x4FU);
    var_s0_2 = 0;
    var_s3 = &D_800A47F4 - 4;
    var_s2 = &D_800A47F4 + 0x10;
    var_s1 = 0;
    *var_a0 = NULL;
    D_800A47F4 = NULL;
    D_800A47F0 = NULL;
    do {
        alLink(&*var_s2, var_s3);
        var_s3 += 0x14;
        var_s2 += 0x14;
        var_s0_2 += 1;
        *(D_800A4800 + var_s1) = alHeapDBAlloc(NULL, 0, M2C_FIELD(arg0, M2C_UNK **, 0x14), 1U, 0x600);
        var_s1 += 0x14;
    } while (var_s0_2 < 0x7FU);
    var_s1_2 = &D_800A2390;
    var_s0_3 = 0;
    D_800A4800[var_s0_2].unk0 = alHeapDBAlloc(NULL, 0, M2C_FIELD(arg0, M2C_UNK **, 0x14), 1U, 0x600);
    do {
        var_s0_3 += 1;
        *var_s1_2 = alHeapDBAlloc(NULL, 0, M2C_FIELD(arg0, M2C_UNK **, 0x14), 1U, M2C_FIELD(temp_s5, s32 *, 8) * 8);
        var_s1_2 += 4;
    } while (var_s0_3 < 2U);
    var_s0_4 = 0;
    var_s1_3 = &D_800A2398;
    do {
        temp_v0_2 = alHeapDBAlloc(NULL, 0, M2C_FIELD(arg0, M2C_UNK **, 0x14), 1U, 0x80);
        *var_s1_3 = temp_v0_2;
        M2C_FIELD(temp_v0_2, s16 *, 0x60) = 2;
        temp_v0_3 = *var_s1_3;
        M2C_FIELD(temp_v0_3, u8 **, 0x64) = temp_v0_3;
        var_s0_4 += 1;
        M2C_FIELD(*var_s1_3, u8 **, 0) = alHeapDBAlloc(NULL, 0, M2C_FIELD(arg0, M2C_UNK **, 0x14), 1U, 0x1000);
        var_s1_3 += 4;
    } while (var_s0_4 < 3U);
    osCreateMesgQueue((s16 *(*)[]) D_800A2670, &D_800A2670[6], (s16 *)0x40);
    osCreateMesgQueue((s16 *(*)[]) (D_800A2670 - 0x118), D_800A2670 - 0x100, (s16 *)0x40);
    osCreateMesgQueue((s16 *(*)[]) D_800A5E00, D_800A5E18, (s16 *)0x80);
    osCreateMesgQueue((s16 *(*)[]) D_800B0508, &D_800C53DC, (s16 *)1);
    osCreateMesgQueue((s16 *(*)[]) D_80043330, &D_80043348, (s16 *)1);
    osCreateThread(D_800A2670 - 0x2C8, 3, func_8000E0A4, NULL, (s16 **) &D_800A47E0, 0xE);
}

void func_8000E0A4(void) {
    s16 sp10;
    s16 *sp18;
    s16 *sp1C;
    s16 temp_v0;
    s16 temp_v1;
    s16 var_s0_2;
    s32 temp_a0;
    s32 temp_s0;
    s32 temp_s0_2;
    s32 temp_s0_3;
    s32 temp_s1;
    s32 var_s0;
    s32 var_s2;
    s32 var_s4;
    s32 var_s5;
    u32 temp_v1_2;
    void *var_s7;

    var_s5 = 0;
    var_s7 = NULL;
    var_s4 = 0;
    sp18 = NULL;
    sp1C = NULL;
    func_8000C6D8(D_800C0AC0, &sp10, (s16 *(*)[]) D_800A2558);
    do {
        osRecvMesg((s16 *(*)[]) D_800A2558, &sp18, 1);
        temp_v1 = M2C_FIELD(sp18, s16 *, 0);
        switch (temp_v1) {                          /* switch 1; irregular */
        case 1:                                     /* switch 1 */
            if ((D_80038480 == 0) && ((D_80038484 == 0) || (D_8003848A != 0))) {
                D_80038480 = 1;
                if (func_8000E740(M2C_FIELD(&D_800A2558[D_800384A4 % 3], void **, -0x1C0), var_s7) != 0) {
                    osRecvMesg((s16 *(*)[]) (D_800A2558 + 0x118), &sp18, 1);
                    var_s7 = M2C_FIELD(sp18, void **, 4);
                }
                var_s2 = 0;
                do {
                    temp_s1 = var_s2 & 0xFF;
                    func_8001FEF0(D_800A2358, *((temp_s1 * 2) + D_800A2368));
                    temp_a0 = func_8001FFC0(D_800A2358);
                    temp_v1_2 = D_800A6020[temp_s1].unk0 & 0xFF;
                    switch (temp_v1_2) {            /* switch 2 */
                    case 6:                         /* switch 2 */
                        func_8001FF70(D_800A2358);
                        D_800A6020[var_s2 & 0xFF].unk0 = 4;
                        var_s2 += 1;
                        break;
                    case 1:                         /* switch 2 */
                    case 4:                         /* switch 2 */
                        if (temp_a0 == 0) {
                            D_800A6020[var_s2 & 0xFF].unk0 = 0;
                            var_s2 += 1;
                        } else {
                        default:                    /* switch 2 */
block_22:
                            var_s2 += 1;
                        }
                        break;
                    case 2:                         /* switch 2 */
                        temp_s0 = var_s2 & 0xFF;
                        if (temp_a0 == 0) {
                            var_s0 = temp_s0 * 0xC;
                            func_8001FFE0(D_800A2358, *(&D_800A6018 + var_s0));
                            func_80020100(D_800A2358, *(&D_800A601F + var_s0));
                            func_800200B0(D_800A2358, *(&D_800A601E + var_s0));
                            func_80020060(D_800A2358, (s16) *(&D_800A601C + var_s0));
                            func_80020030(D_800A2358, *((temp_s0 * 2) + D_800A2368), 0xA);
                            func_8001FF00(D_800A2358);
block_21:
                            *(D_800A6020 + var_s0) = 1;
                            goto block_22;
                        }
                        func_8001FF70(D_800A2358);
                        D_800A6020[var_s2 & 0xFF].unk0 = 3;
                        var_s2 += 1;
                        break;
                    case 3:                         /* switch 2 */
                        if (temp_a0 == 0) {
                            temp_s0_2 = var_s2 & 0xFF;
                            var_s0 = temp_s0_2 * 0xC;
                            func_8001FFE0(D_800A2358, *(&D_800A6018 + var_s0));
                            func_80020100(D_800A2358, *(&D_800A601F + var_s0));
                            func_800200B0(D_800A2358, *(&D_800A601E + var_s0));
                            func_80020060(D_800A2358, (s16) *(&D_800A601C + var_s0));
                            func_80020030(D_800A2358, *((temp_s0_2 * 2) + D_800A2368), 0xA);
                            func_8001FF00(D_800A2358);
                            goto block_21;
                        }
                        goto block_22;
                    case 5:                         /* switch 2 */
                        temp_s0_3 = var_s2 & 0xFF;
                        var_s0 = temp_s0_3 * 0xC;
                        func_8001FFE0(D_800A2358, *(&D_800A6018 + var_s0));
                        func_80020100(D_800A2358, *(&D_800A601F + var_s0));
                        func_800200B0(D_800A2358, *(&D_800A601E + var_s0));
                        func_80020060(D_800A2358, (s16) *(&D_800A601C + var_s0));
                        func_80020030(D_800A2358, *((temp_s0_3 * 2) + D_800A2368), 0xA);
                        goto block_21;
                    }
                } while ((u32) (var_s2 & 0xFF) < 0x1DU);
                if (D_80038498 == 3) {
                    func_8001E390(D_800A2350);
                    D_80038498 = 1;
                }
                if (((u16) D_80038488 & 0xFFFF) != 0xFFFF) {
                    if (var_s4 == 0) {
                        if (func_8001FFC0(D_800A2358) == 1) {
                            var_s4 = 1;
                        }
                    } else if (func_8001FFC0(D_800A2358) == 0) {
                        var_s4 = 0;
                        D_80038488 = -1;
                        osSendMesg(D_800B0508, &sp1C, 0);
                    }
                }
                if (D_80038498 == 2) {
                    if (D_8003848C != 0) {
                        func_8001E450(D_800A2350);
                        func_8001E420(D_800A2350, (s16) D_800A236C);
                        D_8003848C = 0;
                    } else if (func_8001E380(D_800A2350) == 0) {
                        if (D_800384A0 == 1) {
                            osSendMesg(D_80043330, &sp1C, 0);
                            D_800384A0 = 0;
                        }
                        D_80038498 = 0;
                    }
                }
                D_80038480 = 0;
            }
            break;
        case 4:                                     /* switch 1 */
            D_80038484 = 1;
            func_8001E450(D_800A2350);
            if (D_8003849C != 0) {
                var_s0_2 = 0;
                if (M2C_FIELD(D_800A2364, s16 *, 0xE) > 0) {
                    do {
                        func_8001FEF0(D_800A2358, D_800A2368[(s32) (var_s0_2 << 0x10) >> 0xF]);
                        func_8001FF70(D_800A2358);
                        temp_v0 = var_s0_2 + 1;
                        var_s0_2 = temp_v0;
                    } while (temp_v0 < M2C_FIELD(D_800A2364, s16 *, 0xE));
                }
            }
            osViBlack(1);
            var_s5 = 1;
            break;
        }
    } while (var_s5 == 0);
    func_8001D79C(D_800A2788);
loop_48:
    osRecvMesg((s16 *(*)[]) D_800A2558, &sp18, 1);
    goto loop_48;
}

s32 func_8000E740(void *arg0, void *arg1) {
    s32 sp10;
    s16 temp_v1;
    u32 temp_a3;
    u32 temp_s0;
    u8 *temp_t1;

    func_8000EACC();
    if (arg1 != NULL) {
        func_80021140(M2C_FIELD(arg1, u8 **, 0), M2C_FIELD(arg1, s16 *, 4) * 4);
    }
    temp_s0 = osVirtualToPhysical(M2C_FIELD(arg0, u8 **, 0));
    temp_v1 = (((D_800A51F4 - (func_80021000() >> 2)) + 0x50) & 0xFFF0) + 0x10;
    M2C_FIELD(arg0, s16 *, 4) = temp_v1;
    if ((u32) temp_v1 < (u32) D_800A51F0) {
        M2C_FIELD(arg0, s16 *, 4) = (s16) D_800A51F0;
    }
    temp_t1 = func_8002041C((&D_800A2390)[D_800384AC], &sp10, temp_s0, M2C_FIELD(arg0, s16 *, 4));
    if (sp10 != 0) {
        M2C_FIELD(arg0, u8 ***, 0x58) = (u8 **) (&D_800A2390 + 0x2E0);
        M2C_FIELD(arg0, void **, 0x5C) = (void *) (arg0 + 0x60);
        M2C_FIELD(arg0, s32 *, 8) = 0;
        M2C_FIELD(arg0, s32 *, 0x10) = 2;
        M2C_FIELD(arg0, u8 **, 0x48) = (u8 *) (&D_800A2390)[D_800384AC];
        M2C_FIELD(arg0, s32 *, 0x24) = (s32) (D_80034070 - D_80033FA0);
        M2C_FIELD(arg0, M2C_UNK (**)(), 0x28) = D_80034070;
        M2C_FIELD(arg0, M2C_UNK **, 0x30) = &D_8003B790;
        M2C_FIELD(arg0, s32 *, 0x2C) = 0x1000;
        M2C_FIELD(arg0, s32 *, 0x18) = 2;
        M2C_FIELD(arg0, M2C_UNK (**)(), 0x20) = D_80033FA0;
        M2C_FIELD(arg0, s32 *, 0x1C) = 0;
        M2C_FIELD(arg0, s32 *, 0x38) = 0;
        M2C_FIELD(arg0, s32 *, 0x3C) = 0;
        M2C_FIELD(arg0, s32 *, 0x40) = 0;
        M2C_FIELD(arg0, s32 *, 0x44) = 0;
        M2C_FIELD(arg0, s32 *, 0x34) = 0x800;
        M2C_FIELD(arg0, s32 *, 0x50) = 0;
        M2C_FIELD(arg0, s32 *, 0x54) = 0;
        temp_a3 = ((s32) (temp_t1 - (&D_800A2390)[D_800384AC]) >> 3) * 8;
        M2C_FIELD(arg0, u32 *, 0x4C) = temp_a3;
        osSendMesg(D_800B0568, arg0 + 8, 1, temp_a3);
        D_800384AC ^= 1;
        return 1;
    }
    return 0;
}

u32 func_8000E904(u32 arg0, s32 arg1) {
    M2C_UNK *temp_a0;
    M2C_UNK *temp_s0;
    M2C_UNK *temp_s1;
    M2C_UNK *var_s0_2;
    M2C_UNK *var_s1;
    s32 var_s0;
    struct _struct_D_800A5200_0x18 *temp_a0_2;
    u32 temp_v0;
    u8 *temp_s2;
    u8 *temp_s3;

    var_s1 = D_800A47E4;
    var_s0_2 = NULL;
    if (var_s1 != NULL) {
loop_1:
        temp_v0 = M2C_FIELD(var_s1, u32 *, 8);
        if (arg0 >= temp_v0) {
            var_s0_2 = var_s1;
            if ((temp_v0 + 0x600) < (arg0 + arg1)) {
                var_s1 = M2C_FIELD(var_s1, M2C_UNK **, 0);
                if (var_s1 == NULL) {
                    goto block_4;
                }
                goto loop_1;
            }
            M2C_FIELD(var_s1, u32 *, 0xC) = (u32) D_800384A4;
            return osVirtualToPhysical((M2C_FIELD(var_s1, s32 *, 0x10) + arg0) - M2C_FIELD(var_s1, u32 *, 8));
        }
        goto block_4;
    }
block_4:
    temp_s1 = D_800A47E8;
    temp_a0 = temp_s1;
    D_800A47E8 = M2C_FIELD(temp_s1, M2C_UNK **, 0);
    alUnlink(temp_a0);
    if (var_s0_2 != NULL) {
        alLink(temp_s1, var_s0_2);
        var_s0 = arg0 & 1;
    } else {
        temp_s0 = D_800A47E4;
        if (D_800A47E4 != NULL) {
            D_800A47E4 = temp_s1;
            M2C_FIELD(temp_s1, M2C_UNK **, 0) = temp_s0;
            M2C_FIELD(temp_s1, s32 *, 4) = 0;
            M2C_FIELD(temp_s0, M2C_UNK **, 4) = temp_s1;
        } else {
            D_800A47E4 = temp_s1;
            M2C_FIELD(temp_s1, M2C_UNK **, 0) = NULL;
            M2C_FIELD(temp_s1, s32 *, 4) = 0;
        }
        var_s0 = arg0 & 1;
    }
    temp_s3 = arg0 - var_s0;
    temp_s2 = M2C_FIELD(temp_s1, u8 **, 0x10);
    M2C_FIELD(temp_s1, u8 **, 8) = temp_s3;
    temp_a0_2 = &D_800A5200[D_800384A8];
    M2C_FIELD(temp_s1, u32 *, 0xC) = (u32) D_800384A4;
    D_800384A8 += 1;
    func_80025950(temp_a0_2, 0, 0, temp_s3, temp_s2, 0x600, (s16 *(*)[]) D_800A5E00);
    return osVirtualToPhysical(temp_s2) + var_s0;
}

u32 (*func_8000EA88(u8 **arg0))(u32, s32) {
    if (D_800A47E0 == 0) {
        D_800A47E8 = &D_800A47F0;
        D_800A47E4 = NULL;
        D_800A47E0 = 1;
    }
    *arg0 = &D_800A47E0;
    return func_8000E904;
}

u32 func_8000EACC(void) {
    M2C_UNK *temp_s1;
    M2C_UNK *var_s0;

    var_s0 = M2C_FIELD(&D_800A47E4, M2C_UNK **, 0);
    if (var_s0 != NULL) {
        do {
            temp_s1 = M2C_FIELD(var_s0, M2C_UNK **, 0);
            if ((u32) (M2C_FIELD(var_s0, s32 *, 0xC) + 2) < (u32) D_800384A4) {
                if (M2C_FIELD(&D_800A47E4, M2C_UNK **, 0) == var_s0) {
                    M2C_FIELD(&D_800A47E4, M2C_UNK **, 0) = temp_s1;
                }
                alUnlink(var_s0);
                if (M2C_FIELD(&D_800A47E4, M2C_UNK **, 4) == NULL) {
                    M2C_FIELD(&D_800A47E4, M2C_UNK **, 4) = var_s0;
                    M2C_FIELD(var_s0, M2C_UNK **, 0) = NULL;
                    M2C_FIELD(var_s0, s32 *, 4) = 0;
                    goto block_10;
                }
                alLink(var_s0, M2C_FIELD(&D_800A47E4, M2C_UNK **, 4));
                var_s0 = temp_s1;
            } else {
block_10:
                var_s0 = temp_s1;
            }
        } while (var_s0 != NULL);
    }
    D_800384A8 = 0;
    D_800384A4 += 1;
    return D_800384A4;
}

void func_8000EB98(u32 arg0) {
    u32 var_a0;

    var_a0 = arg0;
    if (var_a0 & 0xFF) {
        do {
            if (var_a0 & 1) {

            }
            var_a0 = (u32) (var_a0 & 0xFF) >> 1;
        } while (var_a0 != 0);
    }
}

s32 func_8000EBDC(M2C_UNK **arg0, f32 *arg1, s8 arg2, s32 arg3, u32 arg4, u8 arg5) {
    M2C_UNK *temp_s1;
    f32 var_f0;
    f32 var_f0_2;
    f32 var_f0_3;
    f32 var_f0_4;
    f32 var_f0_5;
    f32 var_f2;
    f32 var_f2_2;
    f32 var_f2_3;
    f32 var_f2_4;
    s16 temp_v0_2;
    s16 temp_v1_2;
    s16 temp_v1_3;
    s16 temp_v1_4;
    s32 temp_a2;
    s32 temp_f0;
    s32 var_s3;
    s32 var_v0;
    s32 var_v0_2;
    s8 temp_v0;
    s8 temp_v0_3;
    u32 temp_v1;
    u32 var_a0;
    u32 var_a0_2;
    u32 var_a0_3;
    u32 var_v1;

    var_s3 = 0;
    if (D_800A6194 != NULL) {
        temp_s1 = D_800A6194;
        var_s3 = arg5 << 0xE;
        D_800A6194 = M2C_FIELD(temp_s1, M2C_UNK **, 0);
        M2C_FIELD(temp_s1, s8 *, 4) = arg2;
        temp_a2 = arg2 & 0xFF;
        *arg0 = temp_s1;
        if (temp_a2 != 4) {
            if (temp_a2 < 5) {
                if (temp_a2 != 2) {
                    if (temp_a2 < 3) {
                        if (temp_a2 != 1) {
                            return var_s3;
                        }
                        temp_v1 = (u32) (arg4 & 0xFF) >> 1;
                        M2C_FIELD(temp_s1, s16 *, 6) = (s16) (0x103 - (arg3 & 0xFF));
                        temp_v0 = 0x7F - temp_v1;
                        M2C_FIELD(temp_s1, s8 *, 0xD) = temp_v0;
                        var_f0 = (f32) (temp_v0 & 0xFF);
                        M2C_FIELD(temp_s1, s16 *, 8) = 0;
                        M2C_FIELD(temp_s1, s8 *, 0xC) = (s8) temp_v1;
                        goto block_40;
                    }
                    var_f0 = 127.0f;
                    M2C_FIELD(temp_s1, s16 *, 6) = (s16) (0x100 - (arg3 & 0xFF));
                    M2C_FIELD(temp_s1, s16 *, 8) = 0;
                    M2C_FIELD(temp_s1, s8 *, 0xD) = (s8) arg4;
                    M2C_FIELD(temp_s1, s8 *, 0xC) = 0x7F;
                    goto block_40;
                }
                var_f0 = 127.0f;
                temp_v0_2 = 0x100 - (arg3 & 0xFF);
                M2C_FIELD(temp_s1, s16 *, 6) = temp_v0_2;
                M2C_FIELD(temp_s1, s16 *, 8) = temp_v0_2;
                M2C_FIELD(temp_s1, s8 *, 0xE) = (s8) (0x7F - arg4);
                M2C_FIELD(temp_s1, s8 *, 5) = 0;
                M2C_FIELD(temp_s1, s8 *, 0xD) = 0x7F;
                M2C_FIELD(temp_s1, s8 *, 0xC) = 0x7F;
                goto block_40;
            }
            if (temp_a2 != 0x81) {
                if (temp_a2 < 0x82) {
                    var_v1 = arg4;
                    if (temp_a2 != 0x80) {
                        return var_s3;
                    }
                    var_f0_2 = 1.030993f;
                    var_f2 = 1.0f;
                    var_v0 = arg3 & 0xFF;
                    if (var_v1 & 0xFF) {
                        do {
                            if (var_v1 & 1) {
                                var_f2 *= var_f0_2;
                            }
                            var_v1 = (u32) (var_v1 & 0xFF) >> 1;
                            var_f0_2 *= var_f0_2;
                        } while (var_v1 != 0);
                        var_v0 = arg3 & 0xFF;
                    }
                    var_f0 = 1.0f;
                    temp_s1[3] = M2C_BITWISE(M2C_UNK *, var_f2);
                    M2C_FIELD(temp_s1, s16 *, 8) = 0;
                    M2C_FIELD(temp_s1, s16 *, 6) = (s16) (0x103 - var_v0);
                    goto block_40;
                }
                if (temp_a2 != 0x82) {
                    var_a0 = arg4;
                    if (temp_a2 != 0x83) {
                        return var_s3;
                    }
                    var_f0_3 = 1.030993f;
                    var_f2_2 = 1.0f;
                    temp_v1_2 = 0x100 - (arg3 & 0xFF);
                    M2C_FIELD(temp_s1, s16 *, 6) = temp_v1_2;
                    M2C_FIELD(temp_s1, s16 *, 8) = temp_v1_2;
                    if (var_a0 & 0xFF) {
                        do {
                            if (var_a0 & 1) {
                                var_f2_2 *= var_f0_3;
                            }
                            var_a0 = (u32) (var_a0 & 0xFF) >> 1;
                            var_f0_3 *= var_f0_3;
                        } while (var_a0 != 0);
                    }
                    var_v0_2 = (s32) var_f2_2;
                    temp_s1[3] = (M2C_UNK *) -var_v0_2;
                    goto block_39;
                }
                var_a0_2 = arg4;
                var_f0_4 = 1.030993f;
                var_f2_3 = 1.0f;
                temp_v1_3 = 0x100 - (arg3 & 0xFF);
                M2C_FIELD(temp_s1, s16 *, 6) = temp_v1_3;
                M2C_FIELD(temp_s1, s16 *, 8) = temp_v1_3;
                if (var_a0_2 & 0xFF) {
                    do {
                        if (var_a0_2 & 1) {
                            var_f2_3 *= var_f0_4;
                        }
                        var_a0_2 = (u32) (var_a0_2 & 0xFF) >> 1;
                        var_f0_4 *= var_f0_4;
                    } while (var_a0_2 != 0);
                }
                var_v0_2 = (s32) var_f2_3;
                temp_s1[3] = (M2C_UNK *) var_v0_2;
block_39:
                temp_s1[4] = (M2C_UNK *) (var_v0_2 * 2);
                var_f0 = alCents2Ratio((s32) temp_s1[3], arg4, temp_a2);
                goto block_40;
            }
            var_a0_3 = arg4;
            var_f0_5 = 1.030993f;
            var_f2_4 = 1.0f;
            temp_v1_4 = 0x100 - (arg3 & 0xFF);
            M2C_FIELD(temp_s1, s16 *, 6) = temp_v1_4;
            M2C_FIELD(temp_s1, s16 *, 8) = temp_v1_4;
            M2C_FIELD(temp_s1, s8 *, 5) = 0;
            if (var_a0_3 & 0xFF) {
                do {
                    if (var_a0_3 & 1) {
                        var_f2_4 *= var_f0_5;
                    }
                    var_a0_3 = (u32) (var_a0_3 & 0xFF) >> 1;
                    var_f0_5 *= var_f0_5;
                } while (var_a0_3 != 0);
            }
            temp_f0 = (s32) var_f2_4;
            temp_s1[3] = M2C_BITWISE(M2C_UNK *, alCents2Ratio(-temp_f0, arg4, temp_a2));
            var_f0 = alCents2Ratio(temp_f0);
            temp_s1[4] = M2C_BITWISE(M2C_UNK *, var_f0);
            goto block_40;
        }
        M2C_FIELD(temp_s1, s16 *, 6) = (s16) (0x100 - (arg3 & 0xFF));
        temp_v0_3 = 0x7F - arg4;
        M2C_FIELD(temp_s1, s8 *, 0xC) = temp_v0_3;
        var_f0 = (f32) (temp_v0_3 & 0xFF);
        M2C_FIELD(temp_s1, s16 *, 8) = 0;
        M2C_FIELD(temp_s1, s8 *, 0xD) = (s8) arg4;
block_40:
        *arg1 = var_f0;
        goto block_41;
    }
block_41:
    return var_s3;
}

s32 func_8000EF58(void *arg0, f32 *arg1) {
    f32 var_f0;
    f32 var_f2;
    s32 var_f0_2;
    u16 temp_v0;
    u16 temp_v0_2;
    u16 temp_v0_3;
    u16 temp_v0_4;
    u16 temp_v0_5;
    u16 temp_v0_6;
    u8 temp_v1;

    temp_v1 = M2C_FIELD(arg0, u8 *, 4);
    if (temp_v1 != 4) {
        if ((s32) temp_v1 < 5) {
            if (temp_v1 != 2) {
                if ((s32) temp_v1 < 3) {
                    if (temp_v1 != 1) {
                        return 0x3E80;
                    }
                    temp_v0 = M2C_FIELD(arg0, u16 *, 8) + 1;
                    M2C_FIELD(arg0, u16 *, 8) = temp_v0;
                    if ((u32) (temp_v0 & 0xFFFF) >= (u16) M2C_FIELD(arg0, u16 *, 6)) {
                        M2C_FIELD(arg0, u16 *, 8) = 0U;
                    }
                    var_f0 = (f32) M2C_FIELD(arg0, u8 *, 0xD) + (func_8001C5A0((f32) ((f64) ((f32) M2C_FIELD(arg0, u16 *, 8) / (f32) M2C_FIELD(arg0, u16 *, 6)) * 6.2831853)) * (f32) M2C_FIELD(arg0, u8 *, 0xC));
                    goto block_40;
                }
                temp_v0_2 = M2C_FIELD(arg0, u16 *, 8) + 1;
                M2C_FIELD(arg0, u16 *, 8) = temp_v0_2;
                if ((u16) M2C_FIELD(arg0, u16 *, 6) < (u32) (temp_v0_2 & 0xFFFF)) {
                    M2C_FIELD(arg0, u16 *, 8) = 0U;
                }
                var_f0 = (f32) M2C_FIELD(arg0, u8 *, 0xC) - (((f32) M2C_FIELD(arg0, u16 *, 8) / (f32) M2C_FIELD(arg0, u16 *, 6)) * (f32) M2C_FIELD(arg0, u8 *, 0xD));
                goto block_40;
            }
            if (M2C_FIELD(arg0, u8 *, 5) == 0) {
                *arg1 = (f32) M2C_FIELD(arg0, u8 *, 0xE);
                M2C_FIELD(arg0, u8 *, 5) = 1U;
            } else {
                *arg1 = (f32) M2C_FIELD(arg0, u8 *, 0xD);
                M2C_FIELD(arg0, u8 *, 5) = 0U;
            }
            goto block_32;
        }
        if (temp_v1 != 0x81) {
            if ((s32) temp_v1 < 0x82) {
                if (temp_v1 != 0x80) {
                    return 0x3E80;
                }
                temp_v0_3 = M2C_FIELD(arg0, u16 *, 8) + 1;
                M2C_FIELD(arg0, u16 *, 8) = temp_v0_3;
                if ((u32) (temp_v0_3 & 0xFFFF) >= (u16) M2C_FIELD(arg0, u16 *, 6)) {
                    M2C_FIELD(arg0, u16 *, 8) = 0U;
                }
                var_f2 = func_8001C5A0((f32) ((f64) ((f32) M2C_FIELD(arg0, u16 *, 8) / (f32) M2C_FIELD(arg0, u16 *, 6)) * 6.2831853)) * M2C_BITWISE(f32, M2C_FIELD(arg0, u8 *, 0xC));
                goto block_39;
            }
            switch (temp_v1) {                      /* irregular */
            case 0x82:
                temp_v0_4 = M2C_FIELD(arg0, u16 *, 8) + 1;
                M2C_FIELD(arg0, u16 *, 8) = temp_v0_4;
                if ((u16) M2C_FIELD(arg0, u16 *, 6) < (u32) (temp_v0_4 & 0xFFFF)) {
                    M2C_FIELD(arg0, u16 *, 8) = 0U;
                }
                var_f2 = (f32) (s32) M2C_FIELD(arg0, u8 *, 0xC) - (((f32) M2C_FIELD(arg0, u16 *, 8) / (f32) M2C_FIELD(arg0, u16 *, 6)) * (f32) M2C_FIELD(arg0, s32 *, 0x10));
block_39:
                var_f0 = alCents2Ratio((s32) var_f2);
                goto block_40;
            case 0x83:
                temp_v0_5 = M2C_FIELD(arg0, u16 *, 8) + 1;
                M2C_FIELD(arg0, u16 *, 8) = temp_v0_5;
                if ((u16) M2C_FIELD(arg0, u16 *, 6) < (u32) (temp_v0_5 & 0xFFFF)) {
                    M2C_FIELD(arg0, u16 *, 8) = 0U;
                }
                var_f2 = (((f32) M2C_FIELD(arg0, u16 *, 8) / (f32) M2C_FIELD(arg0, u16 *, 6)) * (f32) M2C_FIELD(arg0, s32 *, 0x10)) + (f32) (s32) M2C_FIELD(arg0, u8 *, 0xC);
                goto block_39;
            default:
                return 0x3E80;
            }
        } else {
            if (M2C_FIELD(arg0, u8 *, 5) == 0) {
                var_f0_2 = (s32) M2C_FIELD(arg0, u8 *, 0xC);
                M2C_FIELD(arg0, u8 *, 5) = 1U;
            } else {
                var_f0_2 = M2C_FIELD(arg0, s32 *, 0x10);
                M2C_FIELD(arg0, u8 *, 5) = 0U;
            }
            *arg1 = M2C_BITWISE(f32, var_f0_2);
block_32:
            return 0x3E80 * M2C_FIELD(arg0, u16 *, 6);
        }
    } else {
        temp_v0_6 = M2C_FIELD(arg0, u16 *, 8) + 1;
        M2C_FIELD(arg0, u16 *, 8) = temp_v0_6;
        if ((u16) M2C_FIELD(arg0, u16 *, 6) < (u32) (temp_v0_6 & 0xFFFF)) {
            M2C_FIELD(arg0, u16 *, 8) = 0U;
        }
        var_f0 = (f32) M2C_FIELD(arg0, u8 *, 0xC) + (((f32) M2C_FIELD(arg0, u16 *, 8) / (f32) M2C_FIELD(arg0, u16 *, 6)) * (f32) M2C_FIELD(arg0, u8 *, 0xD));
block_40:
        *arg1 = var_f0;
        return 0x3E80;
    }
}

void func_8000F334(M2C_UNK *arg0) {
    M2C_UNK *temp_v0;

    temp_v0 = D_800A6194;
    D_800A6194 = arg0;
    *arg0 = temp_v0;
}

void func_8000F350(void) {
    D_800AF254 = 0;
    D_800A6840 = 1;
    func_8000F6EC();
    *D_800A6810 = 5;
    osCreateMesgQueue((s16 *(*)[]) D_800C30A8, &D_800B0494, (s16 *)1);
    osSetEventMesg(5, (s16 *(*)[]) D_800C30A8, D_800A6810);
    func_800222E0(D_800C30A8, &D_800B0430 + 3, &D_800A67E8);
}

void func_8000F3E0(void) {
    s16 *temp_v0;
    s16 *temp_v0_3;
    s16 temp_a1;
    s16 temp_v1_4;
    s16 var_a0_4;
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_v0_10;
    s32 temp_v0_2;
    s32 temp_v0_4;
    s32 temp_v0_6;
    s32 temp_v0_8;
    s32 temp_v1_3;
    s32 var_a0_2;
    s32 var_t1;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;
    s8 temp_v1;
    s8 temp_v1_2;
    s8 var_a0;
    s8 var_a0_3;
    struct _struct_D_800A67F8_0x6 *temp_t0;
    u16 *temp_a2;
    u16 *temp_a3;
    u16 *temp_v0_5;
    u16 *temp_v1_5;
    u16 *temp_v1_7;
    u16 temp_a0_3;
    u16 temp_a0_4;
    u16 temp_v0_7;
    u16 temp_v0_9;
    u16 temp_v1_6;

    func_80022048((struct _struct_D_800A67F8_0x6 (*)[]) D_800A67F8);
    var_t1 = 0;
    var_v0 = 0 & 0xFFFF;
    do {
        temp_t0 = &D_800A67F8[var_v0];
        temp_v0 = &D_800C30A0[var_v0];
        if (temp_t0->unk4 != 0) {
            temp_t0->unk2 = 0;
            temp_t0->unk3 = 0;
            temp_t0->unk0 = 0;
            *temp_v0 = 0;
        } else {
            *temp_v0 = 1;
        }
        var_a0 = temp_t0->unk2;
        temp_v1 = var_a0;
        if (var_a0 < 0) {
            var_a0 = -var_a0;
        }
        temp_a0 = var_a0 - 7;
        var_a0_2 = temp_a0 & ((s32) ~temp_a0 >> 0x1F);
        if (var_a0_2 >= 0x3D) {
            var_a0_2 = 0x3C;
        }
        if (temp_v1 < 0) {
            var_a0_2 = -var_a0_2;
        }
        temp_v0_2 = var_t1 & 0xFFFF;
        D_800A67E0 = var_a0_2;
        D_800B04AC[temp_v0_2] = (s16) var_a0_2;
        var_a0_3 = temp_t0->unk3;
        temp_v1_2 = var_a0_3;
        if (var_a0_3 < 0) {
            var_a0_3 = -var_a0_3;
        }
        temp_a0_2 = var_a0_3 - 7;
        var_a0_4 = temp_a0_2 & ((s32) ~temp_a0_2 >> 0x1F);
        if (var_a0_4 >= 0x3D) {
            var_a0_4 = 0x3C;
        }
        if (temp_v1_2 < 0) {
            var_a0_4 = -var_a0_4;
        }
        temp_v0_3 = &D_800C5298[temp_v0_2];
        *temp_v0_3 = var_a0_4;
        temp_a1 = *temp_v0_3;
        D_800A67E4 = (s32) var_a0_4;
        temp_v1_3 = temp_t0->unk0 & 0xF0FF;
        D_800A6844 = temp_v1_3;
        if (temp_a1 >= 0x1F) {
            var_v0_2 = temp_v1_3 | 0x800;
            goto block_19;
        }
        var_v0_2 = temp_v1_3 | 0x400;
        if (temp_a1 < -0x1E) {
block_19:
            D_800A6844 = var_v0_2;
        }
        temp_v1_4 = D_800B04AC[var_t1 & 0xFFFF];
        if (temp_v1_4 < 0x1F) {
            if (temp_v1_4 < -0x1E) {
                var_v0_3 = D_800A6844 | 0x200;
                goto block_26;
            }
        } else {
            var_v0_3 = D_800A6844 | 0x100;
block_26:
            D_800A6844 = var_v0_3;
        }
        temp_v0_4 = var_t1 & 0xFFFF;
        temp_a0_3 = (D_800A6838[temp_v0_4] ^ D_800A6844) & D_800A6844;
        (&D_800C5334)[temp_v0_4] = (u16) D_800A6844;
        (&D_800B0596)[temp_v0_4] = temp_a0_3;
        temp_v0_5 = &(&D_800B043A)[temp_v0_4];
        if (temp_a0_3 & 0xFFFF) {
            *temp_v0_5 = temp_a0_3;
            D_800B04A4[temp_v0_4] = 8;
        } else {
            *temp_v0_5 = temp_a0_3;
        }
        temp_v0_6 = var_t1 & 0xFFFF;
        temp_v1_5 = &D_800B04A4[temp_v0_6];
        temp_v0_7 = *temp_v1_5;
        if (temp_v0_7 != 0) {
            *temp_v1_5 = temp_v0_7 - 1;
        } else {
            *temp_v1_5 = 1;
            (&D_800B043A)[temp_v0_6] = (&D_800C5334)[temp_v0_6];
        }
        temp_v0_8 = var_t1 & 0xFFFF;
        D_800A6838[temp_v0_8] = (u16) D_800A6844;
        temp_v1_6 = temp_t0->unk0;
        temp_a0_4 = temp_v1_6 & (temp_v1_6 ^ D_800A6830[temp_v0_8]);
        temp_a3 = &(&D_800B058C)[temp_v0_8];
        temp_a2 = &(&D_800AF238)[temp_v0_8];
        *temp_a3 = temp_v1_6;
        (&D_800AF222)[temp_v0_8] = temp_a0_4;
        *temp_a2 = temp_a0_4;
        if (temp_a0_4 != 0) {
            D_800C167A[temp_v0_8] = 8;
        }
        temp_v1_7 = &D_800C167A[temp_v0_8];
        temp_v0_9 = *temp_v1_7;
        if (temp_v0_9 != 0) {
            *temp_v1_7 = temp_v0_9 - 1;
        } else {
            *temp_v1_7 = 1;
            *temp_a2 = *temp_a3;
        }
        temp_v0_10 = var_t1 & 0xFFFF;
        var_t1 += 1;
        D_800A6830[temp_v0_10] = temp_t0->unk0;
        var_v0 = var_t1 & 0xFFFF;
    } while ((u32) (var_t1 & 0xFFFF) < 4U);
}

void func_8000F6EC(void) {
    s32 temp_v1;
    s32 var_a0;

    var_a0 = 0;
    do {
        temp_v1 = var_a0 & 0xFFFF;
        var_a0 += 1;
        D_800C5304[temp_v1] = 0;
        D_800B0610[temp_v1] = 0;
        D_800B4A38[temp_v1] = 0;
        D_800C30D4[temp_v1] = 0;
    } while ((u32) (var_a0 & 0xFFFF) < 4U);
}

u16 func_8000F750(void) {
    return D_800A6840;
}

void func_8000F75C(void) {
    D_800A6840 = 1;
    if (D_800C51D0 == 2) {
        osSendMesg(D_800B0520, &D_800C5328, 1);
    }
}

void func_8000F7A4(void) {
    s32 temp_v1;
    s32 var_a0;

    D_800A6840 = 0;
    if (D_800B4AA4 != 0) {
        do {

        } while (D_800B4AA4 != 0);
    }
    var_a0 = 0;
    do {
        temp_v1 = var_a0 & 0xFFFF;
        var_a0 += 1;
        D_800C5304[temp_v1] = 0;
        D_800B0610[temp_v1] = 0;
        D_800B4A38[temp_v1] = 0;
        D_800C30D4[temp_v1] = 0;
    } while ((u32) (var_a0 & 0xFFFF) < 4U);
    if (D_800B04D0 != 0) {
        func_8000F3E0(var_a0, D_800C30D4, D_800B4A38, D_800B0610);
        D_800B04D0 = 0;
    }
    if (D_800C51D0 == 1) {
        osSendMesg(D_800C30A8, (s16 **) D_800A6810, 0);
    }
}

void func_8000F890(void) {
    s32 *var_a0;
    s32 *var_a1;
    s32 *var_a2;
    s32 *var_v1;
    s32 var_t0;

    var_t0 = 0;
    var_a2 = &D_800B056C;
    var_a1 = &D_800C51C4;
    var_a0 = &D_800AF264;
    var_v1 = &D_800B0428;
    do {
        *var_v1 = 1;
        *var_a0 = 1;
        *var_a1 = 1;
        *var_a2 = 1;
        var_a2 += 4;
        var_a1 += 4;
        var_a0 += 4;
        var_t0 += 1;
        var_v1 += 4;
    } while (var_t0 < 2);
    D_800C1678 = 0xFFFF;
}

void func_8000F8F4(void) {
    s32 temp_a0;
    s32 temp_v1;
    u16 temp_a1;
    u16 temp_a1_2;
    u16 temp_a2;
    u16 temp_a2_2;
    u16 temp_t0;
    u16 temp_v1_3;
    u32 temp_v1_2;

    temp_a1 = D_800B0498;
    temp_t0 = D_800B049A;
    temp_v1 = ~*(u16 *)0x02000002;
    temp_a2 = temp_v1 & 0xFF;
    temp_v1_2 = (u32) (temp_v1 & 0xFFFF) >> 8;
    temp_a0 = ~*(u16 *)0x02000004;
    D_800C52A8 = temp_a1;
    D_800C52AA = temp_t0;
    D_800B0498 = temp_a2;
    temp_a2_2 = temp_a2 | ((temp_a0 & 1) << 8);
    D_800B049A = (u16) temp_v1_2;
    temp_v1_3 = temp_v1_2 | ((temp_a0 & 2) << 7);
    temp_a1_2 = temp_a2_2 & (temp_a2_2 ^ temp_a1);
    D_800B0498 = temp_a2_2;
    D_800B049A = temp_v1_3;
    D_800C532C = temp_a1_2;
    D_800C532E = temp_v1_3 & (temp_v1_3 ^ temp_t0);
    D_800B4A80 = temp_a1_2;
    if (temp_a1_2 != 0) {
        M2C_FIELD(&D_800C52F0, u16 *, 0) = 8U;
    } else if (M2C_FIELD(&D_800C52F0, u16 *, 0) != 0) {
        M2C_FIELD(&D_800C52F0, u16 *, 0) = (u16) (M2C_FIELD(&D_800C52F0, u16 *, 0) - 1);
    } else {
        M2C_FIELD(&D_800C52F0, u16 *, 0) = 1U;
        D_800B4A80 = temp_a2_2;
    }
    D_800B4A82 = D_800C532E;
    if (D_800C532E != 0) {
        M2C_FIELD(&D_800C52F0, u16 *, 2) = 8U;
        return;
    }
    if (M2C_FIELD(&D_800C52F0, u16 *, 2) != 0) {
        M2C_FIELD(&D_800C52F0, u16 *, 2) = (u16) (M2C_FIELD(&D_800C52F0, u16 *, 2) - 1);
        return;
    }
    M2C_FIELD(&D_800C52F0, u16 *, 2) = 1U;
    D_800B4A82 = D_800B049A;
}

void func_8000FA34(void) {
    D_800C52A0 = ~*(u16 *)0x02000000;
}

void func_8000FA4C(void) {
    s16 *var_v0;
    s32 *temp_v1_7;
    s32 *temp_v1_8;
    s32 *var_a0;
    s32 temp_a1;
    s32 temp_v0_5;
    s32 temp_v1_11;
    s32 temp_v1_12;
    s32 var_a0_2;
    s32 var_a0_3;
    s32 var_a0_4;
    s32 var_a2;
    s32 var_t0;
    s32 var_v0_2;
    u16 *temp_a0_2;
    u16 *temp_v1_10;
    u16 *temp_v1_6;
    u16 *var_t1;
    u16 *var_t3;
    u16 *var_t5;
    u16 temp_a0;
    u16 temp_v0;
    u16 temp_v0_3;
    u16 temp_v0_4;
    u16 temp_v1_2;
    u16 temp_v1_3;
    u16 temp_v1_4;
    u16 var_v0_3;
    u32 *var_a1;
    u32 *var_a3;
    u32 *var_t2;
    u32 temp_v0_2;
    u32 temp_v1;
    u32 temp_v1_5;
    u32 temp_v1_9;

    var_t0 = 0;
    var_a3 = &D_800C52F4;
    var_a2 = 0;
    var_t2 = &D_800B04F8;
    var_t5 = &D_800C53D8;
    var_t3 = &D_800C16E2;
    var_t1 = &D_800C0D4C;
    temp_v1 = ~*(u16 *)0x02000004 & 0xFFFF;
    temp_a0 = D_800C30C4;
    temp_v0 = (temp_v1 >> 5) & 1;
    temp_v1_2 = (temp_v1 >> 4) & 1;
    D_800B0438 = D_800C0AB0;
    temp_a1 = temp_v1_2 ^ D_800C0AB0;
    D_800C0AB0 = temp_v1_2;
    D_800C30C4 = temp_v0;
    *D_800C1898 = temp_v1_2 & temp_a1;
    *D_800C31B0 = temp_a0;
    D_800AF220 = temp_v0 & (temp_v0 ^ temp_a0);
    do {
        *var_t3 = *var_t1;
        if (var_a3 != &D_800C52F4) {
            *var_t1 = (temp_v1 >> 3) & 1;
        } else {
            D_800C0D4C = (temp_v1 >> 2) & 1;
        }
        temp_v1_3 = *var_t1;
        temp_v1_4 = temp_v1_3 & (temp_v1_3 ^ *var_t3);
        *var_t5 = temp_v1_4;
        if (temp_v1_4 != 0) {
            *var_t2 = 1;
        }
        if (*var_t3 != 0) {
            if (*var_t1 != 0) {
                temp_v0_2 = *var_t2 + 1;
                *var_t2 = temp_v0_2;
                var_v0 = var_t0 + &D_800B0574;
                if (temp_v0_2 >= 0x1EU) {
                    goto block_13;
                }
            } else {
                temp_v1_5 = *var_t2;
                if (temp_v1_5 >= 2U) {
                    temp_v1_6 = var_t0 + &D_800C1654;
                    if (temp_v1_5 >= 0x1EU) {
                        var_v0 = var_t0 + &D_800B0574;
block_13:
                        *var_v0 = 1;
                    } else {
                        *temp_v1_6 += 1;
                        temp_v1_7 = var_a2 + &D_800C16B8;
                        *temp_v1_7 += 1;
                        if (D_800C16E0 == 0) {
                            temp_v1_8 = var_a2 + &D_800C52F4;
                            *temp_v1_8 += 1;
                        } else {
                            D_800C52F4 += 1;
                        }
                    }
                }
            }
        }
        temp_v1_9 = *var_a3;
        if (temp_v1_9 != 0) {
            if (*(&D_800B4A30 + var_a2) != 0) {
                var_a1 = var_a2 + &D_800AF264;
                var_a0 = var_a2 + &D_800C30C8;
                if (temp_v1_9 >= (u32) *var_a1) {
                    var_v0_2 = *var_a0 + *(&D_800B0428 + var_a2);
                    goto block_23;
                }
            } else {
                var_a1 = var_a2 + &D_800B056C;
                var_a0 = var_a2 + &D_800C30C8;
                if (temp_v1_9 >= (u32) *var_a1) {
                    var_v0_2 = *var_a0 + *(&D_800C51C4 + var_a2);
block_23:
                    *var_a0 = var_v0_2;
                    *var_a3 -= *var_a1;
                }
            }
        }
        temp_a0_2 = var_t0 + &D_800B0274;
        temp_v0_3 = *temp_a0_2;
        var_v0_3 = temp_v0_3 - 1;
        if (temp_v0_3 == 0) {
            temp_v1_10 = var_t0 + &D_800C1654;
            temp_v0_4 = *temp_v1_10;
            if (temp_v0_4 != 0) {
                *temp_v1_10 = temp_v0_4 - 1;
                var_v0_3 = 0x3F;
                goto block_27;
            }
        } else {
block_27:
            *temp_a0_2 = var_v0_3;
        }
        var_t0 += 2;
        var_a3 += 4;
        var_a2 += 4;
        var_t2 += 4;
        var_t5 += 2;
        var_t3 += 2;
        var_t1 += 2;
    } while ((s32) var_a3 < (s32) (&D_800C52F4 + 8));
    temp_v0_5 = ((u16) D_800B0274 >> 5) & 1;
    if (D_800B0276 & 0x20) {
        var_a0_2 = temp_v0_5 | 2;
    } else {
        var_a0_2 = temp_v0_5;
    }
    temp_v1_11 = var_a0_2 & 0xFFFF;
    if (D_800B0574 == 0) {
        var_a0_3 = temp_v1_11 | 4;
    } else {
        var_a0_3 = temp_v1_11;
    }
    temp_v1_12 = var_a0_3 & 0xFFFF;
    if (D_800B0576 == 0) {
        var_a0_4 = temp_v1_12 | 8;
    } else {
        var_a0_4 = temp_v1_12;
    }
    *(s16 *)0x0200000A = D_800C178C | (var_a0_4 & D_800C1678);
}

void func_8000FDB0(void) {
    M2C_UNK *var_a3;
    s16 *var_a0;
    s16 *var_a0_2;
    s32 *var_a1;
    s32 var_a0_3;
    s32 var_v1;
    s32 var_v1_2;
    struct _struct_osViModeTable_0x50 *var_a2;

    var_a0 = &D_807B2600;
    var_v1 = 0;
    do {
        *var_a0 = 0;
        var_v1 += 1;
        var_a0 += 2;
    } while (var_v1 <= 0x12BFF);
    var_a0_2 = &D_807D7E00;
    var_v1_2 = 0;
    do {
        *var_a0_2 = 0;
        var_v1_2 += 1;
        var_a0_2 += 2;
    } while (var_v1_2 <= 0x12BFF);
    var_a1 = &D_8013B510;
    var_a0_3 = 0x7000;
    D_800B05A0 = 1;
    *D_800B28A8 = &D_807B2600;
    D_800B0504 = 0;
    *D_800B2888 = 2;
    *D_800B4A08 = 2;
    D_800B4A28 = &D_807D7E00;
    do {
        *var_a1 = 0;
        var_a0_3 -= 1;
        var_a1 += 4;
    } while (var_a0_3 != 0);
    var_a3 = &D_800A68A8;
    var_a2 = osViModeTable;
    D_8013A800 = 0;
    D_8013A804 = 0x1C000;
    gScreenWidth = 0x140;
    D_800C5330 = 0xF0;
    D_800C1788 = 0xA;
    D_800C53F8 = 0;
    D_800C1650 = 0;
    D_800B0594 = 0;
    D_800B0618 = 0;
    D_800B0724 = 0;
    D_800C53D0 = 0xBB8;
    do {
        M2C_FIELD(var_a3, s32 *, 0) = (s32) var_a2->unk0;
        M2C_FIELD(var_a3, s32 *, 4) = (s32) var_a2->unk4;
        M2C_FIELD(var_a3, s32 *, 8) = (s32) var_a2->unk8;
        M2C_FIELD(var_a3, s32 *, 0xC) = (s32) var_a2->unkC;
        var_a2 += 0x10;
        var_a3 += 0x10;
    } while (var_a2 != &osViModeTable[1]);
    D_800A68A0 = osViModeTable;
    bcopy(D_8003B110, &D_800A6850, 0x50, var_a3);
    func_80010BF4();
    osViBlack(1);
}

void func_8000FF64(s16 **arg0) {
    f32 sp20;
    f32 temp_f0;
    f32 temp_f6;
    s32 *temp_a0;
    s32 *temp_a0_4;
    s32 *temp_a0_5;
    s32 *temp_a0_6;
    s32 *temp_a0_7;
    s32 *temp_a1;
    s32 *temp_a1_2;
    s32 *temp_fp;
    s32 *temp_s0;
    s32 *temp_s1;
    s32 *temp_v0_2;
    s32 *temp_v0_3;
    s32 *temp_v0_4;
    s32 *temp_v0_5;
    s32 *temp_v0_6;
    s32 *temp_v1;
    s32 *temp_v1_2;
    s32 temp_a0_2;
    s32 temp_a0_3;
    s32 temp_s5;
    void *temp_s2;
    void *temp_s4;
    void *temp_s6;
    void *temp_v0;

    if (D_800C532E & 0x100) {
        D_800B0540 ^= 1;
    }
    temp_fp = arg0 + 0x158;
    D_800B053C = temp_fp;
    func_8001BE98(0xC1);
    func_8001BE98(0x83);
    temp_s6 = arg0 + 0x58;
    func_8000B06C();
    func_8001BE98(0xC3);
    func_8001BE98(0x81);
    func_80019040();
    temp_a1 = D_800B053C;
    D_800B053C = temp_a1 + 8;
    D_800B053C = temp_a1 + 0x10;
    M2C_FIELD(temp_a1, s32 *, 0) = 0xBC000006;
    M2C_FIELD(temp_a1, s32 *, 4) = 0;
    M2C_FIELD(temp_a1, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_a1, M2C_UNK **, 0xC) = &D_800386A0;
    if (D_800386C8 != 0) {
        D_800B053C = temp_a1 + 0x18;
        M2C_FIELD(temp_a1, s32 *, 0x10) = 0x06000000;
        M2C_FIELD(temp_a1, M2C_UNK **, 0x14) = &D_80038560;
        D_800386C8 = 0;
    }
    temp_v1 = D_800B053C;
    temp_a0 = temp_v1 + 8;
    D_800B053C = temp_a0;
    M2C_FIELD(temp_v1, s32 *, 0) = 0x06000000;
    M2C_FIELD(temp_v1, M2C_UNK **, 4) = &D_80038648;
    temp_v0 = func_800107D0(temp_a0, temp_a1, 0x06000000);
    D_800B053C = temp_v0 + 8;
    M2C_FIELD(temp_v0, s32 *, 0) = 0xFE000000;
    temp_s0 = D_800B053C;
    M2C_FIELD(temp_v0, u32 *, 4) = osVirtualToPhysical(&D_80115000);
    D_800B053C = temp_s0 + 8;
    D_800B053C = temp_s0 + 0x10;
    M2C_FIELD(temp_s0, s32 *, 0xC) = 0x300000;
    M2C_FIELD(temp_s0, s32 *, 0) = 0xE7000000;
    M2C_FIELD(temp_s0, s32 *, 4) = 0;
    M2C_FIELD(temp_s0, s32 *, 8) = 0xBA001402;
    D_800B053C = temp_s0 + 0x18;
    M2C_FIELD(temp_s0, s32 *, 0x10) = (s32) (((gScreenWidth - 1) & 0xFFF) | 0xFF100000);
    temp_s1 = D_800B053C;
    M2C_FIELD(temp_s0, u32 *, 0x14) = osVirtualToPhysical(&D_80115000);
    temp_a0_2 = gScreenWidth - 1;
    M2C_FIELD(temp_s1, s32 *, 4) = 0xFFFCFFFC;
    M2C_FIELD(temp_s1, s32 *, 0) = 0xF7000000;
    M2C_FIELD(temp_s1, s32 *, 8) = (s32) (((temp_a0_2 & 0x3FF) << 0xE) | ((((D_800C5330 - 1) & 0x3FF) * 4) | 0xF6000000));
    M2C_FIELD(temp_s1, s32 *, 0xC) = 0;
    M2C_FIELD(temp_s1, s32 *, 0x10) = (s32) ((temp_a0_2 & 0xFFF) | 0xFF100000);
    D_800B053C = temp_s1 + 8;
    D_800B053C = temp_s1 + 0x10;
    D_800B053C = temp_s1 + 0x18;
    M2C_FIELD(temp_s1, u32 *, 0x14) = osVirtualToPhysical(M2C_FIELD(arg0, u8 **, 0x2178));
    if (D_800B05A0 != 0) {
        temp_a1_2 = D_800B053C;
        D_800B053C = temp_a1_2 + 8;
        M2C_FIELD(temp_a1_2, s32 *, 0) = 0xF7000000;
        M2C_FIELD(temp_a1_2, s32 *, 0xC) = 0;
        temp_a0_3 = ((D_800B0724 << 8) & 0xF800) | ((D_800B0618 * 8) & 0x7C0) | (((u8) D_800B0594 >> 2) & 0x3E);
        M2C_FIELD(temp_a1_2, s32 *, 4) = (s32) (((temp_a0_3 | 1) << 0x10) | 1 | temp_a0_3);
        D_800B053C = temp_a1_2 + 0x10;
        M2C_FIELD(temp_a1_2, s32 *, 8) = (s32) ((((gScreenWidth - 1) & 0x3FF) << 0xE) | ((((D_800C5330 - 1) & 0x3FF) * 4) | 0xF6000000));
    }
    temp_f6 = (f32) gScreenWidth;
    temp_f0 = (f32) D_800C5330;
    temp_v1_2 = D_800B053C;
    D_800B053C = temp_v1_2 + 8;
    M2C_FIELD(temp_v1_2, s32 *, 0) = 0xE7000000;
    M2C_FIELD(temp_v1_2, s32 *, 4) = 0;
    D_800B053C = temp_v1_2 + 0x10;
    M2C_FIELD(temp_v1_2, s32 *, 8) = 0xBA001402;
    M2C_FIELD(temp_v1_2, s32 *, 0xC) = 0;
    func_80006F74(&sp20, (f32) ((f64) -temp_f6 / 2.0), (f32) ((f64) temp_f6 / 2.0), (f32) ((f64) -temp_f0 / 2.0), (f32) ((f64) temp_f0 / 2.0), -1.0f, 3000.0f, 1.0f);
    temp_s2 = arg0 + 0x98;
    func_8000A924(temp_s2, &sp20);
    temp_s5 = func_80009AD4(D_800B05B8, D_800C51D4, (f32) gScreenWidth / (f32) D_800C5330, (f32) D_800C1788, (f32) D_800C53D0, 1.0f);
    func_8000A924(temp_s6, D_800B05B8);
    temp_s4 = arg0 + 0x118;
    func_800095B0(D_800C16E8, temp_s4, &D_800B0444, &D_800B04B8, D_800C1664, D_800C1668, D_800C166C);
    if (D_800B0548 != 0) {
        Mtx_Identity(D_800C16E8);
    }
    Mtx_Copy((f32 *) (D_800B04A0 + 0xC), D_800C16E8);
    func_80012320(&D_800C1730, D_800C16E8);
    Mtx_Identity(&sp20);
    func_80011868(D_800C316C, &sp20);
    func_80011868(D_800C30C0, &sp20);
    func_80011868(D_800C0AB4, &sp20);
    func_80011868(D_800AF234, &sp20);
    temp_v0_2 = D_800B053C;
    temp_a0_4 = temp_v0_2 + 0x38;
    D_800B053C = temp_v0_2 + 8;
    D_800B053C = temp_v0_2 + 0x10;
    M2C_FIELD(temp_v0_2, void **, 0xC) = (void *) (arg0 + 0x128);
    D_800B053C = temp_v0_2 + 0x18;
    M2C_FIELD(temp_v0_2, s32 *, 0x14) = (s32) (temp_s5 & 0xFFFF);
    D_800B053C = temp_v0_2 + 0x20;
    D_800B053C = temp_v0_2 + 0x28;
    M2C_FIELD(temp_v0_2, void **, 0x24) = (void *) (&D_800C1730 + 0x48);
    M2C_FIELD(temp_v0_2, s32 *, 0) = 0x03840010;
    M2C_FIELD(temp_v0_2, void **, 4) = temp_s4;
    M2C_FIELD(temp_v0_2, s32 *, 8) = 0x03820010;
    M2C_FIELD(temp_v0_2, s32 *, 0x10) = 0xBC00000E;
    M2C_FIELD(temp_v0_2, s32 *, 0x18) = 0xBC000002;
    M2C_FIELD(temp_v0_2, s32 *, 0x1C) = 0x80000040;
    M2C_FIELD(temp_v0_2, s32 *, 0x20) = 0x03860010;
    D_800B053C = temp_v0_2 + 0x30;
    M2C_FIELD(temp_v0_2, s32 *, 0x28) = 0x03880010;
    M2C_FIELD(temp_v0_2, void **, 0x2C) = (void *) (&D_800C1730 + 0x40);
    D_800B053C = temp_a0_4;
    M2C_FIELD(temp_v0_2, s32 *, 0x30) = 0x01030040;
    M2C_FIELD(temp_v0_2, void **, 0x34) = temp_s2;
    temp_v0_3 = func_80011C88(temp_a0_4, D_800C316C, 0x03840010, 0x03820010);
    temp_a0_5 = temp_v0_3 + 8;
    D_800B053C = temp_a0_5;
    M2C_FIELD(temp_v0_3, s32 *, 0) = 0x01030040;
    M2C_FIELD(temp_v0_3, void **, 4) = temp_s6;
    temp_v0_4 = func_80011C88(temp_a0_5, D_800C30C0);
    temp_a0_6 = temp_v0_4 + 8;
    D_800B053C = temp_a0_6;
    M2C_FIELD(temp_v0_4, s32 *, 0) = 0x01030040;
    M2C_FIELD(temp_v0_4, void **, 4) = temp_s2;
    temp_v0_5 = func_80011C88(temp_a0_6, D_800C0AB4);
    temp_a0_7 = temp_v0_5 + 8;
    D_800B053C = temp_a0_7;
    M2C_FIELD(temp_v0_5, s32 *, 0) = 0x01030040;
    M2C_FIELD(temp_v0_5, void **, 4) = temp_s2;
    temp_v0_6 = func_80011C88(temp_a0_7, D_800AF234);
    D_800B053C = temp_v0_6 + 8;
    M2C_FIELD(temp_v0_6, s32 *, 0) = 0xE9000000;
    D_800B053C = temp_v0_6 + 0x10;
    M2C_FIELD(temp_v0_6, s32 *, 4) = 0;
    M2C_FIELD(temp_v0_6, s32 *, 8) = 0xB8000000;
    M2C_FIELD(temp_v0_6, s32 *, 0xC) = 0;
    arg0[0x11] = ((s32) ((temp_v0_6 - 0xF0) - temp_s6) >> 3) * 8;
    arg0[0x10] = temp_fp;
    arg0[4] = (s16 *)1;
    if (D_800B0504 != 1) {
        arg0[6] = D_80033FA0;
        arg0[7] = D_80034070 - D_80033FA0;
    }
    arg0[0x13] = (s16 *)0x900;
    arg0[5] = NULL;
    switch (D_800B0504) {                           /* irregular */
    case 0:
        arg0[0x13] = (s16 *)0xFC0;
        arg0[8] = D_80037210;
        arg0[0xA] = &D_8003CA50;
block_21:
        arg0[0xE] = &D_8013B510;
        arg0[0xF] = &D_80173510;
block_22:
        arg0[2] = (s16 *)0x63;
        break;
    case 2:
        arg0[8] = D_80034E90;
        arg0[0xA] = &D_8003BA50;
        goto block_22;
    case 3:
        arg0[8] = D_80035E10;
        arg0[0xA] = &D_8003C250;
        goto block_21;
    }
    arg0[9] = (s16 *)0x1000;
    arg0[0xB] = (s16 *)0x800;
    arg0[0xC] = &D_8013B110;
    arg0[0xD] = (s16 *)0x400;
    arg0[0x12] = &D_8013A810;
    arg0[0x14] = D_800C30E0;
    M2C_FIELD(arg0, s16 **, 0) = NULL;
    arg0[0x15] = arg0 + 0x2158;
    arg0[3] = M2C_FIELD(arg0, u8 **, 0x2178);
    func_8001C990();
    osSendMesg(D_800B0568, arg0, 1);
    D_800C1650 += 1;
}

void *func_800107D0(s32 *arg0) {
    s16 var_v0;
    void *temp_t0;
    void *temp_t0_2;

    D_800A68F8 = gScreenWidth * 2;
    if ((u32) D_800B0504 < 2U) {
        var_v0 = D_800C5330 * -2;
    } else {
        var_v0 = D_800C5330 * 2;
    }
    D_800A68FA = var_v0;
    temp_t0_2 = arg0 + 8;
    temp_t0 = temp_t0_2 + 8;
    D_800A68FC = 0x1FF;
    D_800A6900 = gScreenWidth * 2;
    D_800A6902 = D_800C5330 * 2;
    D_800A68FE = 0;
    D_800A6904 = 0x1FF;
    D_800A6906 = 0;
    M2C_FIELD(arg0, s32 *, 0) = 0x03800010;
    M2C_FIELD(arg0, s16 **, 4) = (s16 *) (&D_800A68FC - 4);
    M2C_FIELD(arg0, s32 *, 8) = 0xE7000000;
    M2C_FIELD(temp_t0_2, s32 *, 4) = 0;
    M2C_FIELD(temp_t0_2, s32 *, 8) = 0xED000000;
    D_800A6908 = 0;
    M2C_FIELD(temp_t0, s32 *, 4) = (s32) ((((s32) ((f32) (gScreenWidth - 1) * 4.0f) & 0xFFF) << 0xC) | ((s32) ((f32) (D_800C5330 - 1) * 4.0f) & 0xFFF));
    return temp_t0 + 8;
}

s32 *func_80010900(s32 *arg0, s16 arg1, s16 arg2, s16 arg3, u16 arg4, s32 arg5, s32 arg6) {
    f64 var_f2;
    s32 *temp_t4;
    s32 *var_t7;
    s32 temp_a0;
    s32 temp_a1;
    s32 var_v0;
    void *temp_t3;
    void *temp_t6;
    void *temp_t7;
    void *temp_t7_2;

    var_t7 = arg0;
    if (D_800A6908 < 8) {
        temp_a0 = D_800A6908 * 0x10;
        *(&D_800A6910 + temp_a0) = (s16) (s32) ((f64) (gScreenWidth * 2) * ((f64) (f32) (s16) arg5 / 8192.0));
        if ((u32) D_800B0504 < 2U) {
            var_f2 = (f64) (f32) (s16) arg6 / 8192.0;
            var_v0 = D_800C5330 * -2;
        } else {
            var_f2 = (f64) (f32) (s16) arg6 / 8192.0;
            var_v0 = D_800C5330 * 2;
        }
        *(&D_800A6912 + temp_a0) = (s16) (s32) ((f64) var_v0 * var_f2);
        temp_t4 = var_t7;
        temp_t7 = var_t7 + 8;
        temp_t3 = temp_t7;
        temp_t7_2 = temp_t7 + 8;
        temp_t6 = temp_t7_2;
        var_t7 = temp_t7_2 + 8;
        temp_a1 = D_800A6908 * 0x10;
        *(&D_800A6918 + temp_a1) = (gScreenWidth - ((((s32) gScreenWidth / 2) - (arg1 + ((s32) (arg3 + ((u32) (arg3 << 0x10) >> 0x1F)) >> 1))) * 2)) * 2;
        *(&D_800A6914 + temp_a1) = 0x1FF;
        *(&D_800A6916 + temp_a1) = 0;
        *(&D_800A691A + temp_a1) = (D_800C5330 - ((((s32) D_800C5330 / 2) - (arg2 + ((s32) ((s16) arg4 + ((u32) (arg4 << 0x10) >> 0x1F)) >> 1))) * 2)) * 2;
        *(&D_800A691C + temp_a1) = 0x1FF;
        D_800A691E[D_800A6908].unk0 = 0;
        M2C_FIELD(temp_t4, s32 *, 0) = 0x03800010;
        D_800A6908 += 1;
        M2C_FIELD(temp_t4, void **, 4) = (void *) (temp_a1 + &D_800A6910);
        M2C_FIELD(temp_t3, s32 *, 0) = 0xE7000000;
        M2C_FIELD(temp_t3, s32 *, 4) = 0;
        M2C_FIELD(temp_t6, s32 *, 0) = (s32) ((((s32) ((f32) arg1 * 4.0f) & 0xFFF) << 0xC) | (((s32) ((f32) arg2 * 4.0f) & 0xFFF) | 0xED000000));
        M2C_FIELD(temp_t6, s32 *, 4) = (s32) ((((s32) ((f32) ((arg1 + arg3) - 1) * 4.0f) & 0xFFF) << 0xC) | ((s32) ((f32) ((arg2 + (s16) arg4) - 1) * 4.0f) & 0xFFF));
    }
    return var_t7;
}

void func_80010BF4(void) {
    M2C_UNK *var_a2;
    M2C_UNK *var_a2_2;
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v1;
    struct _struct_osViModeTable_0x50 *temp_a3;
    struct _struct_osViModeTable_0x50 *var_a3;
    struct _struct_osViModeTable_0x50 *var_s0;

    temp_s0 = (gScreenWidth << 0xA) / 640;
    temp_v0 = osSetIntMask(0x80401, MULT_HI((gScreenWidth << 0xA), 0x66666667));
    D_800A6870 = temp_s0;
    *D_800A6858 = gScreenWidth;
    temp_v1 = gScreenWidth * 2;
    *D_800A6878 = temp_v1;
    *D_800A688C = temp_v1;
    osSetIntMask(temp_v0);
    var_a3 = D_800A68A0;
    var_a2 = &D_800A68A8;
    do {
        temp_a1 = M2C_FIELD(var_a2, s32 *, 0xC);
        var_a3->unk0 = M2C_FIELD(var_a2, s32 *, 0);
        var_a3->unk4 = M2C_FIELD(var_a2, s32 *, 4);
        var_a3->unk8 = M2C_FIELD(var_a2, s32 *, 8);
        var_a3->unkC = temp_a1;
        var_a2 += 0x10;
        var_a3 += 0x10;
    } while (var_a2 != (&D_800A68A8 + 0x50));
    var_s0 = &D_800A6850;
    osViSetMode(&D_800A6850, temp_a1, var_a2, var_a3);
    var_a2_2 = &D_800A68A8;
    temp_a3 = &D_800A6850 + 0x50;
    do {
        temp_a0 = var_s0->unk8;
        temp_a1_2 = var_s0->unkC;
        M2C_FIELD(var_a2_2, s32 *, 0) = (s32) var_s0->unk0;
        M2C_FIELD(var_a2_2, s32 *, 4) = (s32) var_s0->unk4;
        M2C_FIELD(var_a2_2, s32 *, 8) = temp_a0;
        M2C_FIELD(var_a2_2, s32 *, 0xC) = temp_a1_2;
        var_s0 += 0x10;
        var_a2_2 += 0x10;
    } while (var_s0 != temp_a3);
    D_800A68A0 = &D_800A6850;
    func_80010D28(temp_a0, temp_a1_2, var_a2_2, temp_a3);
    func_80010DB0();
    func_80021990(0xA2);
}

void func_80010D28(void) {
    D_800A68A0->unk1C = (((((u32) *D_800A68C4 >> 0x10) + D_800386C0) % 65535) << 0x10) | (((*D_800A68C4 & 0xFFFF) + D_800386C0) % 65535);
}

void func_80010DB0(void) {
    D_800A68A0->unk30 = (((((u32) *D_800A68D8 >> 0x10) + D_800386C4) % 65535) << 0x10) | (((*D_800A68D8 & 0xFFFF) + D_800386C4) % 65535);
    D_800A68A0->unk44 = (((((u32) D_800A68EC >> 0x10) + D_800386C4) % 65535) << 0x10) | (((D_800A68EC & 0xFFFF) + D_800386C4) % 65535);
}

void func_80010EB0(void *arg0, s8 arg1, f32 arg2, f32 arg3, M2C_UNK32 arg4, M2C_UNK32 arg5) {
    M2C_UNK sp10;
    f32 *temp_s0;
    s32 var_v0;

    var_v0 = 0x30;
    do {
        *(&D_800386DF + var_v0) = arg1;
        var_v0 -= 0x10;
        temp_s0 = arg0 + 0xC;
    } while (var_v0 >= 0);
    Mtx_Translate(temp_s0, arg2, arg3, 0xC3CA8E7E);
    Mtx_Scale(&sp10, arg4, arg5, 0x3F800000);
    Mtx_Mul((s32) temp_s0, (s32) &sp10, (s32) temp_s0);
    M2C_FIELD(arg0, M2C_UNK **, 0xD0) = &D_800387D0;
}

void func_80010F48(void *arg0, s8 arg1, f32 arg2, f32 arg3, M2C_UNK32 arg4, M2C_UNK32 arg5) {
    M2C_UNK sp10;
    f32 *temp_s0;
    s32 var_v0;

    var_v0 = 0x30;
    do {
        *(&D_8003871F + var_v0) = arg1;
        var_v0 -= 0x10;
        temp_s0 = arg0 + 0xC;
    } while (var_v0 >= 0);
    Mtx_Translate(temp_s0, arg2, arg3, 0xC3CA8E7E);
    Mtx_Scale(&sp10, arg4, arg5, 0x3F800000);
    Mtx_Mul((s32) temp_s0, (s32) &sp10, (s32) temp_s0);
    M2C_FIELD(arg0, M2C_UNK **, 0xD0) = &D_80038828;
}

void func_80010FE0(void *arg0, s8 arg1, s8 arg2, s8 arg3, f32 arg4, f32 arg5, M2C_UNK32 arg6, M2C_UNK32 arg7) {
    M2C_UNK sp10;
    f32 *temp_s0;
    s32 var_v1;

    var_v1 = 0;
    do {
        *(&D_8003875C + var_v1) = arg1;
        *(&D_8003875D + var_v1) = arg2;
        *(&D_8003875E + var_v1) = arg3;
        var_v1 += 0x10;
        temp_s0 = arg0 + 0xC;
    } while (var_v1 < 0x40);
    Mtx_Translate(temp_s0, arg4, arg5, 0xC3CA8E7E);
    Mtx_Scale(&sp10, arg6, arg7, 0x3F800000);
    Mtx_Mul((s32) temp_s0, (s32) &sp10, (s32) temp_s0);
    M2C_FIELD(arg0, M2C_UNK **, 0xD0) = &D_80038880;
}

void func_80011088(void *arg0, s8 arg1, s8 arg2, s8 arg3, u8 arg4, f32 arg5, f32 arg6, M2C_UNK32 arg7, M2C_UNK32 arg8) {
    M2C_UNK sp10;
    f32 *temp_s0;
    s32 var_v1;

    var_v1 = 0;
    do {
        *(&D_8003875C + var_v1) = arg1;
        *(&D_8003875D + var_v1) = arg2;
        *(&D_8003875E + var_v1) = arg3;
        *(&D_8003875F + var_v1) = arg4;
        var_v1 += 0x10;
        temp_s0 = arg0 + 0xC;
    } while (var_v1 < 0x40);
    Mtx_Translate(temp_s0, arg5, arg6, 0xC3CA8E7E);
    Mtx_Scale(&sp10, arg7, arg8, 0x3F800000);
    Mtx_Mul((s32) temp_s0, (s32) &sp10, (s32) temp_s0);
    M2C_FIELD(arg0, M2C_UNK **, 0xD0) = &D_80038880;
}

void func_80011140(void *arg0, s8 arg1, s8 arg2, s8 arg3, f32 arg4, f32 arg5, M2C_UNK32 arg6, M2C_UNK32 arg7) {
    M2C_UNK sp10;
    f32 *temp_s0;
    s32 var_v1;

    var_v1 = 0;
    do {
        *(&D_8003879C + var_v1) = arg1;
        *(&D_8003879D + var_v1) = arg2;
        *(&D_8003879E + var_v1) = arg3;
        var_v1 += 0x10;
        temp_s0 = arg0 + 0xC;
    } while (var_v1 < 0x40);
    Mtx_Translate(temp_s0, arg4, arg5, 0xC3CA8E7E);
    Mtx_Scale(&sp10, arg6, arg7, 0x3F800000);
    Mtx_Mul((s32) temp_s0, (s32) &sp10, (s32) temp_s0);
    M2C_FIELD(arg0, M2C_UNK **, 0xD0) = &D_800388D8;
}

void func_800111E8(void *arg0, s8 arg1, s8 arg2, s8 arg3, u8 arg4, f32 arg5, f32 arg6, M2C_UNK32 arg7, M2C_UNK32 arg8) {
    M2C_UNK sp10;
    f32 *temp_s0;
    s32 var_v1;

    var_v1 = 0;
    do {
        *(&D_8003879C + var_v1) = arg1;
        *(&D_8003879D + var_v1) = arg2;
        *(&D_8003879E + var_v1) = arg3;
        *(&D_8003879F + var_v1) = arg4;
        var_v1 += 0x10;
        temp_s0 = arg0 + 0xC;
    } while (var_v1 < 0x40);
    Mtx_Translate(temp_s0, arg5, arg6, 0xC3CA8E7E);
    Mtx_Scale(&sp10, arg7, arg8, 0x3F800000);
    Mtx_Mul((s32) temp_s0, (s32) &sp10, (s32) temp_s0);
    M2C_FIELD(arg0, M2C_UNK **, 0xD0) = &D_800388D8;
}

void func_800112A0(void *arg0, s32 arg1, f32 arg2, f32 arg3, M2C_UNK32 arg4, M2C_UNK32 arg5) {
    M2C_UNK sp10;
    f32 *temp_s0;

    temp_s0 = arg0 + 0xC;
    Mtx_Translate(temp_s0, arg2, arg3, 0xC3CA8E7E);
    Mtx_Scale(&sp10, arg4, arg5, 0x3F800000);
    Mtx_Mul((s32) temp_s0, (s32) &sp10, (s32) temp_s0);
    M2C_FIELD(arg0, s32 *, 0xD0) = arg1;
}

void func_80011334(void *arg0, s32 arg1, void *arg2) {
    M2C_UNK sp18;
    M2C_UNK sp58;
    f32 *temp_s1;

    temp_s1 = arg0 + 0xC;
    Mtx_Translate(temp_s1, M2C_FIELD(arg2, f32 *, 0), M2C_FIELD(arg2, f32 *, 4), M2C_FIELD(arg2, M2C_UNK32 *, 8));
    Mtx_Scale(&sp18, M2C_FIELD(arg2, M2C_UNK32 *, 0xC), M2C_FIELD(arg2, M2C_UNK32 *, 0x10), M2C_FIELD(arg2, M2C_UNK32 *, 0x14));
    Mtx_Mul((s32) &sp58, (s32) &sp18, (s32) temp_s1);
    func_80008540(temp_s1, M2C_BITWISE(f32, &sp58), M2C_FIELD(arg2, f32 *, 0x1C), M2C_FIELD(arg2, s32 *, 0x18), M2C_FIELD(arg2, f32 *, 0x20));
    M2C_FIELD(arg0, s32 *, 0xD0) = arg1;
}

void func_800113E0(u8 *arg0) {
    u8 *sp4;
    s32 var_a1;
    s32 var_v1;
    s32 var_v1_2;
    u8 **var_a2;
    u8 *var_a0;
    u8 temp_v1_2;
    u8 var_v0;
    u8 var_v0_2;
    void *temp_v1;
    void *var_a3;

    /* Flowgraph is not reducible, falling back to gotos-only mode. */
    var_a0 = arg0;
    var_a1 = 1;
    D_800AF230 = 0;
    D_800B04C8 = 0;
    D_800B04B4 = 0;
    D_800B0728 = 0;
    var_a3 = sp;
    if (var_a0 == NULL) {
        goto block_30;
    }
    var_v0 = *var_a0;
    var_a2 = &sp4;
    if (var_v0 == 0) {
        goto block_30;
    }
    var_v1 = var_v0 != 0;
    goto loop_5;
block_3:
    var_a0 += 1;
    var_v0 = *var_a0;
block_4:
    var_v1 = var_v0 != 0;
loop_5:
    if (!(var_v1 & (var_v0 == 0x20))) {
        goto block_7;
    }
    *var_a0 = 0;
    goto block_3;
block_7:
    var_v0_2 = *var_a0;
    var_v1_2 = var_v0_2 != 0;
    if (var_v0_2 == 0) {
        goto block_11;
    }
    *var_a2 = var_a0;
    var_a2 += 4;
    var_a1 += 1;
block_10:
    var_v0_2 = *var_a0;
    var_v1_2 = var_v0_2 != 0;
block_11:
    if (!(var_v1_2 & (var_v0_2 != 0x20))) {
        goto block_13;
    }
    var_a0 += 1;
    goto block_10;
block_13:
    var_v0 = *var_a0;
    if (var_v0 != 0) {
        goto block_4;
    }
    if (var_a1 < 2) {
        goto block_30;
    }
loop_17:
    temp_v1 = M2C_FIELD(var_a3, void **, 4);
    if (M2C_FIELD(temp_v1, u8 *, 0) != 0x2D) {
        goto block_30;
    }
    temp_v1_2 = M2C_FIELD(temp_v1, u8 *, 1);
    if (temp_v1_2 == 0x6C) {
        goto block_28;
    }
    if ((s32) temp_v1_2 >= 0x6D) {
        goto block_22;
    }
    if (temp_v1_2 == 0x64) {
        goto block_25;
    }
    var_a1 -= 1;
    goto block_29;
block_22:
    if (temp_v1_2 == 0x73) {
        goto block_27;
    }
    if (temp_v1_2 == 0x76) {
        goto block_26;
    }
    var_a1 -= 1;
    goto block_29;
block_25:
    D_800B04C8 = 1;
    var_a1 -= 1;
    goto block_29;
block_26:
    D_800AF230 = 1;
    var_a1 -= 1;
    goto block_29;
block_27:
    D_800B04B4 = 1;
    var_a1 -= 1;
    goto block_29;
block_28:
    D_800B0728 = 1;
    var_a1 -= 1;
block_29:
    var_a3 += 4;
    if (var_a1 >= 2) {
        goto loop_17;
    }
block_30:
    return;
}

void func_80011550(void) {
    M2C_UNK *temp_v0;
    M2C_UNK *temp_v0_2;
    M2C_UNK *temp_v0_3;
    M2C_UNK *temp_v0_4;

    func_80009D40(&D_800B4AB0, 0xC000, 0xE0);
    D_800C316C = func_800116B8(1);
    D_800C30C0 = func_800116B8(1);
    D_800C0AB4 = func_800116B8(1);
    D_800AF234 = func_800116B8(1);
    temp_v0 = func_800116B8(1);
    *M2C_FIELD(M2C_FIELD(D_800C316C, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0;
    M2C_FIELD(temp_v0, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800C316C, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0, M2C_UNK **, 0) = M2C_FIELD(D_800C316C, M2C_UNK **, 0xD4);
    D_800B061C = temp_v0;
    M2C_FIELD(M2C_FIELD(D_800C316C, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0;
    temp_v0_2 = func_800116B8(1);
    *M2C_FIELD(M2C_FIELD(D_800C30C0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_2;
    M2C_FIELD(temp_v0_2, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800C30C0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0_2, M2C_UNK **, 0) = M2C_FIELD(D_800C30C0, M2C_UNK **, 0xD4);
    D_800B04A0 = temp_v0_2;
    M2C_FIELD(M2C_FIELD(D_800C30C0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_2;
    temp_v0_3 = func_800116B8(1);
    *M2C_FIELD(M2C_FIELD(D_800C0AB4, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_3;
    M2C_FIELD(temp_v0_3, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800C0AB4, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0_3, M2C_UNK **, 0) = M2C_FIELD(D_800C0AB4, M2C_UNK **, 0xD4);
    D_800AF240 = temp_v0_3;
    M2C_FIELD(M2C_FIELD(D_800C0AB4, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_3;
    temp_v0_4 = func_800116B8(1);
    *M2C_FIELD(M2C_FIELD(D_800AF234, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_4;
    M2C_FIELD(temp_v0_4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800AF234, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0_4, M2C_UNK **, 0) = M2C_FIELD(D_800AF234, M2C_UNK **, 0xD4);
    D_800B0500 = temp_v0_4;
    M2C_FIELD(M2C_FIELD(D_800AF234, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_4;
}

M2C_UNK *func_800116B8(s32 arg0) {
    M2C_UNK *temp_v0;
    M2C_UNK *var_v1;
    s32 var_v0;

    temp_v0 = func_80009DB4(&D_800B4AB0);
    var_v1 = temp_v0;
    if (temp_v0 != NULL) {
        var_v0 = 0x38;
        do {
            *var_v1 = NULL;
            var_v0 -= 1;
            var_v1 += 4;
        } while (var_v0 != 0);
        M2C_FIELD(temp_v0, s32 *, 8) = arg0;
        if (arg0 == 1) {
            M2C_FIELD(temp_v0, M2C_UNK **, 0xD0) = func_800116B8(0);
            M2C_FIELD(temp_v0, M2C_UNK **, 0xD4) = func_800116B8(0);
            M2C_FIELD(M2C_FIELD(temp_v0, M2C_UNK **, 0xD0), s32 *, 4) = 0;
            M2C_FIELD(M2C_FIELD(temp_v0, M2C_UNK **, 0xD4), M2C_UNK **, 0) = NULL;
            M2C_FIELD(M2C_FIELD(temp_v0, M2C_UNK **, 0xD0), M2C_UNK **, 0) = M2C_FIELD(temp_v0, M2C_UNK **, 0xD4);
            M2C_FIELD(M2C_FIELD(temp_v0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(temp_v0, M2C_UNK **, 0xD0);
        } else if (arg0 == 4) {
            M2C_FIELD(temp_v0, s16 *, 0xD6) = 0xEF;
            M2C_FIELD(temp_v0, M2C_UNK **, 0xD0) = 0;
            M2C_FIELD(temp_v0, s16 *, 0xD2) = 0;
            M2C_FIELD(temp_v0, s16 *, 0xD8) = 0x2000;
            M2C_FIELD(temp_v0, s16 *, 0xDA) = 0x2000;
            M2C_FIELD(temp_v0, M2C_UNK **, 0xD4) = (s16) (gScreenWidth - 1);
        }
        Mtx_Identity((f32 *) (temp_v0 + 0xC));
    }
    return temp_v0;
}

void func_8001179C(void) {
    func_80009DF4(&D_800B4AB0);
}

void func_800117BC(M2C_UNK *arg0) {
    M2C_UNK *var_a0;
    M2C_UNK *var_s0;

    if (M2C_FIELD(arg0, s32 *, 8) == 1) {
        var_a0 = *M2C_FIELD(arg0, M2C_UNK **, 0xD0);
        var_s0 = *var_a0;
        if (var_s0 != NULL) {
            do {
                func_800117BC(var_a0);
                var_a0 = var_s0;
                var_s0 = *var_s0;
            } while (var_s0 != NULL);
        }
        func_80009E2C(&D_800B4AB0, M2C_FIELD(arg0, M2C_UNK **, 0xD0));
        func_80009E2C(&D_800B4AB0, M2C_FIELD(arg0, M2C_UNK **, 0xD4));
    }
    *M2C_FIELD(arg0, void ***, 4) = M2C_FIELD(arg0, M2C_UNK **, 0);
    M2C_FIELD(M2C_FIELD(arg0, M2C_UNK **, 0), M2C_UNK ***, 4) = (M2C_UNK **) M2C_FIELD(arg0, M2C_UNK ***, 4);
    func_80009E2C(&D_800B4AB0, arg0);
}

void func_80011868(M2C_UNK *arg0, f32 *arg1) {
    M2C_UNK *var_a0;
    M2C_UNK *var_s0;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 temp_f2_4;
    f32 temp_f2_5;
    f32 temp_f2_6;
    f32 temp_f2_7;
    f32 temp_f2_8;
    f32 temp_f6;
    f32 temp_f6_2;
    f32 temp_f6_3;
    f32 temp_f6_4;
    f32 temp_f8;
    f32 temp_f8_2;
    f32 temp_f8_3;
    f32 temp_f8_4;

    temp_f2 = M2C_FIELD(arg0, f32 *, 0xC);
    temp_f8 = M2C_FIELD(arg0, f32 *, 0x14);
    temp_f2_2 = M2C_FIELD(arg0, f32 *, 0x10);
    temp_f6 = M2C_FIELD(arg0, f32 *, 0x18);
    temp_f2_3 = M2C_FIELD(arg0, f32 *, 0x1C);
    M2C_FIELD(arg0, f32 *, 0x4C) = (f32) ((temp_f2 * M2C_FIELD(arg1, f32 *, 0)) + (temp_f2_2 * M2C_FIELD(arg1, f32 *, 0x10)) + (temp_f8 * M2C_FIELD(arg1, f32 *, 0x20)) + (temp_f6 * M2C_FIELD(arg1, f32 *, 0x30)));
    M2C_FIELD(arg0, f32 *, 0x50) = (f32) ((temp_f2 * M2C_FIELD(arg1, f32 *, 4)) + (temp_f2_2 * M2C_FIELD(arg1, f32 *, 0x14)) + (temp_f8 * M2C_FIELD(arg1, f32 *, 0x24)) + (temp_f6 * M2C_FIELD(arg1, f32 *, 0x34)));
    M2C_FIELD(arg0, f32 *, 0x54) = (f32) ((temp_f2 * M2C_FIELD(arg1, f32 *, 8)) + (temp_f2_2 * M2C_FIELD(arg1, f32 *, 0x18)) + (temp_f8 * M2C_FIELD(arg1, f32 *, 0x28)) + (temp_f6 * M2C_FIELD(arg1, f32 *, 0x38)));
    M2C_FIELD(arg0, f32 *, 0x58) = (f32) ((temp_f2 * M2C_FIELD(arg1, f32 *, 0xC)) + (temp_f2_2 * M2C_FIELD(arg1, f32 *, 0x1C)) + (temp_f8 * M2C_FIELD(arg1, f32 *, 0x2C)) + (temp_f6 * M2C_FIELD(arg1, f32 *, 0x3C)));
    temp_f8_2 = M2C_FIELD(arg0, f32 *, 0x24);
    temp_f2_4 = M2C_FIELD(arg0, f32 *, 0x20);
    temp_f6_2 = M2C_FIELD(arg0, f32 *, 0x28);
    M2C_FIELD(arg0, f32 *, 0x5C) = (f32) ((temp_f2_3 * M2C_FIELD(arg1, f32 *, 0)) + (temp_f2_4 * M2C_FIELD(arg1, f32 *, 0x10)) + (temp_f8_2 * M2C_FIELD(arg1, f32 *, 0x20)) + (temp_f6_2 * M2C_FIELD(arg1, f32 *, 0x30)));
    temp_f2_5 = M2C_FIELD(arg0, f32 *, 0x2C);
    M2C_FIELD(arg0, f32 *, 0x60) = (f32) ((temp_f2_3 * M2C_FIELD(arg1, f32 *, 4)) + (temp_f2_4 * M2C_FIELD(arg1, f32 *, 0x14)) + (temp_f8_2 * M2C_FIELD(arg1, f32 *, 0x24)) + (temp_f6_2 * M2C_FIELD(arg1, f32 *, 0x34)));
    M2C_FIELD(arg0, f32 *, 0x64) = (f32) ((temp_f2_3 * M2C_FIELD(arg1, f32 *, 8)) + (temp_f2_4 * M2C_FIELD(arg1, f32 *, 0x18)) + (temp_f8_2 * M2C_FIELD(arg1, f32 *, 0x28)) + (temp_f6_2 * M2C_FIELD(arg1, f32 *, 0x38)));
    M2C_FIELD(arg0, f32 *, 0x68) = (f32) ((temp_f2_3 * M2C_FIELD(arg1, f32 *, 0xC)) + (temp_f2_4 * M2C_FIELD(arg1, f32 *, 0x1C)) + (temp_f8_2 * M2C_FIELD(arg1, f32 *, 0x2C)) + (temp_f6_2 * M2C_FIELD(arg1, f32 *, 0x3C)));
    temp_f8_3 = M2C_FIELD(arg0, f32 *, 0x34);
    temp_f2_6 = M2C_FIELD(arg0, f32 *, 0x30);
    temp_f6_3 = M2C_FIELD(arg0, f32 *, 0x38);
    temp_f2_7 = M2C_FIELD(arg0, f32 *, 0x3C);
    M2C_FIELD(arg0, f32 *, 0x6C) = (f32) ((temp_f2_5 * M2C_FIELD(arg1, f32 *, 0)) + (temp_f2_6 * M2C_FIELD(arg1, f32 *, 0x10)) + (temp_f8_3 * M2C_FIELD(arg1, f32 *, 0x20)) + (temp_f6_3 * M2C_FIELD(arg1, f32 *, 0x30)));
    M2C_FIELD(arg0, f32 *, 0x70) = (f32) ((temp_f2_5 * M2C_FIELD(arg1, f32 *, 4)) + (temp_f2_6 * M2C_FIELD(arg1, f32 *, 0x14)) + (temp_f8_3 * M2C_FIELD(arg1, f32 *, 0x24)) + (temp_f6_3 * M2C_FIELD(arg1, f32 *, 0x34)));
    M2C_FIELD(arg0, f32 *, 0x74) = (f32) ((temp_f2_5 * M2C_FIELD(arg1, f32 *, 8)) + (temp_f2_6 * M2C_FIELD(arg1, f32 *, 0x18)) + (temp_f8_3 * M2C_FIELD(arg1, f32 *, 0x28)) + (temp_f6_3 * M2C_FIELD(arg1, f32 *, 0x38)));
    M2C_FIELD(arg0, f32 *, 0x78) = (f32) ((temp_f2_5 * M2C_FIELD(arg1, f32 *, 0xC)) + (temp_f2_6 * M2C_FIELD(arg1, f32 *, 0x1C)) + (temp_f8_3 * M2C_FIELD(arg1, f32 *, 0x2C)) + (temp_f6_3 * M2C_FIELD(arg1, f32 *, 0x3C)));
    temp_f8_4 = M2C_FIELD(arg0, f32 *, 0x44);
    temp_f2_8 = M2C_FIELD(arg0, f32 *, 0x40);
    temp_f6_4 = M2C_FIELD(arg0, f32 *, 0x48);
    M2C_FIELD(arg0, f32 *, 0x7C) = (f32) ((temp_f2_7 * M2C_FIELD(arg1, f32 *, 0)) + (temp_f2_8 * M2C_FIELD(arg1, f32 *, 0x10)) + (temp_f8_4 * M2C_FIELD(arg1, f32 *, 0x20)) + (temp_f6_4 * M2C_FIELD(arg1, f32 *, 0x30)));
    M2C_FIELD(arg0, f32 *, 0x80) = (f32) ((temp_f2_7 * M2C_FIELD(arg1, f32 *, 4)) + (temp_f2_8 * M2C_FIELD(arg1, f32 *, 0x14)) + (temp_f8_4 * M2C_FIELD(arg1, f32 *, 0x24)) + (temp_f6_4 * M2C_FIELD(arg1, f32 *, 0x34)));
    M2C_FIELD(arg0, f32 *, 0x84) = (f32) ((temp_f2_7 * M2C_FIELD(arg1, f32 *, 8)) + (temp_f2_8 * M2C_FIELD(arg1, f32 *, 0x18)) + (temp_f8_4 * M2C_FIELD(arg1, f32 *, 0x28)) + (temp_f6_4 * M2C_FIELD(arg1, f32 *, 0x38)));
    M2C_FIELD(arg0, f32 *, 0x88) = (f32) ((temp_f2_7 * M2C_FIELD(arg1, f32 *, 0xC)) + (temp_f2_8 * M2C_FIELD(arg1, f32 *, 0x1C)) + (temp_f8_4 * M2C_FIELD(arg1, f32 *, 0x2C)) + (temp_f6_4 * M2C_FIELD(arg1, f32 *, 0x3C)));
    if (M2C_FIELD(arg0, s32 *, 8) == 1) {
        var_a0 = *M2C_FIELD(arg0, M2C_UNK ***, 0xD0);
        var_s0 = *var_a0;
        if (var_s0 != NULL) {
            do {
                func_80011868(var_a0, arg0 + 0x4C);
                var_a0 = var_s0;
                var_s0 = *var_s0;
            } while (var_s0 != NULL);
        }
    }
}

void func_80011C20(M2C_UNK *arg0) {
    M2C_UNK *var_a0;
    M2C_UNK *var_s0;

    func_8000A924(arg0 + 0x90, arg0 + 0x4C);
    if (M2C_FIELD(arg0, s32 *, 8) == 1) {
        var_a0 = *M2C_FIELD(arg0, M2C_UNK ***, 0xD0);
        var_s0 = *var_a0;
        if (var_s0 != NULL) {
            do {
                func_80011C20(var_a0);
                var_a0 = var_s0;
                var_s0 = *var_s0;
            } while (var_s0 != NULL);
        }
    }
}

s32 *func_80011C88(s32 *arg0, M2C_UNK *arg1) {
    M2C_UNK *var_a1;
    M2C_UNK *var_s0;
    s32 *temp_v0;
    s32 *temp_v0_2;
    s32 *temp_v0_3;
    s32 *temp_v1_2;
    s32 *var_a0;
    s32 *var_v0;
    s32 temp_v1;
    s32 var_a2;
    void *temp_a0;
    void *temp_v1_3;
    void *var_a1_2;

    var_a0 = arg0;
    temp_v1 = M2C_FIELD(arg1, s32 *, 8);
    if (temp_v1 != 2) {
        if (temp_v1 < 3) {
            if (temp_v1 != 1) {
                return var_a0;
            }
            var_a1 = *M2C_FIELD(arg1, M2C_UNK ***, 0xD0);
            var_s0 = *var_a1;
            var_v0 = var_a0;
            if (var_s0 != NULL) {
                do {
                    temp_v0 = func_80011C88(var_a0, var_a1);
                    var_a1 = var_s0;
                    var_s0 = *var_s0;
                    var_a0 = temp_v0;
                } while (var_s0 != NULL);
                return var_a0;
            }
            /* Duplicate return node #22. Try simplifying control flow for better match */
            return var_v0;
        }
        switch (temp_v1) {                          /* irregular */
        case 3:
            temp_v1_2 = var_a0;
            var_a0 += 8;
            M2C_FIELD(temp_v1_2, s32 *, 0) = 0x01020040;
            M2C_FIELD(temp_v1_2, void **, 4) = (void *) (arg1 + 0x90);
            var_a2 = 0;
            if ((s32) M2C_FIELD(arg1, s16 *, 0xD4) > 0) {
                var_a1_2 = var_a0 - 4;
                do {
                    var_a1_2 += 8;
                    M2C_FIELD(var_a1_2, s32 *, -4) = 0x06000000;
                    M2C_FIELD(var_a1_2, s32 *, 0) = (s32) *(((var_a2 & 0xFFFF) * 4) + M2C_FIELD(arg1, M2C_UNK ***, 0xD0));
                    var_a2 += 1;
                    var_a0 += 8;
                } while ((var_a2 & 0xFFFF) < (s32) M2C_FIELD(arg1, s16 *, 0xD4));
                return var_a0;
            }
            goto block_21;
        case 4:
            M2C_FIELD(var_a0, s32 *, 0) = 0x01020040;
            M2C_FIELD(var_a0, void **, 4) = (void *) (arg1 + 0x90);
            var_a0 = func_80010900(var_a0 + 8, (s16) M2C_FIELD(arg1, M2C_UNK ***, 0xD0), M2C_FIELD(arg1, s16 *, 0xD2), M2C_FIELD(arg1, s16 *, 0xD4), (u16) (s32) M2C_FIELD(arg1, s16 *, 0xD6), (s32) M2C_FIELD(arg1, s16 *, 0xD8), (s32) M2C_FIELD(arg1, s16 *, 0xDA));
            goto block_21;
        default:
            return var_a0;
        }
    } else {
        temp_v0_2 = var_a0;
        if ((s32) M2C_FIELD(arg1, s16 *, 0xD4) != 0) {
            M2C_FIELD(temp_v0_2, s32 *, 0) = 0x06000000;
            var_a0 += 8;
            M2C_FIELD(temp_v0_2, s32 *, 4) = (s32) M2C_FIELD(arg1, s16 *, 0xD4);
        }
        if (M2C_FIELD(arg1, M2C_UNK ***, 0xD0) != NULL) {
            temp_a0 = var_a0 + 8;
            M2C_FIELD(var_a0, s32 *, 0) = 0x01020040;
            M2C_FIELD(var_a0, void **, 4) = (void *) (arg1 + 0x90);
            temp_v1_3 = temp_a0;
            M2C_FIELD(var_a0, s32 *, 8) = 0x06000000;
            var_a0 = temp_a0 + 8;
            M2C_FIELD(temp_v1_3, M2C_UNK ***, 4) = (M2C_UNK **) M2C_FIELD(arg1, M2C_UNK ***, 0xD0);
        }
        temp_v0_3 = var_a0;
        if ((s32) M2C_FIELD(arg1, s16 *, 0xD8) != 0) {
            M2C_FIELD(temp_v0_3, s32 *, 0) = 0x06000000;
            var_a0 += 8;
            M2C_FIELD(temp_v0_3, s32 *, 4) = (s32) M2C_FIELD(arg1, s16 *, 0xD8);
        }
block_21:
        var_v0 = var_a0;
        return var_v0;
    }
}

void func_80011E60(void) {
    D_800B0444 = 0.0f;
    D_800B0448 = 0.0f;
    D_800B04B8 = 0.0f;
    D_800B04BC = 0.0f;
    D_800B04C0 = 0.0f;
    D_800C1664 = 0.0f;
    D_800C166C = 0.0f;
    D_800B044C = 405.11322f;
    D_800C1668 = 1.0f;
    D_800C51D4 = 33.0f;
}

void func_80011ED0(f32 arg0, f32 arg1, f32 arg2) {
    f32 sp10;
    f32 sp20;

    D_800B0444 = arg0;
    D_800B0448 = arg1;
    D_800B044C = arg2;
    func_80011FB0(&sp10);
    func_80015988(&sp20, &D_800C1664, &sp10);
    func_80015988(&D_800C1664, &sp10, &sp20);
}

void func_80011F40(f32 arg0, f32 arg1, f32 arg2) {
    f32 sp10;
    f32 sp20;

    D_800B04B8 = arg0;
    D_800B04BC = arg1;
    D_800B04C0 = arg2;
    func_80011FB0(&sp10);
    func_80015988(&sp20, &D_800C1664, &sp10);
    func_80015988(&D_800C1664, &sp10, &sp20);
}

void func_80011FB0(f32 *arg0) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f12;
    f32 temp_f2;
    f32 temp_f4;
    f32 var_f6;

    M2C_FIELD(arg0, f32 *, 0) = D_800B04B8 - D_800B0444;
    temp_f4 = M2C_FIELD(arg0, f32 *, 0);
    M2C_FIELD(arg0, f32 *, 4) = (f32) (D_800B04BC - D_800B0448);
    temp_f0 = M2C_FIELD(arg0, f32 *, 4);
    temp_f2 = D_800B04C0 - D_800B044C;
    temp_f12 = (temp_f4 * temp_f4) + (temp_f0 * temp_f0) + (temp_f2 * temp_f2);
    var_f6 = sqrtf(temp_f12);
    M2C_FIELD(arg0, f32 *, 8) = temp_f2;
    if (var_f6 != var_f6) {
        var_f6 = sqrtf(temp_f12);
    }
    temp_f0_2 = (f32) (1.0 / (f64) var_f6);
    M2C_FIELD(arg0, f32 *, 0) *= temp_f0_2;
    M2C_FIELD(arg0, f32 *, 4) = (f32) (M2C_FIELD(arg0, f32 *, 4) * temp_f0_2);
    M2C_FIELD(arg0, f32 *, 8) = (f32) (M2C_FIELD(arg0, f32 *, 8) * temp_f0_2);
}

void func_80012090(M2C_UNK *arg0) {
    f32 sp10;

    Mtx_Identity(&sp10);
    M2C_FIELD(arg0, s32 *, 0xC) = 0;
    M2C_FIELD(arg0, s32 *, 0x10) = 0;
    M2C_FIELD(arg0, s32 *, 0x18) = 0;
    M2C_FIELD(arg0, s32 *, 0x1C) = 0;
    M2C_FIELD(arg0, s32 *, 0x20) = 0;
    M2C_FIELD(arg0, f32 *, 0x14) = 810.22644f;
    func_80012760(arg0, arg0);
    func_80012180(arg0, 255.0f, 255.0f, 255.0f);
    func_80012250(arg0, 0.0f, 0.0f, 0.0f);
    func_80012320(arg0, &sp10);
}

void func_80012130(M2C_UNK *arg0, s32 arg1, s32 arg2, s32 arg3) {
    M2C_FIELD(arg0, s32 *, 0xC) = arg1;
    M2C_FIELD(arg0, s32 *, 0x10) = arg2;
    M2C_FIELD(arg0, s32 *, 0x14) = arg3;
    func_80012760(arg0);
}

void func_80012158(M2C_UNK *arg0, s32 arg1, s32 arg2, s32 arg3) {
    M2C_FIELD(arg0, s32 *, 0x18) = arg1;
    M2C_FIELD(arg0, s32 *, 0x1C) = arg2;
    M2C_FIELD(arg0, s32 *, 0x20) = arg3;
    func_80012760(arg0);
}

void func_80012180(M2C_UNK *arg0, f32 arg1, f32 arg2, f32 arg3) {
    f32 var_f0;
    f32 var_f2;
    f32 var_f4;
    f32 var_f6;
    f32 var_f6_2;

    var_f6 = 255.0f;
    var_f0 = arg1;
    var_f2 = arg2;
    var_f4 = arg3;
    if (var_f0 > 255.0f) {
        var_f0 = 255.0f;
    } else {
        if (var_f0 < 0.0f) {
            var_f0 = 0.0f;
        }
        var_f6 = 255.0f;
    }
    if ((var_f2 > 255.0f) || (var_f6 = 0.0f, (var_f2 < 0.0f))) {
        var_f2 = var_f6;
    }
    var_f6_2 = 255.0f;
    if ((var_f4 > 255.0f) || (var_f6_2 = 0.0f, (var_f4 < 0.0f))) {
        var_f4 = var_f6_2;
    }
    M2C_FIELD(arg0, f32 *, 0x24) = var_f0;
    M2C_FIELD(arg0, f32 *, 0x28) = var_f2;
    M2C_FIELD(arg0, f32 *, 0x2C) = var_f4;
}

void func_80012250(M2C_UNK *arg0, f32 arg1, f32 arg2, f32 arg3) {
    f32 var_f0;
    f32 var_f2;
    f32 var_f4;
    f32 var_f6;
    f32 var_f6_2;

    var_f6 = 255.0f;
    var_f0 = arg1;
    var_f2 = arg2;
    var_f4 = arg3;
    if (var_f0 > 255.0f) {
        var_f0 = 255.0f;
    } else {
        if (var_f0 < 0.0f) {
            var_f0 = 0.0f;
        }
        var_f6 = 255.0f;
    }
    if ((var_f2 > 255.0f) || (var_f6 = 0.0f, (var_f2 < 0.0f))) {
        var_f2 = var_f6;
    }
    var_f6_2 = 255.0f;
    if ((var_f4 > 255.0f) || (var_f6_2 = 0.0f, (var_f4 < 0.0f))) {
        var_f4 = var_f6_2;
    }
    M2C_FIELD(arg0, f32 *, 0x30) = var_f0;
    M2C_FIELD(arg0, f32 *, 0x34) = var_f2;
    M2C_FIELD(arg0, f32 *, 0x38) = var_f4;
}

void func_80012320(M2C_UNK *arg0, f32 *arg1) {
    f32 sp10;
    f32 sp14;
    f32 sp18;
    f32 temp_f2;
    f32 temp_f2_10;
    f32 temp_f2_11;
    f32 temp_f2_12;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 temp_f2_4;
    f32 temp_f2_5;
    f32 temp_f2_6;
    f32 temp_f2_7;
    f32 temp_f2_8;
    f32 temp_f2_9;
    f32 temp_f4;
    s32 var_v1;
    s32 var_v1_10;
    s32 var_v1_11;
    s32 var_v1_2;
    s32 var_v1_3;
    s32 var_v1_4;
    s32 var_v1_5;
    s32 var_v1_6;
    s32 var_v1_7;
    s32 var_v1_8;
    s32 var_v1_9;

    sp10 = M2C_FIELD(arg0, f32 *, 0);
    sp14 = M2C_FIELD(arg0, f32 *, 4);
    sp18 = M2C_FIELD(arg0, f32 *, 8);
    Mtx_TransformVector(&sp10, &sp10, arg1);
    temp_f2 = 120.0f / sqrtf((sp10 * sp10) + (sp14 * sp14) + (sp18 * sp18));
    M2C_FIELD(arg0, s8 *, 0x50) = (s8) (s32) (sp10 * temp_f2);
    M2C_FIELD(arg0, s8 *, 0x51) = (s8) (s32) (sp14 * temp_f2);
    temp_f4 = M2C_FIELD(arg0, f32 *, 0x24);
    M2C_FIELD(arg0, s8 *, 0x52) = (s8) (s32) (sp18 * temp_f2);
    if (!(temp_f4 >= 2.1474836e9f)) {
        var_v1 = (s32) temp_f4;
    } else {
        var_v1 = (s32) (temp_f4 - 2.1474836e9f) | 0x80000000;
    }
    temp_f2_2 = M2C_FIELD(arg0, f32 *, 0x28);
    M2C_FIELD(arg0, s8 *, 0x48) = (s8) var_v1;
    if (!(temp_f2_2 >= 2.1474836e9f)) {
        var_v1_2 = (s32) temp_f2_2;
    } else {
        var_v1_2 = (s32) (temp_f2_2 - 2.1474836e9f) | 0x80000000;
    }
    temp_f2_3 = M2C_FIELD(arg0, f32 *, 0x2C);
    M2C_FIELD(arg0, s8 *, 0x49) = (s8) var_v1_2;
    if (!(temp_f2_3 >= 2.1474836e9f)) {
        var_v1_3 = (s32) temp_f2_3;
    } else {
        var_v1_3 = (s32) (temp_f2_3 - 2.1474836e9f) | 0x80000000;
    }
    temp_f2_4 = M2C_FIELD(arg0, f32 *, 0x24);
    M2C_FIELD(arg0, s8 *, 0x4A) = (s8) var_v1_3;
    if (!(temp_f2_4 >= 2.1474836e9f)) {
        var_v1_4 = (s32) temp_f2_4;
    } else {
        var_v1_4 = (s32) (temp_f2_4 - 2.1474836e9f) | 0x80000000;
    }
    temp_f2_5 = M2C_FIELD(arg0, f32 *, 0x28);
    M2C_FIELD(arg0, s8 *, 0x4C) = (s8) var_v1_4;
    if (!(temp_f2_5 >= 2.1474836e9f)) {
        var_v1_5 = (s32) temp_f2_5;
    } else {
        var_v1_5 = (s32) (temp_f2_5 - 2.1474836e9f) | 0x80000000;
    }
    temp_f2_6 = M2C_FIELD(arg0, f32 *, 0x2C);
    M2C_FIELD(arg0, s8 *, 0x4D) = (s8) var_v1_5;
    if (!(temp_f2_6 >= 2.1474836e9f)) {
        var_v1_6 = (s32) temp_f2_6;
    } else {
        var_v1_6 = (s32) (temp_f2_6 - 2.1474836e9f) | 0x80000000;
    }
    temp_f2_7 = M2C_FIELD(arg0, f32 *, 0x30);
    M2C_FIELD(arg0, s8 *, 0x4E) = (s8) var_v1_6;
    M2C_FIELD(arg0, s8 *, 0x4B) = 0;
    M2C_FIELD(arg0, s8 *, 0x4F) = 0;
    M2C_FIELD(arg0, s8 *, 0x53) = 0;
    if (!(temp_f2_7 >= 2.1474836e9f)) {
        var_v1_7 = (s32) temp_f2_7;
    } else {
        var_v1_7 = (s32) (temp_f2_7 - 2.1474836e9f) | 0x80000000;
    }
    temp_f2_8 = M2C_FIELD(arg0, f32 *, 0x34);
    M2C_FIELD(arg0, s8 *, 0x40) = (s8) var_v1_7;
    if (!(temp_f2_8 >= 2.1474836e9f)) {
        var_v1_8 = (s32) temp_f2_8;
    } else {
        var_v1_8 = (s32) (temp_f2_8 - 2.1474836e9f) | 0x80000000;
    }
    temp_f2_9 = M2C_FIELD(arg0, f32 *, 0x38);
    M2C_FIELD(arg0, s8 *, 0x41) = (s8) var_v1_8;
    if (!(temp_f2_9 >= 2.1474836e9f)) {
        var_v1_9 = (s32) temp_f2_9;
    } else {
        var_v1_9 = (s32) (temp_f2_9 - 2.1474836e9f) | 0x80000000;
    }
    temp_f2_10 = M2C_FIELD(arg0, f32 *, 0x30);
    M2C_FIELD(arg0, s8 *, 0x42) = (s8) var_v1_9;
    if (!(temp_f2_10 >= 2.1474836e9f)) {
        var_v1_10 = (s32) temp_f2_10;
    } else {
        var_v1_10 = (s32) (temp_f2_10 - 2.1474836e9f) | 0x80000000;
    }
    temp_f2_11 = M2C_FIELD(arg0, f32 *, 0x34);
    M2C_FIELD(arg0, s8 *, 0x44) = (s8) var_v1_10;
    if (!(temp_f2_11 >= 2.1474836e9f)) {
        var_v1_11 = (s32) temp_f2_11;
    } else {
        var_v1_11 = (s32) (temp_f2_11 - 2.1474836e9f) | 0x80000000;
    }
    temp_f2_12 = M2C_FIELD(arg0, f32 *, 0x38);
    M2C_FIELD(arg0, s8 *, 0x45) = (s8) var_v1_11;
    if (!(temp_f2_12 >= 2.1474836e9f)) {
        M2C_FIELD(arg0, s8 *, 0x46) = (s8) (s32) temp_f2_12;
    } else {
        M2C_FIELD(arg0, s8 *, 0x46) = (s8) ((s32) (temp_f2_12 - 2.1474836e9f) | 0x80000000);
    }
    M2C_FIELD(arg0, s8 *, 0x43) = 0;
    M2C_FIELD(arg0, s8 *, 0x47) = 0;
}

void func_80012760(M2C_UNK *arg0, M2C_UNK *arg1) {
    f32 temp_f0;
    f32 temp_f12;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;
    f32 var_f0;

    M2C_FIELD(arg1, f32 *, 0) = (f32) (M2C_FIELD(arg0, f32 *, 0x18) - M2C_FIELD(arg0, f32 *, 0xC));
    temp_f4 = M2C_FIELD(arg1, f32 *, 0);
    M2C_FIELD(arg1, f32 *, 4) = (f32) (M2C_FIELD(arg0, f32 *, 0x1C) - M2C_FIELD(arg0, f32 *, 0x10));
    temp_f0 = M2C_FIELD(arg1, f32 *, 4);
    temp_f2 = M2C_FIELD(arg0, f32 *, 0x20) - M2C_FIELD(arg0, f32 *, 0x14);
    temp_f12 = (temp_f4 * temp_f4) + (temp_f0 * temp_f0) + (temp_f2 * temp_f2);
    var_f0 = sqrtf(temp_f12);
    M2C_FIELD(arg1, f32 *, 8) = temp_f2;
    if (var_f0 != var_f0) {
        var_f0 = sqrtf(temp_f12);
    }
    temp_f2_2 = -120.0f / var_f0;
    M2C_FIELD(arg1, f32 *, 0) = (f32) (M2C_FIELD(arg1, f32 *, 0) * temp_f2_2);
    M2C_FIELD(arg1, f32 *, 4) = (f32) (M2C_FIELD(arg1, f32 *, 4) * temp_f2_2);
    M2C_FIELD(arg1, f32 *, 8) = (f32) (M2C_FIELD(arg1, f32 *, 8) * temp_f2_2);
}

M2C_UNK *Sprite_NewPlain(M2C_UNK *arg0, M2C_UNK *arg1) {
    M2C_UNK *var_s0;

    var_s0 = arg0;
    if (var_s0 == NULL) {
        var_s0 = Heap_Alloc(&gGameHeap, 0x30);
        bzero(var_s0, 0x30);
        M2C_FIELD(var_s0, s16 *, 0x28) = 1;
    } else {
        bzero(var_s0, 0x30);
        M2C_FIELD(var_s0, s16 *, 0x28) = 0;
    }
    if (var_s0 != NULL) {
        Sprite_Parse(var_s0, arg1, 0, 0);
    }
    return var_s0;
}

M2C_UNK *Sprite_New(M2C_UNK *arg0, M2C_UNK *arg1) {
    M2C_UNK *var_s0;

    var_s0 = arg0;
    if (var_s0 == NULL) {
        var_s0 = Heap_Alloc(&gGameHeap, 0x30);
        bzero(var_s0, 0x30);
        M2C_FIELD(var_s0, s16 *, 0x28) = 1;
    } else {
        bzero(var_s0, 0x30);
        M2C_FIELD(var_s0, s16 *, 0x28) = 0;
    }
    if (var_s0 != NULL) {
        Sprite_Parse(var_s0, arg1, 0x1F, 0);
    }
    return var_s0;
}

M2C_UNK *Sprite_NewSharedPalette(M2C_UNK *arg0, M2C_UNK *arg1) {
    M2C_UNK *var_s0;

    var_s0 = arg0;
    if (var_s0 == NULL) {
        var_s0 = Heap_Alloc(&gGameHeap, 0x30);
        bzero(var_s0, 0x30);
        M2C_FIELD(var_s0, s16 *, 0x28) = 1;
    } else {
        bzero(var_s0, 0x30);
        M2C_FIELD(var_s0, s16 *, 0x28) = 0;
    }
    if (var_s0 != NULL) {
        Sprite_Parse(var_s0, arg1, 0x1F, 1);
    }
    return var_s0;
}

M2C_UNK *Sprite_NewScreen(M2C_UNK *arg0, M2C_UNK *arg1) {
    M2C_UNK *var_s0;

    var_s0 = arg0;
    if (var_s0 == NULL) {
        var_s0 = Heap_Alloc(&gGameHeap, 0x30);
        bzero(var_s0, 0x30);
        M2C_FIELD(var_s0, s16 *, 0x28) = 1;
    } else {
        bzero(var_s0, 0x30);
        M2C_FIELD(var_s0, s16 *, 0x28) = 0;
    }
    if (var_s0 != NULL) {
        Sprite_ParseScreen(var_s0, arg1);
    }
    return var_s0;
}

s32 Sprite_Delete(M2C_UNK *arg0) {
    u16 temp_s0;

    temp_s0 = M2C_FIELD(arg0, u16 *, 0x28);
    Heap_Free(&gGameHeap, M2C_FIELD(arg0, M2C_UNK **, 0));
    if (temp_s0 != 0) {
        Heap_Free(&gGameHeap, arg0);
    }
    return 0;
}

void Sprite_Place(void *arg0, void *arg1, f32 arg2, f32 arg3, M2C_UNK32 arg4, M2C_UNK32 arg5) {
    M2C_UNK sp10;
    f32 *temp_s2;

    temp_s2 = arg1 + 0xC;
    Mtx_Translate(temp_s2, arg2, arg3, 0);
    Mtx_Scale(&sp10, arg4, arg5, 0x3F800000);
    Mtx_Mul((s32) temp_s2, (s32) &sp10, (s32) temp_s2);
    M2C_FIELD(arg1, s32 *, 0xD0) = (s32) M2C_FIELD(arg0, s32 *, 0x14);
    M2C_FIELD(arg1, s32 *, 0xD4) = (s32) M2C_FIELD(arg0, s32 *, 0x10);
    M2C_FIELD(arg1, s32 *, 0xD8) = (s32) M2C_FIELD(arg0, s32 *, 0x18);
}

void Sprite_Place2(void *arg0, void *arg1, f32 arg2, f32 arg3, M2C_UNK32 arg4, M2C_UNK32 arg5) {
    M2C_UNK sp10;
    f32 *temp_s2;

    temp_s2 = arg1 + 0xC;
    Mtx_Translate(temp_s2, arg2, arg3, 0);
    Mtx_Scale(&sp10, arg4, arg5, 0x3F800000);
    Mtx_Mul((s32) temp_s2, (s32) &sp10, (s32) temp_s2);
    M2C_FIELD(arg1, s32 *, 0xD0) = (s32) M2C_FIELD(arg0, s32 *, 0x14);
    M2C_FIELD(arg1, s32 *, 0xD4) = (s32) M2C_FIELD(arg0, s32 *, 0x10);
    M2C_FIELD(arg1, s32 *, 0xD8) = (s32) M2C_FIELD(arg0, s32 *, 0x18);
}

void Sprite_PlaceCustom(s32 arg0, void *arg1, void *arg2, f32 arg3, f32 arg4, M2C_UNK32 arg5, M2C_UNK32 arg6) {
    M2C_UNK sp10;
    f32 *temp_s1;

    temp_s1 = arg2 + 0xC;
    Mtx_Translate(temp_s1, arg3, arg4, 0xC3CA8E7E);
    Mtx_Scale(&sp10, arg5, arg6, 0x3F800000);
    Mtx_Mul((s32) temp_s1, (s32) &sp10, (s32) temp_s1);
    M2C_FIELD(arg2, s32 *, 0xD4) = arg0;
    M2C_FIELD(arg2, s32 *, 0xD0) = (s32) M2C_FIELD(arg1, s32 *, 0x14);
}

void Sprite_PlaceScreen(void *arg0, void *arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    Sprite_DrawScreen(arg2 + 160.0f, 120.0f - arg3, arg4, arg5);
    M2C_FIELD(arg1, s32 *, 0xD0) = (s32) M2C_FIELD(arg0, s32 *, 0x14);
    M2C_FIELD(arg1, s32 *, 0xD4) = (s32) M2C_FIELD(arg0, s32 *, 0x10);
    M2C_FIELD(arg1, s32 *, 0xD8) = (s32) M2C_FIELD(arg0, s32 *, 0x18);
}

s32 Sprite_Parse(M2C_UNK *arg0, M2C_UNK *arg1, s32 arg2, s32 arg3) {
    s32 sp10;
    M2C_UNK sp118;
    M2C_UNK sp11C;
    M2C_UNK sp120;
    M2C_UNK sp124;
    s32 sp14C;
    M2C_UNK *sp154;
    u32 sp15C;
    u16 sp166;
    u16 sp16E;
    u16 sp176;
    s32 sp17C;
    s32 sp184;
    s32 sp18C;
    s32 *sp194;
    s32 sp1B4;
    s32 sp1BC;
    s32 sp1C4;
    M2C_UNK *temp_a2;
    M2C_UNK *temp_a2_10;
    M2C_UNK *temp_a2_11;
    M2C_UNK *temp_a2_12;
    M2C_UNK *temp_a2_13;
    M2C_UNK *temp_a2_14;
    M2C_UNK *temp_a2_15;
    M2C_UNK *temp_a2_2;
    M2C_UNK *temp_a2_3;
    M2C_UNK *temp_a2_4;
    M2C_UNK *temp_a2_5;
    M2C_UNK *temp_a2_6;
    M2C_UNK *temp_a2_7;
    M2C_UNK *temp_a2_8;
    M2C_UNK *temp_a2_9;
    M2C_UNK *temp_t2;
    M2C_UNK *temp_t2_10;
    M2C_UNK *temp_t2_2;
    M2C_UNK *temp_t2_3;
    M2C_UNK *temp_t2_4;
    M2C_UNK *temp_t2_5;
    M2C_UNK *temp_t2_6;
    M2C_UNK *temp_t2_7;
    M2C_UNK *temp_t2_8;
    M2C_UNK *temp_t2_9;
    M2C_UNK *temp_v0;
    M2C_UNK *temp_v0_10;
    M2C_UNK *temp_v0_11;
    M2C_UNK *temp_v0_8;
    M2C_UNK *var_a0;
    M2C_UNK *var_a0_5;
    M2C_UNK *var_a1;
    M2C_UNK *var_a2;
    M2C_UNK *var_a2_2;
    M2C_UNK *var_a2_3;
    M2C_UNK *var_a2_4;
    M2C_UNK *var_a3;
    M2C_UNK *var_s1;
    M2C_UNK *var_t0;
    M2C_UNK *var_t2;
    M2C_UNK *var_t2_2;
    M2C_UNK *var_t2_3;
    M2C_UNK *var_t3;
    M2C_UNK *var_t3_2;
    s16 temp_t8;
    s16 var_s4;
    s16 var_s7;
    s32 temp_a0;
    s32 temp_a1_2;
    s32 temp_fp;
    s32 temp_lo;
    s32 temp_s5;
    s32 temp_s7_2;
    s32 temp_t0;
    s32 temp_v0_2;
    s32 temp_v0_6;
    s32 temp_v0_7;
    s32 temp_v0_9;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_4;
    s32 temp_v1_5;
    s32 temp_v1_6;
    s32 var_a0_3;
    s32 var_a1_10;
    s32 var_a1_2;
    s32 var_a1_3;
    s32 var_a1_6;
    s32 var_a1_9;
    s32 var_a3_2;
    s32 var_a3_3;
    s32 var_t1;
    s32 var_t9;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;
    s32 var_v0_4;
    s32 var_v1;
    s32 var_v1_2;
    s32 var_v1_3;
    s32 var_v1_4;
    u16 temp_a3;
    u16 temp_t1;
    u16 temp_t2_11;
    u16 temp_v0_3;
    u16 var_a1_4;
    u16 var_a1_5;
    u16 var_fp;
    u16 var_s5;
    u16 var_s6;
    u16 var_t1_2;
    u32 temp_a1;
    u32 temp_s7;
    u32 temp_t5;
    u32 temp_t6;
    u32 var_a1_7;
    u32 var_a1_8;
    u8 temp_v0_5;
    u8 temp_v1_3;
    void *temp_t3;
    void *temp_t3_2;
    void *temp_t3_3;
    void *temp_t3_4;
    void *temp_t3_5;
    void *temp_t3_6;
    void *temp_t3_7;
    void *temp_t3_8;
    void *temp_v0_4;
    void *var_a0_2;
    void *var_a0_4;
    void *var_v1_5;

    var_s1 = saved_reg_s1;
    var_s4 = saved_reg_s4;
    var_s5 = saved_reg_s5;
    var_s6 = saved_reg_s6;
    var_s7 = saved_reg_s7;
    var_fp = saved_reg_fp;
    sp14C = arg3;
    sp118 = M2C_UNALIGNED32(M2C_FIELD(&D_8003D780, M2C_UNK *, 0));
    sp11C = M2C_UNALIGNED32(M2C_FIELD(&D_8003D780, M2C_UNK *, 4));
    sp120 = M2C_UNALIGNED32(M2C_FIELD(&D_8003D788, M2C_UNK *, 0));
    sp124 = M2C_UNALIGNED32(M2C_FIELD(&D_8003D788, M2C_UNK *, 4));
    SiffIter_Init(&sp10, &D_8003D788);
    sp154 = Asset_LoadLzss(arg1);
    var_t3 = sp154;
    if (arg2 == 0) {
        arg0[4] = &gSpriteSetupPlainDL;
        arg0[6] = NULL;
    } else {
        arg0[4] = &gSpriteSetupDL;
        arg0[6] = &gSpriteFinishDL;
    }
    M2C_FIELD(arg0, u8 *, 0x26) = 2U;
loop_5:
case 0x53696666:                                    /* switch 1 */
loop_6:
    temp_v1 = M2C_FIELD((sp + (sp10 * 0x10)), s32 *, 0x18);
    var_t3 = SiffIter_Enter(&sp10, var_t3);
    if (temp_v1 == 0x53703364) {
        goto loop_5;
    }
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0x696E666F:                                /* switch 1 */
        temp_t3 = var_t3 + 2;
        sp15C = (u32) M2C_FIELD(var_t3, s16 *, 0);
        temp_t3_2 = temp_t3 + 2;
        sp176 = M2C_FIELD(var_t3, u16 *, 2);
        var_fp = M2C_FIELD(temp_t3, u16 *, 2);
        temp_t3_3 = temp_t3_2 + 2;
        var_s6 = M2C_FIELD(temp_t3_2, u16 *, 2);
        temp_t3_4 = temp_t3_3 + 2;
        var_s5 = M2C_FIELD(temp_t3_3, u16 *, 2);
        temp_t3_5 = temp_t3_4 + 2;
        var_s7 = M2C_FIELD(temp_t3_4, s16 *, 2);
        temp_t3_6 = temp_t3_5 + 2;
        var_s4 = M2C_FIELD(temp_t3_5, s16 *, 2);
        temp_t3_7 = temp_t3_6 + 2;
        temp_t3_8 = temp_t3_7 + 2;
        sp166 = M2C_FIELD(temp_t3_7, u16 *, 2);
        temp_t8 = M2C_FIELD((temp_t3_8 + 2), s16 *, 2);
        sp17C = (s32) temp_t8;
        sp16E = M2C_FIELD(temp_t3_8, u16 *, 2);
        if ((u32) temp_t8 < 2U) {
            temp_v0 = Heap_Alloc(&gGameHeap, (sp15C * 2) + (var_s5 * (var_s6 * var_s7)) + (var_s4 * 0x10) + ((M2C_FIELD(temp_t3_6, s16 *, 2) + 0x18) * 8));
            var_s1 = temp_v0;
            M2C_FIELD(arg0, M2C_UNK **, 0) = temp_v0;
block_14:
            var_t9 = var_s6 & 0xFFFF;
            temp_s5 = var_s5 & 0xFFFF;
            var_t3_2 = sp154;
            sp1C4 = temp_s5;
            sp194 = &sp10;
            temp_s7 = var_t9 * temp_s5 * var_s7;
loop_15:
loop_16:
            var_a1 = var_t3_2;
loop_17:
            sp1B4 = var_t9;
            var_t3_2 = SiffIter_Enter(&sp10, var_a1);
            temp_v1_2 = M2C_FIELD((sp194 + (sp10 * 0x10)), s32 *, 8);
            if (temp_v1_2 != 0x696E666F) {
                if (temp_v1_2 <= 0x696E666F) {
                    if (temp_v1_2 != 0x53703364) {
                        if (temp_v1_2 <= 0x53703364) {
                            var_a1 = var_t3_2;
                            if (temp_v1_2 != 0x53696666) {
                                goto block_117;
                            }
                            goto loop_17;
                        }
                        var_t2 = var_s1;
                        if (temp_v1_2 != 0x646C7374) {
                            goto block_118;
                        }
                        temp_v1_3 = M2C_FIELD(arg0, u8 *, 0x26);
                        arg0[5] = var_s1;
                        if (temp_v1_3 < 2U) {
                            if (sp14C == 0) {
                                temp_t2 = var_t2 + 8;
                                if (temp_v1_3 == 1) {
                                    temp_t2_2 = temp_t2 + 8;
                                    var_a3 = temp_t2_2;
                                    temp_t2_3 = temp_t2_2 + 8;
                                    var_a0 = temp_t2_3;
                                    temp_t2_4 = temp_t2_3 + 8;
                                    var_a2 = temp_t2_4;
                                    var_t2_2 = temp_t2_4 + 8;
                                    var_t1 = 0x023F0000;
                                } else {
                                    temp_t2_5 = temp_t2 + 8;
                                    var_a3 = temp_t2_5;
                                    temp_t2_6 = temp_t2_5 + 8;
                                    var_a0 = temp_t2_6;
                                    temp_t2_7 = temp_t2_6 + 8;
                                    var_a2 = temp_t2_7;
                                    var_t2_2 = temp_t2_7 + 8;
                                    var_t1 = 0x02030000;
                                }
                                M2C_FIELD(var_s1, M2C_UNK **, 0) = (M2C_UNK *)0xFD100000;
                                var_t2 = var_t2_2 + 8;
                                var_s1[1] = arg0[2];
                                *temp_t2 = (M2C_UNK *)0xE8000000;
                                temp_t2[1] = NULL;
                                M2C_FIELD(var_a3, M2C_UNK **, 0) = (M2C_UNK *)0xF5000100;
                                var_a3[1] = (M2C_UNK *)0x02000000;
                                M2C_FIELD(var_a0, M2C_UNK **, 0) = (M2C_UNK *)0xE6000000;
                                var_a0[1] = NULL;
                                M2C_FIELD(var_a2, M2C_UNK **, 0) = (M2C_UNK *)0xF0000000;
                                var_a2[1] = var_t1 | 0xC000;
                                M2C_FIELD(var_t2_2, M2C_UNK **, 0) = (M2C_UNK *)0xE7000000;
                                var_t2_2[1] = NULL;
                            }
                        }
                        temp_v0_2 = var_fp & 0xFFFF;
                        var_a2_2 = var_t2 - 4;
                        temp_t6 = sp176 >> 3;
                        temp_lo = sp176 * temp_v0_2;
                        temp_fp = (sp176 - 1) * 4;
                        temp_s7_2 = (temp_v0_2 - 1) * 4;
                        temp_t5 = sp176 >> 4;
                        sp1BC = ((temp_fp & 0xFFF) << 0xC) | (temp_s7_2 & 0xFFF);
                        sp184 = (s32) (temp_lo + 1) >> 1;
                        sp18C = (s32) (temp_lo + 3) >> 2;
loop_67:
                        temp_a3 = *var_t3_2;
                        temp_v1_4 = temp_a3 & 0xC000;
                        var_t3_2 += 2;
                        if (temp_v1_4 == 0x4000) {
                            var_t1_2 = *var_t3_2;
                            var_a2_2 += 8;
                            temp_v1_5 = temp_a3 & 0xF;
                            temp_a1 = (u32) (temp_a3 & 0xF0) >> 4;
                            temp_t0 = temp_v1_5 + 1;
                            M2C_FIELD(var_a2_2, s32 *, -4) = (s32) ((((temp_v1_5 * 0x10) | temp_a1) << 0x10) | ((temp_t0 * 0x10) | 0x04000000));
                            M2C_FIELD(var_a2_2, M2C_UNK **, 0) = arg0[1] + (var_t1_2 * 0x10);
                            var_t3_2 += 2;
                            var_t2 += 8;
                            if (sp17C != 0) {
                                var_a3_2 = 0;
                                if (temp_t0 != 0) {
                                    var_a0_2 = (temp_a1 * 2) + sp194;
                                    do {
                                        temp_v0_3 = var_t1_2;
                                        var_t1_2 += 1;
                                        M2C_FIELD(var_a0_2, u16 *, 0x118) = temp_v0_3;
                                        var_a3_2 += 1;
                                        var_a0_2 += 2;
                                    } while (var_a3_2 < temp_t0);
                                }
                            }
                            goto loop_67;
                        }
                        switch (temp_v1_4) {        /* switch 3; irregular */
                        case 0x0:                   /* switch 3 */
                            if (sp17C != 0) {
                                var_a2_2 += 8;
                                temp_v0_4 = (M2C_FIELD((((temp_a3 & 0xF) * 2) + sp194), u16 *, 0x118) * 0x10) + arg0[1];
                                var_t2 += 8;
                                temp_a0 = (s32) (M2C_FIELD(temp_v0_4, s16 *, 8) + 5) >> 4;
                                temp_a1_2 = (s32) (M2C_FIELD(temp_v0_4, s16 *, 0xA) + 5) >> 4;
                                M2C_FIELD(var_a2_2, s32 *, -4) = (s32) (((temp_a0 & 0xFFF) << 0xC) | ((temp_a1_2 & 0xFFF) | 0xF2000000));
                                M2C_FIELD(var_a2_2, M2C_UNK **, 0) = (M2C_UNK *) ((((temp_a0 + temp_fp) & 0xFFF) << 0xC) | ((temp_a1_2 + temp_s7_2) & 0xFFF));
                            }
                            var_a2_2 += 8;
                            var_t2 += 8;
                            M2C_FIELD(var_a2_2, s32 *, -4) = 0xBF000000;
                            M2C_FIELD(var_a2_2, M2C_UNK **, 0) = (M2C_UNK *) (((temp_a3 & 0xF) * 0xA0000) | (((u32) (temp_a3 & 0xF0) >> 4) * 0xA00) | (((u32) (temp_a3 & 0xF00) >> 8) * 0xA));
                            goto loop_67;
                        case 0x8000:                /* switch 3 */
                            temp_v0_5 = M2C_FIELD(arg0, u8 *, 0x26);
                            temp_v1_6 = temp_a3 & 0x3FFF;
                            if (temp_v0_5 == 1) {
                                temp_a2 = var_a2_2 + 8;
                                temp_t2_8 = var_t2 + 0x18;
                                M2C_FIELD(temp_a2, s32 *, -4) = 0xFD500000;
                                var_a1_2 = 0x800;
                                var_a2_2[2] = arg0[3] + (temp_v1_6 * var_t9 * sp1C4);
                                temp_a2_2 = temp_a2 + 8;
                                M2C_FIELD(temp_a2_2, s32 *, -4) = 0xF5500000;
                                temp_a2[2] = (M2C_UNK *)0x07080200;
                                temp_a2_3 = temp_a2_2 + 8;
                                M2C_FIELD(temp_a2_3, s32 *, -4) = 0xE6000000;
                                temp_a2_2[2] = NULL;
                                temp_a2_4 = temp_a2_3 + 8;
                                M2C_FIELD(temp_a2_4, s32 *, -4) = 0xF3000000;
                                if (temp_t6 != 0) {
                                    var_a1_2 = temp_t6 + 0x7FF;
                                }
                                var_v1 = sp184 - 1;
                                if (var_v1 >= 0x800) {
                                    var_v1 = 0x7FF;
                                }
                                if (temp_t6 != 0) {
                                    var_v0 = (var_a1_2 / (s32) temp_t6) & 0xFFF;
                                } else {
                                    var_v0 = var_a1_2 & 0xFFF;
                                }
                                temp_t2_8[1] = ((var_v1 & 0xFFF) << 0xC) | 0x07000000 | var_v0;
                                temp_a2_5 = temp_a2_4 + 8;
                                M2C_FIELD(temp_a2_5, s32 *, -4) = 0xE7000000;
                                temp_a2_4[2] = NULL;
                                temp_a2_6 = temp_a2_5 + 8;
                                M2C_FIELD(temp_a2_6, s32 *, -4) = (s32) (((((s32) (sp176 + 7) >> 3) & 0x1FF) << 9) | 0xF5480000);
                                temp_a2_5[2] = (M2C_UNK *)0x80200;
                                var_a2_2 = temp_a2_6 + 8;
                                M2C_FIELD(var_a2_2, s32 *, -4) = 0xF2000000;
                                var_t2 = temp_t2_8 + 8 + 0x18;
                                temp_a2_6[2] = sp1BC;
                            } else {
                                temp_a2_7 = var_a2_2 + 8;
                                if (temp_v0_5 == 0) {
                                    temp_t2_9 = var_t2 + 0x18;
                                    M2C_FIELD(temp_a2_7, s32 *, -4) = 0xFD500000;
                                    var_t2_3 = temp_t2_9 + 8;
                                    var_a1_3 = 0x800;
                                    var_a2_2[2] = arg0[3] + ((temp_a3 & 0x3FFF) * var_t9 * sp1C4);
                                    temp_a2_8 = temp_a2_7 + 8;
                                    M2C_FIELD(temp_a2_8, s32 *, -4) = 0xF5500000;
                                    temp_a2_7[2] = (M2C_UNK *)0x07080200;
                                    temp_a2_9 = temp_a2_8 + 8;
                                    M2C_FIELD(temp_a2_9, s32 *, -4) = 0xE6000000;
                                    temp_a2_8[2] = NULL;
                                    temp_a2_10 = temp_a2_9 + 8;
                                    M2C_FIELD(temp_a2_10, s32 *, -4) = 0xF3000000;
                                    if (temp_t5 != 0) {
                                        var_a1_3 = temp_t5 + 0x7FF;
                                    }
                                    var_v1_2 = sp18C - 1;
                                    if (var_v1_2 >= 0x800) {
                                        var_v1_2 = 0x7FF;
                                    }
                                    if (temp_t5 != 0) {
                                        var_v0_2 = (var_a1_3 / (s32) temp_t5) & 0xFFF;
                                    } else {
                                        var_v0_2 = var_a1_3 & 0xFFF;
                                    }
                                    temp_t2_9[1] = ((var_v1_2 & 0xFFF) << 0xC) | 0x07000000 | var_v0_2;
                                    temp_a2_11 = temp_a2_10 + 8;
                                    M2C_FIELD(temp_a2_11, s32 *, -4) = 0xE7000000;
                                    temp_a2_10[2] = NULL;
                                    var_a2_3 = temp_a2_11 + 8;
                                    var_v0_3 = (((s32) ((sp176 >> 1) + 7) >> 3) & 0x1FF) << 9;
                                    var_v1_3 = 0xF5400000;
                                } else {
                                    temp_t2_10 = var_t2 + 0x18;
                                    M2C_FIELD(temp_a2_7, s32 *, -4) = 0xFD100000;
                                    var_t2_3 = temp_t2_10 + 8;
                                    var_a1_4 = sp176;
                                    var_a2_2[2] = arg0[3] + (temp_v1_6 * var_t9 * sp1C4 * 2);
                                    temp_a2_12 = temp_a2_7 + 8;
                                    M2C_FIELD(temp_a2_12, s32 *, -4) = 0xF5100000;
                                    temp_a2_7[2] = (M2C_UNK *)0x07080200;
                                    temp_a2_13 = temp_a2_12 + 8;
                                    M2C_FIELD(temp_a2_13, s32 *, -4) = 0xE6000000;
                                    temp_a2_12[2] = NULL;
                                    temp_a2_14 = temp_a2_13 + 8;
                                    M2C_FIELD(temp_a2_14, s32 *, -4) = 0xF3000000;
                                    if ((s32) sp176 < 0) {
                                        var_a1_4 = sp176 + 3;
                                    }
                                    temp_v0_6 = (s32) var_a1_4 >> 2;
                                    var_v1_4 = 0x800;
                                    if (temp_v0_6 > 0) {
                                        var_v1_4 = temp_v0_6 + 0x7FF;
                                    }
                                    var_a0_3 = temp_lo - 1;
                                    if (var_a0_3 >= 0x800) {
                                        var_a0_3 = 0x7FF;
                                    }
                                    var_a1_5 = sp176;
                                    if ((s32) sp176 < 0) {
                                        var_a1_5 = sp176 + 3;
                                    }
                                    temp_v0_7 = (s32) var_a1_5 >> 2;
                                    if (temp_v0_7 <= 0) {
                                        var_v0_4 = var_v1_4 & 0xFFF;
                                    } else {
                                        var_v0_4 = (var_v1_4 / temp_v0_7) & 0xFFF;
                                    }
                                    temp_t2_10[1] = ((var_a0_3 & 0xFFF) << 0xC) | 0x07000000 | var_v0_4;
                                    temp_a2_15 = temp_a2_14 + 8;
                                    M2C_FIELD(temp_a2_15, s32 *, -4) = 0xE7000000;
                                    temp_a2_14[2] = NULL;
                                    var_a2_3 = temp_a2_15 + 8;
                                    var_v0_3 = (((s32) ((sp176 * 2) + 7) >> 3) & 0x1FF) << 9;
                                    var_v1_3 = 0xF5100000;
                                }
                                M2C_FIELD(var_a2_3, s32 *, -4) = (s32) (var_v0_3 | var_v1_3);
                                M2C_FIELD(var_a2_3, M2C_UNK **, 0) = (M2C_UNK *)0x80200;
                                var_a2_2 = var_a2_3 + 8;
                                M2C_FIELD(var_a2_2, s32 *, -4) = 0xF2000000;
                                var_t2 = var_t2_3 + 0x18;
                                var_a2_3[2] = sp1BC;
                            }
                            goto loop_67;
                        case 0xC000:                /* switch 3 */
                            M2C_FIELD((var_a2_2 + 8), s32 *, -4) = 0xB8000000;
                            var_a2_2[2] = NULL;
                            break;
                        }
                    } else {
                        goto loop_16;
                    }
                } else {
                    switch (temp_v1_2) {            /* switch 2; irregular */
                    case 0x70616C34:                /* switch 2 */
                        M2C_FIELD(arg0, u8 *, 0x26) = 0U;
                        if (sp14C == 0) {
                            arg0[2] = var_s1;
                            var_a1_6 = (sp15C >> 1) - 1;
                            if (var_a1_6 != -1) {
                                do {
                                    temp_v0_8 = *var_t3_2;
                                    var_t3_2 += 4;
                                    var_a1_6 -= 1;
                                    *var_s1 = temp_v0_8;
                                    var_s1 += 4;
                                } while (var_a1_6 != -1);
                            }
                        }
                        break;
                    case 0x76727478:                /* switch 2 */
                        arg0[1] = var_s1;
                        var_t0 = var_s1;
                        if (sp17C == 1) {
                            var_a1_7 = 0;
                            if (var_s4 != 0) {
                                var_a0_4 = var_s1 + 0xF;
                                do {
                                    *var_t0 = (u16) M2C_FIELD(var_t3_2, u16 *, 0);
                                    M2C_FIELD(var_a0_4, s16 *, -0xB) = 0;
                                    M2C_FIELD(var_a0_4, s16 *, -9) = 0;
                                    M2C_FIELD(var_a0_4, u16 *, -0xD) = (u16) M2C_FIELD(var_t3_2, u16 *, 2);
                                    M2C_FIELD(var_a0_4, s8 *, -3) = 0;
                                    M2C_FIELD(var_a0_4, s8 *, -2) = 0;
                                    M2C_FIELD(var_a0_4, s8 *, -1) = 0x7F;
                                    M2C_FIELD(var_a0_4, s8 *, 0) = 0xFF;
                                    var_a1_7 += 1;
                                    M2C_FIELD(var_a0_4, s16 *, -7) = (s16) (((sp166 + *var_t0) << 6) + arg2);
                                    var_t3_2 = var_t3_2 + 2 + 2;
                                    var_t0 += 0x10;
                                    M2C_FIELD(var_a0_4, s16 *, -5) = (s16) (arg2 - ((sp16E + M2C_FIELD(var_a0_4, u16 *, -0xD)) << 6));
                                    var_a0_4 += 0x10;
                                } while (var_a1_7 < (u32) var_s4);
                                var_s1 = var_t0;
                            } else {
                                goto block_58;
                            }
                        } else {
                            var_a1_8 = 0;
                            if (((u32) var_s4 >> 2) != 0) {
                                do {
                                    temp_t2_11 = M2C_FIELD(var_t3_2, u16 *, 0);
                                    temp_t1 = M2C_FIELD(var_t3_2, u16 *, 2);
                                    var_t3_2 = var_t3_2 + 2 + 2;
                                    var_a3_3 = 0;
                                    var_a2_4 = &sp118;
                                    var_a0_5 = &sp120;
                                    var_v1_5 = var_t0 + 0xF;
loop_56:
                                    *var_t0 = (s16) (temp_t2_11 + (M2C_FIELD(var_a0_5, u8 *, 0) * sp176));
                                    M2C_FIELD(var_v1_5, s16 *, -0xB) = 0;
                                    M2C_FIELD(var_v1_5, s16 *, -9) = 0;
                                    M2C_FIELD(var_v1_5, s16 *, -0xD) = (s16) (temp_t1 - (M2C_FIELD(var_a0_5, u8 *, 1) * var_fp));
                                    M2C_FIELD(var_v1_5, s16 *, -7) = (s16) ((sp176 * M2C_FIELD(var_a2_4, u8 *, 0)) + arg2);
                                    temp_v0_9 = var_fp * M2C_FIELD(var_a2_4, u8 *, 1);
                                    var_a3_3 += 1;
                                    var_t0 += 0x10;
                                    var_a0_5 += 2;
                                    var_a2_4 += 2;
                                    M2C_FIELD(var_v1_5, s8 *, -3) = 0;
                                    M2C_FIELD(var_v1_5, s8 *, -2) = 0;
                                    M2C_FIELD(var_v1_5, s8 *, -1) = 0x7F;
                                    M2C_FIELD(var_v1_5, s8 *, 0) = 0xFF;
                                    M2C_FIELD(var_v1_5, s16 *, -5) = (s16) (temp_v0_9 + arg2);
                                    var_v1_5 += 0x10;
                                    if (var_a3_3 < 4) {
                                        goto loop_56;
                                    }
                                    var_a1_8 += 1;
                                } while (var_a1_8 < (u32) ((u32) var_s4 >> 2));
                            }
block_58:
                            var_s1 = var_t0;
                        }
                        break;
                    case 0x70616C38:                /* switch 2 */
                        M2C_FIELD(arg0, u8 *, 0x26) = 1U;
                        if (sp14C == 0) {
                            arg0[2] = var_s1;
                            var_a1_9 = (sp15C >> 1) - 1;
                            if (var_a1_9 != -1) {
                                do {
                                    temp_v0_10 = *var_t3_2;
                                    var_t3_2 += 4;
                                    var_a1_9 -= 1;
                                    *var_s1 = temp_v0_10;
                                    var_s1 += 4;
                                } while (var_a1_9 != -1);
                            }
                        }
                        break;
                    case 0x74696C65:                /* switch 2 */
                        var_a1_10 = (temp_s7 >> 2) - 1;
                        arg0[3] = var_s1;
                        if (var_a1_10 != -1) {
                            do {
                                temp_v0_11 = *var_t3_2;
                                var_t3_2 += 4;
                                var_a1_10 -= 1;
                                *var_s1 = temp_v0_11;
                                var_s1 += 4;
                            } while (var_a1_10 != -1);
                        }
                        break;
                    }
                    goto block_118;
                }
            } else {
block_117:
block_118:
                sp1B4 = var_t9;
                var_t3_2 = SiffIter_Leave(&sp10);
                if (sp10 != -1) {
                    goto loop_15;
                }
            }
            Heap_Free(&gGameHeap, sp154);
            return 0;
        }
        Heap_Free(&gGameHeap, sp154);
        return -1;
    default:                                        /* switch 1 */
        var_t3 = SiffIter_Leave(&sp10);
        if (sp10 == -1) {
            goto block_14;
        }
        goto loop_6;
    }
}

s32 Sprite_ParseScreen(M2C_UNK *arg0, M2C_UNK *arg1) {
    s32 sp18;
    M2C_UNK *sp124;
    M2C_UNK *temp_v0;
    M2C_UNK *temp_v0_2;
    M2C_UNK *temp_v0_3;
    M2C_UNK *temp_v0_4;
    M2C_UNK *var_a1;
    M2C_UNK *var_a1_2;
    M2C_UNK *var_s1;
    s16 temp_a1_9;
    s16 temp_a2;
    s16 var_fp;
    s16 var_s5;
    s16 var_s6;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_v1;
    s32 var_v1_2;
    s32 var_v1_3;
    s32 var_v1_4;
    u16 temp_v0_5;
    u16 var_s3;
    u16 var_s4;
    void *temp_a1;
    void *temp_a1_2;
    void *temp_a1_3;
    void *temp_a1_4;
    void *temp_a1_5;
    void *temp_a1_6;
    void *temp_a1_7;
    void *temp_a1_8;

    var_s1 = saved_reg_s1;
    var_s3 = saved_reg_s3;
    var_s4 = saved_reg_s4;
    var_s5 = saved_reg_s5;
    var_s6 = saved_reg_s6;
    var_fp = saved_reg_fp;
    SiffIter_Init(&sp18);
    sp124 = Asset_LoadLzss(arg1);
    var_a1 = sp124;
case 0x53696666:                                    /* switch 1 */
loop_1:
    temp_v1 = M2C_FIELD((sp + (sp18 * 0x10)), s32 *, 0x20);
    var_a1 = SiffIter_Enter(&sp18, var_a1);
    if (temp_v1 == 0x53703364) {
        goto loop_1;
    }
    switch (temp_v1) {                              /* switch 1; irregular */
    case 0x696E666F:                                /* switch 1 */
        var_s5 = M2C_FIELD(var_a1, s16 *, 0);
        temp_a1 = var_a1 + 2;
        temp_a1_2 = temp_a1 + 2;
        temp_a1_3 = temp_a1_2 + 2;
        var_s4 = M2C_FIELD(temp_a1_2, u16 *, 2);
        temp_a1_4 = temp_a1_3 + 2;
        var_s3 = M2C_FIELD(temp_a1_3, u16 *, 2);
        temp_a1_5 = temp_a1_4 + 2;
        M2C_FIELD(arg0, s16 *, 0x20) = (s16) (var_s4 * var_s3);
        var_s6 = M2C_FIELD(temp_a1_4, s16 *, 2);
        temp_a1_6 = temp_a1_5 + 2;
        var_fp = M2C_FIELD(temp_a1_5, s16 *, 2);
        temp_a1_7 = temp_a1_6 + 2;
        temp_a2 = M2C_FIELD(temp_a1_6, s16 *, 2);
        temp_a1_8 = temp_a1_7 + 2;
        M2C_FIELD(arg0, u16 *, 0x2A) = (u16) M2C_FIELD(temp_a1_7, u16 *, 2);
        M2C_FIELD(arg0, u16 *, 0x2C) = (u16) M2C_FIELD(temp_a1_8, u16 *, 2);
        temp_a1_9 = M2C_FIELD((temp_a1_8 + 2), s16 *, 2);
        M2C_FIELD(arg0, s8 *, 0x27) = (s8) temp_a1_9;
        if (temp_a1_9 & 2) {
            M2C_FIELD(arg0, u16 *, 0x22) = 0x20U;
            M2C_FIELD(arg0, u16 *, 0x24) = 0x20U;
        } else {
            M2C_FIELD(arg0, u16 *, 0x22) = (u16) M2C_FIELD(var_a1, u16 *, 2);
            M2C_FIELD(arg0, u16 *, 0x24) = (u16) M2C_FIELD(temp_a1, u16 *, 2);
        }
        if (!(temp_a1_9 & 4)) {
            Heap_Free(&gGameHeap, sp124, (s32) temp_a2);
            return -1;
        }
        temp_v0 = Heap_Alloc(&gGameHeap, (var_s5 * 2) + (var_s3 * (var_s4 * var_s6)) + (var_fp * 2) + ((temp_a2 + 0x18) * 8), (s32 *) temp_a2);
        M2C_FIELD(arg0, M2C_UNK **, 0) = temp_v0;
        var_s1 = temp_v0;
block_13:
        var_a1_2 = sp124;
loop_14:
        temp_v1_2 = M2C_FIELD((sp + (sp18 * 0x10)), s32 *, 0x20);
        var_a1_2 = SiffIter_Enter(&sp18, var_a1_2);
        if (temp_v1_2 != 0x696E666F) {
            if (temp_v1_2 > 0x696E666F) {
                switch (temp_v1_2) {                /* switch 2; irregular */
                case 0x70616C34:                    /* switch 2 */
                    var_v1 = ((u32) var_s5 >> 1) - 1;
                    M2C_FIELD(arg0, s8 *, 0x26) = 0;
                    arg0[2] = var_s1;
                    if (var_v1 != -1) {
                        do {
                            temp_v0_2 = *var_a1_2;
                            var_a1_2 += 4;
                            var_v1 -= 1;
                            *var_s1 = temp_v0_2;
                            var_s1 += 4;
                        } while (var_v1 != -1);
                    }
                    break;
                case 0x70616C38:                    /* switch 2 */
                    var_v1_2 = ((u32) var_s5 >> 1) - 1;
                    M2C_FIELD(arg0, s8 *, 0x26) = 1;
                    arg0[2] = var_s1;
                    if (var_v1_2 != -1) {
                        do {
                            temp_v0_3 = *var_a1_2;
                            var_a1_2 += 4;
                            var_v1_2 -= 1;
                            *var_s1 = temp_v0_3;
                            var_s1 += 4;
                        } while (var_v1_2 != -1);
                    }
                    break;
                case 0x74696C65:                    /* switch 2 */
                    var_v1_3 = ((u32) (var_s3 * var_s4 * var_s6) >> 2) - 1;
                    arg0[3] = var_s1;
                    if (var_v1_3 != -1) {
                        do {
                            temp_v0_4 = *var_a1_2;
                            var_a1_2 += 4;
                            var_v1_3 -= 1;
                            *var_s1 = temp_v0_4;
                            var_s1 += 4;
                        } while (var_v1_3 != -1);
                    }
                    break;
                }
                goto block_46;
            }
            if (temp_v1_2 != 0x53703364) {
                if (temp_v1_2 <= 0x53703364) {
                    if (temp_v1_2 != 0x53696666) {
                        goto block_46;
                    }
                    goto loop_14;
                }
                var_v1_4 = var_fp - 1;
                if (temp_v1_2 != 0x646C7374) {
                    goto block_46;
                }
                arg0[7] = var_s1;
                if (var_v1_4 != -1) {
                    do {
                        temp_v0_5 = *var_a1_2;
                        var_a1_2 += 2;
                        var_v1_4 -= 1;
                        *var_s1 = temp_v0_5;
                        var_s1 += 2;
                    } while (var_v1_4 != -1);
                }
                arg0[5] = (s32) (var_s1 + 7) & ~7;
                Sprite_DrawScreen(M2C_BITWISE(f32, arg0), 0.0f, 0.0f, 1.0f, 1.0f);
            } else {
                goto loop_14;
            }
        } else {
block_46:
            var_a1_2 = SiffIter_Leave(&sp18, var_a1_2);
            if (sp18 != -1) {
                goto loop_14;
            }
        }
        Heap_Free(&gGameHeap, sp124);
        return 0;
    default:                                        /* switch 1 */
        var_a1 = SiffIter_Leave(&sp18, var_a1);
        if (sp18 == -1) {
            goto block_13;
        }
        goto loop_1;
    }
}

void Sprite_DrawScreen(void *arg0, f32 arg1, f32 arg2, f32 arg3, f32 arg4) {
    f32 temp_f14;
    f32 temp_f16;
    f32 temp_f4;
    f32 temp_f8;
    f64 temp_f2;
    f64 temp_f2_4;
    f64 temp_f6;
    s32 temp_f0;
    s32 temp_f0_2;
    s32 temp_f2_2;
    s32 temp_f2_3;
    s32 temp_f6_2;
    s32 temp_f6_3;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v1;
    s32 var_a0;
    s32 var_a3;
    s32 var_a3_2;
    s32 var_v0;
    s32 var_v0_3;
    s32 var_v0_4;
    s32 var_v0_5;
    s32 var_v0_6;
    s32 var_v1;
    s32 var_v1_2;
    s32 var_v1_3;
    s32 var_v1_4;
    s32 var_v1_5;
    u16 *var_t5;
    u16 temp_a0;
    u16 temp_a0_4;
    u16 temp_v1_2;
    u16 temp_v1_3;
    u16 var_v0_2;
    u32 temp_a0_2;
    u32 temp_a0_3;
    void *temp_a1;
    void *temp_a1_10;
    void *temp_a1_11;
    void *temp_a1_2;
    void *temp_a1_3;
    void *temp_a1_4;
    void *temp_a1_5;
    void *temp_a1_6;
    void *temp_a1_7;
    void *temp_a1_8;
    void *temp_a1_9;
    void *temp_t3;
    void *temp_t3_10;
    void *temp_t3_11;
    void *temp_t3_12;
    void *temp_t3_13;
    void *temp_t3_14;
    void *temp_t3_2;
    void *temp_t3_3;
    void *temp_t3_4;
    void *temp_t3_5;
    void *temp_t3_6;
    void *temp_t3_7;
    void *temp_t3_8;
    void *temp_t3_9;
    void *var_a1;
    void *var_a1_2;
    void *var_t3;
    void *var_t3_2;

    temp_t3 = M2C_FIELD(arg0, void **, 0x14);
    var_t5 = M2C_FIELD(arg0, u16 **, 0x1C);
    M2C_FIELD(arg0, M2C_UNK **, 0x10) = &gSpriteRectSetupDL;
    M2C_FIELD(arg0, M2C_UNK **, 0x18) = &gSpriteRectFinishDL;
    if (M2C_FIELD(arg0, u8 *, 0x26) == 1) {
        temp_t3_2 = temp_t3 + 8;
        temp_t3_3 = temp_t3_2 + 8;
        temp_t3_4 = temp_t3_3 + 8;
        M2C_FIELD(temp_t3, s32 *, 0) = 0xFD100000;
        M2C_FIELD(temp_t3, s32 *, 4) = (s32) M2C_FIELD(arg0, s32 *, 8);
        M2C_FIELD(temp_t3, s32 *, 8) = 0xE8000000;
        M2C_FIELD(temp_t3_2, s32 *, 4) = 0;
        M2C_FIELD(temp_t3_3, s32 *, 4) = 0x02000000;
        M2C_FIELD(temp_t3_2, s32 *, 8) = 0xF5000100;
        M2C_FIELD(temp_t3_3, s32 *, 8) = 0xE6000000;
        M2C_FIELD(temp_t3_4, s32 *, 4) = 0;
        temp_t3_5 = temp_t3_4 + 8;
        var_t3 = temp_t3_5 + 8;
        M2C_FIELD(temp_t3_3, s32 *, 8) = 0xF0000000;
        M2C_FIELD(temp_t3_4, s32 *, 4) = 0x023FC000;
        M2C_FIELD(temp_t3_4, s32 *, 8) = 0xE7000000;
        M2C_FIELD(temp_t3_5, s32 *, 4) = 0;
    } else {
        temp_t3_6 = temp_t3 + 8;
        temp_t3_7 = temp_t3_6 + 8;
        temp_t3_8 = temp_t3_7 + 8;
        temp_t3_9 = temp_t3_8 + 8;
        temp_t3_10 = temp_t3_9 + 8;
        M2C_FIELD(temp_t3, s32 *, 0) = 0xFD100000;
        var_t3 = temp_t3_10 + 8;
        M2C_FIELD(temp_t3, s32 *, 4) = (s32) M2C_FIELD(arg0, s32 *, 8);
        M2C_FIELD(temp_t3, s32 *, 8) = 0xE8000000;
        M2C_FIELD(temp_t3_6, s32 *, 4) = 0;
        M2C_FIELD(temp_t3_7, s32 *, 4) = 0x02000000;
        M2C_FIELD(temp_t3_6, s32 *, 8) = 0xF5000100;
        M2C_FIELD(temp_t3_7, s32 *, 8) = 0xE6000000;
        M2C_FIELD(temp_t3_8, s32 *, 4) = 0;
        M2C_FIELD(temp_t3_8, s32 *, 8) = 0xF0000000;
        M2C_FIELD(temp_t3_9, s32 *, 4) = 0x0203C000;
        M2C_FIELD(temp_t3_9, s32 *, 8) = 0xE7000000;
        M2C_FIELD(temp_t3_10, s32 *, 4) = 0;
    }
    var_a1 = var_t3 - 4;
    temp_f16 = (f32) (1024.0 / (f64) arg4);
    temp_f0 = (s32) temp_f16;
    temp_f14 = (f32) (1024.0 / (f64) (f32) ((f64) arg3 * ((f64) (f32) gScreenWidth / 320.0)));
    temp_f0_2 = (s32) temp_f14;
loop_4:
    temp_a0 = *var_t5;
    var_t5 += 2;
    temp_v1 = temp_a0 & 0xC000;
    if (temp_v1 == 0x8000) {
        temp_a1 = var_a1 + 8;
        if (M2C_FIELD(arg0, u8 *, 0x26) == 1) {
            M2C_FIELD(temp_a1, s32 *, -4) = 0xFD500000;
            temp_t3_11 = var_t3 + 0x18;
            M2C_FIELD(var_a1, s32 *, 8) = (s32) (M2C_FIELD(arg0, s32 *, 0xC) + ((temp_a0 & 0x3FFF) * M2C_FIELD(arg0, u16 *, 0x20)));
            temp_a1_2 = temp_a1 + 8;
            M2C_FIELD(temp_a1_2, s32 *, -4) = 0xF5500000;
            M2C_FIELD(temp_a1, s32 *, 8) = 0x07080200;
            temp_a1_3 = temp_a1_2 + 8;
            M2C_FIELD(temp_a1_3, s32 *, -4) = 0xE6000000;
            M2C_FIELD(temp_a1_2, s32 *, 8) = 0;
            temp_a1_4 = temp_a1_3 + 8;
            M2C_FIELD(temp_a1_4, s32 *, -4) = 0xF3000000;
            temp_v1_2 = M2C_FIELD(arg0, u16 *, 0x22);
            var_t3_2 = temp_t3_11 + 8;
            temp_a0_2 = temp_v1_2 >> 3;
            var_a3 = 0x800;
            if (temp_a0_2 != 0) {
                var_a3 = temp_a0_2 + 0x7FF;
            }
            var_v1 = ((s32) ((temp_v1_2 * M2C_FIELD(arg0, u16 *, 0x24)) + 1) >> 1) - 1;
            if (var_v1 >= 0x800) {
                var_v1 = 0x7FF;
            }
            if (temp_a0_2 != 0) {
                var_v0 = (var_a3 / (s32) temp_a0_2) & 0xFFF;
            } else {
                var_v0 = var_a3 & 0xFFF;
            }
            temp_a1_5 = temp_a1_4 + 8;
            M2C_FIELD(temp_t3_11, s32 *, 4) = (s32) (((var_v1 & 0xFFF) << 0xC) | 0x07000000 | var_v0);
            M2C_FIELD(temp_a1_5, s32 *, -4) = 0xE7000000;
            M2C_FIELD(temp_a1_4, s32 *, 8) = 0;
            var_a1_2 = temp_a1_5 + 8;
            var_v0_2 = M2C_FIELD(arg0, u16 *, 0x22);
            M2C_FIELD(temp_a1_5, s32 *, 8) = 0x80200;
            var_v1_2 = 0xF5480000;
        } else {
            M2C_FIELD(temp_a1, s32 *, -4) = 0xFD500000;
            temp_t3_12 = var_t3 + 0x18;
            M2C_FIELD(var_a1, s32 *, 8) = (s32) (M2C_FIELD(arg0, s32 *, 0xC) + ((temp_a0 & 0x3FFF) * M2C_FIELD(arg0, u16 *, 0x20)));
            temp_a1_6 = temp_a1 + 8;
            M2C_FIELD(temp_a1_6, s32 *, -4) = 0xF5500000;
            M2C_FIELD(temp_a1, s32 *, 8) = 0x07080200;
            temp_a1_7 = temp_a1_6 + 8;
            M2C_FIELD(temp_a1_7, s32 *, -4) = 0xE6000000;
            M2C_FIELD(temp_a1_6, s32 *, 8) = 0;
            temp_a1_8 = temp_a1_7 + 8;
            M2C_FIELD(temp_a1_8, s32 *, -4) = 0xF3000000;
            temp_v1_3 = M2C_FIELD(arg0, u16 *, 0x22);
            var_t3_2 = temp_t3_12 + 8;
            temp_a0_3 = temp_v1_3 >> 4;
            var_a3_2 = 0x800;
            if (temp_a0_3 != 0) {
                var_a3_2 = temp_a0_3 + 0x7FF;
            }
            var_v1_3 = ((s32) ((temp_v1_3 * M2C_FIELD(arg0, u16 *, 0x24)) + 3) >> 2) - 1;
            if (var_v1_3 >= 0x800) {
                var_v1_3 = 0x7FF;
            }
            if (temp_a0_3 != 0) {
                var_v0_3 = (var_a3_2 / (s32) temp_a0_3) & 0xFFF;
            } else {
                var_v0_3 = var_a3_2 & 0xFFF;
            }
            temp_a1_9 = temp_a1_8 + 8;
            M2C_FIELD(temp_t3_12, s32 *, 4) = (s32) (((var_v1_3 & 0xFFF) << 0xC) | 0x07000000 | var_v0_3);
            M2C_FIELD(temp_a1_9, s32 *, -4) = 0xE7000000;
            M2C_FIELD(temp_a1_8, s32 *, 8) = 0;
            var_a1_2 = temp_a1_9 + 8;
            M2C_FIELD(temp_a1_9, s32 *, 8) = 0x80200;
            var_v1_2 = 0xF5400000;
            var_v0_2 = (u16) ((u16) M2C_FIELD(arg0, u16 *, 0x22) >> 1);
        }
        M2C_FIELD(var_a1_2, s32 *, -4) = (s32) (((((s32) (var_v0_2 + 7) >> 3) & 0x1FF) << 9) | var_v1_2);
        var_a1 = var_a1_2 + 8;
        M2C_FIELD(var_a1, s32 *, -4) = 0xF2000000;
        var_t3 = var_t3_2 + 0x18;
        M2C_FIELD(var_a1_2, s32 *, 8) = (s32) (((((M2C_FIELD(arg0, u16 *, 0x22) - 1) * 4) & 0xFFF) << 0xC) | (((M2C_FIELD(arg0, u16 *, 0x24) - 1) * 4) & 0xFFF));
        goto loop_4;
    }
    switch (temp_v1) {                              /* irregular */
    case 0x0:
        temp_a0_4 = *var_t5;
        temp_f4 = (f32) ((temp_a0 & 0x1FF) - M2C_FIELD(arg0, s16 *, 0x2A));
        temp_f6 = (f64) (f32) gScreenWidth / 320.0;
        temp_f8 = (f32) ((temp_a0_4 & 0x1FF) + M2C_FIELD(arg0, s16 *, 0x2C));
        temp_f2 = ((f64) (arg2 + ((temp_f8 + (f32) ((((temp_a0_4 >> 9) & 0x1F) + 1) * 2)) * arg4)) * 4.0) + 0.0;
        var_t5 += 2;
        temp_a1_10 = var_a1 + 8;
        temp_t3_13 = var_t3 + 8;
        temp_t3_14 = temp_t3_13 + 8;
        temp_f6_2 = (s32) (((f64) (arg1 + ((temp_f4 + (f32) (((((s32) (temp_a0 << 0x10) >> 0x19) & 0x1F) + 1) * 2)) * arg3)) * temp_f6 * 4.0) + 0.0);
        temp_f6_3 = (s32) temp_f2;
        M2C_FIELD(temp_a1_10, s32 *, -4) = (s32) (((temp_f6_2 & ((s32) ~(s16) temp_f6_2 >> 0x1F) & 0xFFF) << 0xC) | ((temp_f6_3 & ((s32) ~(s16) temp_f6_3 >> 0x1F) & 0xFFF) | 0xE4000000));
        temp_f2_2 = (s32) (((f64) (arg1 + (temp_f4 * arg3)) * temp_f6 * 4.0) + 0.0);
        temp_f2_3 = (s32) (((f64) (arg2 + (temp_f8 * arg4)) * 4.0) + 0.0);
        M2C_FIELD(var_a1, s32 *, 8) = (s32) (((temp_f2_2 & ((s32) ~(s16) temp_f2_2 >> 0x1F) & 0xFFF) << 0xC) | (temp_f2_3 & ((s32) ~(s16) temp_f2_3 >> 0x1F) & 0xFFF));
        temp_a1_11 = temp_a1_10 + 8;
        M2C_FIELD(temp_a1_11, s32 *, -4) = 0xB4000000;
        if ((s16) temp_f2_2 < 0) {
            if ((s16) temp_f0_2 < 0) {
                var_v0_4 = (s32) ((s16) temp_f2_2 * (s16) temp_f0_2) >> 7;
                var_v1_4 = (s32) ~var_v0_4 >> 0x1F;
            } else {
                var_v0_4 = (s32) ((s16) temp_f2_2 * (s16) temp_f0_2) >> 7;
                var_v1_4 = -(var_v0_4 < 1);
            }
            var_a0 = (0x20 - (var_v0_4 & var_v1_4)) << 0x10;
        } else {
            var_a0 = 0x20 << 0x10;
        }
        temp_f2_4 = ((f64) (arg2 + (temp_f8 * arg4)) * 4.0) + 0.0;
        var_v0_5 = var_a0 | 0x20;
        if (temp_f2_4 < 0.0) {
            if ((s16) temp_f0 < 0) {
                temp_v0 = (s32) ((s16) (s32) temp_f2_4 * (s16) temp_f0) >> 7;
                var_v0_5 = var_a0 | ((0x20 - (temp_v0 & ((s32) ~temp_v0 >> 0x1F))) & 0xFFFF);
            } else {
                temp_v0_2 = (s32) ((s16) (s32) temp_f2_4 * (s16) temp_f0) >> 7;
                var_v0_5 = var_a0 | ((0x20 - (temp_v0_2 & -(temp_v0_2 < 1))) & 0xFFFF);
            }
        }
        M2C_FIELD(temp_t3_13, s32 *, 4) = var_v0_5;
        var_a1 = temp_a1_11 + 8;
        var_t3 = temp_t3_14 + 8;
        M2C_FIELD(var_a1, s32 *, -4) = 0xB3000000;
        if (!(temp_f14 >= 2.1474836e9f)) {
            var_v1_5 = (s32) temp_f14;
        } else {
            var_v1_5 = (s32) (temp_f14 - 2.1474836e9f) | 0x80000000;
        }
        if (!(temp_f16 >= 2.1474836e9f)) {
            var_v0_6 = (s32) temp_f16 & 0xFFFF;
        } else {
            var_v0_6 = ((s32) (temp_f16 - 2.1474836e9f) | 0x80000000) & 0xFFFF;
        }
        M2C_FIELD(temp_t3_14, s32 *, 4) = (s32) ((var_v1_5 << 0x10) | var_v0_6);
        goto loop_4;
    case 0xC000:
        M2C_FIELD((var_a1 + 8), s32 *, -4) = 0xB8000000;
        M2C_FIELD(var_a1, s32 *, 8) = 0;
        return;
    }
}

M2C_UNK *PhotoView_New(M2C_UNK *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4, s32 arg5) {
    M2C_UNK *var_s1;

    var_s1 = arg0;
    if (var_s1 == NULL) {
        var_s1 = Heap_Alloc(&gGameHeap, 0x1C);
        bzero(var_s1, 0x1C);
        M2C_FIELD(var_s1, M2C_UNK **, 0) = Heap_Alloc(&gGameHeap, 0xD00);
        var_s1[3] = (M2C_UNK *)1;
    } else {
        bzero(var_s1, 0x1C);
        var_s1[3] = NULL;
    }
    if (var_s1 != NULL) {
        var_s1[6] = arg1;
        var_s1[4] = arg2;
        var_s1[5] = arg3;
        PhotoView_Build(var_s1, arg4, arg5);
    }
    return var_s1;
}

s32 PhotoView_Delete(M2C_UNK *arg0) {
    M2C_UNK *temp_s0;

    temp_s0 = arg0[3];
    Heap_Free(&gGameHeap, M2C_FIELD(arg0, M2C_UNK **, 0));
    if (temp_s0 != NULL) {
        Heap_Free(&gGameHeap, arg0);
    }
    return 0;
}

void PhotoView_SetScroll(void *arg0, s32 arg1, s32 arg2) {
    s32 temp_v1;
    s32 temp_v1_2;
    s32 var_a1;
    s32 var_a2;

    var_a1 = arg1;
    var_a2 = arg2;
    if (var_a1 < 0) {
        var_a1 = 0;
    } else {
        temp_v1 = M2C_FIELD(arg0, s32 *, 0x10) - 0x140;
        if (temp_v1 < var_a1) {
            var_a1 = temp_v1;
        }
    }
    if (var_a2 < 0) {
        var_a2 = 0;
    } else {
        temp_v1_2 = M2C_FIELD(arg0, s32 *, 0x14) - 0xF0;
        if (temp_v1_2 < var_a2) {
            var_a2 = temp_v1_2;
        }
    }
    PhotoView_Build((M2C_UNK *) var_a1, var_a2);
}

void PhotoView_Build(M2C_UNK *arg0, s32 arg1, s32 arg2) {
    M2C_UNK *var_t0;
    s16 temp_a2_2;
    s16 temp_v1;
    s32 temp_a2;
    s32 temp_t9;
    s32 temp_v0;
    s32 temp_v1_2;
    s32 var_a0;
    s32 var_t1;
    s32 var_t2;
    s32 var_t4;
    s32 var_t5;
    s32 var_t6;
    void *temp_a3;
    void *temp_a3_2;
    void *temp_a3_3;
    void *temp_a3_4;
    void *temp_a3_5;
    void *temp_a3_6;
    void *temp_a3_7;
    void *temp_a3_8;
    void *temp_a3_9;
    void *temp_t0;
    void *temp_t0_2;
    void *var_a3;

    var_t0 = M2C_FIELD(arg0, M2C_UNK **, 0);
    var_t2 = 0;
    temp_a2 = arg1 & 3;
    temp_v1 = ((-temp_a2 + 0x140) * 4) + 1;
    temp_a2_2 = (temp_a2 * -4) + 1;
    var_t1 = 1;
    var_t6 = 0x10000;
    var_t5 = 0x190000;
    temp_t9 = arg0[4];
    var_t4 = 0x19;
    arg0[1] = &gPhotoSetupDL;
    arg0[2] = &gPhotoFinishDL;
    var_a3 = var_t0 - 4;
    do {
        temp_a3 = var_a3 + 8;
        temp_t0 = var_t0 + 0x40;
        temp_t0_2 = temp_t0 + 8;
        M2C_FIELD(temp_a3, s32 *, -4) = (s32) (((temp_t9 - 1) & 0xFFF) | 0xFD100000);
        M2C_FIELD(var_a3, s32 *, 8) = (s32) (arg0[6] + ((arg1 & ~3) * 2) + ((arg2 + var_t2) * temp_t9 * 2));
        temp_a3_2 = temp_a3 + 8;
        M2C_FIELD(temp_a3_2, s32 *, -4) = 0xF510A200;
        M2C_FIELD(temp_a3, s32 *, 8) = 0x07080200;
        temp_a3_3 = temp_a3_2 + 8;
        M2C_FIELD(temp_a3_3, s32 *, -4) = 0xE6000000;
        M2C_FIELD(temp_a3_2, s32 *, 8) = 0;
        temp_a3_4 = temp_a3_3 + 8;
        M2C_FIELD(temp_a3_4, s32 *, -4) = 0xF4000000;
        M2C_FIELD(temp_a3_3, s32 *, 8) = 0x07500018;
        temp_a3_5 = temp_a3_4 + 8;
        M2C_FIELD(temp_a3_5, s32 *, -4) = 0xE7000000;
        M2C_FIELD(temp_a3_4, s32 *, 8) = 0;
        temp_a3_6 = temp_a3_5 + 8;
        M2C_FIELD(temp_a3_6, s32 *, -4) = 0xF510A200;
        M2C_FIELD(temp_a3_5, s32 *, 8) = 0x80200;
        temp_a3_7 = temp_a3_6 + 8;
        M2C_FIELD(temp_a3_7, s32 *, -4) = 0xF2000000;
        M2C_FIELD(temp_a3_6, s32 *, 8) = 0x500018;
        temp_a3_8 = temp_a3_7 + 8;
        M2C_FIELD(temp_a3_8, s32 *, -4) = (s32) (((temp_v1 & ((s32) ~temp_v1 >> 0x1F) & 0xFFF) << 0xC) | ((var_t4 & ((s32) ~(var_t5 >> 0x10) >> 0x1F) & 0xFFF) | 0xE4000000));
        M2C_FIELD(temp_a3_7, s32 *, 8) = (s32) (((temp_a2_2 & ((s32) ~temp_a2_2 >> 0x1F) & 0xFFF) << 0xC) | (var_t1 & ((s32) ~(var_t6 >> 0x10) >> 0x1F) & 0xFFF));
        temp_a3_9 = temp_a3_8 + 8;
        M2C_FIELD(temp_a3_9, s32 *, -4) = 0xB4000000;
        if (temp_a2_2 < 0) {
            temp_v1_2 = (s32) (temp_a2_2 << 0xA) >> 7;
            var_a0 = (temp_v1_2 & -(temp_v1_2 < 1)) * -0x10000;
        } else {
            var_a0 = 0;
        }
        M2C_FIELD(temp_t0, s32 *, 4) = var_a0;
        if (var_t1 < 0) {
            temp_v0 = (s32) (var_t1 << 0x10) >> 0xD;
            M2C_FIELD(temp_t0, s32 *, 4) = (s32) (var_a0 | (-(temp_v0 & -(temp_v0 < 1)) & 0xFFFF));
        }
        var_a3 = temp_a3_9 + 8;
        var_t0 = temp_t0_2 + 8;
        var_t1 += 0x18;
        var_t6 += 0x180000;
        var_t5 += 0x180000;
        var_t4 += 0x18;
        var_t2 += 6;
        M2C_FIELD(var_a3, s32 *, -4) = 0xB3000000;
        M2C_FIELD(temp_a3_9, s32 *, 8) = 0x04000400;
    } while (var_t2 < 0xF0);
    M2C_FIELD(temp_t0_2, s32 *, 8) = 0xB8000000;
    var_t0[1] = NULL;
}

void Image_Mosaic(s32 arg0, s32 arg1, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, f32 arg8) {
    f32 *temp_a3;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 var_f0_11;
    f32 var_f0_3;
    f32 var_f0_7;
    f64 var_f0;
    f64 var_f0_10;
    f64 var_f0_12;
    f64 var_f0_2;
    f64 var_f0_4;
    f64 var_f0_5;
    f64 var_f0_6;
    f64 var_f0_8;
    f64 var_f0_9;
    s16 *temp_a1;
    s16 *temp_t0;
    s16 temp_v1;
    s32 var_a0;
    s32 var_a2;
    s32 var_a2_2;
    s32 var_a2_3;
    s32 var_t0;
    s32 var_t1;
    s32 var_t1_2;
    s32 var_t2;
    s32 var_t4;
    s32 var_t6;
    s32 var_t7;
    s32 var_t9;
    s32 var_v0;
    u16 temp_v1_2;
    u32 temp_v1_3;

    var_a0 = arg7;
    if (var_a0 >= 6) {
        var_a0 = 5;
    }
    var_t9 = arg4;
    if (var_t9 < (var_t9 + arg6)) {
        temp_a1 = &D_80038A78[var_a0];
        temp_a3 = &D_80038A88[var_a0];
        var_t7 = arg3;
        do {
            var_t6 = 0;
            if (var_t7 < (var_t7 + arg5)) {
                do {
                    var_t4 = 0;
                    var_t2 = 0;
                    var_t1 = 0;
                    if (*temp_a1 > 0) {
                        do {
                            temp_v1 = D_80038A78[var_a0];
                            var_a2 = 0;
                            if (temp_v1 > 0) {
                                var_t0 = (var_t7 * 2) + arg0;
                                do {
                                    temp_v1_2 = *(((var_t9 + var_t1) * arg1 * 2) + var_t0);
                                    var_t0 += 2;
                                    var_a2 += 1;
                                    temp_v1_3 = temp_v1_2 << 0x10;
                                    var_t2 += temp_v1_3 >> 0x1B;
                                    var_t4 += ((s32) temp_v1_3 >> 0x16) & 0x1F;
                                    var_t6 += ((s32) temp_v1_3 >> 0x11) & 0x1F;
                                } while (var_a2 < temp_v1);
                            }
                            var_t1 += 1;
                        } while (var_t1 < D_80038A78[var_a0]);
                    }
                    var_f0 = (f64) var_t2;
                    if (var_t2 < 0) {
                        var_f0 += 4294967296.0;
                    }
                    temp_f2 = *temp_a3;
                    if ((((f32) var_f0 / temp_f2) * arg8) < 0.0f) {
                        var_f0_2 = (f64) var_t2;
                        if (var_t2 < 0) {
                            var_f0_2 += 4294967296.0;
                        }
                        var_f0_3 = (((f32) var_f0_2 / temp_f2) * arg8) - 0.5f;
                    } else {
                        var_f0_4 = (f64) var_t2;
                        if (var_t2 < 0) {
                            var_f0_4 += 4294967296.0;
                        }
                        var_f0_3 = (((f32) var_f0_4 / temp_f2) * arg8) + 0.5f;
                    }
                    var_f0_5 = (f64) var_t4;
                    if (var_t4 < 0) {
                        var_f0_5 += 4294967296.0;
                    }
                    temp_f2_2 = *temp_a3;
                    if ((((f32) var_f0_5 / temp_f2_2) * arg8) < 0.0f) {
                        var_f0_6 = (f64) var_t4;
                        if (var_t4 < 0) {
                            var_f0_6 += 4294967296.0;
                        }
                        var_f0_7 = (((f32) var_f0_6 / temp_f2_2) * arg8) - 0.5f;
                    } else {
                        var_f0_8 = (f64) var_t4;
                        if (var_t4 < 0) {
                            var_f0_8 += 4294967296.0;
                        }
                        var_f0_7 = (((f32) var_f0_8 / temp_f2_2) * arg8) + 0.5f;
                    }
                    var_f0_9 = (f64) var_t6;
                    if (var_t6 < 0) {
                        var_f0_9 += 4294967296.0;
                    }
                    temp_f2_3 = *temp_a3;
                    if ((((f32) var_f0_9 / temp_f2_3) * arg8) < 0.0f) {
                        var_f0_10 = (f64) var_t6;
                        if (var_t6 < 0) {
                            var_f0_10 += 4294967296.0;
                        }
                        var_f0_11 = (((f32) var_f0_10 / temp_f2_3) * arg8) - 0.5f;
                    } else {
                        var_f0_12 = (f64) var_t6;
                        if (var_t6 < 0) {
                            var_f0_12 += 4294967296.0;
                        }
                        var_f0_11 = (((f32) var_f0_12 / temp_f2_3) * arg8) + 0.5f;
                    }
                    var_a2_2 = var_a0 * 2;
                    var_t1_2 = 0;
                    if (*temp_a1 > 0) {
                        do {
                            temp_t0 = var_a2_2 + D_80038A78;
                            var_a2_3 = 0;
                            if (*temp_t0 > 0) {
                                var_v0 = var_t7;
                                do {
                                    *(((var_t9 + var_t1_2) * arg1 * 2) + ((var_v0 * 2) + arg0)) = ((s32) var_f0_3 << 0xB) + ((s32) var_f0_7 << 6) + ((s32) var_f0_11 * 2) + 1;
                                    var_a2_3 += 1;
                                    var_v0 = var_t7 + var_a2_3;
                                } while (var_a2_3 < *temp_t0);
                            }
                            var_a2_2 = var_a0 * 2;
                            var_t1_2 += 1;
                        } while (var_t1_2 < D_80038A78[var_a0]);
                    }
                    var_t7 += *temp_a1;
                    var_t6 = 0;
                } while (var_t7 < (arg3 + arg5));
            }
            var_t9 += *temp_a1;
            var_t7 = arg3;
        } while (var_t9 < (arg4 + arg6));
    }
}

void Image_CopyRect(s32 arg0, s32 arg1, s32 arg3, s32 arg4, s32 arg5, s32 arg6, s32 arg7, s32 arg8, s32 arg10, s32 arg11) {
    s32 temp_v0;
    s32 var_a2;
    s32 var_t0;
    s32 var_t1;
    s32 var_t2;

    var_t2 = 0;
    if (arg6 > 0) {
        do {
            var_t1 = 0;
            if (arg5 > 0) {
                var_t0 = arg0;
                var_a2 = arg7;
                do {
                    temp_v0 = (arg3 * 2) + var_t0;
                    var_t0 += 2;
                    var_t1 += 1;
                    *(((var_t2 + arg11) * arg8 * 2) + ((arg10 * 2) + var_a2)) = *(((var_t2 + arg4) * arg1 * 2) + temp_v0);
                    var_a2 += 2;
                } while (var_t1 < arg5);
            }
            var_t2 += 1;
        } while (var_t2 < arg6);
    }
}

s32 func_80014E90(void) {
    osCreateMesgQueue((s16 *(*)[]) D_800B0520, &D_800C5328, (s16 *)1);
    return 0;
}

s32 func_80014EC0(s32 arg0, s32 arg1) {
    u8 sp20;
    u32 sp24;
    s32 var_s2;
    struct _struct_D_800A6990_0x68 *temp_s1;
    u32 temp_v0;

    func_8000F7A4();
    var_s2 = 0;
    if (func_80024360(D_800C30A8, &sp20) != 0) {
        if (!(sp20 & (1 << arg0))) {
            var_s2 = -1;
        } else {
            goto block_4;
        }
    } else {
block_4:
        temp_s1 = &D_800A6990[arg0];
        if (func_80024A30(D_800C30A8, temp_s1, arg0) != 0) {
            var_s2 = -1;
        } else {
            temp_v0 = func_800230D0(temp_s1, 1U, 1, D_80038AB0, &D_80038AC0, arg1, &sp24);
            if (temp_v0 != 0) {
                func_8001547C(temp_v0, temp_s1, "pfsCreate");
            }
        }
    }
    func_8000F75C();
    return var_s2;
}

s32 func_80014FC0(s32 arg0, u8 *arg1, s32 arg2, s32 arg3) {
    u8 sp18;
    u32 sp1C;
    s32 var_s2;
    struct _struct_D_800A6990_0x68 *temp_s1;
    u32 temp_v0;
    u32 temp_v0_2;

    func_8000F7A4();
    var_s2 = 0;
    if (func_80024360(D_800C30A8, &sp18) != 0) {
        if (!(sp18 & (1 << arg0))) {
            var_s2 = -1;
        } else {
            goto block_4;
        }
    } else {
block_4:
        temp_s1 = &D_800A6990[arg0];
        if (func_80024A30(D_800C30A8, temp_s1, arg0) != 0) {
            var_s2 = -1;
        } else {
            temp_v0 = osPfsFindFile(temp_s1, 1U, 1, D_80038AB0, &D_80038AC0, &sp1C);
            if (temp_v0 != 0) {
                if (func_8001547C(temp_v0, temp_s1, "pfsRead@1") != 0) {
                    var_s2 = -1;
                } else {
                    goto block_11;
                }
            } else {
block_11:
                temp_v0_2 = func_80023B60(temp_s1, sp1C, 0, arg2, arg3, arg1);
                if (temp_v0_2 != 0) {
                    func_8001547C(temp_v0_2, temp_s1, "pfsRead@2");
                }
            }
        }
    }
    func_8000F75C();
    return var_s2;
}

s32 func_80015110(s32 arg0, u8 *arg1, s32 arg2, s32 arg3) {
    u8 sp18;
    u32 sp1C;
    s32 var_s2;
    struct _struct_D_800A6990_0x68 *temp_s1;
    u32 temp_v0;
    u32 temp_v0_2;

    func_8000F7A4();
    var_s2 = 0;
    if (func_80024360(D_800C30A8, &sp18) != 0) {
        if (!(sp18 & (1 << arg0))) {
            var_s2 = -1;
        } else {
            goto block_4;
        }
    } else {
block_4:
        temp_s1 = &D_800A6990[arg0];
        if (func_80024A30(D_800C30A8, temp_s1, arg0) != 0) {
            var_s2 = -1;
        } else {
            temp_v0 = osPfsFindFile(temp_s1, 1U, 1, D_80038AB0, &D_80038AC0, &sp1C);
            if (temp_v0 != 0) {
                if (func_8001547C(temp_v0, temp_s1, "pfsWrite#1") != 0) {
                    var_s2 = -1;
                } else {
                    goto block_11;
                }
            } else {
block_11:
                temp_v0_2 = func_80023B60(temp_s1, sp1C, 1, arg2, arg3, arg1);
                if (temp_v0_2 != 0) {
                    func_8001547C(temp_v0_2, temp_s1, "pfsWrite#2");
                }
            }
        }
    }
    func_8000F75C();
    return var_s2;
}

s32 func_80015260(s32 arg0) {
    u8 sp18;
    s32 var_s2;
    struct _struct_D_800A6990_0x68 *temp_s1;
    u32 temp_v0;

    func_8000F7A4();
    var_s2 = 0;
    if (((func_80024360(D_800C30A8, &sp18) == 0) || (sp18 & (1 << arg0))) && (temp_s1 = &D_800A6990[arg0], (func_80024A30(D_800C30A8, temp_s1, arg0) == 0))) {
        temp_v0 = func_800236C0(temp_s1, 1, 1, D_80038AB0, &D_80038AC0);
        if (temp_v0 != 0) {
            func_8001547C(temp_v0, temp_s1, "pfsWrite#1");
            goto block_5;
        }
    } else {
block_5:
        var_s2 = -1;
    }
    func_8000F75C();
    return var_s2;
}

u8 func_8001534C(void) {
    u8 sp10;

    func_8000F7A4();
    func_80024360(D_800C30A8, &sp10);
    func_8000F75C();
    return sp10;
}

s32 func_80015384(s32 arg0) {
    u8 sp18;
    u32 sp1C;
    s32 var_s0;
    struct _struct_D_800A6990_0x68 *temp_s1;
    u32 var_v0;

    func_8000F7A4();
    if (func_80024360(D_800C30A8, &sp18) != 0) {
        if (!(sp18 & (1 << arg0))) {
            var_s0 = -1;
        } else {
            goto block_4;
        }
    } else {
block_4:
        temp_s1 = &D_800A6990[arg0];
        if (func_80024A30(D_800C30A8, temp_s1, arg0) != 0) {
            var_s0 = -1;
        } else {
            var_v0 = osPfsFindFile(temp_s1, 1U, 1, D_80038AB0, &D_80038AC0, &sp1C);
            if (var_v0 != 0) {
                var_v0 = func_8001547C(var_v0, temp_s1, "pfsChkFile#1");
            }
            var_s0 = -(var_v0 != 0) | 1;
        }
    }
    func_8000F75C();
    return var_s0;
}

void func_80015474(void) {

}

u32 func_8001547C(u32 arg0, struct _struct_D_800A6990_0x68 *arg1) {
    switch (arg0) {                                 /* irregular */
    case 2:
        return func_80024A30(*arg1->unk4, (struct _struct_D_800A6990_0x68 *) arg1->unk8);
    case 3:
        return osPfsChecker(arg1);
    default:
        return -1U;
    }
}

s32 func_800154D0(void) {
    func_8000F7A4();
    if (func_80024850(D_800C30A8) != 0) {
        D_800B0544 = 0;
    } else {
        D_800B0544 = -1;
    }
    func_8000F75C();
    return D_800B0544;
}

u32 func_80015524(u8 *arg0) {
    u32 temp_s0;

    func_8000F7A4();
    temp_s0 = func_800249C0(D_800C30A8, 0, arg0, 0x200);
    func_8000F75C();
    return temp_s0;
}

u32 func_8001556C(u8 *arg0) {
    u32 temp_s0;

    func_8000F7A4();
    temp_s0 = func_800248D0(D_800C30A8, 0, arg0, 0x200);
    func_8000F75C();
    return temp_s0;
}

f32 func_800155C0(void *arg0, void *arg1) {
    f32 temp_f10;
    f32 temp_f12;
    f32 temp_f6;
    f64 temp_f14;

    temp_f10 = M2C_FIELD(arg1, f32 *, 0xC);
    temp_f6 = M2C_FIELD(arg1, f32 *, 0x10);
    temp_f12 = M2C_FIELD(arg1, f32 *, 0x14);
    temp_f14 = (f64) ((temp_f10 * temp_f10) + (temp_f6 * temp_f6) + (temp_f12 * temp_f12));
    if (!(temp_f14 < 1.0000000168623835e-16)) {
        return (f32) ((((f64) temp_f10 * (f64) (M2C_FIELD(arg0, f32 *, 0) - M2C_FIELD(arg1, f32 *, 0))) + ((f64) temp_f6 * (f64) (M2C_FIELD(arg0, f32 *, 4) - M2C_FIELD(arg1, f32 *, 4))) + ((f64) temp_f12 * (f64) (M2C_FIELD(arg0, f32 *, 8) - M2C_FIELD(arg1, f32 *, 8)))) / temp_f14);
    }
    return 0.0f;
}

f32 func_80015664(void *arg0, f32 *arg1, f32 *arg2) {
    f32 sp10;
    f32 *var_a3;
    f32 *var_t0;
    f32 *var_v1;
    f32 temp_f0;
    f32 temp_f2;
    f32 temp_f2_3;
    f32 temp_f2_4;
    f32 var_f0;
    f32 var_f2;
    f32 var_f2_2;
    f32 var_f6;
    f64 temp_f14;
    f64 temp_f2_2;
    s32 var_t1;

    var_t1 = 0;
    var_a3 = arg1;
    var_t0 = arg2;
    var_v1 = &sp10;
    do {
        M2C_FIELD(var_v1, f32 *, 0) = *var_a3;
        var_t1 += 1;
        temp_f2 = *var_t0 - *var_a3;
        var_a3 += 4;
        var_t0 += 4;
        M2C_FIELD(var_v1, f32 *, 0xC) = temp_f2;
        var_v1 += 4;
    } while (var_t1 < 3);
    temp_f14 = (f64) ((sp1C * sp1C) + (sp20 * sp20) + (sp24 * sp24));
    if (!(temp_f14 < 1.0000000168623835e-16)) {
        var_f2_2 = (f32) ((((f64) sp1C * (f64) (M2C_FIELD(arg0, f32 *, 0) - sp10)) + ((f64) sp20 * (f64) (M2C_FIELD(arg0, f32 *, 4) - sp14)) + ((f64) sp24 * (f64) (M2C_FIELD(arg0, f32 *, 8) - sp18))) / temp_f14);
    } else {
        var_f2_2 = 0.0f;
    }
    temp_f2_2 = (f64) var_f2_2;
    if (temp_f2_2 < 0.0) {
        temp_f2_3 = M2C_FIELD(arg0, f32 *, 0) - M2C_FIELD(arg1, f32 *, 0);
        var_f2 = temp_f2_3 * temp_f2_3;
        var_f6 = M2C_FIELD(arg0, f32 *, 4) - M2C_FIELD(arg1, f32 *, 4);
        var_f0 = M2C_FIELD(arg1, f32 *, 8);
        goto block_9;
    }
    if (temp_f2_2 >= 1.0) {
        temp_f2_4 = M2C_FIELD(arg0, f32 *, 0) - M2C_FIELD(arg2, f32 *, 0);
        var_f6 = M2C_FIELD(arg0, f32 *, 4) - M2C_FIELD(arg2, f32 *, 4);
        var_f0 = M2C_FIELD(arg2, f32 *, 8);
        var_f2 = temp_f2_4 * temp_f2_4;
block_9:
        temp_f0 = M2C_FIELD(arg0, f32 *, 8) - var_f0;
        return sqrtf(var_f2 + (var_f6 * var_f6) + (temp_f0 * temp_f0));
    }
    return Line3_DistanceToPoint(M2C_BITWISE(void *, sp24), M2C_BITWISE(void *, temp_f14), &sp10, var_a3);
}

s32 func_8001581C(f32 *arg0, f32 *arg1, f32 *arg2) {
    f32 *var_a0;
    f32 *var_a1;
    f32 *var_a2;
    f32 temp_f10;
    f32 temp_f6;
    f32 temp_f8;
    f64 temp_f0;
    f64 temp_f12;
    f64 temp_f2;
    f64 temp_f4;
    s32 var_v0;
    s32 var_v1;

    var_a0 = arg0;
    var_a1 = arg1;
    var_a2 = arg2;
    temp_f6 = M2C_FIELD(var_a2, f32 *, 0);
    temp_f8 = M2C_FIELD(var_a2, f32 *, 4);
    temp_f10 = M2C_FIELD(var_a2, f32 *, 8);
    temp_f12 = (f64) ((temp_f6 * temp_f6) + (temp_f8 * temp_f8) + (temp_f10 * temp_f10));
    var_v0 = 0;
    if (!(temp_f12 < 1.0000000168623835e-16)) {
        var_v1 = 0;
        temp_f4 = (f64) ((temp_f6 * M2C_FIELD(var_a1, f32 *, 0)) + (temp_f8 * M2C_FIELD(var_a1, f32 *, 4)) + (temp_f10 * M2C_FIELD(var_a1, f32 *, 8)) + M2C_FIELD(var_a2, f32 *, 0xC)) / temp_f12;
        do {
            temp_f0 = (f64) *var_a2 * temp_f4;
            var_a2 += 4;
            temp_f2 = (f64) *var_a1 - temp_f0;
            var_a1 += 4;
            var_v1 += 1;
            *var_a0 = (f32) temp_f2;
            var_a0 += 4;
        } while (var_v1 < 3);
        var_v0 = 1;
    }
    return var_v0;
}

s32 func_800158D4(f64 *arg0, f32 *arg1, f64 *arg2) {
    f32 *var_a1;
    f64 *var_a0;
    f64 *var_a2;
    f64 temp_f0;
    f64 temp_f10;
    f64 temp_f12;
    f64 temp_f2;
    f64 temp_f4;
    f64 temp_f6;
    f64 temp_f8;
    s32 var_v0;
    s32 var_v1;

    var_a0 = arg0;
    var_a1 = arg1;
    var_a2 = arg2;
    temp_f6 = M2C_FIELD(var_a2, f64 *, 0);
    temp_f8 = M2C_FIELD(var_a2, f64 *, 8);
    temp_f10 = M2C_FIELD(var_a2, f64 *, 0x10);
    temp_f12 = (temp_f6 * temp_f6) + (temp_f8 * temp_f8) + (temp_f10 * temp_f10);
    var_v0 = 0;
    if (!(temp_f12 < 1.0000000168623835e-16)) {
        var_v1 = 0;
        temp_f4 = ((temp_f6 * (f64) M2C_FIELD(var_a1, f32 *, 0)) + (temp_f8 * (f64) M2C_FIELD(var_a1, f32 *, 4)) + (temp_f10 * (f64) M2C_FIELD(var_a1, f32 *, 8)) + M2C_FIELD(var_a2, f64 *, 0x18)) / temp_f12;
        do {
            temp_f0 = *var_a2 * temp_f4;
            var_a2 += 8;
            temp_f2 = (f64) *var_a1 - temp_f0;
            var_a1 += 4;
            var_v1 += 1;
            *var_a0 = temp_f2;
            var_a0 += 8;
        } while (var_v1 < 3);
        var_v0 = 1;
    }
    return var_v0;
}

void func_80015988(s32 arg0, s32 arg1, s32 arg2) {
    f32 sp0;
    f32 sp4;
    f32 sp8;
    f32 *var_a1;
    f32 temp_f0;
    s32 var_a0;
    s32 var_v1;

    var_a0 = arg0;
    if ((var_a0 != arg1) & (var_a0 != arg2)) {
        M2C_FIELD(var_a0, f32 *, 0) = (f32) ((M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg2, f32 *, 8)) - (M2C_FIELD(arg2, f32 *, 4) * M2C_FIELD(arg1, f32 *, 8)));
        M2C_FIELD(var_a0, f32 *, 4) = (f32) ((M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg2, f32 *, 0)) - (M2C_FIELD(arg2, f32 *, 8) * M2C_FIELD(arg1, f32 *, 0)));
        M2C_FIELD(var_a0, f32 *, 8) = (f32) ((M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg2, f32 *, 4)) - (M2C_FIELD(arg2, f32 *, 0) * M2C_FIELD(arg1, f32 *, 4)));
        return;
    }
    sp0 = (M2C_FIELD(arg1, f32 *, 4) * M2C_FIELD(arg2, f32 *, 8)) - (M2C_FIELD(arg2, f32 *, 4) * M2C_FIELD(arg1, f32 *, 8));
    sp4 = (M2C_FIELD(arg1, f32 *, 8) * M2C_FIELD(arg2, f32 *, 0)) - (M2C_FIELD(arg2, f32 *, 8) * M2C_FIELD(arg1, f32 *, 0));
    var_v1 = 0;
    var_a1 = sp;
    sp8 = (M2C_FIELD(arg1, f32 *, 0) * M2C_FIELD(arg2, f32 *, 4)) - (M2C_FIELD(arg2, f32 *, 0) * M2C_FIELD(arg1, f32 *, 4));
    do {
        temp_f0 = *var_a1;
        var_a1 += 4;
        var_v1 += 1;
        *var_a0 = temp_f0;
        var_a0 += 4;
    } while (var_v1 < 3);
}

M2C_UNK *func_80015AA0(M2C_UNK *arg0, u8 *arg1, s32 arg2, s32 arg3, u16 arg4, u16 arg5) {
    u8 sp18;
    M2C_UNK *var_s1;
    s32 var_t0;
    u8 *var_a1;
    u8 *var_v1;
    u8 temp_v0;

    var_s1 = arg0;
    if (var_s1 == NULL) {
        var_s1 = Heap_Alloc(&gGameHeap, 0x180);
        bzero(var_s1, 0x180);
        M2C_FIELD(var_s1, s32 *, 0x174) = 1;
    } else {
        bzero(var_s1, 0x180);
        M2C_FIELD(var_s1, s32 *, 0x174) = 0;
    }
    if (var_s1 != NULL) {
        var_t0 = 0;
        if (*arg1 != 0) {
            var_a1 = arg1;
            var_v1 = &sp18;
            do {
                temp_v0 = *var_a1;
                var_a1 += 1;
                *var_v1 = temp_v0;
                var_t0 += 1;
                var_v1 += 1;
            } while (*var_a1 != 0);
        }
        if (var_t0 & 1) {
            (&sp18)[var_t0] = 0x20;
            var_t0 += 1;
        }
        (&sp18)[var_t0] = 0;
        M2C_FIELD(var_s1, s32 *, 0x170) = var_t0;
        M2C_FIELD(var_s1, s32 *, 0x16C) = var_t0;
        func_80015EA4(var_s1, arg2 & 0xFFFF, arg3 & 0xFFFF, arg4, (s32) arg5);
        M2C_FIELD(var_s1, M2C_UNK **, 0x168) = func_800169B4(&sp18, &D_196710, 3U, 1, 2U);
    }
    return var_s1;
}

s32 func_80015BE4(M2C_UNK *arg0) {
    s32 temp_s0;

    temp_s0 = M2C_FIELD(arg0, s32 *, 0x174);
    Heap_Free(&gGameHeap, M2C_FIELD(arg0, M2C_UNK **, 0x168));
    if (temp_s0 != 0) {
        Heap_Free(&gGameHeap, arg0);
    }
    return 0;
}

void func_80015C40(void *arg0, void *arg1, f32 arg2, f32 arg3, M2C_UNK32 arg4, M2C_UNK32 arg5) {
    M2C_UNK sp10;
    f32 *temp_s2;

    temp_s2 = arg0 + 0xC;
    Mtx_Translate(temp_s2, (f32) ((f64) arg2 + 0.5), (f32) ((f64) arg3 + 0.5), arg4);
    Mtx_Scale(&sp10, arg5, arg5, 0x3F800000);
    Mtx_Mul((s32) temp_s2, (s32) &sp10, (s32) temp_s2);
    func_80016298(arg1);
    M2C_FIELD(arg0, void **, 0xD0) = (void *) (arg1 + 8);
    M2C_FIELD(arg0, s32 *, 0xD4) = (s32) M2C_FIELD(arg1, s32 *, 0);
    M2C_FIELD(arg0, s32 *, 0xD8) = (s32) M2C_FIELD(arg1, s32 *, 4);
}

void func_80015D04(void *arg0, void *arg1, f32 arg2, f32 arg3, M2C_UNK32 arg4, f32 arg5, M2C_UNK32 arg6) {
    M2C_UNK sp18;
    f32 *temp_s0;

    temp_s0 = arg0 + 0xC;
    Mtx_Translate(temp_s0, (f32) ((f64) arg2 + 0.5), (f32) ((f64) arg3 + 0.5), arg4);
    func_800071CC(&sp18, arg5, 0.0f, 0.0f, 1.0f);
    Mtx_Mul((s32) temp_s0, (s32) &sp18, (s32) temp_s0);
    Mtx_Scale(&sp18, arg6, arg6, 0x3F800000);
    Mtx_Mul((s32) temp_s0, (s32) &sp18, (s32) temp_s0);
    func_80016298(arg1);
    M2C_FIELD(arg0, void **, 0xD0) = (void *) (arg1 + 8);
    M2C_FIELD(arg0, s32 *, 0xD4) = (s32) M2C_FIELD(arg1, s32 *, 0);
    M2C_FIELD(arg0, s32 *, 0xD8) = (s32) M2C_FIELD(arg1, s32 *, 4);
}

void func_80015E14(void *arg0, void *arg1, f32 arg2, f32 arg3, f32 arg4, f32 arg5) {
    func_8001650C(arg1, (s16) (s32) arg2, (s16) (s32) arg3, arg4, arg5);
    M2C_FIELD(arg0, void **, 0xD0) = (void *) (arg1 + 8);
    M2C_FIELD(arg0, s32 *, 0xD4) = (s32) M2C_FIELD(arg1, s32 *, 0);
    M2C_FIELD(arg0, s32 *, 0xD8) = (s32) M2C_FIELD(arg1, s32 *, 4);
}

void func_80015EA4(M2C_UNK *arg0, s32 arg1, s32 arg2, u16 arg3, u16 arg4) {
    M2C_UNK *var_t2;
    f32 *var_a3;
    f32 *var_t1;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f0_4;
    f32 temp_f10;
    f32 temp_f12;
    f32 temp_f16;
    f32 temp_f18;
    f32 temp_f22;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f4;
    f32 temp_f4_2;
    f32 temp_f8;
    s32 var_t0;
    s32 var_t0_2;
    u32 temp_a1;
    u32 temp_a2;
    void *var_a0;

    temp_a2 = arg2 & 0xFFFF;
    temp_f22 = (f32) ((temp_a2 >> 8) & 0xF8);
    temp_f18 = (f32) ((temp_a2 >> 3) & 0xF8);
    temp_f16 = (f32) ((temp_a2 * 4) & 0xF8);
    var_t0 = 0;
    var_t2 = arg0;
    var_t1 = D_80038BD8;
    temp_a1 = arg1 & 0xFFFF;
    temp_f12 = (f32) ((temp_a1 >> 8) & 0xF8);
    temp_f10 = (f32) ((temp_a1 >> 3) & 0xF8);
    M2C_FIELD(arg0, s16 *, 0x148) = 0;
    M2C_FIELD(arg0, u16 *, 0x14A) = arg3;
    temp_f8 = (f32) ((temp_a1 * 4) & 0xF8);
    M2C_FIELD(arg0, u16 *, 0x14C) = arg4;
    do {
        temp_f0 = *var_t1;
        temp_f2 = temp_f12 + ((255.0f - temp_f12) * temp_f0);
        temp_f0_2 = (255.0f - temp_f8) * temp_f0;
        temp_f4 = temp_f10 + ((255.0f - temp_f10) * temp_f0);
        var_t1 += 4;
        var_t0 += 1;
        M2C_FIELD(var_t2, s16 *, 0x14E) = (s16) ((((s32) temp_f2 << 8) & ~0x7FF) | (((s32) temp_f4 * 8) & 0x7C0) | (((s32) (temp_f8 + temp_f0_2) >> 2) & 0x3E) | 1);
        var_t2 += 2;
    } while (var_t0 < 4);
    var_t0_2 = 4;
    var_a0 = arg0 + 8;
    var_a3 = D_80038BE8;
    do {
        temp_f0_3 = *var_a3;
        temp_f2_2 = temp_f22 + ((255.0f - temp_f22) * temp_f0_3);
        temp_f0_4 = (255.0f - temp_f16) * temp_f0_3;
        temp_f4_2 = temp_f18 + ((255.0f - temp_f18) * temp_f0_3);
        var_a3 += 4;
        var_t0_2 += 1;
        M2C_FIELD(var_a0, s16 *, 0x14E) = (s16) ((((s32) temp_f2_2 << 8) & ~0x7FF) | (((s32) temp_f4_2 * 8) & 0x7C0) | (((s32) (temp_f16 + temp_f0_4) >> 2) & 0x3E) | 1);
        var_a0 += 2;
    } while (var_t0_2 < 8);
}

void func_80016094(void *arg0, s16 arg1, s32 arg2, s16 arg3, u16 arg4) {
    f32 temp_f10;
    f32 temp_f12;
    f32 temp_f14;
    f32 temp_f2;
    f32 temp_f4;
    f32 temp_f6;
    f32 var_f8;
    s32 var_t1;
    u32 temp_a2;
    u32 temp_v1;
    void *var_t0;

    var_f8 = 0.125f;
    var_t1 = 0;
    var_t0 = arg0;
    temp_v1 = arg1 & 0xFFFF;
    temp_f14 = (f32) ((temp_v1 >> 8) & 0xF8);
    temp_a2 = arg2 & 0xFFFF;
    temp_f12 = (f32) ((temp_v1 >> 3) & 0xF8);
    temp_f10 = (f32) ((temp_v1 * 4) & 0xF8);
    M2C_FIELD(var_t0, s16 *, 0x148) = 0;
    M2C_FIELD(var_t0, s16 *, 0x14A) = arg3;
    M2C_FIELD(var_t0, s16 *, 0x14E) = arg1;
    M2C_FIELD(var_t0, u16 *, 0x14C) = arg4;
    do {
        temp_f4 = temp_f14 + (((f32) ((temp_a2 >> 8) & 0xF8) - temp_f14) * var_f8);
        temp_f6 = temp_f12 + (((f32) ((temp_a2 >> 3) & 0xF8) - temp_f12) * var_f8);
        var_t1 += 1;
        temp_f2 = temp_f10 + (((f32) ((temp_a2 * 4) & 0xF8) - temp_f10) * var_f8);
        var_f8 = (f32) ((f64) var_f8 + 0.125);
        M2C_FIELD(var_t0, s16 *, 0x150) = (s16) ((((s32) temp_f4 << 8) & ~0x7FF) | (((s32) temp_f6 * 8) & 0x7C0) | (((s32) temp_f2 >> 2) & 0x3E) | 1);
        var_t0 += 2;
    } while (var_t1 < 8);
}

void *func_800161E8(void *arg0, u8 *arg1) {
    u8 sp18;
    s32 var_v0;
    u32 var_a0;
    u8 *var_a1;
    u8 *var_v1;
    u8 temp_v0;

    var_a1 = arg1;
    var_a0 = 0;
    if (*var_a1 != 0) {
        var_v1 = &sp18;
loop_2:
        var_v0 = var_a0 & 1;
        if (var_a0 < (u32) M2C_FIELD(arg0, u32 *, 0x16C)) {
            temp_v0 = *var_a1;
            var_a1 += 1;
            *var_v1 = temp_v0;
            var_a0 += 1;
            var_v1 += 1;
            if (*var_a1 == 0) {
                goto block_4;
            }
            goto loop_2;
        }
    } else {
block_4:
        var_v0 = var_a0 & 1;
    }
    if (var_v0 != 0) {
        (&sp18)[var_a0] = 0x20;
        var_a0 += 1;
    }
    (&sp18)[var_a0] = 0;
    M2C_FIELD(arg0, u32 *, 0x170) = var_a0;
    func_80016B00(M2C_FIELD(arg0, s32 *, 0x168), &sp18, &D_196710, 3U, 1U, 2U);
    return arg0;
}

void func_80016298(void *arg0) {
    M2C_UNK *var_a3;
    s32 var_t0;
    s32 var_v0;
    u32 temp_a0;
    u32 temp_a1;
    u32 var_a2_2;
    u32 var_v1;
    void *temp_t7;
    void *temp_t7_10;
    void *temp_t7_2;
    void *temp_t7_3;
    void *temp_t7_4;
    void *temp_t7_5;
    void *temp_t7_6;
    void *temp_t7_7;
    void *temp_t7_8;
    void *temp_t7_9;
    void *var_a2;

    var_t0 = 0;
    var_a3 = &D_80038AD0;
    var_a2 = arg0;
    do {
        M2C_FIELD(var_a2, s32 *, 0x108) = (s32) M2C_FIELD(var_a3, s32 *, 0);
        M2C_FIELD(var_a2, s32 *, 0x10C) = (s32) M2C_FIELD(var_a3, s32 *, 4);
        M2C_FIELD(var_a2, s32 *, 0x110) = (s32) M2C_FIELD(var_a3, s32 *, 8);
        M2C_FIELD(var_a2, s32 *, 0x114) = (s32) M2C_FIELD(var_a3, s32 *, 0xC);
        var_a3 += 0x10;
        var_t0 += 1;
        var_a2 += 0x10;
    } while (var_t0 < 4);
    temp_t7 = arg0 + 0x28;
    var_a2_2 = 0x800;
    M2C_FIELD(arg0, s16 *, 0x110) = -5;
    M2C_FIELD(arg0, s16 *, 0x140) = -5;
    M2C_FIELD(arg0, M2C_UNK **, 0) = &D_80038B10;
    M2C_FIELD(arg0, M2C_UNK **, 4) = &D_80038B58;
    M2C_FIELD(arg0, s32 *, 8) = 0xFD500000;
    M2C_FIELD(arg0, s32 *, 0x10) = 0xF5500000;
    M2C_FIELD(arg0, s32 *, 0x18) = 0xE6000000;
    M2C_FIELD(arg0, s32 *, 0x20) = 0xF3000000;
    M2C_FIELD(arg0, s32 *, 0x14) = 0x07000000;
    M2C_FIELD(arg0, s32 *, 0x1C) = 0;
    M2C_FIELD(arg0, s16 *, 0x120) = (s16) ((M2C_FIELD(arg0, u32 *, 0x170) << 9) - 5);
    M2C_FIELD(arg0, s16 *, 0x128) = (s16) (M2C_FIELD(arg0, u32 *, 0x170) * 8);
    temp_a1 = M2C_FIELD(arg0, u32 *, 0x170);
    M2C_FIELD(arg0, s16 *, 0x118) = (s16) (M2C_FIELD(arg0, u32 *, 0x170) * 8);
    M2C_FIELD(arg0, s16 *, 0x130) = (s16) ((M2C_FIELD(arg0, u32 *, 0x170) << 9) - 5);
    temp_a0 = temp_a1 >> 1;
    M2C_FIELD(arg0, s32 *, 0xC) = (s32) M2C_FIELD(arg0, s32 *, 0x168);
    if (temp_a0 != 0) {
        var_a2_2 = temp_a0 + 0x7FF;
    }
    var_v1 = ((u32) (temp_a1 << 6) >> 2) - 1;
    if (var_v1 >= 0x800U) {
        var_v1 = 0x7FF;
    }
    if (temp_a0 != 0) {
        var_v0 = (var_a2_2 / temp_a0) & 0xFFF;
    } else {
        var_v0 = var_a2_2 & 0xFFF;
    }
    temp_t7_2 = temp_t7 + 8;
    temp_t7_3 = temp_t7_2 + 8;
    temp_t7_4 = temp_t7_3 + 8;
    temp_t7_5 = temp_t7_4 + 8;
    temp_t7_6 = temp_t7_5 + 8;
    temp_t7_7 = temp_t7_6 + 8;
    temp_t7_8 = temp_t7_7 + 8;
    temp_t7_9 = temp_t7_8 + 8;
    M2C_FIELD((arg0 + 0x20), s32 *, 4) = (s32) (((var_v1 & 0xFFF) << 0xC) | 0x07000000 | var_v0);
    M2C_FIELD(arg0, s32 *, 0x28) = 0xE7000000;
    M2C_FIELD(temp_t7, s32 *, 4) = 0;
    temp_t7_10 = temp_t7_9 + 8;
    M2C_FIELD(temp_t7_2, s32 *, 4) = 0;
    M2C_FIELD(temp_t7, s32 *, 8) = (s32) (((((u32) (((u32) (M2C_FIELD(arg0, u32 *, 0x170) * 8) >> 1) + 7) >> 3) & 0x1FF) << 9) | 0xF5400000);
    M2C_FIELD(temp_t7_2, s32 *, 8) = 0xF2000000;
    M2C_FIELD(temp_t7_3, s32 *, 4) = (s32) (((((M2C_FIELD(arg0, u32 *, 0x170) << 5) - 4) & 0xFFF) << 0xC) | 0x1C);
    M2C_FIELD(temp_t7_3, s32 *, 8) = 0xFD100000;
    M2C_FIELD(temp_t7_4, void **, 4) = (void *) (arg0 + 0x148);
    M2C_FIELD(temp_t7_4, s32 *, 8) = 0xE8000000;
    M2C_FIELD(temp_t7_5, s32 *, 4) = 0;
    M2C_FIELD(temp_t7_6, s32 *, 4) = 0x07000000;
    M2C_FIELD(temp_t7_5, s32 *, 8) = 0xF5000100;
    M2C_FIELD(temp_t7_6, s32 *, 8) = 0xE6000000;
    M2C_FIELD(temp_t7_7, s32 *, 4) = 0;
    M2C_FIELD(temp_t7_7, s32 *, 8) = 0xF0000000;
    M2C_FIELD(temp_t7_8, s32 *, 4) = 0x0703C000;
    M2C_FIELD(temp_t7_8, s32 *, 8) = 0xE7000000;
    M2C_FIELD(temp_t7_9, s32 *, 4) = 0;
    M2C_FIELD(temp_t7_9, s32 *, 8) = 0x04300040;
    M2C_FIELD(temp_t7_10, void **, 4) = (void *) (arg0 + 0x108);
    M2C_FIELD(temp_t7_10, s32 *, 8) = 0xB8000000;
    M2C_FIELD((temp_t7_10 + 8), s32 *, 4) = 0;
}

void func_8001650C(void *arg0, s16 arg1, s16 arg2, f32 arg3, f32 arg4) {
    f32 temp_f10;
    f32 temp_f4;
    f32 temp_f6;
    f64 temp_f0;
    f64 temp_f2;
    f64 var_f8;
    s16 temp_a0_3;
    s16 temp_a2;
    s16 temp_v1;
    s32 temp_f0_2;
    s32 temp_f0_3;
    s32 temp_f2_2;
    s32 temp_t1;
    s32 var_a1;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;
    s32 var_v0_4;
    s32 var_v1_2;
    s32 var_v1_3;
    s32 var_v1_4;
    u32 temp_a0;
    u32 temp_a0_2;
    u32 temp_v0;
    u32 var_t3;
    u32 var_v1;
    void *temp_t2;
    void *temp_t2_2;
    void *temp_t2_3;
    void *temp_t2_4;
    void *temp_t2_5;
    void *temp_t2_6;

    temp_t2 = arg0 + 0x58;
    M2C_FIELD(arg0, M2C_UNK **, 0) = &D_80038B88;
    M2C_FIELD(arg0, M2C_UNK **, 4) = &D_80038BC0;
    M2C_FIELD(arg0, s32 *, 8) = 0xFD100000;
    M2C_FIELD(arg0, void **, 0xC) = (void *) (arg0 + 0x148);
    M2C_FIELD(arg0, s32 *, 0x10) = 0xE8000000;
    M2C_FIELD(arg0, s32 *, 0x28) = 0xF0000000;
    M2C_FIELD(arg0, s32 *, 0x30) = 0xE7000000;
    M2C_FIELD(arg0, s32 *, 0x38) = 0xFD500000;
    M2C_FIELD(arg0, s32 *, 0x40) = 0xF5500000;
    M2C_FIELD(arg0, s32 *, 0x50) = 0xF3000000;
    temp_v0 = M2C_FIELD(arg0, u32 *, 0x170);
    var_t3 = 0x800;
    M2C_FIELD(arg0, s32 *, 0x18) = 0xF5000100;
    M2C_FIELD(arg0, s32 *, 0x14) = 0;
    M2C_FIELD(arg0, s32 *, 0x1C) = 0x07000000;
    M2C_FIELD(arg0, s32 *, 0x20) = 0xE6000000;
    M2C_FIELD(arg0, s32 *, 0x24) = 0;
    M2C_FIELD(arg0, s32 *, 0x2C) = 0x0703C000;
    M2C_FIELD(arg0, s32 *, 0x34) = 0;
    M2C_FIELD(arg0, s32 *, 0x44) = 0x07000000;
    M2C_FIELD(arg0, s32 *, 0x48) = 0xE6000000;
    M2C_FIELD(arg0, s32 *, 0x4C) = 0;
    M2C_FIELD(arg0, s32 *, 0x3C) = (s32) M2C_FIELD(arg0, s32 *, 0x168);
    temp_a0 = temp_v0 >> 1;
    if (temp_a0 != 0) {
        var_t3 = temp_a0 + 0x7FF;
    }
    var_v1 = ((u32) (temp_v0 << 6) >> 2) - 1;
    if (var_v1 >= 0x800U) {
        var_v1 = 0x7FF;
    }
    if (temp_a0 != 0) {
        var_v0 = (var_t3 / temp_a0) & 0xFFF;
    } else {
        var_v0 = var_t3 & 0xFFF;
    }
    temp_f2 = (f64) (f32) gScreenWidth / 320.0;
    temp_t2_2 = temp_t2 + 8;
    M2C_FIELD((arg0 + 0x50), s32 *, 4) = (s32) (((var_v1 & 0xFFF) << 0xC) | 0x07000000 | var_v0);
    M2C_FIELD(arg0, s32 *, 0x58) = 0xE7000000;
    M2C_FIELD(temp_t2, s32 *, 4) = 0;
    temp_t2_3 = temp_t2_2 + 8;
    M2C_FIELD(temp_t2_2, s32 *, 4) = 0;
    M2C_FIELD(temp_t2, s32 *, 8) = (s32) (((((u32) (((u32) (M2C_FIELD(arg0, u32 *, 0x170) * 8) >> 1) + 7) >> 3) & 0x1FF) << 9) | 0xF5400000);
    M2C_FIELD(temp_t2_2, s32 *, 8) = 0xF2000000;
    M2C_FIELD(temp_t2_3, s32 *, 4) = (s32) (((((M2C_FIELD(arg0, u32 *, 0x170) << 5) - 4) & 0xFFF) << 0xC) | 0x1C);
    temp_a0_2 = M2C_FIELD(arg0, u32 *, 0x170);
    temp_t2_4 = temp_t2_3 + 8;
    var_f8 = (f64) (temp_a0_2 * 8);
    temp_f10 = (f32) ((f64) arg3 * temp_f2);
    temp_f2_2 = (s32) ((f64) (arg1 + 0xA0) * temp_f2 * 4.0);
    temp_t1 = 0x78 - arg2;
    temp_a2 = temp_t1 * 4;
    if (temp_a0_2 & 0x10000000) {
        var_f8 += 4294967296.0;
    }
    temp_t2_5 = temp_t2_4 + 8;
    temp_t2_6 = temp_t2_5 + 8;
    temp_f0 = (f64) arg4;
    M2C_FIELD(temp_t2_4, s32 *, 4) = (s32) (((temp_f2_2 & ((s32) ~(s16) temp_f2_2 >> 0x1F) & 0xFFF) << 0xC) | (temp_a2 & ((s32) ~(s16) (temp_t1 * 4) >> 0x1F) & 0xFFC));
    temp_v1 = temp_f2_2 + ((s32) ((s32) ((f32) var_f8 * temp_f10) << 0x10) >> 0xE);
    temp_a0_3 = temp_a2 + ((s32) ((s32) (temp_f0 * 8.0) << 0x10) >> 0xE);
    M2C_FIELD(temp_t2_3, s32 *, 8) = (s32) (((temp_v1 & ((s32) ~temp_v1 >> 0x1F) & 0xFFF) << 0xC) | ((temp_a0_3 & ((s32) ~temp_a0_3 >> 0x1F) & 0xFFC) | 0xE4000000));
    M2C_FIELD(temp_t2_4, s32 *, 8) = 0xB4000000;
    temp_f6 = (f32) (1024.0 / (f64) temp_f10);
    temp_f4 = (f32) (1024.0 / temp_f0);
    if ((s16) temp_f2_2 < 0) {
        temp_f0_2 = (s32) temp_f6;
        if ((s16) temp_f0_2 < 0) {
            var_v0_2 = (s32) ((s16) temp_f2_2 * (s16) temp_f0_2) >> 7;
            var_v1_2 = (s32) ~var_v0_2 >> 0x1F;
        } else {
            var_v0_2 = (s32) ((s16) temp_f2_2 * (s16) temp_f0_2) >> 7;
            var_v1_2 = -(var_v0_2 < 1);
        }
        var_a1 = (var_v0_2 & var_v1_2) * -0x10000;
    } else {
        var_a1 = 0;
    }
    if (temp_a2 >= 0) {
        M2C_FIELD(temp_t2_5, s32 *, 4) = var_a1;
    } else {
        temp_f0_3 = (s32) temp_f4;
        if ((s16) temp_f0_3 < 0) {
            var_v0_3 = (s32) (temp_a2 * (s16) temp_f0_3) >> 7;
            var_v1_3 = (s32) ~var_v0_3 >> 0x1F;
        } else {
            var_v0_3 = (s32) (temp_a2 * (s16) temp_f0_3) >> 7;
            var_v1_3 = -(var_v0_3 < 1);
        }
        M2C_FIELD(temp_t2_5, s32 *, 4) = (s32) (var_a1 | (-(var_v0_3 & var_v1_3) & 0xFFFF));
    }
    M2C_FIELD(temp_t2_5, s32 *, 8) = 0xB3000000;
    if (!(temp_f6 >= 2.1474836e9f)) {
        var_v1_4 = (s32) temp_f6;
    } else {
        var_v1_4 = (s32) (temp_f6 - 2.1474836e9f) | 0x80000000;
    }
    if (!(temp_f4 >= 2.1474836e9f)) {
        var_v0_4 = (s32) temp_f4 & 0xFFFF;
    } else {
        var_v0_4 = ((s32) (temp_f4 - 2.1474836e9f) | 0x80000000) & 0xFFFF;
    }
    M2C_FIELD(temp_t2_6, s32 *, 4) = (s32) ((var_v1_4 << 0x10) | var_v0_4);
    M2C_FIELD(temp_t2_6, s32 *, 8) = 0xB8000000;
    M2C_FIELD((temp_t2_6 + 8), s32 *, 4) = 0;
}

M2C_UNK *func_800169B4(u8 *arg0, M2C_UNK *arg1, u8 arg2, s32 arg3, u8 arg4) {
    M2C_UNK *sp1C;
    u8 sp27;
    u8 sp2F;
    M2C_UNK *temp_s4;
    s16 temp_s0_2;
    s16 temp_v0;
    s16 var_a1;
    s16 var_v0;
    s32 temp_s0;
    u8 *temp_s2;

    sp1C = arg1;
    sp27 = arg2;
    var_a1 = 0;
    sp2F = arg4;
    if (*arg0 != 0) {
        do {
            temp_v0 = var_a1 + 1;
            var_a1 = temp_v0;
        } while (*(arg0 + temp_v0) != 0);
    }
    temp_s0 = (s32) (var_a1 << 0x12) >> 0xD;
    temp_s4 = Heap_Alloc(&gGameHeap, temp_s0 | 0x10);
    temp_s2 = temp_s4 + temp_s0;
    if (var_a1 > 0) {
        var_v0 = 0;
        do {
            temp_s0_2 = var_v0 + 1;
            Rom_Read(sp1C + ((*(arg0 + var_v0) - 0x20) * 8), temp_s2, 8);
            func_80016C3C(&temp_s4[temp_s0_2] - 4, temp_s2, (s16) (var_a1 * 4), sp27, arg3 & 0xFF, (s32) sp2F);
            var_v0 = temp_s0_2;
        } while (temp_s0_2 < var_a1);
    }
    return temp_s4;
}

s32 func_80016B00(s32 arg0, u8 *arg1, M2C_UNK *arg2, u8 arg3, u8 arg4, u8 arg5) {
    M2C_UNK *sp1C;
    u8 sp27;
    s16 temp_s0;
    s16 temp_v0;
    s16 var_a1;
    s16 var_v0;
    u8 *temp_s1;

    sp1C = arg2;
    sp27 = arg3;
    var_a1 = 0;
    if (*arg1 != 0) {
        do {
            temp_v0 = var_a1 + 1;
            var_a1 = temp_v0;
        } while (*(arg1 + temp_v0) != 0);
    }
    temp_s1 = arg0 + ((s32) (var_a1 << 0x12) >> 0xD);
    if (var_a1 > 0) {
        var_v0 = 0;
        do {
            temp_s0 = var_v0 + 1;
            Rom_Read(sp1C + ((*(arg1 + var_v0) - 0x20) * 8), temp_s1, 8);
            func_80016C3C((arg0 + (temp_s0 * 4)) - 4, temp_s1, (s16) (var_a1 * 4), sp27, arg4 & 0xFF, arg5 & 0xFF);
            var_v0 = temp_s0;
        } while (temp_s0 < var_a1);
    }
    return arg0;
}

void func_80016C3C(M2C_UNK **arg0, u8 *arg1, s16 arg2, u8 arg3, s32 arg4, s32 arg5) {
    s8 sp0;
    u8 sp1;
    s8 sp2;
    u8 sp3;
    M2C_UNK **var_t4;
    s16 temp_v0_2;
    s16 temp_v1;
    s16 var_t0;
    s16 var_t5;
    s32 var_t1;
    u32 temp_v0;
    u32 temp_v1_2;
    u32 var_t2;
    u8 *var_a1;
    u8 temp_a3;
    u8 temp_t3;
    u8 var_a3;

    var_a1 = arg1;
    var_t4 = arg0;
    var_t2 = 0;
    var_t5 = 0;
    temp_a3 = arg3 & 0xF;
    sp1 = temp_a3;
    sp3 = temp_a3;
    sp0 = arg4 & 0xF;
    sp2 = arg5 & 0xF;
    do {
        var_t1 = 0;
        temp_t3 = *var_a1;
        var_a1 += 1;
        var_t0 = 0;
        var_a3 = temp_t3;
loop_2:
        temp_v0 = var_a3 & 0x80;
        var_a3 *= 2;
        temp_v1_2 = var_t2 & 0x80;
        var_t2 *= 2;
        temp_v0_2 = var_t0 + 1;
        var_t0 = temp_v0_2;
        var_t1 = (var_t1 * 0x10) | *(sp + ((temp_v0 >> 7) | (temp_v1_2 >> 6)));
        if (temp_v0_2 < 8) {
            goto loop_2;
        }
        var_t2 = temp_t3 >> 1;
        *var_t4 = var_t1;
        var_t4 += arg2;
        temp_v1 = var_t5 + 1;
        var_t5 = temp_v1;
        sp1 += 1;
        sp3 += 1;
    } while (temp_v1 < 8);
}

M2C_UNK *func_80016D20(s32 arg0) {
    M2C_UNK *temp_s2;
    s32 *temp_s5;
    s32 *temp_v1;
    s32 *var_s0;
    s32 *var_v1;
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_s1;
    s32 var_s1;
    s32 var_s3;

    if (arg0 < 0x20) {
        temp_s5 = D_80038C00[arg0];
        var_s3 = 0;
        var_v1 = temp_s5;
loop_2:
        if (*var_v1 != 0) {
            var_s3 += 1;
            var_v1 += 0xC;
            if (var_s3 < 0xFF) {
                goto loop_2;
            }
        }
        temp_s1 = var_s3 * 4;
        temp_s2 = Heap_Alloc(&gGameHeap, 0xC);
        temp_s2[1] = Heap_Alloc(&gGameHeap, temp_s1);
        var_s1 = 0;
        temp_s2[2] = Heap_Alloc(&gGameHeap, temp_s1);
        if (var_s3 > 0) {
            var_s0 = temp_s5;
loop_6:
            temp_a1 = *var_s0;
            if (temp_a1 != 0) {
                temp_a0 = var_s1 * 4;
                var_s1 += 1;
                *(temp_a0 + temp_s2[1]) = Sprite_New(NULL, (temp_a1 & 0xFFFFFF) + &D_611F0);
                temp_v1 = var_s0 + 4;
                var_s0 += 0xC;
                *(temp_a0 + temp_s2[2]) = temp_v1;
                if (var_s1 < var_s3) {
                    goto loop_6;
                }
            }
        }
        M2C_FIELD(temp_s2, M2C_UNK **, 0) = (M2C_UNK *) var_s3;
        return temp_s2;
    }
    return NULL;
}

M2C_UNK *func_80016E58(s32 arg0) {
    M2C_UNK *temp_s2;
    s32 *temp_s5;
    s32 *temp_v1;
    s32 *var_s0;
    s32 *var_v1;
    s32 temp_a0;
    s32 temp_a1;
    s32 temp_s1;
    s32 var_s1;
    s32 var_s3;

    if (arg0 < 0x20) {
        temp_s5 = D_80038C00[arg0];
        var_s3 = 0;
        var_v1 = temp_s5;
loop_2:
        if (*var_v1 != 0) {
            var_s3 += 1;
            var_v1 += 0xC;
            if (var_s3 < 0xFF) {
                goto loop_2;
            }
        }
        temp_s1 = var_s3 * 4;
        temp_s2 = Heap_Alloc(&gGameHeap, 0xC);
        temp_s2[1] = Heap_Alloc(&gGameHeap, temp_s1);
        var_s1 = 0;
        temp_s2[2] = Heap_Alloc(&gGameHeap, temp_s1);
        if (var_s3 > 0) {
            var_s0 = temp_s5;
loop_6:
            temp_a1 = *var_s0;
            if (temp_a1 != 0) {
                temp_a0 = var_s1 * 4;
                var_s1 += 1;
                *(temp_a0 + temp_s2[1]) = Sprite_NewSharedPalette(NULL, (temp_a1 & 0xFFFFFF) + &D_611F0);
                temp_v1 = var_s0 + 4;
                var_s0 += 0xC;
                *(temp_a0 + temp_s2[2]) = temp_v1;
                if (var_s1 < var_s3) {
                    goto loop_6;
                }
            }
        }
        M2C_FIELD(temp_s2, M2C_UNK **, 0) = (M2C_UNK *) var_s3;
        return temp_s2;
    }
    return NULL;
}

s32 func_80016F90(M2C_UNK *arg0) {
    M2C_UNK *temp_a0;
    s32 temp_s0;
    s32 var_s1;

    var_s1 = 0;
    if (M2C_FIELD(arg0, s32 *, 0) > 0) {
        do {
            temp_s0 = var_s1 * 4;
            temp_a0 = *(temp_s0 + arg0[1]);
            var_s1 += 1;
            *(temp_s0 + arg0[1]) = Sprite_Delete(temp_a0);
        } while (var_s1 < M2C_FIELD(arg0, s32 *, 0));
    }
    Heap_Free(&gGameHeap, arg0[1]);
    Heap_Free(&gGameHeap, arg0[2]);
    Heap_Free(&gGameHeap, arg0);
    return 0;
}

void func_80017030(void) {
    M2C_UNK *temp_v0;

    temp_v0 = Heap_Alloc(&gGameHeap, 0x1510);
    if (temp_v0 != NULL) {
        M2C_FIELD(temp_v0, s32 *, 0x1508) = 0;
    }
}

void func_8001705C(M2C_UNK *arg0) {
    Heap_Free(&gGameHeap, arg0);
}

void *func_80017080(void *arg0, M2C_UNK *arg1, u8 *arg2, f32 arg3, f32 arg4) {
    f32 sp10;
    u8 *sp54;
    f32 var_f20;
    f32 var_f22;
    f32 var_f24;
    s32 temp_s0;
    s32 temp_s0_2;
    s32 temp_s3_2;
    s32 temp_v0;
    u8 *var_s5;
    u8 temp_s3;
    void *temp_s1;
    void *temp_s1_2;
    void *temp_s2;
    void *temp_s2_2;
    void *temp_v0_2;
    void *temp_v1;
    void *temp_v1_2;
    void *var_s1;
    void *var_s2;
    void *var_s4;

    sp54 = arg2;
    var_s5 = sp54;
    var_f22 = 0.0f;
    var_f20 = arg3;
    var_f24 = 0.0f;
    temp_v0 = M2C_FIELD(arg0, s32 *, 0x1508) ^ 1;
    var_s4 = arg0 + ((temp_v0 << 0xB) + 8);
    temp_s2 = arg0 + ((temp_v0 * 0x280) + 0x1008);
    var_s2 = temp_s2 + 8;
    var_s1 = var_s2 - 4;
    M2C_FIELD(arg0, s32 *, 0x1508) = temp_v0;
    M2C_FIELD(temp_s2, s32 *, 0) = 0xBA001701;
    M2C_FIELD(temp_s2, s32 *, 4) = 0x800000;
loop_1:
    temp_s3 = *var_s5;
    temp_s3_2 = temp_s3 - 0x20;
    if (temp_s3 != 0) {
        temp_s0 = temp_s3_2 & 0xFF;
        temp_s0_2 = temp_s0 * 4;
        if (temp_s0 != 0) {
            temp_v1 = *(temp_s0_2 + M2C_FIELD(arg1, s32 *, 8));
            Mtx_Translate(&sp10, (var_f20 - (f32) M2C_FIELD(temp_v1, s16 *, 0)) - var_f24, (arg4 + (f32) M2C_FIELD(temp_v1, s16 *, 4)) - var_f22, 0);
            temp_v1_2 = *(temp_s0_2 + M2C_FIELD(arg1, s32 *, 8));
            temp_s1 = var_s1 + 8;
            var_f24 = var_f20 - (f32) M2C_FIELD(temp_v1_2, s16 *, 0);
            var_f22 = arg4 + (f32) M2C_FIELD(temp_v1_2, s16 *, 4);
            func_8000A924(var_s4, &sp10);
            M2C_FIELD(temp_s1, s32 *, -4) = 0x01000040;
            M2C_FIELD(var_s1, void **, 8) = var_s4;
            var_s1 = temp_s1 + 8;
            M2C_FIELD(var_s1, s32 *, -4) = 0x06000000;
            var_s2 += 0x10;
            var_s4 += 0x40;
            M2C_FIELD(temp_s1, s32 *, 8) = (s32) M2C_FIELD(*(temp_s0_2 + M2C_FIELD(arg1, s32 *, 4)), s32 *, 0x14);
        } else {
            temp_s1_2 = var_s1 + 8;
            M2C_FIELD(temp_s1_2, s32 *, -4) = 0;
            M2C_FIELD(var_s1, void **, 8) = NULL;
            var_s1 = temp_s1_2 + 8;
            var_s2 += 0x10;
            M2C_FIELD(var_s1, s32 *, -4) = 0;
            M2C_FIELD(temp_s1_2, s32 *, 8) = 0;
        }
        temp_v0_2 = *(((temp_s3_2 & 0xFF) * 4) + M2C_FIELD(arg1, s32 *, 8));
        var_s5 += 1;
        var_f20 += (f32) (M2C_FIELD(temp_v0_2, s16 *, 2) - M2C_FIELD(temp_v0_2, s16 *, 0));
        if ((s32) var_s5 < (s32) (sp54 + 0x20)) {
            goto loop_1;
        }
    }
    temp_s2_2 = var_s2 + 8;
    M2C_FIELD(var_s2, s32 *, 0) = 0xE7000000;
    M2C_FIELD(var_s2, s32 *, 4) = 0;
    M2C_FIELD(var_s2, s32 *, 8) = 0xBA001701;
    M2C_FIELD(temp_s2_2, s32 *, 4) = 0;
    M2C_FIELD(temp_s2_2, s32 *, 8) = 0xB8000000;
    M2C_FIELD((temp_s2_2 + 8), s32 *, 4) = 0;
    return arg0 + ((temp_v0 * 0x280) + 0x1008);
}

void *func_80017304(void *arg0, void *arg1, u8 *arg2, f32 arg3, f32 arg4, s32 arg5) {
    f32 sp10;
    u8 *sp54;
    f32 var_f20;
    f32 var_f22;
    f32 var_f24;
    s32 temp_s0;
    s32 temp_s3;
    s32 temp_v0;
    s32 var_t2;
    u8 *var_s4;
    u8 temp_v0_2;
    void *temp_s1;
    void *temp_s1_10;
    void *temp_s1_11;
    void *temp_s1_2;
    void *temp_s1_3;
    void *temp_s1_4;
    void *temp_s1_5;
    void *temp_s1_6;
    void *temp_s1_7;
    void *temp_s1_8;
    void *temp_s1_9;
    void *temp_s2;
    void *temp_v0_3;
    void *temp_v1;
    void *temp_v1_2;
    void *var_a0;
    void *var_a1;
    void *var_a3;
    void *var_s1;
    void *var_s1_2;
    void *var_s2;
    void *var_s5;
    void *var_t0;

    sp54 = arg2;
    var_f24 = arg3;
    temp_v0 = M2C_FIELD(arg0, s32 *, 0x1508) ^ 1;
    temp_s1 = arg0 + ((temp_v0 * 0x280) + 0x1008);
    M2C_FIELD(arg0, s32 *, 0x1508) = temp_v0;
    M2C_FIELD(temp_s1, s32 *, 4) = 0x800000;
    M2C_FIELD(temp_s1, s32 *, 0) = 0xBA001701;
    var_s5 = arg0 + ((temp_v0 << 0xB) + 8);
    temp_s1_2 = temp_s1 + 8;
    if (M2C_FIELD(*M2C_FIELD(arg1, void ***, 4), u8 *, 0x26) == 1) {
        temp_s1_3 = temp_s1_2 + 8;
        var_a0 = temp_s1_3;
        temp_s1_4 = temp_s1_3 + 8;
        var_a3 = temp_s1_4;
        temp_s1_5 = temp_s1_4 + 8;
        var_a1 = temp_s1_5;
        temp_s1_6 = temp_s1_5 + 8;
        var_t0 = temp_s1_6;
        var_s1 = temp_s1_6 + 8;
        var_t2 = 0x023F0000;
    } else {
        temp_s1_7 = temp_s1_2 + 8;
        var_a0 = temp_s1_7;
        temp_s1_8 = temp_s1_7 + 8;
        var_a3 = temp_s1_8;
        temp_s1_9 = temp_s1_8 + 8;
        var_a1 = temp_s1_9;
        temp_s1_10 = temp_s1_9 + 8;
        var_t0 = temp_s1_10;
        var_s1 = temp_s1_10 + 8;
        var_t2 = 0x02030000;
    }
    var_s1_2 = var_s1 + 8;
    *temp_s1_2 = 0xFD100000;
    M2C_FIELD(temp_s1_2, s32 *, 4) = arg5;
    M2C_FIELD(var_a0, s32 *, 0) = 0xE8000000;
    M2C_FIELD(var_a0, s32 *, 4) = 0;
    M2C_FIELD(var_a3, s32 *, 4) = 0x02000000;
    M2C_FIELD(var_a3, s32 *, 0) = 0xF5000100;
    M2C_FIELD(var_a1, s32 *, 0) = 0xE6000000;
    M2C_FIELD(var_a1, s32 *, 4) = 0;
    M2C_FIELD(var_t0, s32 *, 0) = 0xF0000000;
    M2C_FIELD(var_t0, s32 *, 4) = (s32) (var_t2 | 0xC000);
    M2C_FIELD(var_s1, s32 *, 0) = 0xE7000000;
    M2C_FIELD(var_s1, s32 *, 4) = 0;
    var_s4 = sp54;
    var_f20 = 0.0f;
    var_s2 = var_s1_2 - 4;
    var_f22 = 0.0f;
loop_4:
    temp_v0_2 = *var_s4;
    if (temp_v0_2 != 0) {
        temp_s3 = (temp_v0_2 - 0x20) & 0xFF;
        temp_s0 = temp_s3 * 4;
        if (temp_s3 != 0) {
            temp_v1 = *(temp_s0 + M2C_FIELD(arg1, s32 *, 8));
            Mtx_Translate(&sp10, (var_f24 - (f32) M2C_FIELD(temp_v1, s16 *, 0)) - var_f22, (arg4 + (f32) M2C_FIELD(temp_v1, s16 *, 4)) - var_f20, 0);
            temp_v1_2 = *(temp_s0 + M2C_FIELD(arg1, s32 *, 8));
            temp_s2 = var_s2 + 8;
            var_f22 = var_f24 - (f32) M2C_FIELD(temp_v1_2, s16 *, 0);
            var_f20 = arg4 + (f32) M2C_FIELD(temp_v1_2, s16 *, 4);
            func_8000A924(var_s5, &sp10);
            M2C_FIELD(temp_s2, s32 *, -4) = 0x01000040;
            M2C_FIELD(var_s2, void **, 8) = var_s5;
            var_s2 = temp_s2 + 8;
            M2C_FIELD(var_s2, s32 *, -4) = 0x06000000;
            var_s1_2 += 0x10;
            var_s5 += 0x40;
            M2C_FIELD(temp_s2, s32 *, 8) = (s32) M2C_FIELD(*(temp_s0 + M2C_FIELD(arg1, void ***, 4)), s32 *, 0x14);
        }
        temp_v0_3 = *((temp_s3 * 4) + M2C_FIELD(arg1, s32 *, 8));
        var_s4 += 1;
        var_f24 += (f32) (M2C_FIELD(temp_v0_3, s16 *, 2) - M2C_FIELD(temp_v0_3, s16 *, 0));
        if ((s32) var_s4 < (s32) (sp54 + 0x20)) {
            goto loop_4;
        }
    }
    temp_s1_11 = var_s1_2 + 8;
    M2C_FIELD(var_s1_2, s32 *, 0) = 0xE7000000;
    M2C_FIELD(var_s1_2, s32 *, 4) = 0;
    M2C_FIELD(var_s1_2, s32 *, 8) = 0xBA001701;
    M2C_FIELD(temp_s1_11, s32 *, 4) = 0;
    M2C_FIELD(temp_s1_11, s32 *, 8) = 0xB8000000;
    M2C_FIELD((temp_s1_11 + 8), s32 *, 4) = 0;
    return arg0 + ((temp_v0 * 0x280) + 0x1008);
}

void *func_8001763C(void *arg0, void *arg1, M2C_UNK *arg2, u8 *arg3, f32 arg4, f32 arg5) {
    void *temp_v1;

    temp_v1 = func_80017080(arg1, arg2, arg3, arg4, arg5);
    if (arg0 != NULL) {
        M2C_FIELD(arg0, void **, 0xD0) = temp_v1;
        M2C_FIELD(arg0, s32 *, 0xD4) = (s32) M2C_FIELD(*M2C_FIELD(arg2, void ***, 4), s32 *, 0x10);
        M2C_FIELD(arg0, s32 *, 0xD8) = (s32) M2C_FIELD(*M2C_FIELD(arg2, void ***, 4), s32 *, 0x18);
    }
    return arg1;
}

void *func_800176C4(void *arg0, void *arg1, void *arg2, u8 *arg3, f32 arg4, f32 arg5, s32 arg6) {
    void *temp_v1;

    temp_v1 = func_80017304(arg1, arg2, arg3, arg4, arg5, arg6);
    if (arg0 != NULL) {
        M2C_FIELD(arg0, void **, 0xD0) = temp_v1;
        M2C_FIELD(arg0, s32 *, 0xD4) = (s32) M2C_FIELD(*M2C_FIELD(arg2, void ***, 4), s32 *, 0x10);
        M2C_FIELD(arg0, s32 *, 0xD8) = (s32) M2C_FIELD(*M2C_FIELD(arg2, void ***, 4), s32 *, 0x18);
    }
    return arg1;
}

s32 func_80017754(void *arg0, u8 *arg1) {
    s32 temp_a3;
    s32 var_a2;
    u8 *var_a1;
    u8 temp_v0;
    void *temp_v0_2;

    var_a1 = arg1;
    var_a2 = 0;
    temp_a3 = var_a1 + 0x20;
loop_1:
    temp_v0 = *var_a1;
    if (temp_v0 != 0) {
        temp_v0_2 = *((((temp_v0 - 0x20) & 0xFF) * 4) + M2C_FIELD(arg0, s32 *, 8));
        var_a1 += 1;
        var_a2 += M2C_FIELD(temp_v0_2, s16 *, 2) - M2C_FIELD(temp_v0_2, s16 *, 0);
        if ((s32) var_a1 < temp_a3) {
            goto loop_1;
        }
    }
    return var_a2;
}

void func_800177B0(M2C_UNK **arg0, s32 *arg1) {
    M2C_UNK **var_v1;
    s32 *var_v1_2;
    s32 var_v0;
    s32 var_v0_2;

    var_v1 = arg0;
    var_v0 = 4;
    do {
        *var_v1 = NULL;
        var_v0 -= 1;
        var_v1 += 4;
    } while (var_v0 != 0);
    var_v1_2 = arg1;
    var_v0_2 = 4;
    do {
        *var_v1_2 = 0;
        var_v0_2 -= 1;
        var_v1_2 += 4;
    } while (var_v0_2 != 0);
    M2C_FIELD(arg0, s32 *, 4) = 0;
    M2C_FIELD(arg1, s32 *, 0) = 0;
    M2C_FIELD(arg0, s32 **, 0) = arg1;
    M2C_FIELD(arg1, M2C_UNK ***, 4) = arg0;
}

void func_800177F4(s32 arg0, s32 arg1, M2C_UNK *arg2) {
    M2C_UNK *temp_v0;
    M2C_UNK *var_v1;
    s32 var_v0;

    temp_v0 = Heap_Alloc(&gGameHeap, 0x10);
    var_v1 = temp_v0;
    var_v0 = 4;
    do {
        *var_v1 = NULL;
        var_v0 -= 1;
        var_v1 += 4;
    } while (var_v0 != 0);
    *arg2[1] = temp_v0;
    M2C_FIELD(temp_v0, M2C_UNK **, 0) = arg2;
    temp_v0[1] = arg2[1];
    arg2[1] = temp_v0;
    temp_v0[2] = arg0;
    temp_v0[3] = arg1;
}

void func_80017878(M2C_UNK *arg0, M2C_UNK **arg1) {
    M2C_UNK *var_s0;
    M2C_UNK *var_s1;

    var_s1 = *arg1;
    if (*var_s1 != NULL) {
        var_s0 = var_s1;
        do {
            var_s1 = *var_s1;
            Heap_Free(var_s0[2], var_s0[3]);
            *var_s0[1] = M2C_FIELD(var_s0, M2C_UNK **, 0);
            M2C_FIELD(var_s0, M2C_UNK **, 0)[1] = var_s0[1];
            Heap_Free(arg0, var_s0);
            var_s0 = var_s1;
        } while (*var_s1 != NULL);
    }
}

M2C_UNK *func_80017900(M2C_UNK *arg0, M2C_UNK *arg1) {
    M2C_UNK *var_s0;

    var_s0 = arg0;
    if (var_s0 == NULL) {
        var_s0 = Heap_Alloc(&gGameHeap, 0x2C);
        bzero(var_s0, 0x2C);
        M2C_FIELD(var_s0, s16 *, 0x26) = 1;
    } else {
        bzero(var_s0, 0x2C);
        M2C_FIELD(var_s0, s16 *, 0x26) = 0;
    }
    if (var_s0 != NULL) {
        func_80017A64(var_s0, arg1, 1);
    }
    return var_s0;
}

M2C_UNK *func_80017984(M2C_UNK *arg0, M2C_UNK *arg1) {
    M2C_UNK *var_s0;

    var_s0 = arg0;
    if (var_s0 == NULL) {
        var_s0 = Heap_Alloc(&gGameHeap, 0x2C);
        bzero(var_s0, 0x2C);
        M2C_FIELD(var_s0, s16 *, 0x26) = 1;
    } else {
        bzero(var_s0, 0x2C);
        M2C_FIELD(var_s0, s16 *, 0x26) = 0;
    }
    if (var_s0 != NULL) {
        func_80017A64(var_s0, arg1, 0);
    }
    return var_s0;
}

s32 func_80017A08(M2C_UNK *arg0) {
    u16 temp_s0;

    temp_s0 = M2C_FIELD(arg0, u16 *, 0x26);
    Heap_Free(&gGameHeap, M2C_FIELD(arg0, M2C_UNK **, 0));
    if (temp_s0 != 0) {
        Heap_Free(&gGameHeap, arg0);
    }
    return 0;
}

s32 func_80017A64(M2C_UNK *arg0, M2C_UNK *arg1, s32 arg2) {
    s32 sp14;
    M2C_UNK *sp1C;
    M2C_UNK *sp24;
    u16 sp2E;
    s32 sp30;
    s32 sp34;
    s32 sp38;
    M2C_UNK *temp_a0;
    M2C_UNK *temp_fp;
    M2C_UNK *temp_fp_2;
    M2C_UNK *temp_fp_3;
    M2C_UNK *temp_fp_4;
    M2C_UNK *temp_s0;
    M2C_UNK *temp_s1;
    M2C_UNK *temp_s1_2;
    M2C_UNK *temp_s2;
    M2C_UNK *temp_s3;
    M2C_UNK *temp_s4;
    M2C_UNK *temp_s5;
    M2C_UNK *temp_s5_2;
    M2C_UNK *temp_s5_3;
    M2C_UNK *temp_s6;
    M2C_UNK *temp_t7;
    M2C_UNK *temp_v0;
    M2C_UNK *var_s1;
    s32 *var_s0;
    s32 temp_a0_3;
    s32 temp_a0_4;
    s32 temp_a0_5;
    s32 temp_a3;
    s32 temp_s0_2;
    s32 temp_s0_3;
    s32 temp_s2_2;
    s32 temp_s3_2;
    s32 temp_s3_3;
    s32 temp_t1;
    s32 temp_t4;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v1;
    s32 temp_v1_2;
    s32 temp_v1_4;
    s32 var_a0;
    s32 var_a0_2;
    s32 var_a1;
    s32 var_a2;
    s32 var_a2_2;
    s32 var_a2_3;
    s32 var_a2_4;
    s32 var_t6;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;
    s32 var_v0_4;
    s32 var_v0_5;
    s32 var_v0_6;
    s32 var_v0_7;
    s32 var_v1;
    s32 var_v1_2;
    s32 var_v1_3;
    s32 var_v1_4;
    u16 *var_fp;
    u16 temp_a1;
    u16 temp_s2_3;
    u16 temp_s5_4;
    u16 temp_t3;
    u32 temp_a0_6;
    u32 temp_a0_7;
    u32 temp_s4_2;
    u32 temp_s6_2;
    u32 var_a0_3;
    void *temp_a0_2;
    void *temp_s0_10;
    void *temp_s0_11;
    void *temp_s0_12;
    void *temp_s0_13;
    void *temp_s0_14;
    void *temp_s0_15;
    void *temp_s0_16;
    void *temp_s0_17;
    void *temp_s0_18;
    void *temp_s0_19;
    void *temp_s0_20;
    void *temp_s0_4;
    void *temp_s0_5;
    void *temp_s0_6;
    void *temp_s0_7;
    void *temp_s0_8;
    void *temp_s0_9;
    void *temp_s1_3;
    void *temp_s1_4;
    void *temp_s1_5;
    void *temp_v1_3;
    void *var_s0_2;
    void *var_s1_2;

    arg0[7] = &D_80038C10;
    sp14 = arg2;
    temp_v0 = Asset_LoadLzss(arg1);
    sp1C = temp_v0;
    temp_fp = temp_v0 + 0x20;
    temp_s2 = temp_v0[1];
    temp_s3 = temp_v0[6];
    temp_s1 = M2C_FIELD(sp1C, M2C_UNK **, 0);
    temp_s0 = sp1C[2];
    temp_s6 = sp1C[3];
    temp_t7 = sp1C[4];
    sp24 = temp_t7;
    temp_s4 = sp1C[5];
    temp_a0 = Heap_Alloc(&gGameHeap, ((temp_v0[7] + 0x18) * 8) + (temp_s3 - temp_s2));
    temp_s2_2 = temp_s6 - temp_s2;
    M2C_FIELD(arg0, s16 *, 0x24) = (s16) temp_s1;
    temp_s1_2 = temp_a0 + temp_s2_2;
    M2C_FIELD(arg0, M2C_UNK **, 0) = temp_a0;
    arg0[2] = temp_a0;
    arg0[3] = temp_s1_2 - (temp_s6 - temp_s0);
    func_800182E4(temp_a0, temp_fp, temp_s2_2);
    temp_fp_2 = temp_fp + temp_s2_2;
    arg0[5] = temp_s1_2;
    temp_s0_2 = temp_t7 - temp_s6;
    func_800182E4(temp_s1_2, temp_fp_2, temp_s0_2);
    temp_s5 = temp_s1_2 + temp_s0_2;
    temp_fp_3 = temp_fp_2 + temp_s0_2;
    arg0[4] = temp_s5;
    temp_s0_3 = temp_s4 - temp_t7;
    func_800182E4(temp_s5, temp_fp_3, temp_s0_3);
    temp_s5_2 = temp_s5 + temp_s0_3;
    temp_fp_4 = temp_fp_3 + temp_s0_3;
    temp_s3_2 = temp_s3 - temp_s4;
    arg0[1] = temp_s5_2;
    func_800182E4(temp_s5_2, temp_fp_4, temp_s3_2);
    temp_s5_3 = temp_s5_2 + temp_s3_2;
    var_fp = temp_fp_4 + temp_s3_2;
    var_a1 = 0;
loop_1:
    if (var_a1 < ((s32) (sp24 - temp_s6) / 16)) {
        temp_a0_2 = arg0[5] + (var_a1 * 0x10);
        M2C_FIELD(temp_a0_2, void **, 8) = (void *) (M2C_FIELD(temp_a0_2, void **, 8) + arg0[1]);
        var_a1 += 1;
        M2C_FIELD(temp_a0_2, void **, 0xC) = (void *) (M2C_FIELD(temp_a0_2, void **, 0xC) + arg0[4]);
        goto loop_1;
    }
    var_s1 = temp_s5_3;
    var_t6 = 0x07000000;
    var_s0 = var_s1 - 4;
    arg0[6] = temp_s5_3;
    sp2E = 0;
loop_4:
    temp_s2_3 = *var_fp;
    temp_v1 = temp_s2_3 & 0xC000;
    var_fp += 2;
    if (temp_v1 == 0x4000) {
        temp_a0_3 = temp_s2_3 & 0x300;
        if ((sp2E != temp_a0_3) & (sp14 != 0)) {
            temp_s0_4 = var_s0 + 8;
            if (temp_a0_3 == 0x200) {
                M2C_FIELD(temp_s0_4, s32 *, -4) = 0xB7000000;
                M2C_FIELD(var_s0, s32 *, 8) = 0x40000;
                var_s0 = temp_s0_4 + 8;
                var_s1 += 0x10;
                var_v0 = 0x07C007C0;
            } else {
                M2C_FIELD(temp_s0_4, s32 *, -4) = 0xB6000000;
                M2C_FIELD(var_s0, s32 *, 8) = 0x40000;
                var_s0 = temp_s0_4 + 8;
                var_s1 += 0x10;
                var_v0 = 0x80008000;
            }
            M2C_FIELD(var_s0, s32 *, -4) = 0xBB000001;
            M2C_FIELD(var_s0, s32 *, 0) = var_v0;
            sp2E = temp_s2_3 & 0x300;
        }
        temp_a1 = *var_fp;
        var_fp += 2;
        var_s0 += 8;
        temp_v1_2 = temp_s2_3 & 0xF;
        M2C_FIELD(var_s0, s32 *, -4) = (s32) ((((temp_v1_2 * 0x10) | ((u32) (temp_s2_3 & 0xF0) >> 4)) << 0x10) | (((temp_v1_2 + 1) * 0x10) | 0x04000000));
        var_s1 += 8;
        var_a1 = temp_a1 * 0x10;
        M2C_FIELD(var_s0, void **, 0) = arg0[2] + var_a1;
        goto loop_4;
    }
    switch (temp_v1) {                              /* switch 1; irregular */
    default:                                        /* switch 1 */
        var_s0 += 8;
        if (temp_v1 != 0) {

        } else {
            var_s1 += 8;
            M2C_FIELD(var_s0, s32 *, -4) = 0xBF000000;
            var_a0 = ((temp_s2_3 & 0xF) * 0xA0000) | (((u32) (temp_s2_3 & 0xF0) >> 4) * 0xA00);
            var_v0_2 = ((u32) (temp_s2_3 & 0xF00) >> 8) * 0xA;
block_63:
            *var_s0 = var_a0 | var_v0_2;
            goto loop_4;
        }
        break;
    case 0x8000:                                    /* switch 1 */
        temp_s3_3 = (temp_s2_3 & 0x3F) * 0x10;
        temp_v1_3 = temp_s3_3 + arg0[5];
        temp_s5_4 = M2C_FIELD(temp_v1_3, u16 *, 4);
        temp_t3 = M2C_FIELD(temp_v1_3, u16 *, 6);
        sp38 = var_t6;
        temp_s4_2 = temp_s5_4 & 0xFFFF;
        sp34 = (s32) temp_t3;
        temp_v0_2 = func_8001831C(temp_s4_2, var_a1);
        sp30 = temp_v0_2;
        temp_s6_2 = temp_t3 & 0xFFFF;
        temp_t1 = func_8001831C(temp_s6_2);
        temp_a3 = (temp_s5_4 >> 6) & 2;
        temp_v1_4 = temp_s2_3 & 0x300;
        var_a1 = (temp_t3 >> 6) & 2;
        temp_t4 = var_a1;
        switch (temp_v1_4) {                        /* switch 2; irregular */
        case 0x0:                                   /* switch 2 */
            temp_s0_5 = var_s0 + 8;
            temp_s1_3 = var_s1 + 0x18;
            M2C_FIELD(temp_s0_5, s32 *, -4) = 0xFD100000;
            temp_a0_4 = temp_s5_4 & 0x7F;
            var_a2 = temp_a0_4;
            M2C_FIELD(var_s0, s32 *, 8) = (s32) M2C_FIELD((temp_s3_3 + arg0[5]), s32 *, 0xC);
            temp_s0_6 = temp_s0_5 + 8;
            M2C_FIELD(temp_s0_6, s32 *, -4) = 0xF5100000;
            M2C_FIELD(temp_s0_5, s32 *, 8) = (s32) ((var_a1 << 0x12) | (((temp_t1 & 0xF) << 0xE) | var_t6) | (temp_a3 << 8) | ((temp_v0_2 & 0xF) * 0x10));
            temp_s0_7 = temp_s0_6 + 8;
            M2C_FIELD(temp_s0_7, s32 *, -4) = 0xE6000000;
            M2C_FIELD(temp_s0_6, s32 *, 8) = 0;
            temp_s0_8 = temp_s0_7 + 8;
            M2C_FIELD(temp_s0_8, s32 *, -4) = 0xF3000000;
            if (temp_a0_4 < 0) {
                var_a2 = temp_a0_4 + 3;
            }
            temp_v0_3 = var_a2 >> 2;
            var_v1 = 0x800;
            if (temp_v0_3 > 0) {
                var_v1 = temp_v0_3 + 0x7FF;
            }
            var_a1 = temp_s5_4 & 0x7F;
            var_a0_2 = (var_a1 * (temp_t3 & 0x7F)) - 1;
            if (var_a0_2 >= 0x800) {
                var_a0_2 = 0x7FF;
            }
            var_a2_2 = var_a1;
            if (var_a1 < 0) {
                var_a2_2 = var_a1 + 3;
            }
            temp_v0_4 = var_a2_2 >> 2;
            if (temp_v0_4 <= 0) {
                var_v0_3 = var_v1 & 0xFFF;
            } else {
                var_v0_3 = (var_v1 / temp_v0_4) & 0xFFF;
            }
            M2C_FIELD(temp_s1_3, s32 *, 4) = (s32) (((var_a0_2 & 0xFFF) << 0xC) | var_t6 | var_v0_3);
            temp_s0_9 = temp_s0_8 + 8;
            M2C_FIELD(temp_s0_9, s32 *, -4) = 0xE7000000;
            M2C_FIELD(temp_s0_8, s32 *, 8) = 0;
            temp_s0_10 = temp_s0_9 + 8;
            temp_a0_5 = temp_s5_4 & 0x7F;
            M2C_FIELD(temp_s0_10, s32 *, -4) = (s32) ((((u32) ((temp_a0_5 * 2) + 7) >> 3) << 9) | 0xF5100000);
            M2C_FIELD(temp_s0_9, s32 *, 8) = (s32) ((temp_t4 << 0x12) | ((temp_t1 & 0xF) << 0xE) | (temp_a3 << 8) | ((temp_v0_2 & 0xF) * 0x10));
            var_s0 = temp_s0_10 + 8;
            var_s1 = temp_s1_3 + 8 + 0x18;
            var_a0 = (((temp_a0_5 - 1) * 4) & 0xFFF) << 0xC;
            M2C_FIELD(var_s0, s32 *, -4) = 0xF2000000;
            var_v0_4 = (temp_t3 & 0x7F) - 1;
block_62:
            var_v0_2 = (var_v0_4 * 4) & 0xFFF;
            goto block_63;
        case 0x200:                                 /* switch 2 */
            temp_s0_11 = var_s0 + 8;
            M2C_FIELD(temp_s0_11, s32 *, -4) = 0xFD500000;
            temp_s1_4 = var_s1 + 0x18;
            var_s1_2 = temp_s1_4 + 8;
            var_a2_3 = 0x800;
            temp_a0_6 = temp_s4_2 >> 4;
            M2C_FIELD(var_s0, s32 *, 8) = (s32) M2C_FIELD((temp_s3_3 + arg0[5]), s32 *, 0xC);
            temp_s0_12 = temp_s0_11 + 8;
            M2C_FIELD(temp_s0_12, s32 *, -4) = 0xF5500000;
            M2C_FIELD(temp_s0_11, s32 *, 8) = (s32) ((var_a1 << 0x12) | (((temp_t1 & 0xF) << 0xE) | var_t6) | (temp_a3 << 8) | ((temp_v0_2 & 0xF) * 0x10));
            temp_s0_13 = temp_s0_12 + 8;
            M2C_FIELD(temp_s0_13, s32 *, -4) = 0xE6000000;
            M2C_FIELD(temp_s0_12, s32 *, 8) = 0;
            temp_s0_14 = temp_s0_13 + 8;
            M2C_FIELD(temp_s0_14, s32 *, -4) = 0xF3000000;
            if (temp_a0_6 != 0) {
                var_a2_3 = temp_a0_6 + 0x7FF;
            }
            var_v1_2 = ((s32) ((temp_s4_2 * temp_s6_2) + 3) >> 2) - 1;
            if (var_v1_2 >= 0x800) {
                var_v1_2 = 0x7FF;
            }
            var_a1 = ((var_v1_2 & 0xFFF) << 0xC) | var_t6;
            if (temp_a0_6 != 0) {
                var_v0_5 = (var_a2_3 / (s32) temp_a0_6) & 0xFFF;
            } else {
                var_v0_5 = var_a2_3 & 0xFFF;
            }
            M2C_FIELD(temp_s1_4, s32 *, 4) = (s32) (var_a1 | var_v0_5);
            temp_s0_15 = temp_s0_14 + 8;
            M2C_FIELD(temp_s0_15, s32 *, -4) = 0xE7000000;
            M2C_FIELD(temp_s0_14, s32 *, 8) = 0;
            var_s0_2 = temp_s0_15 + 8;
            var_a0_3 = temp_s5_4 & 0xFFFF;
            var_v0_6 = (((s32) ((var_a0_3 >> 1) + 7) >> 3) & 0x1FF) << 9;
            var_v1_3 = 0xF5400000;
block_61:
            M2C_FIELD(var_s0_2, s32 *, -4) = (s32) (var_v0_6 | var_v1_3);
            M2C_FIELD(var_s0_2, s32 *, 0) = (s32) ((temp_t4 << 0x12) | ((temp_t1 & 0xF) << 0xE) | (temp_a3 << 8) | ((temp_v0_2 & 0xF) * 0x10));
            var_s0 = var_s0_2 + 8;
            var_s1 = var_s1_2 + 0x18;
            var_a0 = (((var_a0_3 - 1) * 4) & 0xFFF) << 0xC;
            M2C_FIELD(var_s0, s32 *, -4) = 0xF2000000;
            var_v0_4 = temp_t3 - 1;
            goto block_62;
        case 0x100:                                 /* switch 2 */
            temp_s0_16 = var_s0 + 8;
            M2C_FIELD(temp_s0_16, s32 *, -4) = 0xFD500000;
            temp_s1_5 = var_s1 + 0x18;
            var_s1_2 = temp_s1_5 + 8;
            var_a2_4 = 0x800;
            temp_a0_7 = temp_s4_2 >> 3;
            M2C_FIELD(var_s0, s32 *, 8) = (s32) M2C_FIELD((temp_s3_3 + arg0[5]), s32 *, 0xC);
            temp_s0_17 = temp_s0_16 + 8;
            M2C_FIELD(temp_s0_17, s32 *, -4) = 0xF5500000;
            M2C_FIELD(temp_s0_16, s32 *, 8) = (s32) ((var_a1 << 0x12) | (((temp_t1 & 0xF) << 0xE) | var_t6) | (temp_a3 << 8) | ((temp_v0_2 & 0xF) * 0x10));
            temp_s0_18 = temp_s0_17 + 8;
            M2C_FIELD(temp_s0_18, s32 *, -4) = 0xE6000000;
            M2C_FIELD(temp_s0_17, s32 *, 8) = 0;
            temp_s0_19 = temp_s0_18 + 8;
            M2C_FIELD(temp_s0_19, s32 *, -4) = 0xF3000000;
            if (temp_a0_7 != 0) {
                var_a2_4 = temp_a0_7 + 0x7FF;
            }
            var_v1_4 = ((s32) ((temp_s4_2 * temp_s6_2) + 1) >> 1) - 1;
            if (var_v1_4 >= 0x800) {
                var_v1_4 = 0x7FF;
            }
            var_a1 = ((var_v1_4 & 0xFFF) << 0xC) | var_t6;
            if (temp_a0_7 != 0) {
                var_v0_7 = (var_a2_4 / (s32) temp_a0_7) & 0xFFF;
            } else {
                var_v0_7 = var_a2_4 & 0xFFF;
            }
            M2C_FIELD(temp_s1_5, s32 *, 4) = (s32) (var_a1 | var_v0_7);
            temp_s0_20 = temp_s0_19 + 8;
            M2C_FIELD(temp_s0_20, s32 *, -4) = 0xE7000000;
            M2C_FIELD(temp_s0_19, s32 *, 8) = 0;
            var_s0_2 = temp_s0_20 + 8;
            var_a0_3 = temp_s5_4 & 0xFFFF;
            var_v0_6 = (((s32) (var_a0_3 + 7) >> 3) & 0x1FF) << 9;
            var_v1_3 = 0xF5480000;
            goto block_61;
        }
        goto loop_4;
    case 0xC000:                                    /* switch 1 */
        M2C_FIELD((var_s0 + 8), s32 *, -4) = 0xB8000000;
        M2C_FIELD(var_s0, s32 *, 8) = 0;
        break;
    }
    Heap_Free(&gGameHeap, sp1C);
    return 0;
}

M2C_UNK *func_800182E4(M2C_UNK *arg0, M2C_UNK *arg1, s32 arg2) {
    M2C_UNK *var_a1;
    M2C_UNK *var_a3;
    s32 var_v1;
    u8 temp_v0;

    var_a1 = arg1;
    var_a3 = arg0;
    var_v1 = arg2 - 1;
    if (arg2 != 0) {
        do {
            temp_v0 = *var_a1;
            var_a1 += 1;
            var_v1 -= 1;
            *var_a3 = temp_v0;
            var_a3 += 1;
        } while (var_v1 != -1);
    }
    return arg0;
}

s32 func_8001831C(u32 arg0) {
    s32 var_v1;
    u32 temp_v0;
    u32 var_a1;

    var_a1 = arg0;
    var_v1 = 0;
    if (!(arg0 & 1)) {
        do {
            temp_v0 = (u32) (var_a1 & 0xFFFF) >> 1;
            var_a1 = temp_v0;
            var_v1 += 1;
        } while (!(temp_v0 & 1));
    }
    return var_v1 & 0xFFFF;
}

void *func_80018350(M2C_UNK *arg1, u8 *arg2, M2C_UNK *arg3) {
    void *temp_v0;

    temp_v0 = func_80019174((void *)4);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_800193F0;
    M2C_FIELD(temp_v0, u8 **, 0x5C) = arg2;
    if (arg1 == NULL) {
        M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
        M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038F20;
    } else {
        M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
        M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = arg1;
    }
    if (arg3 == NULL) {
        M2C_FIELD(temp_v0, s32 *, 0x38) = 0x06000000;
        M2C_FIELD(temp_v0, M2C_UNK **, 0x3C) = &D_80038F20;
    } else {
        M2C_FIELD(temp_v0, s32 *, 0x38) = 0x06000000;
        M2C_FIELD(temp_v0, M2C_UNK **, 0x3C) = arg3;
    }
    return temp_v0;
}

void func_800183F4(void) {
    void *temp_v0;

    temp_v0 = func_80019174((void *)4);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_800193F0;
    M2C_FIELD(temp_v0, M2C_UNK **, 0x5C) = &D_80038FA8;
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038F68;
}

void func_8001843C(void) {
    M2C_FIELD(func_80019174((void *)5), void (**)(void *), 0x74) = func_800195F4;
}

void func_80018464(M2C_UNK *arg1) {
    void *temp_v0;

    temp_v0 = func_80019174((void *)3);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_800193F0;
    M2C_FIELD(temp_v0, u16 *, 0x5A) = (u16) (M2C_FIELD(temp_v0, u16 *, 0x5A) | 0x20);
    M2C_FIELD(temp_v0, M2C_UNK **, 0x5C) = func_80017900(NULL, arg1);
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038DE0;
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xB8000000;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
}

void func_800184E4(M2C_UNK *arg1) {
    void *temp_v0;

    temp_v0 = func_80019174((void *)1);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_80019520;
    M2C_FIELD(temp_v0, u16 *, 0x5A) = (u16) (M2C_FIELD(temp_v0, u16 *, 0x5A) | 0x20);
    M2C_FIELD(temp_v0, M2C_UNK **, 0x5C) = Sprite_NewPlain(NULL, arg1);
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038C80;
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xE7000000;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 0xBA000C02;
    M2C_FIELD(temp_v0, s32 *, 0x44) = 0x2000;
}

void func_80018578(M2C_UNK *arg1) {
    void *temp_v0;

    temp_v0 = func_80019174((void *)1);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_80019520;
    M2C_FIELD(temp_v0, u16 *, 0x5A) = (u16) (M2C_FIELD(temp_v0, u16 *, 0x5A) | 0x20);
    M2C_FIELD(temp_v0, M2C_UNK **, 0x5C) = Sprite_NewPlain(NULL, arg1);
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038CC0;
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xE7000000;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 0xBA000C02;
    M2C_FIELD(temp_v0, s32 *, 0x44) = 0x2000;
}

void func_8001860C(void *arg0) {
    M2C_FIELD(arg0, s32 *, 8) = 0x06000000;
    M2C_FIELD(arg0, M2C_UNK **, 0xC) = &D_80038CC0;
}

void func_80018628(void *arg0) {
    M2C_FIELD(arg0, s32 *, 8) = 0x06000000;
    M2C_FIELD(arg0, M2C_UNK **, 0xC) = &D_80038C80;
}

void func_80018644(M2C_UNK *arg1) {
    void *temp_v0;

    temp_v0 = func_80019174((void *)1);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_80019520;
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038C80;
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xE7000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0x5C) = arg1;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 0xBA000C02;
    M2C_FIELD(temp_v0, s32 *, 0x44) = 0x2000;
}

void func_800186B0(M2C_UNK *arg1) {
    void *temp_v0;

    temp_v0 = func_80019174((void *)1);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_80019520;
    M2C_FIELD(temp_v0, u16 *, 0x5A) = (u16) (M2C_FIELD(temp_v0, u16 *, 0x5A) | 0x20);
    M2C_FIELD(temp_v0, M2C_UNK **, 0x5C) = Sprite_New(NULL, arg1);
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038D00;
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xE7000000;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 0xBA000C02;
    M2C_FIELD(temp_v0, s32 *, 0x44) = 0x2000;
    M2C_FIELD(temp_v0, s32 *, 0x48) = 0xBA001701;
    M2C_FIELD(temp_v0, s32 *, 0x4C) = 0;
}

void func_80018754(M2C_UNK *arg1) {
    void *temp_v0;

    temp_v0 = func_80019174((void *)1);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_80019520;
    M2C_FIELD(temp_v0, u16 *, 0x5A) = (u16) (M2C_FIELD(temp_v0, u16 *, 0x5A) | 0x20);
    M2C_FIELD(temp_v0, M2C_UNK **, 0x5C) = Sprite_New(NULL, arg1);
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038D50;
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xE7000000;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 0xBA000C02;
    M2C_FIELD(temp_v0, s32 *, 0x44) = 0x2000;
    M2C_FIELD(temp_v0, s32 *, 0x48) = 0xBA001701;
    M2C_FIELD(temp_v0, s32 *, 0x4C) = 0;
}

void func_800187F8(M2C_UNK *arg1) {
    void *temp_v0;

    temp_v0 = func_80019174((void *)1);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_80019520;
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038D00;
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xE7000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0x5C) = arg1;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 0xBA000C02;
    M2C_FIELD(temp_v0, s32 *, 0x44) = 0x2000;
    M2C_FIELD(temp_v0, s32 *, 0x48) = 0xBA001701;
    M2C_FIELD(temp_v0, s32 *, 0x4C) = 0;
}

void func_80018874(M2C_UNK *arg1) {
    void *temp_v0;

    temp_v0 = func_80019174((void *)2);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_80019688;
    M2C_FIELD(temp_v0, u16 *, 0x5A) = (u16) (M2C_FIELD(temp_v0, u16 *, 0x5A) | 0x20);
    M2C_FIELD(temp_v0, M2C_UNK **, 0x5C) = Sprite_NewScreen(NULL, arg1);
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038DA0;
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xE7000000;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 0xBA001301;
    M2C_FIELD(temp_v0, s32 *, 0x44) = 0x80000;
}

void func_80018908(M2C_UNK *arg1, u8 *arg2) {
    void *temp_v0;

    temp_v0 = func_80019174(NULL);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_800193F0;
    M2C_FIELD(temp_v0, u16 *, 0x5A) = (u16) (M2C_FIELD(temp_v0, u16 *, 0x5A) | 0x20);
    M2C_FIELD(temp_v0, void **, 0x5C) = func_80017030();
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0x60) = arg1;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038D00;
    func_80017080(M2C_FIELD(temp_v0, void **, 0x5C), M2C_FIELD(temp_v0, M2C_UNK **, 0x60), arg2, 0.0f, 0.0f);
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xE7000000;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 0xBA000C02;
    M2C_FIELD(temp_v0, s32 *, 0x44) = 0x2000;
    M2C_FIELD(temp_v0, s32 *, 0x48) = 0xBA001701;
    M2C_FIELD(temp_v0, s32 *, 0x4C) = 0;
}

void func_800189D4(void *arg1, u8 *arg2, s32 arg3) {
    void *temp_v0;

    temp_v0 = func_80019174(NULL);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_800193F0;
    M2C_FIELD(temp_v0, u16 *, 0x5A) = (u16) (M2C_FIELD(temp_v0, u16 *, 0x5A) | 0x20);
    M2C_FIELD(temp_v0, void **, 0x5C) = func_80017030();
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, void **, 0x60) = arg1;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038D00;
    func_80017304(M2C_FIELD(temp_v0, void **, 0x5C), M2C_FIELD(temp_v0, void **, 0x60), arg2, 0.0f, 0.0f, arg3);
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xE7000000;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 0xBA000C02;
    M2C_FIELD(temp_v0, s32 *, 0x44) = 0x2000;
    M2C_FIELD(temp_v0, s32 *, 0x48) = 0xBA001701;
    M2C_FIELD(temp_v0, s32 *, 0x4C) = 0;
}

void func_80018AB0(M2C_UNK *arg1) {
    void *temp_v0;

    temp_v0 = func_80019174((void *)3);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_800193F0;
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038DE0;
    M2C_FIELD(temp_v0, M2C_UNK **, 0x5C) = arg1;
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xB8000000;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
}

void func_80018B08(M2C_UNK *arg1) {
    void *temp_v0;

    temp_v0 = func_80019174((void *)2);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_80019688;
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038DA0;
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xE7000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0x5C) = arg1;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 0xBA001301;
    M2C_FIELD(temp_v0, s32 *, 0x44) = 0x80000;
}

void func_80018B74(M2C_UNK *arg1, void *arg2, u8 *arg3) {
    void *temp_v0;

    temp_v0 = func_80019174(NULL);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_800193F0;
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, void **, 0x5C) = arg2;
    M2C_FIELD(temp_v0, M2C_UNK **, 0x60) = arg1;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038D00;
    func_80017080(M2C_FIELD(temp_v0, void **, 0x5C), M2C_FIELD(temp_v0, M2C_UNK **, 0x60), arg3, 0.0f, 0.0f);
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xE7000000;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 0xBA000C02;
    M2C_FIELD(temp_v0, s32 *, 0x44) = 0x2000;
    M2C_FIELD(temp_v0, s32 *, 0x48) = 0xBA001701;
    M2C_FIELD(temp_v0, s32 *, 0x4C) = 0;
}

void *func_80018C3C(void *arg1, void *arg2, u8 *arg3, s32 arg4) {
    void *temp_v0;

    temp_v0 = func_80019174(NULL);
    M2C_FIELD(temp_v0, void (**)(void *), 0x74) = func_800193F0;
    M2C_FIELD(temp_v0, s32 *, 8) = 0x06000000;
    M2C_FIELD(temp_v0, M2C_UNK **, 0xC) = &D_80038D00;
    M2C_FIELD(temp_v0, s32 *, 0x38) = 0xE7000000;
    M2C_FIELD(temp_v0, void **, 0x5C) = arg2;
    M2C_FIELD(temp_v0, void **, 0x60) = arg1;
    M2C_FIELD(temp_v0, s32 *, 0x3C) = 0;
    M2C_FIELD(temp_v0, s32 *, 0x40) = 0xBA000C02;
    M2C_FIELD(temp_v0, s32 *, 0x44) = 0x2000;
    M2C_FIELD(temp_v0, s32 *, 0x48) = 0xBA001701;
    M2C_FIELD(temp_v0, s32 *, 0x4C) = 0;
    func_80017304(M2C_FIELD(temp_v0, void **, 0x5C), M2C_FIELD(temp_v0, void **, 0x60), arg3, 0.0f, 0.0f, arg4);
    return temp_v0;
}

void func_80018D14(void *arg0, u8 *arg1) {
    func_80017080(M2C_FIELD(arg0, void **, 0x5C), M2C_FIELD(arg0, M2C_UNK **, 0x60), arg1, 0.0f, 0.0f);
    M2C_FIELD(arg0, u16 *, 0x5A) = (u16) (M2C_FIELD(arg0, u16 *, 0x5A) | 1);
}

void func_80018D64(void *arg0, u8 *arg1, s32 arg2) {
    func_80017304(M2C_FIELD(arg0, void **, 0x5C), M2C_FIELD(arg0, void **, 0x60), arg1, 0.0f, 0.0f, arg2);
    M2C_FIELD(arg0, u16 *, 0x5A) = (u16) (M2C_FIELD(arg0, u16 *, 0x5A) | 1);
}

void func_80018DBC(void) {
    M2C_UNK **var_v1;
    s32 *var_v1_2;
    s32 var_v0;
    s32 var_v0_2;

    var_v1 = &D_800A6B30;
    var_v0 = 0x2C;
    do {
        *var_v1 = NULL;
        var_v0 -= 1;
        var_v1 += 4;
    } while (var_v0 != 0);
    var_v1_2 = &D_800A6BE0;
    var_v0_2 = 0x2C;
    do {
        *var_v1_2 = 0;
        var_v0_2 -= 1;
        var_v1_2 += 4;
    } while (var_v0_2 != 0);
    M2C_FIELD(&D_800A6B34, s32 *, 0) = 0;
    D_800A6BE0 = 0;
    M2C_FIELD(&D_800A6B34, s32 **, -4) = &D_800A6BE0;
    *D_800A6BE4 = &D_800A6B34 - 4;
}

s32 func_80018E20(void *arg0) {
    M2C_FIELD(arg0, u16 *, 0x5A) = (u16) (M2C_FIELD(arg0, u16 *, 0x5A) | 4);
    return 0;
}

void *func_80018E34(void *arg0, s32 arg1) {
    s16 temp_v1;

    if (!(M2C_FIELD(arg0, u16 *, 0x5A) & 0x20)) {
        temp_v1 = M2C_FIELD(arg0, s16 *, 0x58);
        switch (temp_v1) {
        case 4:
            break;
        case 2:
            M2C_FIELD(arg0, u16 *, 0x5A) = (u16) (M2C_FIELD(arg0, u16 *, 0x5A) | 2);
            /* fallthrough */
        case 0:
        case 1:
        case 3:
            M2C_FIELD(arg0, s32 *, 0x5C) = arg1;
            break;
        }
        M2C_FIELD(arg0, u16 *, 0x5A) = (u16) (M2C_FIELD(arg0, u16 *, 0x5A) | 1);
    }
    return arg0;
}

void func_80018E90(void *arg0) {
    M2C_FIELD(arg0, u16 *, 0x5A) = (u16) (M2C_FIELD(arg0, u16 *, 0x5A) | 0x401);
}

void func_80018EA4(void *arg0) {
    M2C_FIELD(arg0, u16 *, 0x5A) = (u16) (M2C_FIELD(arg0, u16 *, 0x5A) | 0x201);
}

void func_80018EB8(void *arg0, s32 arg1, s32 arg2, s32 arg3, s32 arg4) {
    M2C_FIELD(arg0, s32 *, 0x28) = 0xFA000000;
    M2C_FIELD(arg0, s32 *, 0x2C) = (s32) ((arg1 << 0x18) | ((arg2 & 0xFF) << 0x10) | ((arg3 & 0xFF) << 8) | (arg4 & 0xFF));
}

void func_80018EF4(void *arg0, s16 arg1, s16 arg2, s16 arg3) {
    M2C_FIELD(arg0, f32 *, 0x78) = (f32) arg1;
    M2C_FIELD(arg0, f32 *, 0x7C) = (f32) arg2;
    M2C_FIELD(arg0, f32 *, 0x80) = (f32) arg3;
    M2C_FIELD(arg0, u16 *, 0x5A) = (u16) (M2C_FIELD(arg0, u16 *, 0x5A) | 2);
}

void func_80018F38(void *arg0) {
    M2C_FIELD(arg0, u16 *, 0x5A) = (u16) (M2C_FIELD(arg0, u16 *, 0x5A) | 0x43);
}

void func_80018F4C(void *arg0) {
    M2C_FIELD(arg0, u16 *, 0x5A) = (u16) (M2C_FIELD(arg0, u16 *, 0x5A) | 0x83);
}

void func_80018F60(void *arg0) {
    M2C_FIELD(arg0, u16 *, 0x5A) = (u16) (M2C_FIELD(arg0, u16 *, 0x5A) | 0x103);
}

void func_80018F74(void *arg0) {
    M2C_FIELD(arg0, u16 *, 0x5A) = (u16) (M2C_FIELD(arg0, u16 *, 0x5A) | 2);
}

void func_80018F88(void *arg0) {
    M2C_FIELD(arg0, u16 *, 0x5A) = (u16) ((M2C_FIELD(arg0, u16 *, 0x5A) & 0xFFEF) | 1);
}

void func_80018FA0(void *arg0) {
    M2C_FIELD(arg0, u16 *, 0x5A) = (u16) ((M2C_FIELD(arg0, u16 *, 0x5A) & 0xFFFE) | 0x10);
}

void func_80018FB8(void *arg0, s32 arg1, s32 arg2) {
    M2C_FIELD(arg0, s32 *, 0x64) = arg1;
    M2C_FIELD(arg0, s32 *, 0x68) = arg2;
}

void func_80018FC8(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x64) = 0;
}

void func_80018FD4(void *arg0) {
    M2C_FIELD(arg0, s32 *, 0x74) = 0;
}

void *func_80018FE0(void *arg0) {
    Mtx_Copy(M2C_FIELD(arg0, s32 *, 0x6C) + 0xC);
    return arg0;
}

void func_80019010(void *arg0, s32 arg1) {
    M2C_FIELD(arg0, s32 *, 0x18) = 0x03860010;
    M2C_FIELD(arg0, s32 *, 0x1C) = (s32) (arg1 + 0x48);
    M2C_FIELD(arg0, s32 *, 0x20) = 0x03880010;
    M2C_FIELD(arg0, s32 *, 0x24) = (s32) (arg1 + 0x40);
}

void func_80019040(void) {
    M2C_UNK (*temp_v0)(M2C_UNK *);
    M2C_UNK (*temp_v0_2)(M2C_UNK *);
    M2C_UNK *var_s0;
    M2C_UNK *var_s1;
    u16 temp_v1;
    u16 temp_v1_2;
    u16 var_v0;

    var_s0 = D_800A6B30;
    var_s1 = *var_s0;
    if (var_s1 != NULL) {
        do {
            temp_v1 = M2C_FIELD(var_s0, u16 *, 0x5A);
            if (!(temp_v1 & 4)) {
                if (temp_v1 & 8) {
                    func_8001933C(var_s0);
                    *var_s0[1] = M2C_FIELD(var_s0, M2C_UNK **, 0);
                    M2C_FIELD(var_s0, M2C_UNK **, 0)[1] = var_s0[1];
                    Heap_Free(&gGameHeap, var_s0);
                    var_s0 = var_s1;
                } else {
                    temp_v0 = var_s0[0x19];
                    if (temp_v0 != NULL) {
                        temp_v0(var_s0);
                    }
                    if (M2C_FIELD(var_s0, u16 *, 0x5A) & 2) {
                        temp_v0_2 = var_s0[0x1D];
                        if (temp_v0_2 != NULL) {
                            temp_v0_2(var_s0);
                        }
                        M2C_FIELD(var_s0, u16 *, 0x5A) = (u16) (M2C_FIELD(var_s0, u16 *, 0x5A) & 0xFFFD);
                    }
                    temp_v1_2 = M2C_FIELD(var_s0, u16 *, 0x5A);
                    if (!(temp_v1_2 & 0x10)) {
                        if (temp_v1_2 & 1) {
                            func_800196BC(var_s0);
                            var_v0 = M2C_FIELD(var_s0, u16 *, 0x5A) & 0xFFFE;
                            goto block_19;
                        }
                    } else {
                        func_800197C8(var_s0);
                        var_v0 = M2C_FIELD(var_s0, u16 *, 0x5A) & 0xFFEE;
                        goto block_19;
                    }
                    goto block_20;
                }
            } else {
                func_800193C4(var_s0);
                var_v0 = (M2C_FIELD(var_s0, u16 *, 0x5A) & 0xFFFB) | 8;
block_19:
                M2C_FIELD(var_s0, u16 *, 0x5A) = var_v0;
block_20:
                var_s0 = var_s1;
            }
            var_s1 = *var_s1;
        } while (var_s1 != NULL);
    }
}

void func_80019174(void *arg0, s16 arg1) {
    M2C_UNK *temp_v0;
    M2C_UNK *temp_v0_2;
    M2C_UNK *temp_v0_3;
    M2C_UNK *var_v1;
    s32 var_v0;

    temp_v0 = Heap_Alloc(&gGameHeap, 0xB0);
    var_v1 = temp_v0;
    var_v0 = 0x2C;
    do {
        *var_v1 = NULL;
        var_v0 -= 1;
        var_v1 += 4;
    } while (var_v0 != 0);
    **D_800A6BE4 = temp_v0;
    M2C_FIELD(temp_v0, M2C_UNK **, 0) = D_800A6BE4 - 4;
    temp_v0[1] = *D_800A6BE4;
    *D_800A6BE4 = temp_v0;
    M2C_FIELD(temp_v0, s16 *, 0x5A) = 2;
    M2C_FIELD(temp_v0, s16 *, 0x58) = arg1;
    if (arg1 != 5) {
        temp_v0[0x29] = (M2C_UNK *)0x3F800000;
        temp_v0[0x28] = (M2C_UNK *)0x3F800000;
        temp_v0[0x27] = (M2C_UNK *)0x3F800000;
        temp_v0_2 = func_800116B8(2);
        temp_v0[0x1B] = temp_v0_2;
        *M2C_FIELD(arg0, M2C_UNK **, 0xD4)[1] = temp_v0_2;
        temp_v0[0x1B][1] = M2C_FIELD(arg0, M2C_UNK **, 0xD4)[1];
        M2C_FIELD(temp_v0[0x1B], M2C_UNK **, 0) = M2C_FIELD(arg0, M2C_UNK **, 0xD4);
        M2C_FIELD(arg0, M2C_UNK **, 0xD4)[1] = temp_v0[0x1B];
        temp_v0[7] = (M2C_UNK *) &D_800C1778;
        temp_v0[9] = &D_800C1778 - 8;
        temp_v0[0xA] = (M2C_UNK *)0xFA000000;
        temp_v0[0xB] = (M2C_UNK *)0xFF;
        temp_v0[4] = (M2C_UNK *)0xBC000002;
        temp_v0[5] = (M2C_UNK *)0x80000040;
        temp_v0[6] = (M2C_UNK *)0x03860010;
        temp_v0[8] = (M2C_UNK *)0x03880010;
        temp_v0[0xC] = (M2C_UNK *)0xB8000000;
        temp_v0[0xD] = NULL;
    } else {
        temp_v0_3 = func_800116B8(4);
        temp_v0[0x1B] = temp_v0_3;
        *M2C_FIELD(arg0, M2C_UNK **, 0xD4)[1] = temp_v0_3;
        temp_v0[0x1B][1] = M2C_FIELD(arg0, M2C_UNK **, 0xD4)[1];
        M2C_FIELD(temp_v0[0x1B], M2C_UNK **, 0) = M2C_FIELD(arg0, M2C_UNK **, 0xD4);
        M2C_FIELD(arg0, M2C_UNK **, 0xD4)[1] = temp_v0[0x1B];
        temp_v0[2] = (M2C_UNK *)0xB8000000;
        temp_v0[3] = NULL;
    }
    temp_v0[0xE] = (M2C_UNK *)0xB8000000;
    temp_v0[0xF] = NULL;
    temp_v0[0x10] = (M2C_UNK *)0xB8000000;
    temp_v0[0x11] = NULL;
    temp_v0[0x12] = (M2C_UNK *)0xB8000000;
    temp_v0[0x13] = NULL;
    temp_v0[0x14] = (M2C_UNK *)0xB8000000;
    temp_v0[0x15] = NULL;
}

void func_8001933C(M2C_UNK *arg0) {
    s16 temp_v1;

    if (M2C_FIELD(arg0, u16 *, 0x5A) & 0x20) {
        temp_v1 = M2C_FIELD(arg0, s16 *, 0x58);
        switch (temp_v1) {
        case 0:
            func_8001705C(arg0[0x17]);
            arg0[0x17] = NULL;
            return;
        case 1:
        case 2:
            Sprite_Delete(arg0[0x17]);
            arg0[0x17] = NULL;
            return;
        case 3:
            func_80017A08(arg0[0x17]);
            arg0[0x17] = NULL;
            break;
        }
    } else {
    default:
    }
}

void func_800193C4(M2C_UNK *arg0) {
    func_800117BC(arg0[0x1B]);
    arg0[0x1B] = NULL;
}

void func_800193F0(void *arg0) {
    f32 sp18;
    f32 sp58;
    f32 sp6C;
    f32 sp80;

    Mtx_Translate(&sp18, M2C_FIELD(arg0, f32 *, 0x78), M2C_FIELD(arg0, f32 *, 0x7C), M2C_FIELD(arg0, M2C_UNK32 *, 0x80));
    func_80008750(&sp58, &sp18, M2C_FIELD(arg0, f32 *, 0x9C), M2C_FIELD(arg0, f32 *, 0xA0), M2C_FIELD(arg0, f32 *, 0xA4));
    func_80008540(&sp18, M2C_BITWISE(f32, &sp58), M2C_FIELD(arg0, f32 *, 0x88), M2C_FIELD(arg0, s32 *, 0x84), M2C_FIELD(arg0, f32 *, 0x8C));
    if (M2C_FIELD(arg0, u16 *, 0x5A) & 0x1C0) {
        Mtx_Identity(&sp58);
        if (M2C_FIELD(arg0, u16 *, 0x5A) & 0x40) {
            sp58 = -1.0f;
        }
        if (M2C_FIELD(arg0, u16 *, 0x5A) & 0x80) {
            sp6C = -1.0f;
        }
        if (M2C_FIELD(arg0, u16 *, 0x5A) & 0x100) {
            sp80 = -1.0f;
        }
        Mtx_Mul((s32) &sp18, (s32) &sp58, (s32) &sp18);
    }
    func_800085B4(M2C_FIELD(arg0, s32 *, 0x6C) + 0xC, &sp18, -M2C_FIELD(arg0, f32 *, 0x90), -M2C_FIELD(arg0, f32 *, 0x94), -M2C_FIELD(arg0, f32 *, 0x98));
}

void func_80019520(void *arg0) {
    f32 sp18;
    f32 sp58;

    Mtx_Translate(&sp18, M2C_FIELD(arg0, f32 *, 0x78) - 160.0f, 120.0f - M2C_FIELD(arg0, f32 *, 0x7C), M2C_FIELD(arg0, M2C_UNK32 *, 0x80));
    func_80008750(&sp58, &sp18, M2C_FIELD(arg0, f32 *, 0x9C), M2C_FIELD(arg0, f32 *, 0xA0), M2C_FIELD(arg0, f32 *, 0xA4));
    func_80008540(&sp18, M2C_BITWISE(f32, &sp58), M2C_FIELD(arg0, f32 *, 0x88), M2C_FIELD(arg0, s32 *, 0x84), M2C_FIELD(arg0, f32 *, 0x8C));
    func_800085B4(M2C_FIELD(arg0, s32 *, 0x6C) + 0xC, &sp18, -M2C_FIELD(arg0, f32 *, 0x90), -M2C_FIELD(arg0, f32 *, 0x94), -M2C_FIELD(arg0, f32 *, 0x98));
}

void func_800195F4(void *arg0) {
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x6C), s16 *, 0xD0) = (s16) (s32) M2C_FIELD(arg0, f32 *, 0x78);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x6C), s16 *, 0xD2) = (s16) (s32) M2C_FIELD(arg0, f32 *, 0x7C);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x6C), s16 *, 0xD4) = (s16) (s32) M2C_FIELD(arg0, f32 *, 0x80);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x6C), s16 *, 0xD6) = (s16) (s32) M2C_FIELD(arg0, f32 *, 0x84);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x6C), s16 *, 0xD8) = (s16) (s32) M2C_FIELD(arg0, f32 *, 0x88);
    M2C_FIELD(M2C_FIELD(arg0, void **, 0x6C), s16 *, 0xDA) = (s16) (s32) M2C_FIELD(arg0, f32 *, 0x8C);
}

void func_80019688(void *arg0) {
    Sprite_DrawScreen(M2C_FIELD(arg0, f32 *, 0x5C), M2C_FIELD(arg0, f32 *, 0x78), M2C_FIELD(arg0, f32 *, 0x7C), M2C_FIELD(arg0, f32 *, 0x9C), M2C_FIELD(arg0, f32 *, 0xA0));
}

void func_800196BC(M2C_UNK *arg0) {
    M2C_UNK *temp_v1;
    u16 temp_v1_2;

    if (M2C_FIELD(arg0, s16 *, 0x58) != 5) {
        M2C_FIELD(arg0[0x1B], void **, 0xD4) = (void *) (arg0 + 8);
        M2C_FIELD(arg0[0x1B], void **, 0xD8) = (void *) (arg0 + 0x38);
    }
    switch ((u32) M2C_FIELD(arg0, s16 *, 0x58)) {
    case 0:
        temp_v1 = arg0[0x17];
        M2C_FIELD(arg0[0x1B], M2C_UNK **, 0xD0) = (M2C_UNK *) (temp_v1 + ((M2C_FIELD(temp_v1, s32 *, 0x1508) * 0x280) + 0x1008));
        return;
    case 1:
    case 2:
        M2C_FIELD(arg0[0x1B], M2C_UNK **, 0xD0) = (M2C_UNK *) arg0[0x17][5];
        return;
    case 3:
        temp_v1_2 = M2C_FIELD(arg0, u16 *, 0x5A);
        if (temp_v1_2 & 0x400) {
            arg0[2] = (M2C_UNK *)0x06000000;
            arg0[3] = &D_80038EE0;
        } else if (temp_v1_2 & 0x200) {
            if (temp_v1_2 & 0x1C0) {
                arg0[2] = (M2C_UNK *)0x06000000;
                arg0[3] = &D_80038EA0;
            } else {
                arg0[2] = (M2C_UNK *)0x06000000;
                arg0[3] = &D_80038E60;
            }
        }
        M2C_FIELD(arg0[0x1B], M2C_UNK **, 0xD0) = (M2C_UNK *) arg0[0x17][6];
        return;
    case 4:
        M2C_FIELD(arg0[0x1B], M2C_UNK **, 0xD0) = (M2C_UNK *) arg0[0x17];
        /* fallthrough */
    default:
        return;
    }
}

void func_800197C8(M2C_UNK *arg0) {
    if (M2C_FIELD(arg0, s16 *, 0x58) != 5) {
        M2C_FIELD(arg0[0x1B], s32 *, 0xD4) = 0;
        M2C_FIELD(arg0[0x1B], s32 *, 0xD0) = 0;
        M2C_FIELD(arg0[0x1B], s32 *, 0xD8) = 0;
    }
}

void func_80019800(void ***arg0, M2C_UNK ***arg1) {
    M2C_UNK ***var_s0;
    M2C_UNK *var_v0;
    s16 temp_v1;
    u8 temp_v0;
    void ***var_s0_2;
    void *temp_a0;
    void *temp_v0_2;
    void *var_v0_2;

    if (*arg1 != NULL) {
        var_s0 = arg1;
        do {
            temp_v1 = M2C_FIELD(var_s0, s16 *, 8);
            if (temp_v1 != 2) {
                if (temp_v1 >= 3) {
                    if (temp_v1 != 4) {
                        if (temp_v1 != 9) {
                            var_s0 += 0xC;
                        } else {
                            var_v0 = Sprite_NewScreen(NULL, (M2C_FIELD(var_s0, s32 *, 4) & 0xFFFFFF) + &D_611F0);
                            goto block_15;
                        }
                    } else {
                        var_v0 = Sprite_New(NULL, (M2C_FIELD(var_s0, s32 *, 4) & 0xFFFFFF) + &D_611F0);
                        goto block_15;
                    }
                } else if (temp_v1 != 0) {
                    var_s0 += 0xC;
                } else {
                    var_v0 = func_80017900(NULL, (M2C_FIELD(var_s0, s32 *, 4) & 0xFFFFFF) + &D_611F0);
                    goto block_15;
                }
            } else {
                var_v0 = Sprite_NewPlain(NULL, (M2C_FIELD(var_s0, s32 *, 4) & 0xFFFFFF) + &D_611F0);
block_15:
                *M2C_FIELD(var_s0, M2C_UNK ***, 0) = var_v0;
                var_s0 += 0xC;
            }
        } while (*var_s0 != NULL);
    }
    if (*arg0 != NULL) {
        var_s0_2 = arg0;
        do {
            temp_v0 = M2C_FIELD(var_s0_2, u8 *, 0x21);
            switch (temp_v0) {
            case 13:
                var_v0_2 = func_8001843C(*M2C_FIELD(var_s0_2, M2C_UNK ***, 4));
block_33:
                *M2C_FIELD(var_s0_2, void ***, 0) = var_v0_2;
                break;
            case 11:
                var_v0_2 = func_800183F4(*M2C_FIELD(var_s0_2, M2C_UNK ***, 4));
                goto block_33;
            case 8:
                var_v0_2 = func_80018908(*M2C_FIELD(var_s0_2, M2C_UNK ***, 4), M2C_FIELD(M2C_FIELD(var_s0_2, void **, 8), u8 **, 0), " ");
                goto block_33;
            case 12:
                temp_v0_2 = M2C_FIELD(var_s0_2, void **, 8);
                var_v0_2 = func_80018350(*M2C_FIELD(var_s0_2, M2C_UNK ***, 4), M2C_FIELD(temp_v0_2, u8 **, 0), M2C_FIELD(temp_v0_2, M2C_UNK **, 4), M2C_FIELD(temp_v0_2, s32 *, 8));
                goto block_33;
            case 1:
                var_v0_2 = func_80018AB0(*M2C_FIELD(var_s0_2, M2C_UNK ***, 4), M2C_FIELD(M2C_FIELD(var_s0_2, void **, 8), u8 **, 0));
                goto block_33;
            case 2:
                var_v0_2 = func_800184E4(*M2C_FIELD(var_s0_2, M2C_UNK ***, 4), ((s32) M2C_FIELD(var_s0_2, void **, 8) & 0xFFFFFF) + &D_611F0);
                goto block_33;
            case 3:
                var_v0_2 = func_80018578(*M2C_FIELD(var_s0_2, M2C_UNK ***, 4), ((s32) M2C_FIELD(var_s0_2, void **, 8) & 0xFFFFFF) + &D_611F0);
                goto block_33;
            case 4:
                var_v0_2 = func_800186B0(*M2C_FIELD(var_s0_2, M2C_UNK ***, 4), ((s32) M2C_FIELD(var_s0_2, void **, 8) & 0xFFFFFF) + &D_611F0);
                goto block_33;
            case 7:
                var_v0_2 = func_80018644(*M2C_FIELD(var_s0_2, M2C_UNK ***, 4), M2C_FIELD(M2C_FIELD(var_s0_2, void **, 8), u8 **, 0));
                goto block_33;
            case 6:
                var_v0_2 = func_800187F8(*M2C_FIELD(var_s0_2, M2C_UNK ***, 4), M2C_FIELD(M2C_FIELD(var_s0_2, void **, 8), u8 **, 0));
                goto block_33;
            case 5:
                var_v0_2 = func_80018754(*M2C_FIELD(var_s0_2, M2C_UNK ***, 4), ((s32) M2C_FIELD(var_s0_2, void **, 8) & 0xFFFFFF) + &D_611F0);
                goto block_33;
            case 10:
                var_v0_2 = func_80018B08(*M2C_FIELD(var_s0_2, M2C_UNK ***, 4), M2C_FIELD(M2C_FIELD(var_s0_2, void **, 8), u8 **, 0));
                goto block_33;
            }
            if (M2C_FIELD(var_s0_2, u8 *, 0x21) != 0xD) {
                func_80018EB8(*M2C_FIELD(var_s0_2, void ***, 0), 0, 0, 0, (s32) M2C_FIELD(var_s0_2, u16 *, 0xC));
                func_80018EF4(*M2C_FIELD(var_s0_2, void ***, 0), M2C_FIELD(var_s0_2, s16 *, 0xE), M2C_FIELD(var_s0_2, s16 *, 0x10), M2C_FIELD(var_s0_2, s16 *, 0x12));
                if (((f64) M2C_FIELD(var_s0_2, f32 *, 0x14) != 1.0) || ((f64) M2C_FIELD(var_s0_2, f32 *, 0x18) != 1.0) || ((f64) M2C_FIELD(var_s0_2, f32 *, 0x1C) != 1.0)) {
                    M2C_FIELD(*M2C_FIELD(var_s0_2, void ***, 0), f32 *, 0x9C) = (f32) M2C_FIELD(var_s0_2, f32 *, 0x14);
                    M2C_FIELD(*M2C_FIELD(var_s0_2, void ***, 0), f32 *, 0xA0) = (f32) M2C_FIELD(var_s0_2, f32 *, 0x18);
                    M2C_FIELD(*M2C_FIELD(var_s0_2, void ***, 0), f32 *, 0xA4) = (f32) M2C_FIELD(var_s0_2, f32 *, 0x1C);
                }
                if (M2C_FIELD(var_s0_2, u8 *, 0x20) != 0) {
                    temp_a0 = *M2C_FIELD(var_s0_2, void ***, 0);
                    var_s0_2 += 0x24;
                    func_80018F88(temp_a0);
                } else {
                    goto block_42;
                }
            } else {
                M2C_FIELD(*M2C_FIELD(var_s0_2, void ***, 0), f32 *, 0x78) = (f32) M2C_FIELD(var_s0_2, s16 *, 0xE);
                M2C_FIELD(*M2C_FIELD(var_s0_2, void ***, 0), f32 *, 0x7C) = (f32) M2C_FIELD(var_s0_2, s16 *, 0x10);
                M2C_FIELD(*M2C_FIELD(var_s0_2, void ***, 0), f32 *, 0x80) = (f32) (M2C_FIELD(var_s0_2, s16 *, 0x12) - 1);
                M2C_FIELD(*M2C_FIELD(var_s0_2, void ***, 0), f32 *, 0x84) = (f32) (M2C_FIELD(var_s0_2, f32 *, 0x14) - 1.0f);
                M2C_FIELD(*M2C_FIELD(var_s0_2, void ***, 0), f32 *, 0x88) = (f32) (s16) (s32) (M2C_FIELD(var_s0_2, f32 *, 0x18) * 8192.0f);
                M2C_FIELD(*M2C_FIELD(var_s0_2, void ***, 0), f32 *, 0x8C) = (f32) (s16) (s32) (M2C_FIELD(var_s0_2, f32 *, 0x1C) * 8192.0f);
block_42:
                var_s0_2 += 0x24;
            }
        } while (*var_s0_2 != NULL);
    }
}

void func_80019C9C(void ***arg0, void *arg1) {
    s16 temp_v1;
    void ***var_s0;
    void *temp_a0;
    void *var_s0_2;

    if (*arg0 != NULL) {
        var_s0 = arg0;
        do {
            temp_a0 = **var_s0;
            var_s0 += 0x24;
            func_80018E20(temp_a0);
        } while (*var_s0 != NULL);
    }
    if (M2C_FIELD(arg1, s32 *, 4) != 0) {
        var_s0_2 = arg1;
        do {
            temp_v1 = M2C_FIELD(var_s0_2, s16 *, 8);
            switch (temp_v1) {                      /* irregular */
            case 0:
                func_80017A08(*M2C_FIELD(var_s0_2, M2C_UNK ***, 0));
                break;
            default:
            case 2:
                Sprite_Delete(*M2C_FIELD(var_s0_2, M2C_UNK ***, 0));
                break;
            }
            *M2C_FIELD(var_s0_2, M2C_UNK ***, 0) = NULL;
            var_s0_2 += 0xC;
        } while (M2C_FIELD(var_s0_2, s32 *, 4) != 0);
    }
}

void Jpeg64_Decode(u8 *arg0, s32 arg1, s32 arg2, s32 arg3, s32 *arg4, s32 *arg5) {
    s32 sp14;
    s32 sp1C;
    u8 *sp24;
    s16 *var_v0;
    s32 temp_s2;
    s32 temp_s6;
    s32 temp_v1;
    s32 var_a0;
    s32 var_a2;
    s32 var_s0;
    s32 var_s1;
    s32 var_v1;
    u16 *var_a1;
    u16 *var_t0;
    u16 temp_s5;
    u8 *temp_v0;

    sp14 = arg2;
    sp1C = arg3;
    temp_v0 = Heap_AllocFront(&gGameHeap, 0x50);
    Rom_Read(arg0, temp_v0, 0x50);
    sp24 = Heap_AllocFront(&gGameHeap, M2C_FIELD(temp_v0, s32 *, 4) + 8);
    temp_s2 = M2C_FIELD(temp_v0, s32 *, 0x34);
    temp_s6 = M2C_FIELD(temp_v0, s32 *, 0x40);
    temp_s5 = M2C_FIELD(temp_v0, u16 *, 0x4E);
    Rom_Read(arg0, sp24, M2C_FIELD(temp_v0, s32 *, 4) + 8);
    Heap_Free(&gGameHeap, (M2C_UNK *) temp_v0);
    *arg4 = temp_s2;
    *arg5 = temp_s6;
    Jpeg64_Init(sp24 + 0x58);
    var_v1 = 5;
    var_v0 = &D_800A7FAA;
    do {
        *var_v0 = 0;
        var_v1 -= 1;
        var_v0 -= 2;
    } while (var_v1 >= 0);
    var_s1 = 0;
    if (temp_s6 > 0) {
        do {
            var_s0 = 0;
            if (temp_s2 > 0) {
                do {
                    Jpeg64_DecodeMacroblock(&D_800A7CA0, &D_800A7FA0);
                    Jpeg64_Dequant(D_800A79A0, &D_800A7CA0, 1.0f / (f32) (s16) temp_s5);
                    Jpeg64_Idct(&D_800A76A0, D_800A79A0);
                    Jpeg64_Upsample(D_800A70A0, &D_800A76A0);
                    Jpeg64_YCbCrToRgba5551(D_800A6E98, D_800A70A0);
                    var_a2 = 0;
                    var_t0 = D_800A6E98;
loop_7:
                    var_a0 = 0;
                    var_a1 = var_t0;
loop_8:
                    temp_v1 = ((var_s1 + var_a2 + sp1C) * temp_s2) + var_s0 + var_a0;
                    var_a0 += 1;
                    *(((temp_v1 + sp14) * 2) + arg1) = *var_a1;
                    var_a1 += 2;
                    if (var_a0 < 0x10) {
                        goto loop_8;
                    }
                    var_a2 += 1;
                    var_t0 += 0x20;
                    if (var_a2 < 0x10) {
                        goto loop_7;
                    }
                    var_s0 += 0x10;
                } while (var_s0 < temp_s2);
            }
            var_s1 += 0x10;
        } while (var_s1 < temp_s6);
    }
    Heap_Free(&gGameHeap, (M2C_UNK *) sp24);
}

void Jpeg64_Init(u8 *arg0) {
    f32 *temp_v0;
    f32 *var_s1;
    f32 *var_s2;
    f32 *var_s4;
    f32 *var_v1;
    f32 temp_f0;
    s32 var_a1;
    s32 var_s0;
    s32 var_s0_2;
    s32 var_s3;

    var_s0 = 0;
    var_a1 = 0;
    var_v1 = D_800A6C98;
    D_800A6C90 = 0;
    D_800A6C94 = arg0;
    do {
        *var_v1 = 0.35355338f;
        *(D_800A6D98 + var_a1) = 0.35355338f;
        var_a1 += 0x20;
        var_s0 += 1;
        var_v1 += 4;
    } while (var_s0 < 8);
    var_s3 = 1;
    var_s4 = D_800A6CB8;
    do {
        var_s0_2 = 0;
        var_s2 = D_800A6D98;
        var_s1 = var_s4;
loop_4:
        temp_f0 = func_8001C450((((2.0f * (f32) var_s0_2) + 1.0f) * (f32) var_s3 * 3.1415927f) / 16.0f) * 0.5f;
        var_s0_2 += 1;
        temp_v0 = &var_s2[var_s3];
        var_s2 += 0x20;
        *var_s1 = temp_f0;
        *temp_v0 = temp_f0;
        var_s1 += 4;
        if (var_s0_2 < 8) {
            goto loop_4;
        }
        var_s3 += 1;
        var_s4 += 0x20;
    } while (var_s3 < 8);
}

s32 func_8001A118(void) {
    return D_800A6C90;
}

void func_8001A124(s16 *arg0, u8 *arg1) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f10;
    f32 temp_f6;
    f32 temp_f8;
    f32 var_f0;
    f32 var_f0_2;
    f32 var_f0_3;
    s16 *var_a0;
    s32 var_t0;
    u8 *var_a1;
    void *var_a2;
    void *var_a3;

    var_a0 = arg0;
    var_a1 = arg1;
    var_t0 = 0xFF;
    var_a2 = var_a0 + 4;
    var_a3 = var_a1 + 2;
    do {
        temp_f10 = (f32) *var_a1;
        temp_f8 = (f32) M2C_FIELD(var_a3, u8 *, -1);
        temp_f6 = (f32) M2C_FIELD(var_a3, u8 *, 0);
        temp_f0 = (temp_f10 * 0.299f) + (temp_f8 * 0.587f) + (temp_f6 * 0.114f);
        if (!(temp_f0 < 0.0f)) {
            var_f0 = temp_f0 + 0.5f;
        } else {
            var_f0 = temp_f0 - 0.5f;
        }
        temp_f0_2 = ((temp_f10 * -0.1684f) - (temp_f8 * 0.3316f)) + (temp_f6 * 0.5f);
        *var_a0 = (s16) (s32) var_f0;
        if (temp_f0_2 < 0.0f) {
            var_f0_2 = temp_f0_2 - 0.5f;
        } else {
            var_f0_2 = temp_f0_2 + 0.5f;
        }
        temp_f0_3 = ((temp_f10 * 0.5f) - (temp_f8 * 0.4187f)) - (temp_f6 * 0.0813f);
        M2C_FIELD(var_a2, s16 *, -2) = (s16) (s32) var_f0_2;
        if (temp_f0_3 < 0.0f) {
            var_f0_3 = temp_f0_3 - 0.5f;
        } else {
            var_f0_3 = temp_f0_3 + 0.5f;
        }
        M2C_FIELD(var_a2, s16 *, 0) = (s16) (s32) var_f0_3;
        var_a3 += 3;
        var_a1 += 3;
        var_a2 += 6;
        var_t0 -= 1;
        var_a0 += 6;
    } while (var_t0 != -1);
}

void Jpeg64_YCbCrToRgba5551(u16 *arg0, s16 *arg1) {
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f4;
    f32 temp_f6;
    f32 temp_f8;
    f32 var_f0;
    f32 var_f0_2;
    f32 var_f0_3;
    s16 *var_a1;
    s32 var_a0;
    s32 var_a2;
    s32 var_t0;
    s32 var_v1;
    u16 *var_t1;
    void *var_a3;

    var_a1 = arg1;
    var_t1 = arg0;
    var_t0 = 0xFF;
    var_a3 = var_a1 + 4;
    do {
        temp_f6 = (f32) M2C_FIELD(var_a3, s16 *, 0);
        temp_f4 = (f32) *var_a1;
        temp_f0 = temp_f4 + (temp_f6 * 1.402f);
        temp_f8 = (f32) M2C_FIELD(var_a3, s16 *, -2);
        if (!(temp_f0 < 0.0f)) {
            var_f0 = temp_f0 + 0.5f;
        } else {
            var_f0 = temp_f0 - 0.5f;
        }
        var_a2 = (s32) var_f0;
        temp_f0_2 = (temp_f4 - (temp_f8 * 0.3441f)) - (temp_f6 * 0.7139f);
        if (!(temp_f0_2 < 0.0f)) {
            var_f0_2 = temp_f0_2 + 0.5f;
        } else {
            var_f0_2 = temp_f0_2 - 0.5f;
        }
        var_v1 = (s32) var_f0_2;
        temp_f0_3 = (temp_f4 + (temp_f8 * 1.7718f)) - (temp_f6 * 0.0012f);
        if (!(temp_f0_3 < 0.0f)) {
            var_f0_3 = temp_f0_3 + 0.5f;
        } else {
            var_f0_3 = temp_f0_3 - 0.5f;
        }
        var_a0 = (s32) var_f0_3;
        if (var_a2 < 0) {
            var_a2 = 0;
        } else if (var_a2 >= 0x100) {
            var_a2 = 0xFF;
        }
        if (var_v1 < 0) {
            var_v1 = 0;
        } else if (var_v1 >= 0x100) {
            var_v1 = 0xFF;
        }
        if (var_a0 < 0) {
            var_a0 = 0;
        } else if (var_a0 >= 0x100) {
            var_a0 = 0xFF;
        }
        var_a3 += 6;
        var_a1 += 6;
        var_t0 -= 1;
        *var_t1 = (*(&D_8003A014 + var_a2) << 0xB) + (*(&D_8003A014 + var_v1) << 6) + (*(&D_8003A014 + var_a0) * 2) + 1;
        var_t1 += 2;
    } while (var_t0 != -1);
}

void func_8001A4A0(void *arg0, void *arg1) {
    s32 var_t0;
    s32 var_t2;
    u16 temp_v0;
    void *var_a0;
    void *var_a1;
    void *var_a2;
    void *var_a3;
    void *var_t1;
    void *var_v1;

    var_a0 = arg0;
    var_a1 = arg1;
    var_t2 = 0;
    var_t1 = var_a1;
    do {
        var_t0 = 0;
        var_a3 = var_a1;
        var_v1 = var_a0;
        var_a2 = var_t1;
loop_2:
        M2C_FIELD(var_v1, s16 *, 0) = (s16) (M2C_FIELD(var_a2, u16 *, 0) - 0x80);
        M2C_FIELD(var_v1, s16 *, 0x80) = (s16) (M2C_FIELD(var_a2, u16 *, 0x30) - 0x80);
        M2C_FIELD(var_v1, s16 *, 0x100) = (s16) (M2C_FIELD(var_a2, u16 *, 0x300) - 0x80);
        M2C_FIELD(var_v1, s16 *, 0x180) = (s16) (M2C_FIELD(var_a2, u16 *, 0x330) - 0x80);
        var_t0 += 1;
        M2C_FIELD(var_v1, u16 *, 0x200) = (u16) M2C_FIELD(var_a3, u16 *, 2);
        temp_v0 = M2C_FIELD(var_a3, u16 *, 4);
        var_a2 += 6;
        var_a3 += 0xC;
        M2C_FIELD(var_v1, u16 *, 0x280) = temp_v0;
        var_v1 += 2;
        if (var_t0 < 8) {
            goto loop_2;
        }
        var_a1 += 0xC0;
        var_t1 += 0x60;
        var_t2 += 1;
        var_a0 += 0x10;
    } while (var_t2 < 8);
}

void Jpeg64_Upsample(s16 *arg0, M2C_UNK *arg1) {
    M2C_UNK *var_a1;
    M2C_UNK *var_a2;
    s16 *var_a0;
    s16 *var_a3;
    s16 *var_t1;
    s16 *var_v1;
    s32 var_t0;
    s32 var_t2;
    u16 temp_v0;
    u16 temp_v0_2;

    var_a0 = arg0;
    var_a1 = arg1;
    var_t2 = 0;
    var_t1 = var_a0;
    do {
        var_t0 = 0;
        var_v1 = var_a0;
        var_a3 = var_t1;
        var_a2 = var_a1;
loop_2:
        M2C_FIELD(var_a3, s16 *, 0) = M2C_FIELD(var_a2, u16 *, 0) + 0x80;
        var_a3[0x18] = M2C_FIELD(var_a2, u16 *, 0x80) + 0x80;
        var_a3[0x180] = M2C_FIELD(var_a2, u16 *, 0x100) + 0x80;
        var_a3[0x198] = M2C_FIELD(var_a2, u16 *, 0x180) + 0x80;
        temp_v0 = M2C_FIELD(var_a2, u16 *, 0x200);
        var_t0 += 1;
        var_v1[0x34] = temp_v0;
        var_v1[0x31] = temp_v0;
        var_v1[4] = temp_v0;
        var_v1[1] = temp_v0;
        temp_v0_2 = M2C_FIELD(var_a2, u16 *, 0x280);
        var_a3 += 6;
        var_a2 += 2;
        var_v1[0x35] = temp_v0_2;
        var_v1[0x32] = temp_v0_2;
        var_v1[5] = temp_v0_2;
        var_v1[2] = temp_v0_2;
        var_v1 += 0xC;
        if (var_t0 < 8) {
            goto loop_2;
        }
        var_a0 += 0xC0;
        var_a1 += 0x10;
        var_t2 += 1;
        var_t1 += 0x60;
    } while (var_t2 < 8);
}

void func_8001A5D8(void *arg0, s16 *arg1) {
    M2C_UNK sp20;
    M2C_UNK *var_v1_3;
    f32 *var_a2;
    f32 *var_a2_2;
    f32 *var_a2_3;
    f32 *var_t1;
    f32 *var_t2_2;
    f32 *var_t4;
    f32 *var_v1;
    f32 *var_v1_2;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f0_4;
    f32 temp_f0_5;
    f32 temp_f0_6;
    f32 temp_f0_7;
    f32 temp_f0_8;
    f32 temp_f0_9;
    f32 temp_f12;
    f32 temp_f12_2;
    f32 temp_f12_3;
    f32 temp_f14;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 var_f0;
    f32 var_f0_2;
    f32 var_f0_3;
    f32 var_f0_4;
    f32 var_f0_5;
    f32 var_f0_6;
    f32 var_f0_7;
    f32 var_f0_8;
    f32 var_f16;
    f32 var_f18;
    f32 var_f22;
    f32 var_f24;
    f32 var_f26;
    f32 var_f28;
    f32 var_f30;
    f32 var_t3_2;
    s16 *var_a1;
    s16 *var_t0;
    s16 *var_t3;
    s32 var_a3;
    s32 var_a3_2;
    s32 var_a3_3;
    s32 var_t1_2;
    s32 var_t2;
    s32 var_t5;
    s32 var_t8;
    void *var_a0;
    void *var_t0_2;

    var_a0 = arg0;
    var_a1 = arg1;
    var_t8 = 0;
    do {
        var_t4 = sp;
        var_t5 = 0;
        var_t3 = var_a1;
loop_2:
        var_a3 = 0;
        var_a2 = var_t4;
        var_v1 = D_800A6D98;
loop_3:
        temp_f0 = (f32) *var_t3 * *var_v1;
        var_v1 += 4;
        var_a3 += 1;
        *var_a2 = temp_f0;
        var_a2 += 4;
        if (var_a3 < 8) {
            goto loop_3;
        }
        var_t2 = 1;
        var_t1 = D_800A6D98 + 0x20;
        var_t0 = var_t3 + 2;
loop_5:
        var_a3_2 = 0;
        var_v1_2 = var_t4;
        var_a2_2 = var_t1;
loop_6:
        temp_f2 = *var_v1_2 + ((f32) *var_t0 * *var_a2_2);
        var_a2_2 += 4;
        var_a3_2 += 1;
        *var_v1_2 = temp_f2;
        var_v1_2 += 4;
        if (var_a3_2 < 8) {
            goto loop_6;
        }
        var_t1 += 0x20;
        var_t2 += 1;
        var_t0 += 2;
        if (var_t2 < 8) {
            goto loop_5;
        }
        var_t4 += 0x20;
        var_t5 += 0x20;
        var_t3 += 0x10;
        if (var_t5 < 0x100) {
            goto loop_2;
        }
        var_t0_2 = var_a0;
        var_t1_2 = 0;
        var_t2_2 = D_800A6C98;
loop_10:
        temp_f12 = *(D_800A6C98 + var_t1_2);
        var_t3_2 = temp_f12 * sp0;
        var_f16 = temp_f12 * sp4;
        var_f18 = temp_f12 * sp8;
        var_f22 = temp_f12 * spC;
        var_f24 = temp_f12 * sp10;
        var_f26 = temp_f12 * sp14;
        var_f28 = temp_f12 * sp18;
        var_a3_3 = 1;
        var_v1_3 = &sp20;
        var_a2_3 = &var_t2_2[1];
        var_f30 = temp_f12 * sp1C;
loop_11:
        temp_f12_2 = *var_a2_3;
        temp_f14 = var_t3_2 + (temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 0));
        var_f16 += temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 4);
        var_f18 += temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 8);
        var_f22 += temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 0xC);
        var_f24 += temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 0x10);
        var_f26 += temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 0x14);
        temp_f2_2 = temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 0x1C);
        var_a2_3 += 4;
        var_a3_3 += 1;
        var_f28 += temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 0x18);
        var_v1_3 += 0x20;
        var_t3_2 = temp_f14;
        var_f30 += temp_f2_2;
        if (var_a3_3 < 7) {
            goto loop_11;
        }
        temp_f12_3 = *(&D_800A6CB4 + var_t1_2);
        temp_f0_2 = temp_f14 + (temp_f12_3 * spE0);
        if (temp_f0_2 < 0.0f) {
            var_f0 = temp_f0_2 - 0.5f;
        } else {
            var_f0 = temp_f0_2 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 0) = (s16) (s32) var_f0;
        temp_f0_3 = var_f16 + (temp_f12_3 * spE4);
        if (temp_f0_3 < 0.0f) {
            var_f0_2 = temp_f0_3 - 0.5f;
        } else {
            var_f0_2 = temp_f0_3 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 2) = (s16) (s32) var_f0_2;
        temp_f0_4 = var_f18 + (temp_f12_3 * spE8);
        if (temp_f0_4 < 0.0f) {
            var_f0_3 = temp_f0_4 - 0.5f;
        } else {
            var_f0_3 = temp_f0_4 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 4) = (s16) (s32) var_f0_3;
        temp_f0_5 = var_f22 + (temp_f12_3 * spEC);
        if (temp_f0_5 < 0.0f) {
            var_f0_4 = temp_f0_5 - 0.5f;
        } else {
            var_f0_4 = temp_f0_5 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 6) = (s16) (s32) var_f0_4;
        temp_f0_6 = var_f24 + (temp_f12_3 * spF0);
        if (temp_f0_6 < 0.0f) {
            var_f0_5 = temp_f0_6 - 0.5f;
        } else {
            var_f0_5 = temp_f0_6 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 8) = (s16) (s32) var_f0_5;
        temp_f0_7 = var_f26 + (temp_f12_3 * spF4);
        if (temp_f0_7 < 0.0f) {
            var_f0_6 = temp_f0_7 - 0.5f;
        } else {
            var_f0_6 = temp_f0_7 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 0xA) = (s16) (s32) var_f0_6;
        temp_f0_8 = var_f28 + (temp_f12_3 * spF8);
        if (temp_f0_8 < 0.0f) {
            var_f0_7 = temp_f0_8 - 0.5f;
        } else {
            var_f0_7 = temp_f0_8 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 0xC) = (s16) (s32) var_f0_7;
        temp_f0_9 = var_f30 + (temp_f12_3 * spFC);
        if (temp_f0_9 < 0.0f) {
            var_f0_8 = temp_f0_9 - 0.5f;
        } else {
            var_f0_8 = temp_f0_9 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 0xE) = (s16) (s32) var_f0_8;
        var_t0_2 += 0x10;
        var_t1_2 += 0x20;
        var_t2_2 += 0x20;
        if (var_t1_2 < 0x100) {
            goto loop_10;
        }
        var_a0 += 0x80;
        var_t8 += 0x80;
        var_a1 += 0x80;
    } while (var_t8 < 0x300);
}

void Jpeg64_Idct(M2C_UNK *arg0, s16 *arg1) {
    M2C_UNK sp20;
    M2C_UNK *var_a0;
    M2C_UNK *var_t0_2;
    M2C_UNK *var_v1_3;
    f32 *var_a2;
    f32 *var_a2_2;
    f32 *var_a2_3;
    f32 *var_t1;
    f32 *var_t2_2;
    f32 *var_t4;
    f32 *var_v1;
    f32 *var_v1_2;
    f32 temp_f0;
    f32 temp_f0_2;
    f32 temp_f0_3;
    f32 temp_f0_4;
    f32 temp_f0_5;
    f32 temp_f0_6;
    f32 temp_f0_7;
    f32 temp_f0_8;
    f32 temp_f0_9;
    f32 temp_f12;
    f32 temp_f12_2;
    f32 temp_f12_3;
    f32 temp_f14;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 var_f0;
    f32 var_f0_2;
    f32 var_f0_3;
    f32 var_f0_4;
    f32 var_f0_5;
    f32 var_f0_6;
    f32 var_f0_7;
    f32 var_f0_8;
    f32 var_f16;
    f32 var_f18;
    f32 var_f22;
    f32 var_f24;
    f32 var_f26;
    f32 var_f28;
    f32 var_f30;
    f32 var_t3_2;
    s16 *var_a1;
    s16 *var_t0;
    s16 *var_t3;
    s32 var_a3;
    s32 var_a3_2;
    s32 var_a3_3;
    s32 var_t1_2;
    s32 var_t2;
    s32 var_t5;
    s32 var_t8;

    var_a0 = arg0;
    var_a1 = arg1;
    var_t8 = 0;
    do {
        var_t4 = sp;
        var_t5 = 0;
        var_t3 = var_a1;
loop_2:
        var_a3 = 0;
        var_a2 = var_t4;
        var_v1 = D_800A6C98;
loop_3:
        temp_f0 = (f32) *var_t3 * *var_v1;
        var_v1 += 4;
        var_a3 += 1;
        *var_a2 = temp_f0;
        var_a2 += 4;
        if (var_a3 < 8) {
            goto loop_3;
        }
        var_t2 = 1;
        var_t1 = D_800A6C98 + 0x20;
        var_t0 = var_t3 + 2;
loop_5:
        var_a3_2 = 0;
        var_v1_2 = var_t4;
        var_a2_2 = var_t1;
loop_6:
        temp_f2 = *var_v1_2 + ((f32) *var_t0 * *var_a2_2);
        var_a2_2 += 4;
        var_a3_2 += 1;
        *var_v1_2 = temp_f2;
        var_v1_2 += 4;
        if (var_a3_2 < 8) {
            goto loop_6;
        }
        var_t1 += 0x20;
        var_t2 += 1;
        var_t0 += 2;
        if (var_t2 < 8) {
            goto loop_5;
        }
        var_t4 += 0x20;
        var_t5 += 0x20;
        var_t3 += 0x10;
        if (var_t5 < 0x100) {
            goto loop_2;
        }
        var_t0_2 = var_a0;
        var_t1_2 = 0;
        var_t2_2 = D_800A6D98;
loop_10:
        temp_f12 = *(D_800A6D98 + var_t1_2);
        var_t3_2 = temp_f12 * sp0;
        var_f16 = temp_f12 * sp4;
        var_f18 = temp_f12 * sp8;
        var_f22 = temp_f12 * spC;
        var_f24 = temp_f12 * sp10;
        var_f26 = temp_f12 * sp14;
        var_f28 = temp_f12 * sp18;
        var_a3_3 = 1;
        var_v1_3 = &sp20;
        var_a2_3 = &var_t2_2[1];
        var_f30 = temp_f12 * sp1C;
loop_11:
        temp_f12_2 = *var_a2_3;
        temp_f14 = var_t3_2 + (temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 0));
        var_f16 += temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 4);
        var_f18 += temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 8);
        var_f22 += temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 0xC);
        var_f24 += temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 0x10);
        var_f26 += temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 0x14);
        temp_f2_2 = temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 0x1C);
        var_a2_3 += 4;
        var_a3_3 += 1;
        var_f28 += temp_f12_2 * M2C_FIELD(var_v1_3, f32 *, 0x18);
        var_v1_3 += 0x20;
        var_t3_2 = temp_f14;
        var_f30 += temp_f2_2;
        if (var_a3_3 < 7) {
            goto loop_11;
        }
        temp_f12_3 = *(&D_800A6DB4 + var_t1_2);
        temp_f0_2 = temp_f14 + (temp_f12_3 * spE0);
        if (temp_f0_2 < 0.0f) {
            var_f0 = temp_f0_2 - 0.5f;
        } else {
            var_f0 = temp_f0_2 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 0) = (s16) (s32) var_f0;
        temp_f0_3 = var_f16 + (temp_f12_3 * spE4);
        if (temp_f0_3 < 0.0f) {
            var_f0_2 = temp_f0_3 - 0.5f;
        } else {
            var_f0_2 = temp_f0_3 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 2) = (s16) (s32) var_f0_2;
        temp_f0_4 = var_f18 + (temp_f12_3 * spE8);
        if (temp_f0_4 < 0.0f) {
            var_f0_3 = temp_f0_4 - 0.5f;
        } else {
            var_f0_3 = temp_f0_4 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 4) = (s16) (s32) var_f0_3;
        temp_f0_5 = var_f22 + (temp_f12_3 * spEC);
        if (temp_f0_5 < 0.0f) {
            var_f0_4 = temp_f0_5 - 0.5f;
        } else {
            var_f0_4 = temp_f0_5 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 6) = (s16) (s32) var_f0_4;
        temp_f0_6 = var_f24 + (temp_f12_3 * spF0);
        if (temp_f0_6 < 0.0f) {
            var_f0_5 = temp_f0_6 - 0.5f;
        } else {
            var_f0_5 = temp_f0_6 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 8) = (s16) (s32) var_f0_5;
        temp_f0_7 = var_f26 + (temp_f12_3 * spF4);
        if (temp_f0_7 < 0.0f) {
            var_f0_6 = temp_f0_7 - 0.5f;
        } else {
            var_f0_6 = temp_f0_7 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 0xA) = (s16) (s32) var_f0_6;
        temp_f0_8 = var_f28 + (temp_f12_3 * spF8);
        if (temp_f0_8 < 0.0f) {
            var_f0_7 = temp_f0_8 - 0.5f;
        } else {
            var_f0_7 = temp_f0_8 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 0xC) = (s16) (s32) var_f0_7;
        temp_f0_9 = var_f30 + (temp_f12_3 * spFC);
        if (temp_f0_9 < 0.0f) {
            var_f0_8 = temp_f0_9 - 0.5f;
        } else {
            var_f0_8 = temp_f0_9 + 0.5f;
        }
        M2C_FIELD(var_t0_2, s16 *, 0xE) = (s16) (s32) var_f0_8;
        var_t0_2 += 0x10;
        var_t1_2 += 0x20;
        var_t2_2 += 0x20;
        if (var_t1_2 < 0x100) {
            goto loop_10;
        }
        var_a0 += 0x80;
        var_t8 += 0x80;
        var_a1 += 0x80;
    } while (var_t8 < 0x300);
}

void func_8001AEA8(s32 arg0, s16 *arg1, f32 arg2) {
    M2C_UNK *var_t3;
    f32 temp_f2;
    f32 var_f2;
    s16 *var_a1;
    s16 *var_a3;
    s16 *var_t2;
    s32 temp_v0;
    s32 var_a0;
    s32 var_a2;
    s32 var_t0;
    s32 var_t1;
    s32 var_t5;

    var_a0 = arg0;
    var_a1 = arg1;
    var_t5 = 0;
    do {
        var_t3 = &gJpeg64QuantChroma;
        var_t1 = 0;
        if (var_t5 < 4) {
            var_t3 = &gJpeg64QuantLuma;
        }
        var_t2 = var_a1;
loop_4:
        var_t0 = 0;
        var_a2 = var_t1 * 8;
        var_a3 = var_t2;
loop_5:
        temp_f2 = (f32) *var_a3;
        if (temp_f2 == 0.0f) {
            *((var_a2 * 2) + var_a0) = 0;
        } else {
            temp_v0 = var_a2 * 4;
            if (temp_f2 < 0.0f) {
                var_f2 = (temp_f2 / (*(temp_v0 + var_t3) * arg2)) - 0.5f;
            } else {
                var_f2 = (temp_f2 / (*(temp_v0 + var_t3) * arg2)) + 0.5f;
            }
            *((var_a2 * 2) + var_a0) = (s16) (s32) var_f2;
        }
        var_a2 += 1;
        var_t0 += 1;
        var_a3 += 2;
        if (var_t0 < 8) {
            goto loop_5;
        }
        var_t1 += 1;
        var_t2 += 0x10;
        if (var_t1 < 8) {
            goto loop_4;
        }
        var_a0 += 0x80;
        var_t5 += 1;
        var_a1 += 0x80;
    } while (var_t5 < 6);
}

void Jpeg64_Dequant(s16 *arg0, M2C_UNK *arg1, f32 arg2) {
    M2C_UNK *var_a1;
    M2C_UNK *var_t2;
    f32 temp_f0;
    s16 *var_a0;
    s16 *var_a3;
    s16 *var_t1;
    s32 temp_v1;
    s32 var_a2;
    s32 var_t0;
    s32 var_t4;

    var_a0 = arg0;
    var_a1 = arg1;
    var_t4 = 0;
    do {
        var_t2 = &gJpeg64QuantChroma;
        var_t0 = 0;
        if (var_t4 < 4) {
            var_t2 = &gJpeg64QuantLuma;
        }
        var_t1 = var_a0;
loop_4:
        var_a2 = 0;
        var_a3 = var_t1;
loop_5:
        temp_v1 = (var_t0 * 8) + var_a2;
        temp_f0 = (f32) *((temp_v1 * 2) + var_a1) * (*((temp_v1 * 4) + var_t2) * arg2);
        var_a2 += 1;
        *var_a3 = (s16) (s32) temp_f0;
        var_a3 += 2;
        if (var_a2 < 8) {
            goto loop_5;
        }
        var_t0 += 1;
        var_t1 += 0x10;
        if (var_t0 < 8) {
            goto loop_4;
        }
        var_a1 += 0x80;
        var_t4 += 1;
        var_a0 += 0x80;
    } while (var_t4 < 6);
}

void func_8001B064(s32 arg0, s32 arg1) {
    s32 sp14;
    M2C_UNK *var_a1;
    M2C_UNK *var_a2;
    M2C_UNK *var_s3;
    M2C_UNK *var_s4;
    s16 temp_v0_2;
    s16 temp_v1_2;
    s16 var_s2;
    s16 var_s5;
    s16 var_v0_2;
    s16 var_v1;
    s32 temp_v0_3;
    s32 var_s0;
    s32 var_v0;
    s32 var_v0_3;
    u16 *temp_v0;
    u16 *temp_v1;
    u16 temp_a0;
    u16 temp_a3;

    var_s5 = 0;
    sp14 = arg1;
    var_v0 = 0 << 0x10;
    do {
        var_s0 = 0;
        if ((var_v0 >> 0x10) < 4) {
            var_a1 = &D_800392D0;
            var_a2 = &D_800392F0;
            var_s4 = &D_80039350;
            var_s3 = &D_80039550;
        } else {
            var_a1 = &D_80039310;
            var_a2 = &D_80039330;
            var_s4 = &D_80039750;
            var_s3 = &D_80039950;
        }
        temp_v1 = (var_s5 << 7) + arg0;
        temp_a3 = *temp_v1;
        temp_v0 = (var_s5 * 2) + sp14;
        temp_a0 = *temp_v0;
        var_s2 = 1;
        *temp_v0 = temp_a3;
        func_8001B4D0((s16) (temp_a3 - temp_a0), var_a1, var_a2, temp_a3);
loop_6:
        var_v1 = var_s2;
loop_7:
        temp_v0_2 = *((*(((s32) (var_v1 << 0x10) >> 0xE) + &gJpeg64Zigzag) * 2) + temp_v1);
        temp_v1_2 = var_v1 + 1;
        var_s2 = temp_v1_2;
        if (temp_v0_2 == 0) {
            var_s0 += 1;
            if (temp_v1_2 < 0x40) {
                var_v1 = var_s2;
                goto loop_7;
            }
            func_8001B55C(0, 0, var_s4, var_s3);
            var_v0_2 = var_s5 + 1;
        } else {
            var_v0_3 = var_s0 << 0x10;
loop_12:
            if ((var_v0_3 >> 0x10) >= 0x10) {
                func_8001B55C(0, 0xF, var_s4, var_s3);
                temp_v0_3 = var_s0 - 0x10;
                var_s0 = temp_v0_3;
                var_v0_3 = temp_v0_3 << 0x10;
                goto loop_12;
            }
            func_8001B55C(temp_v0_2, var_s0 & 0xFFFF, var_s4, var_s3);
            var_s0 = 0;
            if (var_s2 >= 0x40) {
                var_v0_2 = var_s5 + 1;
            } else {
                goto loop_6;
            }
        }
        var_s5 = var_v0_2;
        var_v0 = var_s5 << 0x10;
    } while (var_v0_2 < 6);
}

void Jpeg64_DecodeMacroblock(M2C_UNK *arg0, M2C_UNK *arg1) {
    s16 sp18;
    s16 sp1A;
    M2C_UNK *sp24;
    M2C_UNK *var_a0;
    M2C_UNK *var_s6;
    s16 *temp_v1;
    s16 temp_v0_2;
    s16 temp_v0_3;
    s16 temp_v0_4;
    s16 temp_v1_3;
    s16 var_s0;
    s16 var_s2;
    s16 var_v0_2;
    s32 temp_v0;
    s32 temp_v0_5;
    s32 temp_v1_2;
    s32 var_v0;
    u16 *temp_a1;
    void *var_a1;
    void *var_a2;
    void *var_a3;
    void *var_s3;
    void *var_s4;
    void *var_s5;

    var_s2 = 0;
    sp24 = arg1;
    var_v0 = 0 << 0x10;
    do {
        if ((var_v0 >> 0x10) < 4) {
            var_a0 = &gJpeg64HuffDcLuma;
            var_a1 = &gJpeg64HuffDcLuma + 0x24;
            var_a2 = &gJpeg64HuffDcLuma + 0x48;
            var_a3 = &gJpeg64HuffDcLuma + 0x6C;
            var_s6 = &gJpeg64HuffAcLuma;
            var_s5 = &gJpeg64HuffAcLuma + 0x24;
            var_s4 = &gJpeg64HuffAcLuma + 0x48;
            var_s3 = &gJpeg64HuffAcLuma + 0x6C;
        } else {
            var_a0 = &gJpeg64HuffDcChroma;
            var_a1 = &gJpeg64HuffDcChroma + 0x24;
            var_a2 = &gJpeg64HuffDcChroma + 0x48;
            var_a3 = &gJpeg64HuffDcChroma + 0x6C;
            var_s6 = &gJpeg64HuffAcChroma;
            var_s5 = &gJpeg64HuffAcChroma + 0x24;
            var_s4 = &gJpeg64HuffAcChroma + 0x48;
            var_s3 = &gJpeg64HuffAcChroma + 0x6C;
        }
        var_s0 = 1;
        temp_v0 = Jpeg64_DecodeDC(var_a0, var_a1, var_a2, var_a3);
        temp_a1 = (var_s2 * 2) + sp24;
        temp_v1 = (var_s2 << 7) + arg0;
        *temp_v1 = temp_v0 + *temp_a1;
        *temp_a1 += temp_v0;
loop_5:
        Jpeg64_DecodeAC(&sp18, &sp1A, var_s6, var_s5, var_s4, var_s3);
        if ((sp1A == 0) && (sp18 == 0)) {
            var_v0_2 = var_s2 + 1;
            if (var_s0 < 0x40) {
                do {
                    temp_v1_2 = *(((s32) (var_s0 << 0x10) >> 0xE) + &gJpeg64Zigzag);
                    temp_v0_2 = var_s0 + 1;
                    var_s0 = temp_v0_2;
                    *((temp_v1_2 * 2) + temp_v1) = 0;
                } while (temp_v0_2 < 0x40);
                var_v0_2 = var_s2 + 1;
            }
        } else {
            temp_v0_3 = (u16) sp1A - 1;
            sp1A = temp_v0_3;
            if (temp_v0_3 != -1) {
                do {
                    temp_v0_4 = (u16) sp1A - 1;
                    *((*(((s32) (var_s0 << 0x10) >> 0xE) + &gJpeg64Zigzag) * 2) + temp_v1) = 0;
                    sp1A = temp_v0_4;
                    var_s0 += 1;
                } while (temp_v0_4 != -1);
            }
            temp_v1_3 = var_s0 + 1;
            temp_v0_5 = *(((s32) (var_s0 << 0x10) >> 0xE) + &gJpeg64Zigzag);
            var_s0 = temp_v1_3;
            *((temp_v0_5 * 2) + temp_v1) = (u16) sp18;
            if (temp_v1_3 >= 0x40) {
                var_v0_2 = var_s2 + 1;
            } else {
                goto loop_5;
            }
        }
        var_s2 = var_v0_2;
        var_v0 = var_s2 << 0x10;
    } while (var_v0_2 < 6);
}

void func_8001B4D0(s16 arg0, M2C_UNK *arg1, M2C_UNK *arg2) {
    s16 var_s1;
    s16 var_v1;
    s32 temp_v0;
    u16 var_s0;

    var_s1 = arg0;
    if (arg0 & 0x8000) {
        var_v1 = -arg0;
    } else {
        var_v1 = var_s1;
    }
    var_s0 = 0;
    if (var_v1 & 0xFFFF) {
        do {
            var_v1 = (s16) ((u32) (var_v1 & 0xFFFF) >> 1);
            var_s0 += 1;
        } while (var_v1 != 0);
    }
    temp_v0 = var_s0 * 2;
    Jpeg64_PutBits(*(temp_v0 + arg1), *(temp_v0 + arg2));
    if (var_s0 != 0) {
        if (var_s1 & 0x8000) {
            var_s1 -= 1;
        }
        Jpeg64_PutBits(var_s1 & 0xFFFF, var_s0);
    }
}

void func_8001B55C(s16 arg0, s32 arg1, M2C_UNK *arg2, M2C_UNK *arg3) {
    s16 var_s1;
    s16 var_v1;
    s32 temp_v0;
    u16 var_s0;

    var_s1 = arg0;
    if (arg0 & 0x8000) {
        var_v1 = -arg0;
    } else {
        var_v1 = var_s1;
    }
    var_s0 = 0;
    if (var_v1 & 0xFFFF) {
        do {
            var_v1 = (s16) ((u32) (var_v1 & 0xFFFF) >> 1);
            var_s0 += 1;
        } while (var_v1 != 0);
    }
    temp_v0 = (var_s0 + ((arg1 & 0xFFFF) * 0x10)) * 2;
    Jpeg64_PutBits(*(temp_v0 + arg2), *(temp_v0 + arg3));
    if (var_s0 != 0) {
        if (var_s1 & 0x8000) {
            var_s1 -= 1;
        }
        Jpeg64_PutBits(var_s1 & 0xFFFF, var_s0);
    }
}

s32 Jpeg64_DecodeDC(M2C_UNK *arg0, void *arg1, void *arg2, void *arg3) {
    s32 temp_v1_2;
    s32 var_s0;
    s32 var_s1;
    s32 var_v0;
    s32 var_v1;
    s32 var_v1_2;
    u16 temp_a0;
    u16 temp_s0;
    u16 temp_v1;

    var_s1 = 1;
    temp_v1 = M2C_FIELD(arg2, u16 *, 2);
    var_s0 = Jpeg64_GetBit();
    if ((temp_v1 < (u32) (var_s0 & 0xFFFF)) || (var_v1 = 1 * 2, (temp_v1 == 0xFFFF))) {
        do {
loop_2:
            var_s1 += 1;
            temp_a0 = *((var_s1 * 2) + arg2);
            var_s0 = Jpeg64_GetBit() | (var_s0 * 2);
            if (temp_a0 < (u32) (var_s0 & 0xFFFF)) {
                goto loop_2;
            }
            var_v1 = var_s1 * 2;
        } while (temp_a0 == 0xFFFF);
    }
    temp_s0 = *((((*(var_v1 + arg0) + (var_s0 & 0xFFFF)) - *(var_v1 + arg1)) * 2) + arg3);
    var_v1_2 = 0;
    if (temp_s0 != 0) {
        temp_v1_2 = Jpeg64_GetBits(temp_s0) & 0xFFFF;
        var_v0 = temp_v1_2;
        if (!((temp_v1_2 >> (temp_s0 - 1)) & 1)) {
            var_v1_2 = (temp_v1_2 + 1) - (1 << temp_s0);
            goto block_7;
        }
    } else {
block_7:
        var_v0 = var_v1_2;
    }
    return var_v0;
}

void Jpeg64_DecodeAC(s16 *arg0, s16 *arg1, M2C_UNK *arg2, void *arg3, void *arg4, void *arg5) {
    s16 temp_v0;
    s32 var_s0;
    s32 var_s1;
    s32 var_v1;
    u16 temp_a0;
    u16 temp_s0;
    u16 temp_s0_2;
    u16 temp_v1;

    var_s1 = 1;
    temp_v1 = M2C_FIELD(arg4, u16 *, 2);
    var_s0 = Jpeg64_GetBit();
    if ((temp_v1 < (u32) (var_s0 & 0xFFFF)) || (var_v1 = 1 * 2, (temp_v1 == 0xFFFF))) {
        do {
loop_2:
            var_s1 += 1;
            temp_a0 = *((var_s1 * 2) + arg4);
            var_s0 = Jpeg64_GetBit() | (var_s0 * 2);
            if (temp_a0 < (u32) (var_s0 & 0xFFFF)) {
                goto loop_2;
            }
            var_v1 = var_s1 * 2;
        } while (temp_a0 == 0xFFFF);
    }
    temp_s0 = *((((*(var_v1 + arg2) + (var_s0 & 0xFFFF)) - *(var_v1 + arg3)) * 2) + arg5);
    temp_s0_2 = temp_s0 & 0xF;
    *arg1 = (s16) ((s32) temp_s0 >> 4);
    if (temp_s0_2 != 0) {
        temp_v0 = Jpeg64_GetBits(temp_s0_2);
        *arg0 = temp_v0;
        if (!((temp_v0 >> (temp_s0_2 - 1)) & 1)) {
            *arg0 = (temp_v0 + 1) - (1 << temp_s0_2);
        }
    } else {
        *arg0 = 0;
    }
}

s32 func_8001B84C(u32 arg0, s32 arg1) {
    s32 temp_a1;
    s32 temp_v0;
    s32 var_a2;
    s32 var_v1;
    u32 var_a0;

    var_a0 = arg0;
    temp_a1 = arg1 - 1;
    var_a2 = temp_a1;
    var_v1 = 0;
    if ((temp_a1 & 0xFFFF) != 0xFFFF) {
        do {
            var_v1 = (var_v1 * 2) | (var_a0 & 1);
            var_a0 = (u32) (var_a0 & 0xFFFF) >> 1;
            temp_v0 = var_a2 - 1;
            var_a2 = temp_v0;
        } while ((temp_v0 & 0xFFFF) != 0xFFFF);
    }
    return var_v1 & 0xFFFF;
}

void Jpeg64_PutBits(u32 arg0, s32 arg1) {
    s32 temp_a2;
    s32 temp_a3;
    s32 temp_t2;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_a2;
    s32 var_v0;
    s32 var_v1;
    u32 var_a0;
    u32 var_a1;
    u32 var_t0;
    u8 *temp_a1;
    u8 temp_v0_3;

    var_a0 = arg0;
    var_t0 = var_a0;
    if (arg1 >= 2) {
        temp_v0 = arg1 - 1;
        var_a2 = temp_v0;
        var_a1 = 0;
        if ((temp_v0 & 0xFFFF) != 0xFFFF) {
            var_v1 = 0 * 2;
            do {
                var_a1 = var_v1 | (var_a0 & 1);
                var_a0 = (u32) (var_a0 & 0xFFFF) >> 1;
                temp_v0_2 = var_a2 - 1;
                var_a2 = temp_v0_2;
                var_v1 = var_a1 * 2;
            } while ((temp_v0_2 & 0xFFFF) != 0xFFFF);
        }
        var_t0 = var_a1;
    }
    var_v0 = D_800A6C90;
    if (D_800A6C90 < 0) {
        var_v0 = D_800A6C90 + 7;
    }
    temp_a3 = var_v0 >> 3;
    temp_t2 = D_800A6C90 - (temp_a3 * 8);
    temp_a2 = *(&gJpeg64HuffAcChroma + 0x1B0 + (arg1 * 4));
    temp_a1 = &D_800A6C94[temp_a3];
    temp_v0_3 = ((var_t0 & 0xFFFF & temp_a2) << temp_t2) | ((M2C_FIELD(temp_a1, u8 *, 0) + (M2C_FIELD(temp_a1, u8 *, 1) << 8) + (M2C_FIELD(temp_a1, u8 *, 2) << 0x10)) & ~(temp_a2 << temp_t2));
    M2C_FIELD(temp_a1, u8 *, 0) = temp_v0_3;
    M2C_FIELD(&D_800A6C94[temp_a3], s8 *, 1) = (s8) (temp_v0_3 >> 8);
    M2C_FIELD(&D_800A6C94[temp_a3], s8 *, 2) = (s8) (temp_v0_3 >> 0x10);
    D_800A6C90 += arg1;
}

s32 Jpeg64_GetBit(void) {
    s32 temp_a2;
    u8 *temp_v1;
    u8 temp_a0;
    u8 temp_a1;
    u8 temp_v0;

    temp_v1 = &D_800A6C94[(s32) D_800A6C90 >> 3];
    temp_v0 = M2C_FIELD(temp_v1, u8 *, 0);
    temp_a0 = M2C_FIELD(temp_v1, u8 *, 1);
    temp_a1 = M2C_FIELD(temp_v1, u8 *, 2);
    temp_a2 = D_800A6C90 & 7;
    D_800A6C90 += 1;
    return ((u32) (temp_v0 + (temp_a0 << 8) + (temp_a1 << 0x10)) >> temp_a2) & 1;
}

s16 Jpeg64_GetBits(u16 arg0) {
    s32 temp_a2;
    s32 temp_a3;
    s32 temp_v0_3;
    s32 var_t0;
    s32 var_t1;
    s32 var_v1;
    u32 var_a0;
    u8 *temp_v0;
    u8 temp_a1;
    u8 temp_v0_2;
    u8 temp_v1;

    var_t1 = 0;
    temp_a2 = arg0 - 1;
    var_t0 = temp_a2;
    temp_v0 = &D_800A6C94[(s32) D_800A6C90 >> 3];
    temp_v1 = M2C_FIELD(temp_v0, u8 *, 0);
    temp_a1 = M2C_FIELD(temp_v0, u8 *, 1);
    temp_v0_2 = M2C_FIELD(temp_v0, u8 *, 2);
    temp_a3 = D_800A6C90 & 7;
    D_800A6C90 += arg0;
    var_a0 = (u32) (temp_v1 + (temp_a1 << 8) + (temp_v0_2 << 0x10)) >> temp_a3;
    if ((temp_a2 & 0xFFFF) != 0xFFFF) {
        var_v1 = 0 * 2;
        do {
            var_t1 = var_v1 | (var_a0 & 1);
            var_a0 = (u32) (var_a0 & 0xFFFF) >> 1;
            temp_v0_3 = var_t0 - 1;
            var_t0 = temp_v0_3;
            var_v1 = var_t1 * 2;
        } while ((temp_v0_3 & 0xFFFF) != 0xFFFF);
    }
    return var_t1 & 0xFFFF;
}

s16 func_8001BA90(s32 arg0) {
    s16 var_v0;
    s32 temp_v0;

    if (D_800C51D8 != 0) {
        var_v0 = 0x7FFF;
    } else {
        temp_v0 = arg0 * 0xC;
        var_v0 = func_8000D49C(*(&D_8003A120 + temp_v0), *(&D_8003A124 + temp_v0), *(&D_8003A128 + temp_v0), *(&D_8003A12A + temp_v0), (u8) (s32) D_8003A12B[arg0].unk0);
    }
    return var_v0;
}

s16 func_8001BB18(s32 arg0) {
    s32 temp_v0;

    temp_v0 = arg0 * 0xC;
    return func_8000D49C(*(&D_8003A120 + temp_v0), *(&D_8003A124 + temp_v0), *(&D_8003A128 + temp_v0), *(&D_8003A12A + temp_v0), (u8) (s32) D_8003A12B[arg0].unk0);
}

void func_8001BB8C(s32 arg0) {
    if ((D_800C51D8 == 0) & (arg0 < 0x7FFF)) {
        func_8000D67C(arg0 & 0xFFFF);
    }
}

void func_8001BBC0(s32 arg0) {
    s32 var_a0;

    var_a0 = arg0;
    if (D_800C51D8 == 0) {
        if (var_a0 >= 6) {
            var_a0 = (s32) D_8003A240[var_a0];
        }
        func_8000D1CC((u32) var_a0);
    }
}

void func_8001BC00(void) {
    if (D_800C51D8 == 0) {
        func_8000D160();
    }
}

M2C_UNK *Asset_LoadLzss(M2C_UNK *arg0) {
    u8 sp10;
    u8 *sp114;
    u8 *sp11C;
    s32 sp124;
    s32 temp_a1;
    s32 temp_s2;
    s32 temp_v0;
    s32 var_a1;
    s32 var_fp;
    s32 var_s0;
    s32 var_s3;
    s32 var_s7;
    u32 var_v0;
    u8 *var_a0;
    u8 *var_s1;
    u8 *var_s6;
    u8 *var_v1;
    u8 temp_fp;
    u8 temp_s2_2;
    u8 temp_s5;
    u8 temp_v0_2;
    u8 temp_v0_3;

    Rom_Read(&sp10, (u8 *)0x100);
    temp_a1 = sp10 + (sp11 << 8) + (sp12 << 0x10) + (sp13 << 0x18);
    sp124 = temp_a1;
    var_s1 = arg0 + 0x100;
    sp11C = Heap_AllocFront(&gGameHeap, temp_a1);
    var_s0 = 4;
    var_fp = 0;
    sp114 = sp11C;
    var_s7 = 0;
    var_s3 = 0;
    if (sp124 > 0) {
        var_s6 = sp11C;
        do {
            var_fp *= 2;
            if (var_s7 == 0) {
                temp_v0 = var_s0;
                var_s0 += 1;
                temp_fp = (&sp10)[temp_v0 & 0xFF];
                var_s7 = 8;
                if (!(var_s0 & 0xFF)) {
                    Rom_Read(var_s1, &sp10, 0x100, sp124);
                    var_s1 += 0x100;
                    var_s7 = 8;
                }
                var_fp = temp_fp * 2;
            }
            var_s7 -= 1;
            if (var_fp & 0x100) {
                temp_s2 = var_s0 + 1;
                temp_s5 = (&sp10)[var_s0 & 0xFF];
                if (!(temp_s2 & 0xFF)) {
                    Rom_Read(var_s1, &sp10, 0x100);
                    var_s1 += 0x100;
                }
                var_s0 = temp_s2 + 1;
                temp_s2_2 = (&sp10)[temp_s2 & 0xFF];
                var_v0 = temp_s5 >> 4;
                if (!(var_s0 & 0xFF)) {
                    Rom_Read(var_s1, &sp10, 0x100);
                    var_s1 += 0x100;
                    var_v0 = temp_s5 >> 4;
                }
                var_a1 = var_v0 + 2;
                if (var_a1 != -1) {
                    var_a0 = &sp114[var_s3];
                    var_v1 = &sp114[var_s3 - (temp_s2_2 + ((temp_s5 & 0xF) << 8) + 1)];
                    do {
                        temp_v0_2 = *var_v1;
                        var_v1 += 1;
                        var_s6 += 1;
                        var_s3 += 1;
                        var_a1 -= 1;
                        *var_a0 = temp_v0_2;
                        var_a0 += 1;
                    } while (var_a1 != -1);
                }
            } else {
                temp_v0_3 = (&sp10)[var_s0 & 0xFF];
                var_s0 += 1;
                var_s3 += 1;
                *var_s6 = temp_v0_3;
                var_s6 += 1;
                if (!(var_s0 & 0xFF)) {
                    Rom_Read(var_s1, &sp10, 0x100);
                    var_s1 += 0x100;
                }
            }
        } while (var_s3 < sp124);
    }
    return (M2C_UNK *) sp11C;
}

void func_8001BE80(void) {

}

void func_8001BE88(void) {

}

void func_8001BE90(void) {

}

void func_8001BE98(void) {

}

void func_8001BEA0(void) {

}

void func_8001BEB0(void) {
    s32 var_a0;
    s8 *var_v1;

    var_a0 = 7;
    var_v1 = &D_800C1795;
    D_800C3174 = 0;
    D_800C172C = 0;
    D_800B4AA0 = D_8003A340[D_800C52A0 & 3];
    do {
        *var_v1 = 0;
        var_a0 -= 1;
        var_v1 -= 1;
    } while (var_a0 >= 0);
}

void func_8001BF08(void) {
    s32 temp_v0;
    s32 temp_v0_2;
    s32 temp_v0_3;
    s32 temp_v0_4;
    s32 temp_v1;
    s32 var_a0;
    s8 *var_v1;

    func_8000FA34();
    temp_v1 = ((u16) D_800C52A0 >> 4) & 0x70;
    temp_v0 = *(&D_8003A2C0 + temp_v1);
    D_800B0570 = temp_v0;
    D_800B056C = temp_v0;
    temp_v0_2 = *(&D_8003A2C4 + temp_v1);
    D_800C51C8 = temp_v0_2;
    D_800C51C4 = temp_v0_2;
    temp_v0_3 = *(&D_8003A2C8 + temp_v1);
    D_800C3174 = 0;
    D_800C172C = 0;
    D_800AF268 = temp_v0_3;
    D_800AF264 = temp_v0_3;
    temp_v0_4 = *(&D_8003A2CC + temp_v1);
    D_800B042C = temp_v0_4;
    D_800B0428 = temp_v0_4;
    var_v1 = &D_800C1795;
    var_a0 = 7;
    D_800B4AA0 = D_8003A340[D_800C52A0 & 3];
    do {
        *var_v1 = 0;
        var_a0 -= 1;
        var_v1 -= 1;
    } while (var_a0 >= 0);
    func_8000A76C(Main_GameLoop, 0xA, 0);
}

void Main_GameLoop(void) {
    s32 sp10;
    s32 sp14;
    s32 sp18;
    s32 temp_s0;
    s32 temp_s0_2;
    s32 temp_s0_3;
    s32 temp_s0_4;
    s32 var_a0;
    s32 var_s5;
    s32 var_v0;
    s8 *var_v1;

    var_s5 = 0;
    func_8000AEAC();
    func_8000AEAC();
    func_8000AEAC();
    Overlay_Load(1);
    sp10 = 3;
    sp14 = 0;
    sp18 = 0;
    ((M2C_UNK (*)(s32 *)) &func_800E0000)(&sp10);
loop_1:
    if (D_800C52A0 & 0x10) {
        D_800B0720 = 0;
    } else {
        D_800B0720 = 2;
    }
    if ((D_800C1674 != 0) || (D_800C52A0 & 0x80)) {
        D_800C1674 = 1;
        if (D_800C52A0 & 0x10) {
            D_800B0720 = 0;
        } else {
            D_800B0720 = 2;
        }
        Overlay_Load(0);
        sp10 = 0;
        sp14 = 0;
        sp18 = 0;
        ((M2C_UNK (*)(s32 *)) &func_800E0000)(&sp10);
        D_800C1674 = 0;
    }
    if (D_800AF260 != 0) {
        Overlay_Load(0);
        sp10 = 1;
        sp14 = 0;
        sp18 = 0;
        ((M2C_UNK (*)(s32 *)) &func_800E0000)(&sp10);
        D_800AF260 = 0;
    }
loop_12:
    func_8001BC00();
    Overlay_Load(1);
    sp10 = 0;
    sp14 = 0;
    sp18 = 0;
    temp_s0 = ((s32 (*)(s32 *)) &func_800E0000)(&sp10);
    if ((D_800C1674 == 0) && (D_800AF260 == 0)) {
        if ((temp_s0 & 0xFF) == 4) {
            D_800B0608 = var_s5;
            var_s5 = (var_s5 + 1) & 7;
            D_800B04CC = 2;
            D_800C51D8 = 0x4B0;
            D_800B0720 = 1;
            M2C_FIELD(&D_800B04D4, s32 *, 0) = 4;
            M2C_FIELD(&D_800B04D4, s32 *, 4) = 4;
            Overlay_Load(2);
            sp10 = 0;
            ((M2C_UNK (*)(s32 *)) &func_800E0000)(&sp10);
            if (D_800C52A0 & 0x10) {
                D_800B0720 = 0;
            } else {
                D_800B0720 = 2;
            }
            M2C_FIELD(&D_800B04D4, s32 *, 0) = 0;
            M2C_FIELD(&D_800B04D4, s32 *, 4) = 0;
            if (D_800C1674 == 0) {
                if (D_800AF260 != 0) {

                } else {
                    goto loop_12;
                }
            }
        } else {
            var_v0 = temp_s0 & 2;
            if (temp_s0 & 1) {
                M2C_FIELD(&D_800B04D4, s32 *, 0) = (s32) (4 - (((u16) D_800C52A0 >> 2) & 3));
                var_v0 = temp_s0 & 2;
            }
            if (var_v0 != 0) {
                D_800B04D8 = 4 - (((u16) D_800C52A0 >> 2) & 3);
            }
            var_a0 = 7;
            var_v1 = &D_800C1795;
            D_800C3174 = 0;
            D_800C172C = 0;
            D_800B4AA0 = D_8003A340[D_800C52A0 & 3];
            do {
                *var_v1 = 0;
                var_a0 -= 1;
                var_v1 -= 1;
            } while (var_a0 >= 0);
loop_27:
            D_800C51D8 = 0;
            func_8001BBC0(0xE);
            Overlay_Load(3);
            sp10 = 0;
            sp14 = 0;
            sp18 = 0;
            temp_s0_2 = ((s32 (*)(s32 *)) &func_800E0000)(&sp10);
            if ((D_800C1674 == 0) && (D_800AF260 == 0)) {
                D_800B0608 = temp_s0_2 & 0xFF;
                D_800B04CC = 0;
loop_30:
                func_8001BBC0(1);
                Overlay_Load(1);
                sp10 = 1;
                sp14 = 0;
                sp18 = 0;
                ((M2C_UNK (*)(s32 *)) &func_800E0000)(&sp10);
                Overlay_Load(2);
                sp10 = 0;
                temp_s0_3 = ((s32 (*)(s32 *)) &func_800E0000)(&sp10);
                func_8000D6B8();
                if (D_800C1674 == 0) {
                    temp_s0_4 = temp_s0_3 & 0xFF;
                    if (D_800AF260 == 0) {
                        if (temp_s0_4 != 0xFF) {
                            if (D_800C3174 >= 3) {
                                func_8001BBC0(1);
                                Overlay_Load(1);
                                sp10 = 2;
                                sp14 = 0;
                                sp18 = 0;
                                ((M2C_UNK (*)(s32 *)) &func_800E0000)(&sp10);
                                Overlay_Load(2);
                                sp10 = 1;
                                ((M2C_UNK (*)(s32 *)) &func_800E0000)(&sp10);
                                D_800C3174 = 0;
                            }
                            if (temp_s0_4 == 2) {
                                if (D_800B4AA0 < 0xF) {
                                    D_800B4AA0 += 1;
                                }
                                goto loop_27;
                            }
                            goto loop_30;
                        }
                        goto loop_12;
                    }
                }
            }
        }
    }
    func_8000D160();
    goto loop_1;
}

