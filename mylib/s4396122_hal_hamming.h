/**
 * @file s4396122_hal_hamming.h
 * @brief Library for encoding and decoding of hamming codes
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef HAL_HAMMING_H
#define HAL_HAMMING_H

#include "s4396122_util_matrix.h"

Matrix *GEncoder;   //!< Matrix containing the G encoding
Matrix *HDecoder;   //!< Matrix containing the H decoding

void s4396122_hal_hamming_init();
unsigned int s4396122_hal_hamming_encode(unsigned int in);
unsigned int s4396122_hal_hamming_decode(unsigned int in);

#endif
