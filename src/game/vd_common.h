/* Vivid Dolls decompilation - common types for the game code. */
#ifndef VD_COMMON_H
#define VD_COMMON_H

#include <stdint.h>
#include <stddef.h>

typedef uint8_t  u8;
typedef int8_t   s8;
typedef uint16_t u16;
typedef int16_t  s16;
typedef uint32_t u32;
typedef int32_t  s32;
typedef uint64_t u64;
typedef int64_t  s64;
typedef float    f32;
typedef double   f64;

/* ------------------------------------------------------------------------
 * Byte order (see port/ARCHITECTURE.md section 4 and port/ENDIAN_AUDIT.md).
 *
 * The N64 is big-endian, the Dreamcast SH4 little-endian. Data the game loads
 * from the ROM at run time (Siff chunks, sprite/model fields, jpeg64 headers,
 * audio file headers) stays in ROM (big-endian) order and is read with the
 * BE*() accessors below. On a big-endian build they are plain loads, so the
 * code is unchanged on the original MIPS (difftests); on a little-endian
 * build they assemble the value from bytes (alignment-agnostic).
 *
 * VD_BIG_ENDIAN: 1 on big-endian targets, 0 on little-endian ones.
 * ------------------------------------------------------------------------ */
#if defined(__BYTE_ORDER__) && defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#define VD_BIG_ENDIAN 1
#elif defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define VD_BIG_ENDIAN 0
#else
#error "vd_common.h: cannot tell the byte order (__BYTE_ORDER__ not defined)"
#endif

static inline u16 SWAP16(u16 v) { return (u16)((v << 8) | (v >> 8)); }
static inline u32 SWAP32(u32 v)
{
    return (v << 24) | ((v & 0xFF00u) << 8) | ((v >> 8) & 0xFF00u) | (v >> 24);
}

/* big-endian value <-> native value (no-ops on big-endian builds) */
#if VD_BIG_ENDIAN
#define BE16_TO_NATIVE(v) ((u16)(v))
#define BE32_TO_NATIVE(v) ((u32)(v))
#else
#define BE16_TO_NATIVE(v) SWAP16((u16)(v))
#define BE32_TO_NATIVE(v) SWAP32((u32)(v))
#endif

/* Read a big-endian value from a byte pointer. */
static inline u16 BE16(const void *p)
{
#if VD_BIG_ENDIAN
    return *(const u16 *)p;
#else
    const u8 *b = (const u8 *)p;
    return (u16)(((u32)b[0] << 8) | b[1]);
#endif
}

static inline s16 BE16S(const void *p) { return (s16)BE16(p); }

static inline u32 BE32(const void *p)
{
#if VD_BIG_ENDIAN
    return *(const u32 *)p;
#else
    const u8 *b = (const u8 *)p;
    return ((u32)b[0] << 24) | ((u32)b[1] << 16) | ((u32)b[2] << 8) | b[3];
#endif
}

static inline f32 BEF32(const void *p)
{
#if VD_BIG_ENDIAN
    return *(const f32 *)p;
#else
    union { u32 u; f32 f; } v;
    v.u = BE32(p);
    return v.f;
#endif
}

/* Write a value in big-endian order through a byte pointer (ROM-format output,
 * e.g. the Siff writer). Plain stores on big-endian builds. */
static inline void WBE16(void *p, u16 v)
{
#if VD_BIG_ENDIAN
    *(u16 *)p = v;
#else
    u8 *b = (u8 *)p;
    b[0] = (u8)(v >> 8);
    b[1] = (u8)v;
#endif
}

static inline void WBE32(void *p, u32 v)
{
#if VD_BIG_ENDIAN
    *(u32 *)p = v;
#else
    u8 *b = (u8 *)p;
    b[0] = (u8)(v >> 24);
    b[1] = (u8)(v >> 16);
    b[2] = (u8)(v >> 8);
    b[3] = (u8)v;
#endif
}

#if !VD_BIG_ENDIAN
/* Swap n 16-bit values in place (ROM big-endian -> native), e.g. RGBA16
 * texels and TLUTs that the F3D interpreter reads as native u16. */
static inline void SwapArray16(void *p, u32 n)
{
    u16 *q = (u16 *)p;
    while (n-- != 0) {
        *q = SWAP16(*q);
        q++;
    }
}
#endif

#endif
