/* Vivid Dolls port - overlays, TLB and other hardware-only system calls.
 *
 * All four overlays are linked at distinct addresses and stay resident. The
 * original Overlay_Load clears the whole overlay area and DMAs the overlay
 * (code + data) from the ROM, so every load starts with fresh data and zeroed
 * BSS; the port does the same for the overlay's data block (copy from the ROM
 * image, byte-swap, relocate - the same tables as the boot loader) and BSS
 * block. func_800E0000 (the call the main loop makes into the loaded overlay)
 * goes to the entry of the overlay loaded last.
 *
 * The game C's own Overlay_Load (system.c) is made weak by port/Makefile.dc so
 * this one is used; see port/notes/sys.md.
 */
#include <stdint.h>
#include <string.h>
#include "dataload.h"
#include "vd_port_io.h"
#include "../ultra/ultra_internal.h"
#include "vd_debug.h"

extern s32 Test_Run(s32 *args);        /* ovl0 */
extern s32 Attract_Run(s32 *args);     /* ovl1 */
extern s32 Game_Run(s32 *args);        /* ovl2 */
extern s32 GirlSelect_Run(s32 *args);  /* ovl3 */

typedef s32 (*VdOverlayEntry)(s32 *args);
static const VdOverlayEntry vd_overlay_entry[4] = { Test_Run, Attract_Run, Game_Run, GirlSelect_Run };
static int vd_overlay_current = -1;

static void vd_swap_items(uint8_t *p, uint32_t count, uint32_t width)
{
    uint32_t i, j;
    uint8_t t;

    if (width == 1)
        return;
    for (i = 0; i < count; i++, p += width)
        for (j = 0; j < width / 2; j++) {
            t = p[j];
            p[j] = p[width - 1 - j];
            p[width - 1 - j] = t;
        }
}

/* Reload one data block from the ROM image (as vd_dataload does at boot). */
static void vd_reload_data_block(unsigned blk)
{
    unsigned i, k;
    int le = vd_host_little_endian();

    for (i = 0; i < vd_data_block_count; i++) {
        const vd_data_block *d = &vd_data_blocks[i];
        uint8_t *dst;

        if (d->block != blk)
            continue;
        dst = vd_block_base[blk];
        if (vd_rom_image == NULL || d->rom_offset + d->size > vd_rom_size)
            return;
        memcpy(dst, vd_rom_image + d->rom_offset, d->size);
        if (le)
            for (k = 0; k < d->nruns; k++)
                vd_swap_items(dst + d->runs[k].offset, d->runs[k].count, d->runs[k].width);
        for (k = 0; k < d->nrelocs; k++) {
            uint32_t v = (uint32_t)(uintptr_t)vd_block_base[VD_RT_BLOCK(d->relocs[k].target)] +
                         VD_RT_OFFSET(d->relocs[k].target);
            memcpy(dst + d->relocs[k].offset, &v, 4);
        }
        return;
    }
}

void Overlay_Load(s32 index)
{
    if (index < 0 || index > 3)
        return;
    vd_reload_data_block(VD_BLK_OVL0_DATA + 2 * (unsigned)index);
    vd_block_clear(VD_BLK_OVL0_BSS + 2 * (unsigned)index);
    vd_overlay_current = index;
    VD_LOG(VD_DBG_OVL, "Overlay_Load(%d)\n", (int)index);
}

s32 func_800E0000(s32 *args)
{
    s32 r;

    if (vd_overlay_current < 0)
        return 0;
    VD_LOG(VD_DBG_OVL, "overlay %d entry, args %d %d %d\n", vd_overlay_current, (int)args[0],
           (int)args[1], (int)args[2]);
    r = vd_overlay_entry[vd_overlay_current](args);
    VD_LOG(VD_DBG_OVL, "overlay %d returned 0x%x\n", vd_overlay_current, (unsigned)r);
    return r;
}

void vd_overlay_reset(void)
{
    vd_overlay_current = -1;
}

/* ---- MIPS TLB: everything is directly addressed on the Dreamcast ---- */
typedef struct { u32 entryHi, entryLo0, entryLo1, pageMask; } VdTlbEntry;

VdTlbEntry *Tlb_Read(s32 index, VdTlbEntry *out)
{
    (void)index;
    if (out != NULL)
        memset(out, 0, sizeof(*out));
    return out;
}

VdTlbEntry *Tlb_Write(s32 index, VdTlbEntry *e)
{
    (void)index;
    return e;
}

VD_ALIAS(func_8000ACD0, Tlb_Read);
VD_ALIAS(func_8000ADA4, Tlb_Write);

/* ---- the operator test's SDRAM check (see vd_port_io.h) ---- */
volatile uint32_t vd_sdram_test_area[VD_SDRAM_TEST_WORDS];
