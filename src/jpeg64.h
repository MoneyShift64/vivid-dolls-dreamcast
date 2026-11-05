/* Vivid Dolls - "jpeg64 Ver0.00" photo codec (RAM 0x80019D70-0x8001BA90).
 *
 * Baseline-JPEG maths with every header removed (see docs/FORMATS.md):
 *   - bits are read least-significant first from a 24-bit little-endian window,
 *     no byte stuffing, no markers;
 *   - 16x16 macroblocks in raster order, 6 blocks each: Y0 Y1 Y2 Y3 Cb Cr (4:2:0);
 *   - one DC predictor per block slot;
 *   - float dequantisation and a float matrix (I)DCT built from a cosine table.
 *
 * The ROM also contains the matching encoder (RGB->YCbCr, FDCT, quantiser,
 * Huffman writer). Nothing in the game calls it, but it is decompiled too.
 *
 * Working buffers (all game BSS):
 *   0x800A6C90 s32    bit position          0x800A6C94 u8 *  bitstream
 *   0x800A6C98 f32[8][8] cos table [u][x]   0x800A6D98 f32[8][8] same, transposed [x][u]
 *   0x800A6E98 u16[256]  RGBA5551 macroblock
 *   0x800A70A0 s16[256][3] Y,Cb,Cr per pixel of a 16x16 macroblock
 *   0x800A76A0 s16[6][64] spatial blocks    0x800A79A0 s16[6][64] dequantised blocks
 *   0x800A7CA0 s16[6][64] coefficients      0x800A7FA0 s16[6] DC predictors
 */
#ifndef VD_JPEG64_H
#define VD_JPEG64_H

#include "vd_common.h"

/* Decoder Huffman table as stored in the ROM (0x80039B50...). Codes are
 * grouped by length like libjpeg's derived tables: for length L the codes run
 * from mincode[L] to maxcode[L] (0xFFFF = no codes of this length) and map to
 * huffval[valptr[L] + code - mincode[L]]. */
typedef struct Jpeg64Huff {
    u16 valptr[18];
    u16 mincode[18];
    u16 maxcode[18];
    u16 huffval[1];             /* variable length */
} Jpeg64Huff;

/* --- decoder --- */
void Jpeg64_Decode(u32 rom_addr, u16 *out, s32 x, s32 y, s32 *width, s32 *height); /* 0x80019D70 */
void Jpeg64_Init(u8 *data);                                       /* 0x80019FAC */
s32  Jpeg64_GetBitPos(void);                                      /* 0x8001A118 */
void Jpeg64_YCbCrToRgba5551(u16 *out, s16 *pix);                  /* 0x8001A2D4 */
void Jpeg64_Upsample(s16 *pix, s16 *blocks);                      /* 0x8001A530 */
void Jpeg64_Idct(s16 *out, s16 *in);                              /* 0x8001AA40 */
void Jpeg64_Dequant(s16 *out, s16 *in, f32 scale);                /* 0x8001AFB0 */
void Jpeg64_DecodeMacroblock(s16 *coef, s16 *pred);               /* 0x8001B250 */
s32  Jpeg64_DecodeDC(const u16 *valptr, const u16 *mincode,
                     const u16 *maxcode, const u16 *huffval);     /* 0x8001B5F4 */
void Jpeg64_DecodeAC(s16 *level, s16 *run, const u16 *valptr, const u16 *mincode,
                     const u16 *maxcode, const u16 *huffval);     /* 0x8001B708 */
u32  Jpeg64_GetBit(void);                                         /* 0x8001B9A8 */
u32  Jpeg64_GetBits(u32 n);                                       /* 0x8001B9F8 */

/* --- encoder (unused by the game) --- */
void Jpeg64_RgbToYCbCr(s16 *pix, u8 *rgb);                        /* 0x8001A124 */
void Jpeg64_Downsample(s16 *blocks, s16 *pix);                    /* 0x8001A4A0 */
void Jpeg64_Fdct(s16 *out, s16 *in);                              /* 0x8001A5D8 */
void Jpeg64_Quantize(s16 *out, s16 *in, f32 scale);               /* 0x8001AEA8 */
void Jpeg64_EncodeMacroblock(s16 *coef, s16 *pred);               /* 0x8001B064 */
void Jpeg64_EncodeDC(s32 diff, const u16 *codes, const u16 *lens);            /* 0x8001B4D0 */
void Jpeg64_EncodeAC(s32 level, u32 run, const u16 *codes, const u16 *lens);  /* 0x8001B55C */
u32  Jpeg64_ReverseBits(u32 v, u32 n);                            /* 0x8001B84C */
void Jpeg64_PutBits(u32 v, s32 n);                                /* 0x8001B898 */

#endif
