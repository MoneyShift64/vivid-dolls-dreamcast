/* Vivid Dolls port - libultra libc/libm parts and the remaining small calls.
 *
 * sinf/cosf are libultra's own algorithms (double-precision polynomial after a
 * pi reduction; the constants are the libultra ones), so the game computes the
 * same angles as on the original hardware. Build with -ffp-contract=off so no
 * multiply-add is fused (the VR4300 has none).
 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "ultra_internal.h"

typedef union { struct { u32 hi, lo; } w; f64 d; } vd_du;
typedef union { u32 i; f32 f; } vd_fu;

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define DU(h, l) { .w = { (l), (h) } }
#else
#define DU(h, l) { .w = { (h), (l) } }
#endif

static const vd_du P[5] = {
    DU(0x3ff00000, 0x00000000),
    DU(0xbfc55554, 0xbc83656d),
    DU(0x3f8110ed, 0x3804c2a0),
    DU(0xbf29f6ff, 0xeea56814),
    DU(0x3ec5dbdf, 0x0e314bfe),
};
static const vd_du rpi  = DU(0x3fd45f30, 0x6dc9c883);   /* 1/pi */
static const vd_du pihi = DU(0x400921fb, 0x50000000);
static const vd_du pilo = DU(0x3e6110b4, 0x611a6263);
static const vd_du half = DU(0x3fe00000, 0x00000000);
static const vd_fu qnan = { 0x7F810000 };                 /* __libm_qnan_f */

static s32 vd_fbits(f32 x)
{
    vd_fu u;
    u.f = x;
    return (s32)u.i;
}

f32 sinf(f32 x)
{
    f64 dx, xsq, poly, dn, result;
    s32 n, xpt = (vd_fbits(x) >> 22) & 0x1FF;

    if (xpt < 255) {
        if (xpt >= 230) {
            dx = x;
            xsq = dx * dx;
            poly = ((P[4].d * xsq + P[3].d) * xsq + P[2].d) * xsq + P[1].d;
            result = dx + (dx * xsq) * poly;
            return (f32)result;
        }
        return x;
    }
    if (xpt < 310) {
        dx = x;
        dn = dx * rpi.d;
        if (dn >= 0.0)
            n = (s32)(dn + half.d);
        else
            n = (s32)(dn - half.d);
        dn = (f64)n;
        dx -= dn * pihi.d;
        dx -= dn * pilo.d;
        xsq = dx * dx;
        poly = ((P[4].d * xsq + P[3].d) * xsq + P[2].d) * xsq + P[1].d;
        result = dx + (dx * xsq) * poly;
        if ((n & 1) == 0)
            return (f32)result;
        return -(f32)result;
    }
    if (x != x)
        return qnan.f;
    return 0.0f;
}

f32 cosf(f32 x)
{
    f64 dx, xsq, poly, dn, result;
    s32 n, xpt = (vd_fbits(x) >> 22) & 0x1FF;

    if (xpt < 310) {
        dx = (0.0f < x) ? x : -x;
        dn = dx * rpi.d + half.d;
        if (dn >= 0.0)
            n = (s32)(dn + half.d);
        else
            n = (s32)(dn - half.d);
        dn = (f64)n;
        dn -= half.d;
        dx -= dn * pihi.d;
        dx -= dn * pilo.d;
        xsq = dx * dx;
        poly = ((P[4].d * xsq + P[3].d) * xsq + P[2].d) * xsq + P[1].d;
        result = dx + (dx * xsq) * poly;
        if ((n & 1) == 0)
            return (f32)result;
        return -(f32)result;
    }
    if (x != x)
        return qnan.f;
    return 0.0f;
}

/* sqrt.s on the VR4300, fsqrt on the SH4: both IEEE correctly rounded. */
f32 sqrtf(f32 x)
{
#if defined(__SH4__) || defined(__SH4_SINGLE__) || defined(__sh__)
    __asm__("fsqrt %0" : "+f"(x));
    return x;
#else
    return __builtin_sqrtf(x);
#endif
}

f32 fabsf(f32 x)
{
    return __builtin_fabsf(x);
}

/* bcopy / bzero: newlib's (same semantics as libultra's). */

/* libultra sprintf (only %d/%s/%x style formats are used by the game) */
s32 vd_ultra_sprintf(char *buf, const char *fmt, ...)
{
    va_list ap;
    int r;

    va_start(ap, fmt);
    r = vsprintf(buf, fmt, ap);
    va_end(ap);
    return r;
}

/* osInitialize: the shim is set up by port/sys/main.c before Main_Boot. */
void osInitialize(void)
{
}

/* No CPU exceptions are reported to the game: the fault thread waits forever. */
void *__osGetCurrFaultedThread(void)
{
    return NULL;
}

/* Only referenced by Main_ForceLink (never called). */
void vd_ultra_unused(void)
{
}

VD_ALIAS(func_80025DC0, vd_ultra_sprintf);
VD_ALIAS(func_8001CAC0, osInitialize);
VD_ALIAS(func_8001D340, __osGetCurrFaultedThread);
VD_ALIAS(func_8001D350, vd_ultra_unused);
VD_ALIAS(func_80021EA0, vd_ultra_unused);
VD_ALIAS(func_80024750, vd_ultra_unused);
VD_ALIAS(func_80023F90, vd_ultra_unused);
VD_ALIAS(func_800221A0, vd_ultra_unused);
