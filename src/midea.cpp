#include <Arduino.h>

uint8_t reverse_bits(uint8_t b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

uint8_t midea_checksum(const uint8_t *f) {
  uint8_t sum = 0;
  for (int i = 0; i < 5; i++) sum += reverse_bits(f[i]);
  return reverse_bits((uint8_t)(0u - sum));
}

size_t midea_encode(uint16_t *out, const uint8_t f[6]) {
  size_t n = 0;
  for (int copy = 0; copy < 2; copy++) {
    out[n++] = 4330; out[n++] = 4470; // header timing space
    for (int i = 0; i < 48; i++) {
      bool bit = f[i / 8] & (0x80 >> (i % 8));
      if (copy) bit = !bit;
      out[n++] = 480;
      out[n++] = bit ? 1670 : 620; // bit timing gap
    }
    out[n++] = 480; // stop bit 
    if (copy == 0) out[n++] = 5270; // frame gap
  }
  return n; // size 
}

bool midea_decode(const uint16_t *t, size_t len, uint8_t out[6]) {
  if (len < 99) return false;
  if (t[0] < 3500 || t[0] > 5500 || t[1] < 3500 || t[1] > 5500) return false;  // header checks
  memset(out, 0, 6);
  for (int i = 0; i < 48; i++)
    if (t[3 + 2 * i] > 1000) out[i / 8] |= 0x80 >> (i % 8);  // long space = 1
  return out[5] == midea_checksum(out); 
}
