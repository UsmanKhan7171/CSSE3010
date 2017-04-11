#ifndef HAL_HAMMING_H
#define HAL_HAMMING_H

#include "s4396122_util_matrix.h"

Matrix *GEncoder;
Matrix *HDecoder;

void s4396122_hal_hamming_init();
unsigned int s4396122_hal_hamming_encode(unsigned int in);
unsigned int s4396122_hal_hamming_decode(unsigned int in);

#endif
