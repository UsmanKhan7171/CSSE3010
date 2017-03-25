#include "s4396122_util_queue.h"

Queue* s4396122_util_queue_create() {
    Queue *q = malloc(sizeof(Queue));
    q->size = 0;
    q->head = NULL;
    q->tail = NULL;
    return q;
}

int s4396122_util_queue_size(Queue *q) {
    return q->size;
}

void s4396122_util_queue_push(Queue *q, void *d) {
    struct queue *qElement = malloc(sizeof(struct queue));
    qElement->data = d;
    if (q->size) {
        q->tail->next = qElement;
    } else {
        q->head = qElement;
    }
    q->tail = qElement;
    q->size++;
}

void* s4396122_util_queue_pop(Queue *q) {
    if (q->head == NULL) {
        return NULL;
    }
    struct queue *oldHead = q->head;
    void *d = oldHead->data;
    if (oldHead->next != NULL) {
        q->head = oldHead->next;
    }
    free(oldHead);
    q->size--;
    return d;
}
