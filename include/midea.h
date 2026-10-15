#ifndef MIDEA_H
#define MIDEA_H

size_t midea_encode(uint16_t *out, const uint8_t f[6]);
bool midea_decode(const uint16_t *t, size_t len, uint8_t out[6]);

#endif 