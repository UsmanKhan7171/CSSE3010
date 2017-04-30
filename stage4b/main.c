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
#include "s4396122_os_pantilt.h"
#include "s4396122_util_map.h"
#include "s4396122_util_print.h"
#include "s4396122_util_int_queue.h"
#include "s4396122_hal_tcp.h"

// Task Priorities
#define mainLED_PRIORITY (tskIDLE_PRIORITY + 3)
#define mainLED_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 2)
#define inputTask_PRIORITY (tskIDLE_PRIORITY + 2)
#define inputTask_TASK_STACK_SIZE (configMINIMAL_STACK_SIZE * 6)

// Global Variables
Map *serialMap;
int ptInitialized = 1;

/**
 * @brief Function for handling input for 'w'
 *
 * @param q
 */
void move_up(IntQueue *q) {
    if (s4396122_SemaphoreTiltUp != NULL) {
        xSemaphoreGive(s4396122_SemaphoreTiltUp);
    }
}
/**
 * @brief Function for handling input for 's'
 *
 * @param q
 */
void move_down(IntQueue *q) {
    if (s4396122_SemaphoreTiltDown != NULL) {
        xSemaphoreGive(s4396122_SemaphoreTiltDown);
    }
}
/**
 * @brief Function for handling input for 'a'
 *
 * @param q
 */
void move_left(IntQueue *q) {
    if (s4396122_SemaphorePanLeft != NULL) {
        xSemaphoreGive(s4396122_SemaphorePanLeft);
    }
}
/**
 * @brief Function for handling input for 'd'
 *
 * @param q
 */
void move_right(IntQueue *q) {
    if (s4396122_SemaphorePanRight != NULL) {
        xSemaphoreGive(s4396122_SemaphorePanRight);
    }
}
/**
 * @brief Function for handling input for 'm'
 *
 * @param q
 */
void toggle_metronome(IntQueue *q) {
    if (s4396122_SemaphorePanMetronome != NULL) {
        xSemaphoreGive(s4396122_SemaphorePanMetronome);
    }
}
/**
 * @brief Converts an integer queue of number characters to a single integer 
 * output
 *
 * @param q Integer Queue for data to be extracted from
 *
 * @return integer processed from queue
 */
int process_to_integer(IntQueue *q) {
    int negate = 0;
    int totalVal = 0;
    while (s4396122_util_int_queue_size(q)) {
        int c = s4396122_util_int_queue_pop(q);
        if (c == '-') {
            negate = 1;
        } else if (c > '9' || c < '0') {
            s4396122_util_print_error("Unsupported character range");
        } else {
            totalVal = (totalVal * 10) + (c - '0');
        }
    }
    if (negate) {
        totalVal *= -1;
    }
    return totalVal;
}
/**
 * @brief Function for handling input for 'p'
 *
 * @param q
 */
void move_pan(IntQueue *q) {
    struct PanTiltMessage message;
    message.type = 'p';
    message.angle = process_to_integer(q);
    if (message.angle > 90 || message.angle < -90) {
        s4396122_util_print_error("Angle outside bounds");
        return;
    }
    if (xQueueSendToFront(s4396122_QueuePanTilt, &message, 10) != pdPASS) {
        s4396122_util_print_error("Failed to post message");
    }
}
/**
 * @brief Function for handling input for 't'
 *
 * @param q
 */
void move_tilt(IntQueue *q) {
    struct PanTiltMessage message;
    message.type = 't';
    message.angle = process_to_integer(q);
    if (message.angle > 90 || message.angle < -90) {
        s4396122_util_print_error("Angle outside bounds");
        return;
    }
    if (xQueueSendToFront(s4396122_QueuePanTilt, &message, 10) != pdPASS) {
        s4396122_util_print_error("Failed to post message");
    }
}
/**
 * @brief Function for handling input for 'c'
 *
 * @param q
 */
void init_pt(IntQueue *q) {
    if (ptInitialized) {
        s4396122_util_print_error("PT already initialized");
        return;
    }
    s4396122_os_pantilt_init();
    ptInitialized = 1;
}
/**
 * @brief Function for handling input for 'x'
 *
 * @param q
 */
void deinit_pt(IntQueue *q) {
    if (!ptInitialized) {
        s4396122_util_print_error("PT already de-initialized");
        return;
    }
    s4396122_os_pantilt_deinit();
    ptInitialized = 0;
}

/**
 * Initializes the hardware
 */
void Hardware_init() {
    portDISABLE_INTERRUPTS();

    BRD_init();

    BRD_LEDInit();
    BRD_LEDOn();
    // Initialize libraries
    s4396122_os_pantilt_init();

    // Create a map of commands and functions for easily lookup of commands and
    // actions
    serialMap = s4396122_util_map_create();
    s4396122_util_map_add(serialMap, (int) 'w', &move_up);
    s4396122_util_map_add(serialMap, (int) 's', &move_down);
    s4396122_util_map_add(serialMap, (int) 'a', &move_left);
    s4396122_util_map_add(serialMap, (int) 'd', &move_right);
    s4396122_util_map_add(serialMap, (int) 'm', &toggle_metronome);
    s4396122_util_map_add(serialMap, (int) 'p', &move_pan);
    s4396122_util_map_add(serialMap, (int) 't', &move_tilt);
    s4396122_util_map_add(serialMap, (int) 'c', &init_pt);
    s4396122_util_map_add(serialMap, (int) 'x', &deinit_pt);

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
 * @brief Looks up the command for the first element in the IntQueue. If the
 * action exists then the action function is called and the remainder queue is
 * passed as a parameter
 *
 * @param q IntQueue of the action and any following arguments
 */
void process_command_queue(IntQueue *q) {
    int command = s4396122_util_int_queue_pop(q);
    void (*f)(IntQueue *) = s4396122_util_map_get(serialMap, command);
    if (f == NULL) {
        s4396122_util_print_error("Invalid Command");
        return;
    }
    f(q);
}

IntQueue* string_to_queue(char *message) {
    IntQueue *buffer = s4396122_util_int_queue_create();
    for (int i = 0; ; i++) {
        if (message[i] == '\0')
            break;
        s4396122_util_int_queue_push(buffer, message[i]);
    }
    return buffer;
}

/**
 * @brief Task for handling the network input and sending the input to the 
 * process functions
 */
void input_Task() {
    vTaskDelay(1000);
    s4396122_hal_tcp_init();

    while (1) {

        struct tcpConnection conn = s4396122_hal_tcp_accept();

        s4396122_hal_tcp_print(&conn, string_to_queue("Hello there"));

        while (1) {
            s4396122_hal_tcp_read(&conn, &process_command_queue);
            if (!conn.open) {
                break;
            }
            vTaskDelay(10);
        }

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
    xTaskCreate(&input_Task, "Input", inputTask_TASK_STACK_SIZE, NULL, inputTask_PRIORITY, NULL);

    s4396122_util_print_info("Initialized");

    // Hands main loop control over to freertos. This should never exit
    vTaskStartScheduler();
}
