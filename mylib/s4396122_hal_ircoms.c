#include "s4396122_hal_ircoms.h"

void s4396122_hal_ircoms_send(int hammingCode) {

}

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

Queue* s4396122_hal_ircoms_encode(unsigned int hammingCode) {
    Queue *result = s4396122_util_queue_create();
    manchester_encode(result, 1);
    manchester_encode(result, 1);
    for (int i = 0; i < 8; i++) {
        int bit = (hammingCode & (1 << i));
        manchester_encode(result, bit);
    }
    manchester_encode(result, 0);
    return result;
}

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
