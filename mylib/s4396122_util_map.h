/**
 * @file s4396122_util_map.h
 * @brief Library for adding HashMap functionality
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef UTIL_MAP_H
#define UTIL_MAP_H

#include <stdlib.h>
#include "s4396122_util_linkedlist.h"

#define MAP_SIZE 5  //!< The number used for hash array width

/**
 * @brief A single pair element of the map
 */
struct MapPair {
    int key;    //!< The integer representation of the key
    void *data; //!< The data associated with the key
};

/**
 * @brief Structure containing all the information for the Map
 */
typedef struct {
    LinkedList **map;   //!< An array of LinkedLists of width MAP_SIZE
    int size;           //!< Size of the map
} Map;

Map* s4396122_util_map_create();
void s4396122_util_map_free(Map *m);
void s4396122_util_map_add(Map *m, int key, void *d);
void* s4396122_util_map_get(Map *m, int key);

#endif
