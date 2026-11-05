/* Vivid Dolls port - load and check the player's ROM file (VIVIDOLL.ROM).
 *
 * The disc normally holds the ROM prepared by tools/port/make_cdi.sh (.z64
 * order, cut to the used 0x23B000 bytes), but the full 8 MB dump in any byte
 * order also works (.u3 chip dump = byte-swapped "v64", .z64, word-swapped
 * "n64"): only the first 0x23B000 bytes are read and converted at boot, and
 * those are checked by SHA-1.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vd_rom.h"
#include "vd_sha1.h"

int vd_rom_normalize(uint8_t *p, size_t n)
{
    size_t i;
    uint8_t t;

    if (n < 4)
        return 0;
    if (p[0] == 0x80 && p[1] == 0x37 && p[2] == 0x12 && p[3] == 0x40)
        return VD_ROM_Z64;
    if (p[0] == 0x37 && p[1] == 0x80 && p[2] == 0x40 && p[3] == 0x12) {
        for (i = 0; i + 1 < n; i += 2) {
            t = p[i]; p[i] = p[i + 1]; p[i + 1] = t;
        }
        return VD_ROM_V64;
    }
    if (p[0] == 0x40 && p[1] == 0x12 && p[2] == 0x37 && p[3] == 0x80) {
        for (i = 0; i + 3 < n; i += 4) {
            t = p[i]; p[i] = p[i + 3]; p[i + 3] = t;
            t = p[i + 1]; p[i + 1] = p[i + 2]; p[i + 2] = t;
        }
        return VD_ROM_N64;
    }
    return 0;
}

int vd_rom_check(const uint8_t *p, char hex[41])
{
    uint8_t h[20];
    int i;

    vd_sha1(p, VD_ROM_KEEP, h);
    for (i = 0; i < 20; i++)
        sprintf(hex + i * 2, "%02x", h[i]);
    hex[40] = 0;
    return strcmp(hex, VD_ROM_SHA1_KEEP) == 0;
}

#ifdef VD_DREAMCAST
#include <kos.h>

static const char *const vd_rom_paths[] = {
    "/cd/VIVIDOLL.ROM",
    "/cd/vividoll.rom",
    "/pc/VIVIDOLL.ROM",      /* dc-tool / dcload-ip */
    "/rd/VIVIDOLL.ROM",
    NULL
};

int vd_rom_load(uint8_t **out, uint32_t *size, char *err, size_t errlen)
{
    const char *path = NULL;
    file_t f = -1;
    size_t total, got = 0;
    uint8_t *buf;
    char hex[41];
    int i, order;

    for (i = 0; vd_rom_paths[i] != NULL; i++) {
        f = fs_open(vd_rom_paths[i], O_RDONLY);
        if (f >= 0) {
            path = vd_rom_paths[i];
            break;
        }
    }
    if (f < 0) {
        snprintf(err, errlen,
                 "ROM file not found.\n\n"
                 "Put your Vivid Dolls ROM on the disc\n"
                 "as VIVIDOLL.ROM (tools/port/make_cdi.sh\n"
                 "does this for you).");
        return -1;
    }
    total = fs_total(f);
    if (total < VD_ROM_KEEP) {
        fs_close(f);
        snprintf(err, errlen,
                 "%s is too small\n(%u bytes, need at least %u).\n\n"
                 "This is not a Vivid Dolls ROM.", path, (unsigned)total, (unsigned)VD_ROM_KEEP);
        return -1;
    }
    buf = aligned_alloc(32, VD_ROM_KEEP);
    if (buf == NULL) {
        fs_close(f);
        snprintf(err, errlen, "Out of memory reading the ROM.");
        return -1;
    }
    while (got < VD_ROM_KEEP) {
        ssize_t r = fs_read(f, buf + got, VD_ROM_KEEP - got > 0x10000 ? 0x10000 : VD_ROM_KEEP - got);
        if (r <= 0)
            break;
        got += (size_t)r;
    }
    fs_close(f);
    if (got < VD_ROM_KEEP) {
        free(buf);
        snprintf(err, errlen, "Read error on %s\n(%u of %u bytes).", path,
                 (unsigned)got, (unsigned)VD_ROM_KEEP);
        return -1;
    }
    order = vd_rom_normalize(buf, VD_ROM_KEEP);
    if (order == 0) {
        free(buf);
        snprintf(err, errlen, "%s is not an N64/Aleck64 ROM\n(unknown header).", path);
        return -1;
    }
    if (!vd_rom_check(buf, hex)) {
        free(buf);
        snprintf(err, errlen,
                 "Wrong ROM: %s\n\nSHA-1 of the first 0x%X bytes:\n%s\nexpected:\n%s\n\n"
                 "You need the Vivid Dolls (Aleck64)\nROM dump nus-zsaj-0.u3.",
                 path, (unsigned)VD_ROM_KEEP, hex, VD_ROM_SHA1_KEEP);
        return -1;
    }
    *out = buf;
    *size = VD_ROM_KEEP;
    return 0;
}
#endif
