/* Vivid Dolls - sound/music wrappers, game start and the main game loop
 * (RAM 0x8001BA90-0x8001C43F, minus Asset_LoadLzss).
 *
 * Globals used here (game BSS):
 *   0x800C52A0 u16  settings word, probably the DIP switches. Uses seen here:
 *                   bits 0-1 index the start-level table 0x8003A340, bits 2-3 give
 *                   4 - n for D_800B04D4/D8, bit 4 picks D_800B0720 = 0 or 2,
 *                   bit 7 forces the service overlay, bits 8-10 pick a row of the
 *                   0x8003A2C0 parameter table
 *   0x800C51D8 s32  sound mute; non-zero (1200 frames) while the attract demo runs
 *   0x800C1674 s32  "go to test/service mode" request
 *   0x800AF260 s32  "go to the other service screen" request
 *   0x800B4AA0 s32  current level (0..15)
 *   0x800C3174 s32  counter checked by the main loop (>= 3 -> extra overlay screens)
 * The overlay entry point is 0x800E0000 (all four overlays load there) and is
 * called with a pointer to 3 words of arguments.
 */
#ifndef VD_MAINLOOP_H
#define VD_MAINLOOP_H

#include "vd_common.h"

/* sound effect table entry (0x8003A120, 12 bytes) */
typedef struct SoundDef {
    u16 id;
    u16 pad;
    f32 volume;
    s16 pitch;
    u8  arg3;
    u8  arg4;
} SoundDef;

s16  Sound_Play(s32 n);                 /* 0x8001BA90 - muted: returns 0x7FFF */
s16  Sound_PlayAlways(s32 n);           /* 0x8001BB18 - ignores the mute flag */
void Sound_Stop(s32 handle);            /* 0x8001BB8C */
void Bgm_Request(s32 n);                 /* 0x8001BBC0 */
void Bgm_Cancel(void);                  /* 0x8001BC00 */
void Main_NullHookA(void);              /* 0x8001BE80 */
void Main_NullHookB(void);              /* 0x8001BE88 */
void Main_NullHookC(void);              /* 0x8001BE90 */
void Main_NullHookD(void);              /* 0x8001BE98 */
void Main_NullHookE(void);              /* 0x8001BEA0 */
void Game_ResetProgress(void);          /* 0x8001BEB0 */
void Main_Start(void);                  /* 0x8001BF08 */
void Main_GameLoop(void);               /* 0x8001C000 - never returns */

#endif
