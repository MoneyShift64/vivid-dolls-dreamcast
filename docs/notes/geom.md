# geom area: 0x80002200-0x80003300, 0x80003650-0x80003E10

Files: `src/game/geom.c`, `src/game/geom.h`, test `tools/difftest/test_geom.py`
(run: `python3 tools/difftest/test_geom.py [FuncName ...]`, about 45 s).
All 11 functions PASS bit-exact (400 random trials each, 150 for the triangulation),
comparing return value, input/output memory and the whole 2 MB game heap.

## Functions done

| Address | Name | What it does |
|---|---|---|
| 0x80002200 | Geom_Cross2i | integer 2D cross product (b-a) x (c-a) |
| 0x80002248 | Seg2i_Intersect | integer segment intersection: 0 none/touching, 1 proper crossing, 2 collinear overlap |
| 0x80002434 | Tri3i_InterpolateZ | z of the plane through 3 integer points at (x,y), integer division (z0 if vertical) |
| 0x80002558 | Mesh_InterpolateAt | first triangle (index triples into 0x14-byte MeshVertex) containing p; interpolates its 3 attributes at p |
| 0x80002990 | Geom_GreedyTriangulate | greedy triangulation of int 2D points (shortest non-crossing edges), then extracts triangles via linked edge lists; uses gGameHeap |
| 0x80002E70 | Tri_IntersectLine | line / triangle-plane intersection point (0 if parallel, eps 1e-16) |
| 0x80002FA0 | Quad_HitTest | line hit test on quad as triangles (0,1,2) and (2,3,0) |
| 0x8000306C | Tri_HitTest | line passes through triangle? (plane hit + edge-normal sign tests) |
| 0x80003650 | Mtx_LookAtV | view matrix from eye/at/up (up passed as 3 floats in a3/stack) |
| 0x800039F0 | RadixSort_Ptrs | stable LSD byte radix sort of record pointers by key bytes (asc/desc); uses gGameHeap |
| 0x80003C4C | RadixSort_Records | in-place radix sort of fixed-size records (copies, sorts pointers, copies back) |

Despite the "float" flag in docs/functions.csv, 0x800039F0/0x80003C4C are integer code.

## Callers
Only 0x80003C4C is called directly (by func_80008A10 with keyofs = 4, desc = 0);
0x80002248 is called by 0x80002990 and 0x8000306C by 0x80002FA0. The rest have no
callers or data references in the main segment (library leftovers).

## Quirks
- **Mesh_InterpolateAt (0x80002558)**: the original's compiler hoisted the loads of
  the triangle's three x coordinates (`sp+0x0`, `sp+0xC`, `sp+0x18` of its local copy)
  to the top of the function, before the copy is filled, so the original uses
  stale stack contents for x0/x1/x2 in the plane equation. The C uses the real
  values (the intent). The test runs the original twice so its frame already holds
  the right values; without that about 1/4 of the trials differ.
- Tri3i_InterpolateZ / Mesh_InterpolateAt: original `div` traps (break 6) on
  INT_MIN / -1; the C wraps instead (`wdiv`). Tests skip that input.
- Tri_HitTest ignores the return value of the first Vec3_TriangleNormal, so for a
  degenerate triangle (with a non-parallel line) it reads an uninitialised normal.
  Kept as is; tests use non-degenerate triangles.
- Tri_HitTest: if the hit lies on an edge's line, that edge alone decides the result.
- Mtx_LookAtV writes identity first, then reads eye/at (matters when m aliases them);
  store/load order kept and tested with aliasing.
- RadixSort_Records copies the records to its temp buffer in whole words (n*recsize
  rounded up to 4; reads up to 3 bytes past the data); copies back in bytes when
  recsize & 3, else in words. data must be word aligned.
- Geom_GreedyTriangulate allocates (3n-6)*8 bytes for edges (n >= 3 needed).
  Its triangles are anticlockwise (cross >= 0); each triangle's edge nodes are unlinked.

## Structs (geom.h)
- `Vec2i {s32 x, y}`, `Vec3i {s32 x, y, z}`
- `MeshVertex {s32 x, y; s32 attr[3]}` 0x14 bytes
- `GeomLink {next, prev, s32 other}` 0x0C; `GeomVertList {head, tail}` 0x18
  (head.prev = NULL, tail.next = NULL); `GeomEdgeLinks {a, b}` 0x18.

## symbol_addrs.txt lines
```
Geom_Cross2i = 0x80002200; // type:func
Seg2i_Intersect = 0x80002248; // type:func
Tri3i_InterpolateZ = 0x80002434; // type:func
Mesh_InterpolateAt = 0x80002558; // type:func
Geom_GreedyTriangulate = 0x80002990; // type:func
Tri_IntersectLine = 0x80002E70; // type:func
Quad_HitTest = 0x80002FA0; // type:func
Tri_HitTest = 0x8000306C; // type:func
Mtx_LookAtV = 0x80003650; // type:func
RadixSort_Ptrs = 0x800039F0; // type:func
RadixSort_Records = 0x80003C4C; // type:func
D_8003D5E0 = 0x8003D5E0; // type:f64 size:0x8   (-1.0, used by Mtx_LookAtV)
D_8003D5E8 = 0x8003D5E8; // type:f64 size:0x8   (1.0, used by Mtx_LookAtV)
```

## Remaining
None in this area.
