/* Vivid Dolls - 4x4 float matrices. See matrix.h. */
#include "matrix.h"

#define ROWDOT4(a, i, b, j) \
    ((a[i][0] * b[0][j]) + (a[i][1] * b[1][j]) + (a[i][2] * b[2][j]) + (a[i][3] * b[3][j]))
#define ROWDOT3(a, i, b, j) \
    ((a[i][0] * b[0][j]) + (a[i][1] * b[1][j]) + (a[i][2] * b[2][j]))

/* @ 0x80006038 */
void Mtx_Mul(MtxF out, const MtxF a, const MtxF b)
{
    MtxF t;
    s32 i, j;

    if (out != b && out != a) {
        for (i = 0; i < 4; i++)
            for (j = 0; j < 4; j++)
                out[i][j] = ROWDOT4(a, i, b, j);
        return;
    }
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            t[i][j] = ROWDOT4(a, i, b, j);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            out[i][j] = t[i][j];
}

/* @ 0x80006778  note: the translation is a[3] + b[3], not a[3] * b + b[3];
 * when out aliases a or b, column 3 is left untouched */
void Mtx_MulAffine(MtxF out, const MtxF a, const MtxF b)
{
    f32 t[4][4];
    s32 i, j;

    if (out != b && out != a) {
        for (i = 0; i < 3; i++)
            for (j = 0; j < 3; j++)
                out[i][j] = ROWDOT3(a, i, b, j);
        for (j = 0; j < 3; j++)
            out[3][j] = a[3][j] + b[3][j];
        for (i = 2; i >= 0; i--)
            out[i][3] = 0.0f;
        out[3][3] = 1.0f;
        return;
    }
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            t[i][j] = ROWDOT3(a, i, b, j);
    for (i = 0; i < 3; i++)
        for (j = 0; j < 3; j++)
            out[i][j] = t[i][j];
    for (j = 0; j < 3; j++)
        out[3][j] = a[3][j] + b[3][j];
}

/* @ 0x80006958 */
void Mtx_TransformPoint(Vec3f *out, const Vec3f *p, const MtxF m)
{
    f32 x = p->x, y = p->y, z = p->z;
    out->x = (x * m[0][0]) + (y * m[1][0]) + (z * m[2][0]) + m[3][0];
    out->y = (x * m[0][1]) + (y * m[1][1]) + (z * m[2][1]) + m[3][1];
    out->z = (x * m[0][2]) + (y * m[1][2]) + (z * m[2][2]) + m[3][2];
}

/* @ 0x800069EC */
void Mtx_TransformVector(Vec3f *out, const Vec3f *v, const MtxF m)
{
    f32 x = v->x, y = v->y, z = v->z;
    out->x = (x * m[0][0]) + (y * m[1][0]) + (z * m[2][0]);
    out->y = (x * m[0][1]) + (y * m[1][1]) + (z * m[2][1]);
    out->z = (x * m[0][2]) + (y * m[1][2]) + (z * m[2][2]);
}

/* @ 0x80006A68  matrix that projects points straight onto the plane; 0 if the
 * plane normal is zero */
s32 Mtx_ProjectOntoPlane(MtxF out, const Planef *pl)
{
    f32 a2, b2, c2, len2, inv;

    a2 = pl->a;                     /* (kept in this order: matches the original) */
    b2 = pl->b * pl->b;
    c2 = pl->c * pl->c;
    len2 = (a2 * a2) + b2 + c2;
    if (len2 == 0.0f)
        return 0;
    inv = 1.0f / len2;
    out[0][0] = (b2 + c2) * inv;
    out[1][0] = -pl->a * pl->b * inv;
    out[2][0] = -pl->a * pl->c * inv;
    out[3][0] = -pl->a * pl->d * inv;
    out[0][1] = -pl->a * pl->b * inv;
    out[1][1] = ((pl->a * pl->a) + (pl->c * pl->c)) * inv;
    out[2][1] = -pl->b * pl->c * inv;
    out[3][1] = -pl->b * pl->d * inv;
    out[0][2] = -pl->a * pl->c * inv;
    out[1][2] = -pl->b * pl->c * inv;
    out[2][2] = ((pl->a * pl->a) + (pl->b * pl->b)) * inv;
    out[3][2] = -pl->c * pl->d * inv;
    out[0][3] = 0.0f;
    out[1][3] = 0.0f;
    out[2][3] = 0.0f;
    out[3][3] = 1.0f;
    return 1;
}

/* @ 0x80007308 */
void Mtx_Translate(MtxF m, f32 x, f32 y, f32 z)
{
    Mtx_Identity(m);
    m[3][0] = x;
    m[3][1] = y;
    m[3][2] = z;
}

/* @ 0x8000735C */
void Mtx_Scale(MtxF m, f32 x, f32 y, f32 z)
{
    Mtx_Identity(m);
    m[0][0] = x;
    m[1][1] = y;
    m[2][2] = z;
}

/* @ 0x800078C4 */
void Mtx_Identity(MtxF m)
{
    m[3][2] = 0.0f; m[3][1] = 0.0f; m[3][0] = 0.0f; m[2][3] = 0.0f;
    m[2][1] = 0.0f; m[2][0] = 0.0f; m[1][3] = 0.0f; m[1][2] = 0.0f;
    m[1][0] = 0.0f; m[0][3] = 0.0f; m[0][2] = 0.0f; m[0][1] = 0.0f;
    m[3][3] = 1.0f; m[2][2] = 1.0f; m[1][1] = 1.0f; m[0][0] = 1.0f;
}

/* @ 0x80007918 */
void Mtx_Copy(MtxF dst, const MtxF src)
{
    const f32 *s = &src[0][0];
    f32 *d = &dst[0][0];
    s32 n = 16;
    do {
        *d++ = *s++;
    } while (--n != 0);
}
