/* Vivid Dolls - overlay 2 (the game itself), part D: 0x800F00F4 - end of ovl2 code.
 *
 * This part holds the game's small "effect" sprites (a pool of 64 slots, each
 * driving one pre-made sprite Obj through its per-frame update callback):
 * falling petals, smoke puffs, explosions, bullets, homing missiles, a
 * boomerang and a sticky net, plus a few screen tasks (banners that blink or
 * slide in, the round timer) and the larger per-round state machines.
 *
 * All functions here run with overlay 2 loaded at 0x800E0000.
 */
#ifndef VD_OVL2_D_H
#define VD_OVL2_D_H

#include "vd_common.h"
#include "object.h"
#include "heap.h"
#include "photoview.h"

/* One animation frame: the frame's sprite is *data (a sprite slot filled in at
 * load time). time >= 0: show for time+1 updates; negative values are commands
 * (-1 = loop / next animation, -2 = restart this animation, depends on user). */
typedef struct EffFrame {
    void **data;                /* 0x00 */
    s32 time;                   /* 0x04 */
} EffFrame;

/* One effect slot (ovl2_D_800FBA90[64], 0x4C bytes each). The meaning of some
 * fields depends on the effect type; see the update functions. */
typedef struct Effect {
    s32 active;                 /* 0x00 slot in use */
    s32 *done;                  /* 0x04 boomerang: set to 1 when it is caught again */
    f32 x, y;                   /* 0x08 position (field coordinates) */
    f32 homeX, homeY;           /* 0x10 boomerang: where it returns to */
    f32 tx, ty;                 /* 0x18 move-to target */
    f32 scale;                  /* 0x20 also used as a fade value */
    s32 angle;                  /* 0x24 direction, 0..0x3FF = 0..2pi */
    EffFrame **anims;           /* 0x28 animation table */
    void **cur;                 /* 0x2C current frame's sprite slot */
    s32 frame;                  /* 0x30 next frame index */
    s32 timer;                  /* 0x34 updates left on the current frame */
    s32 target;                 /* 0x38 player index (0/1) a missile/boomerang chases */
    s32 state;                  /* 0x3C missile: life; boomerang: outbound time;
                                   net: animation index (0 flying, 1+ stuck) */
    s32 anim;                   /* 0x40 animation index; boomerang: spin angle in degrees */
    s32 life;                   /* 0x44 net: time left once stuck */
    s16 caught[2];              /* 0x48 net: has caught player 0 / 1 */
} Effect;                       /* 0x4C bytes */

/* The two players (ovl2_D_800FD3F8[2] point at them). Only the fields used
 * in this part are named; the struct is huge (at least 0x3429C bytes). */
typedef struct FxPlayer {
    s32 id;                     /* 0x00 0 or 1 */
    s32 x;                      /* 0x04 */
    s32 y;                      /* 0x08 */
    s32 unkC, unk10;
    s32 hit;                    /* 0x14 set to 1 when hit (enemy, projectile, crawler) */
    s32 active;                 /* 0x18 non-zero while on the field */
    s32 drawing;                /* 0x1C non-zero while drawing a line */
} FxPlayer;
/* fields at the end of the player block */
#define PLAYER_FIELD(p, off, type) (*(type *)((u8 *)(p) + (off)))
#define PLAYER_INVINCIBLE(p) PLAYER_FIELD(p, 0x34284, s32)   /* also item 3 */
#define PLAYER_SPEEDUP(p)    PLAYER_FIELD(p, 0x34288, s32)   /* item 2: two moves per frame */
#define PLAYER_FRAME(p)      PLAYER_FIELD(p, 0x3428C, s32)   /* sprite animation */
#define PLAYER_TIMER(p)      PLAYER_FIELD(p, 0x34290, s32)
#define PLAYER_CUR(p)        PLAYER_FIELD(p, 0x34294, void **)
#define PLAYER_NETTED(p)     PLAYER_FIELD(p, 0x34298, s32)   /* held by a net */

#define EFFECT_COUNT 64

/* Bonus stage tile (ovl2_D_800FCFA8[64]). */
typedef struct BonusTile {
    s32 x, y;                   /* screen position */
    s32 colour;                 /* 0..4 */
} BonusTile;

void Game_MemClear(u8 *p, u32 n);                                   /* 0x800F1D40 */
void Effect_ClearAll(void);                                         /* 0x800F1D6C */
s32  Effect_Spawn(ObjFunc fn, EffFrame **anims, s32 x, s32 y);      /* 0x800F1D90 */
void Effect_UpdatePetal(Obj *obj);                                  /* 0x800F1ED4 */
void Effect_SpawnPetal(void);                                       /* 0x800F200C */
void Effect_UpdateSmoke(Obj *obj);                                  /* 0x800F210C */
void Effect_SpawnSmoke(s32 x, s32 y);                               /* 0x800F2208 */
void Effect_UpdateFlash(Obj *obj);                                  /* 0x800F2268 */
void Effect_SpawnFlash(s32 x, s32 y);                               /* 0x800F2384 */
void Effect_UpdateMoveTo(Obj *obj);                                 /* 0x800F23F4 */
void Effect_SpawnMoveTo(s32 x, s32 y, s32 tx, s32 ty);              /* 0x800F2580 */
void Effect_UpdateExplosion(Obj *obj);                              /* 0x800F265C */
void Effect_SpawnExplosion(s32 x, s32 y);                           /* 0x800F2768 */
void Missile_Update(Obj *obj);                                      /* 0x800F2798 */
void Missile_Spawn(s32 x, s32 y, s32 target, s32 angle);            /* 0x800F2BB0 */
void Bullet_UpdateA(Obj *obj);                                      /* 0x800F2C44 */
void Bullet_SpawnA(s32 x, s32 y, s32 angle);                        /* 0x800F2F6C */
void Bullet_UpdateB(Obj *obj);                                      /* 0x800F2FD4 */
void Bullet_SpawnB(s32 x, s32 y, s32 angle);                        /* 0x800F32FC */
void Bullet_UpdateFast(Obj *obj);                                   /* 0x800F3364 */
void Bullet_SpawnFast(s32 x, s32 y, s32 angle);                     /* 0x800F368C */
void Boomerang_Update(Obj *obj);                                    /* 0x800F36F4 */
void Boomerang_Spawn(s32 x, s32 y, s32 target, s32 angle, s32 *done); /* 0x800F3B2C */
void Net_Update(Obj *obj);                                          /* 0x800F3C24 */
void Net_Spawn(s32 x, s32 y, s32 angle);                            /* 0x800F4060 */
void Task_BlinkBanner(void *task, Obj **objs);                      /* 0x800F4100 */
void Task_SlideBanner(void *task, Obj **objs);                      /* 0x800F4210 */
void Hud_UpdateTimer2(void);                                         /* 0x800F43E0 */
void Bonus_Task(u32 *done);                                         /* 0x800F45DC */
void SpinBoss_Task(s32 unused, Obj **objs, s32 x, s32 y, EffFrame **anims); /* 0x800F00F4 */
void Player_Task(s32 *unused, Obj **objs, s32 unused2, s32 unused3,
                 EffFrame **anims, FxPlayer *p);                    /* 0x800F10F0 */
void Crawler_Task(Obj **objs, EffFrame **anims, s32 fromTop);       /* 0x800F4FC0 */
void Item_Task(Obj *obj, EffFrame **anims);                         /* 0x800F5780 */
void Clock_Task(Obj *obj, EffFrame **anims);                        /* 0x800F5D50 */

#endif
