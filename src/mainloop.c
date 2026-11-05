/* Vivid Dolls - sound/music wrappers, game start and the main game loop.
 * See mainloop.h. */
#include "mainloop.h"

extern void Overlay_Load(s32 n);
extern s32  func_800E0000(s32 *args);           /* entry of the loaded overlay */
extern void func_8000AEAC(void);
extern void func_8000A76C(void (*fn)(void), s32 a1, s32 a2);
extern void func_8000FA34(void);
extern s32  func_8000D49C(u32 id, f32 volume, s32 pitch, u32 a3, u32 a4);
extern void func_8000D67C(u32 handle);
extern void func_8000D1CC(u32 n);
extern void func_8000D160(void);
extern void func_8000D6B8(void);

extern u16 D_800C52A0;          /* settings word */
extern s32 D_800C51D8;          /* sound mute / attract demo timer */
extern s32 D_800C1674;          /* service mode request */
extern s32 D_800AF260;          /* second service request */
extern s32 D_800B0720;
extern s32 D_800B0608;
extern s32 D_800B04CC;
extern s32 D_800B04D4, D_800B04D8;
extern s32 D_800B4AA0;          /* current level */
extern s32 D_800C3174, D_800C172C;
extern u8  D_800C1795;          /* last of 8 bytes cleared on a new game */
extern s32 D_800B0570, D_800B056C, D_800C51C8, D_800C51C4;
extern s32 D_800AF268, D_800AF264, D_800B042C, D_800B0428;

extern SoundDef D_8003A120[];   /* sound effect table */
extern u32 D_8003A240[];        /* music id remap for n >= 6 */
extern s32 D_8003A340[4];       /* start level per settings bits 0-1 */
extern s32 D_8003A2C0[8][4];    /* per-setting parameters, row = settings bits 8-10 */

#define sSettings   D_800C52A0
#define sSoundMute  D_800C51D8
#define sReqService D_800C1674
#define sReqService2 D_800AF260
#define sLevel      D_800B4AA0

/* @ 0x8001BA90 */
/* Plays sound effect n from the table; returns the sound handle, or 0x7FFF
 * (no sound) while sound is muted. */
s16 Sound_Play(s32 n)
{
    SoundDef *s;

    if (sSoundMute != 0) {
        return 0x7FFF;
    }
    s = &D_8003A120[n];
    return func_8000D49C(s->id, s->volume, s->pitch, s->arg3, s->arg4);
}

/* @ 0x8001BB18 */
/* Plays sound effect n even while sound is muted. */
s16 Sound_PlayAlways(s32 n)
{
    SoundDef *s = &D_8003A120[n];

    return func_8000D49C(s->id, s->volume, s->pitch, s->arg3, s->arg4);
}

/* @ 0x8001BB8C */
/* Stops a sound started by Sound_Play (0x7FFF = no sound). */
void Sound_Stop(s32 handle)
{
    if (sSoundMute == 0 && handle < 0x7FFF) {
        func_8000D67C(handle & 0xFFFF);
    }
}

/* @ 0x8001BBC0 */
/* Starts music n (unless muted); n >= 6 is remapped through a table. */
void Bgm_Request(s32 n)
{
    if (sSoundMute == 0) {
        if (n >= 6) {
            n = D_8003A240[n];
        }
        func_8000D1CC(n);
    }
}

/* @ 0x8001BC00 */
/* Stops the music (unless muted). */
void Bgm_Cancel(void)
{
    if (sSoundMute == 0) {
        func_8000D160();
    }
}

/* @ 0x8001BE80 */
/* Empty hook (called from the boot code). */
void Main_NullHookA(void)
{
}

/* @ 0x8001BE88 */
/* Empty hook (called from the scheduler). */
void Main_NullHookB(void)
{
}

/* @ 0x8001BE90 */
/* Empty hook (called from the scheduler). */
void Main_NullHookC(void)
{
}

/* @ 0x8001BE98 */
/* Empty hook (called from five places in the scheduler/graphics code). */
void Main_NullHookD(void)
{
}

/* @ 0x8001BEA0 */
/* Empty hook (called from the boot code). */
void Main_NullHookE(void)
{
}

/* @ 0x8001BEB0 */
/* New game: clears the counters, sets the start level from the settings
 * and clears 8 bytes ending at 0x800C1795 (written from the top down). */
void Game_ResetProgress(void)
{
    u8 *p = &D_800C1795;
    s32 i;

    D_800C3174 = 0;
    D_800C172C = 0;
    sLevel = D_8003A340[sSettings & 3];
    for (i = 7; i >= 0; i--) {
        *p-- = 0;
    }
}

/* @ 0x8001BF08 */
/* Game start: initialises, copies the parameter row selected by settings
 * bits 8-10 into eight globals (each value twice), resets progress and starts
 * Main_GameLoop (priority/slot 10). */
void Main_Start(void)
{
    s32 *row;

    func_8000FA34();
    row = D_8003A2C0[(sSettings >> 8) & 7];
    D_800B0570 = D_800B056C = row[0];
    D_800C51C8 = D_800C51C4 = row[1];
    D_800AF268 = D_800AF264 = row[2];
    D_800B042C = D_800B0428 = row[3];
    Game_ResetProgress();
#ifdef VD_PHOTO_VIEWER
    {   /* Dreamcast photo-viewer build only: show the unused photo instead */
        extern void vd_photo_viewer_task(void);
        func_8000A76C(vd_photo_viewer_task, 10, 0);
        return;
    }
#endif
    func_8000A76C(Main_GameLoop, 10, 0);
}

/* runs the loaded overlay's entry point */
static s32 Main_RunOverlay(s32 *args)
{
    return func_800E0000(args);
}

/* @ 0x8001C000 */
/* The game's top-level loop. It picks which overlay to load and run:
 *   overlay 1: title / attract screens, overlay 2: the game itself,
 *   overlay 3: level select (?), overlay 0: service screens.
 * Each overlay entry gets a 3-word argument block. Like the original, some
 * calls only set args[0] and leave args[1..2] as they were. Return values:
 *   title (ovl1, mode 0): 4 = run the attract demo; otherwise bit 0/1 set
 *     D_800B04D4/D_800B04D8 (players?) and a new game starts.
 *   game (ovl2, mode 0): 0xFF = game over (back to title), 2 = level cleared.
 * Whenever a service request flag is set the loop restarts from the top. */
void Main_GameLoop(void)
{
    s32 args[3];
    s32 demo = 0;
    u32 r;

    func_8000AEAC();
    func_8000AEAC();
    func_8000AEAC();
    Overlay_Load(1);
    args[0] = 3; args[1] = 0; args[2] = 0;
    Main_RunOverlay(args);

    for (;;) {
        D_800B0720 = (sSettings & 0x10) ? 0 : 2;
        if (sReqService != 0 || (sSettings & 0x80)) {
            sReqService = 1;
            D_800B0720 = (sSettings & 0x10) ? 0 : 2;
            Overlay_Load(0);
            args[0] = 0; args[1] = 0; args[2] = 0;
            Main_RunOverlay(args);
            sReqService = 0;
        }
        if (sReqService2 != 0) {
            Overlay_Load(0);
            args[0] = 1; args[1] = 0; args[2] = 0;
            Main_RunOverlay(args);
            sReqService2 = 0;
        }

    title:
        Bgm_Cancel();
        Overlay_Load(1);
        args[0] = 0; args[1] = 0; args[2] = 0;
        r = Main_RunOverlay(args);
        if (sReqService != 0 || sReqService2 != 0) {
            goto restart;
        }
        if ((r & 0xFF) == 4) {
            /* attract demo: the game overlay with sound muted for 1200 frames */
            D_800B0608 = demo;
            demo = (demo + 1) & 7;
            D_800B04CC = 2;
            sSoundMute = 1200;
            D_800B0720 = 1;
            D_800B04D4 = 4;
            D_800B04D8 = 4;
            Overlay_Load(2);
            args[0] = 0;
            Main_RunOverlay(args);
            D_800B0720 = (sSettings & 0x10) ? 0 : 2;
            D_800B04D4 = 0;
            D_800B04D8 = 0;
            if (sReqService != 0 || sReqService2 != 0) {
                goto restart;
            }
            goto title;
        }
        if (r & 1) {
            D_800B04D4 = 4 - ((sSettings >> 2) & 3);
        }
        if (r & 2) {
            D_800B04D8 = 4 - ((sSettings >> 2) & 3);
        }
        Game_ResetProgress();

        for (;;) {
            /* level start */
            sSoundMute = 0;
            Bgm_Request(14);
            Overlay_Load(3);
            args[0] = 0; args[1] = 0; args[2] = 0;
            r = Main_RunOverlay(args);
            if (sReqService != 0 || sReqService2 != 0) {
                goto restart;
            }
            D_800B0608 = r & 0xFF;
            D_800B04CC = 0;

            for (;;) {
                /* play */
                Bgm_Request(1);
                Overlay_Load(1);
                args[0] = 1; args[1] = 0; args[2] = 0;
                Main_RunOverlay(args);
                Overlay_Load(2);
                args[0] = 0;
                r = Main_RunOverlay(args);
                func_8000D6B8();
                if (sReqService != 0 || sReqService2 != 0) {
                    goto restart;
                }
                r &= 0xFF;
                if (r == 0xFF) {
                    goto title;
                }
                if (D_800C3174 >= 3) {
                    Bgm_Request(1);
                    Overlay_Load(1);
                    args[0] = 2; args[1] = 0; args[2] = 0;
                    Main_RunOverlay(args);
                    Overlay_Load(2);
                    args[0] = 1;
                    Main_RunOverlay(args);
                    D_800C3174 = 0;
                }
                if (r == 2) {
                    break;
                }
            }
            if (sLevel < 15) {
                sLevel++;
            }
        }

    restart:
        func_8000D160();
    }
}
