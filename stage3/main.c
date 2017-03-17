/**
 * *****************************************************************************
 * @file    stage2/main.c
 * @author  Daniel Fitzmaurice = 43961229
 * @date    080317
 * @brief   Main file for Stage 2
 * *****************************************************************************
 */

#include <board.h>
#include <debug_printf.h>
#include "s4396122_hal_joystick.h"
#include "s4396122_hal_ledbar.h"
#include "s4396122_hal_ir.h"

int map(int test, int oldMin, int oldMax, int newMin, int newMax) {
    return ((test - oldMin) * (newMax - newMin) / (oldMax - oldMin)) + newMin;
}

void Hardware_init() {
    BRD_init();
    BRD_LEDInit();
    s4396122_hal_joystick_init();
    s4396122_hal_ledbar_init();
    s4396122_hal_ir_init();
}

int main() {

    Hardware_init();

    while (1) {

        if (HAL_GetTick() > 10000) {
            s4396122_hal_ir_carrieron();
        } else if (HAL_GetTick() > 5000) {
            s4396122_hal_ir_carrieroff();
        }

        s4396122_hal_ir_datamodulation_set();
        HAL_Delay(250);
        s4396122_hal_ir_datamodulation_cli();
        HAL_Delay(250);

        int frequency = map(s4396122_hal_joystick_x_read(), 0, 4096, 0, 50);
        int ledBar = 1 << (frequency / 5);
        s4396122_hal_ledbar_write(ledBar);

        BRD_LEDToggle();
        HAL_Delay(100);
    }
}
