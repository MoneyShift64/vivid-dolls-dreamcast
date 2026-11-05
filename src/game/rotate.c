/* Vivid Dolls - view/projection matrices, rotations and inversion. See rotate.h. */
#include "rotate.h"
#include "libm.h"

#define DEG2RAD 0.017453292519943295f   /* 0x3C8EFA35 */

/* 24 permutations of {0,1,2,3}, in lexicographic order (ROM data) */
extern u8 D_800383A0[24][4];

/* @ 0x80006C10
 * Camera matrix looking from eye towards at. f = -(at - eye)/|at - eye|,
 * s = u x f, v = f x s, with u = up/|up|. Quirk: s is NOT normalised, so the
 * matrix is only orthonormal when up is perpendicular to the view direction. */
void Mtx_LookAt(MtxF m, f32 eyeX, f32 eyeY, f32 eyeZ, f32 atX, f32 atY, f32 atZ,
                f32 upX, f32 upY, f32 upZ)
{
    f32 fx = atX - eyeX, fy = atY - eyeY, fz = atZ - eyeZ;
    f32 lenF = sqrtf((fx * fx) + (fy * fy) + (fz * fz));
    f32 lenU = sqrtf((upX * upX) + (upY * upY) + (upZ * upZ));
    f32 inv;
    f32 sx, sy, sz, vx, vy, vz;

    inv = (f32)(-1.0 / (f64)lenF);
    fx *= inv;
    fy *= inv;
    fz *= inv;
    inv = (f32)(1.0 / (f64)lenU);
    upX *= inv;
    upY *= inv;
    upZ *= inv;

    sx = (upY * fz) - (upZ * fy);
    sy = (upZ * fx) - (upX * fz);
    sz = (upX * fy) - (upY * fx);
    vx = (fy * sz) - (fz * sy);
    vy = (fz * sx) - (fx * sz);
    vz = (fx * sy) - (fy * sx);

    m[0][0] = sx;  m[0][1] = vx;  m[0][2] = fx;  m[0][3] = 0.0f;
    m[1][0] = sy;  m[1][1] = vy;  m[1][2] = fy;  m[1][3] = 0.0f;
    m[2][0] = sz;  m[2][1] = vz;  m[2][2] = fz;  m[2][3] = 0.0f;
    m[3][0] = -((eyeX * sx) + (eyeY * sy) + (eyeZ * sz));
    m[3][1] = -((eyeX * vx) + (eyeY * vy) + (eyeZ * vz));
    m[3][2] = -((eyeX * fx) + (eyeY * fy) + (eyeZ * fz));
    m[3][3] = 1.0f;
}

/* @ 0x80006EA8
 * Perspective projection (like guPerspectiveF without the scale argument). */
void Mtx_Perspective(MtxF m, f32 fovyDeg, f32 aspect, f32 near, f32 far)
{
    f32 half, cot;

    Mtx_Identity(m);
    half = (fovyDeg * DEG2RAD) / 2.0f;
    cot = cosf(half);
    cot = cot / sinf(half);
    m[0][0] = cot / aspect;
    m[1][1] = cot;
    m[2][2] = (near + far) / (near - far);
    m[2][3] = -1.0f;
    m[3][2] = ((near + near) * far) / (near - far);
    m[3][3] = 0.0f;
}

/* @ 0x80006F74
 * Orthographic projection, every element then multiplied by scale (guOrthoF). */
void Mtx_Ortho(MtxF m, f32 l, f32 r, f32 b, f32 t, f32 n, f32 f, f32 scale)
{
    s32 i, j;

    Mtx_Identity(m);
    m[0][0] = 2.0f / (r - l);
    m[1][1] = 2.0f / (t - b);
    m[2][2] = -2.0f / (f - n);
    m[3][0] = -(r + l) / (r - l);
    m[3][1] = -(t + b) / (t - b);
    m[3][2] = -(f + n) / (f - n);
    m[3][3] = 1.0f;
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            m[i][j] *= scale;
}

/* @ 0x800070A4
 * Rotation about a unit axis, given the sine and cosine of the angle.
 * The axis is re-read from memory between stores (matters only if it aliases m). */
void Mtx_RotateAxisSinCos(MtxF m, const Vec3f *axis, f32 s, f32 c)
{
    f32 x, y, z, xy, yz, zx, t, sq;

    Mtx_Identity(m);
    x = axis->x;
    y = axis->y;
    z = axis->z;
    t = 1.0f - c;
    xy = (x * y) * t;
    yz = (y * z) * t;
    zx = (z * x) * t;

    sq = x * x;
    m[0][0] = sq + (c * (1.0f - sq));
    m[2][1] = yz - (axis->x * s);
    m[1][2] = yz + (axis->x * s);
    sq = axis->y * axis->y;
    m[1][1] = sq + (c * (1.0f - sq));
    m[2][0] = zx + (axis->y * s);
    m[0][2] = zx - (axis->y * s);
    sq = axis->z * axis->z;
    m[2][2] = sq + (c * (1.0f - sq));
    m[1][0] = xy - (axis->z * s);
    m[0][1] = xy + (axis->z * s);
}

/* @ 0x800071CC
 * Rotation by angleDeg about the axis (x, y, z), which is normalised first. */
void Mtx_RotateAxisDeg(MtxF m, f32 angleDeg, f32 x, f32 y, f32 z)
{
    f32 s, c, t, xy, yz, zx, sq;

    Mtx_Identity(m);
    Vec3_NormalizeXYZ(&x, &y, &z);
    angleDeg *= DEG2RAD;
    s = sinf(angleDeg);
    c = cosf(angleDeg);
    t = 1.0f - c;
    xy = (x * y) * t;
    yz = (y * z) * t;
    zx = (z * x) * t;

    sq = x * x;
    m[0][0] = sq + (c * (1.0f - sq));
    sq = y * y;
    m[1][1] = sq + (c * (1.0f - sq));
    m[2][0] = zx + (y * s);
    m[0][2] = zx - (y * s);
    m[2][1] = yz - (x * s);
    m[1][2] = yz + (x * s);
    sq = z * z;
    m[2][2] = sq + (c * (1.0f - sq));
    m[1][0] = xy - (z * s);
    m[0][1] = xy + (z * s);
}

/* @ 0x800073B0
 * Invert a 4x4 matrix. Uses plain Gauss-Jordan when no diagonal element is
 * zero (always succeeds, returns 1), otherwise the row-permuting version
 * (returns 0 if no permutation gives a non-zero diagonal). */
s32 Mtx_Invert(MtxF out, const MtxF in)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        if (in[i][i] == 0.0f)
            break;
    }
    if (i == 4) {
        Mtx_InvertGaussJordan(out, in);
        return 1;
    }
    return Mtx_InvertPermuted(out, in);
}

/* @ 0x80007420
 * In-place Gauss-Jordan inversion without pivoting (in is copied to out first).
 * A zero pivot is simply not divided (the row is left as is, with 1 on the diagonal).
 * The reciprocal is computed in double precision. */
void Mtx_InvertGaussJordan(MtxF out, const MtxF in)
{
    s32 i, j, k;
    f32 p, f;

    if (out != in) {
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                out[i][j] = in[i][j];
    }
    for (k = 0; k < 4; k++) {
        p = out[k][k];
        out[k][k] = 1.0f;
        if (p != 0.0f) {
            p = (f32)(1.0 / (f64)p);
            for (j = 0; j < 4; j++)
                out[k][j] *= p;
        }
        for (i = 0; i < 4; i++) {
            if (i == k)
                continue;
            f = out[i][k];
            out[i][k] = 0.0f;
            for (j = 0; j < 4; j++)
                out[i][j] -= f * out[k][j];
        }
    }
}

/* @ 0x80007558
 * Inversion that first picks the first row permutation (from the table at
 * D_800383A0) giving a non-zero diagonal, then does LU-style elimination on the
 * permuted copy and back-substitution on out (which starts as the permutation
 * matrix). Later pivots are not checked for zero. Returns 0 if no permutation works. */
s32 Mtx_InvertPermuted(MtxF out, const MtxF in)
{
    s32 perm[4];
    f32 a[4][4];
    s32 n, i, j, k;
    f32 r, f;

    for (n = 0; n < 24; n++) {
        for (i = 0; i < 4; i++) {
            if (in[D_800383A0[n][i]][i] == 0.0f)
                break;
        }
        if (i == 4) {
            for (i = 0; i < 4; i++)
                perm[i] = D_800383A0[n][i];
            break;
        }
    }
    if (n == 24)
        return 0;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            a[i][j] = in[perm[i]][j];
            out[i][j] = (perm[i] == j) ? 1.0f : 0.0f;
        }
    }

    for (k = 0; k < 3; k++) {
        r = (f32)(1.0 / (f64)a[k][k]);
        for (j = k + 1; j < 4; j++)
            a[k][j] *= r;
        for (j = 0; j < 4; j++)
            out[k][j] *= r;
        for (i = k + 1; i < 4; i++) {
            f = a[i][k];
            for (j = k + 1; j < 4; j++)
                a[i][j] -= f * a[k][j];
            for (j = 0; j < 4; j++)
                out[i][j] -= f * out[k][j];
        }
    }
    r = (f32)(1.0 / (f64)a[3][3]);
    for (j = 0; j < 4; j++)
        out[3][j] *= r;

    for (k = 2; k >= 0; k--)
        for (i = k + 1; i < 4; i++)
            for (j = 0; j < 4; j++)
                out[k][j] -= a[k][i] * out[i][j];
    return 1;
}

/* @ 0x8000793C
 * out = v * RotateBetween(from, v) (as a point, translation row included).
 * Quirk: the return value of Mtx_RotateBetween is ignored, so if from and v
 * are parallel the original uses an uninitialised stack matrix. */
void Vec3_RotateFromTo(Vec3f *out, const Vec3f *v, const Vec3f *from)
{
    MtxF m;

    Mtx_RotateBetween(m, from, v);
    Mtx_TransformPoint(out, v, m);
}

/* @ 0x80007A08
 * Rotation matrix turning unit vector a onto unit vector b (about a x b).
 * Returns 0 (out untouched) if |a x b|^2 < 1e-16. If a.b <= -1, uses cos = -1, sin = 0. */
s32 Mtx_RotateBetweenUnit(MtxF out, const Vec3f *a, const Vec3f *b)
{
    Vec3f n;
    f32 len2, inv, c, s, t;
    s32 i;

    n.x = (a->y * b->z) - (a->z * b->y);
    n.y = (a->z * b->x) - (a->x * b->z);
    n.z = (a->x * b->y) - (a->y * b->x);
    len2 = (n.x * n.x) + (n.y * n.y) + (n.z * n.z);
    if (len2 < VD_EPSILON)
        return 0;
    inv = 1.0f / sqrtf(len2);
    n.x *= inv;
    n.y *= inv;
    n.z *= inv;

    c = (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
    if (c <= -1.0f) {
        t = 2.0f;
        s = 0.0f;
        c = -1.0f;
    } else {
        s = sqrtf(1.0f - (c * c));
        t = 1.0f - c;
    }

    out[0][0] = ((n.x * n.x) * t) + c;
    out[0][1] = ((n.x * n.y) * t) + (n.z * s);
    out[0][2] = ((n.x * n.z) * t) - (n.y * s);
    out[1][0] = ((n.y * n.x) * t) - (n.z * s);
    out[1][1] = ((n.y * n.y) * t) + c;
    out[1][2] = ((n.y * n.z) * t) + (n.x * s);
    out[2][0] = ((n.z * n.x) * t) + (n.y * s);
    out[2][1] = ((n.z * n.y) * t) - (n.x * s);
    out[2][2] = ((n.z * n.z) * t) + c;
    for (i = 0; i < 3; i++) {
        out[3][i] = 0.0f;
        out[i][3] = 0.0f;
    }
    out[3][3] = 1.0f;
    return 1;
}

/* @ 0x80007D28
 * Like Mtx_RotateBetweenUnit but normalises a and b first. */
s32 Mtx_RotateBetween(MtxF out, const Vec3f *a, const Vec3f *b)
{
    Vec3f na, nb;

    Vec3_Normalize(&na, a);
    Vec3_Normalize(&nb, b);
    return Mtx_RotateBetweenUnit(out, &na, &nb);
}

/* @ 0x80007D84
 * Basis-change matrix: with n = d x c, the 3x3 part is
 * [b x a | b | a]^T-style combination times the inverse of the frame (c, n, d):
 *   q = n / det, r = (c x n) / det, s = (n x d) / det, p = b x a,
 *   out[0][j] = p.x*q_j + b.x*r_j + a.x*s_j  (rows 1, 2 likewise with .y, .z)
 * where det = triple product of (c, n, d). Quirk: if det < 1e-16 (including any
 * negative det) 1/det is replaced by 0. Translation zero; always returns 1. */
s32 Mtx_BasisChange(MtxF out, const Vec3f *a, const Vec3f *b, const Vec3f *c, const Vec3f *d)
{
    f32 nx, ny, nz, px, py, pz, det, inv;
    f32 q0, q1, q2, r0, r1, r2, s0, s1, s2;

    nx = (d->y * c->z) - (d->z * c->y);
    ny = (d->z * c->x) - (d->x * c->z);
    nz = (d->x * c->y) - (d->y * c->x);
    px = (b->y * a->z) - (b->z * a->y);
    py = (b->z * a->x) - (b->x * a->z);
    pz = (b->x * a->y) - (b->y * a->x);
    det = ((((((d->x * c->y) * nz) - ((d->y * c->x) * nz)) + ((c->x * ny) * d->z))
            - ((nx * c->y) * d->z)) - ((d->x * ny) * c->z)) + ((nx * d->y) * c->z);
    if (VD_EPSILON <= det)
        inv = 1.0f / det;
    else
        inv = 0.0f;

    q0 = ((d->y * c->z) - (c->y * d->z)) * inv;
    q1 = ((c->x * d->z) - (d->x * c->z)) * inv;
    q2 = ((d->x * c->y) - (c->x * d->y)) * inv;
    r0 = ((c->y * nz) - (ny * c->z)) * inv;
    r1 = ((nx * c->z) - (c->x * nz)) * inv;
    r2 = ((c->x * ny) - (nx * c->y)) * inv;
    s0 = ((ny * d->z) - (d->y * nz)) * inv;
    s1 = ((d->x * nz) - (nx * d->z)) * inv;
    s2 = ((nx * d->y) - (d->x * ny)) * inv;

    out[0][0] = (px * q0) + (b->x * r0) + (a->x * s0);
    out[0][1] = (px * q1) + (b->x * r1) + (a->x * s1);
    out[0][2] = (px * q2) + (b->x * r2) + (a->x * s2);
    out[1][0] = (py * q0) + (b->y * r0) + (a->y * s0);
    out[1][1] = (py * q1) + (b->y * r1) + (a->y * s1);
    out[1][2] = (py * q2) + (b->y * r2) + (a->y * s2);
    out[2][0] = (pz * q0) + (b->z * r0) + (a->z * s0);
    out[2][1] = (pz * q1) + (b->z * r1) + (a->z * s1);
    out[2][2] = (pz * q2) + (b->z * r2) + (a->z * s2);
    out[0][3] = 0.0f;
    out[1][3] = 0.0f;
    out[2][3] = 0.0f;
    out[3][0] = 0.0f;
    out[3][1] = 0.0f;
    out[3][2] = 0.0f;
    out[3][3] = 1.0f;
    return 1;
}

/* @ 0x80008190
 * Rotation by angle (radians) about a unit axis; fills the whole matrix. */
void Mtx_RotateAxisRad(MtxF m, const Vec3f *axis, f32 angle)
{
    f32 s = sinf(angle);
    f32 c = cosf(angle);
    f32 x = axis->x, y = axis->y, z = axis->z;
    f32 t = 1.0f - c;
    f32 xx = x * x, yy = y * y, zz = z * z;
    f32 xy = (x * y) * t, xz = (x * z) * t, yz = (y * z) * t;
    f32 xs = x * s, ys = y * s, zs = z * s;

    m[0][0] = xx + ((1.0f - xx) * c);
    m[0][1] = xy + zs;
    m[0][2] = xz - ys;
    m[0][3] = 0.0f;
    m[1][0] = xy - zs;
    m[1][1] = yy + ((1.0f - yy) * c);
    m[1][2] = yz + xs;
    m[1][3] = 0.0f;
    m[2][0] = xz + ys;
    m[2][1] = yz - xs;
    m[2][2] = zz + ((1.0f - zz) * c);
    m[2][3] = 0.0f;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

/* @ 0x800082DC
 * out = RotX(deg) * m, column by column. When out == m the new row 1 value is
 * used for row 2 (kept from the original). */
void Mtx_PreRotateX(MtxF out, const MtxF m, f32 deg)
{
    f32 r = deg * DEG2RAD;
    f32 s = sinf(r);
    f32 c = cosf(r);
    s32 j;

    for (j = 0; j < 4; j++) {
        out[0][j] = m[0][j];
        out[1][j] = (m[1][j] * c) + (m[2][j] * s);
        out[2][j] = (m[2][j] * c) - (m[1][j] * s);
        out[3][j] = m[3][j];
    }
}

/* @ 0x800083A8
 * out = RotY(deg) * m (same aliasing quirk as Mtx_PreRotateX). */
void Mtx_PreRotateY(MtxF out, const MtxF m, f32 deg)
{
    f32 r = deg * DEG2RAD;
    f32 s = sinf(r);
    f32 c = cosf(r);
    s32 j;

    for (j = 0; j < 4; j++) {
        out[0][j] = (m[0][j] * c) - (m[2][j] * s);
        out[1][j] = m[1][j];
        out[2][j] = (m[0][j] * s) + (m[2][j] * c);
        out[3][j] = m[3][j];
    }
}

/* @ 0x80008474
 * out = RotZ(deg) * m (same aliasing quirk as Mtx_PreRotateX). */
void Mtx_PreRotateZ(MtxF out, const MtxF m, f32 deg)
{
    f32 r = deg * DEG2RAD;
    f32 s = sinf(r);
    f32 c = cosf(r);
    s32 j;

    for (j = 0; j < 4; j++) {
        out[0][j] = (m[0][j] * c) + (m[1][j] * s);
        out[1][j] = (m[1][j] * c) - (m[0][j] * s);
        out[2][j] = m[2][j];
        out[3][j] = m[3][j];
    }
}

/* @ 0x80008540
 * out = RotZ(z) * RotX(x) * RotY(y) * m  (Y applied to m first, then X, then Z). */
void Mtx_PreRotateYXZ(MtxF out, const MtxF m, f32 y, f32 x, f32 z)
{
    MtxF t1, t2;

    Mtx_PreRotateY(t1, m, y);
    Mtx_PreRotateX(t2, t1, x);
    Mtx_PreRotateZ(out, t2, z);
}

/* @ 0x800085B4
 * out = Translate(x, y, z) * m. */
void Mtx_PreTranslate(MtxF out, const MtxF m, f32 x, f32 y, f32 z)
{
    s32 j;

    for (j = 0; j < 4; j++) {
        out[0][j] = m[0][j];
        out[1][j] = m[1][j];
        out[2][j] = m[2][j];
        out[3][j] = (m[0][j] * x) + (m[1][j] * y) + (m[2][j] * z) + m[3][j];
    }
}

/* @ 0x80008624 */
void Mtx_PreTranslateX(MtxF out, const MtxF m, f32 x)
{
    s32 j;

    for (j = 0; j < 4; j++) {
        out[0][j] = m[0][j];
        out[1][j] = m[1][j];
        out[2][j] = m[2][j];
        out[3][j] = (m[0][j] * x) + m[3][j];
    }
}

/* @ 0x80008688 */
void Mtx_PreTranslateY(MtxF out, const MtxF m, f32 y)
{
    s32 j;

    for (j = 0; j < 4; j++) {
        out[0][j] = m[0][j];
        out[1][j] = m[1][j];
        out[2][j] = m[2][j];
        out[3][j] = (m[1][j] * y) + m[3][j];
    }
}

/* @ 0x800086EC */
void Mtx_PreTranslateZ(MtxF out, const MtxF m, f32 z)
{
    s32 j;

    for (j = 0; j < 4; j++) {
        out[0][j] = m[0][j];
        out[1][j] = m[1][j];
        out[2][j] = m[2][j];
        out[3][j] = (m[2][j] * z) + m[3][j];
    }
}

/* @ 0x80008750
 * out = Scale(x, y, z) * m (scales rows 0..2). */
void Mtx_PreScale(MtxF out, const MtxF m, f32 x, f32 y, f32 z)
{
    s32 j;

    for (j = 0; j < 4; j++) {
        out[0][j] = m[0][j] * x;
        out[1][j] = m[1][j] * y;
        out[2][j] = m[2][j] * z;
        out[3][j] = m[3][j];
    }
}

/* @ 0x800087BC */
void Mtx_PreScaleX(MtxF out, const MtxF m, f32 x)
{
    s32 j;

    for (j = 0; j < 4; j++) {
        out[0][j] = m[0][j] * x;
        out[1][j] = m[1][j];
        out[2][j] = m[2][j];
        out[3][j] = m[3][j];
    }
}

/* @ 0x80008818 */
void Mtx_PreScaleY(MtxF out, const MtxF m, f32 y)
{
    s32 j;

    for (j = 0; j < 4; j++) {
        out[0][j] = m[0][j];
        out[1][j] = m[1][j] * y;
        out[2][j] = m[2][j];
        out[3][j] = m[3][j];
    }
}

/* @ 0x80008874 */
void Mtx_PreScaleZ(MtxF out, const MtxF m, f32 z)
{
    s32 j;

    for (j = 0; j < 4; j++) {
        out[0][j] = m[0][j];
        out[1][j] = m[1][j];
        out[2][j] = m[2][j] * z;
        out[3][j] = m[3][j];
    }
}

/* @ 0x800088D0  empty function (no callers found) */
void Rotate_EmptyA(void)
{
}

/* @ 0x800088D8  empty function (no callers found) */
void Rotate_EmptyB(void)
{
}
