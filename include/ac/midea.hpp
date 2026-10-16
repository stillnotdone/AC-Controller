#ifndef AC_MIDEA_H
#define AC_MIDEA_H

#include <Arduino.h>

size_t midea_encode(uint16_t *out, const uint8_t f[6]);
bool midea_decode(const uint16_t *t, size_t len, uint8_t out[6]);
uint8_t midea_checksum(const uint8_t *f);

#endif 