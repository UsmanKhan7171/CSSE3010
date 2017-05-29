#ifndef OS_DRAW_H
#define OS_DRAW_H

#include "s4396122_util_iter.h"
#include "s4396122_util_int_queue.h"
#include "s4396122_util_queue.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <math.h>

#include "usbd_hid.h"
#include "usbd_conf.h"
#include "usbd_core.h"
#include "usbd_desc.h"

/* Linux config
#define OS_DRAW_LINE_PADDING 2
#define OS_DRAW_LINE_LENGTH 20
#define OS_DRAW_LINE_WIDTH (OS_DRAW_LINE_LENGTH + OS_DRAW_LINE_PADDING)
#define OS_DRAW_LINE_HEIGHT (OS_DRAW_LINE_LENGTH * 2 + OS_DRAW_LINE_PADDING)
#define OS_DRAW_CANVAS_ORIGINAL_OFFSET_X -115
#define OS_DRAW_CANVAS_ORIGINAL_OFFSET_Y -110
extern int OS_DRAW_CANVAS_OFFSET_X;
extern int OS_DRAW_CANVAS_OFFSET_Y;
#define OS_DRAW_CANVAS_MULTI_FACTOR (1.7)

#define OS_DRAW_RECTANGLE_X -125
#define OS_DRAW_RECTANGLE_Y -90
#define OS_DRAW_PEN_X -125
#define OS_DRAW_PEN_Y -105
#define OS_DRAW_COLOR_BLACK_X -116
#define OS_DRAW_COLOR_BLACK_Y 120
#define OS_DRAW_COLOR_WHITE_X -116
#define OS_DRAW_COLOR_WHITE_Y 123
#define OS_DRAW_COLOR_RED_X -109
#define OS_DRAW_COLOR_RED_Y 120
#define OS_DRAW_COLOR_BLUE_X -96
#define OS_DRAW_COLOR_BLUE_Y 120
#define OS_DRAW_COLOR_ORANGE_X -106
#define OS_DRAW_COLOR_ORANGE_Y 120
*/

#define OS_DRAW_LINE_PADDING 1
#define OS_DRAW_LINE_LENGTH 7
#define OS_DRAW_LINE_WIDTH (OS_DRAW_LINE_LENGTH + OS_DRAW_LINE_PADDING)
#define OS_DRAW_LINE_HEIGHT (OS_DRAW_LINE_LENGTH * 2 + OS_DRAW_LINE_PADDING)
#define OS_DRAW_CANVAS_ORIGINAL_OFFSET_X 1
#define OS_DRAW_CANVAS_ORIGINAL_OFFSET_Y 18
#define OS_DRAW_CANVAS_WIDTH 125
#define OS_DRAW_CANVAS_HEIGHT 118
extern int OS_DRAW_CANVAS_OFFSET_X;
extern int OS_DRAW_CANVAS_OFFSET_Y;
#define OS_DRAW_CANVAS_MULTI_FACTOR (1.8)
#define OS_DRAW_LANDSCAPE 1
#define OS_DRAW_PORTRAIT 0

#define OS_DRAW_RECTANGLE_X 29
#define OS_DRAW_RECTANGLE_Y 7
#define OS_DRAW_PEN_X 16
#define OS_DRAW_PEN_Y 8
#define OS_DRAW_COLOR_BLACK_X 50
#define OS_DRAW_COLOR_BLACK_Y 7
#define OS_DRAW_COLOR_WHITE_X 50
#define OS_DRAW_COLOR_WHITE_Y 10
#define OS_DRAW_COLOR_RED_X 55
#define OS_DRAW_COLOR_RED_Y 7
#define OS_DRAW_COLOR_BLUE_X 62
#define OS_DRAW_COLOR_BLUE_Y 7
#define OS_DRAW_COLOR_ORANGE_X 56
#define OS_DRAW_COLOR_ORANGE_Y 7

#define OS_DRAW_MOVE_AND_CLICK(x, y) s4396122_os_draw_mouse_button(0); s4396122_os_draw_move_mouse(x, y); s4396122_os_draw_mouse_button(1); s4396122_os_draw_mouse_button(0);

enum MouseColor {
    BLACK,
    WHITE,
    RED,
    BLUE,
    ORANGE
};

enum MouseType {
    RECTANGLE,
    PEN
};

extern char s4396122_os_draw_number_to_segment[10];
extern char s4396122_os_draw_letter_to_segment[26];

struct MouseCommand {
    int leftMouse;
    int xMovement;
    int yMovement;
};

struct DrawChar {
    int x;
    int y;
    int c;
};

struct DrawPoly {
    int x;
    int y;
    int length;
    int degree;
};

#define OS_DRAW_POLY_MODE 1
#define OS_DRAW_CHAR_MODE 2
struct DrawCmd {
    int mode;
    struct DrawChar c;
    struct DrawPoly p;
    enum MouseColor color;
    enum MouseType type;
};

enum Orientation {
    PORTRAIT,
    LANDSCAPE
};

void s4396122_DrawerTask();
void s4396122_os_draw_init();
void s4396122_os_draw_add_char(int x, int y, char c);
void s4396122_os_draw_add_poly(int x, int y, int length, int degree);
void s4396122_os_draw_add_line(int x1, int y1, int x2, int y2);
void s4396122_os_draw_change_pen_color(enum MouseColor);
void s4396122_os_draw_change_pen_type(enum MouseType);
void s4396122_os_draw_redraw();
void s4396122_os_draw_reset();
void s4396122_os_draw_remove_top();
void s4396122_os_draw_move_mouse(int xMovement, int yMovement);
void s4396122_os_draw_mouse_button(int leftMouse);
void s4396122_os_draw_add_temp_char(char c);
void s4396122_os_draw_move_temp_char(int x, int y);
void s4396122_os_draw_commit_temp_char();
void s4396122_os_draw_move_origin(int x, int y);
void s4396122_os_draw_set_orientation(int orient);

#endif
