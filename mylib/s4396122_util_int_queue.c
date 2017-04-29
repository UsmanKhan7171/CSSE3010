#include "s4396122_util_int_queue.h"

IntQueue* s4396122_util_int_queue_create() {
    IntQueue *q = malloc(sizeof(IntQueue));
    q->size = 0;
    q->head = NULL;
    return q;
}

int s4396122_util_int_queue_size(IntQueue *q) {
    return q->size;
}

void s4396122_util_int_queue_push(IntQueue *q, int d) {
    struct intQueue *qElement = malloc(sizeof(struct intQueue));
    qElement->next = NULL;
    qElement->data = d;
    if (q->head == NULL) {
        q->head = qElement;
    } else {
        q->tail->next = qElement;
    }
    q->tail = qElement;
    q->size++;
}

int s4396122_util_int_queue_pop(IntQueue *q) {
    if (q->head == NULL) {
        return 0;
    }
    struct intQueue *oldHead = q->head;
    int d = oldHead->data;
    q->head = oldHead->next;
    free(oldHead);
    q->size--;
    return d;
}

void s4396122_util_int_queue_free(IntQueue *q) {
    while (q->size > 0) {
        s4396122_util_queue_pop(q);
    }
    free(q);
}
