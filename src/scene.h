/* Vivid Dolls - scene graph, camera and light (RAM 0x800113E0-0x80012820).
 *
 * Scene nodes are fixed-size 0xE0-byte blocks from a block pool (D_800B4AB0,
 * 0xC000 bytes). Every node sits in a doubly-linked list of siblings. A group
 * node (type 1) owns two empty "sentinel" nodes (type 0): head and tail; its
 * children are linked between them:
 *
 *     group->head <-> child <-> child <-> ... <-> group->tail
 *     (head->prev == NULL, tail->next == NULL)
 *
 * Walking children: c = group->head->next; while (c->next) { ...; c = c->next; }
 * (the tail sentinel is the only node in the list whose next is NULL).
 *
 * Each frame: Scene_UpdateWorld (world = local * parent, recursively),
 * Scene_ConvertMatrices (world -> N64 fixed-point Mtx) and Scene_BuildGfx
 * (emit an F3D display list that draws the tree).
 */
#ifndef VD_SCENE_H
#define VD_SCENE_H

#include "vd_common.h"
#include "vd_math.h"
#include "matrix.h"
#include "sprite.h"

enum {
    SCENE_SENTINEL = 0,     /* empty list head/tail, never drawn */
    SCENE_GROUP    = 1,     /* children between head and tail */
    SCENE_DL       = 2,     /* setup_dl, matrix + dl, finish_dl (sprites) */
    SCENE_DL_LIST  = 3,     /* matrix + an array of display lists */
    SCENE_VIEWPORT = 4      /* matrix + viewport set-up (func_80010900) */
};

typedef struct SceneNode {
    struct SceneNode *next; /* 0x00 next sibling (NULL only on a tail sentinel) */
    struct SceneNode *prev; /* 0x04 previous sibling (NULL only on a head sentinel) */
    s32        type;        /* 0x08 SCENE_* */
    MtxF       mtx;         /* 0x0C local matrix (identity when created) */
    MtxF       world;       /* 0x4C local * parent world, from Scene_UpdateWorld */
    u32        unk8C;       /* 0x8C not used by the scene code */
    s32        gfx_mtx[16]; /* 0x90 N64 fixed-point Mtx of world (func_8000A924) */
    union {                 /* 0xD0 depends on type */
        struct {            /* SCENE_DL (field names used by sprite.c) */
            SpriteGfx *dl;          /* 0xD0 drawn with gfx_mtx loaded */
            SpriteGfx *setup_dl;    /* 0xD4 drawn first (may be NULL) */
            SpriteGfx *finish_dl;   /* 0xD8 drawn last (may be NULL) */
        };
        struct {            /* SCENE_GROUP */
            struct SceneNode *head; /* 0xD0 sentinel before the first child */
            struct SceneNode *tail; /* 0xD4 sentinel after the last child */
        } group;
        struct {            /* SCENE_DL_LIST */
            SpriteGfx **lists;      /* 0xD0 */
            s32         count;      /* 0xD4 */
        } dl_list;
        struct {            /* SCENE_VIEWPORT: the 6 arguments of func_80010900 */
            s16 x, y;               /* 0xD0, 0xD2: 0, 0 */
            s16 w, h;               /* 0xD4, 0xD6: gScreenWidth - 1, 239 */
            s16 sx, sy;             /* 0xD8, 0xDA: 0x2000, 0x2000 */
        } vp;
    };
    u8         unkDC[4];    /* 0xDC */
} SceneNode;                /* 0xE0 bytes (pool block size) */

/* Light: positioned light shining from pos towards target, plus ambient, and
 * the N64 Lights1 block built from them (0x40..0x53). */
typedef struct SceneLight {
    Vec3f dir;              /* 0x00 (target - pos) scaled to length 120, negated */
    Vec3f pos;              /* 0x0C */
    Vec3f target;           /* 0x18 */
    f32   color[3];         /* 0x24 0..255 */
    f32   ambient[3];       /* 0x30 0..255 */
    u8    unk3C[4];         /* 0x3C */
    u8    amb_col[4];       /* 0x40 Lights1.a.l.col (pad byte 0) */
    u8    amb_colc[4];      /* 0x44 Lights1.a.l.colc */
    u8    col[4];           /* 0x48 Lights1.l[0].l.col */
    u8    colc[4];          /* 0x4C Lights1.l[0].l.colc */
    s8    ldir[4];          /* 0x50 Lights1.l[0].l.dir (view space, length 120) + pad */
} SceneLight;               /* 0x54 bytes used */

void       Main_ParseArgs(u8 *cmdline);                         /* 0x800113E0 */
void       Scene_Init(void);                                    /* 0x80011550 */
SceneNode *Scene_NewNode(s32 type);                             /* 0x800116B8 */
s32        Scene_CountFreeNodes(void);                          /* 0x8001179C */
void       Scene_DeleteNode(SceneNode *node);                   /* 0x800117BC */
void       Scene_UpdateWorld(SceneNode *node, const MtxF parent); /* 0x80011868 */
void       Scene_ConvertMatrices(SceneNode *node);              /* 0x80011C20 */
SpriteGfx *Scene_BuildGfx(SpriteGfx *gfx, SceneNode *node);     /* 0x80011C88 */

void Camera_Init(void);                                         /* 0x80011E60 */
void Camera_SetEye(f32 x, f32 y, f32 z);                        /* 0x80011ED0 */
void Camera_SetTarget(f32 x, f32 y, f32 z);                     /* 0x80011F40 */
void Camera_GetForward(Vec3f *out);                             /* 0x80011FB0 */

void Light_Init(SceneLight *l);                                 /* 0x80012090 */
void Light_SetPos(SceneLight *l, f32 x, f32 y, f32 z);          /* 0x80012130 */
void Light_SetTarget(SceneLight *l, f32 x, f32 y, f32 z);       /* 0x80012158 */
void Light_SetColor(SceneLight *l, f32 r, f32 g, f32 b);        /* 0x80012180 */
void Light_SetAmbient(SceneLight *l, f32 r, f32 g, f32 b);      /* 0x80012250 */
void Light_Update(SceneLight *l, const MtxF view);              /* 0x80012320 */
void Light_CalcDir(SceneLight *l, Vec3f *out);                  /* 0x80012760 */

#endif
