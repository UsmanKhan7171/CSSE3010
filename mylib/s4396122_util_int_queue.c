/**
 * @file s4396122_util_int_queue.c
 * @brief Library for easily creating and managing a Queue of integers
 * @author Daniel Fitzmaurice - 43961229
 * @version 1.0
 * @date 2017-04-30
 */
#include "s4396122_util_int_queue.h"

/**
 * @brief Creates an integer queue
 *
 * @return Returns a struct containing all the required information about the
 * queue
 */
IntQueue* s4396122_util_int_queue_create() {
    IntQueue *q = malloc(sizeof(IntQueue));
    q->size = 0;
    q->head = NULL;
    return q;
}

/**
 * @brief Gets the size of the queue
 *
 * @param q Queue to check the size of
 *
 * @return integer representing the size of the queue
 */
int s4396122_util_int_queue_size(IntQueue *q) {
    return q->size;
}

/**
 * @brief Pushes an element into the tail of the queue
 *
 * @param q Queue to push the data to
 * @param d Data to be added to the queue
 */
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

/**
 * @brief Pops an element from the head of the queue
 *
 * @param q Queue for the data to be popped from
 *
 * @return integer from the head of the queue
 */
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

/**
 * @brief Frees the whole queue from memory, as well as all the elements inside
 *
 * @param q Queue to be freed
 */
void s4396122_util_int_queue_free(IntQueue *q) {
    while (q->size > 0) {
        s4396122_util_queue_pop(q);
    }
    free(q);
}
