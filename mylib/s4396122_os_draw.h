/**
 * @file s4396122_os_draw.h
 * @brief Library for drawing using paint
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef OS_DRAW_H
#define OS_DRAW_H

#include "s4396122_util_int_queue.h"
#include "s4396122_util_queue.h"
#include "s4396122_os_mqtt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include <math.h>

#include "usbd_hid.h"
#include "usbd_conf.h"
#include "usbd_core.h"
#include "usbd_desc.h"

#define OS_DRAW_LINE_PADDING 1      //!< Space between characters
#define OS_DRAW_LINE_LENGTH 7       //!< Length of a single line on a character

//! The total width of a character plus padding
#define OS_DRAW_LINE_WIDTH (OS_DRAW_LINE_LENGTH + OS_DRAW_LINE_PADDING)
//! The total height of a character plus padding
#define OS_DRAW_LINE_HEIGHT (OS_DRAW_LINE_LENGTH * 2 + OS_DRAW_LINE_PADDING)
#define OS_DRAW_CANVAS_ORIGINAL_OFFSET_X 1  //!< The original X canvas offset
#define OS_DRAW_CANVAS_ORIGINAL_OFFSET_Y 18 //!< The original Y canvas offset
#define OS_DRAW_CANVAS_WIDTH 125            //!< The width of the canvas
#define OS_DRAW_CANVAS_HEIGHT 118           //!< The height of the canvas

//! Variable to track the position of the x offset
extern int OS_DRAW_CANVAS_OFFSET_X;
//! Variable to track the position of the y offset
extern int OS_DRAW_CANVAS_OFFSET_Y;
//! Multiplication factor for square relativity
#define OS_DRAW_CANVAS_MULTI_FACTOR (1.8)
#define OS_DRAW_LANDSCAPE 1                 //!< Landscape orientation
#define OS_DRAW_PORTRAIT 0                  //!< Portrait orientation

#define OS_DRAW_RECTANGLE_X 29      //!< X location of rectangle tool
#define OS_DRAW_RECTANGLE_Y 7       //!< Y location of rectangle tool
#define OS_DRAW_PEN_X 16            //!< X location of pen tool
#define OS_DRAW_PEN_Y 8             //!< Y location of pen tool
#define OS_DRAW_COLOR_BLACK_X 50    //!< X location of black colour
#define OS_DRAW_COLOR_BLACK_Y 7     //!< Y location of black colour
#define OS_DRAW_COLOR_WHITE_X 50    //!< X location of white colour
#define OS_DRAW_COLOR_WHITE_Y 10    //!< Y location of white colour
#define OS_DRAW_COLOR_RED_X 55      //!< X location of red colour
#define OS_DRAW_COLOR_RED_Y 7       //!< Y location of red colour
#define OS_DRAW_COLOR_BLUE_X 62     //!< X location of blue colour
#define OS_DRAW_COLOR_BLUE_Y 7      //!< Y location of blue colour
#define OS_DRAW_COLOR_ORANGE_X 56   //!< X location of orange colour
#define OS_DRAW_COLOR_ORANGE_Y 7    //!< Y location of orange colour

//! Way of easily declaring a simple mouse click at the coords (x, y)
#define OS_DRAW_MOVE_AND_CLICK(x, y) s4396122_os_draw_mouse_button(0); \
    s4396122_os_draw_move_mouse(x, y); s4396122_os_draw_mouse_button(1); \
    s4396122_os_draw_mouse_button(0);

/**
 * @brief Enum for controlling the pen colour
 */
enum MouseColor {
    BLACK,  //!< Colour Black
    WHITE,  //!< Colour White
    RED,    //!< Colour Red
    BLUE,   //!< Colour Blue
    ORANGE  //!< Colour Orange
};

/**
 * @brief Enum for controlling the pen type
 */
enum MouseType {
    RECTANGLE,  //!< Rectangle tool
    PEN         //!< Pen tool
};

extern char s4396122_os_draw_number_to_segment[10];
extern char s4396122_os_draw_letter_to_segment[26];

/**
 * @brief Simple mouse command struct used for basic things like mouse movement
 * and mouse buttons
 */
struct MouseCommand {
    int leftMouse;  //!< If the left mouse button is down or not
    int xMovement;  //!< X mouse coord
    int yMovement;  //!< Y mouse coord
};

/**
 * @brief Used to store information about a display character
 */
struct DrawChar {
    int x;  //!< Top left x coord
    int y;  //!< Top left y coord
    int c;  //!< Segment encoded data
};

/**
 * @brief Used to store information about a display polygon
 */
struct DrawPoly {
    int x;      //!< Top left x coord
    int y;      //!< Top left y coord
    int length; //!< Length of single polygon side
    int degree; //!< Number of side to the polygon
};

#define OS_DRAW_POLY_MODE 1 //!< The mode for a command to be a polygon
#define OS_DRAW_CHAR_MODE 2 //!< The mode for a command to be a character
/**
 * @brief A general draw command that can be either character or polygon
 */
struct DrawCmd {
    int mode;               //!< Mode switch to decide if poly or char

    //! If char then this contains all the required data
    struct DrawChar c;
    //! If poly then this contains all the required data
    struct DrawPoly p;
    enum MouseColor color;  //!< Contains the colour for object
    enum MouseType type;    //!< Contains the type for the object
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
