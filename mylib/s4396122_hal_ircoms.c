/**
 * @file s4396122_hal_ircoms.c
 * @brief Library for encoding and decoding of data to manchester waveforms
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_hal_ircoms.h"

/**
 * @brief Encodes a bit into a Queue
 * @param q   Queue to add the encoded bit to
 * @param bit Bit to be encoded
 */
void manchester_encode(Queue *q, unsigned int bit) {

    int *d = malloc(sizeof(int));
    *d = 0;
    int *e = malloc(sizeof(int));
    *e = 1;
    if (bit) {

        s4396122_util_queue_push(q, d);
        s4396122_util_queue_push(q, e);
    } else {

        s4396122_util_queue_push(q, e);
        s4396122_util_queue_push(q, d);
    }
}

/**
 * @brief Encodes a byte of data into a Queue of manchester waveform
 * @param  hammingCode Data to be encoded
 * @return             Resulting Queue with manchester coding
 */
Queue* s4396122_hal_ircoms_encode(unsigned int hammingCode) {

    Queue *result = s4396122_util_queue_create();
    // Add the start bits
    manchester_encode(result, 1);
    manchester_encode(result, 1);
    for (int i = 0; i < 8; i++) {
        int bit = (hammingCode & (1 << i));
        manchester_encode(result, bit);
    }
    // Add the stop bit
    manchester_encode(result, 0);
    return result;
}

/**
 * @brief Decodes one group of bits and returns the result
 * @param  inQueue Queue to extract bits from
 * @return         Either 1 or 0 based on the bits decoded
 */
int manchester_decode(Queue *inQueue) {

    int *a = s4396122_util_queue_pop(inQueue);
    int *b = s4396122_util_queue_pop(inQueue);
    int bit = -1;
    if (*a == 0 && *b == 1) {

        bit = 1;
    } else if (*a == 1 && *b == 0) {

        bit = 0;
    } else {

        debug_printf("Error in coding: (%d, %d)\n", *a, *b);
    }
    free(a);
    free(b);
    return bit;
}

/**
 * @brief Decodes a Queue of manchester bits and returns an int of the decoded 
 * Queue
 * @param  inQueue Queue to be decoded
 * @return         Decoded int of the queue
 */
unsigned int s4396122_hal_ircoms_decode(Queue *inQueue) {

    if (manchester_decode(inQueue) != 1 || manchester_decode(inQueue) != 1) {

        debug_printf("Missing Start Bits\n");
        return 0;
    }
    unsigned int result = 0;
    for (int i = 0; i < 8; i++) {
        result |= (manchester_decode(inQueue) << i);
    }
    if (manchester_decode(inQueue) != 0) {

        debug_printf("Missing End Bit\n");
        return 0;
    }
    return result;
}
