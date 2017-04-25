/**
 * @file    s4396122_os_joystick.h
 * @author  Daniel Fitzmaurice
 * @date    250417
 * @brief   Library for os interface to joystick peripherial
 */

#ifndef OS_JOYSTICK_H
#define OS_JOYSTICK_H

// Includes
#include <board.h>
#include <stm32f4xx_hal_conf.h>
#include <debug_printf.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

// Semaphore for the joystick z input interrupt
SemaphoreHandle_t s4396122_SemaphoreJoystickZ;

// External Functions
extern void s4396122_os_joystick_init();

#endif
