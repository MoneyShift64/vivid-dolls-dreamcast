/* Vivid Dolls port - SHA-1 (FIPS 180-1), for checking the player's ROM. */
#include <string.h>
#include "vd_sha1.h"

#define ROL(v, n) (((v) << (n)) | ((v) >> (32 - (n))))

static void vd_sha1_block(uint32_t h[5], const uint8_t *p)
{
    uint32_t w[80], a, b, c, d, e, f, k, t;
    int i;

    for (i = 0; i < 16; i++)
        w[i] = ((uint32_t)p[i * 4] << 24) | ((uint32_t)p[i * 4 + 1] << 16) |
               ((uint32_t)p[i * 4 + 2] << 8) | p[i * 4 + 3];
    for (i = 16; i < 80; i++)
        w[i] = ROL(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
    a = h[0]; b = h[1]; c = h[2]; d = h[3]; e = h[4];
    for (i = 0; i < 80; i++) {
        if (i < 20)      { f = (b & c) | (~b & d);           k = 0x5A827999; }
        else if (i < 40) { f = b ^ c ^ d;                    k = 0x6ED9EBA1; }
        else if (i < 60) { f = (b & c) | (b & d) | (c & d);  k = 0x8F1BBCDC; }
        else             { f = b ^ c ^ d;                    k = 0xCA62C1D6; }
        t = ROL(a, 5) + f + e + k + w[i];
        e = d; d = c; c = ROL(b, 30); b = a; a = t;
    }
    h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e;
}

void vd_sha1(const void *data, size_t len, uint8_t out[20])
{
    uint32_t h[5] = { 0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0 };
    const uint8_t *p = data;
    uint8_t tail[128];
    size_t n = len, rest;
    uint64_t bits = (uint64_t)len * 8;
    int i;

    while (n >= 64) {
        vd_sha1_block(h, p);
        p += 64;
        n -= 64;
    }
    memset(tail, 0, sizeof(tail));
    memcpy(tail, p, n);
    tail[n] = 0x80;
    rest = (n < 56) ? 64 : 128;
    for (i = 0; i < 8; i++)
        tail[rest - 1 - i] = (uint8_t)(bits >> (i * 8));
    vd_sha1_block(h, tail);
    if (rest == 128)
        vd_sha1_block(h, tail + 64);
    for (i = 0; i < 20; i++)
        out[i] = (uint8_t)(h[i / 4] >> (24 - (i % 4) * 8));
}
