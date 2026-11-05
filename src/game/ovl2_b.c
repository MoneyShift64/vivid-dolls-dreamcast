/* Vivid Dolls - overlay 2 (the game), part B: player movement and two enemy tasks
 * (ROM overlay 2 loaded at 0x800E0000; functions 0x800E4EDC-0x800EAD80).
 * See ovl2_b.h for the play field / player structures.
 */
#include "ovl2_b.h"
#include "object.h"
#include "collide.h"
#include "vd_math.h"
#include "libm.h"

/* ---- other parts of overlay 2 (not decompiled here) ---- */
extern s32  ovl2_func_800E29C4(void);                        /* play field width W */
extern s32  ovl2_func_800E29D0(void);                        /* play field height H */
extern void ovl2_func_800E2CC0(s32 x, s32 y);                /* claim cell (1), show photo pixel */
extern void ovl2_func_800E2D54(s32 x, s32 y, u16 pixel);     /* open cell (0), restore pixel */
extern s32  ovl2_func_800E2E24(s32 x, s32 y);                /* read cell (4 = outside) */
extern s32  ovl2_func_800E3C64(u8 *blocks, s32 x, s32 y);    /* is the 8x8 block at (x, y) solid */
extern s32  ovl2_func_800E3D28(GamePlayer *p, s32 x, s32 y, s32 w, s32 h); /* touches player/trail */
extern void ovl2_func_800E4A30(GamePlayer *p);               /* close the shape: fill the claimed area */
extern void ovl2_func_800E4CFC(GamePlayer *p, s32 dx, s32 dy); /* one drawing step (pushes a TrailPoint) */

/* ---- main code ---- */
extern void func_8000AEAC(void);        /* task yield: wait for the next frame */
extern s32  func_8001BA90(s32 sfx);     /* play a sound effect */
extern void bzero(void *p, u32 n);
extern s32  D_800B4AA0;                 /* difficulty level (index into the speed tables) */

/* ---- overlay 2 data ---- */
extern u8 ovl2_D_800F9940[16];          /* d-pad bits (up 1, down 2, left 4, right 8) -> direction 0..8 */
extern u8 ovl2_D_800F9A34[8];           /* ~(1 << i): clear bit i */
extern SpeedRange ovl2_D_800F9A40[];    /* Enemy_BounceTask speed per level */
extern SpeedRange ovl2_D_800F9AC0[];    /* Enemy_WanderTask speed per level */
extern s32 ovl2_D_800FD2B8;             /* scroll x */
extern s32 ovl2_D_800FD2C0;             /* enemies frozen when non-zero */
extern s32 ovl2_D_800FD2C8;             /* scroll y */
extern s32 ovl2_D_800FD2D8;             /* stop flag: tasks end when it is set */
extern GamePlayer *ovl2_D_800FD3F8;     /* player 1 */
extern GamePlayer *ovl2_D_800FD3FC;     /* player 2 */
extern u8 *ovl2_D_800FD42C;             /* solid-block bitmap used by the enemies */

/* Directions (index of ovl2_D_800F9940's values) */
enum { DIR_NONE, DIR_UP, DIR_UP_RIGHT, DIR_RIGHT, DIR_DOWN_RIGHT,
       DIR_DOWN, DIR_DOWN_LEFT, DIR_LEFT, DIR_UP_LEFT };

#define TASK_STOPPED() (ovl2_D_800FD2D8 != 0)

/* ======================================================================
 * helpers
 * ====================================================================== */

/* (x, y) is at least 8 pixels inside the play field and inside the visible
 * window (304 x 224 pixels starting 8 pixels into the 320x240 screen). */
static s32 Field_InView(s32 x, s32 y)
{
    s32 w = ovl2_func_800E29C4();
    s32 h = ovl2_func_800E29D0();

    if (x < 8 || x >= w - 8 || y < 8 || y >= h - 8)
        return 0;
    if ((u32)y - (u32)ovl2_D_800FD2C8 - 8u >= 0xE0u)
        return 0;
    if ((u32)x - (u32)ovl2_D_800FD2B8 - 8u >= 0x130u)
        return 0;
    return 1;
}

/* Clear the "trail passes here" bit of the 8x8 block containing (x, y). */
static void Trail_ClearBlock(GamePlayer *p, s32 x, s32 y)
{
    s32 bx = x >> 3;
    s32 i = (bx + (y >> 3) * (ovl2_func_800E29C4() >> 3)) >> 3;

    p->blockBits[i] &= ovl2_D_800F9A34[bx & 7];
}

/* Take back the last trail step: the cell becomes open again with its old pixel. */
static void Trail_UndoStep(GamePlayer *p)
{
    TrailPoint *t;

    p->trailLen--;
    t = &p->trail[p->trailLen];
    ovl2_func_800E2D54(t->x + t->dx, t->y + t->dy, t->savedPixel);
    Trail_ClearBlock(p, t->x + t->dx, t->y + t->dy);
}

/* Rewind one frame of drawing (4 trail points) and move the player back. */
static void Trail_UndoFrame(GamePlayer *p)
{
    TrailPoint *t;

    Trail_UndoStep(p);
    Trail_UndoStep(p);
    Trail_UndoStep(p);
    Trail_UndoStep(p);
    t = &p->trail[p->trailLen];
    p->x = t->x;
    p->y = t->y;
    Trail_ClearBlock(p, t->x, t->y);
}

/* Turn one frame of trail (4 points, newest first) into claimed cells. */
static void Trail_FillFrame(GamePlayer *p)
{
    TrailPoint *t;
    s32 i;

    for (i = 0; i < 4; i++) {
        p->trailLen--;
        t = &p->trail[p->trailLen];
        ovl2_func_800E2CC0(t->x + t->dx, t->y + t->dy);
    }
    t = &p->trail[p->trailLen];
    p->x = t->x;
    p->y = t->y;
    ovl2_func_800E2CC0(p->x, p->y);
}

/* ======================================================================
 * Drawing (moving through open cells)
 * ====================================================================== */

/* Can the player draw two pixels in direction (dx, dy)? The target must be
 * open, or an edge line reached through an open cell (that closes the shape). */
static s32 Draw_CanStep2(GamePlayer *p, s32 dx, s32 dy)
{
    s32 tx = p->x + dx * 2, ty = p->y + dy * 2;
    s32 target, mid;

    if (p->trailLen >= TRAIL_MAX)
        return 0;
    if (!Field_InView(tx, ty))
        return 0;
    target = ovl2_func_800E2E24(tx, ty);
    mid = ovl2_func_800E2E24(p->x + dx, p->y + dy);
    if (target == 3 && mid == 0)
        return 1;
    return target == 0;
}

/* Simpler test used for the two halves of a diagonal: (tx, ty) must be open. */
static s32 Draw_CanReach(GamePlayer *p, s32 tx, s32 ty)
{
    if (p->trailLen >= TRAIL_MAX)
        return 0;
    if (!Field_InView(tx, ty))
        return 0;
    return ovl2_func_800E2E24(tx, ty) == 0;
}

/* One frame of drawing always pushes 4 trail points: a then a, b then b
 * (straight moves use b = (0, 0)). */
static void Draw_Push(GamePlayer *p, s32 ax, s32 ay, s32 bx, s32 by)
{
    ovl2_func_800E4CFC(p, ax, ay);
    ovl2_func_800E4CFC(p, ax, ay);
    ovl2_func_800E4CFC(p, bx, by);
    ovl2_func_800E4CFC(p, bx, by);
}

static void Draw_Straight(GamePlayer *p, s32 dx, s32 dy)
{
    if (Draw_CanStep2(p, dx, dy))
        Draw_Push(p, dx, dy, 0, 0);
}

/* Diagonal: two pixels horizontally then two vertically (or the other way round
 * if that is the only open path); if the diagonal is blocked, fall back to the
 * vertical, then the horizontal direction. */
static void Draw_Diagonal(GamePlayer *p, s32 dx, s32 dy)
{
    if (Draw_CanStep2(p, dx, dy)) {
        if (Draw_CanReach(p, p->x + dx * 2, p->y)) {
            Draw_Push(p, dx, 0, 0, dy);
            return;
        }
        if (Draw_CanReach(p, p->x, p->y + dy * 2)) {
            Draw_Push(p, 0, dy, dx, 0);
            return;
        }
    }
    if (Draw_CanStep2(p, 0, dy))
        Draw_Push(p, 0, dy, 0, 0);
    else if (Draw_CanStep2(p, dx, 0))
        Draw_Push(p, dx, 0, 0, 0);
}

/* @ 0x800E4EDC */
/* Move while the draw button is held: extend the trail by up to 2 pixels in
 * direction `dir` (0..8, see the DIR_ enum; anything else does nothing). */
void Player_DrawMove(u32 dir, GamePlayer *p)
{
    switch (dir) {
    case DIR_UP:         Draw_Straight(p, 0, -1); break;
    case DIR_UP_RIGHT:   Draw_Diagonal(p, 1, -1); break;
    case DIR_RIGHT:      Draw_Straight(p, 1, 0); break;
    case DIR_DOWN_RIGHT: Draw_Diagonal(p, 1, 1); break;
    case DIR_DOWN:       Draw_Straight(p, 0, 1); break;
    case DIR_DOWN_LEFT:  Draw_Diagonal(p, -1, 1); break;
    case DIR_LEFT:       Draw_Straight(p, -1, 0); break;
    case DIR_UP_LEFT:    Draw_Diagonal(p, -1, -1); break;
    }
}

/* ======================================================================
 * Walking on the edge lines
 * ====================================================================== */

/* (tx, ty) in view and the cell (cx, cy) is an edge line. */
static s32 Edge_Is(s32 tx, s32 ty, s32 cx, s32 cy)
{
    if (!Field_InView(tx, ty))
        return 0;
    return ovl2_func_800E2E24(cx, cy) == 3;
}

static s32 Edge_CanStepX(GamePlayer *p, s32 dx)
{
    return Edge_Is(p->x + dx * 2, p->y, p->x + dx, p->y);
}

static s32 Edge_CanStepY(GamePlayer *p, s32 dy)
{
    return Edge_Is(p->x, p->y + dy * 2, p->x, p->y + dy);
}

static void Edge_StepX(GamePlayer *p, s32 dx)
{
    p->edgeDirX = dx;
    p->x += dx * 2;
}

static void Edge_StepY(GamePlayer *p, s32 dy)
{
    p->edgeDirY = dy;
    p->y += dy * 2;
}

/* Moving vertically (dy) but the edge does not continue that way: try to go
 * round a corner sideways in direction sx. Returns 0 if there is no edge at
 * all on that side (the caller then tries the other side). */
static s32 Edge_SlideX(GamePlayer *p, s32 sx, s32 dy)
{
    s32 x2;

    if (!Edge_CanStepX(p, sx))
        return 0;
    x2 = p->x + sx * 2;
    if (Edge_Is(x2, p->y + dy * 2, x2, p->y + dy * 2) &&
        Edge_Is(x2, p->y + dy, x2, p->y + dy))
        Edge_StepX(p, sx);
    return 1;
}

/* Same for horizontal movement (dx), sliding vertically in direction sy. */
static s32 Edge_SlideY(GamePlayer *p, s32 sy, s32 dx)
{
    s32 y2;

    if (!Edge_CanStepY(p, sy))
        return 0;
    y2 = p->y + sy * 2;
    if (Edge_Is(p->x + dx * 2, y2, p->x + dx * 2, y2) &&
        Edge_Is(p->x + dx, y2, p->x + dx, y2))
        Edge_StepY(p, sy);
    return 1;
}

/* Straight vertical move; when blocked, slide sideways, preferring the last
 * horizontal direction. */
static void Edge_MoveVertical(GamePlayer *p, s32 dy)
{
    s32 sx;

    if (Edge_CanStepY(p, dy)) {
        Edge_StepY(p, dy);
        return;
    }
    sx = (p->edgeDirX >= 0) ? 1 : -1;
    if (!Edge_SlideX(p, sx, dy))
        Edge_SlideX(p, -sx, dy);
}

static void Edge_MoveHorizontal(GamePlayer *p, s32 dx)
{
    s32 sy;

    if (Edge_CanStepX(p, dx)) {
        Edge_StepX(p, dx);
        return;
    }
    sy = (p->edgeDirY >= 0) ? 1 : -1;
    if (!Edge_SlideY(p, sy, dx))
        Edge_SlideY(p, -sy, dx);
}

/* @ 0x800E664C */
/* Move along the edge lines (draw button not held): 2 pixels per frame.
 * Diagonals try both axes, each one only if the edge continues that way;
 * up-right tries vertical first, the other diagonals horizontal first. */
void Player_EdgeMove(u32 dir, GamePlayer *p)
{
    switch (dir) {
    case DIR_UP:
        Edge_MoveVertical(p, -1);
        break;
    case DIR_UP_RIGHT:
        if (Edge_CanStepY(p, -1))
            Edge_StepY(p, -1);
        if (Edge_CanStepX(p, 1))
            Edge_StepX(p, 1);
        break;
    case DIR_RIGHT:
        Edge_MoveHorizontal(p, 1);
        break;
    case DIR_DOWN_RIGHT:
        if (Edge_CanStepX(p, 1))
            Edge_StepX(p, 1);
        if (Edge_CanStepY(p, 1))
            Edge_StepY(p, 1);
        break;
    case DIR_DOWN:
        Edge_MoveVertical(p, 1);
        break;
    case DIR_DOWN_LEFT:
        if (Edge_CanStepX(p, -1))
            Edge_StepX(p, -1);
        if (Edge_CanStepY(p, 1))
            Edge_StepY(p, 1);
        break;
    case DIR_LEFT:
        Edge_MoveHorizontal(p, -1);
        break;
    case DIR_UP_LEFT:
        if (Edge_CanStepX(p, -1))
            Edge_StepX(p, -1);
        if (Edge_CanStepY(p, -1))
            Edge_StepY(p, -1);
        break;
    }
}

/* ======================================================================
 * Player update and trail tasks
 * ====================================================================== */

/* @ 0x800E8F74 */
/* Per-frame player control. input: bits 0-3 d-pad, bit 4 draw button.
 * Draw button: draw (if nothing could be drawn and there is no trail, walk on
 * the edge instead). No button: walk on the edge, or, if there is a trail,
 * rewind it by one frame - but only while that point is on screen (otherwise
 * return at once, skipping the shape-closed check). */
void Player_Update(u16 input, GamePlayer *p)
{
    u32 in = input;
    u32 dir;

    if (p->inputLock != 0) {
        p->inputLock--;
        in = 0;
    }
    dir = ovl2_D_800F9940[in & 0xF];
    if (in & 0x10) {
        Player_DrawMove(dir, p);
        if (p->trailLen == 0)
            Player_EdgeMove(dir, p);
    } else if (p->trailLen == 0) {
        Player_EdgeMove(dir, p);
    } else {
        TrailPoint *t = p->trail + (p->trailLen - 4);

        if ((u32)t->y - (u32)ovl2_D_800FD2C8 - 8u >= 0xE0u)
            return;
        if ((u32)t->x - (u32)ovl2_D_800FD2B8 - 8u >= 0x130u)
            return;
        Trail_UndoFrame(p);
    }
    if (p->closed)
        ovl2_func_800E4A30(p);
}

/* @ 0x800E93EC */
/* Task: rewind the whole trail, two frames of trail (2 x 4 points) per yield,
 * until it is empty or the stop flag is set; then *done = 1. */
void Player_RewindTask(GamePlayer *p, s32 *done)
{
    while (p->trailLen != 0) {
        func_8000AEAC();
        if (TASK_STOPPED())
            break;
        Trail_UndoFrame(p);
        if (p->trailLen == 0)
            break;
        Trail_UndoFrame(p);
    }
    *done = 1;
}

/* @ 0x800E9B10 */
/* Task: turn the whole trail into claimed cells (newest first), two frames of
 * trail per yield, until it is empty or the stop flag is set; then *done = 1. */
void Player_FillTrailTask(GamePlayer *p, s32 *done)
{
    while (p->trailLen != 0) {
        func_8000AEAC();
        if (TASK_STOPPED())
            break;
        Trail_FillFrame(p);
        if (p->trailLen == 0)
            break;
        Trail_FillFrame(p);
    }
    *done = 1;
}

/* ======================================================================
 * Enemy tasks
 * ====================================================================== */

/* Random speed (base + rand(range)) / 10 for the current level, computed in double. */
static f32 Enemy_RandSpeed(s32 *seed, SpeedRange *tbl)
{
    s32 r = (s32)Rand_Range(seed, tbl[D_800B4AA0].range);

    return (f32)((f64)(f32)(tbl[D_800B4AA0].base + r) / 10.0);
}

/* Snap a coordinate that went into a solid block back out of it. */
static s32 Enemy_SnapOut(f32 pos, f32 vel)
{
    s32 i = (s32)pos;

    if (vel > 0.0f)
        return (i & ~7) - 1;
    return (i + 7) & ~7;
}

static void Enemy_ShowAt(Obj *obj, f32 x, f32 y)
{
    Obj_SetPos(obj, (s32)(x - (f32)ovl2_D_800FD2B8), (s32)(y - (f32)ovl2_D_800FD2C8), 0);
}

/* Wait 10..19 seconds; returns 0 if the task was stopped meanwhile. */
static s32 Enemy_WaitRandom(s32 *seed)
{
    s32 n = ((s32)Rand_Range(seed, 10) + 10) * 60 - 1;

    while (n != -1) {
        func_8000AEAC();
        if (TASK_STOPPED())
            return 0;
        n--;
    }
    return 1;
}

/* Pick a random open cell at least 32 pixels from the field border (yielding every
 * 64 tries); returns 0 if the task was stopped meanwhile. */
static s32 Enemy_FindSpot(s32 *seed, s32 *x, s32 *y)
{
    s32 w = ovl2_func_800E29C4();
    s32 h = ovl2_func_800E29D0();
    s32 tries = 1;

    for (;;) {
        *x = (s32)Rand_Range(seed, w - 64) + 32;
        *y = (s32)Rand_Range(seed, h - 64) + 32;
        if (ovl2_func_800E2E24(*x, *y) == 0)
            return 1;
        if ((tries++ & 0x3F) == 0) {
            func_8000AEAC();
            if (TASK_STOPPED())
                return 0;
        }
    }
}

/* Next frame of a one-shot animation; returns 0 at its end. */
static s32 Enemy_AnimOnce(Obj *obj, AnimFrame *list, s32 *frame, s32 *timer)
{
    if (*timer == 0) {
        AnimFrame *f = &list[++*frame];

        *timer = f->frames;
        if (*timer < 0) {
            *frame = 0;
            return 0;
        }
        Obj_SetData(obj, *f->sprite);
    } else {
        (*timer)--;
    }
    return 1;
}

/* Next frame of a looping animation (wraps to index 0). */
static void Enemy_AnimLoop(Obj *obj, EnemyAnims *anims, s32 *frame, s32 *timer)
{
    AnimFrame *f;

    if (*timer == 0) {
        ++*frame;
        for (;;) {
            f = &anims->move[*frame];
            *timer = f->frames;
            if (*timer >= 0)
                break;
            *frame = 0;
        }
        Obj_SetData(obj, *f->sprite);
    } else {
        (*timer)--;
    }
}

/* Did the enemy at (x, y) touch a player (sets its hit flag) or run into a
 * solid block? */
static s32 Enemy_Collide(f32 x, f32 y)
{
    if (ovl2_D_800FD3F8->hit == 0 &&
        ovl2_func_800E3D28(ovl2_D_800FD3F8, (s32)x, (s32)y, 16, 16)) {
        ovl2_D_800FD3F8->hit = 1;
        return 1;
    }
    if (ovl2_D_800FD3FC->hit == 0 &&
        ovl2_func_800E3D28(ovl2_D_800FD3FC, (s32)x, (s32)y, 16, 16)) {
        ovl2_D_800FD3FC->hit = 1;
        return 1;
    }
    return ovl2_func_800E3C64(ovl2_D_800FD42C, (s32)x, (s32)y) != 0;
}

/* @ 0x800E9DC0 */
/* Enemy task (bouncing): wait 10-19 s, appear on a random open cell, move in
 * straight lines bouncing off solid blocks (1 in 10 bounces picks a new random
 * speed), until it touches a player (player->hit = 1) or ends up in a solid
 * block; play the vanish animation, hide, start again. Runs until the stop flag
 * is set, then hides the object. The random seed starts as the object's address. */
void Enemy_BounceTask(void *unused, Obj *obj, EnemyAnims *anims)
{
    struct { f32 x, y, vx, vy; s32 frame, timer; void *cur; } st;
    s32 seed = (s32)obj;
    s32 sx, sy;

    (void)unused;
    bzero(&st, sizeof(st));
    for (;;) {
        if (!Enemy_WaitRandom(&seed))
            break;
        if (!Enemy_FindSpot(&seed, &sx, &sy))
            break;
        st.x = (f32)sx;
        st.y = (f32)sy;
        st.vx = Enemy_RandSpeed(&seed, ovl2_D_800F9A40);
        st.vy = Enemy_RandSpeed(&seed, ovl2_D_800F9A40);
        func_8001BA90(0xD);

        /* appear */
        st.frame = 0;
        st.timer = 0;
        for (;;) {
            func_8000AEAC();
            if (TASK_STOPPED())
                goto stop;
            if (!Enemy_AnimOnce(obj, anims->appear, &st.frame, &st.timer))
                break;
            Enemy_ShowAt(obj, st.x, st.y);
        }

        /* move */
        st.timer = 0;
        for (;;) {
            func_8000AEAC();
            if (TASK_STOPPED())
                goto stop;
            Enemy_AnimLoop(obj, anims, &st.frame, &st.timer);
            if (ovl2_D_800FD2C0 == 0) {
                st.x = st.x + st.vx;
                if (ovl2_func_800E3C64(ovl2_D_800FD42C, (s32)st.x, (s32)st.y)) {
                    st.x = (f32)Enemy_SnapOut(st.x, st.vx);
                    st.vx = -st.vx;
                    if (Rand_Range(&seed, 10) == 0) {
                        if (st.vx < 0.0f)
                            st.vx = -Enemy_RandSpeed(&seed, ovl2_D_800F9A40);
                        else
                            st.vx = Enemy_RandSpeed(&seed, ovl2_D_800F9A40);
                    }
                }
                st.y = st.y + st.vy;
                if (ovl2_func_800E3C64(ovl2_D_800FD42C, (s32)st.x, (s32)st.y)) {
                    st.y = (f32)Enemy_SnapOut(st.y, st.vy);
                    st.vy = -st.vy;
                    if (Rand_Range(&seed, 10) == 0) {
                        if (st.vy < 0.0f)
                            st.vy = -Enemy_RandSpeed(&seed, ovl2_D_800F9A40);
                        else
                            st.vy = Enemy_RandSpeed(&seed, ovl2_D_800F9A40);
                    }
                }
            }
            Enemy_ShowAt(obj, st.x, st.y);
            if (Enemy_Collide(st.x, st.y))
                break;
        }

        /* vanish */
        func_8001BA90(0x11);
        st.frame = 0;
        st.timer = 0;
        for (;;) {
            func_8000AEAC();
            if (TASK_STOPPED())
                goto stop;
            if (!Enemy_AnimOnce(obj, anims->vanish, &st.frame, &st.timer))
                break;
            Enemy_ShowAt(obj, st.x, st.y);
        }
        Obj_Hide(obj);
    }
stop:
    Obj_Hide(obj);
}

/* @ 0x800EA610 */
/* Enemy task (wandering): like Enemy_BounceTask, but moves at a random speed in
 * a direction angle (0..1023 = full turn) that turns 4 units per frame towards
 * a target angle re-chosen every 60-89 frames; bounces reflect the velocity and
 * recompute the angle. The sprite is rotated to face the angle (obj->rot[2]). */
void Enemy_WanderTask(void *unused, Obj *obj, EnemyAnims *anims)
{
    struct { f32 x, y, vx, vy, speed; s32 angle, target, turn, frame, timer; void *cur; } st;
    s32 seed = (s32)obj;
    s32 sx, sy;

    (void)unused;
    bzero(&st, sizeof(st));
    for (;;) {
        if (!Enemy_WaitRandom(&seed))
            break;
        if (!Enemy_FindSpot(&seed, &sx, &sy))
            break;
        st.x = (f32)sx;
        st.y = (f32)sy;
        st.angle = (s32)Rand_Range(&seed, 0x400);
        st.target = st.angle;
        st.speed = Enemy_RandSpeed(&seed, ovl2_D_800F9AC0);
        st.turn = 0;
        func_8001BA90(0xD);

        /* appear */
        st.frame = 0;
        st.timer = 0;
        for (;;) {
            func_8000AEAC();
            if (TASK_STOPPED())
                goto stop;
            if (!Enemy_AnimOnce(obj, anims->appear, &st.frame, &st.timer))
                break;
            Enemy_ShowAt(obj, st.x, st.y);
        }

        /* move */
        st.timer = 0;
        for (;;) {
            func_8000AEAC();
            if (TASK_STOPPED())
                goto stop;
            Enemy_AnimLoop(obj, anims, &st.frame, &st.timer);
            if (ovl2_D_800FD2C0 == 0) {
                st.vx = cosf((f32)st.angle / 512.0f * 3.1415927f) * st.speed;
                st.vy = sinf((f32)st.angle / 512.0f * 3.1415927f) * st.speed;
                st.x = st.x + st.vx;
                if (ovl2_func_800E3C64(ovl2_D_800FD42C, (s32)st.x, (s32)st.y)) {
                    st.x = (f32)Enemy_SnapOut(st.x, st.vx);
                    st.vx = -st.vx;
                    st.angle = Math_VecToAngle(st.vx, st.vy);
                }
                st.y = st.y + st.vy;
                if (ovl2_func_800E3C64(ovl2_D_800FD42C, (s32)st.x, (s32)st.y)) {
                    st.y = (f32)Enemy_SnapOut(st.y, st.vy);
                    st.vy = -st.vy;
                    st.angle = Math_VecToAngle(st.vx, st.vy);
                }
                obj->rot[2] = (f32)(-st.angle & 0x3FF) / 512.0f * 180.0f;
                if (st.target != st.angle) {
                    if ((st.target - st.angle) & 0x200)
                        st.angle -= 4;
                    else
                        st.angle += 4;
                    st.angle &= 0x3FF;
                }
                if (st.turn == 0) {
                    st.turn = (s32)Rand_Range(&seed, 30) + 60;
                    st.target = (s32)Rand_Range(&seed, 0x400);
                } else {
                    st.turn--;
                }
            }
            Enemy_ShowAt(obj, st.x, st.y);
            if (Enemy_Collide(st.x, st.y))
                break;
        }

        /* vanish */
        func_8001BA90(0x11);
        st.frame = 0;
        st.timer = 0;
        for (;;) {
            func_8000AEAC();
            if (TASK_STOPPED())
                goto stop;
            if (!Enemy_AnimOnce(obj, anims->vanish, &st.frame, &st.timer))
                break;
            Enemy_ShowAt(obj, st.x, st.y);
        }
        Obj_Hide(obj);
    }
stop:
    Obj_Hide(obj);
}
