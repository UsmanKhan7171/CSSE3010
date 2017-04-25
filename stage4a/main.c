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
#include "s4396122_os_joystick.h"
#include "s4396122_hal_joystick.h"

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

// Task Priorities
#define mainLED_PRIORITY (tskIDLE_PRIORITY + 2)
#define mainLED_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define task1_PRIORITY (tskIDLE_PRIORITY + 2)
#define task1_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 0.3)
#define task2_PRIORITY (tskIDLE_PRIORITY + 2)
#define task2_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define task3_PRIORITY (tskIDLE_PRIORITY + 1)
#define task3_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)

// Global Variables
int task2Exists = 1;
TaskHandle_t task2Handle;

/**
 * Initializes the hardware
 */
void Hardware_init() {
    portDISABLE_INTERRUPTS();

    BRD_init();

    BRD_LEDInit();
    BRD_LEDOff();
    s4396122_hal_sysmon_init();
    s4396122_os_joystick_init();
    s4396122_hal_joystick_init();

    portENABLE_INTERRUPTS();
}

/**
 * vApplicationStackOverflowHook
 * @param pxTask     Task Handler
 * @param pcTaskName Task Name
 */
void vApplicationStackOverflowHook(xTaskHandle pxTask,
        signed char *pcTaskName) {
    BRD_LEDOff();
    (void) pxTask;
    (void) pcTaskName;

    while (1);
}

/**
 * Blinking LED Task to ensure a visual feedback that system is alive
 */
void LED_Task() {
    while (1) {
        BRD_LEDToggle();
        vTaskDelay(250);
    }
}

/**
 * Task 1 for managing output of sysmon channel 1
 */
void Task1_Task() {
    s4396122_hal_sysmon_chan0_clr();

    while (1) {
        s4396122_hal_sysmon_chan0_set();
        vTaskDelay(3);

        s4396122_hal_sysmon_chan0_clr();
        vTaskDelay(1);
    }
}

/**
 * Task 2 for managing output of sysmon channel 2. This is dynamically created
 * and deleted
 */
void Task2_Task() {
    s4396122_hal_sysmon_chan1_clr();

    while (1) {
        s4396122_hal_sysmon_chan1_set();
        vTaskDelay(3);

        s4396122_hal_sysmon_chan1_clr();
        vTaskDelay(1);
    }
}

/**
 * Task 3 for managing output of sysmon channel 3. As well as processing the
 * Z input of the joystick
 */
void Task3_Task() {
    s4396122_hal_sysmon_chan2_clr();

    while (1) {
        s4396122_hal_sysmon_chan2_set();
        vTaskDelay(0);

        s4396122_hal_sysmon_chan2_clr();

        if (s4396122_SemaphoreJoystickZ != NULL) {
            if (xSemaphoreTake(s4396122_SemaphoreJoystickZ, 10) == pdTRUE) {
                if (task2Exists) {
                    vTaskDelay(0);
                    vTaskDelete(task2Handle);
                } else {
                    xTaskCreate(&Task2_Task, "Task2", task2_TASK_STACK_SIZE,
                        NULL, task2_PRIORITY, &task2Handle);
                }
                task2Exists ^= 1;
            }
        }
    }
}

/**
 * Main system loop
 * @return Exit code status
 */
int main() {
    BRD_init(); // Initialise the board
    Hardware_init(); // Initialise peripherials

    // Create the LED Task
    xTaskCreate(&LED_Task, "LED", mainLED_TASK_STACK_SIZE, NULL,
        mainLED_PRIORITY, NULL);

    // Initialise the Sysmon Channel Tasks
    xTaskCreate(&Task1_Task, "Task1", task1_TASK_STACK_SIZE, NULL,
        task1_PRIORITY, NULL);
    xTaskCreate(&Task2_Task, "Task2", task2_TASK_STACK_SIZE, NULL,
        task2_PRIORITY, &task2Handle);
    xTaskCreate(&Task3_Task, "Task3", task3_TASK_STACK_SIZE, NULL,
        task3_PRIORITY, NULL);

    // Hands main loop control over to freertos. This should never exit
    vTaskStartScheduler();
}
