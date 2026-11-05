/* Vivid Dolls port - basic types shared by the portable asset code. */
#ifndef VD_TYPES_H
#define VD_TYPES_H

#include <stdint.h>
#include <stddef.h>

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;

/* The ROM is big-endian; these read values regardless of the host CPU
 * (the Dreamcast's SH4 is little-endian). */
static inline u16 vd_be16(const u8 *p) { return (u16)((p[0] << 8) | p[1]); }
static inline u32 vd_be32(const u8 *p) { return ((u32)p[0] << 24) | ((u32)p[1] << 16) | ((u32)p[2] << 8) | p[3]; }
static inline u32 vd_le32(const u8 *p) { return ((u32)p[3] << 24) | ((u32)p[2] << 16) | ((u32)p[1] << 8) | p[0]; }

/* N64 RGBA5551 (rrrrrgggggbbbbba) -> Dreamcast ARGB1555 (arrrrrgggggbbbbb) */
static inline u16 vd_rgba5551_to_argb1555(u16 c) { return (u16)(((c & 1) << 15) | (c >> 1)); }

#endif
