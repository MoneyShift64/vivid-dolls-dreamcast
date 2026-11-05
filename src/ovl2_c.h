/* Vivid Dolls - overlay 2 (the game itself), enemies part C
 * (RAM 0x800EAD80-0x800F00F4, overlay loaded at 0x800E0000).
 *
 * The game is a "claim the area" game (Qix / Gals Panic style). The play field
 * has a claimed-area bitmap (1 bit per 8x8 cell, ovl2_D_800FD42C) and up to two
 * players (ovl2_D_800FD3F8[2]); enemies bounce off claimed cells and kill a
 * player by touching the player or the line the player is drawing.
 *
 * This file holds the enemies:
 *  - Minion_Task:  small enemy that appears at a random free spot, wanders, and
 *                  dies when it hits a player or ends up inside claimed area
 *  - Boss_*:       a boss with a body and four wiggling 5-segment arms
 *  - Snake_*:      an 8-segment snake boss (head + 7 body segments)
 *  - Spinner_Move: a bouncing, spinning enemy (task lives elsewhere)
 *
 * The *_Task functions are cooperative tasks: they run forever, calling
 * Task_Yield (func_8000AEAC) once per frame, and return when the stage ends
 * (ovl2_D_800FD2D8 != 0).
 */
#ifndef VD_OVL2_C_H
#define VD_OVL2_C_H

#include "vd_common.h"
#include "object.h"

/* Animation sequence entry. A negative duration marks the end of the sequence. */
typedef struct AnimFrame {
    void **frame;               /* 0x00 *frame is the sprite data given to Obj_SetData */
    s32 duration;               /* 0x04 frames to wait after showing it (<0: end marker) */
} AnimFrame;

/* Player as seen by the enemy code (the real struct is much bigger: the
 * line-in-progress bitmap lives at +0x68048). */
typedef struct GamePlayer {
    s32 unk00;
    s32 x;                      /* 0x04 */
    s32 y;                      /* 0x08 */
    s32 unk0C;
    s32 unk10;
    s32 hit;                    /* 0x14 set to 1 by an enemy that touched the player / its line */
    s32 unk18;
    s32 drawing;                /* 0x1C nonzero while a line is being drawn */
} GamePlayer;

/* Boss body (local to Boss_Task, passed to the arm functions). */
typedef struct BossBody {
    f32 x, y;                   /* 0x00 field position */
    f32 vx, vy;                 /* 0x08 */
    f32 reach;                  /* 0x10 arm segment spacing factor (1.0 normal) */
    f32 scale;                  /* 0x14 shrinks slowly over time */
    s32 spin;                   /* 0x18 body rotation, 0..0x3FF */
    s32 animIdx;                /* 0x1C */
    s32 animTimer;              /* 0x20 */
    void **frame;               /* 0x24 */
    s32 target;                 /* 0x28 player index the arms reach for */
} BossBody;                     /* 0x2C bytes */

/* One boss arm. Boss_UpdateArm and Boss_UpdateArmAim write the tip fields in a
 * different order (see the functions); the boss task reads them in the
 * Boss_UpdateArmAim layout. */
typedef struct BossArm {
    f32 bend[4];                /* 0x00 angle step between segments, -120..120 */
    f32 tip10;                  /* 0x10 aim: tip x      / wiggle: angle */
    f32 tip14;                  /* 0x14 aim: tip y      / wiggle: tip x */
    f32 tip18;                  /* 0x18 aim: tip angle  / wiggle: tip y */
    f32 pad1C;
} BossArm;                      /* 0x20 bytes */

/* Snake segment / generic moving enemy (0x34 bytes). */
typedef struct Mover {
    f32 x, y;                   /* 0x00 field position */
    f32 vx, vy;                 /* 0x08 */
    f32 speed;                  /* 0x10 */
    f32 scale;                  /* 0x14 */
    s32 angle;                  /* 0x18 heading, 0..0x3FF (0x400 = 360 degrees) */
    s32 turnTo;                 /* 0x1C heading it steers towards */
    s32 timer;                  /* 0x20 frames until a new random heading */
    s32 cooldown;               /* 0x24 frames until a bounce may set a new heading */
    s32 animIdx;                /* 0x28 */
    s32 animTimer;              /* 0x2C */
    void **frame;               /* 0x30 */
} Mover;                        /* 0x34 bytes */

/* Spinning bouncer (same first 0x1C bytes as Mover). */
typedef struct Spinner {
    f32 x, y;                   /* 0x00 */
    f32 vx, vy;                 /* 0x08 */
    f32 speed;                  /* 0x10 */
    f32 scale;                  /* 0x14 */
    s32 angle;                  /* 0x18 heading */
    s32 spin;                   /* 0x1C sprite rotation, 0..0x3FF */
    s32 spinSpeed;              /* 0x20 -0x40..0x40 */
    s32 timer;                  /* 0x24 frames until the heading turns by 90 degrees */
} Spinner;

void Minion_Task(s32 unused, Obj *obj, AnimFrame **anims);                       /* 0x800EAD80 */
void Boss_UpdateArm(BossBody *b, s32 baseAngle, Obj **objs, BossArm *arm, s32 *curl,
                    AnimFrame **anims, s32 *seed, s32 animBase);                 /* 0x800EB4F0 */
void Boss_UpdateArmAim(BossBody *b, s32 baseAngle, Obj **objs, BossArm *arm, s32 *curl,
                       AnimFrame **anims, s32 *seed, s32 animBase);              /* 0x800EB998 */
void Boss_MoveBounce(BossBody *b, s32 *seed);                                    /* 0x800EBE9C */
void Boss_Task(s32 unused, Obj **objs, s32 x, s32 y, AnimFrame **anims);         /* 0x800EC0CC */
void Snake_MoveHead(Mover *head, Obj *obj, AnimFrame **anims, s32 *seed, s32 frozen); /* 0x800EE230 */
void Snake_FollowBody(Mover *seg, Obj **objs, AnimFrame **anims);                /* 0x800EE510 */
void Snake_CurlFire(Mover *seg, Obj **objs, AnimFrame **anims, s32 step,
                    s32 countdown, s32 frame);                                   /* 0x800EE7FC */
void Snake_CurlBeam(Mover *seg, Obj **objs, AnimFrame **anims, s32 step,
                    s32 *fire, s32 *done, s32 target);                           /* 0x800EEB78 */
void Snake_Task(s32 unused, Obj **objs, s32 x, s32 y, AnimFrame **anims);        /* 0x800EEEEC */
void Spinner_Move(Spinner *s, Obj *obj, AnimFrame **anims, s32 *seed, s32 mode);  /* 0x800EFCF0 */

#endif
