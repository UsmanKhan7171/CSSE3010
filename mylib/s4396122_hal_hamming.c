#include "s4396122_hal_hamming.h"

unsigned int s4396122_hal_hamming_encode(unsigned int in) {
    unsigned int d0 = !!(in & 0x01);
    unsigned int d1 = !!(in & 0x02);
    unsigned int d2 = !!(in & 0x04);
    unsigned int d3 = !!(in & 0x08);

    unsigned int h0 = d0 ^ d1 ^ d2;
    unsigned int h1 = d0 ^ d2 ^ d3;
    unsigned int h2 = d0 ^ d1 ^ d3;

    unsigned int out = (h0 << 1) | (h1 << 2) | (h2 << 3) | (d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

    unsigned int p0 = 0;
    for (int i = 1; i < 8; i++) {
        p0 = p0 ^ !!(out & (1 << i));
    }

    out |= p0;

    return out;
}
