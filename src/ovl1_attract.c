/* Vivid Dolls - overlay 1 (boot logo, title screen, stage / bonus intros).
 * See ovl1_attract.h for an overview. Every function is checked against the
 * original by tools/difftest/test_ovl1.py. */
#include "ovl1_attract.h"
#include "heap.h"
#include "object.h"
#include "photoview.h"
#include "system.h"
#include "jpeg64.h"

void Camera_Init(void);
void Light_Init(void *light);
void Task_Yield(void);          /* wait one frame (switch to the next task) */

#define ASSET_ROM(off) (((u32)(off) & 0xFFFFFF) + 0x611F0)

/* ---- main globals ---- */
extern Heap gGameHeap;
extern ObjSceneNode *gSceneGroup0;
extern u8  gSceneLight[];       /* SceneLight */
extern s32 gRetracesPerFrame;
extern s32 gCredits;
extern s32 gCoins;              /* coins inserted towards the next credit */
extern u32 D_800B056C;          /* coins per credit (coin slot 1) */
extern s32 D_800B4A30;          /* selects D_800AF264 instead of D_800B056C */
extern s32 D_800AF264;          /* coins per credit (alternative) */
extern u16 D_800C30C4;          /* service / test switch */
extern s32 gServiceRequest;
extern s32 gServiceRequest2;
extern u16 D_800C0D48;          /* player 1 buttons (0x100 = start) */
extern u16 D_800C0D4A;          /* player 2 buttons */
extern s32 D_800B0608;          /* current girl 0..7 */
extern s32 D_800B04CC;          /* current photo of that girl 0..2 */

/* ---- overlay 1 data ---- */
extern SceneObjDesc   ovl1_D_800E0FC0[];        /* title screen objects */
extern SceneAssetDesc ovl1_D_800E114C[];        /* title screen sprites */
extern f32 ovl1_D_800E1200[8][2];               /* stage intro: letter targets */
extern f32 ovl1_D_800E1240[8][2];               /* stage intro: letter start positions */
extern SceneObjDesc   ovl1_D_800E1280[];        /* stage intro objects */
extern SceneAssetDesc ovl1_D_800E13E8[];        /* stage intro sprites */
extern f32 ovl1_D_800E14C0[10][2];              /* bonus intro: letter targets */
extern f32 ovl1_D_800E1510[10][2];              /* bonus intro: letter start positions */
extern SceneObjDesc   ovl1_D_800E1560[];        /* bonus intro objects */
extern SceneAssetDesc ovl1_D_800E1710[];        /* (empty) */
extern SceneObjDesc   ovl1_D_800E1720[];        /* boot logo object */
extern SceneAssetDesc ovl1_D_800E1768[];        /* (empty) */
extern f64 ovl1_D_800E1780;                     /* 0.9 zoom factor per frame */
extern f64 ovl1_D_800E1790;                     /* 0.9 */

/* ---- overlay 1 BSS ---- */
/* title objects: [0] logo (shown in phase 3), [1] right logo half, [2] left logo
 * half, [3] "press start" (blinks), [4] second blinking text (with credits only),
 * [5] copyright, [8] "CREDIT" label, [9..11] credit digits */
extern Obj *ovl1_D_800E17A0[12];
extern Obj *ovl1_D_800E17C4;                    /* credit digit 1 (tens / coins) */
extern Obj *ovl1_D_800E17C8;                    /* credit digit 2 (units / "/") */
extern Obj *ovl1_D_800E17CC;                    /* credit digit 3 (coins per credit) */
extern void *ovl1_D_800E17F0[10];               /* digit sprites 0..9 */
extern void *ovl1_D_800E1818;                   /* "/" sprite */
extern Obj *ovl1_D_800E1820[8];                 /* stage intro letters; [5] girl, [7] photo digit */
extern Obj *ovl1_D_800E1840;                    /* stage intro zoom sprite */
extern void *ovl1_D_800E186C[];                 /* stage intro digit sprites */
extern Obj *ovl1_D_800E18A0[10];                /* bonus intro letters */
extern Obj *ovl1_D_800E18C8;                    /* bonus intro zoom sprite */

/* @ 0x800E0000 */
/* Overlay entry: reset camera and light, start the screen task selected by
 * args[0] (0 title, 1 stage intro, 2 bonus intro, 3 boot logo) and yield until it
 * stores its result; returns the low 16 bits. Quirk kept: any other args[0]
 * starts no task, so it waits forever. */
s32 Attract_Run(s32 *args) {
    s32 done = 0;

    Camera_Init();
    Light_Init(gSceneLight);
    gRetracesPerFrame = 1;
    switch (args[0]) {
    case 0:
        Task_Create(Title_Main, 0x14, 8, &done, 0);
        break;
    case 1:
        Task_Create(StageIntro_Main, 0x14, 8, &done, 0);
        break;
    case 2:
        Task_Create(BonusIntro_Main, 0x14, 8, &done, 0);
        break;
    case 3:
        Task_Create(Boot_Logo, 0x14, 8, &done, 0);
        break;
    }
    while (done == 0) {
        Task_Yield();
    }
    gRetracesPerFrame = 1;
    return done & 0xFFFF;
}

/* @ 0x800E00E4 */
/* Credit counter on the title screen: credits (max 99) as two digits (tens hidden
 * when 0, third digit hidden); with no credits and more than one coin per credit,
 * "coins / coins-per-credit" instead. Same as Hud_UpdateCredits in ovl2. */
void Attract_UpdateCredits(void) {
    s32 n = gCredits;

    if (n >= 100) {
        n = 99;
    }
    if (n == 0 && D_800B056C >= 2) {
        s32 per;
        if (D_800B4A30 == 0) {
            Obj_SetData(ovl1_D_800E17C4, ovl1_D_800E17F0[gCoins]);
            Obj_SetData(ovl1_D_800E17C8, ovl1_D_800E1818);
            per = D_800B056C;
        } else {
            Obj_SetData(ovl1_D_800E17C4, ovl1_D_800E17F0[gCoins]);
            Obj_SetData(ovl1_D_800E17C8, ovl1_D_800E1818);
            per = D_800AF264;
        }
        Obj_SetData(ovl1_D_800E17CC, ovl1_D_800E17F0[per]);
        return;
    }
    if (n / 10 != 0) {
        Obj_SetData(ovl1_D_800E17C4, ovl1_D_800E17F0[n / 10]);
    } else {
        Obj_Hide(ovl1_D_800E17C4);
    }
    Obj_SetData(ovl1_D_800E17C8, ovl1_D_800E17F0[n % 10]);
    Obj_Hide(ovl1_D_800E17CC);
}

/* One phase of the title logo slide-in: up to 30 frames; each frame the two logo
 * halves are placed at their current position, then each coordinate moves
 * 1/div of the way to its target. Returns 1 (stop sliding) when a credit is in. */
static s32 title_slide(f32 pos[4], f32 lx, f32 ly, f32 rx, f32 ry, f32 div) {
    Obj **o = ovl1_D_800E17A0;
    s32 i = 0;

    for (;;) {
        Task_Yield();
        if (++i >= 31) {
            return 0;
        }
        Attract_UpdateCredits();
        if (gCredits != 0) {
            return 1;
        }
        /* pos: [0] left x, [1] right x, [2] left y, [3] right y */
        Obj_SetPos(o[1], (s32)pos[1], (s32)pos[3], 0);
        Obj_SetPos(o[2], (s32)pos[0], (s32)pos[2], 0);
        pos[0] += (lx - pos[0]) / div;
        pos[2] += (ly - pos[2]) / div;
        pos[1] += (rx - pos[1]) / div;
        pos[3] += (ry - pos[3]) / div;
    }
}

/* @ 0x800E0288 */
/* Title screen task. Decodes the title photo (asset 0x3EB4) into a 640x480 16-bit
 * buffer, copies it into a second one shown by a PhotoView, builds the title
 * scene, slides the logo in (3 phases, skipped when a credit is inserted), then
 * blinks "press start" until player 1 or 2 presses start with a credit (result 1
 * / 2, one credit used), 301 frames pass without credits (4 = attract demo), or a
 * service flag is raised (0). Stores 0x10000 | result in *done.
 * Quirk: the original leaves the result register uninitialised when a service
 * flag is raised on the very first frame of the last loop (the caller ignores the
 * result then); this version uses 0. */
void Title_Main(s32 *done) {
    Obj **o = ovl1_D_800E17A0;
    u16 *photo, *image;
    s32 i, w, h;
    PhotoView *view;
    Obj *bg;
    f32 pos[4];
    s32 frame, idle, result = 0, c;

    photo = Heap_Alloc(&gGameHeap, 0x96000);
    for (i = 0; i <= 0x4AFFF; i++) {
        photo[i] = 0;
    }
    image = Heap_Alloc(&gGameHeap, 0x96000);
    for (i = 0; i <= 0x4AFFF; i++) {
        image[i] = 0;
    }
    Jpeg64_Decode(ASSET_ROM(0x3EB4), photo, 0, 0, &w, &h);
    Image_CopyRect(photo, w, h, 0, 0, w, h, image, w, h, 0, 0);
    view = PhotoView_New(NULL, image, w, h, 0, 0);
    bg = Obj_NewDL(gSceneGroup0, view->setup_dl, view->dl, view->finish_dl);
    Obj_Show(bg);
    Scene_Build(ovl1_D_800E0FC0, ovl1_D_800E114C);
    Task_Yield();

    pos[0] = -256.5f;
    pos[2] = 71.5f;
    pos[1] = 576.5f;
    pos[3] = 71.5f;
    if (!title_slide(pos, 94.5f, 71.5f, 216.5f, 71.5f, 8.0f) &&
        !title_slide(pos, 94.5f, 54.5f, 216.5f, 96.5f, 4.0f)) {
        Obj_Show(o[0]);
        title_slide(pos, 94.5f, 50.5f, 216.5f, 92.5f, 4.0f);
    }

    Obj_SetPos(o[2], 0x5E, 0x32, 0);
    Obj_SetPos(o[1], 0xD8, 0x5C, 0);
    frame = 0;
    idle = 0;
    for (;;) {
        Task_Yield();
        frame++;
        if (gCredits != 0) {
            idle = 0;
        } else {
            idle++;
        }
        if (idle >= 301) {
            result = 4;
            break;
        }
        if (D_800C30C4 != 0) {
            gServiceRequest = 1;
            break;
        }
        if (gServiceRequest2 != 0) {
            break;
        }
        Attract_UpdateCredits();
        if (frame & 0x10) {
            Obj_Show(o[3]);
        } else {
            Obj_Hide(o[3]);
        }
        if (gCredits != 0) {
            if (frame & 0x10) {
                Obj_Show(o[4]);
            } else {
                Obj_Hide(o[4]);
            }
        }
        c = gCredits;
        result = 0;
        if (c == 0) {
            continue;
        }
        if (D_800C0D48 & 0x100) {
            result = 1;
        } else if (D_800C0D4A & 0x100) {
            result = 2;
        } else {
            continue;
        }
        gCredits = c - 1;
        break;
    }

    Obj_Kill(bg);
    Scene_Destroy(ovl1_D_800E0FC0, ovl1_D_800E114C);
    Task_Yield();
    Task_Yield();
    PhotoView_Delete(view);
    Heap_Free(&gGameHeap, image);
    Heap_Free(&gGameHeap, photo);
    Task_Yield();
    *done = result | 0x10000;
}

/* Shared body of the two intros (the original has two copies): n sprites slide
 * from start[] towards target[] (1/8 of the way per frame) for 61 frames, then
 * *zoom is shown scaling from 4x down by factor per frame (min 1x) for 61 frames.
 * Returns early when a service flag is raised. */
static void intro_animate(Obj **objs, s32 n, f32 (*target)[2], f32 (*pos)[2],
                          Obj **zoom, f64 factor) {
    s32 frame, i;
    f32 scale;

    frame = 0;
    do {
        Task_Yield();
        frame++;
        if (D_800C30C4 != 0) {
            gServiceRequest = 1;
            return;
        }
        if (gServiceRequest2 != 0) {
            return;
        }
        for (i = 0; i < n; i++) {
            pos[i][0] += (target[i][0] - pos[i][0]) / 8.0f;
            pos[i][1] += (target[i][1] - pos[i][1]) / 8.0f;
            Obj_SetPos(objs[i], (s32)pos[i][0], (s32)pos[i][1], 0);
        }
    } while (frame < 61);

    scale = 4.0f;
    frame = 0;
    do {
        Task_Yield();
        frame++;
        if (D_800C30C4 != 0) {
            gServiceRequest = 1;
            return;
        }
        if (gServiceRequest2 != 0) {
            return;
        }
        (*zoom)->scale[0] = scale;
        (*zoom)->scale[1] = scale;
        Obj_Invalidate(*zoom);
        Obj_Show(*zoom);
        scale = (f32)((f64)scale * factor);
        if (scale < 1.0f) {
            scale = 1.0f;
        }
    } while (frame < 61);
}

/* @ 0x800E0970 */
/* Stage intro task ("STAGE girl-photo"): 8 sprites placed at their start
 * positions (off screen right), sprites 5 and 7 set to the digits of D_800B0608
 * and D_800B04CC, then slide in and zoom (intro_animate). Stores 0x10000. */
void StageIntro_Main(s32 *done) {
    f32 pos[8][2];
    s32 i;

    Scene_Build(ovl1_D_800E1280, ovl1_D_800E13E8);
    for (i = 0; i < 8; i++) {
        pos[i][0] = ovl1_D_800E1240[i][0];
        pos[i][1] = ovl1_D_800E1240[i][1];
        Obj_SetPos(ovl1_D_800E1820[i], (s32)pos[i][0], (s32)pos[i][1], 0);
    }
    Obj_SetData(ovl1_D_800E1820[5], ovl1_D_800E186C[D_800B0608]);
    Obj_SetData(ovl1_D_800E1820[7], ovl1_D_800E186C[D_800B04CC]);
    intro_animate(ovl1_D_800E1820, 8, ovl1_D_800E1200, pos, &ovl1_D_800E1840, ovl1_D_800E1780);
    Scene_Destroy(ovl1_D_800E1280, ovl1_D_800E13E8);
    Task_Yield();
    Task_Yield();
    *done = 0x10000;
}

/* @ 0x800E0C80 */
/* Bonus stage intro task: like StageIntro_Main with 10 fixed letters. */
void BonusIntro_Main(s32 *done) {
    f32 pos[10][2];
    s32 i;

    Scene_Build(ovl1_D_800E1560, ovl1_D_800E1710);
    for (i = 0; i < 10; i++) {
        pos[i][0] = ovl1_D_800E1510[i][0];
        pos[i][1] = ovl1_D_800E1510[i][1];
        Obj_SetPos(ovl1_D_800E18A0[i], (s32)pos[i][0], (s32)pos[i][1], 0);
    }
    intro_animate(ovl1_D_800E18A0, 10, ovl1_D_800E14C0, pos, &ovl1_D_800E18C8, ovl1_D_800E1790);
    Scene_Destroy(ovl1_D_800E1560, ovl1_D_800E1710);
    Task_Yield();
    Task_Yield();
    *done = 0x10000;
}

/* @ 0x800E0F40 */
/* Boot logo task: one full-screen sprite for 181 frames (no early exit). */
void Boot_Logo(s32 *done) {
    s32 i;

    Scene_Build(ovl1_D_800E1720, ovl1_D_800E1768);
    for (i = 0; i < 181; i++) {
        Task_Yield();
    }
    Scene_Destroy(ovl1_D_800E1720, ovl1_D_800E1768);
    Task_Yield();
    Task_Yield();
    *done = 0x10000;
}
