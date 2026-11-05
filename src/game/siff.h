/* Vivid Dolls - Siff chunk reader/writer and the LZSS asset loader
 * (RAM 0x80009EC0-0x8000A090 and 0x8001BC30).
 *
 * Siff files are big-endian IFF-style chunks: 4-char tag, u32 size, data.
 * SiffIter keeps a stack of open chunks; Enter pushes one, Leave pops it.
 */
#ifndef VD_SIFF_H
#define VD_SIFF_H

#include "vd_common.h"

#define SIFF_TAG(a, b, c, d) (((u32)(a) << 24) | ((u32)(b) << 16) | ((u32)(c) << 8) | (u32)(d))
#define TAG_SIFF SIFF_TAG('S', 'i', 'f', 'f')
#define TAG_SP3D SIFF_TAG('S', 'p', '3', 'd')
#define TAG_INFO SIFF_TAG('i', 'n', 'f', 'o')
#define TAG_DLST SIFF_TAG('d', 'l', 's', 't')
#define TAG_PAL4 SIFF_TAG('p', 'a', 'l', '4')
#define TAG_PAL8 SIFF_TAG('p', 'a', 'l', '8')
#define TAG_VRTX SIFF_TAG('v', 'r', 't', 'x')
#define TAG_TILE SIFF_TAG('t', 'i', 'l', 'e')

typedef struct {
    u8 *write_start;    /* writer: where the chunk's data starts */
    u32 tag;            /* reader: tag of the open chunk */
    u32 size;           /* reader: its size */
    u8 *end;            /* reader: first byte after it */
} SiffLevel;

typedef struct {
    s32 depth;          /* -1 = nothing open */
    SiffLevel level[16];
} SiffIter;             /* 0x104 bytes */

void SiffIter_Init(SiffIter *it);                                  /* 0x80009EC0 */
u8  *SiffWriter_Begin(SiffIter *it, u32 *out, u32 tag);            /* 0x80009EE4 */
u8  *SiffWriter_End(SiffIter *it, u8 *cursor);                     /* 0x80009F10 */
u8  *SiffIter_Enter(SiffIter *it, u32 *chunk);                     /* 0x80009F68 - returns the chunk data */
u8  *SiffIter_Leave(SiffIter *it);                                 /* 0x80009FBC - returns the next chunk */
u8  *Siff_WriteString(u32 *out, u32 tag, const char *str);         /* 0x80009FD8 */
u8  *Siff_WriteU32(u32 *out, u32 tag, u32 value);                  /* 0x8000A03C */
u8  *Siff_WriteData(u32 *out, u32 tag, const u32 *src, s32 size);  /* 0x8000A054 */

/* Loads an LZSS-compressed asset from ROM into a new block on the game heap. */
u8  *Asset_LoadLzss(u32 rom_addr);                                 /* 0x8001BC30 */

/* Provided by the platform layer (on the N64: PI DMA, func_8000BFE8). */
void Rom_Read(u32 rom_addr, void *dst, u32 size);

#endif
