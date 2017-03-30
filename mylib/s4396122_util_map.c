#include "s4396122_util_map.h"

int generateHash(int key) {
    return key % MAP_SIZE;
}

Map* s4396122_util_map_create() {
    Map *m = malloc(sizeof(Map));
    m->map = malloc(sizeof(LinkedList*) * MAP_SIZE);
    for (int i = 0; i < MAP_SIZE; i++) {
        m->map[i] = s4396122_util_list_create();
    }
    m->size = 0;
    return m;
}

void s4396122_util_map_free(Map *m) {
    for (int i = 0; i < MAP_SIZE; i++) {
        s4396122_util_list_free(m->map[i]);
    }
    free(m->map);
    free(m);
}

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

void* s4396122_util_map_get(Map *m, int key) {
    LinkedList *list = m->map[generateHash(key)];
    struct linkedlist *posList = list->head;

    for (int i = 0; i < list->size; i++) {
        struct MapPair *pair = posList->data;
        if (pair->key == key) {
            return pair->data;
        }
    }

    return NULL;
}
