/* Vivid Dolls - 4x4 float matrices (RAM 0x80006038-0x80007940).
 * Row-major, row vectors: p' = p * M, translation in m[3][0..2] (N64 style).
 * Bit-identical to the original (checked in tools/difftest).
 */
#ifndef VD_MATRIX_H
#define VD_MATRIX_H

#include "vd_common.h"
#include "vd_math.h"

typedef f32 MtxF[4][4];

void Mtx_Mul(MtxF out, const MtxF a, const MtxF b);              /* 0x80006038 out = a * b (out may alias) */
void Mtx_MulAffine(MtxF out, const MtxF a, const MtxF b);        /* 0x80006778 3x3 product, translations added */
void Mtx_TransformPoint(Vec3f *out, const Vec3f *p, const MtxF m);   /* 0x80006958 */
void Mtx_TransformVector(Vec3f *out, const Vec3f *v, const MtxF m);  /* 0x800069EC */
s32  Mtx_ProjectOntoPlane(MtxF out, const Planef *plane);        /* 0x80006A68 */
void Mtx_Translate(MtxF m, f32 x, f32 y, f32 z);                 /* 0x80007308 */
void Mtx_Scale(MtxF m, f32 x, f32 y, f32 z);                     /* 0x8000735C */
void Mtx_Identity(MtxF m);                                       /* 0x800078C4 */
void Mtx_Copy(MtxF dst, const MtxF src);                         /* 0x80007918 */

#endif
