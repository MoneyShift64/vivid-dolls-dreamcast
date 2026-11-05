/* Vivid Dolls port - memory blocks and the boot-time data loader.
 *
 * The game C refers to its globals and tables through fixed symbols (D_8003xxxx,
 * ovlN_D_800Exxxx, gFoo...). The port keeps the original layout inside named blocks
 * (port/ARCHITECTURE.md section 2); the generated linker script port/gen/vd_blocks.ld
 * (vd_blocks_sh.ld for sh-elf) defines every symbol as vd_blk_<block> + offset.
 *
 * At boot, vd_dataload() fills the data blocks from the player's ROM image: copy,
 * byte-swap according to the generated swap map (little-endian hosts only), and
 * relocate every word that holds an N64 address. All other blocks are zeroed.
 *
 * Portable C99, no KallistiOS dependency. Pointers must be 32 bits wide for
 * vd_dataload() (SH4, mipsel); the core vd_dataload_apply() works on any host with
 * caller-supplied 32-bit block addresses (used by the host tests).
 *
 * Include paths: port/sys and port/gen.
 */
#ifndef VD_DATALOAD_H
#define VD_DATALOAD_H

#include <stddef.h>
#include <stdint.h>
#include "vd_blocks.h"

#ifdef __cplusplus
extern "C" {
#endif

/* One run of `count` items of `width` bytes (1, 2, 4 or 8) starting at block offset
 * `offset`. Width-1 runs are listed too, so the runs of a block cover every byte once. */
typedef struct {
    uint32_t offset;
    uint16_t count;
    uint8_t  width;
    uint8_t  pad;
} vd_swap_run;

/* A 32-bit word at block offset `offset` holds the N64 address of
 * (target block, target offset); target = block << 24 | offset. */
typedef struct {
    uint32_t offset;
    uint32_t target;
} vd_reloc;

#define VD_RT(blk, off)        ((uint32_t)(blk) << 24 | (uint32_t)(off))
#define VD_RT_BLOCK(t)         ((unsigned)((t) >> 24))
#define VD_RT_OFFSET(t)        ((t) & 0xFFFFFFu)

typedef struct {
    uint8_t  block;            /* VD_BLK_* */
    uint32_t rom_offset;       /* where the block's bytes are in the (big-endian .z64) ROM */
    uint32_t size;
    const vd_swap_run *runs;
    uint32_t nruns;
    const vd_reloc *relocs;
    uint32_t nrelocs;
} vd_data_block;

/* generated: port/gen/vd_dataload_tables.c */
extern const vd_data_block vd_data_blocks[];
extern const unsigned vd_data_block_count;

/* Block storage (defined in dataload.c) and its description. */
extern uint8_t *const  vd_block_base[VD_BLK_COUNT];
extern const uint32_t  vd_block_size[VD_BLK_COUNT];
extern const uint32_t  vd_block_n64[VD_BLK_COUNT];   /* original N64 start address */

/* Error codes (negative). */
#define VD_DL_OK              0
#define VD_DL_ERR_ROM_SIZE   -1   /* ROM too small for a data block */
#define VD_DL_ERR_RELOC      -2   /* a relocated word does not hold the expected N64 address (wrong ROM) */
#define VD_DL_ERR_TABLE      -3   /* inconsistent tables (bad width / range) */
#define VD_DL_ERR_PTR64      -4   /* vd_dataload() needs 32-bit pointers */

/* Fill all data blocks from `rom` (the .z64 image, big-endian byte order) and zero all
 * other blocks. Can be called again (soft reset) to restore the boot state.
 * Returns VD_DL_OK or an error code; on error *bad_offset (if non-NULL) receives the
 * ROM offset involved. */
int vd_dataload(const uint8_t *rom, size_t rom_size, uint32_t *bad_offset);

/* Core used by vd_dataload(): `base[i]` = host memory of block i, `addr[i]` = the 32-bit
 * address value relocations to block i must hold (normally (uint32_t)base[i]).
 * `swap` != 0 byte-swaps (little-endian target). Zeroes non-data blocks too if
 * `zero_others` != 0. */
int vd_dataload_apply(const uint8_t *rom, size_t rom_size, uint8_t *const base[],
                      const uint32_t addr[], int swap, int zero_others, uint32_t *bad_offset);

/* Zero one block (e.g. Overlay_Load clearing an overlay's BSS). */
void vd_block_clear(unsigned block);

/* N64 address -> port pointer, for an address inside a block (overlay addresses need
 * the overlay number 0..3, or -1 if not an overlay address). NULL if not in a block. */
void *vd_n64_to_port(uint32_t n64_addr, int ovl);

/* Port pointer -> original N64 address (0 if the pointer is not inside a block). */
uint32_t vd_port_to_n64(const void *p);

/* 1 if this build is little-endian (data blocks get swapped). */
int vd_host_little_endian(void);

#ifdef __cplusplus
}
#endif
#endif
