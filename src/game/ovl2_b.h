/* Vivid Dolls - overlay 2 (the game itself), part B: 0x800E4EDC-0x800EAD80.
 *
 * Vivid Dolls is a "claim the area" game (Qix / Gals Panic style). The play field
 * is a W x H grid of 2-bit cells (4 per byte, ovl2_D_800FBA7C, width
 * ovl2_D_800FBA70, height ovl2_D_800FBA74):
 *     0 = open (not claimed yet)   1 = claimed (photo pixel revealed)
 *     2 = the player's current trail   3 = edge line (border of the claimed area)
 * ovl2_func_800E2E24(x, y) reads a cell (4 when outside the grid).
 * A 16-bit frame buffer of the same size (ovl2_D_800FBA80) holds what is shown;
 * ovl2_D_800FBA84 is the hidden photo. The visible window scrolls:
 * ovl2_D_800FD2B8 = scroll x, ovl2_D_800FD2C8 = scroll y (screen 320x240,
 * positions must stay 8 pixels inside it).
 *
 * This part contains the player's movement (drawing a trail into the open area,
 * walking along the edge lines, rewinding the trail) and two "enemy" tasks (small
 * bouncing sprites that kill the player when they touch the trail).
 */
#ifndef VD_OVL2_B_H
#define VD_OVL2_B_H

#include "vd_common.h"
#include "object.h"

/* One trail point, pushed for every 1-pixel step while drawing
 * (ovl2_func_800E4CFC). Each frame pushes exactly 4 of them. */
typedef struct TrailPoint {
    s8 dx, dy;          /* 0x00 the step taken (-1/0/1) */
    u16 dirCode;        /* 0x02 from a table at ovl2_D_800F997A, indexed by dx and dy */
    s16 x, y;           /* 0x04 position BEFORE the step */
    u16 savedPixel;     /* 0x08 frame-buffer pixel at (x+dx, y+dy) before it was drawn over */
} TrailPoint;           /* 10 bytes */

#define TRAIL_MAX 0x2000

/* A player (ovl2_D_800FD3F8 = player 1, ovl2_D_800FD3FC = player 2). */
typedef struct GamePlayer {
    s32 id;                         /* 0x00000 0 = player 1 (trail colour 0xFD29), else 0xA53F */
    s32 x, y;                       /* 0x00004 position in grid pixels */
    s32 unk0C;
    s32 closed;                     /* 0x00010 set by a step that reached an edge line: the shape is closed */
    s32 hit;                        /* 0x00014 set when an enemy touched the player / trail */
    s32 unk18;
    s32 trailLen;                   /* 0x0001C number of TrailPoints */
    TrailPoint trail[TRAIL_MAX];    /* 0x00020 */
    u8 fillBufA[0x10000];           /* 0x14020 work buffers of the area fill (ovl2_func_800E41B8) */
    u8 fillBufB[0x10000];           /* 0x24020 */
    s16 fillResultA, fillResultB;   /* 0x34020 */
    u8 blockBits[0x258];            /* 0x34024 1 bit per 8x8 block the trail passes through
                                     * (bit (bx & 7), LSB first; row stride W/64 bytes) */
    s32 edgeDirY;                   /* 0x3427C last vertical direction walked on the edge (-1/1) */
    s32 edgeDirX;                   /* 0x34280 last horizontal direction walked on the edge (-1/1) */
    u8 unk34284[0x14];
    s32 inputLock;                  /* 0x34298 frames left during which input is ignored */
} GamePlayer;

/* Animation frame list entry used by the enemy tasks: a list starts at index 1
 * (index 0 is never shown) and ends with frames < 0. */
typedef struct AnimFrame {
    void **sprite;      /* Obj_SetData(obj, *sprite) */
    s32 frames;         /* frames to wait after showing it */
} AnimFrame;

typedef struct EnemyAnims {
    AnimFrame *appear;  /* played once */
    AnimFrame *move;    /* looped while moving */
    AnimFrame *vanish;  /* played once, then the enemy hides and waits */
} EnemyAnims;

/* (base, range) speed table entry: speed = (base + rand(range)) / 10 */
typedef struct SpeedRange { s32 base, range; } SpeedRange;

void Player_DrawMove(u32 dir, GamePlayer *p);                       /* 0x800E4EDC */
void Player_EdgeMove(u32 dir, GamePlayer *p);                       /* 0x800E664C */
void Player_Update(u16 input, GamePlayer *p);                       /* 0x800E8F74 */
void Player_RewindTask(GamePlayer *p, s32 *done);                   /* 0x800E93EC */
void Player_FillTrailTask(GamePlayer *p, s32 *done);                /* 0x800E9B10 */
void Enemy_BounceTask(void *unused, Obj *obj, EnemyAnims *anims);   /* 0x800E9DC0 */
void Enemy_WanderTask(void *unused, Obj *obj, EnemyAnims *anims);   /* 0x800EA610 */

#endif
