/**
 * @file s4396122_util_int_queue.h
 * @brief Library for easily creating and managing a Queue of integers
 * @author Daniel Fitzmaurice - 43961229
 * @version 1.0
 * @date 2017-04-30
 */
#ifndef UTIL_INT_QUEUE_H
#define UTIL_INT_QUEUE_H

#include <stdio.h>
#include <stdlib.h>
#include "s4396122_util_print.h"

struct intQueue {
    struct intQueue *next;
    int data;
};

typedef struct {
    struct intQueue *head;
    struct intQueue *tail;
    int size;
} IntQueue;

IntQueue* s4396122_util_int_queue_create();
int s4396122_util_int_queue_size(IntQueue *q);
void s4396122_util_int_queue_push(IntQueue *q, int d);
int s4396122_util_int_queue_pop(IntQueue *q);
void s4396122_util_int_queue_free(IntQueue *q);

void s4396122_util_int_queue_from_string(IntQueue *q, char *message);
void s4396122_util_int_queue_to_string(IntQueue *q, char *buffer);
int s4396122_util_int_queue_to_integer(IntQueue *q);

#endif
