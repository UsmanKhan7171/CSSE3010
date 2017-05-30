/**
 * @file s4396122_util_func_queue.h
 * @brief Library for function queue / scheduler
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef UTIL_FUNC_QUEUE_H
#define UTIL_FUNC_QUEUE_H

#include <board.h>
#include <stdlib.h>
#include "s4396122_util_linkedlist.h"

/**
 * @brief Used to pair up a function to an interval
 */
struct funcPair {
    void (*function)(void);     //!< Function to be called
    int intervalTime;           //!< Interval to call function on
    unsigned int lastTrigger;   //!< Time it was last triggered
    int active;                 //!< If the current element is active
};

/**
 * @brief A struct containing all the required FuncQueue info
 */
typedef struct {
    LinkedList *queue;  //!< LinkedList of all the funcPairs
} FuncQueue;

FuncQueue* s4396122_util_func_queue_create();
void s4396122_util_func_queue_add(FuncQueue *queue, void (*function)(void), int intervalTime);
void s4396122_util_func_queue_tick(FuncQueue *queue);
void s4396122_util_func_queue_pause(FuncQueue *queue, int pos);
void s4396122_util_func_queue_resume(FuncQueue *queue, int pos);

#endif
