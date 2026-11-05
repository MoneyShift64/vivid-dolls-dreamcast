/* Vivid Dolls port - memory blocks and the boot-time data loader (see dataload.h).
 * Portable C99; no KallistiOS dependency. */
#include <string.h>
#include "dataload.h"

/* ---- block storage ------------------------------------------------------------------
 * One global array per block; the generated linker script defines the game's data
 * symbols relative to these (vd_blk_<name> + offset). Zero-sized blocks get 1 byte. */
#if defined(__GNUC__) || defined(__clang__)
#define VD_ALIGNED(a) __attribute__((aligned(a)))
#else
#define VD_ALIGNED(a)
#endif

#define X(name, id, size, align, n64, is_data, ovl) \
    uint8_t vd_blk_##name[(size) ? (size) : 1] VD_ALIGNED(align);
VD_BLOCK_LIST(X)
#undef X

#define X(name, id, size, align, n64, is_data, ovl) vd_blk_##name,
uint8_t *const vd_block_base[VD_BLK_COUNT] = { VD_BLOCK_LIST(X) };
#undef X
#define X(name, id, size, align, n64, is_data, ovl) (size),
const uint32_t vd_block_size[VD_BLK_COUNT] = { VD_BLOCK_LIST(X) };
#undef X
#define X(name, id, size, align, n64, is_data, ovl) (n64),
const uint32_t vd_block_n64[VD_BLK_COUNT] = { VD_BLOCK_LIST(X) };
#undef X
#define X(name, id, size, align, n64, is_data, ovl) (is_data),
static const uint8_t vd_block_is_data[VD_BLK_COUNT] = { VD_BLOCK_LIST(X) };
#undef X
#define X(name, id, size, align, n64, is_data, ovl) (ovl),
static const int8_t vd_block_ovl[VD_BLK_COUNT] = { VD_BLOCK_LIST(X) };
#undef X

/* ---- helpers ------------------------------------------------------------------------- */
int vd_host_little_endian(void)
{
#if defined(__BYTE_ORDER__) && defined(__ORDER_LITTLE_ENDIAN__)
    return __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__;
#else
    const union { uint32_t u; uint8_t b[4]; } t = { 1 };
    return t.b[0] == 1;
#endif
}

static uint32_t be32(const uint8_t *p)
{
    return (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
}

static void put_native32(uint8_t *p, uint32_t v)
{
    memcpy(p, &v, 4);
}

static void swap_items(uint8_t *p, uint32_t count, unsigned width)
{
    uint32_t i;
    uint8_t t;
    switch (width) {
    case 2:
        for (i = 0; i < count; i++, p += 2) {
            t = p[0]; p[0] = p[1]; p[1] = t;
        }
        break;
    case 4:
        for (i = 0; i < count; i++, p += 4) {
            t = p[0]; p[0] = p[3]; p[3] = t;
            t = p[1]; p[1] = p[2]; p[2] = t;
        }
        break;
    case 8:   /* f64 / s64 / u64: full 8-byte reversal (IEEE little-endian double layout) */
        for (i = 0; i < count; i++, p += 8) {
            t = p[0]; p[0] = p[7]; p[7] = t;
            t = p[1]; p[1] = p[6]; p[6] = t;
            t = p[2]; p[2] = p[5]; p[5] = t;
            t = p[3]; p[3] = p[4]; p[4] = t;
        }
        break;
    default:  /* 1: nothing to do */
        break;
    }
}

/* ---- loader -------------------------------------------------------------------------- */
int vd_dataload_apply(const uint8_t *rom, size_t rom_size, uint8_t *const base[],
                      const uint32_t addr[], int swap, int zero_others, uint32_t *bad_offset)
{
    unsigned i, k;

    if (zero_others) {
        for (i = 0; i < VD_BLK_COUNT; i++)
            if (!vd_block_is_data[i] && vd_block_size[i])
                memset(base[i], 0, vd_block_size[i]);
    }
    for (i = 0; i < vd_data_block_count; i++) {
        const vd_data_block *d = &vd_data_blocks[i];
        uint8_t *dst = base[d->block];
        uint32_t covered = 0;

        if (d->rom_offset > rom_size || rom_size - d->rom_offset < d->size) {
            if (bad_offset) *bad_offset = d->rom_offset;
            return VD_DL_ERR_ROM_SIZE;
        }
        memcpy(dst, rom + d->rom_offset, d->size);

        /* relocations: check the ROM word first (catches a wrong ROM), then swap, then patch */
        for (k = 0; k < d->nrelocs; k++) {
            const vd_reloc *r = &d->relocs[k];
            unsigned tb = VD_RT_BLOCK(r->target);
            uint32_t to = VD_RT_OFFSET(r->target);
            if (r->offset + 4 > d->size || tb >= VD_BLK_COUNT || to > vd_block_size[tb]) {
                if (bad_offset) *bad_offset = d->rom_offset + r->offset;
                return VD_DL_ERR_TABLE;
            }
            if (be32(rom + d->rom_offset + r->offset) != vd_block_n64[tb] + to) {
                if (bad_offset) *bad_offset = d->rom_offset + r->offset;
                return VD_DL_ERR_RELOC;
            }
        }
        for (k = 0; k < d->nruns; k++) {
            const vd_swap_run *s = &d->runs[k];
            uint32_t len = (uint32_t)s->count * s->width;
            if ((s->width != 1 && s->width != 2 && s->width != 4 && s->width != 8) ||
                s->offset != covered || len > d->size - s->offset) {
                if (bad_offset) *bad_offset = d->rom_offset + s->offset;
                return VD_DL_ERR_TABLE;
            }
            if (swap)
                swap_items(dst + s->offset, s->count, s->width);
            covered += len;
        }
        if (covered != d->size) {
            if (bad_offset) *bad_offset = d->rom_offset + covered;
            return VD_DL_ERR_TABLE;
        }
        for (k = 0; k < d->nrelocs; k++) {
            const vd_reloc *r = &d->relocs[k];
            put_native32(dst + r->offset, addr[VD_RT_BLOCK(r->target)] + VD_RT_OFFSET(r->target));
        }
    }
    return VD_DL_OK;
}

int vd_dataload(const uint8_t *rom, size_t rom_size, uint32_t *bad_offset)
{
#if UINTPTR_MAX == 0xFFFFFFFFu
    uint32_t addr[VD_BLK_COUNT];
    unsigned i;
    for (i = 0; i < VD_BLK_COUNT; i++)
        addr[i] = (uint32_t)(uintptr_t)vd_block_base[i];
    return vd_dataload_apply(rom, rom_size, vd_block_base, addr, vd_host_little_endian(), 1, bad_offset);
#else
    (void)rom; (void)rom_size;
    if (bad_offset) *bad_offset = 0;
    return VD_DL_ERR_PTR64;
#endif
}

void vd_block_clear(unsigned block)
{
    if (block < VD_BLK_COUNT && vd_block_size[block])
        memset(vd_block_base[block], 0, vd_block_size[block]);
}

void *vd_n64_to_port(uint32_t a, int ovl)
{
    unsigned i;
    for (i = 0; i < VD_BLK_COUNT; i++) {
        if (vd_block_ovl[i] >= 0 && vd_block_ovl[i] != ovl)
            continue;
        if (a >= vd_block_n64[i] && a - vd_block_n64[i] < vd_block_size[i])
            return vd_block_base[i] + (a - vd_block_n64[i]);
    }
    return NULL;
}

uint32_t vd_port_to_n64(const void *p)
{
    unsigned i;
    const uint8_t *q = (const uint8_t *)p;
    for (i = 0; i < VD_BLK_COUNT; i++) {
        /* compare as integers: pointers into different arrays are not ordered in C */
        uintptr_t lo = (uintptr_t)vd_block_base[i], v = (uintptr_t)q;
        if (v >= lo && v - lo < vd_block_size[i])
            return vd_block_n64[i] + (uint32_t)(v - lo);
    }
    return 0;
}
