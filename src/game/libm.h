/* Vivid Dolls - small float maths library (RAM 0x80003E10-0x80005FF0). */
#ifndef VD_LIBM_H
#define VD_LIBM_H
#include "vd_common.h"
#include "vd_math.h"

s32 vd_abs(s32 x);                      /* 0x80003E10 */
f32 vd_fabsf(f32 x);                    /* 0x80003E24 */
f32 vd_frexpf(f32 x, s32 *exp);         /* 0x80003E4C */
f32 vd_ldexpf(f32 x, s32 n);            /* 0x80003EBC */
f32 vd_expf(f32 x);                     /* 0x80003F08 */
f32 vd_expf_cf(f32 x);                  /* 0x80004034 */
f32 vd_logf(f32 x);                     /* 0x80004130 */
f32 vd_logf_cf(f32 x);                  /* 0x800042AC */
f32 vd_powif(f32 x, s32 n);             /* 0x80004434 */
f32 vd_powf(f32 x, f32 y);              /* 0x80004488 */
f32 vd_powf_series(f32 x, f32 y);       /* 0x8000468C */
f32 vd_atanf(f32 x);                    /* 0x80004708 */
f32 vd_atan2f_pos(f32 y, f32 x);        /* 0x80004838 */
f32 vd_tan_reduce(f32 x, s32 *quadrant);/* 0x80004A50 */
f32 vd_tanf(f32 x);                     /* 0x80004B14 */
f32 vd_sinf(f32 x);                     /* 0x80004C3C */
f32 vd_cosf(f32 x);                     /* 0x80004D44 */
f32 vd_sqrtf_newton(f32 x);             /* 0x80004D90 */
f32 vd_cbrtf(f32 x);                    /* 0x80004E04 */
s32 vd_csqrt(f32 re, f32 im, f32 out[4]);                  /* 0x80004EA4 */
s32 vd_ccbrt(f32 re, f32 im, f32 out[6]);                  /* 0x80004F80 */
s32 vd_cdiv(f32 a, f32 b, f32 c, f32 d, f32 *re, f32 *im); /* 0x8000512C */
void vd_mat3_swap_rows(f32 *m, s32 a, s32 b, s32 c);       /* 0x80005270 */
s32 vd_mat3_eliminate(f32 a[3][3], f32 inv[3][3], s32 col);  /* 0x80005360 */
s32 vd_mat3_invert(f32 a[3][3], f32 inv[3][3]);             /* 0x80005684 */
s32 vd_mtx_normalize_w(f32 m[4][4], Vec3f *t, f32 *w, s32 *pivot);  /* 0x80005870 */
s32 vd_mtx_invert(f32 out[4][4], f32 in[4][4]);          /* 0x80005A58 */
void vd_mtx_mul4(f32 out[4][4], f32 a[4][4], f32 b[4][4]); /* 0x80005FC0 */

/* libultra (0x8001C450 / 0x8001C5A0) */
f32 cosf(f32 x);
f32 sinf(f32 x);
f32 sqrtf(f32 x);
#endif
