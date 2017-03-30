#ifndef UTIL_MAP_H
#define UTIL_MAP_H

#include <stdlib.h>
#include "s4396122_util_linkedlist.h"

#define MAP_SIZE 5

struct MapPair {
    int key;
    void *data;
};

typedef struct {
    LinkedList **map;
    int size;
} Map;

Map* s4396122_util_map_create();
void s4396122_util_map_free(Map *m);
void s4396122_util_map_add(Map *m, int key, void *d);
void* s4396122_util_map_get(Map *m, int key);

#endif
