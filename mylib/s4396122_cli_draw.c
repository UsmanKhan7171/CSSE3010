#include "s4396122_cli_draw.h"

CLI_Command_Definition_t xMove = {
    "move",
    "move:\n Move the cursor by the x and y position.\n\n",
    prvMoveCommand,
    2
};

CLI_Command_Definition_t xPenDown = {
    "pendown",
    "pendown:\n Puts the pen down on the screen.\n\n",
    prvPenDownCommand,
    0
};

CLI_Command_Definition_t xPenUp = {
    "penup",
    "penup:\n Releases the pen from the screen.\n\n",
    prvPenUpCommand,
    0
};

CLI_Command_Definition_t xBox = {
    "box",
    "box:\n Draw a box on the screen in the center.\n\n",
    prvBoxCommand,
    0
};

CLI_Command_Definition_t xLine = {
    "line",
    "line:\n Draw a line from x1 y1 to x2 y2. {(x1 y1) (x2 y2)}\n\n",
    prvLineCommand,
    4
};

void s4396122_cli_draw_init() {
    FreeRTOS_CLIRegisterCommand(&xMove);
    FreeRTOS_CLIRegisterCommand(&xPenDown);
    FreeRTOS_CLIRegisterCommand(&xPenUp);
    FreeRTOS_CLIRegisterCommand(&xBox);
    FreeRTOS_CLIRegisterCommand(&xLine);
}

static BaseType_t prvMoveCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    long paramLen;
    const char *arguments = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramLen);

    int x, y;
    sscanf(arguments, "%d %d", &x, &y);
    s4396122_os_draw_move_mouse(x, y);
    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "Move Cursor: (%d %d)\n", x, y);
    return pdFALSE;
}

static BaseType_t prvPenDownCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    s4396122_os_draw_mouse_button(1);
    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "");
    return pdFALSE;
}

static BaseType_t prvPenUpCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    s4396122_os_draw_mouse_button(0);
    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "");
    return pdFALSE;
}

static BaseType_t prvBoxCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    s4396122_os_draw_mouse_button(0);
    s4396122_os_draw_move_mouse(-100, -100);
    s4396122_os_draw_mouse_button(1);
    s4396122_os_draw_move_mouse(-100, 100);
    s4396122_os_draw_move_mouse(100, 100);
    s4396122_os_draw_move_mouse(100, -100);
    s4396122_os_draw_move_mouse(-100, -100);
    s4396122_os_draw_mouse_button(0);
    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "");
    return pdFALSE;
}

static BaseType_t prvLineCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    long paramLen;
    const char *arguments = FreeRTOS_CLIGetParameter(pcCommandString, 1, &paramLen);

    int x1, y1, x2, y2;
    sscanf(arguments, "%d %d %d %d", &x1, &y1, &x2, &y2);
    s4396122_os_draw_mouse_button(0);
    s4396122_os_draw_move_mouse(x1, y1);
    s4396122_os_draw_mouse_button(1);
    s4396122_os_draw_move_mouse(x1, y1);
    s4396122_os_draw_move_mouse(x2, y2);
    s4396122_os_draw_mouse_button(0);
    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "Drawing (%d, %d) to (%d, %d)\n", x1, y1, x2, y2);
    return pdFALSE;
}
