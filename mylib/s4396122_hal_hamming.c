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

    int HRow1[7] = {1, 0, 1, 1, 1, 0, 0};
    int HRow2[7] = {1, 1, 0, 1, 0, 1, 0};
    int HRow3[7] = {0, 1, 1, 1, 0, 0, 1};
    HDecoder = s4396122_util_matrix_gen(7, 3, HRow1, HRow2, HRow3);
}

unsigned int s4396122_hal_hamming_encode(unsigned int in) {
    unsigned int d0 = !!(in & 0x01);
    unsigned int d1 = !!(in & 0x02);
    unsigned int d2 = !!(in & 0x04);
    unsigned int d3 = !!(in & 0x08);

    int row[4] = {d3, d2, d1, d0};
    Matrix *dataInput = s4396122_util_matrix_gen(4, 1, row);
    Matrix *result = s4396122_util_matrix_multi(dataInput, GEncoder);
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

    s4396122_util_matrix_free(dataInput);
    s4396122_util_matrix_free(result);

    return out;
}

unsigned int s4396122_hal_hamming_decode(unsigned int in) {
    unsigned int data = in >> 4;

    unsigned int p0 = 0;
    for (int i = 1; i < 8; i++) {
        p0 ^= !!(in & (1 << i));
    }

    int row[7][1];
    for (int i = 0; i < 7; i++) {
        row[i][0] = !!(in & (1 << (i + 1)));
    }
    Matrix *dataInput = s4396122_util_matrix_gen(1, 7, row[6], row[5], row[4], row[3], row[2], row[1], row[0]);
    Matrix *result = s4396122_util_matrix_multi(HDecoder, dataInput);
    s4396122_util_matrix_exec(result, &modulate_matrix);

    if (s4396122_util_matrix_get(result, 0, 0) || s4396122_util_matrix_get(result, 1, 0) || s4396122_util_matrix_get(result, 2, 0)) {
        int keyLocation = -1;
        for (int i = 0; i < 7; i++) {
            if (s4396122_util_matrix_get(result, 0, 0) == s4396122_util_matrix_get(HDecoder, i, 0)) {
                if (s4396122_util_matrix_get(result, 1, 0) == s4396122_util_matrix_get(HDecoder, i, 1) && s4396122_util_matrix_get(result, 2, 0) == s4396122_util_matrix_get(HDecoder, i, 2)) {
                    keyLocation = i;
                }
            }
        }
        if ((in & 1) == p0) {
            debug_printf("Too many errors\n");
            data = 0xFF;
        } else if (keyLocation == -1) {
            // TODO: Request for new data
            debug_printf("Too many errors\n");
            data = 0xFF;
        } else if (keyLocation < 4) {
            data ^= (1 << (3 - keyLocation));
            debug_printf("Error in %d bit\n", 3 - keyLocation);
        } else {
            debug_printf("Error in hamming bit\n");
        }
    }

    s4396122_util_matrix_free(result);
    s4396122_util_matrix_free(dataInput);

    return data;
}
