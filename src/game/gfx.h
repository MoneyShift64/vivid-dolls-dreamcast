/* Vivid Dolls - frame rendering, video mode, viewports, cabinet I/O, controller
 * pak / EEPROM wrappers and a few geometry helpers (RAM 0x8000F3E0-0x80015AA0).
 * See docs/notes/gfx.md for the globals and structures.
 */
#ifndef VD_GFX_H
#define VD_GFX_H

#include "vd_common.h"
#include "vd_math.h"
#include "matrix.h"
#include "sprite.h"
#include "scene.h"

/* libultra OSContPad */
typedef struct PadData {
    u16 button;
    s8  stick_x;
    s8  stick_y;
    u8  errnum;
    u8  pad;
} PadData;

/* Per-frame graphics task (one per frame buffer). 0x10..0x4F is a libultra OSTask. */
typedef struct GfxTask {
    u32        unk00;           /* 0x00 set to 0 */
    u32        unk04;           /* 0x04 */
    u32        kind;            /* 0x08 0x63 for normal frames */
    u16       *fb_out;          /* 0x0C copy of fb */
    u32        type;            /* 0x10 OSTask.type = M_GFXTASK (1) */
    u32        flags;           /* 0x14 */
    void      *ucode_boot;      /* 0x18 */
    u32        ucode_boot_size; /* 0x1C */
    void      *ucode;           /* 0x20 */
    u32        ucode_size;      /* 0x24 */
    void      *ucode_data;      /* 0x28 */
    u32        ucode_data_size; /* 0x2C */
    void      *dram_stack;      /* 0x30 */
    u32        dram_stack_size; /* 0x34 */
    void      *output_buff;     /* 0x38 */
    void      *output_buff_end; /* 0x3C */
    SpriteGfx *data_ptr;        /* 0x40 display list */
    u32        data_size;       /* 0x44 */
    void      *yield_data;      /* 0x48 */
    u32        yield_data_size; /* 0x4C */
    void      *mq;              /* 0x50 reply queue (D_800C30E0) */
    void      *msg;             /* 0x54 -> msgbuf */
    s32        persp[16];       /* 0x58 perspective Mtx */
    s32        ortho[16];       /* 0x98 orthographic Mtx */
    u8         unkD8[0x40];     /* 0xD8 */
    u8         lookat[0x20];    /* 0x118 LookAt (x at 0x118, y at 0x128) */
    u8         unk138[0x20];    /* 0x138 */
    SpriteGfx  dl[0x400];       /* 0x158 */
    u8         msgbuf[0x20];    /* 0x2158 */
    u16       *fb;              /* 0x2178 colour frame buffer */
} GfxTask;

/* controller / serial interface */
void Pad_Read(void);                                                /* 0x8000F3E0 */
void Pad_ClearState(void);                                          /* 0x8000F6EC */
u16  Si_IsReleased(void);                                           /* 0x8000F750 */
void Si_Release(void);                                              /* 0x8000F75C */
void Si_Acquire(void);                                              /* 0x8000F7A4 */
/* cabinet I/O (Aleck64 registers at 0x02000000) */
void Coin_InitRates(void);                                          /* 0x8000F890 */
void Io_ReadButtons(void);                                          /* 0x8000F8F4 */
void Io_ReadDipSwitches(void);                                      /* 0x8000FA34 */
void Coin_Update(void);                                             /* 0x8000FA4C */
/* video / rendering */
void Video_Init(void);                                              /* 0x8000FDB0 */
void Gfx_RenderFrame(GfxTask *task);                                /* 0x8000FF64 */
SpriteGfx *Gfx_SetFullViewport(SpriteGfx *gfx);                     /* 0x800107D0 */
SpriteGfx *Gfx_SetViewport(SpriteGfx *gfx, s32 x, s32 y, s32 w, s32 h, s32 sx, s32 sy); /* 0x80010900 (all used as s16) */
void Video_ApplyMode(void);                                         /* 0x80010BF4 */
void Video_ApplyHStart(void);                                       /* 0x80010D28 */
void Video_ApplyVStart(void);                                       /* 0x80010DB0 */
/* full-screen quads (scene nodes with a fixed display list) */
void ScreenQuad_SetAlpha0(SceneNode *n, u8 a, f32 x, f32 y, f32 sx, f32 sy);   /* 0x80010EB0 */
void ScreenQuad_SetAlpha1(SceneNode *n, u8 a, f32 x, f32 y, f32 sx, f32 sy);   /* 0x80010F48 */
void ScreenQuad_SetColor2(SceneNode *n, u8 r, u8 g, u8 b, f32 x, f32 y, f32 sx, f32 sy); /* 0x80010FE0 */
void ScreenQuad_SetColorA2(SceneNode *n, u8 r, u8 g, u8 b, u8 a, f32 x, f32 y, f32 sx, f32 sy); /* 0x80011088 */
void ScreenQuad_SetColor3(SceneNode *n, u8 r, u8 g, u8 b, f32 x, f32 y, f32 sx, f32 sy); /* 0x80011140 */
void ScreenQuad_SetColorA3(SceneNode *n, u8 r, u8 g, u8 b, u8 a, f32 x, f32 y, f32 sx, f32 sy); /* 0x800111E8 */
void ScreenQuad_SetDL(SceneNode *n, SpriteGfx *dl, f32 x, f32 y, f32 sx, f32 sy); /* 0x800112A0 */

typedef struct NodeXform {
    f32 tx, ty, tz;     /* 0x00 */
    f32 sx, sy, sz;     /* 0x0C */
    f32 rx, ry, rz;     /* 0x18 degrees */
} NodeXform;
void ScreenQuad_SetDLXform(SceneNode *n, SpriteGfx *dl, const NodeXform *x); /* 0x80011334 */

/* controller pak (file on the pak) and EEPROM */
s32  Pfs_Init(void);                                                /* 0x80014E90 */
s32  Pfs_CreateFile(s32 channel, s32 length);                       /* 0x80014EC0 */
s32  Pfs_ReadFile(s32 channel, u8 *buf, s32 offset, s32 size);      /* 0x80014FC0 */
s32  Pfs_WriteFile(s32 channel, u8 *buf, s32 offset, s32 size);     /* 0x80015110 */
s32  Pfs_DeleteFile(s32 channel);                                   /* 0x80015260 */
u8   Pfs_GetPluggedMask(void);                                      /* 0x8001534C */
s32  Pfs_CheckFile(s32 channel);                                    /* 0x80015384 */
void Pfs_Nop(void);                                                 /* 0x80015474 */
s32  Pfs_HandleError(s32 err, void *pfs, const char *where);        /* 0x8001547C */
s32  Eeprom_Probe(void);                                            /* 0x800154D0 */
s32  Eeprom_Read(u8 *buf);                                          /* 0x80015524 */
s32  Eeprom_Write(u8 *buf);                                         /* 0x8001556C */

/* geometry */
f32  Line3_ProjectParam(const Vec3f *p, const Line3f *line);        /* 0x800155C0 */
f32  Segment3_DistanceToPoint(const Vec3f *p, const Vec3f *a, const Vec3f *b); /* 0x80015664 */
s32  Plane_ProjectPoint(Vec3f *out, const Vec3f *p, const Planef *plane);      /* 0x8001581C */
s32  Plane_ProjectPointD(f64 out[3], const Vec3f *p, const f64 plane[4]);      /* 0x800158D4 */
void Vec3_Cross(Vec3f *out, const Vec3f *a, const Vec3f *b);        /* 0x80015988 */

#endif
