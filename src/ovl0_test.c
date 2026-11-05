/* Vivid Dolls - overlay 0: operator test / service mode (0x800E0000-0x800E4AB0).
 * See ovl0_test.h for an overview. Every function is checked against the
 * original by tools/difftest/test_ovl0.py. */
#include "ovl0_test.h"
#include "scene.h"
#include "text.h"
#include "object.h"

/* main code */
void *Task_Create(void (*func)(), s32 prio, s32 argBytes, ...);   /* 0x8000A76C */
void  Task_Yield(void);                                           /* 0x8000AEAC one frame */
void  Music_Stop(void);                                           /* 0x8000D160 */
void  Music_Play(u32 song);                                       /* 0x8000D1CC */
s32   Eeprom_Read(u8 *buf);                                       /* 0x80015524 */
s32   Eeprom_Write(u8 *buf);                                      /* 0x8001556C */
s32   func_80025DC0(char *buf, const char *fmt, ...);             /* libultra sprintf */

extern SceneNode    *gSceneGroup1;     /* 0x800B04A0 screen-space group (texts) */
extern ObjSceneNode *gSceneGroup0;     /* 0x800B061C sprite group */
extern SceneLight    gSceneLight;      /* 0x800C1730 */
extern s32  gRetracesPerFrame;         /* 0x800C51CC */
extern u16  gIoOutputMask;             /* 0x800C1678 coin counter / lockout outputs allowed */
extern u16  D_800C178C;                /* fixed output bits (OR-ed into the outputs) */
extern u16  gCoinJam[2];               /* 0x800B0574 */
extern u16  gDipSwitches;              /* 0x800C52A0 */
extern u16  gIoHeld[2];                /* 0x800B0498 cabinet buttons per player */
extern u16  gPadRawHeld[2];            /* 0x800B058C N64 pad buttons per player */
extern s16  gPadStickX[2];             /* 0x800B04AC */
extern s16  gPadStickY[2];             /* 0x800C5298 */
extern u16  D_800C0D4C[2];             /* coin switches now */
extern u16  D_800C0AB0;                /* IO bit 4 now: labelled "SERVICE SW" here */
extern u16  D_800C30C4;                /* IO bit 5 now: labelled "TEST SW" here */
extern u16  D_800AF220;                /* IO bit 5 pressed this frame: "PUSH TEST SW" */
extern u16  D_800B060C[2];             /* player buttons held (mapped) */
extern u16  D_800C0D48[2];             /* player buttons pressed (mapped) */

#define ASSET_ROM_BASE  0x611F0u       /* asset table in ROM; asset 0 = test pattern */
#define SDRAM_TEST      ((volatile u32 *)0x01000000)  /* extra 4 MB SDRAM (TLB-mapped) */
#define SDRAM_WORDS     0x100000

/* label colours */
#define COL_TOP     0xFFFF
#define COL_BOTTOM  0xE739
#define COL_BG      1
#define COL_SHADOW  0x4211
#define COL_ON      0xFFC1     /* yellow: input active now */
#define COL_SEEN    0xF801     /* red: input was active before */
#define COL_PLAIN   0xFFFF
#define COL_CURSOR  0xFA11
#define COL_STICK   0x07FF

/* Make a screen-space label: Text_Create, a new SCENE_DL node linked just
 * before gSceneGroup1's tail sentinel, Text_PlaceScreen at (x, y), scale 1. */
static TextObj *Test_Label(const char *str, f32 x, f32 y, SceneNode **nodeOut) {
    TextObj *t = Text_Create(NULL, (const u8 *)str, COL_TOP, COL_BOTTOM, COL_BG, COL_SHADOW);
    SceneNode *node = Scene_NewNode(SCENE_DL);
    SceneNode *g = gSceneGroup1;

    g->group.tail->prev->next = node;
    node->prev = g->group.tail->prev;
    node->next = g->group.tail;
    g->group.tail->prev = node;
    Text_PlaceScreen(node, t, x, y, 1.0f, 1.0f);
    *nodeOut = node;
    return t;
}

/* Run fn as a task and wait (one frame per Task_Yield) until it writes *done. */
static s32 Test_RunTask(void (*fn)(s32 *)) {
    volatile s32 done = 0;

    Task_Create((void (*)())fn, 0x14, 8, &done, 0);
    while (done == 0) {
        Task_Yield();
    }
    return done;
}

/* @ 0x800E0000 */
/* Overlay entry. Outputs are disabled (gIoOutputMask = 0, D_800C178C = 0)
 * while in test mode. Mode 0: menu loop; the menu result (low byte) picks the
 * screen (0 system check, 1 screen test, 2 input test, 3 output test, 4 sound
 * test, other values: back to the menu); 0xFF would leave, but the menu never
 * returns it. Mode 1: coin error screen. Other modes return at once.
 * Returns the last menu/coin-error result & 0xFFFF, outputs re-enabled.
 * Quirk: for modes other than 0/1 the original returns an uninitialised stack
 * word (the C returns 0); Main_GameLoop only uses modes 0 and 1. */
s32 Test_Run(s32 *args) {
    s32 done = 0;

    gIoOutputMask = 0;
    D_800C178C = 0;
    Camera_Init();
    Light_Init(&gSceneLight);
    gRetracesPerFrame = 1;
    if (args[0] == 0) {
        for (;;) {
            done = Test_RunTask(Test_Menu);
            if ((done & 0xFF) == 0xFF) {
                break;
            }
            switch (done & 0xFF) {
                case 0: Test_RunTask(Test_SystemCheck); break;
                case 1: Test_RunTask(Test_ScreenTest);  break;
                case 2: Test_RunTask(Test_InputTest);   break;
                case 3: Test_RunTask(Test_OutputTest);  break;
                case 4: Test_RunTask(Test_SoundTest);   break;
            }
        }
    } else if (args[0] == 1) {
        done = Test_RunTask(Test_CoinError);
    }
    gIoOutputMask = 0xFFFF;
    return done & 0xFFFF;
}

/* @ 0x800E029C */
/* Screen test: full-screen sprite of ROM asset 0 (a test pattern, not a photo)
 * until the test switch is pressed. */
void Test_ScreenTest(s32 *done) {
    Obj *pattern;
    TextObj *t;
    SceneNode *node;

    pattern = Obj_NewSpritePlain(gSceneGroup0, (0 & 0xFFFFFF) + ASSET_ROM_BASE);
    Obj_Show(pattern);
    t = Test_Label("PUSH TEST SW TO EXIT", -80.0f, -96.0f, &node);
    do {
        Task_Yield();
    } while (D_800AF220 == 0);
    Task_Yield();
    Obj_Kill(pattern);
    Scene_DeleteNode(node);
    Task_Yield();
    Task_Yield();
    Text_Delete(t);
    Task_Yield();
    *done = 0x10000;
}

/* ---- input test ---- */

enum { IT_NLABELS = 56 };

/* Labels in creation order; slot = index in the node/text arrays, which is the
 * order they are deleted in (the four stick readouts are created X1, Y1, X2,
 * Y2 but deleted X1, X2, Y1, Y2). str NULL = DIP switch bit string buffer. */
typedef struct { const char *str; f32 x, y; u8 slot; } TestLabelDef;

static const TestLabelDef sInputLabels[IT_NLABELS] = {
    { "INPUT TEST", -40.0f, 110.0f, 0 },
    { "PUSH TEST SW TO EXIT", -80.0f, -96.0f, 1 },
    { "1P", -132.0f, 90.0f, 2 },
    { "U", -100.0f, 90.0f, 3 }, { "D", -84.0f, 90.0f, 4 },
    { "L", -68.0f, 90.0f, 5 },  { "R", -52.0f, 90.0f, 6 },
    { "P1", -100.0f, 80.0f, 7 }, { "P2", -84.0f, 80.0f, 8 },
    { "P3", -68.0f, 80.0f, 9 },  { "P4", -52.0f, 80.0f, 10 },
    { "START", -100.0f, 70.0f, 11 },
    { "2P", -132.0f, 50.0f, 12 },
    { "U", -100.0f, 50.0f, 13 }, { "D", -84.0f, 50.0f, 14 },
    { "L", -68.0f, 50.0f, 15 },  { "R", -52.0f, 50.0f, 16 },
    { "P1", -100.0f, 40.0f, 17 }, { "P2", -84.0f, 40.0f, 18 },
    { "P3", -68.0f, 40.0f, 19 },  { "P4", -52.0f, 40.0f, 20 },
    { "START", -100.0f, 30.0f, 21 },
    { "DIPSW1", 32.0f, 90.0f, 22 },
    { "12345678", 32.0f, 80.0f, 23 },
    { NULL, 32.0f, 70.0f, 24 },                 /* DIP switch 1 (bits 8-15) */
    { "DIPSW2", 32.0f, 50.0f, 25 },
    { "12345678", 32.0f, 40.0f, 26 },
    { NULL, 32.0f, 30.0f, 27 },                 /* DIP switch 2 (bits 0-7) */
    { "BR1", -132.0f, -20.0f, 28 },
    { "U", -100.0f, -20.0f, 29 }, { "D", -84.0f, -20.0f, 30 },
    { "L", -68.0f, -20.0f, 31 },  { "R", -52.0f, -20.0f, 32 },
    { "P1", -100.0f, -30.0f, 33 }, { "P2", -84.0f, -30.0f, 34 },
    { "P3", -68.0f, -30.0f, 35 },  { "P4", -52.0f, -30.0f, 36 },
    { "START", -100.0f, -40.0f, 37 },
    { "BR2", 18.0f, -20.0f, 38 },
    { "U", 50.0f, -20.0f, 39 }, { "D", 66.0f, -20.0f, 40 },
    { "L", 82.0f, -20.0f, 41 }, { "R", 98.0f, -20.0f, 42 },
    { "P1", 50.0f, -30.0f, 43 }, { "P2", 66.0f, -30.0f, 44 },
    { "P3", 82.0f, -30.0f, 45 }, { "P4", 98.0f, -30.0f, 46 },
    { "START", 50.0f, -40.0f, 47 },
    { "BR1 X:000", -100.0f, -60.0f, 48 },
    { "BR1 Y:000", -100.0f, -70.0f, 50 },
    { "BR2 X:000", 50.0f, -60.0f, 49 },
    { "BR2 Y:000", 50.0f, -70.0f, 51 },
    { "COIN1", -100.0f, 10.0f, 52 },
    { "COIN2", 0.0f, 10.0f, 53 },
    { "SERVICE SW", -100.0f, 0.0f, 54 },
    { "TEST SW", 0.0f, 0.0f, 55 },
};

/* Inputs shown, in the order they are checked each frame: label slot, the
 * input word and its mask. */
typedef struct { u16 *input; u16 mask; u8 slot; } TestInputDef;

#define IT_NINPUTS 40
static const TestInputDef sInputs[IT_NINPUTS] = {
    { &gIoHeld[0], 0x0001, 3 },  { &gIoHeld[0], 0x0002, 4 },  { &gIoHeld[0], 0x0004, 5 },
    { &gIoHeld[0], 0x0008, 6 },  { &gIoHeld[0], 0x0010, 7 },  { &gIoHeld[0], 0x0020, 8 },
    { &gIoHeld[0], 0x0040, 9 },  { &gIoHeld[0], 0x0080, 10 }, { &gIoHeld[0], 0x0100, 11 },
    { &gIoHeld[1], 0x0001, 13 }, { &gIoHeld[1], 0x0002, 14 }, { &gIoHeld[1], 0x0004, 15 },
    { &gIoHeld[1], 0x0008, 16 }, { &gIoHeld[1], 0x0010, 17 }, { &gIoHeld[1], 0x0020, 18 },
    { &gIoHeld[1], 0x0040, 19 }, { &gIoHeld[1], 0x0080, 20 }, { &gIoHeld[1], 0x0100, 21 },
    /* N64 pad: d-pad U/D/L/R, A, B, Z, R trigger, Start */
    { &gPadRawHeld[0], 0x0800, 29 }, { &gPadRawHeld[0], 0x0400, 30 }, { &gPadRawHeld[0], 0x0200, 31 },
    { &gPadRawHeld[0], 0x0100, 32 }, { &gPadRawHeld[0], 0x8000, 33 }, { &gPadRawHeld[0], 0x4000, 34 },
    { &gPadRawHeld[0], 0x0010, 35 }, { &gPadRawHeld[0], 0x0001, 36 }, { &gPadRawHeld[0], 0x1000, 37 },
    { &gPadRawHeld[1], 0x0800, 39 }, { &gPadRawHeld[1], 0x0400, 40 }, { &gPadRawHeld[1], 0x0200, 41 },
    { &gPadRawHeld[1], 0x0100, 42 }, { &gPadRawHeld[1], 0x8000, 43 }, { &gPadRawHeld[1], 0x4000, 44 },
    { &gPadRawHeld[1], 0x0010, 45 }, { &gPadRawHeld[1], 0x0001, 46 }, { &gPadRawHeld[1], 0x1000, 47 },
    { &D_800C0D4C[0], 1, 52 }, { &D_800C0D4C[1], 1, 53 },
    { &D_800C0AB0, 1, 54 }, { &D_800C30C4, 1, 55 },
};

/* The four stick readouts, in the order they are updated. */
typedef struct { s16 *value; const char *fmt; u8 slot; } TestStickDef;

static const TestStickDef sSticks[4] = {
    { &gPadStickX[0], "BR1 X:%d  ", 48 },
    { &gPadStickY[0], "BR1 Y:%d  ", 50 },
    { &gPadStickX[1], "BR2 X:%d  ", 49 },
    { &gPadStickY[1], "BR2 Y:%d  ", 51 },
};

/* 8 DIP switches as "H"/"L" characters (bit first+0 .. first+7) */
static void Test_DipString(char *out, s32 first) {
    s32 i;

    for (i = 0; i < 8; i++) {
        out[i] = ((gDipSwitches >> (i + first)) & 1) ? 'H' : 'L';
    }
    out[i] = '\0';
}

/* @ 0x800E03F0 */
/* Input test: cabinet buttons of both players, both N64 pads (buttons and
 * stick values), coin switches, the service/test switches and both DIP
 * switch banks. A label turns yellow while its input is on and red once it
 * has been on. Stick values are shown while outside -4..3 (yellow) and, once
 * moved, keep updating in red inside the dead zone. Note that the sprintf
 * buffer is the DIP switch 1 string buffer (rebuilt right after). */
void Test_InputTest(s32 *done) {
    s32 seen[IT_NINPUTS];
    s32 stickSeen[4];
    char dip1[16];              /* also the sprintf buffer for the stick readouts */
    char dip2[20];
    SceneNode *node[IT_NLABELS];
    TextObj *text[IT_NLABELS];
    s32 i;

    for (i = 0; i < IT_NINPUTS; i++) {
        seen[i] = 0;
    }
    for (i = 0; i < 4; i++) {
        stickSeen[i] = 0;
    }
    Test_DipString(dip1, 8);
    Test_DipString(dip2, 0);

    for (i = 0; i < IT_NLABELS; i++) {
        const TestLabelDef *d = &sInputLabels[i];
        const char *s = d->str;

        if (s == NULL) {
            s = (d->slot == 24) ? dip1 : dip2;
        }
        text[d->slot] = Test_Label(s, d->x, d->y, &node[d->slot]);
    }

    do {
        Task_Yield();
        for (i = 0; i < IT_NINPUTS; i++) {
            const TestInputDef *in = &sInputs[i];

            if (*in->input & in->mask) {
                seen[i] = 1;
                Text_SetGradient(text[in->slot], COL_ON, COL_ON, 1, 1);
            } else if (seen[i] != 0) {
                Text_SetGradient(text[in->slot], COL_SEEN, COL_SEEN, 1, 1);
            }
        }
        for (i = 0; i < 4; i++) {
            const TestStickDef *st = &sSticks[i];
            s32 v = *st->value;
            TextObj *t = text[st->slot];

            if ((u32)(v + 4) < 8) {
                if (stickSeen[i] != 0) {
                    func_80025DC0(dip1, st->fmt, v);
                    Text_SetString(t, (const u8 *)dip1);
                    Text_SetGradient(t, COL_SEEN, COL_SEEN, 1, 1);
                }
            } else {
                stickSeen[i] = 1;
                func_80025DC0(dip1, st->fmt, v);
                Text_SetString(t, (const u8 *)dip1);
                Text_SetColors(t, COL_ON, COL_ON, 1, 1);
            }
        }
        Test_DipString(dip1, 8);
        Test_DipString(dip2, 0);
        Text_SetString(text[24], (const u8 *)dip1);
        Text_SetString(text[27], (const u8 *)dip2);
    } while (D_800AF220 == 0);

    Task_Yield();
    for (i = 0; i < IT_NLABELS; i++) {
        if (i == 22 || i == 38) {
            Task_Yield();
        }
        Scene_DeleteNode(node[i]);
    }
    Task_Yield();
    Task_Yield();
    for (i = 0; i < IT_NLABELS; i++) {
        if (i == 22 || i == 38) {
            Task_Yield();
        }
        Text_Delete(text[i]);
    }
    Task_Yield();
    *done = 0x10000;
}

/* @ 0x800E33C8 */
/* Sound test: plays music 7 until the test switch is pressed. */
void Test_SoundTest(s32 *done) {
    SceneNode *nodeExit, *nodeTitle;
    TextObj *exitText, *title;

    exitText = Test_Label("PUSH TEST SW TO EXIT", -80.0f, -96.0f, &nodeExit);
    title = Test_Label("SOUND TEST", -40.0f, 110.0f, &nodeTitle);
    Music_Play(7);
    do {
        Task_Yield();
    } while (D_800AF220 == 0);
    Music_Stop();
    Task_Yield();
    Scene_DeleteNode(nodeExit);
    Scene_DeleteNode(nodeTitle);
    Task_Yield();
    Task_Yield();
    Text_Delete(exitText);
    Text_Delete(title);
    Task_Yield();
    *done = 0x10000;
}

/* @ 0x800E3588 */
/* System check: version string, SDRAM test (write i to each of the 1M words
 * at 0x01000000, read back, stop at the first mismatch) and EEPROM test
 * (write bytes 0..0x1FF & 0xFF to all 512 bytes, read back, compare). The
 * EEPROM keeps the test pattern afterwards (the settings are lost). */
void Test_SystemCheck(s32 *done) {
    u8 eep[0x200];
    char buf[0x100];
    SceneNode *nExit, *nTitle, *nVer, *nSdram, *nEep;
    TextObj *tExit, *tTitle, *tVer, *tSdram, *tEep;
    s32 i, err;

    tExit = Test_Label("PUSH TEST SW TO EXIT", -80.0f, -96.0f, &nExit);
    tTitle = Test_Label("SYSTEM CHECK", -48.0f, 110.0f, &nTitle);
    Task_Yield();

    for (i = 0; i < SDRAM_WORDS; i++) {
        SDRAM_TEST[i] = i;
    }
    err = 0;
    for (i = 0; i < SDRAM_WORDS; i++) {
        if (SDRAM_TEST[i] != (u32)i) {
            err = 1;
            break;
        }
    }
    tVer = Test_Label("VERSION 00.09", -100.0f, 40.0f, &nVer);
    func_80025DC0(buf, err ? "SDRAM ERROR" : "SDRAM OK");
    tSdram = Test_Label(buf, -100.0f, 20.0f, &nSdram);

    for (i = 0x1FF; i >= 0; i--) {
        eep[i] = i;
    }
    Eeprom_Write(eep);
    err = 0;
    Eeprom_Read(eep);
    for (i = 0; i < 0x200; i++) {
        if (eep[i] != (i & 0xFF)) {
            err = 1;
            break;
        }
    }
    func_80025DC0(buf, err ? "EEPROM ERROR" : "EEPROM OK");
    tEep = Test_Label(buf, -100.0f, 0.0f, &nEep);

    do {
        Task_Yield();
    } while (D_800AF220 == 0);
    Task_Yield();
    Scene_DeleteNode(nExit);
    Scene_DeleteNode(nTitle);
    Scene_DeleteNode(nVer);
    Scene_DeleteNode(nSdram);
    Scene_DeleteNode(nEep);
    Task_Yield();
    Task_Yield();
    Text_Delete(tExit);
    Text_Delete(tTitle);
    Text_Delete(tVer);
    Text_Delete(tSdram);
    Text_Delete(tEep);
    Task_Yield();
    *done = 0x10000;
}

/* Labels of the output test and the menu: string, x, y. */
typedef struct { const char *str; f32 x, y; } TestTextDef;

static const TestTextDef sOutputLabels[8] = {
    { "OUTPUT TEST", -40.0f, 110.0f },
    { "PUSH TEST SW TO EXIT", -80.0f, -96.0f },
    { "1P UP/DOWN TO SELECT", -80.0f, -76.0f },
    { "PUSH 1P BUTTON TO ENABLE", -96.0f, -66.0f },
    { "LOCKOUT COIL1", -52.0f, 60.0f },
    { "LOCKOUT COIL2", -52.0f, 40.0f },
    { "COIN COUNTER1", -52.0f, 20.0f },
    { "COIN COUNTER2", -52.0f, 0.0f },
};

/* output bit driven by each selectable item (see Coin_Update: bits 0/1 coin
 * counters, bits 2/3 coin lockouts) */
static const u16 sOutputBits[4] = { 4, 8, 1, 2 };

/* @ 0x800E3A48 */
/* Output test: 1P up/down (pressed bits 0/1) selects one of 4 outputs (red);
 * while 1P button (held bit 0x10) is down it turns yellow and its bit is put
 * in D_800C178C (else 0). The original also zeroes an unused 59-word stack
 * array at the start (left over from the input test); not reproduced. */
void Test_OutputTest(s32 *done) {
    SceneNode *node[8];
    TextObj *text[8];
    s32 sel = 0;
    u16 out = 0;
    s32 i;

    for (i = 0; i < 8; i++) {
        text[i] = Test_Label(sOutputLabels[i].str, sOutputLabels[i].x, sOutputLabels[i].y, &node[i]);
    }
    for (;;) {
        Task_Yield();
        if (D_800AF220 != 0) {
            break;
        }
        if (D_800C0D48[0] & 1) {
            if (sel > 0) {
                Text_SetGradient(text[4 + sel], COL_PLAIN, COL_PLAIN, 1, 1);
                sel--;
            }
        } else if ((D_800C0D48[0] & 2) && sel < 3) {
            Text_SetGradient(text[4 + sel], COL_PLAIN, COL_PLAIN, 1, 1);
            sel++;
        }
        if (D_800B060C[0] & 0x10) {
            out = sOutputBits[sel];
            Text_SetColors(text[4 + sel], COL_ON, COL_ON, 1, 1);
        } else {
            out = 0;
            Text_SetGradient(text[4 + sel], COL_SEEN, COL_SEEN, 1, 1);
        }
        D_800C178C = out;
    }
    Task_Yield();
    for (i = 0; i < 8; i++) {
        Scene_DeleteNode(node[i]);
    }
    Task_Yield();
    Task_Yield();
    for (i = 0; i < 8; i++) {
        Text_Delete(text[i]);
    }
    Task_Yield();
    *done = 0x10000;
}

static const TestTextDef sMenuLabels[11] = {
    { "TEST MODE MENU", -56.0f, 110.0f },
    { "                    ", -80.0f, -96.0f },
    { "1P UP/DOWN TO SELECT", -80.0f, -76.0f },
    { "PUSH TEST SW TO ENABLE", -96.0f, -66.0f },
    { "SYSTEM TEST  ", -52.0f, 60.0f },
    { "SCREEN TEST  ", -52.0f, 40.0f },
    { "INPUT TEST   ", -52.0f, 20.0f },
    { "OUTPUT TEST  ", -52.0f, 0.0f },
    { "SOUND TEST   ", -52.0f, -20.0f },
    { "1P:JOYSTICK  ", -52.0f, -36.0f },
    { "2P:JOYSTICK  ", -52.0f, -46.0f },
};

/* @ 0x800E40D0 */
/* Test mode menu: 1P up/down (pressed bits 0/1) moves the cursor over the 5
 * entries, the test switch picks one: *done = 0x10000 | entry. The bottom two
 * lines show the controller type per player: DIP bit 4 = JAMMA sticks, else
 * "3DSTICK" when the player's N64 pad reports d-pad up+down or left+right at
 * the same time (raw bits 0xC00 or 0x300 both set: impossible on a real
 * d-pad, so presumably how the cabinet's stick adapter identifies itself),
 * else "JOYSTICK". Like the output test, the original zeroes an unused stack
 * array first (not reproduced). */
void Test_Menu(s32 *done) {
    SceneNode *node[11];
    TextObj *text[11];
    s32 sel = 0;
    s32 i;

    for (i = 0; i < 11; i++) {
        text[i] = Test_Label(sMenuLabels[i].str, sMenuLabels[i].x, sMenuLabels[i].y, &node[i]);
    }
    Text_SetGradient(text[9], COL_STICK, COL_STICK, 1, 1);
    Text_SetGradient(text[10], COL_STICK, COL_STICK, 1, 1);
    for (;;) {
        Task_Yield();
        if (D_800AF220 != 0) {
            break;
        }
        if (D_800C0D48[0] & 1) {
            if (sel > 0) {
                Text_SetGradient(text[4 + sel], COL_PLAIN, COL_PLAIN, 1, 1);
                sel--;
            }
        } else if ((D_800C0D48[0] & 2) && sel < 4) {
            Text_SetGradient(text[4 + sel], COL_PLAIN, COL_PLAIN, 1, 1);
            sel++;
        }
        Text_SetGradient(text[4 + sel], COL_CURSOR, COL_CURSOR, 1, 1);
        if (gDipSwitches & 0x10) {
            Text_SetString(text[9], (const u8 *)"1P:JAMMASTICK");
            Text_SetString(text[10], (const u8 *)"2P:JAMMASTICK");
        } else {
            if ((gPadRawHeld[0] & 0xC00) == 0xC00 || (gPadRawHeld[0] & 0x300) == 0x300) {
                Text_SetString(text[9], (const u8 *)"1P:3DSTICK   ");
            } else {
                Text_SetString(text[9], (const u8 *)"1P:JOYSTICK  ");
            }
            if ((gPadRawHeld[1] & 0xC00) == 0xC00 || (gPadRawHeld[1] & 0x300) == 0x300) {
                Text_SetString(text[10], (const u8 *)"2P:3DSTICK   ");
            } else {
                Text_SetString(text[10], (const u8 *)"2P:JOYSTICK  ");
            }
        }
    }
    Task_Yield();
    for (i = 0; i < 11; i++) {
        Scene_DeleteNode(node[i]);
    }
    Task_Yield();
    Task_Yield();
    for (i = 0; i < 11; i++) {
        Text_Delete(text[i]);
    }
    Task_Yield();
    *done = 0x10000 | sel;
}

/* @ 0x800E4988 */
/* Coin error: "COIN ERROR" for 300 frames, then the coin jam flags of both
 * slots are cleared. */
void Test_CoinError(s32 *done) {
    SceneNode *node;
    TextObj *t;
    s32 i;

    t = Test_Label("COIN ERROR", -40.0f, 0.0f, &node);
    for (i = 0; i < 300; i++) {
        Task_Yield();
    }
    Task_Yield();
    Scene_DeleteNode(node);
    Task_Yield();
    Task_Yield();
    Text_Delete(t);
    Task_Yield();
    gCoinJam[1] = 0;
    gCoinJam[0] = 0;
    *done = 0x10000;
}
