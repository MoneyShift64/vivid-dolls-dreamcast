/* Vivid Dolls - heap allocator. See heap.h. */
#include "heap.h"

#define USED_MARK(b) ((HeapBlock *)~(uintptr_t)(b))

/* @ 0x80003300  heap header followed by (size - 0x20) bytes of free space */
void Heap_Init(Heap *heap, u32 size)
{
    heap->freep = &heap->base;
    heap->base.next = &heap->base;
    heap->base.units = 0;
    Heap_AddRegion(heap, (u8 *)heap + 0x20, size - 0x20);
}

/* @ 0x8000332C  hand a block of memory to the heap */
void Heap_AddRegion(Heap *heap, void *mem, u32 size)
{
    HeapBlock *b = (HeapBlock *)mem;
    b->next = USED_MARK(b);
    b->units = size >> 4;
    Heap_Free(heap, b + 1);
}

/* @ 0x80003358 */
void *Heap_Alloc(Heap *heap, u32 size)
{
    u32 units = ((size + 15) >> 4) + 1;
    HeapBlock *prev = heap->freep;
    HeapBlock *b = prev->next;

    for (;;) {
        if (b->units >= units) {
            if (b->units == units) {
                prev->next = b->next;
            } else {
                b->units -= units;
                b += b->units;
                b->units = units;
            }
            b->next = USED_MARK(b);
            heap->freep = prev;
            return b + 1;
        }
        prev = b;
        if (b == heap->freep)
            return NULL;
        b = b->next;
    }
}

/* @ 0x800033D8 */
void *Heap_Free(Heap *heap, void *ptr)
{
    HeapBlock *b, *p;

    if (ptr == NULL)
        return NULL;
    b = (HeapBlock *)ptr - 1;
    if (b->next != USED_MARK(b))
        return NULL;

    /* find p with p < b < p->next, or the wrap-around point */
    for (p = heap->freep; !(p < b && b < p->next); p = p->next) {
        if (p >= p->next && (p < b || b < p->next))
            break;
    }

    if (b + b->units == p->next) {          /* merge with the following block */
        b->units += p->next->units;
        b->next = p->next->next;
    } else {
        b->next = p->next;
    }
    if (p + p->units == b) {                /* merge with the preceding block */
        p->units += b->units;
        p->next = b->next;
    } else {
        p->next = b;
    }
    heap->freep = p;
    return p;
}

/* @ 0x800034AC */
void *Heap_AllocFront(Heap *heap, u32 size)
{
    u32 units = ((size + 15) >> 4) + 1;
    HeapBlock *prev = heap->freep;
    HeapBlock *b = prev->next;

    for (;;) {
        if (b->units >= units) {
            if (b->units == units) {
                prev->next = b->next;
            } else {
                u32 avail = b->units;
                HeapBlock *rest = b + units;
                b->units = units;
                rest->units = avail - units;
                rest->next = b->next;
                prev->next = rest;
            }
            b->next = USED_MARK(b);
            heap->freep = prev;
            return b + 1;
        }
        prev = b;
        if (b == heap->freep)
            return NULL;
        b = b->next;
    }
}

/* @ 0x80003538  total free bytes */
u32 Heap_TotalFree(Heap *heap)
{
    HeapBlock *start = heap->freep;
    HeapBlock *b = start->next;
    u32 units = b->units;

    while (b != start) {
        b = b->next;
        units += b->units;
    }
    return units * 16;
}

/* @ 0x80003564  largest free block in bytes */
u32 Heap_LargestFree(Heap *heap)
{
    u32 best = 0;
    HeapBlock *b = heap->freep->next;

    for (;;) {
        if (best < b->units)
            best = b->units;
        if (b == heap->freep)
            break;
        b = b->next;
    }
    return best * 16;
}

/* @ 0x800035A0  smallest n with (1 << n) >= x */
s32 Math_Log2Ceil(u32 x)
{
    s32 n = 0;
    if (x > 1) {
        do { n++; } while ((u32)(1 << n) < x);
    }
    return n;
}

/* @ 0x800035D0  largest n with (1 << n) <= x */
s32 Math_Log2Floor(u32 x)
{
    s32 n = 0;
    if (x > 1) {
        do { n++; } while ((u32)(1 << n) < x);
    }
    return n - ((u32)(1 << n) != x);
}

/* @ 0x80003610 */
s32 Math_Log2Exact(u32 x)
{
    s32 n = 0;
    if (x > 1) {
        do { n++; } while ((u32)(1 << n) < x);
    }
    if ((u32)(1 << n) != x)
        n = -1;
    return n;
}
