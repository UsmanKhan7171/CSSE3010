#include "s4396122_hal_hamming.h"

int modulate_matrix(int in) {
    return in % 2;
}

void s4396122_hal_hamming_init() {
    int GRow1[3] = {1, 1, 0};
    int GRow2[3] = {0, 1, 1};
    int GRow3[3] = {1, 0, 1};
    int GRow4[3] = {1, 1, 1};
    GEncoder = s4396122_util_matrix_gen(3, 4, GRow1, GRow2, GRow3, GRow4);
    debug_printf("Got G Matrix\n");
    s4396122_util_matrix_print(GEncoder);
}

unsigned int s4396122_hal_hamming_encode(unsigned int in) {
    unsigned int d0 = !!(in & 0x01);
    unsigned int d1 = !!(in & 0x02);
    unsigned int d2 = !!(in & 0x04);
    unsigned int d3 = !!(in & 0x08);

    int row[4] = {d3, d2, d1, d0};
    Matrix *DataInput = s4396122_util_matrix_gen(4, 1, row);
    Matrix *result = s4396122_util_matrix_multi(DataInput, GEncoder);
    s4396122_util_matrix_exec(result, &modulate_matrix);

    int h2 = s4396122_util_matrix_get(result, 0, 0);
    int h1 = s4396122_util_matrix_get(result, 0, 1);
    int h0 = s4396122_util_matrix_get(result, 0, 2);

    unsigned int out = (h0 << 1) | (h1 << 2) | (h2 << 3) | (d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

    unsigned int p0 = 0;
    for (int i = 0; i < 8; i++) {
        p0 ^= !!(out & (1 << i));
    }
    out |= p0;

    s4396122_util_matrix_free(DataInput);
    s4396122_util_matrix_free(result);

    return out;
}
