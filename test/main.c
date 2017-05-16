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
#include <stdio.h>

// Scheduler includes
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "s4396122_util_print.h"
#include "s4396122_util_iter.h"
#include "s4396122_os_print.h"

// Task Priorities
#define mainLED_PRIORITY (tskIDLE_PRIORITY + 3)
#define mainLED_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define mainTask_PRIORITY (tskIDLE_PRIORITY + 2)
#define mainTask_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 6)

#define TEST(a) test(a, __FILE__, __LINE__)

int numTests = 0;
void test(int a, const char *fileName, int lineNo) {
    numTests++;
    s4396122_util_print_reset();
    s4396122_os_print("Test %d ... ", numTests);
    if (a) {
        s4396122_util_print_color(s4396122_util_print_green);
        s4396122_util_print_bold();
        s4396122_os_print("Passed\n");
    } else {
        s4396122_util_print_color(s4396122_util_print_red);
        s4396122_util_print_bold();
        s4396122_os_print("Failed: %s:%d\n", fileName, lineNo);
    }
    s4396122_util_print_reset();
}

/**
 * Initializes the hardware
 */
void Hardware_init() {
    portDISABLE_INTERRUPTS();

    BRD_init();

    BRD_LEDInit();
    BRD_LEDOn();
    s4396122_os_print_init();

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

void test_iter() {
    s4396122_util_print_info("Running Iter Tests");

    Iter *i = s4396122_util_iter_create();
    s4396122_util_iter_add_tail(i, 0);
    s4396122_util_iter_add_tail(i, 1);
    TEST(s4396122_util_iter_size(i) == 2);
    TEST(s4396122_util_iter_get_pos(i) == 0);
    TEST(s4396122_util_iter_get_next(i) == 1);
    s4396122_util_iter_add_head(i, 2);
    TEST(s4396122_util_iter_get_next(i) == 2);
    TEST(s4396122_util_iter_jump_head(i) == 2);
    // Array: 2, 0, 1
    s4396122_util_iter_remove_head(i);
    TEST(s4396122_util_iter_get_next(i) == 1);
    s4396122_util_iter_remove_tail(i);
    TEST(s4396122_util_iter_get_pos(i) == 0);
}

/**
 * @brief Task for handling the network input and sending the input to the 
 * process functions
 */
void main_Task() {
    s4396122_util_print_info("Beginning Tests");

    int one = 1;
    s4396122_os_print_add_detail("Here", &one);

    vTaskDelay(1500);
    TEST(1);
    test_iter();

    while (1) {
        vTaskDelay(1000);
        s4396122_util_print_debug("Main Loop");
    }
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
 * Main system loop
 * @return Exit code status
 */
int main() {

    Hardware_init(); // Initialise peripherials
    LwIP_Init();
    
    // Create the LED Task
    xTaskCreate(&LED_Task, "LED", mainLED_TASK_STACK_SIZE, NULL,
        mainLED_PRIORITY, NULL);

    // Create Task to handle the serial/network input
    xTaskCreate(&main_Task, "Main", mainTask_TASK_STACK_SIZE, NULL, mainTask_PRIORITY, NULL);

    // Hands main loop control over to freertos. This should never exit
    vTaskStartScheduler();
}
