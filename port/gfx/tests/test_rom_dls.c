/* Smoke test: run the game's static display lists (main data segment) from
 * the player's ROM through the interpreter and check that every command is
 * understood.  Reads the ROM at run time; nothing from it is stored.
 *   ./test_rom_dls ../../../baserom.z64
 * The data is word-swapped as a whole (good for Gfx words; vertex halfwords
 * come out scrambled, which is fine here: only command coverage is checked). */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../f3d.h"
#include "../null_backend.h"

#define RAM_SIZE (1u << 24)
static unsigned char *ram;
static void *addr_hook(uint32_t a) { return ram + (a & 0x00FFFFFF); }

static const uint32_t dls[] = {
    0x80038560, 0x80038648, 0x800386A0, 0x800387D0, 0x80038828, 0x80038880, 0x800388D8,
    0x80038930, 0x80038968, 0x800389A8, 0x800389C0, 0x80038A00, 0x80038A20, 0x80038A60,
    0x80038B10, 0x80038B58, 0x80038B88, 0x80038BC0, 0x80038C10, 0x80038C80, 0x80038CC0,
    0x80038D00, 0x80038D50, 0x80038DA0, 0x80038DE0, 0x80038E20, 0x80038E60, 0x80038EA0,
    0x80038EE0, 0x80038F20, 0x80038F68, 0x80038FA8,
};

int main(int argc, char **argv)
{
    F3DBackend be;
    static NullBackend nb;
    FILE *f;
    unsigned char *rom;
    long n;
    uint32_t i, va0 = 0x80038150, va1 = 0x8003D540;
    int fails = 0;

    if (argc < 2) { printf("usage: %s baserom.z64 (skipped)\n", argv[0]); return 0; }
    f = fopen(argv[1], "rb");
    if (!f) { perror(argv[1]); return 1; }
    fseek(f, 0, SEEK_END); n = ftell(f); fseek(f, 0, SEEK_SET);
    rom = malloc((size_t)n); ram = calloc(1, RAM_SIZE);
    if (fread(rom, 1, (size_t)n, f) != (size_t)n) return 1;
    fclose(f);
    for (i = va0; i < va1; i += 4) {
        const unsigned char *p = rom + (i - 0x7FFFF400u);
        uint32_t w = (uint32_t)p[0] << 24 | (uint32_t)p[1] << 16 | (uint32_t)p[2] << 8 | p[3];
        memcpy(ram + (i & 0xFFFFFF), &w, 4);
    }
    null_backend_init(&be, &nb);
    f3d_init(&be, addr_hook);
    for (i = 0; i < sizeof(dls) / sizeof(dls[0]); i++) {
        f3d_clear_stats();
        f3d_run_dl(dls[i]);
        if (f3d_stats()->unknown || f3d_stats()->cmds == 0) {
            printf("  FAIL DL 0x%08X: %u unknown (last %08X)\n", dls[i], f3d_stats()->unknown,
                   f3d_stats()->last_unknown);
            fails++;
        }
    }
    printf("%u static display lists, %d failures\n", (unsigned)(sizeof(dls) / sizeof(dls[0])), fails);
    free(rom); free(ram);
    return fails != 0;
}
