/**
 * @file s4396122_util_func_queue.c
 * @brief Library for function queue / scheduler
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_util_func_queue.h"

/**
 * @brief Creates an empty function queue for use
 * @return Pointer to FuncQueue for use
 */
FuncQueue* s4396122_util_func_queue_create() {

    FuncQueue *queue = malloc(sizeof(FuncQueue));
    queue->queue = s4396122_util_list_create();     // Creates empty linkedlist
    return queue;
}

/**
 * @brief Adds a function to the queue and sets the interval time for the 
 * function to be called
 * @param queue        FuncQueue to add function and interval to
 * @param function     Function to be called after said interval time
 * @param intervalTime Timeout for function to be called after. It is possible
 * for the function to be called after the time and not before
 */
void s4396122_util_func_queue_add(FuncQueue *queue, void (*function)(void), int intervalTime) {

    struct funcPair *newPair = malloc(sizeof(struct funcPair));
    newPair->active = 1;
    newPair->intervalTime = intervalTime;
    newPair->lastTrigger = HAL_GetTick();
    newPair->function = function;
    // Adds the pair to the queue list
    s4396122_util_list_add(queue->queue, newPair);
}

/**
 * @brief The tick function for the FuncQueue. This will execute any functions 
 * with the matched interval time
 * @param queue The FuncQueue to interate through and run the functions from
 */
void s4396122_util_func_queue_tick(FuncQueue *queue) {

    int numQueues = s4396122_util_list_size(queue->queue);
    struct linkedlist *pos = queue->queue->head;
    int minRemaining = 10000;
    for (int i = 0; i < numQueues; i++) {
        // For all the pairs in the queue, check if the time has been set for a
        // trigger
        struct funcPair *pair = (struct funcPair *) pos->data;
        // Set the current tick to a variable to try and reduce processing time
        unsigned int currentTick = HAL_GetTick();
        if (currentTick >= (pair->lastTrigger + pair->intervalTime)) {

            // If the trigger has been activated, then set trigger holder to
            // the current trigger
            pair->lastTrigger = currentTick;
            if (pair->active) {     // If the trigger is active, execute it

                pair->function();
            }
        }
        pos = pos->next;
    }
}

/**
 * @brief Pauses the Function at position pos
 * @param queue The FuncQueue to access the element from
 * @param pos   Position to pause from
 */
void s4396122_util_func_queue_pause(FuncQueue *queue, int pos) {

    struct funcPair *pair = s4396122_util_list_get(queue->queue, pos);
    pair->active = 0;
}

/**
 * @brief Resumes the Function at position pos
 * @param queue The FuncQueue to access the element from
 * @param pos   Position to resume from
 */
void s4396122_util_func_queue_resume(FuncQueue *queue, int pos) {

    struct funcPair *pair = s4396122_util_list_get(queue->queue, pos);
    pair->active = 1;
}
