#ifndef UTIL_ITER_H
#define UTIL_ITER_H

#include <stdlib.h>

struct IterElement {
    struct IterElement *prev;
    struct IterElement *next;
    int data;
};

typedef struct {
    struct IterElement *head;
    struct IterElement *tail;
    struct IterElement *pos;
    int size;
} Iter;

Iter* s4396122_util_iter_create();
void s4396122_util_iter_add_head(Iter *i, int d);
void s4396122_util_iter_add_tail(Iter *i, int d);
void s4396122_util_iter_remove_head(Iter *i);
void s4396122_util_iter_remove_tail(Iter *i);
int s4396122_util_iter_get_pos(Iter *i);
int s4396122_util_iter_get_next(Iter *i);
int s4396122_util_iter_get_prev(Iter *i);
int s4396122_util_iter_jump_head(Iter *i);
int s4396122_util_iter_jump_tail(Iter *i);
void s4396122_util_iter_free(Iter *i);
int s4396122_util_iter_size(Iter *i);

#endif
