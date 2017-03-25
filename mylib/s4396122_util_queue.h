#ifndef UTIL_QUEUE_H
#define UTIL_QUEUE_H

#include <stdio.h>
#include <stdlib.h>

struct queue {
    struct queue *next;
    void *data;
};

typedef struct {
    struct queue *head;
    struct queue *tail;
    int size;
} Queue;

Queue* s4396122_util_queue_create();
int s4396122_util_queue_size(Queue *queue);
void s4396122_util_queue_push(Queue *queue, void *data);
void* s4396122_util_queue_pop(Queue *queue);

#endif
