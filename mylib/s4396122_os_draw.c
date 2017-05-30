/**
 * @file s4396122_os_draw.c
 * @brief Library for drawing using paint
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_os_draw.h"

//! The canvas offset for the x position
int OS_DRAW_CANVAS_OFFSET_X = OS_DRAW_CANVAS_ORIGINAL_OFFSET_X;
//! The canvas offset for the y position
int OS_DRAW_CANVAS_OFFSET_Y = OS_DRAW_CANVAS_ORIGINAL_OFFSET_Y;

Queue *drawList;    //!< A Queue of all the items that are added to the canvas
//! Semaphore to restrict access to drawList one at a time
SemaphoreHandle_t s4396122_SemaphoreDrawList;
//! A Queue of all the mouse commands that need to be executed
Queue *mouseQueue;  
//! Semaphore to restrict access to the mouseQueue one at a time
SemaphoreHandle_t s4396122_SemaphoreMouseQueue;
int updateDraw; //!< The canvas has data to be drawn
USBD_HandleTypeDef hidDevice;   //!< The HID Handler for the mouse
int leftMouseState;             //!< The last mouse click state
int lastMouseX;                 //!< The last mouse X coord
int lastMouseY;                 //!< The last mouse Y coord
enum MouseColor currentColor;   //!< The current pen colour
enum MouseType currentType;     //!< The current pen type

//! Temporary char to be drawn (typically the ir character)
struct DrawCmd tempChar;        
int currentOrient;              //!< The current display orientation

/**
 * @brief Used to convert a number to a segment representation
 */
char s4396122_os_draw_number_to_segment[] = {
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
/**
 * @brief Used to convert a letter to a segment representation
 */
char s4396122_os_draw_letter_to_segment[] = {
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
    0x54,   // l
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

/**
 * @brief Task that handles drawing on the screen
 */
void s4396122_DrawerTask() {
    
    while (1) {

        if (updateDraw) {

            updateDraw = 0;
            s4396122_os_draw_redraw();
        }

        if (s4396122_SemaphoreMouseQueue != NULL && 
                xSemaphoreTake(s4396122_SemaphoreMouseQueue, 1000)) {
            
            // While there are mouse commands to execute, execute them
            while (s4396122_util_queue_size(mouseQueue) > 0) {

                struct MouseCommand *cmd = s4396122_util_queue_pop(mouseQueue);
                uint8_t buff[4];
                buff[0] = cmd->leftMouse;
                buff[1] = (char) cmd->xMovement;
                buff[2] = (char) cmd->yMovement;
                buff[3] = 0;
                USBD_HID_SendReport(&hidDevice, buff, 4);
                free(cmd);
                vTaskDelay(10);
            }
            xSemaphoreGive(s4396122_SemaphoreMouseQueue);
        }

        vTaskDelay(100);
    }
}

/**
 * @brief Initializes the drawing tasks and sets the defaults values
 */
void s4396122_os_draw_init() {

    s4396122_SemaphoreDrawList = xSemaphoreCreateMutex();
    s4396122_SemaphoreMouseQueue = xSemaphoreCreateMutex();
    updateDraw = 1;
    leftMouseState = 0;
    currentColor = BLACK;
    currentType = PEN;
    tempChar.mode = 0;
    currentOrient = OS_DRAW_LANDSCAPE;

    if (s4396122_SemaphoreDrawList != NULL && 
            xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {

        drawList = s4396122_util_queue_create();
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
    if (s4396122_SemaphoreMouseQueue != NULL && 
            xSemaphoreTake(s4396122_SemaphoreMouseQueue, 1000)) {

        mouseQueue = s4396122_util_queue_create();
        xSemaphoreGive(s4396122_SemaphoreMouseQueue);
    }

    // Initialize the hid drivers
    USBD_Init(&hidDevice, &HID_Desc, 0);
    USBD_RegisterClass(&hidDevice, &USBD_HID);
    USBD_Start(&hidDevice);
}

/**
 * @brief Adds a char to the display
 *
 * @param x X coord of top left corner to draw to
 * @param y Y coord of top left corner to draw to
 * @param c character segment to add
 */
void s4396122_os_draw_add_char(int x, int y, char c) {

    if (s4396122_SemaphoreDrawList != NULL && 
            xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {

        struct DrawCmd *d = malloc(sizeof(struct DrawCmd));
        d->mode = OS_DRAW_CHAR_MODE;
        d->c.x = x;
        d->c.y = y;
        d->c.c = c;
        d->color = currentColor;
        d->type = currentType;
        s4396122_util_queue_push(drawList, d);
        updateDraw = 1;
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}

/**
 * @brief Adds a polygon to the display
 *
 * @param x         X coord of top left corner to draw to
 * @param y         Y coord of top left corner to draw to
 * @param length    Length of the sides
 * @param degree    Number of sides to draw
 */
void s4396122_os_draw_add_poly(int x, int y, int length, int degree) {

    if (s4396122_SemaphoreDrawList != NULL && 
            xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {

        struct DrawCmd *d = malloc(sizeof(struct DrawCmd));
        d->mode = OS_DRAW_POLY_MODE;
        d->p.x = x;
        d->p.y = y;
        d->p.length = length;
        d->p.degree = degree;
        d->color = currentColor;
        d->type = currentType;
        s4396122_util_queue_push(drawList, d);
        updateDraw = 1;
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}

/**
 * @brief Draws a line on the display, respects display ratio and display 
 * orientation
 *
 * @param x1    Line origin
 * @param y1    Line origin
 * @param x2    Line end point
 * @param y2    Line end point
 */
void s4396122_os_draw_add_line(int x1, int y1, int x2, int y2) {

    s4396122_os_draw_mouse_button(0);

    if (currentOrient == PORTRAIT) {

        s4396122_os_draw_move_mouse(
                (y1 - OS_DRAW_CANVAS_OFFSET_Y + OS_DRAW_CANVAS_OFFSET_X), 
                OS_DRAW_CANVAS_HEIGHT 
                    - (x1 - OS_DRAW_CANVAS_OFFSET_X + OS_DRAW_CANVAS_OFFSET_Y));
    } else {

        s4396122_os_draw_move_mouse(x1, y1);
    }

    s4396122_os_draw_mouse_button(1);

    if (currentOrient == PORTRAIT) {

        s4396122_os_draw_move_mouse(
                (y2 - OS_DRAW_CANVAS_OFFSET_Y + OS_DRAW_CANVAS_OFFSET_X) 
                    * OS_DRAW_CANVAS_MULTI_FACTOR, 
                OS_DRAW_CANVAS_HEIGHT 
                    - (x2 - OS_DRAW_CANVAS_OFFSET_X + OS_DRAW_CANVAS_OFFSET_Y));
    } else {

        s4396122_os_draw_move_mouse(x2, y2 * OS_DRAW_CANVAS_MULTI_FACTOR);
    }

    s4396122_os_draw_mouse_button(0);
}

/**
 * @brief Draws a line on the display, respects display ratio and display 
 * orientation
 *
 * @param x         Line start point
 * @param y         Line start point
 * @param width     Width of the line
 * @param height    Height of the line
 */
void s4396122_os_draw_add_relative_line(int x, int y, int width, int height) {

    s4396122_os_draw_mouse_button(0);
    int origX = x + OS_DRAW_CANVAS_OFFSET_X;
    int origY = y + OS_DRAW_CANVAS_OFFSET_Y;
    if (currentOrient == PORTRAIT) {

        origX = y + OS_DRAW_CANVAS_OFFSET_X;
        origY = OS_DRAW_CANVAS_HEIGHT - x;
    }

    s4396122_os_draw_move_mouse(origX, origY);
    s4396122_os_draw_mouse_button(1);

    int newX = origX + width;
    int newY = origY + (height);
    if (currentOrient == PORTRAIT) {

        newX = origX + height;
        newY = origY - width;
    }

    s4396122_os_draw_move_mouse(newX, newY);
    s4396122_os_draw_mouse_button(0);
}

/**
 * @brief Changes the pen colour to match the parameter
 *
 * @param c New colour for the pen to be
 */
void s4396122_os_draw_change_pen_color(enum MouseColor c) {

    currentColor = c;
    switch(c) {

        case BLACK:

            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_COLOR_BLACK_X, 
                    OS_DRAW_COLOR_BLACK_Y);
            break;

        case WHITE:

            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_COLOR_WHITE_X, 
                    OS_DRAW_COLOR_WHITE_Y);
            break;

        case RED:

            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_COLOR_RED_X, OS_DRAW_COLOR_RED_Y);
            break;

        case BLUE:

            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_COLOR_BLUE_X, OS_DRAW_COLOR_BLUE_Y);
            break;

        case ORANGE:

            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_COLOR_ORANGE_X, 
                    OS_DRAW_COLOR_ORANGE_Y);
            break;
    }
}

/**
 * @brief Changes the pen type to match the parameter
 *
 * @param t New pen type for the pen to be
 */
void s4396122_os_draw_change_pen_type(enum MouseType t) {

    currentType = t;
    switch(t) {

        case RECTANGLE:

            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_RECTANGLE_X, OS_DRAW_RECTANGLE_Y);
            break;

        case PEN:

            OS_DRAW_MOVE_AND_CLICK(OS_DRAW_PEN_X, OS_DRAW_PEN_Y);
            break;
    }
}

/**
 * @brief Renders the entire drawList Queue into the mouseQueue for the 
 * s4396122_DrawerTask to execute on the display
 */
void s4396122_os_draw_redraw() {
    if (s4396122_SemaphoreDrawList != NULL && 
            xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {

        // Send a mqtt message to show that the display is being redraw
        s4396122_os_mqtt_publish("draw", "redrawing");

        // Store the current pen colour and type to be restored later
        enum MouseColor origColor = currentColor;
        enum MouseType origType = currentType;
        
        // Create a new drawList that contains everything that has been drawn
        Queue *newDrawList = s4396122_util_queue_create();

        while (s4396122_util_queue_size(drawList) > 0) {

            // For every element in drawList, restore the colour and type
            struct DrawCmd *c = s4396122_util_queue_pop(drawList);
            s4396122_util_queue_push(newDrawList, c);
            s4396122_os_draw_change_pen_color(c->color);
            s4396122_os_draw_change_pen_type(c->type);
            
            // Work out if the draw object is poly or char
            if (c->mode == OS_DRAW_CHAR_MODE) {

                int origX = c->c.x * OS_DRAW_LINE_WIDTH;
                int origY = c->c.y * OS_DRAW_LINE_HEIGHT;
                if (c->c.c & 0x01) {
                    
                    s4396122_os_draw_add_relative_line(origX, origY, 
                            OS_DRAW_LINE_LENGTH, 0);
                }
                if (c->c.c & 0x02) {

                    s4396122_os_draw_add_relative_line(
                            origX + OS_DRAW_LINE_LENGTH, origY, 0, 
                            OS_DRAW_LINE_LENGTH);
                }
                if (c->c.c & 0x04) {

                    s4396122_os_draw_add_relative_line(origX, origY, 0, 
                            OS_DRAW_LINE_LENGTH);
                }
                if (c->c.c & 0x08) {

                    s4396122_os_draw_add_relative_line(origX, 
                            origY + OS_DRAW_LINE_LENGTH, OS_DRAW_LINE_LENGTH, 
                            0);
                }
                if (c->c.c & 0x10) {

                    s4396122_os_draw_add_relative_line(origX, 
                            origY + OS_DRAW_LINE_LENGTH, 0, 
                            OS_DRAW_LINE_LENGTH);
                }
                if (c->c.c & 0x20) {

                    s4396122_os_draw_add_relative_line(
                            origX + OS_DRAW_LINE_LENGTH, 
                            origY + OS_DRAW_LINE_LENGTH, 0, 
                            OS_DRAW_LINE_LENGTH);
                }
                if (c->c.c & 0x40) {

                    s4396122_os_draw_add_relative_line(origX, 
                            origY + 2 * OS_DRAW_LINE_LENGTH, 
                            OS_DRAW_LINE_LENGTH, 0);
                }
            } else if (c->mode == OS_DRAW_POLY_MODE) {

                double origX = c->p.x;
                double origY = c->p.y;
                double angle = 2 * M_PI / c->p.degree;
                for (int i = 0; i < c->p.degree; i++) {
                    double width = c->p.length * sin(angle * i);
                    double height = c->p.length * cos(angle * i);
                    s4396122_os_draw_add_relative_line(origX, origY, width, 
                            height);
                    origX += width;
                    origY += height;
                }
            }
        }
        
        // Swap the new drawList into as the next drawList
        s4396122_util_queue_free(drawList);
        drawList = newDrawList;

        // If there is a tempChar to be drawn, draw it
        if (tempChar.mode != 0) {

            s4396122_os_draw_change_pen_color(tempChar.color);
            s4396122_os_draw_change_pen_type(tempChar.type);

            int origX = tempChar.c.x * OS_DRAW_LINE_WIDTH 
                + OS_DRAW_CANVAS_OFFSET_X;
            int origY = tempChar.c.y * OS_DRAW_LINE_HEIGHT 
                + OS_DRAW_CANVAS_OFFSET_Y;
            if (tempChar.c.c & 0x01) {

                s4396122_os_draw_add_line(origX, origY, 
                        origX + OS_DRAW_LINE_LENGTH, origY);
            }
            if (tempChar.c.c & 0x02) {

                s4396122_os_draw_add_line(origX + OS_DRAW_LINE_LENGTH, origY, 
                        origX + OS_DRAW_LINE_LENGTH, 
                        origY + OS_DRAW_LINE_LENGTH);
            }
            if (tempChar.c.c & 0x04) {

                s4396122_os_draw_add_line(origX, origY, origX, 
                        origY + OS_DRAW_LINE_LENGTH);
            }
            if (tempChar.c.c & 0x08) {

                s4396122_os_draw_add_line(origX, origY + OS_DRAW_LINE_LENGTH, 
                        origX + OS_DRAW_LINE_LENGTH, 
                        origY + OS_DRAW_LINE_LENGTH);
            }
            if (tempChar.c.c & 0x10) {

                s4396122_os_draw_add_line(origX, origY + OS_DRAW_LINE_LENGTH, 
                        origX, origY + 2 * OS_DRAW_LINE_LENGTH);
            }
            if (tempChar.c.c & 0x20) {

                s4396122_os_draw_add_line(origX + OS_DRAW_LINE_LENGTH, 
                        origY + OS_DRAW_LINE_LENGTH, 
                        origX + OS_DRAW_LINE_LENGTH, 
                        origY + 2 * OS_DRAW_LINE_LENGTH);
            }
            if (tempChar.c.c & 0x40) {

                s4396122_os_draw_add_line(origX, 
                        origY + 2 * OS_DRAW_LINE_LENGTH, 
                        origX + OS_DRAW_LINE_LENGTH, 
                        origY + 2 * OS_DRAW_LINE_LENGTH);
            }
        }

        // Restore the original color and pen type
        s4396122_os_draw_change_pen_color(origColor);
        s4396122_os_draw_change_pen_type(origType);
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}

/**
 * @brief Clears all the content on the screen
 */
void s4396122_os_draw_reset() {

    // Publish a message to the mqtt stream
    s4396122_os_mqtt_publish("draw", "clear screen");

    s4396122_os_draw_mouse_button(0);
    s4396122_os_draw_move_mouse(OS_DRAW_RECTANGLE_X, OS_DRAW_RECTANGLE_Y);
    s4396122_os_draw_mouse_button(1);
    s4396122_os_draw_mouse_button(0);
    s4396122_os_draw_move_mouse(OS_DRAW_CANVAS_ORIGINAL_OFFSET_X, 
            OS_DRAW_CANVAS_ORIGINAL_OFFSET_Y);
    s4396122_os_draw_mouse_button(1); // set this to 2 for linux
    s4396122_os_draw_move_mouse(OS_DRAW_CANVAS_WIDTH, OS_DRAW_CANVAS_HEIGHT);
    s4396122_os_draw_mouse_button(0);
    s4396122_os_draw_move_mouse(OS_DRAW_PEN_X, OS_DRAW_PEN_Y);
    s4396122_os_draw_mouse_button(1);
    s4396122_os_draw_mouse_button(0);
}

/**
 * @brief Removes the last added item to the drawList Queue
 */
void s4396122_os_draw_remove_top() {

    if (s4396122_SemaphoreDrawList != NULL && 
            xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {

        Queue *newDrawList = s4396122_util_queue_create();
        while (s4396122_util_queue_size(drawList) > 1) {

            s4396122_util_queue_push(newDrawList, 
                    s4396122_util_queue_pop(drawList));
        }
        if (s4396122_util_queue_size(drawList) > 0) {

            free(s4396122_util_queue_pop(drawList));
        }
        drawList = newDrawList;
        s4396122_os_draw_reset();
        updateDraw = 1;
        xSemaphoreGive(s4396122_SemaphoreDrawList);
    }
}

/**
 * @brief Moves the cursor to the provided location
 *
 * @param xMovement X coord to move to
 * @param yMovement Y coord to move to
 */
void s4396122_os_draw_move_mouse(int xMovement, int yMovement) {

    if (s4396122_SemaphoreMouseQueue != NULL && 
            xSemaphoreTake(s4396122_SemaphoreMouseQueue, 1000)) {

        struct MouseCommand *cmd = malloc(sizeof(struct MouseCommand));
        cmd->leftMouse = leftMouseState;
        cmd->xMovement = xMovement;
        cmd->yMovement = yMovement;
        lastMouseX = xMovement;
        lastMouseY = yMovement;
        s4396122_util_queue_push(mouseQueue, cmd);
        xSemaphoreGive(s4396122_SemaphoreMouseQueue);
    }
}

/**
 * @brief Sets whether the mouse button is down or up
 *
 * @param leftMouse Sets the mouse button status
 */
void s4396122_os_draw_mouse_button(int leftMouse) {

    if (s4396122_SemaphoreMouseQueue != NULL && 
            xSemaphoreTake(s4396122_SemaphoreMouseQueue, 1000)) {

        leftMouseState = leftMouse;
        struct MouseCommand *cmd = malloc(sizeof(struct MouseCommand));
        cmd->leftMouse = leftMouseState;
        cmd->xMovement = lastMouseX;
        cmd->yMovement = lastMouseY;
        s4396122_util_queue_push(mouseQueue, cmd);
        xSemaphoreGive(s4396122_SemaphoreMouseQueue);
    }
}

/**
 * @brief Sets the tempChar to the character c and if not enabled, also enables
 * the tempChar
 *
 * @param c Character segment map to be drawn
 */
void s4396122_os_draw_add_temp_char(char c) {

    tempChar.mode = OS_DRAW_CHAR_MODE;
    s4396122_os_draw_reset();
    updateDraw = 1;
    tempChar.c.c = c;
}

/**
 * @brief Moves the tempChar relative to it's current coordinate
 *
 * @param x X coord relative value
 * @param y Y coord relative value
 */
void s4396122_os_draw_move_temp_char(int x, int y) {

    s4396122_os_draw_reset();
    updateDraw = 1;
    tempChar.c.x += x;
    tempChar.c.y += y;
}

/**
 * @brief Commits the tempChar to be apart of drawList
 */
void s4396122_os_draw_commit_temp_char() {

    if (tempChar.mode != 0) {

        if (s4396122_SemaphoreDrawList != NULL && 
                xSemaphoreTake(s4396122_SemaphoreDrawList, 1000)) {

            struct DrawCmd *d = malloc(sizeof(struct DrawCmd));
            d->mode = OS_DRAW_CHAR_MODE;
            d->c = tempChar.c;
            d->color = tempChar.color;
            d->type = tempChar.type;
            tempChar.c.x++;
            tempChar.mode = 0;
            s4396122_util_queue_push(drawList, d);
            updateDraw = 1;
            xSemaphoreGive(s4396122_SemaphoreDrawList);
        }
    }
}

/**
 * @brief Moves the origin of the canvas
 *
 * @param x New X coordinate to set as the origin
 * @param y New Y coordinate to set as the origin
 */
void s4396122_os_draw_move_origin(int x, int y) {

    OS_DRAW_CANVAS_OFFSET_X += x;
    OS_DRAW_CANVAS_OFFSET_Y += y;

    s4396122_os_draw_reset();
    updateDraw = 1;
}

/**
 * @brief Sets the canvas orientation
 *
 * @param orient    New orientation for the display to be
 */
void s4396122_os_draw_set_orientation(int orient) {

    if (currentOrient != orient) {

        currentOrient = orient;

        s4396122_os_draw_reset();
        updateDraw = 1;
    }
}
