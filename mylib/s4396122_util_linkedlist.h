/**
 * @file   s4396122_util_linkedlist.h
 * @author Daniel Fitzmaurice = 43961229
 * @date   120417
 * @brief  Library for adding linkedlist functionality
 */
#ifndef UTIL_LINKEDLIST_H
#define UTIL_LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>

struct linkedlist {
    struct linkedlist *prev;
    struct linkedlist *next;
    void *data;
};

typedef struct {
    struct linkedlist *head;
    int size;
} LinkedList;

LinkedList* s4396122_util_list_create();
void s4396122_util_list_free(LinkedList *list);
void s4396122_util_list_add(LinkedList *list, void *data);
void* s4396122_util_list_get(LinkedList *list, int pos);
void s4396122_util_list_remove(LinkedList *list, int pos);
int s4396122_util_list_size(LinkedList *list);

#endif
