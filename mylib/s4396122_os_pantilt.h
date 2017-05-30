/**
 * @file s4396122_os_pantilt.h
 * @brief OS library for controlling the pantilt
 * @author Daniel Fitzmaurice - 43961229
 * @version 1.0
 * @date 2017-04-30
 */
#ifndef OS_PANTILT_H
#define OS_PANTILT_H

#include "s4396122_hal_pantilt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "s4396122_util_print.h"
#include "s4396122_hal_util.h"
#include "s4396122_hal_sysmon.h"

//! PanTilt Task Priority
#define pantilt_PRIORITY (tskIDLE_PRIORITY + 2)
//! PanTilt Task Stack Size
#define pantilt_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

//! Queue containing PanTiltMessages
QueueHandle_t s4396122_QueuePanTilt;
//! Semaphore to enable the MetronomeMode
SemaphoreHandle_t s4396122_SemaphorePanMetronome;
//! Semaphore to Pan left
SemaphoreHandle_t s4396122_SemaphorePanLeft;
//! Semaphore to Pan right
SemaphoreHandle_t s4396122_SemaphorePanRight;
//! Semaphore to Tilt up
SemaphoreHandle_t s4396122_SemaphoreTiltUp;
//! Semaphore to Tilt down
SemaphoreHandle_t s4396122_SemaphoreTiltDown;

/**
 * @brief Used to store a pantilt command
 */
struct PanTiltMessage {
    char type;          //!< Whether the angle applies to Pan or Tilt
    signed short angle; //!< Then angle to move
};

extern void s4396122_os_pantilt_init();
extern void s4396122_os_pantilt_deinit();

#endif
