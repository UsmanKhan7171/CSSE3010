#include "s4396122_util_func_queue.h"

FuncQueue* s4396122_util_func_queue_create() {
    FuncQueue *queue = malloc(sizeof(FuncQueue));

    queue->queue = s4396122_util_list_create();
    return queue;
}

void s4396122_util_func_queue_add(FuncQueue *queue, void (*function)(void), int intervalTime) {
    struct funcPair *newPair = malloc(sizeof(struct funcPair));
    newPair->active = 1;
    newPair->intervalTime = intervalTime;
    newPair->lastTrigger = HAL_GetTick();
    newPair->function = function;
    s4396122_util_list_add(queue->queue, newPair);
}

void s4396122_util_func_queue_tick(FuncQueue *queue) {
    int numQueues = s4396122_util_list_size(queue->queue);
    unsigned int currentTick = HAL_GetTick();
    struct linkedlist *pos = queue->queue->head;
    for (int i = 0; i < numQueues; i++) {
        struct funcPair *pair = (struct funcPair *) pos->data;
        if (currentTick > (pair->lastTrigger + pair->intervalTime)) {
            pair->lastTrigger = currentTick;
            if (pair->active) {
                pair->function();
            }
        }
        pos = pos->next;
    }
}

void s4396122_util_func_queue_pause(FuncQueue *queue, int pos) {
    struct funcPair *pair = s4396122_util_list_get(queue->queue, pos);
    pair->active = 0;
}

void s4396122_util_func_queue_resume(FuncQueue *queue, int pos) {
    struct funcPair *pair = s4396122_util_list_get(queue->queue, pos);
    pair->active = 1;
}
