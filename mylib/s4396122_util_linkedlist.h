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

LinkedList* s4396122_util_create_list();
void s4396122_util_free_list(LinkedList *list);
void s4396122_util_add_data(LinkedList *list, void *data);
void* s4396122_util_get_data(LinkedList *list, int pos);
void s4396122_util_remove_data(LinkedList *list, int pos);
int s4396122_util_get_size(LinkedList *list);

#endif
