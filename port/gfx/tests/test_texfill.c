/* Host test of the PVR texture layout (pvr_texfill.h): edge replication and the
 * wrap-side copies of row 0 / column 0 that the PVR's bilinear filter reads for
 * samples before the first row / column. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pvr_texfill.h"

static int fails;
#define CHECK(c, ...) do { if (!(c)) { fails++; printf("FAIL: " __VA_ARGS__); printf("\n"); } } while (0)

static uint16_t buf[512 * 512], rowb[1024];

static void strip(int alpha_bit)
{
    /* a photo-strip window: 321x6 RGBA16, clamped in s and t */
    static uint16_t px[321 * 6];
    F3DTexImage img;
    PvrTexLayout L;
    int x, y;
    for (y = 0; y < 6; y++)
        for (x = 0; x < 321; x++)
            px[y * 321 + x] = (uint16_t)(((x * 7 + y * 131) & 0xFFFE) | alpha_bit);
    memset(&img, 0, sizeof(img));
    img.width = 321; img.height = 6; img.siz = 2; img.alpha = alpha_bit ? 0 : 1; img.pad = 3; img.rgba16 = px;
    pvrtf_layout(&img, &L);
    CHECK(L.w2 == 512 && L.h2 == 8 && L.fw == 322 && L.fh == 7, "strip layout %d %d %d %d", L.w2, L.h2, L.fw, L.fh);
    CHECK(L.wrap_row && L.wrap_col, "strip wrap padding");
    memset(buf, 0xAB, sizeof(buf));
    pvrtf_fill(&img, &L, buf, rowb);
    for (y = 0; y < 6; y++) {
        CHECK(buf[y * 512 + 321] == buf[y * 512 + 320], "right edge pad row %d", y);
        CHECK(buf[y * 512 + 511] == buf[y * 512 + 0], "wrap column row %d", y);
    }
    for (x = 0; x < 322; x++) {
        CHECK(buf[6 * 512 + x] == buf[5 * 512 + x], "bottom edge pad col %d", x);
        CHECK(buf[7 * 512 + x] == buf[0 * 512 + x], "wrap row col %d", x);
    }
    CHECK(buf[7 * 512 + 511] == buf[0], "wrap corner");
}

/* rgba16 rows read with a pitch (and from a 2-aligned pointer) give the same
 * texture as the ARGB8888 path of the expanded colours */
static uint32_t exp5551(uint16_t c)
{
    uint32_t r = (c >> 11) & 31, g = (c >> 6) & 31, b = (c >> 1) & 31;
    r = (r << 3) | (r >> 2); g = (g << 3) | (g >> 2); b = (b << 3) | (b >> 2);
    return ((c & 1) ? 0xFF000000u : 0) | (r << 16) | (g << 8) | b;
}

static void stride_equiv(int w, int h, int pitch, int off, int alpha, int pad)
{
    static uint16_t src[700 * 16 + 8], b2[512 * 512];
    static uint32_t argb[700 * 16];
    F3DTexImage a, b;
    PvrTexLayout La, Lb;
    int x, y, i, bad = 0;
    for (i = 0; i < 700 * 16 + 8; i++)
        src[i] = (uint16_t)((i * 2654435761u) >> 13) | (alpha ? 0 : 1);
    for (y = 0; y < h; y++)
        for (x = 0; x < w; x++)
            argb[y * w + x] = exp5551(src[off + y * pitch + x]);
    memset(&a, 0, sizeof(a));
    a.width = (uint16_t)w; a.height = (uint16_t)h; a.siz = 2; a.alpha = (uint8_t)(alpha ? 1 : 0); a.pad = (uint8_t)pad;
    b = a;
    a.argb = argb;
    b.rgba16 = src + off; b.stride = (uint32_t)pitch;
    pvrtf_layout(&a, &La);
    pvrtf_layout(&b, &Lb);
    memset(buf, 0x5A, sizeof(buf));
    memset(b2, 0x5A, sizeof(b2));
    pvrtf_fill(&a, &La, buf, rowb);
    pvrtf_fill(&b, &Lb, b2, rowb);
    for (y = 0; y < La.h2; y++)
        for (x = 0; x < La.w2; x++)
            if ((y < La.fh || (La.wrap_row && y == La.h2 - 1)) && (x < La.fw || (La.wrap_col && x >= La.w2 - 2)))
                bad += buf[y * La.w2 + x] != b2[y * La.w2 + x];
    CHECK(bad == 0, "stride rgba16 == argb path (w %d h %d pitch %d off %d alpha %d pad %d): %d texels differ",
          w, h, pitch, off, alpha, pad, bad);
}

int main(void)
{
    F3DTexImage img;
    PvrTexLayout L;
    static uint32_t argb[64 * 64];
    strip(1);
    strip(0);
    stride_equiv(321, 6, 640, 3, 0, 3);
    stride_equiv(319, 6, 640, 1, 0, 3);
    stride_equiv(320, 6, 640, 2, 1, 3);
    stride_equiv(321, 6, 321, 0, 1, 3);
    stride_equiv(5, 3, 9, 1, 0, 0);
    stride_equiv(5, 3, 9, 1, 1, 3);
    stride_equiv(16, 16, 700, 7, 0, 0);
    /* power-of-two clamped texture: no spare row / column, nothing extra */
    memset(&img, 0, sizeof(img));
    img.width = 64; img.height = 64; img.pad = 3; img.argb = argb;
    pvrtf_layout(&img, &L);
    CHECK(!L.wrap_row && !L.wrap_col && L.fw == 64 && L.fh == 64, "pow2 layout");
    /* small wrapped texture: tiled, no wrap copies */
    img.width = 4; img.height = 4; img.pad = 0;
    pvrtf_layout(&img, &L);
    CHECK(!L.wrap_row && !L.wrap_col && L.fw == 8 && L.fh == 8, "small wrapped layout");
    printf("texfill: %d failures\n", fails);
    return fails != 0;
}
