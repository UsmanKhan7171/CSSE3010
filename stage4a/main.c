/**
 * *****************************************************************************
 * @file    stage4a/main.c
 * @author  Daniel Fitzmaurice
 * @date    240417
 * @brief   Main file for the stage4a design task
 * *****************************************************************************
 */

// Includes
#include "FreeRTOSConfig.h"
#include "board.h"
#include "s4396122_hal_sysmon.h"

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Task Priorities
#define mainLED_PRIORITY (tskIDLE_PRIORITY + 2)
#define mainLED_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define task1_PRIORITY (tskIDLE_PRIORITY + 2)
#define task1_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define task2_PRIORITY (tskIDLE_PRIORITY + 2)
#define task2_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define task3_PRIORITY (tskIDLE_PRIORITY + 1)
#define task3_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

void Hardware_init() {
    portDISABLE_INTERRUPTS();

    BRD_LEDInit();
    BRD_LEDOff();
    s4396122_hal_sysmon_init();

    portENABLE_INTERRUPTS();
}

void vApplicationStackOverflowHook(xTaskHandle pxTask, signed char *pcTaskName) {
    BRD_LEDOff();
    (void) pxTask;
    (void) pcTaskName;

    while (1);
}

void LED_Task() {
    while (1) {
        BRD_LEDToggle();
        vTaskDelay(250);
    }
}

void Task1_Task() {
    s4396122_hal_sysmon_chan0_clr();

    while (1) {
        s4396122_hal_sysmon_chan0_set();
        vTaskDelay(3);

        s4396122_hal_sysmon_chan0_clr();
        vTaskDelay(1);
    }
}

void Task2_Task() {
    s4396122_hal_sysmon_chan1_clr();

    while (1) {
        s4396122_hal_sysmon_chan1_set();
        vTaskDelay(3);

        s4396122_hal_sysmon_chan1_clr();
        vTaskDelay(1);
    }
}

void Task3_Task() {
    s4396122_hal_sysmon_chan2_clr();

    while (1) {
        s4396122_hal_sysmon_chan2_set();
        vTaskDelay(0);

        s4396122_hal_sysmon_chan2_clr();
    }
}

int main() {
    BRD_init();
    Hardware_init();

    // s4396122_hal_sysmon_chan1_set();

    xTaskCreate(&LED_Task, "LED", mainLED_TASK_STACK_SIZE, NULL, mainLED_PRIORITY, NULL);

    xTaskCreate(&Task1_Task, "Task1", task1_TASK_STACK_SIZE, NULL, task1_PRIORITY, NULL);
    xTaskCreate(&Task2_Task, "Task2", task2_TASK_STACK_SIZE, NULL, task2_PRIORITY, NULL);
    xTaskCreate(&Task3_Task, "Task3", task3_TASK_STACK_SIZE, NULL, task3_PRIORITY, NULL);

    vTaskStartScheduler();
}
