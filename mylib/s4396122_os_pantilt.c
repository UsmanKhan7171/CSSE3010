/**
 * @file s4396122_os_pantilt.c
 * @brief OS functions for controlling the pantilt
 * @author Daniel Fitzmaurice - 43961229
 * @version 1.0
 * @date 2017-04-30
 */
#include "s4396122_os_pantilt.h"

TaskHandle_t s4396122_TaskPanTiltHandle;
int metronomeMode = 0;
int panVal = 0;
int tiltVal = 0;

/**
 * @brief Task for controlling the pantilt by reading the input from the 
 * semaphores and queues
 */
void s4396122_TaskPanTilt() {
    // Debug output to the system monitor to see thread running time
    s4396122_hal_sysmon_chan0_clr();

    while (1) {
        // Debug output to the system monitor to see thread running time
        s4396122_hal_sysmon_chan0_set();

        if (s4396122_SemaphorePanMetronome != NULL && 
                xSemaphoreTake(s4396122_SemaphorePanMetronome, 1) == pdTRUE) {
            // Toggle Metronome value
            metronomeMode ^= 1;
            if (metronomeMode) {
                s4396122_util_print_debug("Metronome On");
            } else {
                s4396122_util_print_debug("Metronome Off");
            }
        } else if (s4396122_SemaphorePanLeft != NULL && 
                xSemaphoreTake(s4396122_SemaphorePanLeft, 1) == pdTRUE) {
            // Move Pan Left
            s4396122_util_print_debug("Pan Left");
            panVal += 10;
        } else if (s4396122_SemaphorePanRight != NULL && 
                xSemaphoreTake(s4396122_SemaphorePanRight, 1) == pdTRUE) {
            // Move Pan Right
            s4396122_util_print_debug("Pan Right");
            panVal -= 10;
        }

        if (s4396122_SemaphoreTiltUp != NULL && 
                xSemaphoreTake(s4396122_SemaphoreTiltUp, 1) == pdTRUE) {
            // Move Tilt Up
            s4396122_util_print_debug("Tilt Up");
            tiltVal -= 10;
        } else if (s4396122_SemaphoreTiltDown != NULL && 
                xSemaphoreTake(s4396122_SemaphoreTiltDown, 1) == pdTRUE) {
            // Move Tilt Down
            s4396122_util_print_debug("Tilt Down");
            tiltVal += 10;
        }

        if (metronomeMode) {
            // Calculate the time position using ticks (6s to do a full swing)
            int position = (HAL_GetTick() % 6000) - 3000;
            if (position < 0)
                position *= -1;
            // Map the value to a suitable swing position
            panVal = map(position, 0, 3000, -80, 80);
        }

        // Check to see if there a messsage available in the queue
        if (s4396122_QueuePanTilt != NULL) {
            struct PanTiltMessage message;
            if (xQueueReceive(s4396122_QueuePanTilt, &message, 1)) {
                s4396122_util_print_debug("Got queue message");
                // Set the current pt value to the message in the queue
                if (message.type == 't') {
                    tiltVal = message.angle;
                } else if (message.type == 'p') {
                    panVal = message.angle;
                } else {
                    s4396122_util_print_error("Unsupported message for P/T");
                }
            }
        }

        // Check the upper and lower bounds
        if (tiltVal > 80) {
            tiltVal = 80;
        } else if (tiltVal < -80) {
            tiltVal = -80;
        }
        if (panVal > 80) {
            panVal = 80;
        } else if (panVal < -80) {
            panVal = -80;
        }

        // Write the values to the pt
        s4396122_hal_pantilt_pan_write(panVal);
        s4396122_hal_pantilt_tilt_write(tiltVal);

        // Debug output to the system monitor
        s4396122_hal_sysmon_chan0_clr();

        // Delay to ensure that the pt doesn't consume all the cpu time
        vTaskDelay(10);
    }
}

/**
 * @brief Initializer for the pantilt. This also calls the hal init as well
 */
void s4396122_os_pantilt_init() {
    s4396122_util_print_debug("Init PanTilt");
    s4396122_hal_pantilt_init();
    s4396122_hal_sysmon_init();

    // Setup the semaphores and message queue
    s4396122_SemaphorePanMetronome = xSemaphoreCreateBinary();
    s4396122_SemaphorePanLeft = xSemaphoreCreateBinary();
    s4396122_SemaphorePanRight = xSemaphoreCreateBinary();
    s4396122_SemaphoreTiltUp = xSemaphoreCreateBinary();
    s4396122_SemaphoreTiltDown = xSemaphoreCreateBinary();
    s4396122_QueuePanTilt = xQueueCreate(10, sizeof(struct PanTiltMessage));

    // Create the task for handling pantilt input
    xTaskCreate(&s4396122_TaskPanTilt, "PanTilt", pantilt_TASK_STACK_SIZE, 
            NULL, pantilt_PRIORITY, &s4396122_TaskPanTiltHandle);
}

/**
 * @brief Deinitializes the pantilt os functions and calls the deinit for the
 * hal pantilt as well
 */
void s4396122_os_pantilt_deinit() {
    s4396122_util_print_debug("DeInit PanTilt");
    vTaskDelete(s4396122_TaskPanTiltHandle);
    vQueueDelete(s4396122_SemaphorePanMetronome);
    vQueueDelete(s4396122_SemaphorePanLeft);
    vQueueDelete(s4396122_SemaphorePanRight);
    vQueueDelete(s4396122_SemaphoreTiltUp);
    vQueueDelete(s4396122_SemaphoreTiltDown);
    vQueueDelete(s4396122_QueuePanTilt);
    s4396122_hal_pantilt_deinit();
}
