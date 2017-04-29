#include "s4396122_os_pantilt.h"

TaskHandle_t s4396122_TaskPanTiltHandle;
int metronomeMode = 0;
int panVal = 0;
int tiltVal = 0;

void s4396122_TaskPanTilt() {
    while (1) {
        if (s4396122_SemaphorePanMetronome != NULL && xSemaphoreTake(s4396122_SemaphorePanMetronome, 10) == pdTRUE) {
            // Toggle Metronome value
            s4396122_util_print_debug("Toggle Metronome");
            metronomeMode ^= 1;
        } else if (s4396122_SemaphorePanLeft != NULL && xSemaphoreTake(s4396122_SemaphorePanLeft, 10) == pdTRUE) {
            // Move Pan Left
            s4396122_util_print_debug("Pan Left");
            panVal += 10;
        } else if (s4396122_SemaphorePanRight != NULL && xSemaphoreTake(s4396122_SemaphorePanRight, 10) == pdTRUE) {
            // Move Pan Right
            s4396122_util_print_debug("Pan Right");
            panVal -= 10;
        }

        if (s4396122_SemaphoreTiltUp != NULL && xSemaphoreTake(s4396122_SemaphoreTiltUp, 10) == pdTRUE) {
            s4396122_util_print_debug("Tilt Up");
            tiltVal -= 10;
        } else if (s4396122_SemaphoreTiltDown != NULL && xSemaphoreTake(s4396122_SemaphoreTiltDown, 10) == pdTRUE) {
            s4396122_util_print_debug("Tilt Down");
            tiltVal += 10;
        }

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

        s4396122_hal_pantilt_pan_write(panVal);
        s4396122_hal_pantilt_tilt_write(tiltVal);

        vTaskDelay(10);
    }
}

void s4396122_os_pantilt_init() {
    s4396122_hal_pantilt_init();

    s4396122_SemaphorePanMetronome = xSemaphoreCreateBinary();
    s4396122_SemaphorePanLeft = xSemaphoreCreateBinary();
    s4396122_SemaphorePanRight = xSemaphoreCreateBinary();
    s4396122_SemaphoreTiltUp = xSemaphoreCreateBinary();
    s4396122_SemaphoreTiltDown = xSemaphoreCreateBinary();

    xTaskCreate(&s4396122_TaskPanTilt, "PanTilt", pantilt_TASK_STACK_SIZE, NULL, pantilt_PRIORITY, &s4396122_TaskPanTiltHandle);
}

void s4396122_os_pantilt_deinit() {
    vTaskDelete(s4396122_TaskPanTilt);
    // TODO: Add this function in
    /*s4396122_hal_pantilt_deinit();*/
}
