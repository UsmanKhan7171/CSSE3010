/**
 * @file s4396122_cli_joystick.h
 * @brief Provides joystick commands for CLI
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef CLI_JOYSTICK_H
#define CLI_JOYSTICK_H

#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "s4396122_os_joystick.h"
#include "s4396122_hal_joystick.h"

void s4396122_cli_joystick_init();

#endif
