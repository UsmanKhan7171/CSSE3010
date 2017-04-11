/**
 * @file   s4396122_util_func_queue.h
 * @author Daniel Fitzmaurice = 43961229
 * @date   120417
 * @brief  Library for function queue / scheduler
 */
#ifndef UTIL_FUNC_QUEUE_H
#define UTIL_FUNC_QUEUE_H

#include <board.h>
#include <stdlib.h>
#include "s4396122_util_linkedlist.h"

struct funcPair {
    void (*function)(void);
    int intervalTime;
    unsigned int lastTrigger;
    int active;
};

typedef struct {
    LinkedList *queue;
} FuncQueue;

FuncQueue* s4396122_util_func_queue_create();
void s4396122_util_func_queue_add(FuncQueue *queue, void (*function)(void), int intervalTime);
void s4396122_util_func_queue_tick(FuncQueue *queue);
void s4396122_util_func_queue_pause(FuncQueue *queue, int pos);
void s4396122_util_func_queue_resume(FuncQueue *queue, int pos);

#endif
