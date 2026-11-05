/* Vivid Dolls port - hardware the game C touches directly (Dreamcast build).
 *
 * The game reads the Aleck64 cabinet registers with
 *     #define IO_REG16(a) (*(volatile u16 *)(a))      (src/game/gfx.c)
 * at 0x02000000+, which on the Dreamcast is not RAM. The DC build compiles
 * gfx.c with IO_REG16 redirected to VD_IO_REG16 (see port/Makefile.dc and the
 * game-C change requested in port/notes/sys.md); the registers are emulated
 * from the Dreamcast pads in port/ultra/os_cont.c.
 *
 * Same for the operator test's 4 MB "SDRAM" check at 0x01000000 (ovl0_test.c):
 * redirected to a small RAM area, so the test reports "SDRAM OK".
 */
#ifndef VD_PORT_IO_H
#define VD_PORT_IO_H

#include <stdint.h>

volatile uint16_t *vd_io_reg(uint32_t addr);
#define VD_IO_REG16(a) (*vd_io_reg((uint32_t)(a)))

/* Test cheat: nonzero while L + R are held (see port/ultra/os_cont.c). */
int vd_cheat_stage_clear(void);

extern volatile uint32_t vd_sdram_test_area[];
#define VD_SDRAM_TEST_WORDS 0x1000

#endif
