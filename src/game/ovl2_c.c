/* Vivid Dolls - overlay 2 (the game), enemies part C (RAM 0x800EAD80-0x800F00F4).
 * See ovl2_c.h for an overview. Float expressions keep the original's exact
 * operation order (single precision, except where the original divides in
 * double precision). */
#include "ovl2_c.h"
#include "collide.h"
#include "vd_math.h"
#include "libm.h"

#define PI_F 3.1415927f

/* ---- game globals (overlay 2 BSS / main BSS) ---- */
extern s32 ovl2_D_800FD2D8;             /* nonzero: stage over, tasks must quit */
extern s32 ovl2_D_800FD2C0;             /* nonzero: game paused (enemies freeze) */
extern s32 ovl2_D_800FD2B8;             /* scroll x (field -> screen) */
extern s32 ovl2_D_800FD2C8;             /* scroll y */
extern s16 ovl2_D_800FD2C4;             /* boss x (for the camera) */
extern s16 ovl2_D_800FD2B4;             /* boss y */
extern u8 *ovl2_D_800FD42C;             /* claimed-area bitmap, 1 bit per 8x8 cell */
extern GamePlayer *ovl2_D_800FD3F8[2];  /* players */
extern u8 ovl2_D_800FD4F0[];            /* argument block for the effect task */
extern s32 D_800B4AA0;                  /* difficulty level (0..7) */
extern s32 D_800B04D4[2];               /* nonzero: player 1/2 is playing */

/* ---- overlay 2 data tables ---- */
extern s32 ovl2_D_800F9B40[][2];        /* minion speed per level: {base, random range} (x0.1) */
extern s32 ovl2_D_800F9BC0[];           /* boss: 1-in-N chance per frame to attack, per level */
extern s32 ovl2_D_800F9C00[4][2];       /* boss: unstick offsets {dx, dy} */
extern s32 ovl2_D_800F9C20[];           /* snake: 1-in-N chance per frame to attack, per level */
extern s32 ovl2_D_800F9C60[][2];        /* snake speed per level: {base, random range} (x0.1) */
extern s32 ovl2_D_800F9CE0[4][2];       /* snake: unstick offsets {dx, dy} */

/* ---- other overlay 2 functions ---- */
s32  ovl2_func_800E29C4(void);                                  /* field width */
s32  ovl2_func_800E29D0(void);                                  /* field height */
s32  ovl2_func_800E2E24(s32 x, s32 y);                          /* field pixel state (nonzero: not free) */
s32  ovl2_func_800E3C64(u8 *bitmap, s32 x, s32 y);              /* claimed cell at (x, y)? (borders count) */
s32  ovl2_func_800E3D28(GamePlayer *p, s32 x, s32 y, s32 w, s32 h); /* box touches player or its line? */
void ovl2_func_800F2BB0(s32 x, s32 y, s32 target, s32 angle);   /* boss: spawn a shot */
void ovl2_func_800F2F6C(s32 x, s32 y, s32 angle);               /* boss: spawn an arm shot */
void ovl2_func_800F32FC(s32 x, s32 y, s32 angle);               /* snake: spawn fire */
void ovl2_func_800F3B2C(s32 x, s32 y, s32 target, s32 angle, s32 *done); /* snake: spawn beam */
void ovl2_func_800F4060(s32 x, s32 y, s32 angle);               /* snake: spawn a small enemy */
void ovl2_func_800F4100(void);                                  /* effect task body */

/* ---- main game functions ---- */
void func_8000AEAC(void);                                       /* Task_Yield: wait one frame */
void *func_8000A76C(void *fn, s32 pri, s32 argSize, void *arg0, void *arg1); /* Task_Spawn */
s32  func_8001BA90(s32 id);                                     /* Sound_Play (returns an s16 handle) */
void func_8001BB8C(s32 handle);                                 /* Sound_Stop */
void bzero(void *p, u32 n);

#define gStageOver  ovl2_D_800FD2D8
#define gPaused     ovl2_D_800FD2C0
#define gScrollX    ovl2_D_800FD2B8
#define gScrollY    ovl2_D_800FD2C8
#define gArea       ovl2_D_800FD42C
#define gPlayers    ovl2_D_800FD3F8

/* Hit test of a square box (size x size) against player i; sets its hit flag. */
static void HitPlayer(s32 i, s32 x, s32 y, s32 size)
{
    if (gPlayers[i]->hit == 0 && ovl2_func_800E3D28(gPlayers[i], x, y, size, size))
        gPlayers[i]->hit = 1;
}

static void Obj_SetScale1(Obj *o, f32 s)
{
    o->scale[2] = s;
    o->scale[1] = s;
    o->scale[0] = s;
}

/* Angle units (0..0x400) to degrees for Obj.rot[2]. */
static f32 AngleToDeg(s32 a)
{
    return ((f32)a / 512.0f) * 180.0f;
}

/* Random speed: (f32)((base + Rand(range)) / 10.0), divided in double precision. */
static f32 RandSpeed(s32 *seed, s32 *tbl)
{
    s32 r = Rand_Range(seed, tbl[1]);
    return (f32)((f64)(f32)(tbl[0] + r) / 10.0);
}

static f32 RandSpeed10(s32 *seed)
{
    return (f32)((f64)(f32)(s32)(Rand_Range(seed, 10) + 10) / 10.0);
}

/* @ 0x800EB4F0 */
/* Boss arm, wiggling: lays out the 4 segments (objs[0..3]) and the tip
 * (objs[4]) from the body outwards, each segment turned by arm->bend[i]; every
 * bend swings by 4 per frame between -120 and 120 (*curl gives the direction).
 * Segments hit-test the players. Writes the tip as {angle, x, y} at tip10..18
 * (a different order from Boss_UpdateArmAim). 1 in 360 frames the curl
 * direction flips at random. */
void Boss_UpdateArm(BossBody *b, s32 baseAngle, Obj **objs, BossArm *arm, s32 *curl,
                    AnimFrame **anims, s32 *seed, s32 animBase)
{
    f32 ang = (f32)(b->spin + baseAngle);
    f32 t = (ang / 512.0f) * PI_F;
    f32 dx = sinf(t) * 36.0f * b->scale;
    f32 dy = cosf(t) * 36.0f * b->scale;
    s32 i;

    for (i = 0; i < 4; i++) {
        f32 *bend = &arm->bend[i];
        b->frame = anims[animBase + 2][0].frame;
        objs[i]->rot[2] = (ang / 512.0f) * 180.0f;
        Obj_SetData(objs[i], *b->frame);
        Obj_SetPos(objs[i], (s32)((b->x + dx) - (f32)gScrollX), (s32)((b->y + dy) - (f32)gScrollY), 0);
        Obj_SetScale1(objs[i], b->scale);
        if (gPlayers[0]->hit == 0) {
            s32 sz = (s32)(b->scale * 8.0f);
            if (ovl2_func_800E3D28(gPlayers[0], (s32)(b->x + dx), (s32)(b->y + dy), sz, sz))
                gPlayers[0]->hit = 1;
        }
        if (gPlayers[1]->hit == 0) {
            s32 sz = (s32)(b->scale * 8.0f);
            if (ovl2_func_800E3D28(gPlayers[1], (s32)(b->x + dx), (s32)(b->y + dy), sz, sz))
                gPlayers[1]->hit = 1;
        }
        ang += *bend;
        t = (ang / 512.0f) * PI_F;
        dx += sinf(t) * 12.0f * b->reach * b->scale;
        dy += cosf(t) * 12.0f * b->reach * b->scale;
        if (*curl != 0) {
            *bend = *bend - 4.0f;
            if (*bend < -120.0f) {
                *bend = -120.0f;
                *curl ^= 1;
            }
        } else {
            *bend = *bend + 4.0f;
            if (120.0f < *bend) {
                *bend = 120.0f;
                *curl ^= 1;
            }
        }
    }
    /* tip */
    {
        f32 deg = (ang / 512.0f) * 180.0f;
        Obj *tip = objs[4];
        b->frame = anims[animBase + 1][0].frame;
        tip->rot[2] = deg;
        Obj_SetData(tip, *b->frame);
        Obj_SetPos(tip, (s32)((b->x + dx) - (f32)gScrollX), (s32)((b->y + dy) - (f32)gScrollY), 0);
        Obj_SetScale1(tip, b->scale);
    }
    arm->tip10 = ang;
    arm->tip14 = b->x + dx;
    arm->tip18 = b->y + dy;
    if (Rand_Range(seed, 360) == 0)
        *curl ^= 1;
}

/* @ 0x800EB998 */
/* Boss arm, reaching: like Boss_UpdateArm, but each bend steers the next
 * segment towards player b->target (by 4 per frame, clamped to +-120).
 * Writes the tip as {x, y, angle} at tip10..18. curl and seed are unused.
 * Quirk kept: when the heading difference is >= 0x200 the original uses
 * baseAngle - 0x400 (not the difference - 0x400); only its sign matters. */
void Boss_UpdateArmAim(BossBody *b, s32 baseAngle, Obj **objs, BossArm *arm, s32 *curl,
                       AnimFrame **anims, s32 *seed, s32 animBase)
{
    f32 ang = (f32)(b->spin + baseAngle);
    f32 t = (ang / 512.0f) * PI_F;
    f32 dx = sinf(t) * 36.0f * b->scale;
    f32 dy = cosf(t) * 36.0f * b->scale;
    s32 i;
    (void)curl;
    (void)seed;

    for (i = 0; i < 4; i++) {
        f32 *bend = &arm->bend[i];
        GamePlayer *p;
        s32 d;
        b->frame = anims[animBase + 2][0].frame;
        objs[i]->rot[2] = (ang / 512.0f) * 180.0f;
        Obj_SetData(objs[i], *b->frame);
        Obj_SetPos(objs[i], (s32)((b->x + dx) - (f32)gScrollX), (s32)((b->y + dy) - (f32)gScrollY), 0);
        Obj_SetScale1(objs[i], b->scale);
        if (gPlayers[0]->hit == 0) {
            s32 sz = (s32)(b->scale * 8.0f);
            if (ovl2_func_800E3D28(gPlayers[0], (s32)(b->x + dx), (s32)(b->y + dy), sz, sz))
                gPlayers[0]->hit = 1;
        }
        if (gPlayers[1]->hit == 0) {
            s32 sz = (s32)(b->scale * 8.0f);
            if (ovl2_func_800E3D28(gPlayers[1], (s32)(b->x + dx), (s32)(b->y + dy), sz, sz))
                gPlayers[1]->hit = 1;
        }
        ang += *bend;
        t = (ang / 512.0f) * PI_F;
        dx += sinf(t) * 12.0f * b->reach * b->scale;
        dy += cosf(t) * 12.0f * b->reach * b->scale;

        p = gPlayers[b->target];
        d = -Math_VecToAngle((f32)(s32)((f32)p->x - (b->x + dx)), (f32)(s32)((f32)p->y - (b->y + dy)));
        d = (d - 0x100) & 0x3FF;
        d = (d - (s32)ang) & 0x3FF;
        if (d >= 0x200)
            d = baseAngle - 0x400;
        if (d != 0) {
            if (d >= 0) {
                *bend = *bend - 4.0f;
                if (*bend < -120.0f)
                    *bend = -120.0f;
            } else {
                *bend = *bend + 4.0f;
                if (120.0f < *bend)
                    *bend = 120.0f;
            }
        }
    }
    {
        f32 deg = (ang / 512.0f) * 180.0f;
        Obj *tip = objs[4];
        b->frame = anims[animBase + 1][0].frame;
        tip->rot[2] = deg;
        Obj_SetData(tip, *b->frame);
        Obj_SetPos(tip, (s32)((b->x + dx) - (f32)gScrollX), (s32)((b->y + dy) - (f32)gScrollY), 0);
        Obj_SetScale1(tip, b->scale);
    }
    arm->tip10 = b->x + dx;
    arm->tip18 = ang;
    arm->tip14 = b->y + dy;
}

/* @ 0x800EBE9C */
/* Boss body drift (unless paused): move by (vx, vy), bouncing off claimed
 * cells; after a bounce there is a 1 in 10 chance of a new random speed
 * (1.0..1.9, same direction). Publishes the position for the camera. */
void Boss_MoveBounce(BossBody *b, s32 *seed)
{
    if (gPaused != 0)
        return;
    b->x = b->x + b->vx;
    if (ovl2_func_800E3C64(gArea, (s32)b->x, (s32)b->y)) {
        f32 nx = b->x - b->vx;
        b->vx = -b->vx;
        b->x = nx;
        if (Rand_Range(seed, 10) == 0) {
            if (b->vx < 0.0f)
                b->vx = (f32)-((f64)(f32)(s32)(Rand_Range(seed, 10) + 10) / 10.0);
            else
                b->vx = (f32)((f64)(f32)(s32)(Rand_Range(seed, 10) + 10) / 10.0);
        }
    }
    b->y = b->y + b->vy;
    if (ovl2_func_800E3C64(gArea, (s32)b->x, (s32)b->y)) {
        f32 ny = b->y - b->vy;
        b->vy = -b->vy;
        b->y = ny;
        if (Rand_Range(seed, 10) == 0) {
            if (b->vy < 0.0f)
                b->vy = (f32)-((f64)(f32)(s32)(Rand_Range(seed, 10) + 10) / 10.0);
            else
                b->vy = (f32)((f64)(f32)(s32)(Rand_Range(seed, 10) + 10) / 10.0);
        }
    }
    ovl2_D_800FD2C4 = (s16)(s32)b->x;
    ovl2_D_800FD2B4 = (s16)(s32)b->y;
}

/* @ 0x800EE230 */
/* Snake head (unless paused or frozen): move along the heading, bouncing off
 * claimed cells (a bounce picks the new direction as the heading to steer to,
 * then waits 60 frames before another bounce may); steer 1/8 of the way to
 * turnTo each frame; every 60..89 frames pick a random turnTo. Always places
 * obj and publishes the position for the camera. */
void Snake_MoveHead(Mover *s, Obj *obj, AnimFrame **anims, s32 *seed, s32 frozen)
{
    (void)anims;
    if ((gPaused == 0) & (frozen == 0)) {
        s32 d;
        s->vx = cosf(((f32)s->angle / 512.0f) * PI_F) * s->speed;
        s->vy = sinf(((f32)s->angle / 512.0f) * PI_F) * s->speed;
        s->x = s->x + s->vx;
        if (ovl2_func_800E3C64(gArea, (s32)s->x, (s32)s->y)) {
            f32 nx = s->x - s->vx;
            s->vx = -s->vx;
            s->x = nx;
            if (s->cooldown == 0) {
                s->turnTo = Math_VecToAngle(s->vx, s->vy);
                s->cooldown = 60;
            }
        }
        s->y = s->y + s->vy;
        if (ovl2_func_800E3C64(gArea, (s32)s->x, (s32)s->y)) {
            f32 ny = s->y - s->vy;
            s->vy = -s->vy;
            s->y = ny;
            if (s->cooldown == 0) {
                s->turnTo = Math_VecToAngle(s->vx, s->vy);
                s->cooldown = 60;
            }
        }
        if (s->cooldown != 0)
            s->cooldown--;
        obj->rot[2] = AngleToDeg((-s->angle - 0x100) & 0x3FF);
        d = (s->turnTo - s->angle) & 0x3FF;
        if (d >= 0x200)
            d -= 0x400;
        s->angle = (s->angle + d / 8) & 0x3FF;
        if (s->timer == 0) {
            s->timer = Rand_Range(seed, 30) + 60;
            s->turnTo = Rand_Range(seed, 0x400);
        } else {
            s->timer--;
        }
    }
    Obj_SetPos(obj, (s32)(s->x - (f32)gScrollX), (s32)(s->y - (f32)gScrollY), 0);
    ovl2_D_800FD2C4 = (s16)(s32)s->x;
    ovl2_D_800FD2B4 = (s16)(s32)s->y;
}

/* @ 0x800EE510 */
/* Snake body, normal movement: each of segments 1..7 keeps 16*scale away from
 * the one before it, in the direction it already lies (a trailing chain);
 * segment i is drawn by objs[7 - i] with the first frame of anims[i]. Segments
 * hit-test the players. */
void Snake_FollowBody(Mover *seg, Obj **objs, AnimFrame **anims)
{
    s32 i;
    for (i = 1; i < 8; i++) {
        Mover *s = &seg[i];
        Obj **po = &objs[7 - i];
        s32 a;
        f32 t;
        s->frame = anims[i][0].frame;
        Obj_SetData(*po, *s->frame);
        a = Math_VecToAngle((f32)(s32)(s->x - s[-1].x), (f32)(s32)(s->y - s[-1].y));
        t = ((f32)a / 512.0f) * PI_F;
        s->x = cosf(t) * 16.0f * seg->scale + s[-1].x;
        s->y = sinf(t) * 16.0f * seg->scale + s[-1].y;
        Obj_SetPos(*po, (s32)(s->x - (f32)gScrollX), (s32)(s->y - (f32)gScrollY), 0);
        (*po)->rot[2] = AngleToDeg((-a - 0x100) & 0x3FF);
        Obj_SetScale1(*po, seg->scale);
        HitPlayer(0, (s32)s->x, (s32)s->y, (s32)(seg->scale * 8.0f));
        HitPlayer(1, (s32)s->x, (s32)s->y, (s32)(seg->scale * 8.0f));
    }
}

/* Snake body curled into an arc behind the head (attack poses): segment i is
 * placed 16*scale from segment i-1 at heading (head + 0x200) + (i-1)*step and
 * animated with anims[8 + i]. Returns the heading after the last segment. */
static s32 Snake_CurlBody(Mover *seg, Obj **objs, AnimFrame **anims, s32 step)
{
    s32 a = (seg->angle + 0x200) & 0x3FF;
    s32 i;
    for (i = 1; i < 8; i++) {
        Mover *s = &seg[i];
        Obj **po = &objs[7 - i];
        f32 t;
        if (s->animTimer == 0) {
            s->animIdx++;
            for (;;) {
                AnimFrame *f = &anims[8 + i][s->animIdx];
                s->frame = f->frame;
                s->animTimer = f->duration;
                if (s->animTimer >= 0)
                    break;
                s->animIdx = 0;
            }
            Obj_SetData(*po, *s->frame);
        } else {
            s->animTimer--;
        }
        t = ((f32)a / 512.0f) * PI_F;
        s->x = cosf(t) * 16.0f * seg->scale + s[-1].x;
        s->y = sinf(t) * 16.0f * seg->scale + s[-1].y;
        Obj_SetPos(*po, (s32)(s->x - (f32)gScrollX), (s32)(s->y - (f32)gScrollY), 0);
        (*po)->rot[2] = AngleToDeg((-a - 0x100) & 0x3FF);
        Obj_SetScale1(*po, seg->scale);
        a += step;
        HitPlayer(0, (s32)s->x, (s32)s->y, (s32)(seg->scale * 8.0f));
        HitPlayer(1, (s32)s->x, (s32)s->y, (s32)(seg->scale * 8.0f));
    }
    return a;
}

/* @ 0x800EE7FC */
/* Snake fire attack pose: curl the body; once countdown reaches 0 the tail
 * spits fire every frame (with a sound on frames that are not multiples of 8). */
void Snake_CurlFire(Mover *seg, Obj **objs, AnimFrame **anims, s32 step, s32 countdown, s32 frame)
{
    s32 a = Snake_CurlBody(seg, objs, anims, step);
    if (countdown == 0) {
        ovl2_func_800F32FC((s32)seg[7].x, (s32)seg[7].y, a - step);
        if (frame & 7)
            func_8001BA90(0x12);
    }
}

/* @ 0x800EEB78 */
/* Snake beam attack pose: curl the body; while *fire is set, the tail spawns a
 * beam at player 'target' (the beam sets *done when finished). */
void Snake_CurlBeam(Mover *seg, Obj **objs, AnimFrame **anims, s32 step, s32 *fire, s32 *done, s32 target)
{
    s32 a = Snake_CurlBody(seg, objs, anims, step);
    if (*fire != 0)
        ovl2_func_800F3B2C((s32)seg[7].x, (s32)seg[7].y, target, a - step, done);
}

/* @ 0x800EFCF0 */
/* Spinning bouncer (unless paused). mode 0: move and bounce off claimed cells
 * (bounces set the heading from the new velocity) and spin faster or slower by
 * the direction of travel; mode 1: hold still, let the spin run down and the
 * sprite turn back upright; mode 2: hold still and spin up. Every 30..59
 * frames the heading turns by +-90 degrees. Always places obj and publishes
 * the position for the camera. */
void Spinner_Move(Spinner *s, Obj *obj, AnimFrame **anims, s32 *seed, s32 mode)
{
    (void)anims;
    if (gPaused == 0) {
        s->vx = cosf(((f32)s->angle / 512.0f) * PI_F) * s->speed;
        s->vy = sinf(((f32)s->angle / 512.0f) * PI_F) * s->speed;
        if (mode == 0) {
            s->x = s->x + s->vx;
            if (ovl2_func_800E3C64(gArea, (s32)s->x, (s32)s->y)) {
                f32 nx = s->x - s->vx;
                s->vx = -s->vx;
                s->x = nx;
                s->angle = Math_VecToAngle(s->vx, s->vy);
            }
            s->y = s->y + s->vy;
            if (ovl2_func_800E3C64(gArea, (s32)s->x, (s32)s->y)) {
                f32 ny = s->y - s->vy;
                s->vy = -s->vy;
                s->y = ny;
                s->angle = Math_VecToAngle(s->vx, s->vy);
            }
        }
        switch (mode) {
        case 0:
            if (s->vx < 0.0f) {
                s->spinSpeed += 2;
                if (s->spinSpeed > 0x40)
                    s->spinSpeed = 0x40;
            } else {
                s->spinSpeed -= 2;
                if (s->spinSpeed < -0x40)
                    s->spinSpeed = -0x40;
            }
            if (s->vy < 0.0f) {
                s->spinSpeed += 2;
                if (s->spinSpeed > 0x40)
                    s->spinSpeed = 0x40;
            } else {
                s->spinSpeed -= 2;
                if (s->spinSpeed < -0x40)
                    s->spinSpeed = -0x40;
            }
            break;
        case 1:
            if (s->spinSpeed != 0)
                s->spinSpeed = (s->spinSpeed >= 0) ? s->spinSpeed - 1 : s->spinSpeed + 1;
            if (s->spin > 0x200) {
                s->spin += 8;
                if (s->spin > 0x400)
                    s->spin = 0;
            } else {
                s->spin -= 8;
                if (s->spin < 0)
                    s->spin = 0;
            }
            break;
        case 2:
            if (s->spinSpeed >= 0) {
                s->spinSpeed += 1;
                if (s->spinSpeed > 0x40)
                    s->spinSpeed = 0x40;
            } else {
                s->spinSpeed -= 1;
                if (s->spinSpeed < -0x40)
                    s->spinSpeed = -0x40;
            }
            break;
        }
        s->spin = (s->spin + s->spinSpeed) & 0x3FF;
        obj->rot[2] = AngleToDeg((~s->spin - 0x100) & 0x3FF);
        if (s->timer == 0) {
            s->timer = Rand_Range(seed, 30) + 30;
            if (Rand_Range(seed, 2))
                s->angle = (s->angle + 0x100) & 0x3FF;
            else
                s->angle = (s->angle - 0x100) & 0x3FF;
        } else {
            s->timer--;
        }
    }
    Obj_SetPos(obj, (s32)(s->x - (f32)gScrollX), (s32)(s->y - (f32)gScrollY), 0);
    ovl2_D_800FD2C4 = (s16)(s32)s->x;
    ovl2_D_800FD2B4 = (s16)(s32)s->y;
}

/* Advance an animation by one frame: when the current frame's time is up,
 * step to the next entry of seq (a negative duration means "end": the index
 * goes back to 0 and the function returns 1 without showing anything). */
static s32 Anim_StepOnce(Obj *obj, AnimFrame *seq, s32 *idx, s32 *timer)
{
    if (*timer == 0) {
        AnimFrame *f;
        (*idx)++;
        f = &seq[*idx];
        *timer = f->duration;
        if (*timer < 0) {
            *idx = 0;
            return 1;
        }
        Obj_SetData(obj, *f->frame);
    } else {
        (*timer)--;
    }
    return 0;
}

/* Looping version: after the end marker it starts again at entry 0. Note that
 * the sequence pointer is re-read through anims[seq] (callers pass the slot). */
static void Anim_StepLoop(Obj *obj, AnimFrame **slot, s32 *idx, s32 *timer, void ***frame)
{
    if (*timer == 0) {
        (*idx)++;
        for (;;) {
            AnimFrame *f = &(*slot)[*idx];
            *frame = f->frame;
            *timer = f->duration;
            if (*timer >= 0)
                break;
            *idx = 0;
        }
        Obj_SetData(obj, **frame);
    } else {
        (*timer)--;
    }
}

/* @ 0x800EAD80 */
/* Minion task. Forever: wait 10..19 seconds, appear at a random free spot of
 * the field (anims[0], sound 0xD), wander (anims[1]) at a level-dependent speed
 * in one of 3 directions, bouncing off claimed cells (snapping to the cell
 * edge) and turning by +-90 degrees every 30..59 frames; it dies (sound 0x11,
 * anims[2], then hidden) when it touches a player / line (setting the player's
 * hit flag) or ends up inside claimed area. The random seed starts as the
 * object's address. Returns (hiding the object) when the stage ends. */
void Minion_Task(s32 unused, Obj *obj, AnimFrame **anims)
{
    s32 seed = (s32)obj;
    f32 x = 0.0f, y = 0.0f, vx = 0.0f, vy = 0.0f, speed = 0.0f;
    s32 angle = 0, turn = 0, idx = 0, timer = 0;
    void **frame = NULL;
    (void)unused;

    for (;;) {
        s32 n, w, h, px, py, tries;

        for (n = (Rand_Range(&seed, 10) + 10) * 60; n != 0; n--) {
            func_8000AEAC();
            if (gStageOver)
                goto quit;
        }
        /* find a free spot */
        w = ovl2_func_800E29C4();
        h = ovl2_func_800E29D0();
        tries = 1;
        for (;;) {
            px = Rand_Range(&seed, w - 0x40) + 0x20;
            py = Rand_Range(&seed, h - 0x40) + 0x20;
            if (ovl2_func_800E2E24(px, py) == 0)
                break;
            if ((tries++ & 0x3F) == 0) {
                func_8000AEAC();
                if (gStageOver)
                    goto quit;
            }
        }
        x = (f32)px;
        y = (f32)py;
        angle = Rand_Range(&seed, 3) << 8;
        speed = RandSpeed(&seed, ovl2_D_800F9B40[D_800B4AA0]);
        turn = 0;
        func_8001BA90(0xD);

        /* appear */
        idx = 0;
        timer = 0;
        for (;;) {
            func_8000AEAC();
            if (gStageOver)
                goto quit;
            if (Anim_StepOnce(obj, anims[0], &idx, &timer))
                break;
            Obj_SetPos(obj, (s32)(x - (f32)gScrollX), (s32)(y - (f32)gScrollY), 0);
        }

        /* wander */
        timer = 0;
        idx = 0;
        for (;;) {
            func_8000AEAC();
            if (gStageOver)
                goto quit;
            Anim_StepLoop(obj, &anims[1], &idx, &timer, &frame);
            if (gPaused == 0) {
                vx = cosf(((f32)angle / 512.0f) * PI_F) * speed;
                vy = sinf(((f32)angle / 512.0f) * PI_F) * speed;
                x = x + vx;
                if (ovl2_func_800E3C64(gArea, (s32)x, (s32)y)) {
                    s32 c = (s32)x;
                    c = (0.0f < vx) ? (c & ~7) - 1 : (c + 7) & ~7;
                    x = (f32)c;
                    vx = -vx;
                    angle = Math_VecToAngle(vx, vy);
                }
                y = y + vy;
                if (ovl2_func_800E3C64(gArea, (s32)x, (s32)y)) {
                    s32 c = (s32)y;
                    c = (0.0f < vy) ? (c & ~7) - 1 : (c + 7) & ~7;
                    y = (f32)c;
                    vy = -vy;
                    angle = Math_VecToAngle(vx, vy);
                }
                obj->rot[2] = AngleToDeg(-angle & 0x3FF);
                if (turn == 0) {
                    turn = Rand_Range(&seed, 30) + 30;
                    if (Rand_Range(&seed, 2))
                        angle = (angle + 0x100) & 0x3FF;
                    else
                        angle = (angle - 0x100) & 0x3FF;
                } else {
                    turn--;
                }
            }
            Obj_SetPos(obj, (s32)(x - (f32)gScrollX), (s32)(y - (f32)gScrollY), 0);
            if (gPlayers[0]->hit == 0 && ovl2_func_800E3D28(gPlayers[0], (s32)x, (s32)y, 16, 16)) {
                gPlayers[0]->hit = 1;
                break;
            }
            if (gPlayers[1]->hit == 0 && ovl2_func_800E3D28(gPlayers[1], (s32)x, (s32)y, 16, 16)) {
                gPlayers[1]->hit = 1;
                break;
            }
            if (ovl2_func_800E3C64(gArea, (s32)x, (s32)y))
                break;
        }

        /* die */
        func_8001BA90(0x11);
        idx = 0;
        timer = 0;
        for (;;) {
            func_8000AEAC();
            if (gStageOver)
                goto quit;
            if (Anim_StepOnce(obj, anims[2], &idx, &timer))
                break;
            Obj_SetPos(obj, (s32)(x - (f32)gScrollX), (s32)(y - (f32)gScrollY), 0);
        }
        Obj_Hide(obj);
    }
quit:
    Obj_Hide(obj);
}

/* If (x, y) is inside claimed area, nudge it by 1 pixel towards the first of
 * the 4 neighbours (8 pixels away, in table order) that is free. */
static void Enemy_Unstick(f32 *x, f32 *y, s32 (*tbl)[2])
{
    s32 xi = (s32)*x;
    s32 yi = (s32)*y;
    s32 i;
    if (ovl2_func_800E3C64(gArea, xi, yi)) {
        for (i = 0; i < 4; i++) {
            if (ovl2_func_800E3C64(gArea, xi + tbl[i][0], yi + tbl[i][1]) == 0) {
                *x = *x + (f32)(tbl[i][0] / 8);
                *y = *y + (f32)(tbl[i][1] / 8);
                break;
            }
        }
    }
}

/* Per-frame start of every Snake_Task state: animate the head (objs[7]) with
 * anims[seq], apply the scale and get the head out of claimed area. */
static void Snake_HeadFrame(Mover *seg, Obj **objs, AnimFrame **anims, s32 seq)
{
    Obj *head = objs[7];
    Anim_StepLoop(head, &anims[seq], &seg->animIdx, &seg->animTimer, &seg->frame);
    Obj_SetScale1(head, seg->scale);
    Enemy_Unstick(&seg->x, &seg->y, ovl2_D_800F9CE0);
}

static void Snake_HitHead(Mover *seg)
{
    HitPlayer(0, (s32)seg->x, (s32)seg->y, (s32)(seg->scale * 32.0f));
    HitPlayer(1, (s32)seg->x, (s32)seg->y, (s32)(seg->scale * 32.0f));
}

/* Pick a player that is playing: random one, else the other, else -1. */
static s32 PickPlayer(s32 *seed)
{
    s32 t = Rand_Range(seed, 2);
    if (D_800B04D4[t] == 0) {
        t ^= 1;
        if (D_800B04D4[t] == 0)
            t = -1;
    }
    return t;
}

/* @ 0x800EEEEC */
/* Snake boss task. objs[7] is the head, objs[6..0] the body segments; all 24
 * objects are hidden when the stage ends. The snake roams (Snake_MoveHead +
 * Snake_FollowBody) and slowly shrinks (x0.9997 per frame, down to 0.167),
 * now and then (1 in 200 frames) dropping a small enemy. At a level-dependent
 * random rate it starts an effect task and attacks:
 *  - fire: the body curls back and forth while the head keeps moving; after
 *    120 frames the tail spits fire for 120 more frames;
 *  - beam: the head stops, the body winds up, then the tail fires a beam at a
 *    playing player and the snake waits until the beam reports it is done.
 * Pausing the game cancels a fire attack.
 * Quirks kept: the random start heading of every segment ends up in seg[0]
 * (the last one wins), and the original passes an uninitialised stack word as
 * the beam target while waiting for the beam (unused there, since the fire
 * flag is 0); this version passes the last picked target. */
void Snake_Task(s32 unused, Obj **objs, s32 x, s32 y, AnimFrame **anims)
{
    Mover seg[8];
    s32 seed = (s32)objs;
    s32 i, step, dir, countdown, frame, fire, done, target = -1;
    (void)unused;

    bzero(seg, sizeof(seg));
    for (i = 0; i < 8; i++) {
        s32 r;
        seg[i].scale = 1.0f;
        seg[i].x = (f32)x;
        seg[i].y = (f32)y;
        r = Rand_Range(&seed, 0x400);
        seg[0].angle = r;
        seg[i].turnTo = r;
        seg[i].speed = RandSpeed(&seed, ovl2_D_800F9C60[D_800B4AA0]);
        seg[i].timer = 0;
        seg[i].animIdx = 0;
        seg[i].animTimer = 0;
    }

roam:
    for (;;) {
        func_8000AEAC();
        if (gStageOver)
            goto quit;
        Snake_HeadFrame(seg, objs, anims, 0);
        Snake_MoveHead(&seg[0], objs[7], anims, &seed, 0);
        Snake_FollowBody(seg, objs, anims);
        Snake_HitHead(seg);
        if (gPaused)
            continue;
        if (Rand_Range(&seed, ovl2_D_800F9C20[D_800B4AA0]) == 0) {
            func_8000A76C(ovl2_func_800F4100, 0x19, 8, NULL, ovl2_D_800FD4F0);
            if (Rand_Range(&seed, 2) == 0)
                goto beam;
            goto fire;
        }
        if (Rand_Range(&seed, 200) == 0) {
            func_8001BA90(0x13);
            ovl2_func_800F4060((s32)seg[0].x, (s32)seg[0].y, seg[0].angle);
        }
        seg[0].scale = seg[0].scale * 0.9997f;
        if ((f64)seg[0].scale <= 0.167)
            seg[0].scale = 0.167f;
    }

fire:
    countdown = 120;
    step = 0;
    dir = 1;
    frame = 0;
    for (;;) {
        func_8000AEAC();
        if (gStageOver)
            goto quit;
        if (gPaused)
            goto roam;
        if (countdown == 0) {
            frame++;
            if (frame > 120)
                goto roam;
        }
        countdown -= (countdown != 0);
        Snake_HeadFrame(seg, objs, anims, 8);
        Snake_MoveHead(&seg[0], objs[7], anims, &seed, 0);
        Snake_CurlFire(seg, objs, anims, step, countdown, frame);
        if (dir == 0) {
            step -= 8;
            if (step < -0x40) {
                step = -0x40;
                dir = 1;
            }
        } else {
            step += 8;
            if (step > 0x40) {
                step = 0x40;
                dir ^= 1;
            }
        }
        Snake_HitHead(seg);
    }

beam:
    step = 0;
    dir = 1;
    done = 0;
    fire = 0;
    /* wind up until the beam fires */
    for (;;) {
        func_8000AEAC();
        if (gStageOver)
            goto quit;
        Snake_HeadFrame(seg, objs, anims, 8);
        Snake_MoveHead(&seg[0], objs[7], anims, &seed, 1);
        if (dir == 0) {
            step -= 8;
            if (step < -0x40) {
                step = -0x40;
                fire = 1;
                func_8001BA90(0x14);
            }
        } else {
            step += 1;
            if (step > 0x40) {
                step = 0x40;
                dir ^= 1;
            }
        }
        target = PickPlayer(&seed);
        Snake_CurlBeam(seg, objs, anims, step, &fire, &done, target);
        Snake_HitHead(seg);
        if (fire != 0)
            break;
    }
    /* hold still until the beam is done */
    done = 0;
    fire = 0;
    for (;;) {
        func_8000AEAC();
        if (gStageOver)
            goto quit;
        Snake_HeadFrame(seg, objs, anims, 8);
        Snake_MoveHead(&seg[0], objs[7], anims, &seed, 1);
        Snake_CurlBeam(seg, objs, anims, step, &fire, &done, target);
        if (done) {
            func_8001BA90(0x15);
            goto roam;
        }
        Snake_HitHead(seg);
    }

quit:
    for (i = 0; i < 24; i++)
        Obj_Hide(objs[i]);
}

/* ---- Boss_Task helpers ---- */

/* Animate the body (objs[23]) with anims[seq] and apply the body scale. */
static void Boss_Animate(BossBody *b, Obj **objs, AnimFrame **anims, s32 seq)
{
    Anim_StepLoop(objs[23], &anims[seq], &b->animIdx, &b->animTimer, &b->frame);
    Obj_SetScale1(objs[23], b->scale);
}

/* Attack movement: bounce off claimed cells without picking new speeds, then
 * slow down to 95% (unless paused; the caller checks). */
static void Boss_MoveDamped(BossBody *b)
{
    f32 vx, vy;
    b->x = b->x + b->vx;
    if (ovl2_func_800E3C64(gArea, (s32)b->x, (s32)b->y)) {
        f32 nx = b->x - b->vx;
        b->vx = -b->vx;
        b->x = nx;
    }
    b->y = b->y + b->vy;
    if (ovl2_func_800E3C64(gArea, (s32)b->x, (s32)b->y)) {
        f32 ny = b->y - b->vy;
        b->vy = -b->vy;
        b->y = ny;
    }
    vx = b->vx * 0.95f;
    vy = b->vy * 0.95f;
    ovl2_D_800FD2C4 = (s16)(s32)b->x;
    ovl2_D_800FD2B4 = (s16)(s32)b->y;
    b->vx = vx;
    b->vy = vy;
}

/* Place the body object. Quirk kept: during attacks the original stores the
 * raw spin value (not converted to degrees) as the rotation. */
static void Boss_PlaceBody(BossBody *b, Obj **objs, f32 rot)
{
    objs[23]->rot[2] = rot;
    Obj_SetPos(objs[23], (s32)(b->x - (f32)gScrollX), (s32)(b->y - (f32)gScrollY), 0);
}

/* Update all four arms (objs[0..19], at 90 degree steps from 0x80). */
static void Boss_Arms(BossBody *b, Obj **objs, BossArm *arms, s32 *curl, AnimFrame **anims,
                      s32 *seed, s32 aim, s32 animBase)
{
    s32 k;
    for (k = 0; k < 4; k++) {
        if (aim)
            Boss_UpdateArmAim(b, 0x80 + 0x100 * k, objs + 5 * k, &arms[k], &curl[k], anims, seed, animBase);
        else
            Boss_UpdateArm(b, 0x80 + 0x100 * k, objs + 5 * k, &arms[k], &curl[k], anims, seed, animBase);
    }
}

static void Boss_HitBody(BossBody *b)
{
    HitPlayer(0, (s32)b->x, (s32)b->y, (s32)(b->scale * 50.0f));
    HitPlayer(1, (s32)b->x, (s32)b->y, (s32)(b->scale * 50.0f));
}

/* One frame of an attack's moving phase (anims[3], arms curled). */
static void Boss_AttackFrame(BossBody *b, Obj **objs, BossArm *arms, s32 *curl, AnimFrame **anims,
                             s32 *seed, s32 aim)
{
    Boss_Animate(b, objs, anims, 3);
    Enemy_Unstick(&b->x, &b->y, ovl2_D_800F9C00);
    if (gPaused == 0)
        Boss_MoveDamped(b);
    Boss_PlaceBody(b, objs, (f32)b->spin);
    Boss_Arms(b, objs, arms, curl, anims, seed, aim, 3);
    Boss_HitBody(b);
}

/* One frame of an attack's standing phase (no unsticking, no movement). */
static void Boss_StandFrame(BossBody *b, Obj **objs, BossArm *arms, s32 *curl, AnimFrame **anims,
                            s32 *seed, s32 aim)
{
    Boss_Animate(b, objs, anims, 3);
    Boss_PlaceBody(b, objs, (f32)b->spin);
    Boss_Arms(b, objs, arms, curl, anims, seed, aim, 3);
    Boss_HitBody(b);
}

/* @ 0x800EC0CC */
/* Boss task. objs[23] is the body; objs[5k..5k+4] are arm k's four segments
 * and tip; all 24 objects are hidden when the stage ends. The random seed
 * starts as the objs address + 1.
 * Idle: the body drifts (Boss_MoveBounce), turns slowly and shrinks
 * (x0.9997 per frame, down to 0.125) while the arms wiggle. At a
 * level-dependent rate it starts an effect task and attacks for about 9
 * seconds (3 x 181 frames), then drifts on with a new random speed:
 *  - A: arms pull in while the body coasts to a stop, then it fires 8 shots in
 *    a ring at a playing player, then the arms stretch back out;
 *  - B: the arms reach for a playing player and every 16 frames each arm tip
 *    fires a shot (sound 0xE plays meanwhile; pausing cuts this part short).
 * Quirk kept: during attacks the body rotation is set to the raw spin value
 * instead of degrees; the aiming arms index the player array with the target
 * even when it is -1 (as left by an earlier attack B with no players). */
void Boss_Task(s32 unused, Obj **objs, s32 x, s32 y, AnimFrame **anims)
{
    BossBody b;
    BossArm arms[4];
    s32 curl[4];
    s32 seed = (s32)objs + 1;
    s32 i, n;
    f32 nvx, nvy;
    s16 snd;
    (void)unused;

    bzero(&b, sizeof(b));
    for (i = 0; i < 7; i++) {
        ((f32 *)&arms[0])[i] = 0.0f;
        ((f32 *)&arms[1])[i] = 0.0f;
        ((f32 *)&arms[2])[i] = 0.0f;
        ((f32 *)&arms[3])[i] = 0.0f;
    }
    b.x = (f32)x;
    curl[3] = 0;
    curl[2] = 0;
    curl[1] = 0;
    curl[0] = 0;
    b.spin = 0;
    b.y = (f32)y;
    b.reach = 1.0f;
    b.scale = 1.0f;
    b.vx = RandSpeed10(&seed);
    b.vy = RandSpeed10(&seed);
    b.animIdx = 0;
    b.frame = anims[0][0].frame;
    b.animTimer = anims[0][0].duration;

idle:
    for (;;) {
        func_8000AEAC();
        if (gStageOver)
            goto quit;
        Boss_Animate(&b, objs, anims, 0);
        Enemy_Unstick(&b.x, &b.y, ovl2_D_800F9C00);
        Boss_MoveBounce(&b, &seed);
        Boss_PlaceBody(&b, objs, AngleToDeg(b.spin));
        b.spin = (b.spin - 1) & 0x3FF;
        Boss_Arms(&b, objs, arms, curl, anims, &seed, 0, 0);
        Boss_HitBody(&b);
        if (gPaused)
            continue;
        if (Rand_Range(&seed, ovl2_D_800F9BC0[D_800B4AA0]) != 0) {
            b.scale = b.scale * 0.9997f;
            if ((f64)b.scale <= 0.125)
                b.scale = 0.125f;
            continue;
        }
        func_8000A76C(ovl2_func_800F4100, 0x19, 8, NULL, ovl2_D_800FD4F0);
        n = 0;
        if (Rand_Range(&seed, 2) != 0)
            goto attackB;
        break;
    }

    /* attack A: pull the arms in while coasting to a stop */
    for (;;) {
        n++;
        func_8000AEAC();
        if (gStageOver)
            goto quit;
        Boss_AttackFrame(&b, objs, arms, curl, anims, &seed, 0);
        b.reach = (f32)((f64)b.reach * 0.9);
        if ((f64)b.reach < 0.1)
            b.reach = 0.1f;
        if (n >= 181)
            break;
    }
    b.vx = 0.0f;
    b.vy = 0.0f;
    /* one frame: fire a ring of 8 shots */
    func_8000AEAC();
    if (gStageOver)
        goto quit;
    Boss_StandFrame(&b, objs, arms, curl, anims, &seed, 0);
    {
        s32 t = PickPlayer(&seed);
        if (t >= 0 && gPaused == 0) {
            f32 a = 0.0f;
            func_8001BA90(0x10);
            do {
                s32 sx = (s32)(b.x + cosf(a) * 25.0f * b.scale);
                s32 sy = (s32)(b.y + sinf(a) * 25.0f * b.scale);
                s32 ang = (s32)((a / PI_F) * 512.0f);
                a += 0.7853982f;
                ovl2_func_800F2BB0(sx, sy, t, ang);
            } while (a < 6.2831855f);
        }
    }
    nvx = RandSpeed10(&seed);
    nvy = RandSpeed10(&seed);
    /* stretch the arms out again and speed up */
    n = 0;
    for (;;) {
        n++;
        func_8000AEAC();
        if (gStageOver)
            goto quit;
        Boss_AttackFrame(&b, objs, arms, curl, anims, &seed, 0);
        b.reach = b.reach + 0.005f;
        if (1.0f < b.reach)
            b.reach = 1.0f;
        {
            f32 ex = (nvx - b.vx) / 64.0f;
            f32 ey = (nvy - b.vy) / 64.0f;
            b.vx = b.vx + ex;
            b.vy = b.vy + ey;
        }
        if (n >= 181)
            break;
    }
    b.vx = nvx;
    b.vy = nvy;
    b.reach = 1.0f;
    goto idle;

attackB:
    /* reach for the target while coasting to a stop */
    for (;;) {
        n++;
        func_8000AEAC();
        if (gStageOver)
            goto quit;
        Boss_AttackFrame(&b, objs, arms, curl, anims, &seed, 1);
        if (n >= 181)
            break;
    }
    b.vx = 0.0f;
    b.vy = 0.0f;
    b.target = PickPlayer(&seed);
    snd = (s16)func_8001BA90(0xE);
    /* stand and shoot from the arm tips */
    n = 0;
    for (;;) {
        n++;
        func_8000AEAC();
        if (gStageOver)
            goto quit;
        if (b.target < 0)
            break;
        Boss_StandFrame(&b, objs, arms, curl, anims, &seed, 1);
        if (gPaused)
            break;
        if ((n & 0xF) == 0) {
            for (i = 0; i < 4; i++)
                ovl2_func_800F2F6C((s32)arms[i].tip10, (s32)arms[i].tip14, (s32)(-arms[i].tip18 + 256.0f));
        }
        if (n >= 181)
            break;
    }
    func_8001BB8C(snd);
    nvx = RandSpeed10(&seed);
    nvy = RandSpeed10(&seed);
    /* speed up again */
    n = 0;
    for (;;) {
        n++;
        func_8000AEAC();
        if (gStageOver)
            goto quit;
        Boss_AttackFrame(&b, objs, arms, curl, anims, &seed, 1);
        {
            f32 ex = (nvx - b.vx) / 64.0f;
            f32 ey = (nvy - b.vy) / 64.0f;
            b.vx = b.vx + ex;
            b.vy = b.vy + ey;
        }
        if (n >= 181)
            break;
    }
    b.vx = nvx;
    b.vy = nvy;
    b.reach = 1.0f;
    goto idle;

quit:
    for (i = 0; i < 24; i++)
        Obj_Hide(objs[i]);
}
