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
    char s4396122_os_draw_number_to_segment[10] = {
        0x77,   // 0
        0x22,   // 1
        0x5B,   // 2
        0x6B,   // 3
        0x2E,   // 4
        0x6D,   // 5
        0x7D,   // 6
        0x27,   // 7
        0x7F,   // 8
        0x6F    // 9
    };
    char s4396122_os_draw_letter_to_segment[26] = {
        0x3F,   // a
        0x7C,   // b
        0x58,   // c
        0x7A,   // d
        0x5D,   // e
        0x1D,   // f
        0x6F,   // g
        0x3C,   // h
        0x22,   // i
        0x72,   // j
        0x1C,   // k
        0x34,   // l
        0x38,   // m
        0x38,   // n
        0x78,   // o
        0x1F,   // p
        0x2F,   // q
        0x18,   // r
        0x6D,   // s
        0x15,   // t
        0x70,   // u
        0x30,   // v
        0x70,   // w
        0x48,   // x
        0x6E,   // y
        0x5B    // z
    };
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
