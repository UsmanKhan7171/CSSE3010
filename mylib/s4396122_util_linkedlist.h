/**
 * @file s4396122_util_linkedlist.h
 * @brief Library for adding linkedlist functionality
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef UTIL_LINKEDLIST_H
#define UTIL_LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>

/**
 * @brief A linkedlist element
 */
struct linkedlist {
    struct linkedlist *prev;    //!< Pointer to the prev element in the array
    struct linkedlist *next;    //!< Pointer to the next element in the array
    void *data;                 //!< Pointer to the data being stored
};

/**
 * @brief Structure containing all the information needed for a linkedlist
 */
typedef struct {
    struct linkedlist *head;    //!< Pointer to the head to the list
    int size;                   //!< Size of the list
} LinkedList;

LinkedList* s4396122_util_list_create();
void s4396122_util_list_free(LinkedList *list);
void s4396122_util_list_add(LinkedList *list, void *data);
void* s4396122_util_list_get(LinkedList *list, int pos);
void s4396122_util_list_remove(LinkedList *list, int pos);
int s4396122_util_list_size(LinkedList *list);

#endif
