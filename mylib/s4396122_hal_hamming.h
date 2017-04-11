/**
 * @file   s4396122_hal_hamming.h
 * @author Daniel Fitzmaurice = 43961229
 * @date   120417
 * @brief  Library for encoding and decoding of hamming codes
 */
#ifndef HAL_HAMMING_H
#define HAL_HAMMING_H

#include "s4396122_util_matrix.h"

Matrix *GEncoder;
Matrix *HDecoder;

void s4396122_hal_hamming_init();
unsigned int s4396122_hal_hamming_encode(unsigned int in);
unsigned int s4396122_hal_hamming_decode(unsigned int in);

#endif
