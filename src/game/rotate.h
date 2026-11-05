/* Vivid Dolls - view/projection matrices, rotations and matrix inversion
 * (RAM 0x80006C10-0x800088E0). Row-major f32[4][4], row vectors (p' = p * M).
 * "Pre" functions compute out = T * m, i.e. apply T before m; out may alias m.
 * Angles are in degrees unless the name says Rad / SinCos.
 * Bit-identical to the original (checked by tools/difftest/test_rotate.py).
 */
#ifndef VD_ROTATE_H
#define VD_ROTATE_H

#include "vd_common.h"
#include "vd_math.h"
#include "matrix.h"

void Mtx_LookAt(MtxF m, f32 eyeX, f32 eyeY, f32 eyeZ, f32 atX, f32 atY, f32 atZ,
                f32 upX, f32 upY, f32 upZ);                                  /* 0x80006C10 */
void Mtx_Perspective(MtxF m, f32 fovyDeg, f32 aspect, f32 near, f32 far);   /* 0x80006EA8 */
void Mtx_Ortho(MtxF m, f32 l, f32 r, f32 b, f32 t, f32 n, f32 f, f32 scale); /* 0x80006F74 */
void Mtx_RotateAxisSinCos(MtxF m, const Vec3f *axis, f32 s, f32 c);         /* 0x800070A4 */
void Mtx_RotateAxisDeg(MtxF m, f32 angleDeg, f32 x, f32 y, f32 z);          /* 0x800071CC */
s32  Mtx_Invert(MtxF out, const MtxF in);                                   /* 0x800073B0 */
void Mtx_InvertGaussJordan(MtxF out, const MtxF in);                        /* 0x80007420 */
s32  Mtx_InvertPermuted(MtxF out, const MtxF in);                           /* 0x80007558 */
void Vec3_RotateFromTo(Vec3f *out, const Vec3f *v, const Vec3f *from);      /* 0x8000793C */
s32  Mtx_RotateBetweenUnit(MtxF out, const Vec3f *a, const Vec3f *b);       /* 0x80007A08 */
s32  Mtx_RotateBetween(MtxF out, const Vec3f *a, const Vec3f *b);           /* 0x80007D28 */
s32  Mtx_BasisChange(MtxF out, const Vec3f *a, const Vec3f *b,
                     const Vec3f *c, const Vec3f *d);                       /* 0x80007D84 */
void Mtx_RotateAxisRad(MtxF m, const Vec3f *axis, f32 angle);               /* 0x80008190 */
void Mtx_PreRotateX(MtxF out, const MtxF m, f32 deg);                       /* 0x800082DC */
void Mtx_PreRotateY(MtxF out, const MtxF m, f32 deg);                       /* 0x800083A8 */
void Mtx_PreRotateZ(MtxF out, const MtxF m, f32 deg);                       /* 0x80008474 */
void Mtx_PreRotateYXZ(MtxF out, const MtxF m, f32 y, f32 x, f32 z);         /* 0x80008540 */
void Mtx_PreTranslate(MtxF out, const MtxF m, f32 x, f32 y, f32 z);         /* 0x800085B4 */
void Mtx_PreTranslateX(MtxF out, const MtxF m, f32 x);                      /* 0x80008624 */
void Mtx_PreTranslateY(MtxF out, const MtxF m, f32 y);                      /* 0x80008688 */
void Mtx_PreTranslateZ(MtxF out, const MtxF m, f32 z);                      /* 0x800086EC */
void Mtx_PreScale(MtxF out, const MtxF m, f32 x, f32 y, f32 z);             /* 0x80008750 */
void Mtx_PreScaleX(MtxF out, const MtxF m, f32 x);                          /* 0x800087BC */
void Mtx_PreScaleY(MtxF out, const MtxF m, f32 y);                          /* 0x80008818 */
void Mtx_PreScaleZ(MtxF out, const MtxF m, f32 z);                          /* 0x80008874 */
void Rotate_EmptyA(void);                                                   /* 0x800088D0 */
void Rotate_EmptyB(void);                                                   /* 0x800088D8 */

#endif
