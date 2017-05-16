#include "s4396122_util_iter.h"

Iter* s4396122_util_iter_create() {
    Iter *i = malloc(sizeof(Iter));
    i->head = NULL;
    i->tail = NULL;
    i->pos = NULL;
    i->size = 0;
    return i;
}

void s4396122_util_iter_add_head(Iter *i, int d) {
    struct IterElement *e = malloc(sizeof(struct IterElement));
    e->prev = NULL;
    e->next = i->head;
    e->data = d;
    if (i->head == NULL) {
        i->head = e;
        i->tail = e;
        i->pos = e;
    } else {
        i->head->prev = e;
        i->head = e;
    }
    i->size++;
}

void s4396122_util_iter_add_tail(Iter *i, int d) {
    struct IterElement *e = malloc(sizeof(struct IterElement));
    e->prev = i->tail;
    e->next = NULL;
    e->data = d;
    if (i->tail == NULL) {
        i->head = e;
        i->tail = e;
        i->pos = e;
    } else {
        i->tail->next = e;
        i->tail = e;
    }
    i->size++;
}

void s4396122_util_iter_remove_head(Iter *i) {
    struct IterElement *e = i->head;
    if (e == NULL)
        return;
    i->head = i->head->next;
    if (i->pos == e)
        i->pos = i->head;
    free(e);
}

void s4396122_util_iter_remove_tail(Iter *i) {
    struct IterElement *e = i->tail;
    if (e == NULL)
        return;
    i->tail = i->tail->prev;
    if (i->pos == e)
        i->pos = i->tail;
    free(e);
}

int s4396122_util_iter_get_pos(Iter *i) {
    return i->pos->data;
}

int s4396122_util_iter_get_next(Iter *i) {
    i->pos = i->pos->next;
    if (i->pos == NULL) {
        i->pos = i->head;
    }
    return i->pos->data;
}

int s4396122_util_iter_get_prev(Iter *i) {
    i->pos = i->pos->prev;
    if (i->pos == NULL) {
        i->pos = i->tail;
    }
    return i->pos->data;
}

int s4396122_util_iter_jump_head(Iter *i) {
    i->pos = i->head;
    return i->pos->data;
}

int s4396122_util_iter_jump_tail(Iter *i) {
    i->pos = i->tail;
    return i->pos->data;
}

void s4396122_util_iter_free(Iter *i) {
    struct IterElement *currentPos = i->head;
    for (int j = 0; j < i->size; j++) {
        struct IterElement *oldPos = currentPos;
        currentPos = currentPos->next;
        free(oldPos);
    }
    free(i);
}

int s4396122_util_iter_size(Iter *i) {
    return i->size;
}
