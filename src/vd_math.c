/* Vivid Dolls - maths and geometry library (RAM 0x80000450-0x80002200).
 * See vd_math.h. "@ 0x..." tags give the original address (used by tools/difftest).
 */
#include <math.h>
#include "vd_math.h"

/* @ 0x80000450
 * atan2 by a 14-term continued fraction: atan(t) = t / (1 + t^2/(3 + 4t^2/(5 + ...))).
 * Uses double precision for the final quadrant fix-ups, like the original. */
f32 Math_Atan2f(f32 y, f32 x)
{
    f32 t, r, frac, res;
    s32 n, odd;

    if (x == 0.0f && y == 0.0f)
        return 0.0f;
    if (x == 0.0f)
        return (y > 0.0f) ? 1.5707964f : -1.5707964f;

    t = y / x;
    r = t;
    if (!(fabsf(t) <= 1.0f))
        r = 1.0f / t;

    frac = 0.0f;
    n = 14;
    odd = 29;
    do {
        f32 num = (f32)(n * n) * r * r;
        f32 den = (f32)odd + frac;
        odd -= 2;
        n -= 1;
        frac = num / den;
    } while (n > 0);
    res = r / (frac + 1.0f);

    if (t > 1.0f)
        res = (f32)(1.5707963267948966 - (f64)res);
    if (t < -1.0f)
        res = (f32)(-1.5707963267948966 - (f64)res);
    if (x > 0.0f)
        return res;
    if (y > 0.0f)
        return (f32)((f64)res + 3.141592653589793);
    return (f32)((f64)res - 3.141592653589793);
}

/* @ 0x80000610 */
f32 Math_Atanf(f32 x)
{
    return Math_Atan2f(x, 1.0f);
}

/* @ 0x80000634
 * Direction of (x, y) as a binary angle, 0x400 = full turn, 0 = +x, 0x100 = +y. */
s32 Math_VecToAngle(f32 x, f32 y)
{
    s32 quadrant = 0;
    s32 a;

    if (x == 0.0f) {
        if (y == 0.0f) return 0;
        return (y < 0.0f) ? 0x300 : 0x100;
    }
    if (y == 0.0f)
        return (x < 0.0f) ? 0x200 : 0;

    if (x < 0.0f) { quadrant = 1; x = -x; }
    if (y < 0.0f) { quadrant |= 2; y = -y; }

    a = (s32)((f64)((Math_Atan2f(y / x, 1.0f) / 3.1415927f) * 512.0f) + 0.5);
    switch (quadrant) {
        case 0:  return a & 0x3FF;
        case 1:  return (0x200 - a) & 0x3FF;
        case 2:  return (0x400 - a) & 0x3FF;
        case 3:  return (a + 0x200) & 0x3FF;
        default: return 1;
    }
}

/* @ 0x800010A0  plane through three points */
void Plane_FromPoints(Planef *out, const Vec3f *a, const Vec3f *b, const Vec3f *c)
{
    f32 aby = b->y - a->y, acz = c->z - a->z, abz = b->z - a->z, acy = c->y - a->y;
    f32 acx = c->x - a->x, abx = b->x - a->x;
    f32 nz = (abx * acy) - (aby * acx);

    out->a = (aby * acz) - (abz * acy);
    out->b = (abz * acx) - (abx * acz);
    out->c = nz;
    out->d = -((b->x * out->a) + (b->y * out->b) + (b->z * nz));
}

/* @ 0x80001144  same, for three consecutive points */
s32 Plane_FromTriangle(Planef *out, const Vec3f tri[3])
{
    const Vec3f *a = &tri[0], *b = &tri[1], *c = &tri[2];
    f32 aby = b->y - a->y, acz = c->z - a->z, abz = b->z - a->z, acy = c->y - a->y;
    f32 acx = c->x - a->x, abx = b->x - a->x;
    f32 nz = (abx * acy) - (aby * acx);

    out->a = (aby * acz) - (abz * acy);
    out->b = (abz * acx) - (abx * acz);
    out->c = nz;
    out->d = -((b->x * out->a) + (b->y * out->b) + (b->z * nz));
    return 0;
}

/* @ 0x800011EC  returns 1 and the hit point, or 0 if the line is parallel */
s32 Plane_IntersectLine(Vec3f *out, const Line3f *line, const Planef *plane)
{
    f32 denom = (plane->a * line->dir.x) + (plane->b * line->dir.y) + (plane->c * line->dir.z);
    f32 t;

    if (denom > -VD_EPSILON && denom < VD_EPSILON)
        return 0;
    t = -((plane->a * line->point.x) + (plane->b * line->point.y) + (plane->c * line->point.z) + plane->d) / denom;
    out->x = line->point.x + (line->dir.x * t);
    out->y = line->point.y + (line->dir.y * t);
    out->z = line->point.z + (line->dir.z * t);
    return 1;
}

/* @ 0x800012E4  line where two planes meet; 0 if they are parallel.
 * The direction is normalised to length 1. */
s32 Line3_FromPlanes(Line3f *out, const Planef *p, const Planef *q)
{
    f32 dx, dy, dz, len2, inv, e, f, g, s;

    out->dir.x = (p->b * q->c) - (q->b * p->c);
    out->dir.y = (p->c * q->a) - (q->c * p->a);
    dy = out->dir.y;
    dx = out->dir.x;
    dz = (p->a * q->b) - (q->a * p->b);
    len2 = (dx * dx) + (dy * dy) + (dz * dz);
    out->dir.z = dz;
    if (len2 < VD_EPSILON)
        return 0;

    inv = 1.0f / len2;
    e = (p->d * q->b) - (p->b * q->d);
    f = (p->d * q->c) - (p->c * q->d);
    s = sqrtf(inv);
    g = (p->a * q->d) - (q->a * p->d);
    out->point.x = ((dy * f) - (dz * e)) * inv;
    out->point.z = ((out->dir.x * e) + (out->dir.y * g)) * inv;
    out->point.y = -((out->dir.x * f) + (out->dir.z * g)) * inv;
    out->dir.x = out->dir.x * s;
    out->dir.y = out->dir.y * sqrtf(inv);
    out->dir.z = out->dir.z * sqrtf(inv);
    return 1;
}

/* @ 0x800014DC */
s32 Line3_FromPoints(Line3f *out, const Vec3f *a, const Vec3f *b)
{
    out->point.x = a->x;
    out->point.y = a->y;
    out->point.z = a->z;
    out->dir.x = b->x - a->x;
    out->dir.y = b->y - a->y;
    out->dir.z = b->z - a->z;
    return 1;
}

/* @ 0x8000152C  perpendicular distance from p to the line (0 for a degenerate line) */
f32 Line3_DistanceToPoint(const Vec3f *p, const Line3f *line)
{
    f32 dx = line->dir.x, dy = line->dir.y, dz = line->dir.z;
    f32 len2 = (dx * dx) + (dy * dy) + (dz * dz);
    f32 vy, vx, vz, c1, c2, c3, rx, ry, rz;

    if (len2 < VD_EPSILON)
        return 0.0f;
    vy = p->y - line->point.y;
    vx = p->x - line->point.x;
    vz = p->z - line->point.z;
    c1 = (dx * vy) - (dy * vx);
    c2 = (dx * vz) - (dz * vx);
    c3 = (dy * vz) - (dz * vy);
    rx = (dy * c1) + (dz * c2);
    ry = (dz * c3) - (dx * c1);
    rz = (-dx * c2) - (dy * c3);
    return sqrtf((rx * rx) + (ry * ry) + (rz * rz)) / len2;
}

/* @ 0x80001648 */
f32 Vec3_Distance(const Vec3f *a, const Vec3f *b)
{
    f32 dx = b->x - a->x, dy = b->y - a->y, dz = b->z - a->z;
    return sqrtf((dx * dx) + (dy * dy) + (dz * dz));
}

/* @ 0x800016B0 */
f32 Vec3_Length(const Vec3f *v)
{
    return sqrtf((v->x * v->x) + (v->y * v->y) + (v->z * v->z));
}

/* @ 0x80001700 */
f32 Vec3_Dot(const Vec3f *a, const Vec3f *b)
{
    return (a->x * b->x) + (a->y * b->y) + (a->z * b->z);
}

/* @ 0x80001730  cosine of the angle between a and b (0 if either is zero) */
f32 Vec3_CosAngle(const Vec3f *a, const Vec3f *b)
{
    f32 m = ((a->x * a->x) + (a->y * a->y) + (a->z * a->z)) *
            ((b->x * b->x) + (b->y * b->y) + (b->z * b->z));
    if (m < VD_EPSILON)
        return 0.0f;
    return ((a->x * b->x) + (a->y * b->y) + (a->z * b->z)) / sqrtf(m);
}

/* @ 0x800017FC  unit normal of triangle (a, b, c); 0 if degenerate */
s32 Vec3_TriangleNormal(Vec3f *out, const Vec3f *a, const Vec3f *b, const Vec3f *c)
{
    f32 aby = b->y - a->y, bcz = c->z - b->z, abz = b->z - a->z, bcy = c->y - b->y;
    f32 bcx = c->x - b->x, abx = b->x - a->x;
    f32 nx = (aby * bcz) - (abz * bcy);
    f32 ny = (abz * bcx) - (abx * bcz);
    f32 nz = (abx * bcy) - (aby * bcx);
    f32 len2 = (nx * nx) + (ny * ny) + (nz * nz), inv;

    if (len2 < VD_EPSILON)
        return 0;
    inv = 1.0f / sqrtf(len2);
    out->x = nx * inv;
    out->y = ny * inv;
    out->z = nz * inv;
    return 1;
}

/* @ 0x8000192C */
void Vec3_NormalizeXYZ(f32 *x, f32 *y, f32 *z)
{
    f32 inv = 1.0f / sqrtf((*x * *x) + (*y * *y) + (*z * *z));
    *x *= inv;
    *y *= inv;
    *z *= inv;
}

/* @ 0x800019D4 */
void Vec3_Normalize(Vec3f *out, const Vec3f *in)
{
    f32 inv = 1.0f / sqrtf((in->x * in->x) + (in->y * in->y) + (in->z * in->z));
    out->x = in->x * inv;
    out->y = in->y * inv;
    out->z = in->z * inv;
}

/* @ 0x80001A70  normalised line equation through a and b; 0 if a == b */
s32 Line2_FromPoints(Line2f *out, const Vec2f *a, const Vec2f *b)
{
    f32 dx = b->x - a->x, dy = b->y - a->y;
    f32 len2 = (dx * dx) + (dy * dy), inv;

    if (len2 < VD_EPSILON)
        return 0;
    inv = 1.0f / sqrtf(len2);
    out->a = -dy * inv;
    out->b = dx * inv;
    out->c = ((a->x * b->y) - (b->x * a->y)) * inv;
    return 1;
}

/* @ 0x80001B70  crossing point of two line equations; 0 if parallel */
s32 Line2_Intersect(Vec2f *out, const Line2f *l, const Line2f *m)
{
    f32 det = (l->a * m->b) - (m->a * l->b);
    f32 adet = det < 0.0f ? -det : det, inv;

    if (adet < VD_EPSILON)
        return 0;
    inv = 1.0f / det;
    out->x = ((l->b * m->c) - (m->b * l->c)) * inv;
    out->y = ((m->a * l->c) - (l->a * m->c)) * inv;
    return 1;
}

/* @ 0x80001C2C  z of (p1 - p0) x (p2 - p0) */
f32 Math_Cross2(f32 x0, f32 y0, f32 x1, f32 y1, f32 x2, f32 y2)
{
    return ((x1 - x0) * (y2 - y0)) - ((x2 - x0) * (y1 - y0));
}

/* @ 0x80001C64  where a ray (as an infinite line) crosses a circle.
 * Returns 0 (miss), 1 (touching: both points equal) or 2. */
s32 Ray2_IntersectCircle(Vec2f out[2], const Ray2f *ray, const Vec2f *centre, f32 radius)
{
    f32 dx = ray->dx, dy = ray->dy;
    f32 len2 = (dx * dx) + (dy * dy);
    f32 vy, vx, cr, disc, along, s, inv, t0, t1;

    if (len2 < VD_EPSILON)
        return 0;
    vy = centre->y - ray->y;
    vx = centre->x - ray->x;
    cr = (dx * vy) - (dy * vx);
    disc = (radius * radius * len2) - (cr * cr);
    if (disc < -VD_EPSILON)
        return 0;
    along = (dx * vx) + (dy * vy);
    if (disc < VD_EPSILON) {
        f32 t = along / len2;
        f32 px = ray->x + (dx * t), py;
        out[1].x = px;
        out[0].x = px;
        py = ray->y + (ray->dy * t);
        out[1].y = py;
        out[0].y = py;
        return 1;
    }
    s = sqrtf(disc);
    inv = 1.0f / len2;
    t0 = (along - s) * inv;
    out[0].x = ray->x + (ray->dx * t0);
    t1 = (along + s) * inv;
    out[0].y = ray->y + (ray->dy * t0);
    out[1].x = ray->x + (ray->dx * t1);
    out[1].y = ray->y + (ray->dy * t1);
    return 2;
}

/* @ 0x80001E1C  the two points at distance dist either side of the ray start */
s32 Ray2_PerpendicularPoints(Vec2f out[2], const Ray2f *ray, f32 dist)
{
    f32 nx = ray->dy, ny = -ray->dx;
    f32 len2 = (nx * nx) + (ny * ny), inv, k, kp, kn;

    if (len2 < VD_EPSILON) {
        out[1].x = ray->x;
        out[0].x = ray->x;
        out[1].y = ray->y;
        out[0].y = ray->y;
        return 0;
    }
    inv = 1.0f / len2;
    k = dist * sqrtf(len2);
    kp = k * inv;
    kn = -k * inv;
    out[0].x = ray->x + (nx * kp);
    out[0].y = ray->y + (ny * kp);
    out[1].x = ray->x + (nx * kn);
    out[1].y = ray->y + (ny * kn);
    return 2;
}

/* @ 0x80001F4C  points at distance dist either side of (x0, y0), across the
 * segment towards (x1, y1); "left" ends up on the left-hand side. */
s32 Segment2_Offset(Vec2f *left, Vec2f *right, f32 x0, f32 y0, f32 x1, f32 y1, f32 dist)
{
    f32 dx = x1 - x0, dy = y1 - y0;
    f32 inv = sqrtf(1.0f / ((dx * dx) + (dy * dy)));
    f32 nd = -dist, lx;

    left->x = (dist * dy * inv) + x0;
    left->y = (nd * dx * inv) + y0;
    right->x = (nd * dy * inv) + x0;
    right->y = (dist * dx * inv) + y0;
    lx = left->x;
    if (((dx * (left->y - y0)) - ((lx - x0) * dy)) < 0.0f) {
        f32 ly;
        left->x = right->x;
        right->x = lx;
        ly = left->y;
        left->y = right->y;
        right->y = ly;
    }
    return 1;
}

/* @ 0x800020A4  squared distance from (x, y) to the ray's line */
f32 Ray2_DistanceSqToPoint(const Ray2f *ray, f32 x, f32 y)
{
    f32 dx = ray->dx, dx2 = dx * dx;
    f32 dy = ray->dy, dy2 = dy * dy;
    f32 len2 = dx2 + dy2;

    if (!(len2 < VD_EPSILON)) {
        f32 dxy = dx * dy;
        f32 vx = x - ray->x, vy = y - ray->y;
        f32 inv = 1.0f / len2;
        f32 ex = (dy2 * vx) - (dxy * vy);
        f32 ey = (dx2 * vy) - (dxy * vx);
        return ((ex * ex) + (ey * ey)) * (inv * inv);
    }
    return 0.0f;
}

/* @ 0x80002144 */
f32 Vec2_CosAngle(const Vec2f *a, const Vec2f *b)
{
    f32 ax = a->x, ay = a->y;
    f32 ay2 = ay * ay;
    f32 bx = b->x, by = b->y;
    f32 m = ((ax * ax) + ay2) * ((bx * bx) + (by * by));
    return ((ax * bx) + (ay * by)) / sqrtf(m);
}

/* @ 0x800021BC  line through p with normal n */
void Line2_FromPointNormal(Line2f *out, const Vec2f *p, const Vec2f *n)
{
    out->a = n->x;
    out->b = n->y;
    out->c = -((n->x * p->x) + (n->y * p->y));
}
