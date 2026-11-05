/* Vivid Dolls - small float maths library linked into the game
 * (RAM 0x80003E10-0x80005FF0). Series-based, single precision.
 * Bit-identical to the original (tools/difftest/test_funcs.py).
 */
#include "libm.h"

#define LN2 0.6931472f      /* 0x3F317218 */

static inline u32 fbits(f32 f) { union { f32 f; u32 u; } v; v.f = f; return v.u; }
static inline f32 bitsf(u32 u) { union { f32 f; u32 u; } v; v.u = u; return v.f; }

/* x * 2^n by repeated squaring (shared by several functions below) */
static inline f32 scale2(f32 x, s32 n)
{
    f32 f;
    if (n >= 0) {
        f = 2.0f;
    } else {
        f = 0.5f;
        n = -n;
    }
    while (n != 0) {
        if (n & 1)
            x *= f;
        n >>= 1;
        f *= f;
    }
    return x;
}

/* @ 0x80003E10 */
s32 vd_abs(s32 x)
{
    return x < 0 ? -x : x;
}

/* @ 0x80003E24 */
f32 vd_fabsf(f32 x)
{
    if (!(x >= 0.0f))
        x = -x;
    return x;
}

/* @ 0x80003E4C  mantissa in [0.5, 1) and exponent; 0 gives 0; infinity gives exp 0x401 */
f32 vd_frexpf(f32 x, s32 *exp)
{
    u32 e;
    *exp = 0;
    e = (fbits(x) >> 23) & 0xFF;
    if (e == 0xFF) {
        if (fbits(x) & 0x7FFFFF)
            return x;                   /* NaN */
        *exp = 0x401;
    } else if (e != 0) {
        *exp = (s32)e - 0x7E;
    } else {
        return 0.0f;
    }
    return bitsf((fbits(x) & 0x807FFFFF) | 0x3F000000);
}

/* @ 0x80003EBC */
f32 vd_ldexpf(f32 x, s32 n)
{
    return scale2(x, n);
}

/* @ 0x80003F08  exp by Taylor series after range reduction */
f32 vd_expf(f32 x)
{
    s32 k = (s32)((x / LN2) + (x >= 0.0f ? 0.5f : -0.5f));
    f32 r = x - ((f32)k * LN2);
    s32 neg = 0, n = 2;
    f32 term, sum, prev;

    if (!(r > 0.0f)) {
        neg = 1;
        r = -r;
    }
    term = r;
    sum = r + 1.0f;
    do {
        term *= r / (f32)n;
        prev = sum;
        sum += term;
        n++;
    } while (sum != prev);
    if (neg)
        sum = 1.0f / sum;
    return scale2(sum, k);
}

/* @ 0x80004034  exp by continued fraction */
f32 vd_expf_cf(f32 x)
{
    s32 k = (s32)((x / LN2) + (x >= 0.0f ? 0.5f : -0.5f));
    f32 r = x - ((f32)k * LN2);
    f32 r2 = r * r;
    f32 f = r2 / (f32)22;
    s32 v = 18;
    f32 d;

    for (;;) {
        f32 t = f + (f32)v;
        v -= 4;
        f = r2 / t;
        if (v < 6)
            break;
    }
    d = f + 2.0f;
    return scale2((d + r) / (d - r), k);
}

/* @ 0x80004130  natural log (0 for x <= 0) */
f32 vd_logf(f32 x)
{
    s32 e, n;
    f32 y, z, z2, t, sum, prev, p, f;

    if (x <= 0.0f)
        return 0.0f;
    y = x / 1.41421354f;                /* 0x3FB504F3 */
    vd_frexpf(y, &e);
    /* x / 2^e */
    p = 1.0f;
    n = e;
    if (n < 0) {
        f = 0.5f;
        n = -n;
    } else {
        f = 2.0f;
    }
    while (n != 0) {
        if (n & 1)
            p *= f;
        n >>= 1;
        f *= f;
    }
    x = x / p;

    z = (x - 1.0f) / (x + 1.0f);
    sum = z;
    z2 = z * z;
    t = z * z2;
    n = 1;
    for (;;) {
        n += 2;
        prev = sum;
        sum += t / (f32)n;
        if (prev == sum)
            break;
        t *= z2;
    }
    return ((f32)e * LN2) + (sum + sum);
}

/* @ 0x800042AC  natural log by continued fraction (0 for x <= 0) */
f32 vd_logf_cf(f32 x)
{
    s32 e, n;
    f32 p, f, z, acc;

    if (x <= 0.0f)
        return 0.0f;
    vd_frexpf(x / 1.4142135f, &e);
    p = 1.0f;
    n = e;
    if (n < 0) {
        n = -n;
        f = 0.5f;
    } else {
        f = 2.0f;
    }
    while (n != 0) {
        if (n & 1)
            p *= f;
        n >>= 1;
        f *= f;
    }
    z = (x / p) - 1.0f;
    acc = 0.0f;
    for (n = 9; n > 0; n--) {
        f32 k = (f32)n;
        f32 kz = k * z;
        f32 d = (kz / ((2.0f * k) + 1.0f + acc)) + 2.0f;
        acc = kz / d;
    }
    return ((f32)e * LN2) + (z / (acc + 1.0f));
}

/* @ 0x80004434  x to an integer power */
f32 vd_powif(f32 x, s32 n)
{
    s32 m = n < 0 ? -n : n;
    f32 r = 1.0f;
    while (m != 0) {
        if (m & 1)
            r *= x;
        m >>= 1;
        x *= x;
    }
    if (n < 0)
        r = 1.0f / r;
    return r;
}

/* @ 0x80004488  x^y: exact repeated multiplication for whole y, otherwise
 * exp(y * log(x)) with the continued-fraction versions (0 if x <= 0) */
f32 vd_powf(f32 x, f32 y)
{
    if (y <= 2.1474836e9f && y >= -2.1474836e9f) {
        s32 n = (s32)y;
        if (y == (f32)n) {
            s32 m = n < 0 ? -n : n;
            f32 r = 1.0f;
            while (m != 0) {
                if (m & 1)
                    r *= x;
                m >>= 1;
                x *= x;
            }
            return n >= 0 ? r : 1.0f / r;
        }
    }
    if (x > 0.0f) {
        f32 t = y * vd_logf_cf(x);
        s32 k = (s32)((t / LN2) + (t >= 0.0f ? 0.5f : -0.5f));
        f32 r = t - ((f32)k * LN2);
        f32 r2 = r * r;
        f32 f = r2 / (f32)22;
        s32 v = 18;
        f32 d;
        do {
            f32 s = f + (f32)v;
            v -= 4;
            f = r2 / s;
        } while (v >= 6);
        d = f + 2.0f;
        return scale2((d + r) / (d - r), k);
    }
    return 0.0f;
}

/* @ 0x8000468C  x^y by the binomial series of (1 + (x - 1))^y, 21 terms.
 * The factorial is an int and overflows exactly like the original. */
f32 vd_powf_series(f32 x, f32 y)
{
    u32 fact = 1;
    s32 k = 1;
    f32 sum = 1.0f, coef = 1.0f, pw = 1.0f;
    do {
        pw *= x - 1.0f;
        coef *= (y - (f32)k) + 1.0f;
        fact *= (u32)k;
        k++;
        sum += (coef * pw) / (f32)(s32)fact;
    } while (k < 0x16);
    return sum;
}

/* @ 0x80004708  atan by continued fraction */
f32 vd_atanf(f32 x)
{
    s32 side = 1, n;
    f32 acc = 0.0f;

    if (x > 1.0f) {
        x = 1.0f / x;
    } else {
        side = -1;
        if (x < -1.0f)
            x = 1.0f / x;
        else
            side = 0;
    }
    for (n = 0x16; n > 0; n--) {
        f32 num = (f32)(n * n) * x * x;
        f32 den = (2.0f * (f32)n) + 1.0f + acc;
        acc = num / den;
    }
    if (side > 0)
        return 1.5707964f - (x / (acc + 1.0f));
    if (side >= 0)
        return x / (acc + 1.0f);
    return -1.5707964f - (x / (acc + 1.0f));
}

/* @ 0x80004838  atan2(y, x) in [0, 2pi) for x >= 0 cases, (pi/2, 3pi/2) for x < 0 */
f32 vd_atan2f_pos(f32 y, f32 x)
{
    f32 r;
    if (x == 0.0f) {
        if (y == 0.0f)
            return 0.0f;
        return y > 0.0f ? 1.5707964f : -1.5707964f;
    }
    if (y == 0.0f)
        return x > 0.0f ? 0.0f : 3.1415927f;
    r = vd_atanf(y / x);
    if (x >= 0.0f)
        return (y >= 0.0f) ? r : r + 6.2831855f;
    return r + 3.1415927f;
}

/* tan by continued fraction after reduction by pi/2 (two-part constant) */
static inline f32 tan_reduced(f32 x, s32 *quadrant)
{
    f32 k, r, f = 0.0f;
    s32 v;
    *quadrant = (s32)((x / 1.5707964f) + (x >= 0.0f ? 0.5f : -0.5f));
    k = (f32)*quadrant;
    r = (x - (k * 1.5708008f)) + (k * 0.000004454455f);
    for (v = 0x13; v >= 3; v -= 2) {
        f32 t = (f32)v - f;
        f = (r * r) / t;
    }
    return r / (1.0f - f);
}

/* @ 0x80004A50  tan of x reduced to [-pi/4, pi/4], plus the quadrant */
f32 vd_tan_reduce(f32 x, s32 *quadrant)
{
    return tan_reduced(x, quadrant);
}

/* @ 0x80004B14 */
f32 vd_tanf(f32 x)
{
    s32 q;
    f32 t = tan_reduced(x, &q);
    if (!(q & 1))
        return t;
    if (t == 0.0f)
        return 1e16f;
    return -1.0f / t;
}

/* @ 0x80004C3C  sin via the half-angle tangent */
f32 vd_sinf(f32 x)
{
    s32 q;
    f32 t = tan_reduced(x / 2.0f, &q);
    f32 s = (2.0f * t) / ((t * t) + 1.0f);
    return (q & 1) ? -s : s;
}

/* @ 0x80004D44  cos(x) = sin(pi/2 - |x|), using libultra's sinf */
f32 vd_cosf(f32 x)
{
    return sinf(1.5707964f - (x >= 0.0f ? x : -x));
}

/* @ 0x80004D90  square root by Newton's method (returns the last estimate) */
f32 vd_sqrtf_newton(f32 x)
{
    f32 g, prev;
    if (!(x > 0.0f))
        return 0.0f;
    g = x > 1.0f ? x : 1.0f;
    do {
        prev = g;
        g = ((x / g) + g) / 2.0f;
    } while (g < prev);
    return prev;
}

static inline f32 cbrt_newton(f32 x)
{
    s32 pos = 1;
    f32 g, prev;
    if (x == 0.0f)
        return 0.0f;
    if (!(x > 0.0f)) {
        pos = 0;
        x = -x;
    }
    g = x > 1.0f ? x : 1.0f;
    do {
        prev = g;
        g = ((x / (prev * prev)) + (2.0f * prev)) / 3.0f;
    } while (g < prev);
    return pos ? prev : -prev;
}

/* @ 0x80004E04  cube root by Newton's method */
f32 vd_cbrtf(f32 x)
{
    return cbrt_newton(x);
}

/* @ 0x80004EA4  both square roots of the complex number re + i*im:
 * out = { re0, im0, -re0, -im0 }; 0 if re + |z| == 0 */
s32 vd_csqrt(f32 re, f32 im, f32 out[4])
{
    f32 m = sqrtf((re * re) + (im * im));
    f32 a = re + m, s;
    if (a == 0.0f)
        return 0;
    s = sqrtf(a);
    out[0] = s / 1.4142135f;
    out[1] = im / (s * 1.4142135f);
    out[2] = -out[0];
    out[3] = -out[1];
    return 1;
}

/* @ 0x80004F80  the three complex cube roots of re + i*im: out = 3 x {re, im} */
s32 vd_ccbrt(f32 re, f32 im, f32 out[6])
{
    f32 m = sqrtf((re * re) + (im * im));
    f32 ang = vd_atan2f_pos(im, re);
    f32 r = (m == 0.0f) ? 0.0f : cbrt_newton(m);
    s32 i;
    for (i = 0; i < 3; i++) {
        f32 a = (((f32)i * 6.2831855f) + ang) / 3.0f;
        out[i * 2 + 0] = r * cosf(a);
        out[i * 2 + 1] = r * sinf(a);
    }
    return 1;
}

/* @ 0x8000512C  (a + ib) / (c + id) in polar form; 0 if |a+ib|/|c+id| <= 1e-16 */
s32 vd_cdiv(f32 a, f32 b, f32 c, f32 d, f32 *out_re, f32 *out_im)
{
    f32 r = sqrtf((a * a) + (b * b)) / sqrtf((c * c) + (d * d));
    f32 ang;
    if (r <= 1e-16f)
        return 0;
    ang = vd_atan2f_pos(b, a);
    ang = ang - vd_atan2f_pos(d, c);
    *out_re = r * cosf(ang);
    *out_im = r * sinf(ang);
    return 1;
}

/* @ 0x80005270  row swaps for the 3x3 solver (rows are 3 floats) */
void vd_mat3_swap_rows(f32 *m, s32 a, s32 b, s32 c)
{
    s32 i;
    if (a != 0) {
        if (b == 0) {
            for (i = 0; i < 3; i++) {
                f32 t = m[i];
                m[i] = m[a * 3 + i];
                m[a * 3 + i] = t;
            }
            b = a;
        } else {
            for (i = 0; i < 3; i++) {
                f32 t = m[c * 3 + i];
                m[c * 3 + i] = m[a * 3 + i];
                m[a * 3 + i] = t;
            }
            c = a;
        }
    }
    if (b != 1) {
        for (i = 0; i < 3; i++) {
            f32 t = m[c * 3 + i];
            m[c * 3 + i] = m[b * 3 + i];
            m[b * 3 + i] = t;
        }
    }
}

/* ---- 3x3 inverse by Gauss-Jordan elimination (rows of 3 floats) ---- */

/* @ 0x80005360  eliminates columns 1 and 2 after column 0 used pivot row `col` */
s32 vd_mat3_eliminate(f32 a[3][3], f32 inv[3][3], s32 col)
{
    s32 r1, r2, p, i;
    f32 x1, x2, piv, f;

    if (col == 0) { r1 = 1; r2 = 2; }
    else if (col == 1) { r1 = 0; r2 = 2; }
    else { r1 = 0; r2 = 1; }

    x1 = a[r1][1];
    if (x1 < 0.0f) x1 = -x1;
    x2 = a[r2][1];
    if (x2 < 0.0f) x2 = -x2;
    p = r2;
    if (x2 < x1) p = r1;
    if (p == r1) r1 = r2;

    if ((f64)a[p][1] < 1e-08 && (f64)a[p][1] > -1e-08)
        return 0;
    piv = 1.0f / a[p][1];
    f = a[p][2] * piv;
    a[p][1] = 1.0f;
    a[p][2] = f;
    inv[p][p] = piv;
    inv[p][col] = inv[p][col] * piv;
    for (i = 0; i < 3; i++) {
        if (i != p) {
            f32 nf = -a[i][1];
            a[i][1] = 0.0f;
            a[i][2] = a[i][2] + (nf * a[p][2]);
            inv[i][p] = nf * inv[p][p];
            inv[i][col] += nf * inv[p][col];
        }
    }

    if ((f64)a[r1][2] < 1e-08 && (f64)a[r1][2] > -1e-08)
        return 0;
    piv = 1.0f / a[r1][2];
    a[r1][2] = 1.0f;
    inv[r1][r1] = piv;
    inv[r1][col] *= piv;
    inv[r1][p] = inv[r1][p] * piv;
    for (i = 0; i < 3; i++) {
        if (i != r1) {
            f32 nf = -a[i][2];
            a[i][2] = 0.0f;
            inv[i][col] += nf * inv[r1][col];
            inv[i][p] += nf * inv[r1][p];
            inv[i][r1] += nf * inv[r1][r1];
        }
    }
    vd_mat3_swap_rows(&inv[0][0], col, p, r1);
    return 1;
}

/* @ 0x80005684  inv = a^-1 (a is destroyed); 0 if a is (nearly) singular */
s32 vd_mat3_invert(f32 a[3][3], f32 inv[3][3])
{
    f32 x0, x1, x2, piv;
    s32 p, i;

    inv[2][1] = 0.0f; inv[2][0] = 0.0f; inv[1][2] = 0.0f;
    inv[1][0] = 0.0f; inv[0][2] = 0.0f; inv[0][1] = 0.0f;
    inv[2][2] = 1.0f; inv[1][1] = 1.0f; inv[0][0] = 1.0f;

    x0 = a[0][0]; if (x0 < 0.0f) x0 = -x0;
    x1 = a[1][0]; if (x1 < 0.0f) x1 = -x1;
    x2 = a[2][0]; if (x2 < 0.0f) x2 = -x2;
    if (x1 < x0)
        p = (x2 < x0) ? 0 : 2;
    else
        p = (x2 < x1) ? 1 : 2;

    if ((f64)a[p][0] < 1e-08 && (f64)a[p][0] > -1e-08)
        return 0;
    piv = 1.0f / a[p][0];
    a[p][0] = 1.0f;
    a[p][1] = a[p][1] * piv;
    a[p][2] = a[p][2] * piv;
    inv[p][p] = piv;
    for (i = 0; i < 3; i++) {
        if (i != p) {
            f32 nf = -a[i][0];
            a[i][0] = 0.0f;
            a[i][1] = a[i][1] + (nf * a[p][1]);
            a[i][2] = a[i][2] + (nf * a[p][2]);
            inv[i][p] = nf * inv[p][p];
        }
    }
    return vd_mat3_eliminate(a, inv, p) != 0;
}

/* @ 0x80005870  first step of a 4x4 inverse: make m[3][3] = 1 and clear column 3.
 * If m[3][3] is ~0, row 3 is swapped with the row whose column-3 entry is
 * largest (*pivot = that row, else -1). Returns 0 if no pivot exists. */
s32 vd_mtx_normalize_w(f32 m[4][4], Vec3f *t, f32 *w, s32 *pivot)
{
    f32 a = m[3][3], best = 0.0f;
    s32 i, k;

    *pivot = -1;
    if (!(a >= 0.0f))
        a = -a;
    if ((f64)a < 1e-08) {
        for (i = 0; i < 4; i++) {
            f32 v = m[i][3];
            if (!(best < v)) {
                if (v < -best) {
                    *pivot = i;
                    best = -m[i][3];
                }
            } else {
                *pivot = i;
                best = m[i][3];
            }
        }
        if (*pivot < 0)
            return 0;
        for (k = 0; k < 4; k++) {
            f32 tmp = m[3][k];
            m[3][k] = m[*pivot][k];
            m[*pivot][k] = tmp;
        }
    }
    t->x = -m[0][3];
    t->y = -m[1][3];
    t->z = -m[2][3];
    *w = 1.0f / m[3][3];
    m[2][3] = 0.0f;
    m[1][3] = 0.0f;
    m[0][3] = 0.0f;
    m[3][3] = 1.0f;
    m[3][0] = m[3][0] * *w;
    m[3][1] = m[3][1] * *w;
    m[3][2] = m[3][2] * *w;
    for (k = 0; k < 3; k++) {
        m[0][k] += t->x * m[3][k];
        m[1][k] = m[1][k] + (t->y * m[3][k]);
        m[2][k] = m[2][k] + (t->z * m[3][k]);
    }
    return 1;
}

/* @ 0x80005A58
 * General 4x4 inverse built from the 3x3 inverse. Affine matrices (column 3 =
 * 0,0,0,1) skip the projective normalisation step. Returns 0 if singular. */
s32 vd_mtx_invert(f32 out[4][4], f32 in[4][4]) {
    f32 t[4][4];
    f32 m3[3][3];
    f32 inv3[3][3];
    Vec3f v;
    f32 tr[3];
    f32 w;
    s32 pivot;
    s32 affine;
    s32 i, j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            t[i][j] = in[i][j];
            out[i][j] = 0.0f;
        }
    }
    out[0][0] = 1.0f;
    out[1][1] = 1.0f;
    out[2][2] = 1.0f;
    out[3][3] = 1.0f;

    affine = 0;
    if (t[0][3] == 0.0f && t[1][3] == 0.0f && t[2][3] == 0.0f && t[3][3] == 1.0f) {
        affine = 1;
    }
    if (!affine && !vd_mtx_normalize_w(t, &v, &w, &pivot)) {
        return 0;
    }
    tr[0] = t[3][0];
    tr[1] = t[3][1];
    tr[2] = t[3][2];
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            m3[i][j] = t[i][j];
        }
    }
    if (!vd_mat3_invert(m3, inv3)) {
        return 0;
    }
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            out[i][j] = inv3[i][j];
        }
    }
    for (j = 0; j < 3; j++) {
        for (i = 0; i < 3; i++) {
            out[3][j] = out[3][j] - (tr[i] * inv3[i][j]);
        }
    }
    if (!affine) {
        for (i = 0; i < 4; i++) {
            out[i][3] = (out[i][3] + (((v.x * out[i][0]) + (v.y * out[i][1])) + (v.z * out[i][2]))) * w;
        }
        if (pivot >= 0) {
            for (i = 0; i < 4; i++) {
                f32 tmp = out[i][3];
                out[i][3] = out[i][pivot];
                out[i][pivot] = tmp;
            }
        }
    }
    return 1;
}

/* @ 0x80005FC0 - out = a * b (4x4, row-major), sums start from 0.0f. */
void vd_mtx_mul4(f32 out[4][4], f32 a[4][4], f32 b[4][4]) {
    s32 i, j, k;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            f32 sum = 0.0f;
            for (k = 0; k < 4; k++) {
                sum += a[i][k] * b[k][j];
            }
            out[i][j] = sum;
        }
    }
}
