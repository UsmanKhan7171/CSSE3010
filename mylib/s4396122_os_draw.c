#include "s4396122_os_draw.h"

Iter *drawList;
SemaphoreHandle_t s4396122_SemaphoreDrawList;
int charLast;

void s4396122_os_draw_init() {
    s4396122_SemaphoreDrawList = xSemaphoreCreateMutex();
    if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
        drawList = s4396122_util_iter_create();
        charLast = 2;
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}

void s4396122_os_draw_add_char(int x, int y, char c) {
    int d = c | (y << 8) | (x << 16) | (0 << 31);

    if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
        s4396122_util_iter_add_tail(drawList, d);
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}

void s4396122_os_draw_add_poly(int x, int y, int length, int degree) {
    int d = length | (degree << 8) | (y << 16) | (x << 24) | (1 << 31);

    if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
        s4396122_util_iter_add_tail(drawList, d);
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}

void s4396122_os_draw_change_pen(char c) {

}

void s4396122_os_draw_redraw() {

}

void s4396122_os_draw_remove_top() {
    if (s4396122_SemaphoreDrawList != NULL && xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {
        s4396122_util_iter_remove_tail(drawList);
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}
