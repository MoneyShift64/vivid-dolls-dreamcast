/* Vivid Dolls - integer 2D geometry, mesh helpers, triangle hit tests, look-at matrix
 * and radix sorts. See geom.h. "@ 0x..." tags give the original address.
 *
 * None of 0x80002200-0x80003300 / 0x80003650 except RadixSort_Records is called
 * directly by the game code we have looked at (most are library leftovers), but
 * they are kept for completeness.
 */
#include "geom.h"
#include "heap.h"
#include "libm.h"

extern Heap gGameHeap;

/* 32-bit wrapping integer helpers (the original uses plain subu/mult/mflo). */
static s32 wsub(s32 a, s32 b) { return (s32)((u32)a - (u32)b); }
static s32 wmul(s32 a, s32 b) { return (s32)((u32)a * (u32)b); }
static s32 wadd(s32 a, s32 b) { return (s32)((u32)a + (u32)b); }
static s32 wneg(s32 a) { return (s32)(0u - (u32)a); }

/* Signed division as done by MIPS div. The original traps (break 6) on
 * INT_MIN / -1; here that case wraps instead of being undefined behaviour. */
static s32 wdiv(s32 n, s32 d)
{
    if (d == -1)
        return wneg(n);
    return n / d;
}

/* @ 0x80002200
 * 2D cross product (b - a) x (c - a): > 0 if a, b, c turn anticlockwise. */
s32 Geom_Cross2i(const Vec2i *a, const Vec2i *b, const Vec2i *c)
{
    return wsub(wmul(wsub(b->x, a->x), wsub(c->y, a->y)),
                wmul(wsub(c->x, a->x), wsub(b->y, a->y)));
}

/* @ 0x80002248
 * Do segments a-b and c-d intersect?
 *   0: no (touching at an end point or along a line through an end point counts as no)
 *   1: they properly cross
 *   2: they are collinear and overlap (sharing an end coordinate counts as overlap)
 * The collinear test compares the coordinate (x or y) in which the two segments
 * extend furthest combined. */
s32 Seg2i_Intersect(const Vec2i *a, const Vec2i *b, const Vec2i *c, const Vec2i *d)
{
    s32 d1 = Geom_Cross2i(a, b, c);
    s32 d2 = Geom_Cross2i(a, b, d);
    s32 e1, e2;

    if (d1 == 0 && d2 == 0) {
        s32 abx = wsub(b->x, a->x), aby = wsub(b->y, a->y);
        s32 cdx = wsub(d->x, c->x), cdy = wsub(d->y, c->y);
        s32 useY = wadd(wmul(abx, abx), wmul(cdx, cdx)) < wadd(wmul(aby, aby), wmul(cdy, cdy));
        s32 lo1 = useY ? a->y : a->x, hi1 = useY ? b->y : b->x;
        s32 lo2 = useY ? c->y : c->x, hi2 = useY ? d->y : d->x;
        s32 t;

        if (hi1 < lo1) { t = lo1; lo1 = hi1; hi1 = t; }
        if (hi2 < lo2) { t = lo2; lo2 = hi2; hi2 = t; }
        if (lo1 == lo2 || hi1 == hi2)
            return 2;
        if (lo2 < lo1)
            return (lo1 < hi2) ? 2 : 0;
        return (lo2 < hi1) ? 2 : 0;
    }
    if (d1 == 0 || d2 == 0)
        return 0;
    if ((d1 ^ d2) >= 0)             /* c and d on the same side of a-b */
        return 0;
    e1 = Geom_Cross2i(c, d, a);
    e2 = Geom_Cross2i(c, d, b);
    if (e1 == 0 || e2 == 0)
        return 0;
    return (s32)((u32)(e1 ^ e2) >> 31);
}

/* Plane through three integer points, evaluated for z at (px, py):
 * z = -(A*px + B*py + D) / C, or z0 if the triangle is vertical (C == 0). */
static s32 PlaneZi(s32 px, s32 py, s32 x0, s32 y0, s32 z0, s32 x1, s32 y1, s32 z1,
                   s32 x2, s32 y2, s32 z2)
{
    s32 a = wsub(wmul(wsub(y1, y0), wsub(z2, z0)), wmul(wsub(z1, z0), wsub(y2, y0)));
    s32 b = wsub(wmul(wsub(z1, z0), wsub(x2, x0)), wmul(wsub(x1, x0), wsub(z2, z0)));
    s32 c = wsub(wmul(wsub(x1, x0), wsub(y2, y0)), wmul(wsub(y1, y0), wsub(x2, x0)));
    s32 d = wneg(wadd(wadd(wmul(x1, a), wmul(y1, b)), wmul(z1, c)));

    if (c == 0)
        return z0;
    return wdiv(wneg(wadd(wadd(wmul(a, px), wmul(b, py)), d)), c);
}

/* @ 0x80002434
 * Height (z) of the plane through tri[0..2] at the 2D point p (integer maths,
 * division truncates). Returns tri[0].z if the triangle is vertical. */
s32 Tri3i_InterpolateZ(const Vec2i *p, const Vec3i tri[3])
{
    return PlaneZi(p->x, p->y, tri[0].x, tri[0].y, tri[0].z, tri[1].x, tri[1].y, tri[1].z,
                   tri[2].x, tri[2].y, tri[2].z);
}

/* @ 0x80002558
 * Find the first triangle (vertex index triples into verts) whose 2D projection
 * contains p (edges included; triangles of either winding; degenerate ones skipped)
 * and linearly interpolate the three vertex attributes at p into out[0..2].
 * Returns 1, or 0 if no triangle contains p (out untouched).
 * Quirk not reproduced: the original compiler hoisted the loads of the three vertex
 * x coordinates from its local copy above the code that fills it, so the original
 * actually used stale stack contents for x0/x1/x2 in the plane equation. This C
 * uses the real vertex x values (the evident intent). */
s32 Mesh_InterpolateAt(s32 out[3], const Vec2i *p, const s32 (*tris)[3],
                       const MeshVertex *verts, s32 ntris)
{
    const s32 (*t)[3];

    for (t = tris; t < tris + ntris; t++) {
        const MeshVertex *v[3];
        s32 area, i, k;

        v[0] = &verts[(*t)[0]];
        v[1] = &verts[(*t)[1]];
        v[2] = &verts[(*t)[2]];
        area = Geom_Cross2i((const Vec2i *)v[0], (const Vec2i *)v[1], (const Vec2i *)v[2]);
        if (area == 0)
            continue;
        for (i = 0; i < 3; i++) {
            const Vec2i *e0 = (const Vec2i *)&verts[(*t)[i]];
            const Vec2i *e1 = (const Vec2i *)&verts[(*t)[(i != 2) ? i + 1 : 0]];
            s32 side = Geom_Cross2i(p, e0, e1);
            if (area > 0 ? side < 0 : side > 0)
                break;
        }
        if (i != 3)
            continue;

        for (k = 0; k < 3; k++)
            out[k] = PlaneZi(p->x, p->y,
                             v[0]->x, v[0]->y, v[0]->attr[k],
                             v[1]->x, v[1]->y, v[1]->attr[k],
                             v[2]->x, v[2]->y, v[2]->attr[k]);
        return 1;
    }
    return 0;
}

/* @ 0x80002990
 * Greedy triangulation of n integer 2D points: repeatedly add the shortest
 * connection that crosses or overlaps no edge chosen so far (ties: first pair found),
 * then find the triangles of the resulting edge graph using per-vertex linked edge
 * lists. Each triangle (i, j, k) is written once, anticlockwise or degenerate
 * (cross >= 0), and its three edge nodes are unlinked. Returns the triangle count.
 * Uses three temporary blocks from gGameHeap. Needs n >= 3. */
s32 Geom_GreedyTriangulate(s32 (*tris)[3], const Vec2i *pts, s32 n)
{
    s32 (*edges)[2] = Heap_Alloc(&gGameHeap, wmul(wsub(wmul(n, 3), 6), 8));
    s32 nedges = 0, count = 0;
    GeomEdgeLinks *links;
    GeomVertList *vl;
    s32 v, e;

    /* 1: greedy edge selection */
    for (;;) {
        s32 best = 0x7FFFFFFF, bi = 0, bj = 0, i, j, k;

        for (i = 0; i < n - 1; i++) {
            for (j = i + 1; j < n; j++) {
                s32 dx = wsub(pts[j].x, pts[i].x), dy = wsub(pts[j].y, pts[i].y);
                s32 dist2 = wadd(wmul(dx, dx), wmul(dy, dy));

                if (dist2 >= best)
                    continue;
                for (k = 0; k < nedges; k++) {
                    if (Seg2i_Intersect(&pts[i], &pts[j], &pts[edges[k][0]], &pts[edges[k][1]]) != 0)
                        break;
                }
                if (k == nedges) {
                    best = dist2;
                    bi = i;
                    bj = j;
                }
            }
        }
        if (best == 0x7FFFFFFF)
            break;
        edges[nedges][0] = bi;
        edges[nedges][1] = bj;
        nedges++;
    }

    /* 2: per-vertex edge lists */
    links = Heap_Alloc(&gGameHeap, wmul(nedges, sizeof(GeomEdgeLinks)));
    vl = Heap_Alloc(&gGameHeap, wmul(n, sizeof(GeomVertList)));
    for (v = 0; v < n; v++) {
        vl[v].head.prev = NULL;
        vl[v].tail.next = NULL;
        vl[v].head.next = &vl[v].tail;
        vl[v].tail.prev = &vl[v].head;
    }
    for (e = 0; e < nedges; e++) {
        s32 i = edges[e][0], j = edges[e][1];
        GeomLink *a = &links[e].a, *b = &links[e].b;
        GeomVertList *li = &vl[i], *lj = &vl[j];

        a->other = j;
        b->other = i;
        li->tail.prev->next = a;            /* append a to i's list */
        a->next = &li->tail;
        a->prev = li->tail.prev;
        li->tail.prev = a;
        lj->tail.prev->next = b;            /* append b to j's list */
        b->next = &lj->tail;
        b->prev = lj->tail.prev;
        lj->tail.prev = b;
    }

    /* 3: triangles v -> s -> t -> v */
    for (v = 0; v < n; v++) {
        GeomLink *n1 = vl[v].head.next, *n1next = n1->next;

        while (n1next != NULL) {
            s32 s = n1->other, found = 0;
            GeomLink *n2 = vl[s].head.next, *n2next = n2->next;

            while (n2next != NULL) {
                s32 t = n2->other;
                GeomLink *n3 = vl[t].head.next, *n3next = n3->next;

                while (n3next != NULL) {
                    if (n3->other == v && Geom_Cross2i(&pts[v], &pts[s], &pts[t]) >= 0) {
                        tris[count][0] = v;
                        tris[count][1] = s;
                        tris[count][2] = t;
                        n1->prev->next = n1->next;
                        n1->next->prev = n1->prev;
                        n2->prev->next = n2->next;
                        n2->next->prev = n2->prev;
                        n3->prev->next = n3->next;
                        n3->next->prev = n3->prev;
                        found = 1;
                        count++;
                        break;
                    }
                    n3 = n3next;
                    n3next = n3next->next;
                }
                if (found)
                    break;
                n2 = n2next;
                n2next = n2next->next;
            }
            n1 = n1next;
            n1next = n1next->next;
        }
    }

    Heap_Free(&gGameHeap, vl);
    Heap_Free(&gGameHeap, links);
    Heap_Free(&gGameHeap, edges);
    return count;
}

/* @ 0x80002E70
 * Intersect a line with the plane of triangle tri. Returns 0 if the line is
 * (nearly) parallel to it (|n . dir| < 1e-16, n unnormalised), else writes the hit
 * point to out and returns 1. */
s32 Tri_IntersectLine(Vec3f *out, const Line3f *line, const Vec3f tri[3])
{
    Planef pl;
    f32 den, t;

    Plane_FromPoints(&pl, &tri[0], &tri[1], &tri[2]);
    den = (pl.a * line->dir.x) + (pl.b * line->dir.y) + (pl.c * line->dir.z);
    if (den > -VD_EPSILON && den < VD_EPSILON)
        return 0;
    t = -((pl.a * line->point.x) + (pl.b * line->point.y) + (pl.c * line->point.z) + pl.d) / den;
    out->x = line->point.x + (line->dir.x * t);
    out->y = line->point.y + (line->dir.y * t);
    out->z = line->point.z + (line->dir.z * t);
    return 1;
}

/* @ 0x80002FA0
 * Hit test of a line against quad (0,1,2,3) as triangles (0,1,2) and (2,3,0). */
s32 Quad_HitTest(Vec3f *out, const Line3f *line, const Vec3f quad[4])
{
    Vec3f tri[3];

    if (Tri_HitTest(out, line, quad))
        return 1;
    tri[0] = quad[2];
    tri[1] = quad[3];
    tri[2] = quad[0];
    return Tri_HitTest(out, line, tri);
}

/* @ 0x8000306C
 * Does the line pass through triangle tri? Writes the plane hit point to out
 * whenever the line is not parallel (even if it misses). Inside test: for each edge
 * the normal of (edge, hit) must not point against the triangle normal; if the hit
 * point is on an edge's line (degenerate normal), the answer is decided by that edge
 * alone: inside iff the hit lies between its end points (cos angle <= 0).
 * Quirk kept: the triangle normal's success flag is ignored, so for a degenerate
 * triangle the original uses whatever was on the stack. */
s32 Tri_HitTest(Vec3f *out, const Line3f *line, const Vec3f tri[3])
{
    Vec3f n, en, va, vb;
    s32 i;

    if (!Tri_IntersectLine(out, line, tri))
        return 0;
    Vec3_TriangleNormal(&n, &tri[0], &tri[1], &tri[2]);
    for (i = 0; i < 3; i++) {
        const Vec3f *p = &tri[i];
        const Vec3f *q = &tri[(i + 1 < 3) ? i + 1 : 0];

        if (!Vec3_TriangleNormal(&en, p, q, out)) {
            va.x = p->x - out->x;
            vb.x = q->x - out->x;
            va.y = p->y - out->y;
            vb.y = q->y - out->y;
            va.z = p->z - out->z;
            vb.z = q->z - out->z;
            return Vec3_CosAngle(&va, &vb) <= 0.0f;
        }
        if (Vec3_Dot(&n, &en) < 0.0f)
            return 0;
    }
    return 1;
}

/* @ 0x80003650
 * Build a view matrix looking from eye towards at, with the given up vector
 * (row-vector convention: m[0..2][0] = right, [1] = up, [2] = -forward, row 3 =
 * translation). The matrix is first set to identity, before eye/at are read. */
void Mtx_LookAtV(MtxF m, const Vec3f *eye, const Vec3f *at, f32 upx, f32 upy, f32 upz)
{
    f32 dx, dy, dz, s, fx, fy, fz, rx, ry, rz, ux, uy, uz, dot;
    s32 i, j;

    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
            m[i][j] = (i == j) ? 1.0f : 0.0f;

    dx = at->x - eye->x;
    dy = at->y - eye->y;
    dz = at->z - eye->z;
    s = (f32)(-1.0 / (f64)sqrtf((dx * dx) + (dy * dy) + (dz * dz)));
    fz = dz * s;
    fx = dx * s;
    fy = dy * s;

    rx = (upy * fz) - (upz * fy);
    ry = (upz * fx) - (upx * fz);
    rz = (upx * fy) - (upy * fx);
    s = (f32)(1.0 / (f64)sqrtf((rx * rx) + (ry * ry) + (rz * rz)));
    rx = rx * s;
    rz = rz * s;
    ry = ry * s;

    ux = (fy * rz) - (fz * ry);
    uy = (fz * rx) - (fx * rz);
    uz = (fx * ry) - (fy * rx);
    s = (f32)(1.0 / (f64)sqrtf((ux * ux) + (uy * uy) + (uz * uz)));
    m[0][0] = rx;
    m[1][0] = ry;
    m[2][0] = rz;
    dot = (eye->x * rx) + (eye->y * ry) + (eye->z * rz);
    ux = ux * s;
    uy = uy * s;
    uz = uz * s;
    m[0][1] = ux;
    m[1][1] = uy;
    m[2][1] = uz;
    m[3][0] = -dot;
    dot = (eye->x * ux) + (eye->y * uy) + (eye->z * uz);
    m[0][2] = fx;
    m[1][2] = fy;
    m[2][2] = fz;
    m[3][1] = -dot;
    dot = (eye->x * fx) + (eye->y * fy) + (eye->z * fz);
    m[0][3] = 0.0f;
    m[1][3] = 0.0f;
    m[2][3] = 0.0f;
    m[3][3] = 1.0f;
    m[3][2] = -dot;
}

/* @ 0x800039F0
 * Stable LSD radix sort of n record pointers by the keylen key bytes at
 * record + keyofs (byte keyofs is the most significant). desc != 0 sorts in
 * descending order. Temporary tables come from gGameHeap. */
void RadixSort_Ptrs(u8 **items, s32 n, s32 keyofs, s32 keylen, s32 desc)
{
    u32 *counts = Heap_Alloc(&gGameHeap, (u32)keylen << 10);
    u8 **tmp = Heap_Alloc(&gGameHeap, wmul(n, 4));
    u8 **src = items, **dst = tmp, **sw;
    u32 *tbl = counts, *p = counts;
    s32 words = (s32)((u32)keylen << 10) >> 2;
    s32 i, k, pos;

    while (words != 0) {
        *p++ = 0;
        words--;
    }
    for (i = 0; i < n; i++) {
        const u8 *key = items[i] + keyofs;
        u32 *t = counts;
        for (k = 0; k < keylen; k++, t += 256)
            t[*key++]++;
    }
    for (k = 0; k < keylen; k++, tbl += 256) {
        u32 sum = 0, c;
        s32 b;
        if (desc == 0) {
            for (b = 0; b < 256; b++) { c = tbl[b]; tbl[b] = sum; sum += c; }
        } else {
            for (b = 255; b >= 0; b--) { c = tbl[b]; tbl[b] = sum; sum += c; }
        }
    }
    for (pos = keyofs + keylen - 1; pos >= keyofs; pos--) {
        tbl -= 256;
        for (i = 0; i < n; i++) {
            u32 slot = tbl[src[i][pos]]++;
            dst[slot] = src[i];
        }
        sw = src; src = dst; dst = sw;
    }
    if (src != items) {
        s32 bytes = wmul(n, 4);
        u8 **from = tmp, **to = items;
        if (bytes != 0) {
            do {
                *to++ = *from++;
                bytes -= 4;
            } while (bytes > 0);
        }
    }
    Heap_Free(&gGameHeap, tmp);
    Heap_Free(&gGameHeap, counts);
}

/* @ 0x80003C4C
 * Sort n records of recsize bytes in place by the key bytes at keyofs..keyofs+keylen-1
 * (see RadixSort_Ptrs). Quirk kept: the records are first copied to a temporary
 * buffer in whole words, so n*recsize is rounded up to a multiple of 4 bytes
 * (reading up to 3 bytes past the data). data must be 4-byte aligned. */
void RadixSort_Records(void *data, s32 n, s32 recsize, s32 keyofs, s32 keylen, s32 desc)
{
    s32 bytes = wmul(n, recsize);
    u8 *copy = Heap_Alloc(&gGameHeap, bytes);
    u8 **ptrs = Heap_Alloc(&gGameHeap, wmul(n, 4));
    u8 *rec = copy;
    s32 i, j;

    if (bytes != 0) {
        const u32 *s = data;
        u32 *d = (u32 *)copy;
        do {
            *d++ = *s++;
            bytes -= 4;
        } while (bytes > 0);
    }
    for (i = 0; i < n; i++) {
        ptrs[i] = rec;
        rec += recsize;
    }
    RadixSort_Ptrs(ptrs, n, keyofs, keylen, desc);

    if (recsize & 3) {
        u8 *out = data;
        for (i = 0; i < n; i++) {
            const u8 *s = ptrs[i];
            for (j = 0; j < recsize; j++)
                *out++ = *s++;
        }
    } else {
        u32 *out = data;
        for (i = 0; i < n; i++) {
            const u32 *s = (const u32 *)ptrs[i];
            for (j = 0; j < recsize; j += 4)
                *out++ = *s++;
        }
    }
    Heap_Free(&gGameHeap, ptrs);
    Heap_Free(&gGameHeap, copy);
}
