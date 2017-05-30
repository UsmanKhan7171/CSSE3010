/**
 * @file s4396122_cli_joystick.c
 * @brief Provides joystick commands for CLI
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_cli_joystick.h"

/**
 * @brief Outputs the joystick value
 *
 * @param pcWriteBuffer     The buffer to write the command result to
 * @param xWriteBufferLen   The number of bits written to the buffer
 * @param pcCommandString   Any extra cli parameters
 *
 * @return If this command needs to run again
 */
static BaseType_t prvJoystickCommand(char *pcWriteBuffer, 
        size_t xWriteBufferLen, const char *pcCommandString) {

    int x = (s4396122_hal_joystick_x_read() - 2048) / 200;
    int y = (s4396122_hal_joystick_y_read() - 2048) / 200;

    xWriteBufferLen = sprintf((char *) pcWriteBuffer, "(%d, %d)\n", x, y);
    return pdFALSE;
}

/**
 * @brief CLI struct for the joystick command
 */
CLI_Command_Definition_t xJoystick = {
    "joystick",
    "joystick:\n Return the joystick vector.\n\n",
    prvJoystickCommand,
    0
};

/**
 * @brief Init function for the joystick commands. Adds all the commands to
 * FreeRTOS
 */
void s4396122_cli_joystick_init() {
    FreeRTOS_CLIRegisterCommand(&xJoystick);
}
