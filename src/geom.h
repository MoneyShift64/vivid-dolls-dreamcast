/* Vivid Dolls - integer 2D geometry, mesh helpers, triangle hit tests, look-at matrix
 * and radix sorts (RAM 0x80002200-0x80003300 and 0x80003650-0x80003E10).
 *
 * Decompiled to functionally equivalent C; every function is checked bit-for-bit
 * against the original MIPS code by tools/difftest/test_geom.py.
 * Integer maths wraps at 32 bits like the original (done with u32 arithmetic).
 */
#ifndef VD_GEOM_H
#define VD_GEOM_H

#include "vd_common.h"
#include "vd_math.h"
#include "matrix.h"

typedef struct { s32 x, y; } Vec2i;
typedef struct { s32 x, y, z; } Vec3i;

/* Vertex of an integer height-field mesh (0x14 bytes): 2D position + 3 attributes
 * that Mesh_InterpolateAt interpolates (e.g. height and two more values). */
typedef struct { s32 x, y; s32 attr[3]; } MeshVertex;

/* Doubly linked list node used by Geom_GreedyTriangulate (0x0C bytes). */
typedef struct GeomLink {
    struct GeomLink *next;      /* 0x00 */
    struct GeomLink *prev;      /* 0x04 */
    s32 other;                  /* 0x08 index of the vertex at the other end of the edge */
} GeomLink;

/* Per-vertex edge list: head and tail sentinels (0x18 bytes). */
typedef struct { GeomLink head, tail; } GeomVertList;
/* Per-edge pair of nodes: a sits in the first vertex's list, b in the second's (0x18 bytes). */
typedef struct { GeomLink a, b; } GeomEdgeLinks;

s32  Geom_Cross2i(const Vec2i *a, const Vec2i *b, const Vec2i *c);                     /* 0x80002200 */
s32  Seg2i_Intersect(const Vec2i *a, const Vec2i *b, const Vec2i *c, const Vec2i *d);  /* 0x80002248 */
s32  Tri3i_InterpolateZ(const Vec2i *p, const Vec3i tri[3]);                           /* 0x80002434 */
s32  Mesh_InterpolateAt(s32 out[3], const Vec2i *p, const s32 (*tris)[3],
                        const MeshVertex *verts, s32 ntris);                           /* 0x80002558 */
s32  Geom_GreedyTriangulate(s32 (*tris)[3], const Vec2i *pts, s32 n);                  /* 0x80002990 */
s32  Tri_IntersectLine(Vec3f *out, const Line3f *line, const Vec3f tri[3]);            /* 0x80002E70 */
s32  Quad_HitTest(Vec3f *out, const Line3f *line, const Vec3f quad[4]);                /* 0x80002FA0 */
s32  Tri_HitTest(Vec3f *out, const Line3f *line, const Vec3f tri[3]);                  /* 0x8000306C */
void Mtx_LookAtV(MtxF m, const Vec3f *eye, const Vec3f *at, f32 upx, f32 upy, f32 upz); /* 0x80003650 */
void RadixSort_Ptrs(u8 **items, s32 n, s32 keyofs, s32 keylen, s32 desc);              /* 0x800039F0 */
void RadixSort_Records(void *data, s32 n, s32 recsize, s32 keyofs, s32 keylen, s32 desc); /* 0x80003C4C */

#endif
