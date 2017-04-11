/**
 * @file   s4396122_util_map.c
 * @author Daniel Fitzmaurice = 43961229
 * @date   120417
 * @brief  Library for adding HashMap functionality
 */
#include "s4396122_util_map.h"

/**
 * Creates a number between 0 and MAP_SIZE for hashing key values
 * @param  key The key to be hashed
 * @return     The hashed key
 */
int generateHash(int key) {
    return key % MAP_SIZE;
}

/**
 * Creates and empty map for adding data
 * @return The Map pointer that was created
 */
Map* s4396122_util_map_create() {
    Map *m = malloc(sizeof(Map));
    m->map = malloc(sizeof(LinkedList*) * MAP_SIZE);
    for (int i = 0; i < MAP_SIZE; i++) {
        m->map[i] = s4396122_util_list_create();
    }
    m->size = 0;
    return m;
}

/**
 * Frees the Map from memory
 * @param m The Map to be freed from memory
 */
void s4396122_util_map_free(Map *m) {
    for (int i = 0; i < MAP_SIZE; i++) {
        s4396122_util_list_free(m->map[i]);
    }
    free(m->map);
    free(m);
}

/**
 * Adds an element to the map, if the key exists in memory than the old pair
 * will be removed from memory
 * @param m   The Map to add the data to
 * @param key The key for the new data to be referenced from
 * @param d   Data to be added with the key
 */
void s4396122_util_map_add(Map *m, int key, void *d) {
    LinkedList *list = m->map[generateHash(key)];
    struct linkedlist *posList = list->head;
    int foundKey = 0;

    struct MapPair *newPair = malloc(sizeof(struct MapPair));
    newPair->key = key;
    newPair->data = d;

    for (int i = 0; i < list->size; i++) {
        struct MapPair *pair = posList->data;
        if (pair->key == key) {
            free(pair);
            posList->data = newPair;
            foundKey = 1;
            break;
        }
        posList = posList->next;
    }
    if (!foundKey) {
        s4396122_util_list_add(list, newPair);
        m->size++;
    }
}

/**
 * Gets the data referenced with the key
 * @param m   Map to get the data from
 * @param key Key to get the data from
 */
void* s4396122_util_map_get(Map *m, int key) {
    LinkedList *list = m->map[generateHash(key)];
    struct linkedlist *posList = list->head;

    for (int i = 0; i < list->size; i++) {
        struct MapPair *pair = posList->data;
        if (pair->key == key) {
            return pair->data;
        }
        posList = posList->next;
    }

    return NULL;
}
