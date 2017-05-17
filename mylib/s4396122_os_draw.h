#ifndef OS_DRAW_H
#define OS_DRAW_H

#include "s4396122_util_iter.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

char s4396122_os_draw_number_to_segment[10];
char s4396122_os_draw_letter_to_segment[26];

void s4396122_os_draw_init();
void s4396122_os_draw_add_char(int x, int y, char c);
void s4396122_os_draw_add_poly(int x, int y, int length, int degree);
void s4396122_os_draw_change_pen(char c);
void s4396122_os_draw_redraw();
void s4396122_os_draw_remove_top();

#endif
