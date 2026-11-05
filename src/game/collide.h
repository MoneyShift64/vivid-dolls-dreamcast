/* Vivid Dolls - geometry helpers, object pools and small leaf functions
 * (RAM 0x800088E0-0x8000A5A0, excluding the Siff code at 0x80009EC0-0x8000A090).
 *
 * Every function here was checked against the original MIPS code in an emulator
 * (tools/difftest/test_collide.py) and gives bit-identical results.
 */
#ifndef VD_COLLIDE_H
#define VD_COLLIDE_H

#include "vd_common.h"
#include "vd_math.h"

/* Stack pool: a NULL-terminated table of free item pointers, handed out from
 * the top. Built in place by Pool_Init inside a caller-supplied memory block:
 *   [0] top, [1] base, [2..] slot table (count+1 words), then the items (8-aligned). */
typedef struct StackPool {
    void **top;     /* next slot to hand out */
    void **base;    /* first slot */
    void *slots[1]; /* variable length, NULL-terminated */
} StackPool;

/* Ring pool: a circular NULL-terminated table of item pointers (FIFO).
 *   [0] base, [1] read, [2] stop (read limit, 0 = none), [3] write, [4..] slots, items. */
typedef struct RingPool {
    void **base;
    void **read;
    void **stop;
    void **write;
    void *slots[1];
} RingPool;

/* libultra-style LookAt (two directional "lights": right and up vectors). */
typedef struct VdLight {
    u8 col[3], pad1;
    u8 colc[3], pad2;
    s8 dir[3], pad3;
    u32 pad4;
} VdLight;
typedef struct VdLookAt { VdLight l[2]; } VdLookAt;

/* Input triangle for Mesh_BuildVertexBatches (only the low 16 bits of each
 * field are used for the triangle itself). */
typedef struct MeshTri {
    s32 v[3];       /* vertex indices */
    s32 group;      /* material/state group */
} MeshTri;

/* Triangle list node (heap) / list sentinel. */
typedef struct MeshNode {
    struct MeshNode *next;
    struct MeshNode *prev;
    s32 v[3];
    s32 group;
} MeshNode;

typedef struct MeshList {
    MeshNode head;  /* only next/prev used */
    MeshNode tail;  /* only next/prev used */
} MeshList;

void  Mtx_FromRowsNormalized(f32 out[4][4], const f32 in[4][4]);     /* 0x800088E0 */
s32   Mesh_BuildVertexBatches(u16 *out, s32 *order, const MeshTri *tris, s32 ntris); /* 0x80008A10 */
void  Pool_Init(StackPool *pool, u32 memSize, u32 itemSize);         /* 0x800094D0 */
void *Pool_Take(StackPool *pool);                                    /* 0x8000953C */
s32   Pool_CountFree(StackPool *pool);                               /* 0x80009564 */
void  Pool_Give(StackPool *pool, void *item);                        /* 0x80009590 */
void  Mtx_LookAtReflect(f32 mf[4][4], VdLookAt *l, const Vec3f *eye, const Vec3f *at,
                        f32 upX, f32 upY, f32 upZ);                  /* 0x800095B0 */
u16   Mtx_PerspectiveG(f32 m[4][4], f32 fovy, f32 aspect, f32 near, f32 far, f32 scale); /* 0x80009AD4 */
s32   Rand_Next(s32 *seed);                                          /* 0x80009CB0 */
u32   Rand_Range(s32 *seed, s32 range);                              /* 0x80009CE8 */
void  Ring_Init(RingPool *ring, u32 memSize, u32 itemSize);          /* 0x80009D40 */
void *Ring_Get(RingPool *ring);                                      /* 0x80009DB4 */
s32   Ring_Count(RingPool *ring);                                    /* 0x80009DF4 */
void  Ring_Put(RingPool *ring, void *item);                          /* 0x80009E2C */
void  Ring_SetStop(RingPool *ring);                                  /* 0x80009E6C */
void  Stub_80009E90(void);                                           /* 0x80009E90 */
void  Stub_80009E98(void);                                           /* 0x80009E98 */
void  Debug_Printf(const char *fmt, ...);                            /* 0x80009EA0 */
void  Stub_8000A090(void);                                           /* 0x8000A090 */
void  Mtx_RotateAxisSinCosG(f32 m[4][4], f32 s, f32 c, f32 x, f32 y, f32 z); /* 0x8000A0A0 */
void  Mtx_RotateBetweenG(f32 m[4][4], const Vec3f *a, const Vec3f *b);/* 0x8000A1F4 */
f32   BSpline2_Eval(f32 t, f32 p0, f32 p1, f32 p2);                  /* 0x8000A3D0 */
s32   BSpline2_Solve(f32 out[2], f32 x, f32 p0, f32 p1, f32 p2);     /* 0x8000A440 */
f32   BSpline2_Deriv(f32 t, f32 p0, f32 p1, f32 p2);                 /* 0x8000A574 */

#endif
