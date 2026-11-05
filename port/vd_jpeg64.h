/* Vivid Dolls port - "jpeg64 Ver0.00" photo decoder.
 *
 * Reimplements the game's decoder step for step:
 *   Jpeg64_Decode           func_80019D70   top level, 16x16 macroblocks
 *   Jpeg64_Init             func_80019FAC   bit reader + IDCT cosine table
 *   Jpeg64_DecodeMacroblock func_8001B250   Huffman, 6 blocks (Y Y Y Y Cb Cr)
 *   Jpeg64_DecodeDC / AC    func_8001B5F4 / func_8001B708
 *   Jpeg64_GetBit / GetBits func_8001B9A8 / func_8001B9F8  (LSB-first reader)
 *   Jpeg64_Dequant          func_8001AFB0   coef * (quant / sc), truncated
 *   Jpeg64_Idct             func_8001AA40   float matrix IDCT, rounded
 *   Jpeg64_Upsample         func_8001A530   Y + 128, chroma 2x2
 *   Jpeg64_YCbCrToRgba5551  func_8001A2D4   colour convert + 5-bit table
 * All tables come from the ROM (vd_jpeg64_tables.h).
 *
 * For bit-exact output build with -ffp-contract=off (no fused multiply-add),
 * since the N64 rounds every float operation separately.
 */
#ifndef VD_JPEG64_H
#define VD_JPEG64_H

#include "vd_types.h"

typedef struct {
    u32 width, height;   /* "xw  " / "yw  " chunks (multiples of 16) */
    u32 scale;           /* "sc  " chunk: quant tables are divided by this */
    const u8 *data;      /* "data" chunk: the bitstream */
    u32 data_size;
} VdJpeg64Info;

/* siff = raw jpeg64 Siff container as stored in ROM. Returns 0 if it is one. */
int vd_jpeg64_info(const u8 *siff, u32 size, VdJpeg64Info *out);

/* Decodes into out (width*height pixels, row stride = stride pixels).
 * Pixels are N64 RGBA5551 like the game; use vd_rgba5551_to_argb1555 for the
 * Dreamcast. Returns the number of bitstream bytes used (should be within
 * 3 bytes of data_size), or 0 on error. */
u32 vd_jpeg64_decode(const VdJpeg64Info *info, u16 *out, u32 stride);

#endif
