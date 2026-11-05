/* Vivid Dolls port - the player's ROM file. */
#ifndef VD_ROM_H
#define VD_ROM_H

#include <stddef.h>
#include <stdint.h>

/* Only the used first part of the 8 MB ROM is kept (the rest is 0xFF padding). */
#define VD_ROM_KEEP 0x23B000u

/* SHA-1 of the first VD_ROM_KEEP bytes in .z64 (big-endian) order. The full
 * 8 MB .z64 has SHA-1 747e9420cce23ff82b5922cd91ec43272d03fa4f (vividdolls.sha1). */
#define VD_ROM_SHA1_KEEP "eead45676f0104545eaba3abccd4e1bdd59db322"

/* Byte orders accepted (detected from the first word): */
enum { VD_ROM_Z64 = 1, VD_ROM_V64 = 2, VD_ROM_N64 = 3 };

/* Convert `n` bytes (n % 4 == 0) whose first word is the ROM header to .z64
 * order in place. Returns the detected order, or 0 if it is not an N64 ROM. */
int vd_rom_normalize(uint8_t *p, size_t n);

/* 1 if the first VD_ROM_KEEP bytes (z64 order) have the expected SHA-1;
 * hex (41 bytes) receives the computed hash. */
int vd_rom_check(const uint8_t *p, char hex[41]);

#ifdef VD_DREAMCAST
/* Find, read, convert and check the ROM file. On success *out / *size are the
 * image (z64 order, VD_ROM_KEEP bytes) and 0 is returned; otherwise a
 * message for the player is written into err. */
int vd_rom_load(uint8_t **out, uint32_t *size, char *err, size_t errlen);
#endif

#endif
