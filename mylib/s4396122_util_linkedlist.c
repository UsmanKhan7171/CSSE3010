#include "s4396122_util_linkedlist.h"

LinkedList* s4396122_util_list_create() {
    LinkedList *list = malloc(sizeof(LinkedList));
    list->size = 0;
    list->head = NULL;
    return list;
}

void s4396122_util_list_free(LinkedList *list) {
    struct linkedlist *pos = list->head;
    for (int i = 0; i < list->size; i++) {
        struct linkedlist *temp = pos->next;
        free(pos);
        pos = temp;
    }
}

void s4396122_util_list_add(LinkedList *list, void *data) {
    // debug_printf("Got: %d\n", *((int*) data));
    struct linkedlist *pos = list->head;
    if (pos != NULL) {
        while (pos->next != NULL) {
            // debug_printf("Loop\n");
            pos = pos->next;
        }
    }

    struct linkedlist *newList = malloc(sizeof(struct linkedlist));
    newList->prev = NULL;
    newList->next = NULL;
    newList->data = data;
    if (pos == NULL) {
        // debug_printf("Creating first element\n");
        list->head = newList;
    } else {
        pos->next = newList;
        newList->prev = pos;
    }

    list->size++;
}

void* s4396122_util_list_get(LinkedList *list, int pos) {
    struct linkedlist *posList = list->head;
    if (pos > list->size) {
        return;
    }
    for (int i = 0; i < pos; i++) {
        posList = posList->next;
    }
    return posList->data;
}

void s4396122_util_list_remove(LinkedList *list, int pos) {
    struct linkedlist *posList = list->head;
    if (pos > list->size || posList == NULL) {
        return;
    }

    for (int i = 0; i < pos; i++) {
        posList = posList->next;
    }

    debug_printf("Got: %d\n", *((int *) posList->data));
    debug_printf("Prev: %d\n", *((int *) posList->prev->data));
    debug_printf("Next: %d\n", *((int *) posList->next->data));

    if (posList->prev != NULL) {
        posList->prev->next = posList->next;
    } else {
        list->head = posList->next;
    }
    if (posList->next != NULL) {
        posList->next->prev = posList->prev;
    }

    free(posList);
    list->size--;
}

int s4396122_util_list_size(LinkedList *list) {
    return list->size;
}
