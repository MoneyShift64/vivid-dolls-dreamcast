/* Vivid Dolls - overlay 2 (the game), part D: 0x800F00F4 - end of ovl2 code.
 * See ovl2_d.h for an overview. Every function is checked against the original
 * by tools/difftest/test_ovl2_d.py. */
#include "ovl2_d.h"
#include "libm.h"
#include "vd_math.h"

void bzero(void *p, u32 n);
u32  Rand_Range(s32 *seed, s32 range);

/* sound (main code): play sound effect id -> voice handle; stop a voice */
extern s16  func_8001BA90(s32 id);
extern void func_8001BB8C(s16 voice);
/* task system: wait for the next frame (switches to the next task) */
extern void func_8000AEAC(void);

/* other parts of ovl2 */
extern s32 ovl2_func_800E29C4(void);                                  /* Board_GetWidth */
extern s32 ovl2_func_800E29D0(void);                                  /* Board_GetHeight */
extern u8  ovl2_func_800E2AE8(s32 x, s32 y);                          /* Board_TestBlock */
extern u8  ovl2_func_800E3D28(FxPlayer *p, s32 x, s32 y, s32 w, s32 h); /* Player_HitTest */
extern s32 ovl2_func_800E3E4C(FxPlayer *p, s32 x, s32 y, s32 w, s32 h); /* Player_InBox */

/* The update callbacks are stored by address (Obj.update); keep the original
 * addresses so memory matches. A port maps these to the C functions. */
extern void ovl2_func_800F1ED4(Obj *obj);   /* = Effect_UpdatePetal */
extern void ovl2_func_800F210C(Obj *obj);   /* = Effect_UpdateSmoke */
extern void ovl2_func_800F2268(Obj *obj);   /* = Effect_UpdateFlash */
extern void ovl2_func_800F23F4(Obj *obj);   /* = Effect_UpdateMoveTo */
extern void ovl2_func_800F265C(Obj *obj);   /* = Effect_UpdateExplosion */
extern void ovl2_func_800F2798(Obj *obj);   /* = Missile_Update */
extern void ovl2_func_800F2C44(Obj *obj);   /* = Bullet_UpdateA */
extern void ovl2_func_800F2FD4(Obj *obj);   /* = Bullet_UpdateB */
extern void ovl2_func_800F3364(Obj *obj);   /* = Bullet_UpdateFast */
extern void ovl2_func_800F36F4(Obj *obj);   /* = Boomerang_Update */
extern void ovl2_func_800F3C24(Obj *obj);   /* = Net_Update */
#define FN_PETAL     ovl2_func_800F1ED4
#define FN_SMOKE     ovl2_func_800F210C
#define FN_FLASH     ovl2_func_800F2268
#define FN_MOVETO    ovl2_func_800F23F4
#define FN_EXPLOSION ovl2_func_800F265C
#define FN_MISSILE   ovl2_func_800F2798
#define FN_BULLET_A  ovl2_func_800F2C44
#define FN_BULLET_B  ovl2_func_800F2FD4
#define FN_BULLET_F  ovl2_func_800F3364
#define FN_BOOMERANG ovl2_func_800F36F4
#define FN_NET       ovl2_func_800F3C24

/* ---- overlay data / BSS ---- */
extern Effect ovl2_D_800FBA90[EFFECT_COUNT];   /* effect slots */
extern Obj *ovl2_D_800FD2F8[EFFECT_COUNT];     /* one pre-made sprite object per slot */
extern s32 ovl2_D_800FD2B8;                    /* scroll x (subtracted from field x) */
extern s32 ovl2_D_800FD2C8;                    /* scroll y */
extern s32 ovl2_D_800FD2D8;                    /* round over: effects/tasks stop */
extern FxPlayer *ovl2_D_800FD3F8[2];         /* the two players */
extern s32 ovl2_D_800F9DE0;                    /* random seed */
extern s32 ovl2_D_800FD424;                    /* round timer, in frames */
extern Obj *ovl2_D_800FCEB0, *ovl2_D_800FCEB4, *ovl2_D_800FCEB8, *ovl2_D_800FCEBC; /* timer HUD */
extern void *ovl2_D_800FCF58[10];              /* small digit sprites */
extern void *ovl2_D_800FCF80[10];              /* big digit sprites */
extern void *ovl2_D_800FD44C, *ovl2_D_800FD450, *ovl2_D_800FD454; /* banner sprites */

/* animation tables */
extern EffFrame *ovl2_D_800F8F84[];   /* petals (4 colours) */
extern EffFrame *ovl2_D_800F8FC4[];   /* bullet A */
extern EffFrame *ovl2_D_800F8FD8[];   /* bullet B */
extern EffFrame *ovl2_D_800F8FEC[];   /* boomerang */
extern EffFrame *ovl2_D_800F9008[];   /* fast bullet */
extern EffFrame *ovl2_D_800F9084[];   /* net: flying, stuck... */
extern EffFrame *ovl2_D_800F90E0[];   /* missile ([0]) / explosion ([1]) */
extern EffFrame *ovl2_D_800F90F8[];   /* smoke */
extern EffFrame *ovl2_D_800F9124[];   /* flash */
extern EffFrame *ovl2_D_800F9140[];   /* move-to */

#define PI_F 3.1415927f
#define EFF(o) ((Effect *)(o)->update_arg)

/* Place the sprite at the effect's field position minus the scroll. */
static void eff_place_scrolled(Obj *obj, Effect *e)
{
    Obj_SetPos(obj, (s32)(e->x - (f32)ovl2_D_800FD2B8), (s32)(e->y - (f32)ovl2_D_800FD2C8), 0);
}

/* Free the slot and hide its object. */
static void eff_kill(Obj *obj, Effect *e)
{
    e->active = 0;
    Obj_SetUpdate(obj, NULL, NULL);
    Obj_Hide(obj);
}

/* Advance a looping animation (*pframes): a negative time restarts it. */
static void eff_anim_loop(Obj *obj, Effect *e, EffFrame **pframes)
{
    if (e->timer == 0) {
        for (;;) {
            EffFrame *f = *pframes;
            e->cur = f[e->frame].data;
            e->timer = f[e->frame].time;
            if (e->timer >= 0)
                break;
            e->frame = 0;
        }
        e->frame++;
        Obj_SetData(obj, *e->cur);
    } else {
        e->timer--;
    }
}

/* Advance an animation that stays on its last frame (negative time). */
static void eff_anim_hold(Obj *obj, Effect *e)
{
    if (e->timer == 0) {
        EffFrame *f = e->anims[0];
        e->cur = f[e->frame].data;
        e->timer = f[e->frame].time;
        if (e->timer >= 0)
            e->frame++;
        Obj_SetData(obj, *e->cur);
    } else {
        e->timer--;
    }
}

/* Rotation (degrees, rot z) for a sprite pointing along angle (0..0x3FF). */
static f32 angle_to_rot(s32 angle)
{
    return (f32)((~angle - 0x100) & 0x3FF) / 512.0f * 180.0f;
}

/* Move along e->angle by speed (x += cos * speed, y += sin * speed). */
static void eff_move(Effect *e, f32 speed)
{
    f32 c = cosf((f32)e->angle / 512.0f * PI_F);
    e->x = e->x + c * speed;
    e->y = e->y + sinf((f32)e->angle / 512.0f * PI_F) * speed;
}

/* Turn angle towards the direction (dx, dy) by 1/div of the difference. */
static s32 turn_towards(s32 angle, f32 dx, f32 dy, s32 shift16)
{
    s32 d = (Math_VecToAngle(dx, dy) - angle) & 0x3FF;
    if (d >= 0x200)
        d -= 0x400;
    return (angle + (shift16 ? d / 16 : d / 4)) & 0x3FF;
}

/* @ 0x800F1D40 */
/* Clear n bytes. */
void Game_MemClear(u8 *p, u32 n)
{
    while (n-- != 0)
        *p++ = 0;
}

/* @ 0x800F1D6C */
/* Mark every effect slot free (only the 'active' words are touched). */
void Effect_ClearAll(void)
{
    s32 i;
    for (i = EFFECT_COUNT - 1; i >= 0; i--)
        ovl2_D_800FBA90[i].active = 0;
}

/* @ 0x800F1D90 */
/* Take a free effect slot: its object gets update callback fn (with the slot as
 * argument) and scale 1; the slot is cleared, then position (x, y), scale 1 and
 * the animation table are set. Returns the slot index, or -1 if all are busy. */
s32 Effect_Spawn(ObjFunc fn, EffFrame **anims, s32 x, s32 y)
{
    s32 i;
    for (i = 0; i < EFFECT_COUNT; i++) {
        Effect *e = &ovl2_D_800FBA90[i];
        if (e->active == 0) {
            Obj *obj;
            Obj_SetUpdate(ovl2_D_800FD2F8[i], fn, e);
            obj = ovl2_D_800FD2F8[i];
            obj->scale[2] = 1.0f;
            obj->scale[1] = 1.0f;
            obj->scale[0] = 1.0f;
            bzero(e, sizeof(Effect));
            e->active = 1;
            e->x = (f32)x;
            e->scale = 1.0f;
            e->anims = anims;
            e->y = (f32)y;
            return i;
        }
    }
    return -1;
}

/* @ 0x800F1ED4 */
/* Falling petal: animates (animation e->anim), scales, falls by 3*scale per
 * frame (screen coordinates, no scroll) and disappears below y = 240. */
void Effect_UpdatePetal(Obj *obj)
{
    Effect *e = EFF(obj);
    f32 s;

    eff_anim_loop(obj, e, &e->anims[e->anim]);
    s = e->scale;
    obj->scale[2] = s;
    obj->scale[1] = s;
    obj->scale[0] = s;
    Obj_SetPos(obj, (s32)e->x, (s32)e->y, 0);
    e->y = e->y + e->scale * 3.0f;
    if (240.0f < e->y) {
        e->active = 0;
        Obj_SetUpdate(obj, NULL, NULL);
        Obj_Hide(obj);
    }
}

/* @ 0x800F200C */
/* New petal at a random x (0..316.8) at the top, colour = slot & 3,
 * scale 1.00..1.99. */
void Effect_SpawnPetal(void)
{
    s32 i = Effect_Spawn(FN_PETAL, ovl2_D_800F8F84, 0, 0);
    if (i >= 0) {
        Effect *e = &ovl2_D_800FBA90[i];
        f32 x;
        e->y = 0;   /* stored as an integer 0 (same bits as 0.0f) */
        x = (f32)(s32)Rand_Range(&ovl2_D_800F9DE0, 100) / 100.0f * 320.0f;
        e->anim = i & 3;
        e->x = x;
        e->scale = (f32)(s32)Rand_Range(&ovl2_D_800F9DE0, 100) / 100.0f + 1.0f;
    }
}

/* @ 0x800F210C */
/* Smoke puff: shows frame e->frame of animation 0 (never advances), shrinks by
 * 0.7 per frame and disappears below 0.1. */
void Effect_UpdateSmoke(Obj *obj)
{
    Effect *e = EFF(obj);
    f32 s;

    e->cur = e->anims[0][e->frame].data;
    Obj_SetData(obj, *e->cur);
    s = e->scale;
    obj->scale[2] = s;
    obj->scale[1] = s;
    obj->scale[0] = s;
    eff_place_scrolled(obj, e);
    e->scale = e->scale * 0.7f;
    if ((f64)e->scale < 0.1)
        eff_kill(obj, e);
}

/* @ 0x800F2208 */
/* New smoke puff at (x, y), scale 0.25. */
void Effect_SpawnSmoke(s32 x, s32 y)
{
    s32 i = Effect_Spawn(FN_SMOKE, ovl2_D_800F90F8, x, y);
    if (i >= 0)
        ovl2_D_800FBA90[i].scale = 0.25f;
}

/* @ 0x800F2268 */
/* Flash: like the smoke puff but drawn with the alternative sprite set-up,
 * colour alpha = scale * 255 (screen coordinates), fades by 0.9 per frame and
 * disappears below 0.2 (switching the set-up back). */
void Effect_UpdateFlash(Obj *obj)
{
    Effect *e = EFF(obj);
    f32 s;

    Obj_UseSetupCC0(obj);
    e->cur = e->anims[0][e->frame].data;
    Obj_SetData(obj, *e->cur);
    s = e->scale;
    obj->scale[2] = s;
    obj->scale[1] = s;
    obj->scale[0] = s;
    Obj_SetPos(obj, (s32)e->x, (s32)e->y, 0);
    Obj_SetColor(obj, 0, 0, 0, (s32)(e->scale * 255.0f));
    e->scale = e->scale * 0.9f;
    if ((f64)e->scale < 0.2) {
        e->active = 0;
        Obj_SetUpdate(obj, NULL, NULL);
        Obj_UseSetupC80(obj);
        Obj_Hide(obj);
    }
}

/* @ 0x800F2384 */
/* New flash at (x, y) (screen coordinates), frame = slot & 3. */
void Effect_SpawnFlash(s32 x, s32 y)
{
    s32 i = Effect_Spawn(FN_FLASH, ovl2_D_800F9124, x, y);
    if (i >= 0) {
        ovl2_D_800FBA90[i].frame = i & 3;
        ovl2_D_800FBA90[i].scale = 1.0f;
    }
}

/* @ 0x800F23F4 */
/* Moves 1/16 of the way to (tx, ty) each frame (screen coordinates); freed
 * once within 1 pixel on both axes (it still moves that frame). */
void Effect_UpdateMoveTo(Obj *obj)
{
    Effect *e = EFF(obj);
    f32 d, x, y;

    eff_anim_loop(obj, e, &e->anims[0]);
    d = e->tx - e->x;
    if (d < 0.0f)
        d = -d;
    if (d < 1.0f) {
        d = e->ty - e->y;
        if (d < 0.0f)
            d = -d;
        if (d < 1.0f)
            eff_kill(obj, e);
    }
    x = e->x;
    y = e->y;
    {
        f32 dx = (e->tx - x) / 16.0f;
        f32 dy = (e->ty - y) / 16.0f;
        e->x = x + dx;
        y = y + dy;
    }
    e->y = y;
    Obj_SetPos(obj, (s32)e->x, (s32)y, 0);
}

/* @ 0x800F2580 */
/* New move-to effect from (x, y) to (tx, ty). */
void Effect_SpawnMoveTo(s32 x, s32 y, s32 tx, s32 ty)
{
    s32 i = Effect_Spawn(FN_MOVETO, ovl2_D_800F9140, x, y);
    if (i >= 0) {
        Effect *e = &ovl2_D_800FBA90[i];
        e->x = (f32)x;
        e->y = (f32)y;
        e->tx = (f32)tx;
        e->ty = (f32)ty;
    }
}

/* @ 0x800F265C */
/* Explosion: plays sound 0x11 every frame, runs animation 1 once and frees
 * the slot at its end. */
void Effect_UpdateExplosion(Obj *obj)
{
    Effect *e = EFF(obj);

    func_8001BA90(0x11);
    if (e->timer == 0) {
        EffFrame *f = e->anims[1];
        e->cur = f[e->frame].data;
        e->timer = f[e->frame].time;
        if (e->timer < 0) {
            e->frame = 0;
            eff_kill(obj, e);
            return;
        }
        e->frame++;
        Obj_SetData(obj, *e->cur);
    } else {
        e->timer--;
    }
    eff_place_scrolled(obj, e);
}

/* @ 0x800F2768 */
/* New explosion at (x, y). */
void Effect_SpawnExplosion(s32 x, s32 y)
{
    Effect_Spawn(FN_EXPLOSION, ovl2_D_800F90E0, x, y);
}

/* Projectile hit: mark the player, explode and free the slot. */
static void eff_hit(Obj *obj, Effect *e, FxPlayer *p)
{
    p->hit = 1;
    Effect_Spawn(FN_EXPLOSION, ovl2_D_800F90E0, (s32)e->x, (s32)e->y);
    eff_kill(obj, e);
}

/* @ 0x800F2798 */
/* Homing missile: turns 1/16 of the way towards its target player each frame,
 * flies at speed 4, explodes on hitting either player (8x8), leaves a smoke
 * puff behind every 4th frame and explodes when its life runs out or the round
 * is over. */
void Missile_Update(Obj *obj)
{
    Effect *e = EFF(obj);
    FxPlayer *t;
    s32 angle;

    eff_anim_loop(obj, e, &e->anims[0]);
    t = ovl2_D_800FD3F8[e->target];
    angle = turn_towards(e->angle, (f32)t->x - e->x, (f32)t->y - e->y, 1);
    e->angle = angle;
    obj->rot[2] = angle_to_rot(angle);
    eff_move(e, 4.0f);
    eff_place_scrolled(obj, e);

    if (ovl2_D_800FD3F8[0]->hit == 0
        && ovl2_func_800E3D28(ovl2_D_800FD3F8[0], (s32)e->x, (s32)e->y, 8, 8) != 0) {
        eff_hit(obj, e, ovl2_D_800FD3F8[0]);
        return;
    }
    if (ovl2_D_800FD3F8[1]->hit == 0
        && ovl2_func_800E3D28(ovl2_D_800FD3F8[1], (s32)e->x, (s32)e->y, 8, 8) != 0) {
        eff_hit(obj, e, ovl2_D_800FD3F8[1]);
        return;
    }
    if ((e->state & 3) == 0) {
        s32 sx = (s32)(e->x + sinf(obj->rot[2] / 180.0f * PI_F) * 10.0f);
        s32 sy = (s32)(e->y + cosf(obj->rot[2] / 180.0f * PI_F) * 10.0f);
        s32 i = Effect_Spawn(FN_SMOKE, ovl2_D_800F90F8, sx, sy);
        if (i >= 0)
            ovl2_D_800FBA90[i].scale = 0.25f;
    }
    if (e->state != 0 && ovl2_D_800FD2D8 == 0) {
        e->state--;
        return;
    }
    Effect_Spawn(FN_EXPLOSION, ovl2_D_800F90E0, (s32)e->x, (s32)e->y);
    eff_kill(obj, e);
}

/* @ 0x800F2BB0 */
/* New homing missile at (x, y) chasing player `target`, life 240 frames. */
void Missile_Spawn(s32 x, s32 y, s32 target, s32 angle)
{
    s32 i = Effect_Spawn(FN_MISSILE, ovl2_D_800F90E0, x, y);
    if (i >= 0) {
        Effect *e = &ovl2_D_800FBA90[i];
        e->state = 240;
        e->target = target;
        e->angle = angle;
    }
}

/* Straight bullet: flies at `speed`, freed when it leaves the field, explodes
 * on hitting either player (16x16). */
static void bullet_update(Obj *obj, f32 speed)
{
    Effect *e = EFF(obj);
    s32 w, h;

    eff_anim_hold(obj, e);
    obj->rot[2] = angle_to_rot(e->angle);
    eff_move(e, speed);
    w = ovl2_func_800E29C4();
    h = ovl2_func_800E29D0();
    if (e->x < 0.0f || (f32)w <= e->x || e->y < 0.0f || (f32)h <= e->y) {
        eff_kill(obj, e);
        return;
    }
    eff_place_scrolled(obj, e);
    if (ovl2_D_800FD3F8[0]->hit == 0
        && ovl2_func_800E3D28(ovl2_D_800FD3F8[0], (s32)e->x, (s32)e->y, 16, 16) != 0) {
        eff_hit(obj, e, ovl2_D_800FD3F8[0]);
        return;
    }
    if (ovl2_D_800FD3F8[1]->hit == 0
        && ovl2_func_800E3D28(ovl2_D_800FD3F8[1], (s32)e->x, (s32)e->y, 16, 16) != 0) {
        eff_hit(obj, e, ovl2_D_800FD3F8[1]);
    }
}

/* @ 0x800F2C44 */
/* Bullet (speed 4). */
void Bullet_UpdateA(Obj *obj)
{
    bullet_update(obj, 4.0f);
}

/* @ 0x800F2F6C */
/* New bullet A at (x, y) flying along angle. */
void Bullet_SpawnA(s32 x, s32 y, s32 angle)
{
    s32 i = Effect_Spawn(FN_BULLET_A, ovl2_D_800F8FC4, x, y);
    if (i >= 0)
        ovl2_D_800FBA90[i].angle = angle;
}

/* @ 0x800F2FD4 */
/* Bullet B (speed 4, other sprite). */
void Bullet_UpdateB(Obj *obj)
{
    bullet_update(obj, 4.0f);
}

/* @ 0x800F32FC */
/* New bullet B at (x, y) flying along angle. */
void Bullet_SpawnB(s32 x, s32 y, s32 angle)
{
    s32 i = Effect_Spawn(FN_BULLET_B, ovl2_D_800F8FD8, x, y);
    if (i >= 0)
        ovl2_D_800FBA90[i].angle = angle;
}

/* @ 0x800F3364 */
/* Fast bullet (speed 8). */
void Bullet_UpdateFast(Obj *obj)
{
    bullet_update(obj, 8.0f);
}

/* @ 0x800F368C */
/* New fast bullet at (x, y) flying along angle. */
void Bullet_SpawnFast(s32 x, s32 y, s32 angle)
{
    s32 i = Effect_Spawn(FN_BULLET_F, ovl2_D_800F9008, x, y);
    if (i >= 0)
        ovl2_D_800FBA90[i].angle = angle;
}

/* @ 0x800F36F4 */
/* Boomerang: spins (32 degrees per frame) and flies at speed 4; while its
 * outbound time runs it homes in on the target player (turning 1/16), then on
 * its thrower's position (turning 1/4) until it is within the 16x16 box around
 * home (minus 8), which sets *done and frees it. Hits mark players (16x16)
 * but don't stop it; the end of the round frees it (it still runs
 * the rest of that frame). */
void Boomerang_Update(Obj *obj)
{
    Effect *e = EFF(obj);
    f32 dx, dy;

    eff_anim_loop(obj, e, &e->anims[0]);
    if (e->state != 0) {
        FxPlayer *t = ovl2_D_800FD3F8[e->target];
        dx = (f32)t->x - e->x;
        dy = (f32)t->y - e->y;
    } else {
        dx = e->homeX - e->x;
        dy = e->homeY - e->y;
    }
    e->angle = turn_towards(e->angle, dx, dy, e->state != 0);
    obj->rot[2] = (f32)(e->anim % 360);
    e->anim += 32;
    eff_move(e, 4.0f);
    eff_place_scrolled(obj, e);
    if (ovl2_D_800FD3F8[0]->hit == 0
        && ovl2_func_800E3D28(ovl2_D_800FD3F8[0], (s32)e->x, (s32)e->y, 16, 16) != 0)
        ovl2_D_800FD3F8[0]->hit = 1;
    if (ovl2_D_800FD3F8[1]->hit == 0
        && ovl2_func_800E3D28(ovl2_D_800FD3F8[1], (s32)e->x, (s32)e->y, 16, 16) != 0)
        ovl2_D_800FD3F8[1]->hit = 1;
    if (ovl2_D_800FD2D8 != 0)
        eff_kill(obj, e);
    if (e->state == 0) {
        f32 hx = e->homeX - e->x + 8.0f;
        f32 hy = e->homeY - e->y + 8.0f;
        if (0.0f <= hx && hx < 16.0f && 0.0f <= hy && hy < 16.0f) {
            e->active = 0;
            *e->done = 1;
            Obj_SetUpdate(obj, NULL, NULL);
            Obj_Hide(obj);
        }
        return;
    }
    e->state--;
}

/* @ 0x800F3B2C */
/* New boomerang thrown from (x, y) at player `target`; *done is set when it
 * returns. Outbound time 180 frames. */
void Boomerang_Spawn(s32 x, s32 y, s32 target, s32 angle, s32 *done)
{
    s32 i = Effect_Spawn(FN_BOOMERANG, ovl2_D_800F8FEC, x, y);
    if (i >= 0) {
        Effect *e = &ovl2_D_800FBA90[i];
        e->homeX = (f32)x;
        e->homeY = (f32)y;
        e->state = 180;
        e->target = target;
        e->angle = angle;
        e->anim = 0;
        e->done = done;
    }
}

/* Stuck net vs player n: a player that is not already hit and touches the net
 * (12x12) pulls it onto himself; once caught he is kept netted every frame. */
static void net_check(Effect *e, s32 n)
{
    FxPlayer *p = ovl2_D_800FD3F8[n];
    if (e->caught[n] == 0) {
        if (p->hit == 0 && ovl2_func_800E3E4C(p, (s32)e->x, (s32)e->y, 12, 12) != 0) {
            e->x = (f32)p->x;
            e->y = (f32)p->y;
            e->caught[n] = 1;
        }
    } else {
        PLAYER_NETTED(p) = 1;
    }
}

/* @ 0x800F3C24 */
/* Net: animation e->state (time -1 = go to the next animation, -2 = restart,
 * other negatives hold). Flies at speed 8 until it hits a wall, then stays
 * (state 1+) for `life` frames, blinking (hidden on odd frames - never shown
 * again by this code). A stuck net that touches a player (12x12, players not
 * already hit) moves onto that player; afterwards that player is kept netted
 * every frame. Freed off-field, at the end of its life or the round. */
void Net_Update(Obj *obj)
{
    Effect *e = EFF(obj);
    EffFrame *f = e->anims[e->state];
    s32 w, h;

    if (e->timer == 0) {
        for (;;) {
            e->cur = f[e->frame].data;
            e->timer = f[e->frame].time;
            if (e->timer >= 0) {
                e->frame++;
                break;
            }
            if (e->timer == -1) {
                e->frame = 0;
                e->timer = 0;
                e->state++;
                break;
            }
            if (e->timer != -2)
                break;
            e->frame = 0;
        }
    } else {
        e->timer--;
    }
    Obj_SetData(obj, *e->cur);

    if (ovl2_D_800FD2D8 != 0) {
        eff_kill(obj, e);
        return;
    }
    if (e->state != 0) {
        if (e->life != 0) {
            e->life--;
            if (e->life == 0) {
                eff_kill(obj, e);
                return;
            }
        }
    } else {
        obj->rot[2] = angle_to_rot(e->angle);
        eff_move(e, 8.0f);
        if (ovl2_func_800E2AE8((s32)e->x, (s32)e->y) != 0)
            e->state = 1;
    }
    if (e->life & 1)
        Obj_Hide(obj);
    w = ovl2_func_800E29C4();
    h = ovl2_func_800E29D0();
    if (e->x < 0.0f || (f32)w <= e->x || e->y < 0.0f || (f32)h <= e->y) {
        eff_kill(obj, e);
        return;
    }
    eff_place_scrolled(obj, e);
    if (e->state == 0)
        return;
    net_check(e, 0);
    net_check(e, 1);
}

/* @ 0x800F4060 */
/* New net at (x, y) flying along angle; stays 300 frames once stuck. */
void Net_Spawn(s32 x, s32 y, s32 angle)
{
    s32 i = Effect_Spawn(FN_NET, ovl2_D_800F9084, x, y);
    if (i >= 0) {
        Effect *e = &ovl2_D_800FBA90[i];
        e->angle = angle;
        e->state = 0;
        e->life = 300;
        e->caught[0] = 0;
        e->caught[1] = 0;
    }
}

/* @ 0x800F4100 */
/* Task: show banner objs[0] and objs[1] (sprites D_800FD44C / D_800FD450) at the
 * screen centre, blinking every 8 frames for 90 frames (or until the round is
 * over), with sound 6 playing meanwhile. */
void Task_BlinkBanner(void *task, Obj **objs)
{
    s16 voice = func_8001BA90(6);
    s32 i = 0;

    (void)task;
    Obj_SetData(objs[0], ovl2_D_800FD44C);
    Obj_SetData(objs[1], ovl2_D_800FD450);
    Obj_SetPos(objs[0], 160, 120, 0);
    Obj_SetPos(objs[1], 160, 120, 0);
    if (ovl2_D_800FD2D8 == 0) {
        do {
            func_8000AEAC();
            i++;
            if (i & 8) {
                Obj_Show(objs[0]);
                Obj_Show(objs[1]);
            } else {
                Obj_Hide(objs[0]);
                Obj_Hide(objs[1]);
            }
        } while (i < 91 && ovl2_D_800FD2D8 == 0);
    }
    Obj_Hide(objs[0]);
    Obj_Hide(objs[1]);
    func_8001BB8C(voice);
}

/* @ 0x800F4210 */
/* Task: banner objs[0] (sprite D_800FD454) slides in from x = 420 towards the
 * centre (1/8 of the distance per frame, 31 frames), waits 31 frames, then
 * slides off towards x = -160 (31 frames); sound 7 plays meanwhile. Stops
 * early when the round is over. */
void Task_SlideBanner(void *task, Obj **objs)
{
    s16 voice = func_8001BA90(7);
    f32 x = 420.0f;
    s32 y = 120;
    s32 ix = 0;
    s32 i;

    (void)task;
    Obj_SetData(objs[0], ovl2_D_800FD454);
    Obj_SetPos(objs[0], 420, 120, 0);
    i = 0;
    do {
        func_8000AEAC();
        i++;
        if (ovl2_D_800FD2D8 != 0)
            goto done;
        x = x + (160.0f - x) / 8.0f;
        ix = (s32)x;
        Obj_SetPos(objs[0], ix, y, 0);
    } while (i < 31);
    i = 0;
    do {
        func_8000AEAC();
        i++;
        if (ovl2_D_800FD2D8 != 0)
            goto done;
        Obj_SetPos(objs[0], ix, 120, 0);
    } while (i < 31);
    i = 0;
    do {
        func_8000AEAC();
        i++;
        if (ovl2_D_800FD2D8 != 0)
            goto done;
        x = x + (-160.0f - x) / 8.0f;
        Obj_SetPos(objs[0], (s32)x, 120, 0);
    } while (i < 31);
done:
    Obj_Hide(objs[0]);
    func_8001BB8C(voice);
}

/* @ 0x800F43E0 */
/* Count the round timer down (to 0) and show it: under 30 seconds as two big
 * digits (tens, ones), otherwise as three small digits plus the frame object
 * D_800FCEB0. */
void Hud_UpdateTimer2(void)
{
    s32 t, secs, tens10, ones, tens, hundreds;

    if (ovl2_D_800FD424 != 0)
        ovl2_D_800FD424--;
    t = ovl2_D_800FD424;
    secs = t / 60;
    tens10 = secs / 10;
    ones = secs - tens10 * 10;
    hundreds = tens10 / 10;
    tens = tens10 - hundreds * 10;
    if (t < 1800) {
        Obj_SetData(ovl2_D_800FCEB4, ovl2_D_800FCF80[tens]);
        Obj_SetPos(ovl2_D_800FCEB4, 154, 28, 0);
        Obj_SetData(ovl2_D_800FCEB8, ovl2_D_800FCF80[ones]);
        Obj_SetPos(ovl2_D_800FCEB8, 174, 28, 0);
        Obj_Hide(ovl2_D_800FCEB0);
        Obj_Hide(ovl2_D_800FCEBC);
    } else {
        Obj_SetData(ovl2_D_800FCEB4, ovl2_D_800FCF58[hundreds]);
        Obj_SetData(ovl2_D_800FCEB8, ovl2_D_800FCF58[tens]);
        Obj_SetData(ovl2_D_800FCEBC, ovl2_D_800FCF58[ones]);
        Obj_SetPos(ovl2_D_800FCEB4, 152, 30, 0);
        Obj_SetPos(ovl2_D_800FCEB8, 164, 30, 0);
        Obj_SetPos(ovl2_D_800FCEBC, 176, 30, 0);
        Obj_Show(ovl2_D_800FCEB0);
    }
}

/* ======================================================================== */
/* Bonus stage (0x800F45DC)                                                  */
/* ======================================================================== */

extern Heap gGameHeap;
extern ObjSceneNode *gSceneGroup0;
extern s32 D_800B0608;          /* current girl 0..7 */
extern s32 D_800B04D4[2];       /* lives of player 1 / 2 (0 = not playing) */
extern u16 D_800C0D48;          /* player 1 buttons */
extern u16 D_800C0D4A;          /* player 2 buttons */
extern u16 D_800C30C4;          /* service request */
extern s32 D_800C1674;          /* set when the service request aborted the game */
extern s32 D_800AF260;          /* abort request */

void Jpeg64_Decode(u32 rom, u16 *dst, s32 x, s32 y, s32 *w, s32 *h);
extern void func_8001BBC0(s32 song);    /* play music */
extern void ovl2_func_800E2910(u16 *image, u16 *photo, u8 *cells, u8 *blocks, s32 w, s32 h); /* Board_Init */

extern u8  *ovl2_D_800FD42C;    /* board: dirty blocks (0x9600) */
extern u8  *ovl2_D_800FD2BC;    /* board: cells (0x12C00) */
extern u16 *ovl2_D_800FD2D4;    /* decoded photo (0x96000) */
extern u16 *ovl2_D_800FD548;    /* displayed image (0x96000) */
extern u32 ovl2_D_800FAA98[];   /* per girl, 16 bytes: [0] = bonus photo asset */
extern SceneObjDesc ovl2_D_800F9DF0[];
extern SceneAssetDesc ovl2_D_800FA87C[];
extern Obj *ovl2_D_800FCD90[64];   /* the 64 bonus tiles */
extern Obj *ovl2_D_800FCE90, *ovl2_D_800FCE94, *ovl2_D_800FCE98, *ovl2_D_800FCE9C;
extern Obj *ovl2_D_800FCEA0, *ovl2_D_800FCEA4;
extern void *ovl2_D_800FCEF8[5 * 4];   /* tile sprites: 5 colours x 4 */
extern void *ovl2_D_800FCF4C[2];       /* blinking "press" sprite */
extern BonusTile ovl2_D_800FCFA8[64];

#define ASSET_BASE 0x611F0

/* Buttons of whoever plays (both players' when both play). */
static u32 bonus_buttons(void)
{
    if (D_800B04D4[0] != 0) {
        if (D_800B04D4[1] != 0)
            return D_800C0D48 | D_800C0D4A;
        return D_800C0D48;
    }
    return D_800C0D4A;
}

/* Scatter the 64 tiles (always the same layout: fixed seed). */
static void bonus_scatter(void)
{
    s32 seed = 0x3234;
    s32 i;
    for (i = 0; i < 64; i++) {
        ovl2_D_800FCFA8[i].x = Rand_Range(&seed, 260) + 30;
        ovl2_D_800FCFA8[i].y = Rand_Range(&seed, 220) + 10;
        ovl2_D_800FCFA8[i].colour = Rand_Range(&seed, 5);
    }
}

/* Show the first n/4 tiles (each one is set 4 times, the last sprite wins)
 * and hide the rest. */
static void bonus_draw(s32 n)
{
    s32 i;
    for (i = 0; i < n; ) {
        s32 k = i / 4;
        Obj **o = &ovl2_D_800FCD90[k];
        Obj_SetData(*o, ovl2_D_800FCEF8[ovl2_D_800FCFA8[k].colour * 4 + (i & 3)]);
        i++;
        Obj_SetPos(*o, ovl2_D_800FCFA8[k].x, ovl2_D_800FCFA8[k].y, 0);
    }
    for (i = (i + 3) & 0xFFFC; i < 256; i++)
        Obj_Hide(ovl2_D_800FCD90[i / 4]);
}

/* @ 0x800F45DC */
/* Bonus stage task: the girl's bonus photo is covered by 64 tiles; every press
 * of button 0x10 removes one (4 counts) within 600 frames. Shows the tiles for
 * 121 frames, then plays; clearing all tiles hides the HUD and pans over the
 * whole photo (300/640/480 frames for 320x240/640x240/320x480); time up shows
 * D_800FCE9C for 121 frames. Then a mosaic fade-out, teardown (also frees both
 * players) and *done = leftover | 0x10000.
 * Quirk: the original ORs 0x10000 into a never-written stack slot; the C uses
 * 0 for it (the test primes that slot to 0). */
void Bonus_Task(u32 *done)
{
    s32 w, h;
    PhotoView *view;
    Obj *viewObj;
    s32 i, n, frame, mode, dir, left;
    u32 leftover = 0;

    ovl2_D_800FD2D8 = 0;
    ovl2_D_800FD42C = Heap_Alloc(&gGameHeap, 0x9600);
    ovl2_D_800FD2BC = Heap_Alloc(&gGameHeap, 0x12C00);
    ovl2_D_800FD2D4 = Heap_Alloc(&gGameHeap, 0x96000);
    for (i = 0; i <= 0x4AFFF; i++)
        ovl2_D_800FD2D4[i] = 0;
    ovl2_D_800FD548 = Heap_Alloc(&gGameHeap, 0x96000);
    for (i = 0; i <= 0x4AFFF; i++)
        ovl2_D_800FD548[i] = 0;
    Jpeg64_Decode((ovl2_D_800FAA98[D_800B0608 * 4] & 0xFFFFFF) + ASSET_BASE,
                  ovl2_D_800FD2D4, 0, 0, &w, &h);
    Image_CopyRect(ovl2_D_800FD2D4, w, h, 0, 0, w, h, ovl2_D_800FD548, w, h, 0, 0);
    ovl2_D_800FD2C8 = 0;
    ovl2_D_800FD2B8 = 0;
    view = PhotoView_New(NULL, ovl2_D_800FD548, w, h, 0, 0);
    viewObj = Obj_NewDL(gSceneGroup0, view->setup_dl, view->dl, view->finish_dl);
    Obj_Show(viewObj);
    ovl2_func_800E2910(ovl2_D_800FD548, ovl2_D_800FD2D4, ovl2_D_800FD2BC, ovl2_D_800FD42C, w, h);
    Scene_Build(ovl2_D_800F9DF0, ovl2_D_800FA87C);
    Obj_Hide(ovl2_D_800FCE90);
    Obj_Hide(ovl2_D_800FCE94);
    bonus_scatter();
    frame = 0;
    ovl2_D_800FD424 = 659;
    Hud_UpdateTimer2();
    n = 256;

    /* show the tiles */
    do {
        frame++;
        func_8000AEAC();
        if (D_800C30C4 != 0)
            goto service;
        if (D_800AF260 != 0)
            goto finish;
        bonus_draw(n);
    } while (frame < 121);

    /* play */
    Obj_Hide(ovl2_D_800FCE98);
    func_8001BBC0(D_800B0608 + 6);
    Obj_Show(ovl2_D_800FCE90);
    Obj_Show(ovl2_D_800FCE94);
    bonus_scatter();
    n = 256;
    ovl2_D_800FD424 = 600;
    for (;;) {
        frame++;
        func_8000AEAC();
        if (D_800C30C4 != 0)
            goto service;
        if (D_800AF260 != 0)
            goto finish;
        Obj_SetData(ovl2_D_800FCE94, ovl2_D_800FCF4C[(frame & 4) >> 2]);
        if ((bonus_buttons() & 0x10) && n > 0)
            n -= 4;
        bonus_draw(n);
        if (n == 0)
            break;
        Hud_UpdateTimer2();
        if (ovl2_D_800FD424 == 0)
            goto time_up;
    }

    /* cleared: pan over the photo */
    Obj_Hide(ovl2_D_800FCE90);
    Obj_Hide(ovl2_D_800FCE94);
    Obj_Hide(ovl2_D_800FCEA0);
    Obj_Hide(ovl2_D_800FCEA4);
    Obj_Hide(ovl2_D_800FCEB0);
    Obj_Hide(ovl2_D_800FCEB4);
    Obj_Hide(ovl2_D_800FCEB8);
    Obj_Hide(ovl2_D_800FCEBC);
    w = ovl2_func_800E29C4();
    h = ovl2_func_800E29D0();
    mode = 0;
    if (w == 320 && h == 240) {
        dir = 0;
        left = 300;
    } else if (w == 640) {
        mode = 1;
        dir = 1;
        left = 640;
    } else {
        mode = 2;
        dir = 1;
        left = 480;
    }
    ovl2_D_800FD2D8 = 1;
    func_8001BA90(0x18);
    do {
        func_8000AEAC();
        if (D_800C30C4 != 0)
            goto service;
        if (D_800AF260 != 0)
            goto finish;
        if (mode == 1) {
            if (dir != 0) {
                ovl2_D_800FD2B8++;
                dir &= -(ovl2_D_800FD2B8 < 319);
            } else {
                ovl2_D_800FD2B8--;
                if (ovl2_D_800FD2B8 <= 0)
                    dir = 1;
            }
            left--;
        } else if (mode == 2) {
            if (dir != 0) {
                ovl2_D_800FD2C8++;
                dir &= -(ovl2_D_800FD2C8 < 239);
            } else {
                ovl2_D_800FD2C8--;
                if (ovl2_D_800FD2C8 <= 0)
                    dir = 1;
            }
            left--;
        } else {
            left--;
        }
        PhotoView_SetScroll(view, ovl2_D_800FD2B8, ovl2_D_800FD2C8);
    } while (left > 0);
    goto finish;

time_up:
    func_8001BBC0(0);
    Obj_Hide(ovl2_D_800FCE90);
    frame = 0;
    Obj_Hide(ovl2_D_800FCE94);
    Obj_Show(ovl2_D_800FCE9C);
    do {
        frame++;
        func_8000AEAC();
        if (D_800C30C4 != 0)
            goto service;
        if (D_800AF260 != 0)
            goto finish;
    } while (frame < 121);
    goto finish;

service:
    D_800C1674 = 1;
finish:
    ovl2_D_800FD2D8 = 1;
    func_8000AEAC();
    func_8000AEAC();
    for (i = 1; i < 7; i++) {
        Image_Mosaic(ovl2_D_800FD548, 320, 480, 0, 0, 320, 480, i, 1.0f);
        func_8000AEAC();
    }
    Obj_Kill(viewObj);
    Scene_Destroy(ovl2_D_800F9DF0, ovl2_D_800FA87C);
    func_8000AEAC();
    func_8000AEAC();
    PhotoView_Delete(view);
    Heap_Free(&gGameHeap, ovl2_D_800FD548);
    Heap_Free(&gGameHeap, ovl2_D_800FD2D4);
    Heap_Free(&gGameHeap, ovl2_D_800FD2BC);
    Heap_Free(&gGameHeap, ovl2_D_800FD42C);
    Heap_Free(&gGameHeap, ovl2_D_800FD3F8[0]);
    Heap_Free(&gGameHeap, ovl2_D_800FD3F8[1]);
    *done = leftover | 0x10000;
}

/* ======================================================================== */
/* Field items (0x800F5780, 0x800F5D50)                                      */
/* ======================================================================== */

extern s32 ovl2_func_800E2E24(s32 x, s32 y);   /* Board_GetCell: 0 unclaimed, 1 claimed */
extern s32 ovl2_D_800FAB30[97];                /* item kind table (random pick) */
extern s32 ovl2_D_800FD2C0;                    /* item 0 effect timer */

/* An item's animation state (a 0x1C-byte local in the original, cleared once
 * when the task starts and kept across items). */
typedef struct ItemState {
    f32 x, y;                   /* 0x00 field position */
    s32 unk08, unk0C;
    s32 frame;                  /* 0x10 index of the current frame (lists start at 1) */
    s32 timer;                  /* 0x14 */
    void **cur;                 /* 0x18 */
} ItemState;

/* Next frame of a looping list (index 0 is skipped on the first pass only:
 * the index is incremented before use and reset to 0 on an end marker). */
static void item_anim_loop(Obj *obj, ItemState *m, EffFrame **plist)
{
    if (m->timer == 0) {
        m->frame++;
        for (;;) {
            EffFrame *f = &(*plist)[m->frame];
            m->cur = f->data;
            m->timer = f->time;
            if (m->timer >= 0)
                break;
            m->frame = 0;
        }
        Obj_SetData(obj, *m->cur);
    } else {
        m->timer--;
    }
}

static void item_place(Obj *obj, ItemState *m)
{
    Obj_SetPos(obj, (s32)(m->x - (f32)ovl2_D_800FD2B8), (s32)(m->y - (f32)ovl2_D_800FD2C8), 0);
}

/* Pick a random unclaimed spot at least 32 pixels inside the field (yielding
 * every 64 failed tries). Returns 0 if the round ended meanwhile. */
static s32 item_find_spot(s32 *seed, s32 *px, s32 *py)
{
    s32 tries = 0;
    s32 w = ovl2_func_800E29C4();
    s32 h = ovl2_func_800E29D0();
    s32 x, y;

    tries++;
    for (;;) {
        x = Rand_Range(seed, w - 64) + 32;
        y = Rand_Range(seed, h - 64) + 32;
        if (ovl2_func_800E2E24(x, y) == 0)
            break;
        if ((tries & 0x3F) != 0) {
            tries++;
            continue;
        }
        tries++;
        func_8000AEAC();
        if (ovl2_D_800FD2D8 != 0)
            return 0;
    }
    *px = x;
    *py = y;
    return 1;
}

/* Wait n frames; returns 0 if the round ended meanwhile. */
static s32 item_wait(s32 n)
{
    s32 i = n - 1;
    if (i != -1) {
        do {
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                return 0;
            i--;
        } while (i != -1);
    }
    return 1;
}

/* @ 0x800F5780 */
/* Item task (obj, anims): every 20..39 seconds an item of a random kind
 * (ovl2_D_800FAB30; kind 1 = extra life becomes kind 3 if a player already has
 * 3 lives) appears on an unclaimed spot. anims[kind*2] = appear (once),
 * anims[kind*2+1] = idle (blinks in its last 3 seconds, gone after 900 frames),
 * anims[8+kind] = collected (floats up from (160, 140) for 120 frames).
 * It is collected when its spot gets claimed: sound 11 and
 *   kind 0: ovl2_D_800FD2C0 = 600; kind 1: +1 life for each playing player;
 *   kind 2 / 3: each playing player gets 600 frames of speed-up / invincibility.
 * The random seed starts as the object's address. Ends (hidden) with the round. */
void Item_Task(Obj *obj, EffFrame **anims)
{
    s32 seed = (s32)obj;
    ItemState m;
    s32 x, y, kind, life, r, t;

    bzero(&m, sizeof(m));
    for (;;) {
        if (!item_wait((Rand_Range(&seed, 20) + 20) * 60))
            break;
        if (!item_find_spot(&seed, &x, &y))
            break;
        m.x = (f32)x;
        m.y = (f32)y;
        kind = ovl2_D_800FAB30[Rand_Range(&seed, 97)];
        if (kind == 1 && (D_800B04D4[0] >= 3 || D_800B04D4[1] >= 3))
            kind = 3;
        m.timer = 0;

        /* appear */
        for (;;) {
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
            if (m.timer == 0) {
                EffFrame *f;
                m.frame++;
                f = &anims[kind * 2][m.frame];
                m.cur = f->data;
                m.timer = f->time;
                if (m.timer < 0)
                    break;
                Obj_SetData(obj, *m.cur);
            } else {
                m.timer--;
            }
            item_place(obj, &m);
        }
        m.frame = 0;
        m.timer = 0;

        /* wait to be collected */
        life = 900;
        for (;;) {
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
            if (life != 0) {
                life--;
                if (life == 0)
                    goto gone;
            }
            item_anim_loop(obj, &m, &anims[kind * 2 + 1]);
            if (life < 180) {
                if (life & 1)
                    Obj_Show(obj);
                else
                    Obj_Hide(obj);
            }
            item_place(obj, &m);
            r = ovl2_func_800E2E24((s32)m.x, (s32)m.y);
            if (r == 1)
                break;
        }
        func_8001BA90(0xB);
        if (kind == r) {
            if (D_800B04D4[0] != 0)
                D_800B04D4[0]++;
            if (D_800B04D4[1] != 0)
                D_800B04D4[1]++;
        } else if (kind < 2) {
            if (kind == 0)
                ovl2_D_800FD2C0 = 600;
        } else if (kind == 2) {
            if (D_800B04D4[0] != 0)
                PLAYER_SPEEDUP(ovl2_D_800FD3F8[0]) = 600;
            if (D_800B04D4[1] != 0)
                PLAYER_SPEEDUP(ovl2_D_800FD3F8[1]) = 600;
        } else if (kind == 3) {
            if (D_800B04D4[0] != 0)
                PLAYER_INVINCIBLE(ovl2_D_800FD3F8[0]) = 600;
            if (D_800B04D4[1] != 0)
                PLAYER_INVINCIBLE(ovl2_D_800FD3F8[1]) = 600;
        }

        /* collected: float up */
        t = 0;
        m.frame = 0;
        m.timer = 0;
        m.x = 160.0f;
        m.y = 140.0f;
        for (;;) {
            t++;
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
            if (t >= 121)
                break;
            item_anim_loop(obj, &m, &anims[8 + kind]);
            m.y = m.y - 1.0f;
            Obj_SetPos(obj, (s32)m.x, (s32)m.y, 0);
        }
gone:
        Obj_Hide(obj);
    }
end:
    Obj_Hide(obj);
}

/* Run a looping animation for up to 240 frames while waiting for the item's
 * spot to be claimed. Returns 1 = claimed, 0 = time over, -1 = round over. */
static s32 clock_phase_wait(Obj *obj, ItemState *m, EffFrame **plist)
{
    s32 t = 0;
    m->frame = 0;
    m->timer = 0;
    for (;;) {
        t++;
        func_8000AEAC();
        if (ovl2_D_800FD2D8 != 0)
            return -1;
        if (t >= 241)
            return 0;
        item_anim_loop(obj, m, plist);
        if (ovl2_func_800E2E24((s32)m->x, (s32)m->y) == 1)
            return 1;
        item_place(obj, m);
    }
}

/* Play a one-shot animation (no claim check). Returns 0 if the round ended. */
static s32 clock_phase_once(Obj *obj, ItemState *m, EffFrame **plist)
{
    for (;;) {
        func_8000AEAC();
        if (ovl2_D_800FD2D8 != 0)
            return 0;
        if (m->timer == 0) {
            EffFrame *f;
            m->frame++;
            f = &(*plist)[m->frame];
            m->cur = f->data;
            m->timer = f->time;
            if (m->timer < 0) {
                m->frame = 0;
                m->timer = 0;
                return 1;
            }
            Obj_SetData(obj, *m->cur);
        } else {
            m->timer--;
        }
        item_place(obj, m);
    }
}

/* @ 0x800F5D50 */
/* Clock item task (obj, anims): every 10..19 seconds a clock appears on a
 * random unclaimed spot and alternates forever between two states, 240 frames
 * each: anims[0] (claiming it takes 10 seconds off the round timer, minimum
 * 1 second) -> anims[1] once -> anims[2] (claiming it adds 20 seconds) ->
 * anims[3] once -> anims[0]... Claiming plays sound 11 and shows anims[5] /
 * anims[4] floating up from (160, 120) for 120 frames. Ends with the round. */
void Clock_Task(Obj *obj, EffFrame **anims)
{
    s32 seed = (s32)obj;
    ItemState m;
    s32 x, y, kind, r, t;

    bzero(&m, sizeof(m));
    for (;;) {
        if (!item_wait((Rand_Range(&seed, 10) + 10) * 60))
            break;
        if (!item_find_spot(&seed, &x, &y))
            break;
        m.x = (f32)x;
        m.y = (f32)y;
        for (;;) {
            r = clock_phase_wait(obj, &m, &anims[0]);
            if (r < 0)
                goto end;
            if (r > 0) {
                kind = 5;
                break;
            }
            if (!clock_phase_once(obj, &m, &anims[1]))
                goto end;
            r = clock_phase_wait(obj, &m, &anims[2]);
            if (r < 0)
                goto end;
            if (r > 0) {
                kind = 4;
                break;
            }
            if (!clock_phase_once(obj, &m, &anims[3]))
                goto end;
        }
        func_8001BA90(0xB);
        if (kind == 4) {
            ovl2_D_800FD424 = ovl2_D_800FD424 + 1200;
        } else {
            ovl2_D_800FD424 = ovl2_D_800FD424 - 600;
            if (ovl2_D_800FD424 <= 0)
                ovl2_D_800FD424 = 60;
        }
        t = 0;
        m.frame = 0;
        m.timer = 0;
        m.x = 160.0f;
        m.y = 120.0f;
        for (;;) {
            t++;
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
            if (t >= 121)
                break;
            item_anim_loop(obj, &m, &anims[kind]);
            m.y = m.y - 1.0f;
            Obj_SetPos(obj, (s32)m.x, (s32)m.y, 0);
        }
        Obj_Hide(obj);
    }
end:
    Obj_Hide(obj);
}

/* ======================================================================== */
/* Edge crawler (0x800F4FC0)                                                 */
/* ======================================================================== */

typedef struct { s32 dx, dy; } DirStep;
extern DirStep ovl2_D_800FAB10[4];      /* crawl directions */

/* The crawler's state (a 0x20-byte local, cleared once at the start). */
typedef struct CrawlerState {
    f32 x, y;                   /* 0x00 field position */
    s32 dir;                    /* 0x08 0..3 into ovl2_D_800FAB10 */
    s32 unk0C;
    s32 unk10;
    s32 frame;                  /* 0x14 */
    s32 timer;                  /* 0x18 */
    void **cur;                 /* 0x1C */
} CrawlerState;

/* Edge cell search: scan rows (top-down or bottom-up), yielding every 64
 * cells. Returns -1 if the round ended, 1 if found, 0 if not. */
static s32 crawler_scan_row(s32 y, s32 w, s32 *px)
{
    s32 x = 0;
    while (x < w) {
        if (ovl2_func_800E2E24(x, y) == 3) {
            *px = x;
            return 1;
        }
        if ((x & 0x3F) == 0) {
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                return -1;
        }
        x++;
    }
    return 0;
}

/* Is the crawler on player p (|dx| <= 1 and dy in -1..dyMax)? */
static s32 crawler_touch(CrawlerState *s, FxPlayer *p, u32 dyRange)
{
    s32 d;
    if (p->hit != 0)
        return 0;
    d = (s32)((f32)p->x - s->x);
    if ((u32)(d + 1) >= 3)
        return 0;
    d = (s32)((f32)p->y - s->y);
    return (u32)(d + 1) < dyRange;
}

/* @ 0x800F4FC0 */
/* Edge crawler task (objs[5], anims, fromTop): every 3600 frames, find the
 * first edge cell (cell value 3) scanning rows from the top (fromTop != 0) or
 * from the bottom; four sprites spiral in onto it (anims[1], radius 320 -> 0,
 * 4 per frame), then objs[0] crawls along the edge (anims[0]; each frame it
 * tries the directions left-turn first; frozen while ovl2_D_800FD2C0 runs).
 * Touching a player (not already hit) marks him hit; then, or when its cell
 * stops being an edge, it plays anims[2] once and hides. Quirk kept: the
 * player 2 test only accepts dy of -1 or 0. Ends with the round (hiding
 * objs[0..4]). */
void Crawler_Task(Obj **objs, EffFrame **anims, s32 fromTop)
{
    CrawlerState s;
    s32 i, w, h, x, y, r;
    f32 radius, angle;

    bzero(&s, sizeof(s));
    for (;;) {
        if (!item_wait(3600))
            goto end;
        s.timer = 0;
        s.frame = 0;
        w = ovl2_func_800E29C4();
        h = ovl2_func_800E29D0();
        if (fromTop != 0) {
            for (y = 0; y < h; y++) {
                r = crawler_scan_row(y, w, &x);
                if (r < 0)
                    goto end;
                if (r > 0)
                    goto found;
            }
        } else {
            for (y = h - 1; y >= 0; y--) {
                r = crawler_scan_row(y, w, &x);
                if (r < 0)
                    goto end;
                if (r > 0)
                    goto found;
            }
        }
        goto spiral;
found:
        s.x = (f32)x;
        s.y = (f32)y;
spiral:
        radius = 320.0f;
        angle = 0.0f;
        do {
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
            if (s.timer == 0) {
                s.frame++;
                for (;;) {
                    EffFrame *f = anims[1];
                    s.cur = f[s.frame].data;
                    s.timer = f[s.frame].time;
                    if (s.timer >= 0)
                        break;
                    s.frame = 0;
                }
                Obj_SetData(objs[0], *s.cur);
                Obj_SetData(objs[1], *s.cur);
                Obj_SetData(objs[2], *s.cur);
                Obj_SetData(objs[3], *s.cur);
            } else {
                s.timer--;
            }
            for (i = 0; i < 4; i++) {
                f32 a = angle + (f32)i * PI_F / 2.0f;
                s32 cx = (s32)(cosf(a) * radius);
                s32 cy = (s32)(sinf(a) * radius);
                f32 px = s.x - (f32)ovl2_D_800FD2B8 + (f32)cx;
                f32 py = s.y - (f32)ovl2_D_800FD2C8 + (f32)cy;
                Obj_SetPos(objs[i], (s32)px, (s32)py, 0);
            }
            radius = radius - 4.0f;
            angle = angle + 0.13962634f;
        } while (!(radius <= 0.0f));
        for (i = 0; i < 4; i++)
            Obj_Hide(objs[i]);

        /* crawl */
        s.frame = 0;
        s.timer = 0;
        for (;;) {
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
            if (s.timer == 0) {
                s.frame++;
                for (;;) {
                    EffFrame *f = anims[0];
                    s.cur = f[s.frame].data;
                    s.timer = f[s.frame].time;
                    if (s.timer >= 0)
                        break;
                    s.frame = 0;
                }
                Obj_SetData(objs[0], *s.cur);
            } else {
                s.timer--;
            }
            if (ovl2_D_800FD2C0 == 0) {
                s.dir = (s.dir - 1) & 3;
                for (i = 0; i < 4; i++) {
                    if (ovl2_func_800E2E24((s32)(s.x + (f32)ovl2_D_800FAB10[s.dir].dx),
                                           (s32)(s.y + (f32)ovl2_D_800FAB10[s.dir].dy)) == 3)
                        break;
                    s.dir = (s.dir + 1) & 3;
                }
                s.x = s.x + (f32)ovl2_D_800FAB10[s.dir].dx;
                s.y = s.y + (f32)ovl2_D_800FAB10[s.dir].dy;
            }
            Obj_SetPos(objs[0], (s32)(s.x - (f32)ovl2_D_800FD2B8), (s32)(s.y - (f32)ovl2_D_800FD2C8), 0);
            if (crawler_touch(&s, ovl2_D_800FD3F8[0], 3)) {
                ovl2_D_800FD3F8[0]->hit = 1;
                break;
            }
            if (crawler_touch(&s, ovl2_D_800FD3F8[1], 2)) {
                ovl2_D_800FD3F8[1]->hit = 1;
                break;
            }
            if (ovl2_func_800E2E24((s32)s.x, (s32)s.y) != 3)
                break;
        }

        /* vanish */
        s.frame = 0;
        s.timer = 0;
        for (;;) {
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
            if (s.timer == 0) {
                EffFrame *f;
                s.frame++;
                f = anims[2];
                s.cur = f[s.frame].data;
                s.timer = f[s.frame].time;
                if (s.timer < 0) {
                    s.frame = 0;
                    break;
                }
                Obj_SetData(objs[0], *s.cur);
            } else {
                s.timer--;
            }
            Obj_SetPos(objs[0], (s32)(s.x - (f32)ovl2_D_800FD2B8), (s32)(s.y - (f32)ovl2_D_800FD2C8), 0);
        }
        Obj_Hide(objs[0]);
    }
end:
    for (i = 0; i < 5; i++)
        Obj_Hide(objs[i]);
}

/* ======================================================================== */
/* Player task (0x800F10F0)                                                  */
/* ======================================================================== */

extern s32 D_800C51D8;          /* attract demo running (non-zero) */
extern u16 D_800B060C[2];       /* player inputs */
extern s32 ovl2_D_800FD2CC;     /* players wait (respawn held) while non-zero */
extern void *func_8000A76C(void *fn, s32 prio, s32 argBytes, ...);   /* start a task */
extern void ovl2_func_800E8F74(u16 input, FxPlayer *p);              /* Player_Update */
extern void ovl2_func_800E93EC(FxPlayer *p, s32 *done);              /* Player_RewindTask */
extern void ovl2_func_800E9B10(FxPlayer *p, s32 *done);              /* Player_FillTrailTask */

/* Next frame of the player's looping animation list, shown on objs[0..n-1]. */
static void player_anim(FxPlayer *p, EffFrame **plist, Obj **objs, s32 n)
{
    if (PLAYER_TIMER(p) == 0) {
        s32 i;
        PLAYER_FRAME(p)++;
        for (;;) {
            EffFrame *f = *plist;
            PLAYER_CUR(p) = f[PLAYER_FRAME(p)].data;
            PLAYER_TIMER(p) = f[PLAYER_FRAME(p)].time;
            if (PLAYER_TIMER(p) >= 0)
                break;
            PLAYER_FRAME(p) = 0;
        }
        for (i = 0; i < n; i++)
            Obj_SetData(objs[i], *PLAYER_CUR(p));
    } else {
        PLAYER_TIMER(p)--;
    }
}

/* Four sprites on a circle of the given radius around the player. */
static void player_ring(FxPlayer *p, Obj **objs, f32 radius, f32 angle)
{
    s32 i;
    for (i = 0; i < 4; i++) {
        f32 a = angle + (f32)i * PI_F / 2.0f;
        s32 cx = (s32)(cosf(a) * radius);
        s32 cy = (s32)(sinf(a) * radius);
        Obj_SetPos(objs[i], p->x - ovl2_D_800FD2B8 + cx, p->y - ovl2_D_800FD2C8 + cy, 0);
    }
}

/* @ 0x800F10F0 */
/* Player task (unused, objs[5], unused, unused, anims, player). Per life:
 * wait until the player has lives; find an edge cell (the old position if it is
 * an edge on screen, else scan the screen in 2-pixel steps, yielding every 4
 * rows); four sprites spiral in (anims[1]); then play: 180 frames of
 * invincibility (blinking; anims[3]/[4] while it lasts, idle/drawing, else
 * anims[0]), input from D_800B060C[id] (random in the attract demo), one extra
 * Player_Update per frame while speeded up. Getting hit (when not invincible)
 * or standing on a claimed cell starts the rewind task; being surrounded by
 * claimed cells on 3+ sides starts the fill task. Either way: sound 5, death
 * animation anims[2], sprites spiral out, wait for that task, lose a life
 * (not in the demo), wait while ovl2_D_800FD2CC, next life. */
void Player_Task(s32 *unused, Obj **objs, s32 unused2, s32 unused3, EffFrame **anims, FxPlayer *p)
{
    s32 seed = p->id * 1236;
    s32 i, x, y, x0, y0, n, done;
    u16 input, demoTimer;
    f32 radius, angle;
    void *fn;

    (void)unused;
    (void)unused2;
    (void)unused3;
    for (;;) {
        p->hit = 1;
        p->active = 0;
        input = 0;
        demoTimer = 0;
        done = 0;
        while (D_800B04D4[p->id] == 0) {
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
        }

        /* find a place on the edge */
        for (;;) {
            if (p->x >= ovl2_D_800FD2B8 + 8 && p->x < ovl2_D_800FD2B8 + 312
                && p->y >= ovl2_D_800FD2C8 + 8 && p->y < ovl2_D_800FD2C8 + 232
                && ovl2_func_800E2E24(p->x, p->y) == 3)
                break;
            for (;;) {
                y0 = ovl2_D_800FD2C8 & ~1;
                y = y0 + 8;
                x0 = ovl2_D_800FD2B8 & ~1;
                do {
                    for (x = x0 + 8; x < x0 + 312; x += 2) {
                        if (ovl2_func_800E2E24(x, y) == 3) {
                            p->x = x;
                            p->y = y;
                            func_8000AEAC();
                            if (ovl2_D_800FD2D8 != 0)
                                goto end;
                            goto recheck;
                        }
                    }
                    if ((y & 6) == 0) {
                        y += 2;
                        func_8000AEAC();
                        if (ovl2_D_800FD2D8 != 0)
                            goto end;
                    } else {
                        y += 2;
                    }
                } while (y < y0 + 232);
            }
recheck:;
        }

        /* appear */
        p->active = 1;
        radius = 320.0f;
        angle = 0.0f;
        do {
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
            player_anim(p, &anims[1], objs, 4);
            player_ring(p, objs, radius, angle);
            radius = radius - 4.0f;
            angle = angle + 0.13962634f;
        } while (!(radius <= 0.0f));
        for (i = 0; i < 4; i++)
            Obj_Hide(objs[i]);
        PLAYER_FRAME(p) = 0;
        PLAYER_TIMER(p) = 0;
        p->hit = 0;
        PLAYER_INVINCIBLE(p) = 180;
        func_8001BA90(0xD);

        /* play */
        for (;;) {
            EffFrame **list;
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
            if (p->hit != 0 && PLAYER_INVINCIBLE(p) == 0) {
                fn = ovl2_func_800E93EC;
                break;
            }
            p->hit = 0;
            if (ovl2_func_800E2E24(p->x, p->y) == 1) {
                fn = ovl2_func_800E93EC;
                break;
            }
            n = (ovl2_func_800E2E24(p->x, p->y + 1) == 1);
            n += (ovl2_func_800E2E24(p->x, p->y - 1) == 1);
            n += (ovl2_func_800E2E24(p->x + 1, p->y) == 1);
            n += (ovl2_func_800E2E24(p->x - 1, p->y) == 1);
            if (n >= 3) {
                fn = ovl2_func_800E9B10;
                break;
            }
            if (PLAYER_INVINCIBLE(p) != 0)
                list = p->drawing != 0 ? &anims[4] : &anims[3];
            else
                list = &anims[0];
            player_anim(p, list, objs, 1);
            if (PLAYER_INVINCIBLE(p) != 0) {
                s32 t = PLAYER_INVINCIBLE(p) - 1;
                PLAYER_INVINCIBLE(p) = t;
                if (t < 60 ? (t & 2) : (t & 1))
                    Obj_Show(objs[0]);
                else
                    Obj_Hide(objs[0]);
            }
            if (D_800C51D8 != 0) {
                if (demoTimer == 0) {
                    input = 0x10;
                    if (Rand_Range(&seed, 2) != 0) {
                        u32 r = Rand_Range(&seed, 2);
                        if (r == 0)
                            input = 0x11;
                        else if (r == 1)
                            input = 0x12;
                    } else {
                        u32 r = Rand_Range(&seed, 2);
                        if (r == 0)
                            input = 0x14;
                        else if (r == 1)
                            input = 0x18;
                    }
                    demoTimer = Rand_Range(&seed, 10) + 10;
                } else {
                    demoTimer--;
                }
            } else {
                input = D_800B060C[p->id];
            }
            if (PLAYER_SPEEDUP(p) != 0) {
                PLAYER_SPEEDUP(p)--;
                ovl2_func_800E8F74(input, p);
            }
            ovl2_func_800E8F74(input, p);
            Obj_SetPos(objs[0], p->x - ovl2_D_800FD2B8, p->y - ovl2_D_800FD2C8, 0);
        }

        /* lost the life */
        done = 0;
        func_8000A76C(fn, 0x19, 8, p, &done);
        func_8001BA90(5);
        p->active = 0;
        PLAYER_FRAME(p) = 0;
        PLAYER_TIMER(p) = 0;
        for (;;) {
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
            if (PLAYER_TIMER(p) == 0) {
                EffFrame *f;
                PLAYER_FRAME(p)++;
                f = anims[2];
                PLAYER_CUR(p) = f[PLAYER_FRAME(p)].data;
                PLAYER_TIMER(p) = f[PLAYER_FRAME(p)].time;
                if (PLAYER_TIMER(p) < 0) {
                    PLAYER_FRAME(p) = 0;
                    break;
                }
                Obj_SetData(objs[0], *PLAYER_CUR(p));
            } else {
                PLAYER_TIMER(p)--;
            }
            Obj_SetPos(objs[0], p->x - ovl2_D_800FD2B8, p->y - ovl2_D_800FD2C8, 0);
        }
        radius = 0.0f;
        PLAYER_TIMER(p) = 0;
        angle = radius;
        do {
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
            player_anim(p, &anims[1], objs, 4);
            player_ring(p, objs, radius, angle);
            radius = radius + 8.0f;
            angle = angle + 0.13962634f;
        } while (!(320.0f <= radius));
        for (i = 0; i < 4; i++)
            Obj_Hide(objs[i]);
        while (done == 0) {
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
        }
        if (D_800C51D8 == 0 && D_800B04D4[p->id] != 0)
            D_800B04D4[p->id]--;
        while (ovl2_D_800FD2CC != 0) {
            func_8000AEAC();
            if (ovl2_D_800FD2D8 != 0)
                goto end;
        }
    }
end:
    for (i = 0; i < 5; i++)
        Obj_Hide(objs[i]);
}

/* ======================================================================== */
/* Spinning boss (0x800F00F4)                                                */
/* ======================================================================== */

/* Boss state: a Spinner (see ovl2_c.h; Spinner_Move works on the first 0x28
 * bytes) followed by the animation fields. 0x38 bytes, cleared at the start. */
typedef struct SpinBoss {
    f32 x, y;                   /* 0x00 field position */
    f32 vx, vy;                 /* 0x08 */
    f32 speed;                  /* 0x10 */
    f32 scale;                  /* 0x14 shrinks slowly while roaming */
    s32 angle;                  /* 0x18 heading */
    s32 spin;                   /* 0x1C sprite rotation, 0..0x3FF (fast bullets go this way) */
    s32 spinSpeed;              /* 0x20 */
    s32 timer;                  /* 0x24 */
    s32 unk28;
    s32 frame;                  /* 0x2C */
    s32 animTimer;              /* 0x30 */
    void **cur;                 /* 0x34 */
} SpinBoss;

extern s32 D_800B4AA0;                      /* difficulty */
extern s32 ovl2_D_800F9D00[];               /* per difficulty: 1-in-n chance to attack per frame */
extern s32 ovl2_D_800F9D40[][2];            /* per difficulty: speed base, random range (x0.1) */
extern DirStep ovl2_D_800F9DC0[4];          /* unstick steps (+-8) */
extern s32 ovl2_D_800FD4F0[];               /* objects of the warning banner task */
extern u8 ovl2_func_800E3C64(u8 *bits, s32 x, s32 y);          /* Bits_TestInner */
extern void ovl2_func_800EFCF0(SpinBoss *s, Obj *obj, EffFrame **anims, s32 *seed, s32 mode); /* Spinner_Move */
extern void ovl2_func_800F4100(void *task, Obj **objs);         /* = Task_BlinkBanner */
#define FN_BLINK_BANNER ovl2_func_800F4100

/* One frame of the boss: animate objs[0] with anims[seq], scale it, step out
 * of claimed area (one pixel towards the first free side), move (Spinner_Move
 * mode) and hit-test both players with a box of 32 * scale. */
static void spinboss_frame(SpinBoss *s, Obj **objs, EffFrame **anims, s32 seq, s32 mode, s32 *seed)
{
    Obj *obj = objs[0];
    s32 x, y, i;
    f32 sc;

    if (s->animTimer == 0) {
        s->frame++;
        for (;;) {
            EffFrame *f = anims[seq];
            s->cur = f[s->frame].data;
            s->animTimer = f[s->frame].time;
            if (s->animTimer >= 0)
                break;
            s->frame = 0;
        }
        Obj_SetData(obj, *s->cur);
    } else {
        s->animTimer--;
    }
    sc = s->scale;
    obj->scale[2] = sc;
    obj->scale[1] = sc;
    obj->scale[0] = sc;
    x = (s32)s->x;
    y = (s32)s->y;
    if (ovl2_func_800E3C64(ovl2_D_800FD42C, x, y)) {
        for (i = 0; i < 4; i++) {
            if (!ovl2_func_800E3C64(ovl2_D_800FD42C, x + ovl2_D_800F9DC0[i].dx, y + ovl2_D_800F9DC0[i].dy)) {
                s->x = s->x + (f32)(ovl2_D_800F9DC0[i].dx / 8);
                s->y = s->y + (f32)(ovl2_D_800F9DC0[i].dy / 8);
                break;
            }
        }
    }
    ovl2_func_800EFCF0(s, objs[0], anims, seed, mode);
    for (i = 0; i < 2; i++) {
        FxPlayer *p = ovl2_D_800FD3F8[i];
        if (p->hit == 0) {
            s32 size = (s32)(s->scale * 32.0f);
            if (ovl2_func_800E3D28(ovl2_D_800FD3F8[i], (s32)s->x, (s32)s->y, size, size))
                ovl2_D_800FD3F8[i]->hit = 1;
        }
    }
}

/* @ 0x800F00F4 */
/* Spinning boss task (unused, objs[24], x, y, anims): roams with Spinner_Move
 * (anims[0]), shrinking x0.9997 per frame down to 0.167. Each frame (unless
 * ovl2_D_800FD2C0 freezes the attacks) it attacks with a difficulty-dependent
 * chance, starting the warning banner task, then either
 *  - 180 frames of fast spinning (anims[1], mode 2) followed by 180 frames that
 *    also fire a fast bullet along the spin 3 frames out of 4 (sound 0x16), or
 *  - 120 frames of mode 1 (anims[1]), then one frame of anims[2] after which
 *    8 homing missiles (sound 0x10) are launched from an ellipse around it at a
 *    random playing player (nothing if none plays or attacks are frozen).
 * Ends (hiding all 24 objects) with the round. The random seed starts as the
 * objs pointer. */
void SpinBoss_Task(s32 unused, Obj **objs, s32 x, s32 y, EffFrame **anims)
{
    SpinBoss s;
    s32 seed = (s32)objs;
    s32 i, n, target;

    (void)unused;
    bzero(&s, sizeof(s));
    s.x = (f32)x;
    s.y = (f32)y;
    s.angle = 0;
    s.spin = 0;
    s.spinSpeed = 0;
    s.scale = 1.0f;
    {
        s32 r = Rand_Range(&seed, ovl2_D_800F9D40[D_800B4AA0][1]);
        s.speed = (f32)((f64)(f32)(ovl2_D_800F9D40[D_800B4AA0][0] + r) / 10.0);
    }
    s.timer = 0;
    s.frame = 0;
    s.animTimer = 0;

roam:
    s.frame = 0;
    s.animTimer = 0;
    for (;;) {
        func_8000AEAC();
        if (ovl2_D_800FD2D8 != 0)
            goto end;
        spinboss_frame(&s, objs, anims, 0, 0, &seed);
        if (ovl2_D_800FD2C0 != 0)
            continue;
        if (Rand_Range(&seed, ovl2_D_800F9D00[D_800B4AA0]) == 0) {
            func_8000A76C(FN_BLINK_BANNER, 0x19, 8, NULL, ovl2_D_800FD4F0);
            n = 0;
            if (Rand_Range(&seed, 2) == 0)
                goto missiles;
            goto spin;
        }
        s.scale = s.scale * 0.9997f;
        if ((f64)s.scale <= 0.167)
            s.scale = 0.167f;
    }

spin:
    s.frame = 0;
    s.animTimer = 0;
    for (;;) {
        n++;
        func_8000AEAC();
        if (ovl2_D_800FD2D8 != 0)
            goto end;
        if (n >= 181)
            break;
        spinboss_frame(&s, objs, anims, 1, 2, &seed);
    }
    n = 0;
    s.frame = 0;
    s.animTimer = 0;
    for (;;) {
        n++;
        func_8000AEAC();
        if (ovl2_D_800FD2D8 != 0)
            goto end;
        if (n >= 181)
            goto roam;
        spinboss_frame(&s, objs, anims, 1, 2, &seed);
        if (ovl2_D_800FD2C0 == 0 && (n & 3) != 0) {
            func_8001BA90(0x16);
            Bullet_SpawnFast((s32)s.x, (s32)s.y, s.spin);
        }
    }

missiles:
    s.frame = 0;
    s.animTimer = 0;
    for (;;) {
        n++;
        func_8000AEAC();
        if (ovl2_D_800FD2D8 != 0)
            goto end;
        if (n >= 121)
            break;
        spinboss_frame(&s, objs, anims, 1, 1, &seed);
    }
    s.frame = 0;
    s.animTimer = 0;
    func_8000AEAC();
    if (ovl2_D_800FD2D8 != 0)
        goto end;
    spinboss_frame(&s, objs, anims, 2, 1, &seed);
    target = Rand_Range(&seed, 2);
    if (D_800B04D4[target] == 0) {
        target ^= 1;
        if (D_800B04D4[target] == 0)
            goto roam;
    }
    if (ovl2_D_800FD2C0 != 0)
        goto roam;
    func_8001BA90(0x10);
    {
        f32 a = 0.0f;
        do {
            s32 mx = (s32)(s.x + cosf(a) * 40.0f * s.scale);
            f32 sn = sinf(a);
            f32 deg = a / PI_F;
            s32 my = (s32)(s.y + sn * 20.0f * s.scale);
            Missile_Spawn(mx, my, target, (s32)(deg * 512.0f));
            a = a + 0.7853982f;
        } while (a < 6.2831855f);
    }
    goto roam;

end:
    for (i = 0; i < 24; i++)
        Obj_Hide(objs[i]);
}
