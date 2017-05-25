#include "s4396122_cli_joystick.h"

static BaseType_t prvJoystickCommand(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    int x = (s4396122_hal_joystick_x_read() - 2048) / 200;
    int y = (s4396122_hal_joystick_y_read() - 2048) / 200;
    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "(%d, %d)\n", x, y);
    return pdFALSE;
}

CLI_Command_Definition_t xJoystick = {
    "joystick",
    "joystick:\n Return the joystick vector.\n\n",
    prvJoystickCommand,
    0
};

void s4396122_cli_joystick_init() {
    FreeRTOS_CLIRegisterCommand(&xJoystick);
}
