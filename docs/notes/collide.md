# Area "collide": 0x800088E0 - 0x8000A5A0 (main segment)

Source: `src/game/collide.c`, `src/game/collide.h`
Test: `python3 tools/difftest/test_collide.py` (about 2 minutes; every line PASS)

All 24 not-yet-done functions in the range are finished and verified. The Siff functions at
0x80009EC0-0x8000A08F were already named and were left alone.

## Functions done

| Address | Name | Description |
|---|---|---|
| 0x800088E0 | Mtx_FromRowsNormalized | Copies the 3x3 part of a 4x4 matrix with each row normalised (a zero row stays zero). Translation is cleared and w = 1. |
| 0x80008A10 | Mesh_BuildVertexBatches | Turns a triangle list into an RSP command stream that never needs more than 16 vertices loaded: batches -> chunks -> vertex sort/renumber -> commands. Uses the game heap. |
| 0x800094D0 | Pool_Init | Builds a stack pool (a table of free-item pointers ending in NULL) inside a memory block. |
| 0x8000953C | Pool_Take | Pops an item from the pool. Returns NULL and changes nothing when the pool is empty. |
| 0x80009564 | Pool_CountFree | Counts the items left in the pool. |
| 0x80009590 | Pool_Give | Pushes an item back. There are no checks. |
| 0x800095B0 | Mtx_LookAtReflect | Clone of guLookAtReflectF, except that eye and at are passed as Vec3f pointers. Fills the view matrix and the LookAt light dirs. |
| 0x80009AD4 | Mtx_PerspectiveG | Clone of guPerspectiveF. Returns perspNorm (u16). |
| 0x80009CB0 | Rand_Next | LCG: seed = seed*1103515245+12345. Returns (seed/65536)&0x7FFF. |
| 0x80009CE8 | Rand_Range | (Rand_Next * range) >> 15, using u32 arithmetic. |
| 0x80009D40 | Ring_Init | Builds a ring (FIFO) pool inside a memory block. |
| 0x80009DB4 | Ring_Get | Reads the next entry and wraps at the NULL terminator. Returns NULL once the read position reaches `stop`. |
| 0x80009DF4 | Ring_Count | Counts the entries between the read and write positions. |
| 0x80009E2C | Ring_Put | Writes an entry and advances the write position. If it catches the read position, stop is set to NULL. |
| 0x80009E6C | Ring_SetStop | stop = write, unless read == write. |
| 0x80009E90 | Stub_80009E90 | Empty function. |
| 0x80009E98 | Stub_80009E98 | Empty function. |
| 0x80009EA0 | Debug_Printf | Varargs debug print, compiled out. The original only spills a0-a3 to the caller's home area. |
| 0x8000A090 | Stub_8000A090 | Empty function. |
| 0x8000A0A0 | Mtx_RotateAxisSinCosG | Rotation about an axis (normalised with Vec3_NormalizeXYZ), with the angle given as sin and cos. |
| 0x8000A1F4 | Mtx_RotateBetweenG | Rotation about the normal of the plane (a, b, origin) by the angle with cos = -cos(a, b). Same arithmetic as Mtx_RotateAxisSinCosG, which it calls. |
| 0x8000A3D0 | BSpline2_Eval | Uniform quadratic B-spline segment value at t. |
| 0x8000A440 | BSpline2_Solve | Solves BSpline2_Eval(t) == x for t. Returns 0, 1 or 2 roots. |
| 0x8000A574 | BSpline2_Deriv | dP/dt of the quadratic B-spline. |

## Symbols (ready for symbol_addrs.txt)

```
Mtx_FromRowsNormalized = 0x800088E0; // type:func
Mesh_BuildVertexBatches = 0x80008A10; // type:func
Pool_Init = 0x800094D0; // type:func
Pool_Take = 0x8000953C; // type:func
Pool_CountFree = 0x80009564; // type:func
Pool_Give = 0x80009590; // type:func
Mtx_LookAtReflect = 0x800095B0; // type:func
Mtx_PerspectiveG = 0x80009AD4; // type:func
Rand_Next = 0x80009CB0; // type:func
Rand_Range = 0x80009CE8; // type:func
Ring_Init = 0x80009D40; // type:func
Ring_Get = 0x80009DB4; // type:func
Ring_Count = 0x80009DF4; // type:func
Ring_Put = 0x80009E2C; // type:func
Ring_SetStop = 0x80009E6C; // type:func
Stub_80009E90 = 0x80009E90; // type:func
Stub_80009E98 = 0x80009E98; // type:func
Debug_Printf = 0x80009EA0; // type:func
Stub_8000A090 = 0x8000A090; // type:func
Mtx_RotateAxisSinCosG = 0x8000A0A0; // type:func
Mtx_RotateBetweenG = 0x8000A1F4; // type:func
BSpline2_Eval = 0x8000A3D0; // type:func
BSpline2_Solve = 0x8000A440; // type:func
BSpline2_Deriv = 0x8000A574; // type:func
gVec3Zero = 0x80038400; // type:f32 size:0xC  (0,0,0; third point for Mtx_RotateBetweenG's plane)
D_8003D680 = 0x8003D680; // type:f64 size:0x8 (pi/180 = 0.017453292519943295, used by Mtx_PerspectiveG; the C uses the literal)
```

## Remaining

None in this range.

## Structs and findings

- **StackPool** `{ void **top; void **base; void *slots[]; }`. The slots table ends in NULL; the items
  come after it, aligned to 8. Item count = (memSize-12)/(align8(itemSize)+4).
- **RingPool** `{ void **base, **read, **stop, **write; void *slots[]; }`. Item count =
  (memSize-20)/(align8(itemSize)+4).
- **Mesh_BuildVertexBatches(u16 *out, s32 *order, MeshTri *tris, s32 ntris)**:
  - `MeshTri` is `{ s32 v[3]; s32 group; }`. Only the low 16 bits of each field are used, but the
    vertex/group counts come from the full s32 maximum.
  - Internally it uses a 24-byte heap node `{next, prev, v[3], group}` and Amiga-style lists with
    head and tail sentinels: `lists[257]` on the stack. List 0 is "unassigned" and lists 1..256 are the batches.
  - Output words: `2, slot, firstVertex, count` (vertex load), `1, group` (state change),
    `3, s0, s1, s2` (triangle in buffer slots). The stream ends with `0`.
  - `order[newIndex] = oldIndex`.
  - Returns the number of used vertices.
  - It returns 0 **without freeing** its heap blocks if a vertex is out of range (e.g. negative)
    or if more than 256 batches are needed. The C keeps this behaviour.
  - It calls func_80003C4C (= RadixSort_Records in another area's geom.h) to sort the vertex rows
    by the per-batch usage bytes. The C calls it as an extern.
  - The test resets the heap before each call and compares the first 0x20000 bytes of the heap.
    Words that point into the stack (list sentinels left in freed node memory) are masked out,
    because they depend on each version's stack frame layout.
  - Inputs with a vertex index >= 0x10000 make the original run out of the 2 MB heap (its
    NULL pointers then corrupt low RAM). Groups >= 0x10000 make it loop over 65536+ groups, so
    the test leaves both out.
- Mtx_PerspectiveG converts with `(u32)p`. NaN or out-of-range values can't reach this code
  (near+far > 2), and the NaN inputs in the test pass.
- Debug_Printf: clang's empty varargs function doesn't spill a0-a3 to the caller's home area.
  This only touches the caller's stack, so it doesn't matter.
