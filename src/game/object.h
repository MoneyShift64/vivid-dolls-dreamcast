/* Vivid Dolls - object / entity system (RAM 0x80016D20-0x80019D70).
 *
 * Everything the game shows on screen (sprites, text, 3D models, raw display
 * lists, the viewport) is an Obj: a 0xB0-byte heap block on one global doubly
 * linked list (sentinels D_800A6B30 / D_800A6BE0). Each Obj owns a scene node
 * (from the scene system, func_800116B8) that is linked under a parent group
 * node; Obj_UpdateAll runs once per frame and pushes the object's state
 * (display list, matrix) into that node.
 *
 * Also in this range:
 *  - bitmap fonts (Font) and the double-buffered text display-list builder (TextBuf)
 *  - a 16-byte "allocation list" helper
 *  - Model: a small display-list "model" asset (vertices + textures + command stream)
 *  - Scene_Build / Scene_Destroy: build a whole screen from static description tables
 */
#ifndef VD_OBJECT_H
#define VD_OBJECT_H

#include "vd_common.h"
#include "matrix.h"
#include "heap.h"
#include "sprite.h"

/* ---- scene nodes (as seen by the object code; owned by the scene system) ---- */
typedef struct ObjSceneNode {
    struct ObjSceneNode *next;  /* 0x00 */
    struct ObjSceneNode *prev;  /* 0x04 */
    s32 kind;                   /* 0x08 1 = group, 2 = drawable, 4 = viewport */
    MtxF mtx;                   /* 0x0C */
    u8 unk4C[0x84];
    union {                     /* 0xD0 */
        struct { SpriteGfx *dl, *setup_dl, *finish_dl; } draw;          /* kind 2 */
        struct { struct ObjSceneNode *head, *tail; } group;             /* kind 1: child list sentinels */
        struct { s16 v[6]; } view;                                      /* kind 4 */
    } u;
    u8 unkDC[4];
} ObjSceneNode;                 /* 0xE0 bytes (pool block size) */

/* ---- objects ---- */
enum {
    OBJ_TEXT = 0,       /* data = TextBuf, data2 = Font */
    OBJ_SPRITE = 1,     /* data = Sprite */
    OBJ_SCREEN = 2,     /* data = screen Sprite (drawn with Sprite_DrawScreen) */
    OBJ_MODEL = 3,      /* data = Model */
    OBJ_DL = 4,         /* data = display list */
    OBJ_VIEW = 5        /* viewport; no display lists */
};

enum {
    OBJF_ATTACH    = 0x0001,   /* (re)attach display lists to the node next update */
    OBJF_DIRTY     = 0x0002,   /* run the transform callback next update */
    OBJF_KILL      = 0x0004,   /* free the scene node next update ... */
    OBJF_DEAD      = 0x0008,   /* ... and the object itself the update after */
    OBJF_DETACH    = 0x0010,   /* detach (hide) next update */
    OBJF_OWNS_DATA = 0x0020,   /* data was created by the constructor; freed with the object */
    OBJF_FLIP_X    = 0x0040,
    OBJF_FLIP_Y    = 0x0080,
    OBJF_FLIP_Z    = 0x0100,
    OBJF_MODEL_ALT = 0x0200,   /* models: alternative setup display lists */
    OBJF_MODEL_ALT2 = 0x0400
};

struct Obj;
typedef void (*ObjFunc)(struct Obj *obj);

typedef struct Obj {
    struct Obj *next;           /* 0x00 */
    struct Obj *prev;           /* 0x04 */
    SpriteGfx setup[6];         /* 0x08 node setup_dl: [0] call/end, [1] num lights, [2][3] lights, [4] prim colour, [5] end */
    SpriteGfx finish[4];        /* 0x38 node finish_dl */
    s16 type;                   /* 0x58 OBJ_* */
    u16 flags;                  /* 0x5A OBJF_* */
    void *data;                 /* 0x5C */
    void *data2;                /* 0x60 */
    ObjFunc update;             /* 0x64 called every frame */
    void *update_arg;           /* 0x68 */
    ObjSceneNode *node;         /* 0x6C */
    u32 unk70;
    ObjFunc transform;          /* 0x74 called when OBJF_DIRTY */
    f32 pos[3];                 /* 0x78 */
    f32 rot[3];                 /* 0x84 */
    f32 pivot[3];               /* 0x90 */
    f32 scale[3];               /* 0x9C */
    u32 unkA8[2];
} Obj;                          /* 0xB0 bytes */

/* ---- fonts and text ---- */
typedef struct FontGlyph { s16 x0, x1, y, pad; } FontGlyph;   /* advance = x1 - x0 */
typedef struct FontDef { u32 rom; FontGlyph glyph; } FontDef;  /* 12 bytes; table ends with rom == 0 */
typedef struct Font {
    s32 count;                  /* 0x00 */
    Sprite **sprites;           /* 0x04 one per character from ' ' */
    FontGlyph **glyphs;         /* 0x08 */
} Font;

typedef struct { u32 w[16]; } ObjMtx;  /* N64 fixed-point Mtx */

typedef struct TextBuf {
    u32 pad0[2];                /* 0x0000 */
    ObjMtx mtx[2][32];          /* 0x0008 one per character, per buffer */
    SpriteGfx dl[2][80];        /* 0x1008 */
    s32 cur;                    /* 0x1508 buffer last built */
    u32 pad1;
} TextBuf;                      /* 0x1510 bytes */

/* ---- allocation list ---- */
typedef struct AllocNode {
    struct AllocNode *next, *prev;
    Heap *heap;
    void *ptr;
} AllocNode;

/* ---- models ---- */
typedef struct ModelTex {
    u32 unk0;
    u16 width;                  /* 0x04 bit 7 -> clamp/mirror S */
    u16 height;                 /* 0x06 bit 7 -> clamp/mirror T */
    u8 *aux;                    /* 0x08 relative to Model.aux on disk */
    u8 *image;                  /* 0x0C relative to Model.images on disk */
} ModelTex;

typedef struct Model {
    void *buffer;               /* 0x00 one heap block for everything */
    u8 *aux;                    /* 0x04 */
    SpriteVtx *vertices;        /* 0x08 (= buffer) */
    u8 *section2;               /* 0x0C */
    u8 *images;                 /* 0x10 */
    ModelTex *textures;         /* 0x14 */
    SpriteGfx *dl;              /* 0x18 built from the command stream */
    SpriteGfx *setup_dl;        /* 0x1C D_80038C10 */
    u32 unk20;
    s16 header0;                /* 0x24 */
    u16 owned;                  /* 0x26 */
    u32 unk28;
} Model;                        /* 0x2C bytes */

/* ---- scene description tables ---- */
typedef struct SceneAssetDesc {
    void **slot;                /* 0x00 where to store the asset; NULL ends the table */
    u32 rom;                    /* 0x04 asset (low 24 bits + 0x611F0) */
    s16 type;                   /* 0x08 0 model, 2 plain sprite, 4 sprite, 9 screen sprite */
    s16 pad;
} SceneAssetDesc;

typedef struct SceneObjDesc {
    Obj **slot;                 /* 0x00 NULL ends the table */
    ObjSceneNode **parent;      /* 0x04 */
    void *arg;                  /* 0x08 rom address or pointer to an asset slot */
    u16 alpha;                  /* 0x0C */
    s16 x, y, z;                /* 0x0E */
    f32 sx, sy, sz;             /* 0x14 */
    u8 visible;                 /* 0x20 */
    u8 kind;                    /* 0x21 1..13 */
    u8 pad[2];
} SceneObjDesc;

Font   *Font_New(s32 id);                                                         /* 0x80016D20 */
Font   *Font_NewSharedPalette(s32 id);                                            /* 0x80016E58 */
s32     Font_Delete(Font *font);                                                  /* 0x80016F90 */
TextBuf *TextBuf_New(void);                                                       /* 0x80017030 */
void    TextBuf_Delete(TextBuf *tb);                                              /* 0x8001705C */
SpriteGfx *Text_Build(TextBuf *tb, Font *font, const char *str, f32 x, f32 y);    /* 0x80017080 */
SpriteGfx *Text_BuildTlut(TextBuf *tb, Font *font, const char *str, f32 x, f32 y, void *tlut); /* 0x80017304 */
TextBuf *Text_Attach(ObjSceneNode *node, TextBuf *tb, Font *font, const char *str, f32 x, f32 y); /* 0x8001763C */
TextBuf *Text_AttachTlut(ObjSceneNode *node, TextBuf *tb, Font *font, const char *str,
                         f32 x, f32 y, void *tlut);                               /* 0x800176C4 */
s32     Font_TextWidth(Font *font, const char *str);                              /* 0x80017754 */
void    AllocList_Init(AllocNode *head, AllocNode *tail);                         /* 0x800177B0 */
void    AllocList_Add(Heap *heap, void *ptr, AllocNode *tail);                    /* 0x800177F4 */
void    AllocList_FreeAll(Heap *heap, AllocNode *head);                           /* 0x80017878 */
Model  *Model_New(Model *m, u32 rom);                                             /* 0x80017900 */
Model  *Model_NewUnlit(Model *m, u32 rom);                                        /* 0x80017984 */
s32     Model_Delete(Model *m);                                                   /* 0x80017A08 */
s32     Model_Load(Model *m, u32 rom, s32 lighting);                              /* 0x80017A64 */
void   *Mem_Copy(void *dst, const void *src, u32 n);                              /* 0x800182E4 */
u16     Math_Ctz16(u32 x);                                                        /* 0x8001831C */

Obj *Obj_NewDL(ObjSceneNode *parent, SpriteGfx *setup, void *dl, SpriteGfx *finish); /* 0x80018350 */
Obj *Obj_NewDefaultDL(ObjSceneNode *parent);                                      /* 0x800183F4 */
Obj *Obj_NewView(ObjSceneNode *parent);                                           /* 0x8001843C */
Obj *Obj_NewModel(ObjSceneNode *parent, u32 rom);                                 /* 0x80018464 */
Obj *Obj_NewSpritePlain(ObjSceneNode *parent, u32 rom);                           /* 0x800184E4 */
Obj *Obj_NewSpritePlain2(ObjSceneNode *parent, u32 rom);                          /* 0x80018578 */
Obj *Obj_UseSetupCC0(Obj *obj);                                                   /* 0x8001860C */
Obj *Obj_UseSetupC80(Obj *obj);                                                   /* 0x80018628 */
Obj *Obj_NewSpritePlainRef(ObjSceneNode *parent, Sprite *spr);                    /* 0x80018644 */
Obj *Obj_NewSprite(ObjSceneNode *parent, u32 rom);                                /* 0x800186B0 */
Obj *Obj_NewSprite2(ObjSceneNode *parent, u32 rom);                               /* 0x80018754 */
Obj *Obj_NewSpriteRef(ObjSceneNode *parent, Sprite *spr);                         /* 0x800187F8 */
Obj *Obj_NewScreen(ObjSceneNode *parent, u32 rom);                                /* 0x80018874 */
Obj *Obj_NewText(ObjSceneNode *parent, Font *font, const char *str);              /* 0x80018908 */
Obj *Obj_NewTextTlut(ObjSceneNode *parent, Font *font, const char *str, void *tlut); /* 0x800189D4 */
Obj *Obj_NewModelRef(ObjSceneNode *parent, Model *m);                             /* 0x80018AB0 */
Obj *Obj_NewScreenRef(ObjSceneNode *parent, Sprite *spr);                         /* 0x80018B08 */
Obj *Obj_NewTextRef(ObjSceneNode *parent, Font *font, TextBuf *tb, const char *str); /* 0x80018B74 */
Obj *Obj_NewTextRefTlut(ObjSceneNode *parent, Font *font, TextBuf *tb, const char *str,
                        void *tlut);                                              /* 0x80018C3C */
Obj *Obj_SetText(Obj *obj, const char *str);                                      /* 0x80018D14 */
Obj *Obj_SetTextTlut(Obj *obj, const char *str, void *tlut);                      /* 0x80018D64 */
void ObjList_Init(void);                                                          /* 0x80018DBC */
s32  Obj_Kill(Obj *obj);                                                          /* 0x80018E20 */
Obj *Obj_SetData(Obj *obj, void *data);                                           /* 0x80018E34 */
Obj *Obj_SetModelAlt2(Obj *obj);                                                  /* 0x80018E90 */
Obj *Obj_SetModelAlt(Obj *obj);                                                   /* 0x80018EA4 */
Obj *Obj_SetColor(Obj *obj, u32 r, u32 g, u32 b, u32 a);                          /* 0x80018EB8 */
Obj *Obj_SetPos(Obj *obj, s32 x, s32 y, s32 z);                                   /* 0x80018EF4 */
Obj *Obj_FlipX(Obj *obj);                                                         /* 0x80018F38 */
Obj *Obj_FlipY(Obj *obj);                                                         /* 0x80018F4C */
Obj *Obj_FlipZ(Obj *obj);                                                         /* 0x80018F60 */
Obj *Obj_Invalidate(Obj *obj);                                                    /* 0x80018F74 */
Obj *Obj_Show(Obj *obj);                                                          /* 0x80018F88 */
Obj *Obj_Hide(Obj *obj);                                                          /* 0x80018FA0 */
Obj *Obj_SetUpdate(Obj *obj, ObjFunc fn, void *arg);                              /* 0x80018FB8 */
Obj *Obj_ClearUpdate(Obj *obj);                                                   /* 0x80018FC8 */
Obj *Obj_ClearTransform(Obj *obj);                                                /* 0x80018FD4 */
Obj *Obj_SetMatrix(Obj *obj, MtxF m);                                             /* 0x80018FE0 */
Obj *Obj_SetLights(Obj *obj, u8 *lights);                                         /* 0x80019010 */
void Obj_UpdateAll(void);                                                         /* 0x80019040 */
Obj *Obj_Create(ObjSceneNode *parent, s32 type);                                  /* 0x80019174 */
void Obj_FreeData(Obj *obj);                                                      /* 0x8001933C */
void Obj_FreeNode(Obj *obj);                                                      /* 0x800193C4 */
void Obj_Transform3D(Obj *obj);                                                   /* 0x800193F0 */
void Obj_Transform2D(Obj *obj);                                                   /* 0x80019520 */
void Obj_TransformView(Obj *obj);                                                 /* 0x800195F4 */
void Obj_TransformScreen(Obj *obj);                                               /* 0x80019688 */
void Obj_Attach(Obj *obj);                                                        /* 0x800196BC */
void Obj_Detach(Obj *obj);                                                        /* 0x800197C8 */
void Scene_Build(SceneObjDesc *objs, SceneAssetDesc *assets);                     /* 0x80019800 */
void Scene_Destroy(SceneObjDesc *objs, SceneAssetDesc *assets);                   /* 0x80019C9C */

#endif
