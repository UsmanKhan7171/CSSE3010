#ifndef OS_JOYSTICK_H
#define OS_JOYSTICK_H

#include <board.h>
#include <stm32f4xx_hal_conf.h>
#include <debug_printf.h>
#include "FreeRTOS.h"
#include "semphr.h"
#include "queue.h"

SemaphoreHandle_t s4396122_SemaphoreJoystickZ;

extern void s4396122_os_joystick_init();

#endif
