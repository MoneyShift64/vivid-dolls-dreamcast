/* Vivid Dolls - overlay 3: girl select screen. See ovl3_select.h for an overview.
 * Every function is checked against the original by tools/difftest/test_ovl3.py. */
#include "ovl3_select.h"
#include "libm.h"

void Camera_Init(void);
void Light_Init(void *light);
void Task_Yield(void);
s16  Sound_Play(s32 n);
void *Task_Create(void (*func)(), s32 prio, s32 argBytes, ...);

extern u8  gSceneLight[];
extern u32 gRetracesPerFrame;
extern s32 gServiceRequest;
extern s32 gPlayerActive[2];       /* player 1/2 is playing */
extern u16 D_800C0D48;             /* P1 buttons (pressed) */
extern u16 D_800C0D4A;             /* P2 buttons (pressed) */
extern u16 D_800C30C4;             /* service/abort request */
extern u8  D_800C178E[8];          /* girl already cleared */

/* overlay data (filled by Scene_Build) */
extern SceneObjDesc   ovl3_D_800E0B40[];   /* object table */
extern SceneAssetDesc ovl3_D_800E0DA4[];   /* asset table */
extern Obj *ovl3_D_800E0F38[2];            /* [0] portrait, [1] name/profile */
extern Obj *ovl3_D_800E0F40[8];            /* girl icons */
extern Obj *ovl3_D_800E0F60;               /* arrow */
extern Obj *ovl3_D_800E0F64;               /* front frame */
extern Obj *ovl3_D_800E0F68;               /* timer tens digit */
extern Obj *ovl3_D_800E0F6C;               /* timer units digit */
extern void *ovl3_D_800E0F70[2];           /* arrow frames */
extern void *ovl3_D_800E0F78[4];           /* frame animation */
extern void *ovl3_D_800E0F88[8];           /* portraits per girl */
extern void *ovl3_D_800E0FA8[8];           /* name/profile per girl */
extern void *ovl3_D_800E0FD0[10];          /* digits 0-9 */

#define BTN_OK    0x10
#define BTN_LEFT  0x08
#define BTN_RIGHT 0x04

#define SELECT_FRAMES 0x293      /* 659 frames */

/* buttons of whoever is playing (both players ORed) */
static u16 GirlSelect_ReadButtons(void)
{
    if (gPlayerActive[0] != 0) {
        if (gPlayerActive[1] != 0)
            return D_800C0D48 | D_800C0D4A;
        return D_800C0D48;
    }
    return D_800C0D4A;
}

/* @ 0x800E0000 */
/* Overlay entry: init camera/light, run GirlSelect_Task (prio 0x14) unless
 * args[0] != 0, wait until it writes its result. Returns result & 0xFFFF.
 * Quirk kept: with args[0] != 0 nothing ever writes the result (waits forever). */
s32 GirlSelect_Run(s32 *args)
{
    s32 result = 0;

    Camera_Init();
    Light_Init(gSceneLight);
    gRetracesPerFrame = 1;
    if (args[0] == 0)
        Task_Create((void (*)())GirlSelect_Task, 0x14, 8, &result, 0);
    while (result == 0)
        Task_Yield();
    gRetracesPerFrame = 1;
    return result & 0xFFFF;
}

/* @ 0x800E0094 */
/* Put the 8 girl icons on the ellipse: icon i at angle angle/256*pi + i*pi/4
 * (angle in 1/512 turns; the angle is accumulated in float), position
 * (int)(cos*120 + 160), (int)(sin*50 + 120). */
void GirlSelect_PlaceIcons(s32 angle)
{
    f32 a = (f32)angle / 256.0f * 3.14159274f;
    s32 i;

    for (i = 0; i < 8; i++) {
        f32 x = cosf(a) * 120.0f;
        f32 y;
        x = x + 160.0f;
        y = sinf(a) * 50.0f;
        y = y + 120.0f;
        Obj_SetPos(ovl3_D_800E0F40[i], (s32)x, (s32)y, 0);
        a = a + 0.785398185f;
    }
}

/* @ 0x800E01A0 */
/* Countdown display: seconds = frames / 60; tens digit hidden when 0. */
void GirlSelect_ShowTimer(s32 frames)
{
    s32 sec = frames / 60;

    if (sec / 10 != 0)
        Obj_SetData(ovl3_D_800E0F68, ovl3_D_800E0FD0[sec / 10]);
    else
        Obj_Hide(ovl3_D_800E0F68);
    Obj_SetData(ovl3_D_800E0F6C, ovl3_D_800E0FD0[sec % 10]);
}

/* @ 0x800E0278 */
/* The select screen task. Writes 0x10000 | girl to *result when done.
 * sel is the ring slot index; the girl at the front is (sel + 2) & 7.
 * Quirk kept: on a service request the result is 0x10000 | sel (not + 2). */
void GirlSelect_Task(s32 *result)
{
    s32 sel = 6;
    s32 angle = 0x80;           /* current ring angle (1/512 turns) */
    s32 target = 0x80;          /* wanted ring angle */
    s32 timer;
    s32 i, d, step;
    u16 btn;
    Obj **icons = ovl3_D_800E0F40;

    Scene_Build(ovl3_D_800E0B40, ovl3_D_800E0DA4);
    for (i = 0; i < 8; i++) {
        if (D_800C178E[i] != 0) {
            Obj_UseSetupCC0(icons[i]);
            Obj_SetColor(icons[i], 0, 0, 0, 0x80);
        }
    }
    GirlSelect_PlaceIcons(0);
    Obj_SetUpdate(ovl3_D_800E0F60, GirlSelect_UpdateArrow, 0);
    ovl3_D_800E0F60->unkA8[0] = 0;
    Obj_SetUpdate(ovl3_D_800E0F64, GirlSelect_UpdateFrame, 0);
    ovl3_D_800E0F64->unkA8[0] = 0;

    timer = SELECT_FRAMES;
    for (;;) {
        Task_Yield();
        if (D_800C30C4 != 0) {
            gServiceRequest = 1;
            goto done;
        }
        if (timer-- == 0) {
            /* time up: front girl, else the first one not yet cleared */
            if (D_800C178E[(sel + 2) & 7] != 0) {
                for (i = 0; i < 8; i++) {
                    if (D_800C178E[i] == 0) {
                        sel = (i - 2) & 7;
                        break;
                    }
                }
            }
            break;
        }
        btn = GirlSelect_ReadButtons();
        GirlSelect_ShowTimer(timer);
        if ((btn & BTN_OK) && D_800C178E[(sel + 2) & 7] == 0) {
            Sound_Play(1);
            break;
        }
        if (btn & BTN_LEFT) {
            target = (target + 0x40) & 0x1FF;
            sel = (sel - 1) & 7;
            Sound_Play(2);
        } else if (btn & BTN_RIGHT) {
            target = (target - 0x40) & 0x1FF;
            sel = (sel + 1) & 7;
            Sound_Play(2);
        }
        /* turn a quarter of the remaining way (at least 1) the short way round */
        d = (target - angle) & 0x1FF;
        if (d >= 0x100)
            d -= 0x200;
        if (d == 0) {
            step = 0;
        } else {
            step = d / 4;
            if (step == 0)
                step = (d < 0) ? -1 : 1;
        }
        angle = (angle + step) & 0x1FF;
        GirlSelect_PlaceIcons(angle);
    }

    /* chosen: zoom the girl's icon, fade the others out */
    Obj_Hide(ovl3_D_800E0F68);
    Obj_Hide(ovl3_D_800E0F6C);
    sel = (sel + 2) & 7;
    Obj_ClearUpdate(ovl3_D_800E0F60);
    Obj_ClearUpdate(ovl3_D_800E0F64);
    Obj_Hide(ovl3_D_800E0F60);
    Obj_Hide(ovl3_D_800E0F64);
    for (i = 0; i < 8; i++) {
        if (i == sel) {
            Obj_SetUpdate(icons[sel], GirlSelect_UpdateZoom, 0);
        } else {
            Obj_SetUpdate(icons[i], GirlSelect_UpdateFadeOut, 0);
            icons[i]->unkA8[0] = 0xFF;
            Obj_UseSetupCC0(icons[i]);
        }
    }
    for (i = 60; i != 0; i--)
        Task_Yield();

    /* portrait + profile fade in */
    Obj_SetData(ovl3_D_800E0F38[0], ovl3_D_800E0F88[sel]);
    Obj_SetData(ovl3_D_800E0F38[1], ovl3_D_800E0FA8[sel]);
    Obj_UseSetupCC0(ovl3_D_800E0F38[0]);
    Obj_SetColor(ovl3_D_800E0F38[0], 0, 0, 0, 0);
    Obj_SetColor(ovl3_D_800E0F38[1], 0, 0, 0, 0);
    Obj_Show(ovl3_D_800E0F38[0]);
    Obj_Show(ovl3_D_800E0F38[1]);
    i = 0;
    do {
        Task_Yield();
        i += 16;
        if (i >= 0x100)
            i = 0xFF;
        Obj_SetColor(ovl3_D_800E0F38[0], 0, 0, 0, i);
        Obj_SetColor(ovl3_D_800E0F38[1], 0, 0, 0, i);
    } while (i < 0xFF);

    /* show it for up to 120 frames or until OK */
    for (i = 1; ; i++) {
        Task_Yield();
        if (i >= 0x79)
            break;
        if (GirlSelect_ReadButtons() & BTN_OK)
            break;
    }

done:
    Scene_Destroy(ovl3_D_800E0B40, ovl3_D_800E0DA4);
    Task_Yield();
    Task_Yield();
    Task_Yield();
    *result = sel | 0x10000;
}

/* @ 0x800E09C4 */
/* Chosen icon: move 1/8 of the way to (100,120) and scale 1/8 of the way to 2.0 per frame. */
void GirlSelect_UpdateZoom(Obj *obj)
{
    f32 x = obj->pos[0], y = obj->pos[1];
    f32 sx = obj->scale[0], sy = obj->scale[1];
    f32 dx = (100.0f - x) / 8.0f;
    f32 dy = (120.0f - y) / 8.0f;
    f32 dsx = (2.0f - sx) / 8.0f;
    f32 dsy = (2.0f - sy) / 8.0f;

    obj->pos[0] = x + dx;
    obj->pos[1] = y + dy;
    obj->scale[0] = sx + dsx;
    obj->scale[1] = sy + dsy;
    Obj_Invalidate(obj);
}

/* @ 0x800E0A5C */
/* Other icons: alpha = unkA8, minus 4 per frame (clamped at 0); stops when it was 0. */
void GirlSelect_UpdateFadeOut(Obj *obj)
{
    s32 a;

    Obj_SetColor(obj, 0, 0, 0, obj->unkA8[0]);
    if (obj->unkA8[0] == 0) {
        Obj_ClearUpdate(obj);
        return;
    }
    a = (s32)obj->unkA8[0] - 4;
    obj->unkA8[0] = a;
    if (a < 0)
        obj->unkA8[0] = 0;
}

/* @ 0x800E0AB8 */
/* Arrow: 2 frames, 4 ticks each (counter in unkA8). */
void GirlSelect_UpdateArrow(Obj *obj)
{
    Obj_SetData(obj, ovl3_D_800E0F70[(obj->unkA8[0] >> 2) & 1]);
    obj->unkA8[0]++;
}

/* @ 0x800E0AFC */
/* Front frame: 4 frames, 4 ticks each (counter in unkA8). */
void GirlSelect_UpdateFrame(Obj *obj)
{
    Obj_SetData(obj, ovl3_D_800E0F78[(obj->unkA8[0] >> 2) & 3]);
    obj->unkA8[0]++;
}
