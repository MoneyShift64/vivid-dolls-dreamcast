/* Host test for the portable asset code.
 *
 *   ./host_test baserom.z64 [outdir]
 *
 * Walks all 506 assets: LZSS-decompresses them, parses every sprite, decodes
 * every jpeg64 photo and checks the bitstream is consumed exactly. With an
 * outdir it also writes the raw results (sprites as ARGB1555, decompressed
 * containers) so they can be compared with other tools.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../vd_assets.h"
#include "../vd_jpeg64.h"

static u8 *load(const char *path, long *size)
{
    FILE *f = fopen(path, "rb");
    u8 *b;
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    *size = ftell(f);
    fseek(f, 0, SEEK_SET);
    b = malloc((size_t)*size);
    if (fread(b, 1, (size_t)*size, f) != (size_t)*size) { fclose(f); free(b); return NULL; }
    fclose(f);
    return b;
}

static void save(const char *dir, const char *name, int idx, u32 rom, const void *p, size_t n)
{
    char path[512];
    FILE *f;
    if (!dir) return;
    snprintf(path, sizeof path, "%s/%s_%03d_%06X.bin", dir, name, idx, rom);
    f = fopen(path, "wb");
    if (f) { fwrite(p, 1, n, f); fclose(f); }
}

int main(int argc, char **argv)
{
    long rom_size;
    u8 *rom;
    const char *outdir = argc > 2 ? argv[2] : NULL;
    int n_assets = 0, n_sprites = 0, n_photos = 0, failures = 0;

    if (argc < 2) { fprintf(stderr, "usage: %s baserom.z64 [outdir]\n", argv[0]); return 2; }
    rom = load(argv[1], &rom_size);
    if (!rom || rom_size < (long)VD_ASSET_ROM_END) { fprintf(stderr, "cannot read ROM\n"); return 2; }

    /* Assets start on 4-byte boundaries: either "Siff" or LZSS whose first
     * flag byte has literals for the "Siff" tag. */
    for (u32 o = VD_ASSET_ROM_START; o < VD_ASSET_ROM_END; o += 4) {
        const u8 *a = rom + o;
        u8 *buf;
        u32 size;
        int raw = vd_asset_is_raw(a);
        if (!raw && !(a[4] < 0x10 && memcmp(a + 5, "Siff", 4) == 0)) continue;

        if (raw) {
            size = 8 + vd_be32(a + 4);
            buf = malloc(size);
            memcpy(buf, a, size);
        } else {
            size = vd_lzss_size(a);
            buf = malloc(size);
            vd_lzss_decompress(a, buf);
        }
        save(outdir, "siff", n_assets, o, buf, size);

        if (memcmp(buf + 8, "Sp3d", 4) == 0) {
            static VdSprite spr;
            if (vd_sprite_parse(buf, size, &spr) != 0) {
                printf("FAIL sprite %d @%06X\n", n_assets, o);
                failures++;
            } else {
                u16 *px = malloc((size_t)spr.width * spr.height * 2);
                vd_sprite_render_argb1555(&spr, px);
                if (outdir) {
                    char nm[64];
                    snprintf(nm, sizeof nm, "sprite_%ux%u", spr.width, spr.height);
                    save(outdir, nm, n_assets, o, px, (size_t)spr.width * spr.height * 2);
                }
                free(px);
                n_sprites++;
            }
        } else {
            VdJpeg64Info ji;
            if (vd_jpeg64_info(buf, size, &ji) == 0) {
                u16 *px = malloc(ji.width * ji.height * 2);
                u32 used = vd_jpeg64_decode(&ji, px, ji.width);
                int slack = (int)ji.data_size - (int)used;
                if (slack < 0 || slack > 3) {
                    printf("FAIL photo %d @%06X used %u of %u\n", n_assets, o, used, ji.data_size);
                    failures++;
                }
                if (outdir) {
                    char nm[64];
                    snprintf(nm, sizeof nm, "photo_%ux%u", ji.width, ji.height);
                    save(outdir, nm, n_assets, o, px, ji.width * ji.height * 2);
                }
                free(px);
                n_photos++;
            }
        }
        free(buf);
        n_assets++;
    }
    printf("assets %d  sprites %d  photos %d  failures %d\n", n_assets, n_sprites, n_photos, failures);
    free(rom);
    return failures ? 1 : 0;
}
