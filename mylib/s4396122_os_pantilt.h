#ifndef OS_PANTILT_H
#define OS_PANTILT_H

#include "s4396122_hal_pantilt.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "s4396122_util_print.h"
#include "s4396122_hal_util.h"

#define pantilt_PRIORITY (tskIDLE_PRIORITY + 2)
#define pantilt_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

QueueHandle_t s4396122_QueuePanTilt;
SemaphoreHandle_t s4396122_SemaphorePanMetronome;
SemaphoreHandle_t s4396122_SemaphorePanLeft;
SemaphoreHandle_t s4396122_SemaphorePanRight;
SemaphoreHandle_t s4396122_SemaphoreTiltUp;
SemaphoreHandle_t s4396122_SemaphoreTiltDown;

struct PanTiltMessage {
    char type;
    signed short angle;
};

extern void s4396122_os_pantilt_init();
extern void s4396122_os_pantilt_deinit();

#endif
