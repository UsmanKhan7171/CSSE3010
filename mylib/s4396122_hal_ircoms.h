/**
 * @file s4396122_hal_ircoms.h
 * @brief Library for encoding and decoding of data to manchester waveforms
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef HAL_IRCOMS_H
#define HAL_IRCOMS_H

#include "s4396122_util_queue.h"

//! Number of manchester bits in a byte
#define MANCHESTER_BYTE_SIZE 11
//! Number of bits in a hamming encoded manchester byte
#define MANCHESTER_ENCODED_SIZE MANCHESTER_BYTE_SIZE * 4

Queue* s4396122_hal_ircoms_encode(unsigned int hammingCode);
unsigned int s4396122_hal_ircoms_decode(Queue *inQueue);

#endif
