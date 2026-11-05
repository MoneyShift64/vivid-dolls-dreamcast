/* Vivid Dolls - maths and geometry library (ROM 0x1050-0x2E00, RAM 0x80000450-0x80002200).
 *
 * Decompiled to functionally equivalent C. Every function was checked against the
 * original MIPS code in an emulator (tools/difftest) and gives bit-identical
 * results. Keep the expression order when editing: floating-point results depend on it.
 */
#ifndef VD_MATH_H
#define VD_MATH_H

#include "vd_common.h"

typedef struct { f32 x, y; } Vec2f;
typedef struct { f32 x, y, z; } Vec3f;

/* Plane: a*x + b*y + c*z + d = 0, (a, b, c) = normal */
typedef struct { f32 a, b, c, d; } Planef;

/* 3D line: point + direction (not normalised) */
typedef struct { Vec3f point; Vec3f dir; } Line3f;

/* 2D ray: point + direction */
typedef struct { f32 x, y, dx, dy; } Ray2f;

/* 2D line equation: a*x + b*y + c = 0 */
typedef struct { f32 a, b, c; } Line2f;

#define VD_EPSILON 1e-16f
/* Binary angles: 0x400 units = one full turn */
#define VD_ANGLE_FULL 0x400

f32  Math_Atan2f(f32 y, f32 x);                                           /* 0x80000450 */
f32  Math_Atanf(f32 x);                                                   /* 0x80000610 */
s32  Math_VecToAngle(f32 x, f32 y);                                       /* 0x80000634 */

void Plane_FromPoints(Planef *out, const Vec3f *a, const Vec3f *b, const Vec3f *c);   /* 0x800010A0 */
s32  Plane_FromTriangle(Planef *out, const Vec3f tri[3]);                 /* 0x80001144 */
s32  Plane_IntersectLine(Vec3f *out, const Line3f *line, const Planef *plane);        /* 0x800011EC */
s32  Line3_FromPlanes(Line3f *out, const Planef *p, const Planef *q);     /* 0x800012E4 */
s32  Line3_FromPoints(Line3f *out, const Vec3f *a, const Vec3f *b);       /* 0x800014DC */
f32  Line3_DistanceToPoint(const Vec3f *p, const Line3f *line);           /* 0x8000152C */
f32  Vec3_Distance(const Vec3f *a, const Vec3f *b);                       /* 0x80001648 */
f32  Vec3_Length(const Vec3f *v);                                         /* 0x800016B0 */
f32  Vec3_Dot(const Vec3f *a, const Vec3f *b);                            /* 0x80001700 */
f32  Vec3_CosAngle(const Vec3f *a, const Vec3f *b);                       /* 0x80001730 */
s32  Vec3_TriangleNormal(Vec3f *out, const Vec3f *a, const Vec3f *b, const Vec3f *c); /* 0x800017FC */
void Vec3_NormalizeXYZ(f32 *x, f32 *y, f32 *z);                           /* 0x8000192C */
void Vec3_Normalize(Vec3f *out, const Vec3f *in);                         /* 0x800019D4 */
s32  Line2_FromPoints(Line2f *out, const Vec2f *a, const Vec2f *b);       /* 0x80001A70 */
s32  Line2_Intersect(Vec2f *out, const Line2f *l, const Line2f *m);       /* 0x80001B70 */
f32  Math_Cross2(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2);         /* 0x80001C2C */
s32  Ray2_IntersectCircle(Vec2f out[2], const Ray2f *ray, const Vec2f *centre, f32 radius); /* 0x80001C64 */
s32  Ray2_PerpendicularPoints(Vec2f out[2], const Ray2f *ray, f32 dist);  /* 0x80001E1C */
s32  Segment2_Offset(Vec2f *left, Vec2f *right, f32 x0, f32 y0, f32 x1, f32 y1, f32 dist); /* 0x80001F4C */
f32  Ray2_DistanceSqToPoint(const Ray2f *ray, f32 x, f32 y);              /* 0x800020A4 */
f32  Vec2_CosAngle(const Vec2f *a, const Vec2f *b);                       /* 0x80002144 */
void Line2_FromPointNormal(Line2f *out, const Vec2f *p, const Vec2f *n);  /* 0x800021BC */

#endif
