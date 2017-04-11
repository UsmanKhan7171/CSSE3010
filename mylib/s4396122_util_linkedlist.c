/**
 * @file   s4396122_util_linkedlist.c
 * @author Daniel Fitzmaurice = 43961229
 * @date   120417
 * @brief  Library for adding linkedlist functionality
 */
#include "s4396122_util_linkedlist.h"

/**
 * Creates and empty LinkedList in memory
 * @return Pointer to LinkedList
 */
LinkedList* s4396122_util_list_create() {
    LinkedList *list = malloc(sizeof(LinkedList));
    list->size = 0;
    list->head = NULL;
    return list;
}

/**
 * Frees up the LinkedList and any list elements from memory
 * @param list LinkedList to be freed from memory
 */
void s4396122_util_list_free(LinkedList *list) {
    struct linkedlist *pos = list->head;
    for (int i = 0; i < list->size; i++) {
        struct linkedlist *temp = pos->next;
        free(pos);
        pos = temp;
    }
    free(list);
}

/**
 * Adds an element to the LinkedList
 * @param list LinkedList for data to be added to
 * @param data Pointer to data to be added to the list
 */
void s4396122_util_list_add(LinkedList *list, void *data) {
    struct linkedlist *pos = list->head;
    if (pos != NULL) {
        while (pos->next != NULL) {
            pos = pos->next;
        }
    }

    struct linkedlist *newList = malloc(sizeof(struct linkedlist));
    newList->prev = NULL;
    newList->next = NULL;
    newList->data = data;
    if (pos == NULL) {
        list->head = newList;
    } else {
        pos->next = newList;
        newList->prev = pos;
    }

    list->size++;
}

/**
 * Gets an element from the position in the LinkedList
 * @param list LinkedList to retreve the element from
 * @param pos  Position in list to return
 */
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

/**
 * Removes an element from the LinkedList from position
 * @param list LinkedList to remove the element from
 * @param pos  Position in LinkedList to remove from
 */
void s4396122_util_list_remove(LinkedList *list, int pos) {
    struct linkedlist *posList = list->head;
    if (pos > list->size || posList == NULL) {
        return;
    }

    for (int i = 0; i < pos; i++) {
        posList = posList->next;
    }

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

/**
 * Returns the size of the LinkedList
 * @param  list LinkedList for the size to be collected from
 * @return      Number of elements in LinkedList
 */
int s4396122_util_list_size(LinkedList *list) {
    return list->size;
}
