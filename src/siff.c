/* Vivid Dolls - Siff chunks and LZSS asset loading. See siff.h. */
#include "siff.h"
#include "heap.h"

extern Heap gGameHeap;  /* 0x80173600 */

/* @ 0x80009EC0 */
void SiffIter_Init(SiffIter *it)
{
    s32 *w = (s32 *)it;
    s32 n = 0x41;
    do {
        *w++ = 0;
    } while (--n != 0);
    it->depth = -1;
}

/* @ 0x80009EE4  writes the tag and leaves room for the size */
u8 *SiffWriter_Begin(SiffIter *it, u32 *out, u32 tag)
{
    it->depth++;
    WBE32(&out[0], tag);                      /* endian: ROM-format output */
    it->level[it->depth].write_start = (u8 *)(out + 2);
    return (u8 *)(out + 2);
}

/* @ 0x80009F10  pads to 4 bytes with zeros and fills in the size */
u8 *SiffWriter_End(SiffIter *it, u8 *cursor)
{
    u8 *start = it->level[it->depth].write_start;
    u8 *aligned = (u8 *)(((uintptr_t)cursor + 3) & ~(uintptr_t)3);
    s32 size = (s32)(cursor - start);

    if (cursor < aligned) {
        do {
            *cursor++ = 0;
        } while (cursor < aligned);
        size = (s32)(cursor - start);
    }
    WBE32(start - 4, (u32)size);              /* endian: ROM-format output */
    it->depth--;
    return start + size;
}

/* @ 0x80009F68 */
u8 *SiffIter_Enter(SiffIter *it, u32 *chunk)
{
    SiffLevel *l;
    it->depth++;
    l = &it->level[it->depth];
    l->tag = BE32(&chunk[0]);                 /* endian: chunk headers are big-endian */
    l->size = BE32(&chunk[1]);
    l->end = (u8 *)(chunk + 2) + l->size;
    return (u8 *)(chunk + 2);
}

/* @ 0x80009FBC */
u8 *SiffIter_Leave(SiffIter *it)
{
    s32 d = it->depth;
    it->depth = d - 1;
    return it->level[d].end;
}

/* @ 0x80009FD8  string chunk, zero-terminated and padded to 4 bytes */
u8 *Siff_WriteString(u32 *out, u32 tag, const char *str)
{
    u8 *start = (u8 *)(out + 1);
    u8 *p = start + 4;
    u8 *aligned;
    s32 size;
    u8 c;

    WBE32(&out[0], tag);
    do {
        c = (u8)*str++;
        *p++ = c;
    } while (c != 0);
    aligned = (u8 *)(((uintptr_t)p + 3) & ~(uintptr_t)3);
    size = (s32)(p - start);
    if (p < aligned) {
        do {
            *p++ = 0;
        } while (p < aligned);
        size = (s32)(p - start);
    }
    WBE32(&out[1], (u32)(size - 4));
    return p;
}

/* @ 0x8000A03C */
u8 *Siff_WriteU32(u32 *out, u32 tag, u32 value)
{
    WBE32(&out[1], 4);
    WBE32(&out[0], tag);
    WBE32(&out[2], value);
    return (u8 *)(out + 3);
}

/* @ 0x8000A054  (size is rounded up to 4; at least one word is always copied) */
u8 *Siff_WriteData(u32 *out, u32 tag, const u32 *src, s32 size)
{
    s32 n = (size + 3) & ~3;
    u32 *dst = out + 2;
    WBE32(&out[0], tag);
    WBE32(&out[1], (u32)n);
    do {
        *dst++ = *src++;
        n -= 4;
    } while (n != 0);
    return (u8 *)dst;
}

/* @ 0x8001BC30
 * Reads through a 256-byte window that is refilled from ROM as it is used up.
 * Like the original, a final back-reference may write up to 17 bytes past the
 * end of the output. */
u8 *Asset_LoadLzss(u32 rom_addr)
{
    u8 win[0x100];
    u32 rom = rom_addr + 0x100;
    u32 pos = 4;            /* read position in the window (low 8 bits used) */
    u32 flags = 0;
    s32 bits = 0;
    s32 out = 0;
    s32 size;
    u8 *dst, *wp;

    Rom_Read(rom_addr, win, 0x100);
    size = (s32)(win[0] + (win[1] << 8) + (win[2] << 16) + ((u32)win[3] << 24));
    dst = Heap_AllocFront(&gGameHeap, (u32)size);
    wp = dst;

    while (out < size) {
        flags <<= 1;
        if (bits == 0) {
            flags = win[pos++ & 0xFF];
            bits = 8;
            if ((pos & 0xFF) == 0) {
                Rom_Read(rom, win, 0x100);
                rom += 0x100;
                bits = 8;
            }
            flags <<= 1;
        }
        bits--;
        if (flags & 0x100) {
            u32 b1, b2, n;
            u8 *src, *d;
            b1 = win[pos++ & 0xFF];
            if ((pos & 0xFF) == 0) {
                Rom_Read(rom, win, 0x100);
                rom += 0x100;
            }
            b2 = win[pos++ & 0xFF];
            if ((pos & 0xFF) == 0) {
                Rom_Read(rom, win, 0x100);
                rom += 0x100;
            }
            n = (b1 >> 4) + 3;
            d = &dst[out];
            src = &dst[out - (s32)(b2 + ((b1 & 0xF) << 8) + 1)];
            do {
                *d++ = *src++;
                wp++;
                out++;
            } while (--n != 0);
        } else {
            *wp++ = win[pos++ & 0xFF];
            out++;
            if ((pos & 0xFF) == 0) {
                Rom_Read(rom, win, 0x100);
                rom += 0x100;
            }
        }
    }
    return dst;
}
