/* Vivid Dolls - heap allocator (RAM 0x80003300-0x80003650).
 *
 * A K&R-style first-fit allocator with a circular free list, in 16-byte units.
 * Every block starts with a 16-byte header:
 *   word 0: next free block, or ~(header address) while the block is in use
 *   word 1: size in 16-byte units, header included
 * The game's main heap lives at 0x80173600.
 */
#ifndef VD_HEAP_H
#define VD_HEAP_H

#include "vd_common.h"

typedef struct HeapBlock {
    struct HeapBlock *next;     /* free: next block; used: ~(this block) */
    u32 units;                  /* size in 16-byte units */
    u32 pad[2];
} HeapBlock;

typedef struct Heap {
    HeapBlock base;             /* zero-sized sentinel block (0x00) */
    HeapBlock *freep;           /* where the next search starts (0x10) */
    u32 pad[3];
} Heap;                         /* 0x20 bytes; the managed area follows it */

void  Heap_Init(Heap *heap, u32 size);                  /* 0x80003300 */
void  Heap_AddRegion(Heap *heap, void *mem, u32 size);  /* 0x8000332C */
void *Heap_Alloc(Heap *heap, u32 size);                 /* 0x80003358 - takes the end of a free block */
void *Heap_Free(Heap *heap, void *ptr);                 /* 0x800033D8 - returns NULL if ptr is not a live block */
void *Heap_AllocFront(Heap *heap, u32 size);            /* 0x800034AC - takes the start of a free block */
u32   Heap_TotalFree(Heap *heap);                       /* 0x80003538 */
u32   Heap_LargestFree(Heap *heap);                     /* 0x80003564 */

s32   Math_Log2Ceil(u32 x);                             /* 0x800035A0 */
s32   Math_Log2Floor(u32 x);                            /* 0x800035D0 */
s32   Math_Log2Exact(u32 x);                            /* 0x80003610 - -1 unless x is a power of two */

#endif
