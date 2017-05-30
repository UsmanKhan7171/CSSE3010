/**
 * @file s4396122_cli_draw.c
 * @brief Provides drawing commands for CLI
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_cli_draw.h"

/**
 * @brief CLI struct for the move command
 */
CLI_Command_Definition_t xMove = {
    "move",
    "move:\n Move the cursor by the x and y position.\n\n",
    prvMoveCommand,
    2
};
/**
 * @brief CLI struct for the pendown command
 */
CLI_Command_Definition_t xPenDown = {
    "pendown",
    "pendown:\n Puts the pen down on the screen.\n\n",
    prvPenDownCommand,
    0
};
/**
 * @brief CLI struct for the penup command
 */
CLI_Command_Definition_t xPenUp = {
    "penup",
    "penup:\n Releases the pen from the screen.\n\n",
    prvPenUpCommand,
    0
};
/**
 * @brief CLI struct for the box command
 */
CLI_Command_Definition_t xBox = {
    "box",
    "box:\n Draw a box on the screen in the center.\n\n",
    prvBoxCommand,
    0
};
/**
 * @brief CLI struct for the line command
 */
CLI_Command_Definition_t xLine = {
    "line",
    "line:\n Draw a line from x1 y1 to x2 y2. {(x1 y1) (x2 y2)}\n\n",
    prvLineCommand,
    4
};
/**
 * @brief CLI struct for the reset command
 */
CLI_Command_Definition_t xReset = {
    "reset",
    "reset:\n Clears the canvas for a fresh drawing\n\n",
    prvResetCommand,
    0
};
/**
 * @brief CLI struct for the pen command
 */
CLI_Command_Definition_t xPen = {
    "pen",
    "pen:\n Sets the type of pen for drawing with.\n\n",
    prvPenCommand,
    1
};
/**
 * @brief CLI struct for the rotate command
 */
CLI_Command_Definition_t xRotate = {
    "rotate",
    "rotate:\n Changes the drawing orientation\n\n",
    prvRotateCommand,
    1
};

/**
 * @brief Init function for the draw commands. Adds all the commands to 
 * FreeRTOS
 */
void s4396122_cli_draw_init() {
    FreeRTOS_CLIRegisterCommand(&xMove);
    FreeRTOS_CLIRegisterCommand(&xPenDown);
    FreeRTOS_CLIRegisterCommand(&xPenUp);
    FreeRTOS_CLIRegisterCommand(&xBox);
    FreeRTOS_CLIRegisterCommand(&xLine);
    FreeRTOS_CLIRegisterCommand(&xReset);
    FreeRTOS_CLIRegisterCommand(&xPen);
    FreeRTOS_CLIRegisterCommand(&xRotate);
}

/**
 * @brief Moves the cursor to the coords provided
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvMoveCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    // Get the arguments in a string for easy formatting
    long paramLen;
    const char *arguments = FreeRTOS_CLIGetParameter(pcCommandString, 1, 
            &paramLen);

    // Convert the string of arguments to the required values
    int x, y;
    sscanf(arguments, "%d %d", &x, &y);

    s4396122_os_draw_move_mouse(x, y);

    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "Move Cursor: (%d %d)\n",
            x, y);
    return pdFALSE;
}

/**
 * @brief Sets the pen down on the screen for drawing
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvPenDownCommand(char *pcWriteBuffer, 
        size_t xWriteBufferLen, const char *pcCommandString) {

    s4396122_os_draw_mouse_button(1);

    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "");
    return pdFALSE;
}

/**
 * @brief Sets the pen up from the screen
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvPenUpCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    s4396122_os_draw_mouse_button(0);

    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "");
    return pdFALSE;
}

/**
 * @brief Draws a box around the edge of the canvas drawing space
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvBoxCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    s4396122_os_draw_mouse_button(0);
    s4396122_os_draw_move_mouse(-OS_DRAW_CANVAS_OFFSET_X + 1, 
            -OS_DRAW_CANVAS_OFFSET_Y + 1);
    s4396122_os_draw_mouse_button(1);
    s4396122_os_draw_move_mouse(-OS_DRAW_CANVAS_OFFSET_X + 1, 
            OS_DRAW_CANVAS_OFFSET_Y - 1);
    s4396122_os_draw_move_mouse(OS_DRAW_CANVAS_OFFSET_X - 1, 
            OS_DRAW_CANVAS_OFFSET_Y - 1);
    s4396122_os_draw_move_mouse(OS_DRAW_CANVAS_OFFSET_X - 1, 
            -OS_DRAW_CANVAS_OFFSET_Y + 1);
    s4396122_os_draw_move_mouse(-OS_DRAW_CANVAS_OFFSET_X + 1, 
            -OS_DRAW_CANVAS_OFFSET_Y + 1);
    s4396122_os_draw_mouse_button(0);

    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "");
    return pdFALSE;
}

/**
 * @brief Draws a line on the screen
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvLineCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    // Get the arguments string 
    long paramLen;
    const char *arguments = FreeRTOS_CLIGetParameter(pcCommandString, 1, 
            &paramLen);

    // Convert the arguments string to the required variables
    int x1, y1, x2, y2;
    sscanf(arguments, "%d %d %d %d", &x1, &y1, &x2, &y2);

    s4396122_os_draw_add_line(x1, y1, x2, y2);

    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "");
    return pdFALSE;
}

/**
 * @brief Clears the screen of all the currently draw
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvResetCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    s4396122_os_draw_reset();

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}

/**
 * @brief Changes the pen type
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvPenCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    // Get the arguments in string format
    long paramLen;
    const char *arguments = FreeRTOS_CLIGetParameter(pcCommandString, 1, 
            &paramLen);

    // convert char pen type to enum pen type
    enum MouseType t;
    switch (arguments[0]) {

        case 'b':

            t = RECTANGLE;
            break;

        case 'p':

            t = PEN;
            break;

        default:
            xWriteBufferLen = sprintf(pcWriteBuffer, "Unsupported value.\n");
            return pdFALSE;
    }

    s4396122_os_draw_change_pen_type(t);

    xWriteBufferLen = sprintf(pcWriteBuffer, "Selected Marker: %c\n", 
            arguments[0]);
    return pdFALSE;
}

/**
 * @brief Rotates the display to either portrait or landscape
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvRotateCommand(char *pcWriteBuffer, size_t xWriteBufferLen, 
        const char *pcCommandString) {

    // Get the arguments in string format
    long paramLen;
    const char *arguments = FreeRTOS_CLIGetParameter(pcCommandString, 1, 
            &paramLen);

    // convert the char representation to the integer representation
    int o;
    switch (arguments[0]) {

        case 'p':

            o = OS_DRAW_PORTRAIT;
            break;

        case 'l':

            o = OS_DRAW_LANDSCAPE;
            break;

        default:
            xWriteBufferLen = sprintf(pcWriteBuffer, "Unsupported value\n");
            return pdFALSE;
    }

    s4396122_os_draw_set_orientation(o);

    xWriteBufferLen = sprintf(pcWriteBuffer, "");
    return pdFALSE;
}
