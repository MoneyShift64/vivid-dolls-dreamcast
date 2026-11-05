#ifndef VD_SHA1_H
#define VD_SHA1_H
#include <stddef.h>
#include <stdint.h>
void vd_sha1(const void *data, size_t len, uint8_t out[20]);
#endif
