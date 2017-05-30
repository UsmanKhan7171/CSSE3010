/**
 * @file s4396122_util_queue.h
 * @brief Library for adding Queue functionality
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef UTIL_QUEUE_H
#define UTIL_QUEUE_H

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief A single Queue element
 */
struct queue {
    struct queue *next; //!< Pointer to the next item in the Queue
    void *data;         //!< Pointer to the data for this element
};

/**
 * @brief Structure of all the information required for a Queue
 */
typedef struct {
    struct queue *head; //!< Pointer to the head of the Queue
    struct queue *tail; //!< Pointer to the tail of the Queue
    int size;           //!< Size of the Queue
} Queue;

Queue* s4396122_util_queue_create();
int s4396122_util_queue_size(Queue *queue);
void s4396122_util_queue_push(Queue *queue, void *data);
void* s4396122_util_queue_pop(Queue *queue);
void s4396122_util_queue_free(Queue *queue);

#endif
