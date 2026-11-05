/* Vivid Dolls - geometry helpers, object pools and small leaf functions
 * (RAM 0x800088E0-0x8000A5A0). See collide.h. */
#include "collide.h"
#include "libm.h"

extern Vec3f D_80038400;    /* third point for Mtx_RotateBetweenG's plane (origin) */

/* @ 0x800088E0 */
/* Copies the 3x3 rotation part of `in` into `out` with every row normalised to
 * unit length (a zero row stays zero); clears the translation and sets w = 1. */
void Mtx_FromRowsNormalized(f32 out[4][4], const f32 in[4][4])
{
    s32 i, j;
    f32 len2;

    for (i = 0; i < 3; i++) {
        out[3][i] = 0.0f;
        out[i][3] = 0.0f;
    }
    out[3][3] = 1.0f;
    for (i = 0; i < 3; i++) {
        len2 = 0.0f;
        for (j = 0; j < 3; j++) {
            len2 += in[i][j] * in[i][j];
        }
        if (len2 != 0.0f) {
            len2 = 1.0f / sqrtf(len2);      /* now the scale factor */
        }
        for (j = 0; j < 3; j++) {
            out[i][j] = in[i][j] * len2;
        }
    }
}

/* @ 0x800094D0 */
/* Builds a stack pool in a memory block of memSize bytes: as many items of
 * itemSize (rounded up to 8) as fit, each with a 4-byte slot pointer. */
void Pool_Init(StackPool *pool, u32 memSize, u32 itemSize)
{
    u32 n;
    void **slot;
    u8 *item;

    itemSize = (itemSize + 7) & ~7u;
    n = (memSize - 12) / (itemSize + 4);
    slot = pool->slots;
    pool->top = slot;
    item = (u8 *)slot + ((n * 4 + 11) & ~7u);
    pool->base = slot;
    for (; n != 0; n--) {
        *slot++ = item;
        item += itemSize;
    }
    *slot = NULL;
}

/* @ 0x8000953C */
/* Takes an item from the pool; NULL (and no change) when it is empty. */
void *Pool_Take(StackPool *pool)
{
    void **p = pool->top;
    void *item;

    pool->top = p + 1;
    item = *p;
    if (item == NULL) {
        pool->top = p;
    }
    return item;
}

/* @ 0x80009564 */
/* Number of items still available in the pool. */
s32 Pool_CountFree(StackPool *pool)
{
    void **p = pool->top;
    s32 n = 0;

    while (*p++ != NULL) {
        n++;
    }
    return n;
}

/* @ 0x80009590 */
/* Returns an item to the pool (no checks). */
void Pool_Give(StackPool *pool, void *item)
{
    *--pool->top = item;
}

/* @ 0x80009AD4 */
/* guPerspectiveF clone: builds a perspective matrix (fovy in degrees), scales
 * every element by `scale` and returns the RSP perspective normaliser. */
u16 Mtx_PerspectiveG(f32 m[4][4], f32 fovy, f32 aspect, f32 near, f32 far, f32 scale)
{
    f32 cot, nf, p;
    s32 i, j;
    u32 norm;

    fovy = (f32)((f64)fovy * 0.017453292519943295) / 2.0f;
    cot = cosf(fovy);
    cot = cot / sinf(fovy);
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    nf = near - far;
    m[1][1] = cot;
    m[3][3] = 0.0f;
    m[2][3] = -1.0f;
    m[0][0] = cot / aspect;
    m[2][2] = (near + far) / nf;
    m[3][2] = ((near + near) * far) / nf;
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            m[i][j] *= scale;
        }
    }
    norm = 0xFFFF;
    if (!(near + far <= 2.0f)) {
        p = 131072.0f / (near + far);
        norm = (u32)p;
        if ((norm & 0xFFFF) == 0) {
            norm = 1;
        }
    }
    return (u16)norm;
}

/* @ 0x80009CB0 */
/* ANSI C style LCG: advances *seed and returns 15 random bits (0..32767). */
s32 Rand_Next(s32 *seed)
{
    s32 s = (s32)((u32)*seed * 0x41C64E6Du + 0x3039u);

    *seed = s;
    return (s / 65536) & 0x7FFF;
}

/* @ 0x80009CE8 */
/* Random number in 0..range-1 (for 0 <= range <= 0x10000). */
u32 Rand_Range(s32 *seed, s32 range)
{
    s32 s = (s32)((u32)*seed * 0x41C64E6Du + 0x3039u);

    *seed = s;
    return ((u32)((s / 65536) & 0x7FFF) * (u32)range) >> 15;
}

/* @ 0x80009D40 */
/* Builds a ring pool in a memory block of memSize bytes (see Pool_Init). */
void Ring_Init(RingPool *ring, u32 memSize, u32 itemSize)
{
    u32 n;
    void **slot;
    u8 *item;

    itemSize = (itemSize + 7) & ~7u;
    n = (memSize - 20) / (itemSize + 4);
    slot = ring->slots;
    ring->write = slot;
    ring->read = slot;
    ring->base = slot;
    item = (u8 *)slot + ((n * 4 + 11) & ~7u);
    ring->stop = NULL;
    for (; n != 0; n--) {
        *slot++ = item;
        item += itemSize;
    }
    *slot = NULL;
}

/* @ 0x80009DB4 */
/* Reads the next entry from the ring, wrapping at the NULL terminator;
 * NULL when the read position has reached `stop`. */
void *Ring_Get(RingPool *ring)
{
    void **r = ring->read;
    void *item;

    if (r == ring->stop) {
        return NULL;
    }
    ring->read = r + 1;
    item = *r;
    if (item == NULL) {
        r = ring->base;
        ring->read = r + 1;
        item = *r;
    }
    return item;
}

/* @ 0x80009DF4 */
/* Number of slots between the read and write positions. */
s32 Ring_Count(RingPool *ring)
{
    void **r = ring->read;
    void **w = ring->write;
    s32 n = 0;

    while (r != w) {
        r++;
        if (*r == NULL) {
            r = ring->base;
        }
        n++;
    }
    return n;
}

/* @ 0x80009E2C */
/* Writes an entry at the write position and advances it (wrapping). If it
 * catches up with the read position, the read limit is cleared. */
void Ring_Put(RingPool *ring, void *item)
{
    void **w = ring->write;

    ring->write = w + 1;
    *w = item;
    if (*ring->write == NULL) {
        ring->write = ring->base;
    }
    if (ring->write == ring->read) {
        ring->stop = NULL;
    }
}

/* @ 0x80009E6C */
/* Sets the read limit to the current write position (unless the ring is empty). */
void Ring_SetStop(RingPool *ring)
{
    if (ring->read != ring->write) {
        ring->stop = ring->write;
    }
}

/* @ 0x80009E90 */
/* Empty function. */
void Stub_80009E90(void)
{
}

/* @ 0x80009E98 */
/* Empty function. */
void Stub_80009E98(void)
{
}

/* @ 0x80009EA0 */
/* Debug print, compiled out in the release game. The original only spills its
 * four argument registers to the caller's home area (varargs prologue). */
void Debug_Printf(const char *fmt, ...)
{
    (void)fmt;
}

/* @ 0x8000A090 */
/* Empty function. */
void Stub_8000A090(void)
{
}

/* @ 0x8000A0A0 */
/* Rotation matrix about the axis (x, y, z) (normalised here) for an angle
 * given by its sine and cosine. Translation zero, w = 1. */
void Mtx_RotateAxisSinCosG(f32 m[4][4], f32 s, f32 c, f32 x, f32 y, f32 z)
{
    s32 i, j;
    f32 t, xy, yz, zx, sq;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            m[i][j] = (i == j) ? 1.0f : 0.0f;
        }
    }
    Vec3_NormalizeXYZ(&x, &y, &z);
    t = 1.0f - c;
    xy = (x * y) * t;
    yz = (y * z) * t;
    zx = (z * x) * t;
    sq = x * x;
    m[0][0] = sq + c * (1.0f - sq);
    m[2][0] = zx + y * s;
    m[2][1] = yz - x * s;
    m[1][2] = yz + x * s;
    m[0][2] = zx - y * s;
    sq = y * y;
    m[1][1] = sq + c * (1.0f - sq);
    m[1][0] = xy - z * s;
    m[0][1] = xy + z * s;
    sq = z * z;
    m[2][2] = sq + c * (1.0f - sq);
}

/* @ 0x8000A1F4 */
/* Rotation matrix about the normal of the plane through a, b and the origin,
 * by the angle whose cosine is -cos(a, b). */
void Mtx_RotateBetweenG(f32 m[4][4], const Vec3f *a, const Vec3f *b)
{
    Planef plane;
    f32 c, s;

    c = -Vec3_CosAngle(a, b);
    s = sqrtf(1.0f - c * c);
    Plane_FromPoints(&plane, a, b, &D_80038400);
    Mtx_RotateAxisSinCosG(m, s, c, plane.a, plane.b, plane.c);
}

/* @ 0x8000A3D0 */
/* Uniform quadratic B-spline segment at t (0..1) with control values p0..p2. */
f32 BSpline2_Eval(f32 t, f32 p0, f32 p1, f32 p2)
{
    f32 u = 1.0f - t;

    return (((u * u) / 2.0f) * p0) + (((u * t) + 0.5f) * p1) + (((t * t) / 2.0f) * p2);
}

/* @ 0x8000A440 */
/* Solves BSpline2_Eval(t, p0, p1, p2) == x for t. Returns the number of roots
 * written to out (0, 1 or 2; 2 roots in the order (-b - r)/a, (-b + r)/a).
 * A degenerate linear case with no slope gives one root of 0. */
s32 BSpline2_Solve(f32 out[2], f32 x, f32 p0, f32 p1, f32 p2)
{
    f32 b = p1 - p0;
    f32 a = (p0 - (p1 + p1)) + p2;
    f32 c = (p0 + p1) - (x + x);
    f32 d, r;

    if (a == 0.0f) {
        if (b != 0.0f) {
            out[0] = -c / b;
        } else {
            out[0] = 0.0f;
        }
        return 1;
    }
    d = (b * b) - (a * c);
    if (d < 0.0f) {
        return 0;
    }
    if (d == 0.0f) {
        out[0] = -b / a;
        return 1;
    }
    r = sqrtf(d);
    out[0] = (-b - r) / a;
    out[1] = (-b + r) / a;
    return 2;
}

/* @ 0x8000A574 */
/* Derivative (dP/dt) of BSpline2_Eval. */
f32 BSpline2_Deriv(f32 t, f32 p0, f32 p1, f32 p2)
{
    return (((p0 - (p1 + p1)) + p2) * t) + (p1 - p0);
}

/* 0.0..1.0 to a signed 8-bit fraction, saturating at 127 (FTOFRAC8). */
static s8 FloatToFrac8(f32 v)
{
    v = v * 128.0f;
    return (s8)((v < 127.0f) ? (s32)v : 127);
}

/* @ 0x800095B0 */
/* guLookAtReflectF clone with eye/at passed by pointer: builds the view matrix
 * and fills the LookAt structure used for reflection mapping. */
void Mtx_LookAtReflect(f32 mf[4][4], VdLookAt *l, const Vec3f *eye, const Vec3f *at,
                       f32 upX, f32 upY, f32 upZ)
{
    f32 lx, ly, lz, rx, ry, rz, ux, uy, uz, len, t;

    lx = at->x - eye->x;
    ly = at->y - eye->y;
    lz = at->z - eye->z;
    len = -1.0f / sqrtf((lx * lx) + (ly * ly) + (lz * lz));
    lz = lz * len;
    lx = lx * len;
    ly = ly * len;

    rx = (upY * lz) - (upZ * ly);
    ry = (upZ * lx) - (upX * lz);
    rz = (upX * ly) - (upY * lx);
    len = 1.0f / sqrtf((rx * rx) + (ry * ry) + (rz * rz));
    rx = rx * len;
    rz = rz * len;
    ry = ry * len;

    ux = (ly * rz) - (lz * ry);
    uy = (lz * rx) - (lx * rz);
    uz = (lx * ry) - (ly * rx);
    len = 1.0f / sqrtf((ux * ux) + (uy * uy) + (uz * uz));
    ux = ux * len;
    uy = uy * len;
    uz = uz * len;

    l->l[0].dir[0] = FloatToFrac8(rx);
    l->l[0].dir[1] = FloatToFrac8(ry);
    l->l[0].dir[2] = FloatToFrac8(rz);
    l->l[1].dir[0] = FloatToFrac8(ux);
    l->l[1].dir[1] = FloatToFrac8(uy);
    l->l[1].dir[2] = FloatToFrac8(uz);

    l->l[0].col[0] = 0x00; l->l[0].col[1] = 0x00; l->l[0].col[2] = 0x00; l->l[0].pad1 = 0x00;
    l->l[0].colc[0] = 0x00; l->l[0].colc[1] = 0x00; l->l[0].colc[2] = 0x00; l->l[0].pad2 = 0x00;
    l->l[1].col[0] = 0x00; l->l[1].col[1] = 0x80; l->l[1].col[2] = 0x00; l->l[1].pad1 = 0x00;
    l->l[1].colc[0] = 0x00; l->l[1].colc[1] = 0x80; l->l[1].colc[2] = 0x00; l->l[1].pad2 = 0x00;

    mf[0][0] = rx;
    mf[1][0] = ry;
    mf[2][0] = rz;
    t = -((eye->x * rx) + (eye->y * ry) + (eye->z * rz));
    mf[0][1] = ux;
    mf[1][1] = uy;
    mf[2][1] = uz;
    mf[3][0] = t;
    t = -((eye->x * ux) + (eye->y * uy) + (eye->z * uz));
    mf[0][2] = lx;
    mf[1][2] = ly;
    mf[2][2] = lz;
    mf[3][1] = t;
    mf[3][2] = -((eye->x * lx) + (eye->y * ly) + (eye->z * lz));
    mf[0][3] = 0.0f;
    mf[1][3] = 0.0f;
    mf[2][3] = 0.0f;
    mf[3][3] = 1.0f;
}

/* ---------------------------------------------------------------------------
 * Mesh_BuildVertexBatches
 * ------------------------------------------------------------------------- */
#include "heap.h"

extern Heap gGameHeap;
/* Sorts n records of recsize bytes by the keylen key bytes at keyofs
 * (decompiled elsewhere; resolves to the original). */
extern void func_80003C4C(void *data, s32 n, s32 recsize, s32 keyofs, s32 keylen, s32 desc);

#define MESH_MAX_BATCHES 256
#define MESH_VTX_CACHE   16     /* RSP vertex buffer size */

static void MeshList_Init(MeshList *list)
{
    list->head.next = &list->tail;
    list->head.prev = NULL;
    list->tail.next = NULL;
    list->tail.prev = &list->head;
}

static void MeshList_Append(MeshList *list, MeshNode *n)
{
    list->tail.prev->next = n;
    n->next = &list->tail;
    n->prev = list->tail.prev;
    list->tail.prev = n;
}

/* @ 0x80008A10 */
/* Converts a triangle list into an RSP-friendly command stream that never
 * needs more than 16 vertices loaded at once.
 *
 * 1. Triangles are grouped into "batches": repeatedly pick the (group, vertex)
 *    pair used by the most remaining triangles, then move triangles of that
 *    group sharing the vertex into a batch while it has <= 16 distinct vertices.
 * 2. Consecutive batches are merged into "chunks" whose vertex union is <= 16.
 * 3. Vertices are sorted by their per-batch usage bitmap (func_80003C4C) so each
 *    chunk's vertices form few contiguous runs; order[i] receives the original
 *    index of new vertex i and the triangles are renumbered.
 * 4. Output (u16 words), per chunk: {2, slot, firstVertex, count} vertex loads,
 *    then per triangle optional {1, group} on group change and {3, s0, s1, s2}
 *    (vertex buffer slots). Terminated by 0.
 * Returns the number of vertices used, or 0 on failure (a vertex index out of
 * range or more than 256 batches) - in that case the heap blocks are leaked,
 * like in the original. */
s32 Mesh_BuildVertexBatches(u16 *out, s32 *order, const MeshTri *tris, s32 ntris)
{
    MeshList lists[1 + MESH_MAX_BATCHES];   /* [0] = unassigned, [1 + b] = batch b */
    s32 vset[MESH_VTX_CACHE + 1];
    s32 nverts, ngroups, nbatches, nchunks, nset;
    s32 *remap, *count, *chunks, *pos;
    MeshNode *nodes, *n, *next;
    u8 *table;
    s32 keyLen, stride, best, selGroup = 0, selVert = 0;
    s32 i, j, k, g, b, c, start, end, cnt, used, group, slot, runStart, runLen;
    u32 bits;

    /* highest vertex index and group */
    nverts = 0;
    ngroups = 0;
    for (i = 0; i < ntris; i++) {
        if (ngroups < tris[i].group) {
            ngroups = tris[i].group;
        }
        for (k = 0; k < 3; k++) {
            if (nverts < tris[i].v[k]) {
                nverts = tris[i].v[k];
            }
        }
    }
    nverts++;
    ngroups++;
    remap = Heap_Alloc(&gGameHeap, nverts * 4);
    count = Heap_Alloc(&gGameHeap, nverts * 4);
    nodes = Heap_Alloc(&gGameHeap, ntris * sizeof(MeshNode));

    MeshList_Init(&lists[0]);
    for (i = 0; i < ntris; i++) {
        for (k = 0; k < 3; k++) {
            nodes[i].v[k] = (u16)tris[i].v[k];
        }
        nodes[i].group = (u16)tris[i].group;
        MeshList_Append(&lists[0], &nodes[i]);
    }

    /* 1. build batches */
    nbatches = 0;
    for (;;) {
        best = 0;
        for (g = 0; g < ngroups; g++) {
            for (i = 0; i < nverts; i++) {
                count[i] = 0;
            }
            for (n = lists[0].head.next; (next = n->next) != NULL; n = next) {
                if (n->group == g) {
                    for (k = 0; k < 3; k++) {
                        if (!(n->v[k] < nverts)) {
                            return 0;
                        }
                        count[n->v[k]]++;
                    }
                }
            }
            for (i = 0; i < nverts; i++) {
                if (best < count[i]) {
                    best = count[i];
                    selGroup = g;
                    selVert = i;
                }
            }
        }
        if (best == 0) {
            break;
        }

        for (;;) {
            for (i = 0; i < nverts; i++) {
                count[i] = 0;
            }
            for (n = lists[0].head.next; (next = n->next) != NULL; n = next) {
                if (n->group == selGroup) {
                    for (k = 0; k < 3; k++) {
                        count[n->v[k]]++;
                    }
                }
            }
            best = 0;
            for (i = 0; i < nverts; i++) {
                if (best < count[i]) {
                    best = count[i];
                    selVert = i;
                }
            }
            if (best == 0) {
                break;          /* group exhausted: pick a new group */
            }

            MeshList_Init(&lists[1 + nbatches]);
            nset = 0;
            for (n = lists[0].head.next; (next = n->next) != NULL; n = next) {
                if (n->group == selGroup) {
                    for (k = 0; k < 3; k++) {
                        if (n->v[k] == selVert) {
                            break;
                        }
                    }
                    if (k < 3) {
                        /* note: the vertices are added even if the triangle
                         * then doesn't fit (harmless, the loop ends) */
                        for (k = 0; k < 3; k++) {
                            for (j = 0; j < nset; j++) {
                                if (n->v[k] == vset[j]) {
                                    break;
                                }
                            }
                            if (j == nset) {
                                vset[nset++] = n->v[k];
                            }
                        }
                        if (nset <= MESH_VTX_CACHE) {
                            n->prev->next = n->next;
                            n->next->prev = n->prev;
                            MeshList_Append(&lists[1 + nbatches], n);
                        }
                    }
                }
                if (nset >= MESH_VTX_CACHE) {
                    break;
                }
            }
            nbatches++;
            if (nbatches >= MESH_MAX_BATCHES) {
                return 0;
            }
        }
    }

    /* per-vertex rows: s32 original index, then one byte per batch:
     * 0xFE = not used yet, 1 = used, 0 = not used (after first use) */
    keyLen = (nbatches + 3) & ~3;
    stride = keyLen + 4;
    table = Heap_Alloc(&gGameHeap, stride * nverts);
    for (i = 0; i < nverts; i++) {
        *(s32 *)(table + i * stride) = i;
        for (j = 0; j < keyLen; j++) {
            table[i * stride + 4 + j] = 0xFE;
        }
    }
    for (b = 0; b < nbatches; b++) {
        for (n = lists[1 + b].head.next; (next = n->next) != NULL; n = next) {
            for (k = 0; k < 3; k++) {
                table[b + n->v[k] * stride + 4] = 1;
            }
        }
    }
    for (i = 0; i < nverts; i++) {
        s32 seen = 0;
        for (j = 0; j < keyLen; j++) {
            u8 *p = &table[i * stride + 4 + j];
            if (*p != 0xFE) {
                seen = 1;
            } else if (seen) {
                *p = 0;
            }
        }
    }

    /* 2. merge consecutive batches into chunks of <= 16 vertices */
    chunks = Heap_Alloc(&gGameHeap, (nbatches + 1) * 4);
    nchunks = 0;
    start = 0;
    if (nbatches > 0) {
        do {
            end = start;
            for (;;) {
                cnt = 0;
                for (i = 0; i < nverts; i++) {
                    bits = 0;
                    for (j = start; j <= end; j++) {
                        bits |= table[i * stride + j + 4];
                    }
                    cnt += bits & 1;
                }
                if (cnt <= MESH_VTX_CACHE) {
                    end++;
                    if (end < nbatches) {
                        continue;
                    }
                }
                end--;
                break;
            }
            chunks[nchunks++] = end - start + 1;
            start = end + 1;
        } while (start < nbatches);
        start = 0;
    }
    /* make each vertex's bytes uniform within a chunk */
    for (c = 0; c < nchunks; c++) {
        end = start + chunks[c] - 1;
        for (i = 0; i < nverts; i++) {
            bits = 0;
            for (j = start; j <= end; j++) {
                bits |= table[i * stride + j + 4];
            }
            if (bits & 1) {
                bits = 1;
            }
            for (j = start; j <= end; j++) {
                table[i * stride + j + 4] = (u8)bits;
            }
        }
        start += chunks[c];
    }

    /* 3. sort vertices by usage pattern and renumber */
    func_80003C4C(table, nverts, stride, 4, keyLen, 0);
    for (i = 0; i < nverts; i++) {
        s32 *row = (s32 *)(table + i * stride);
        order[i] = *row;
        remap[*row] = i;
    }
    for (b = 0; b < nbatches; b++) {
        for (n = lists[1 + b].head.next; (next = n->next) != NULL; n = next) {
            for (k = 0; k < 3; k++) {
                n->v[k] = remap[n->v[k]];
            }
        }
    }
    used = 0;
    for (i = 0; i < nverts; i++) {
        bits = 0;
        for (j = 0; j < nbatches; j++) {
            bits |= table[i * stride + j + 4];
        }
        used += bits & 1;
    }

    /* 4. emit the command stream */
    group = -1;
    pos = Heap_Alloc(&gGameHeap, nverts * 4);   /* vertex -> buffer slot */
    start = 0;
    for (c = 0; c < nchunks; c++) {
        slot = 0;
        runLen = 0;
        runStart = -1;
        end = start + chunks[c] - 1;
        for (i = 0; i <= nverts; i++) {
            if (i != nverts && (table[i * stride + start + 4] & 1)) {
                pos[i] = slot++;
                if (runStart >= 0) {
                    runLen++;
                } else {
                    runStart = i;
                    runLen = 1;
                }
            } else if (runStart >= 0) {
                *out++ = 2;
                *out++ = (u16)pos[runStart];
                *out++ = (u16)runStart;
                *out++ = (u16)runLen;
                runStart = -1;
            }
        }
        for (b = start; b <= end; b++) {
            for (n = lists[1 + b].head.next; (next = n->next) != NULL; n = next) {
                if (group != n->group) {
                    group = n->group;
                    *out++ = 1;
                    *out++ = (u16)group;
                }
                *out++ = 3;
                for (k = 0; k < 3; k++) {
                    *out++ = (u16)pos[n->v[k]];
                }
            }
        }
        start += chunks[c];
    }
    *out = 0;

    Heap_Free(&gGameHeap, pos);
    Heap_Free(&gGameHeap, chunks);
    Heap_Free(&gGameHeap, table);
    Heap_Free(&gGameHeap, nodes);
    Heap_Free(&gGameHeap, count);
    Heap_Free(&gGameHeap, remap);
    return used;
}
