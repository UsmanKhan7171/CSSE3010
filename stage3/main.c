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
#include <stm32f4xx_hal_conf.h>
#include "s4396122_hal_joystick.h"
#include "s4396122_hal_ledbar.h"
#include "s4396122_hal_ir.h"

unsigned int lastSignal;
int frequency;

int map(int test, int oldMin, int oldMax, int newMin, int newMax) {
    return ((test - oldMin) * (newMax - newMin) / (oldMax - oldMin)) + newMin;
}

void exti_d0_irqhandler() {
    HAL_GPIO_EXTI_IRQHandler(BRD_D0_PIN);

    unsigned int tickDiff = HAL_GetTick() - lastSignal;
    frequency = 1000 / tickDiff;
    // debug_printf("Diff: %d\n", tickDiff);
    // debug_printf("Frequency: %d\n", frequency);
    lastSignal = HAL_GetTick();
}

void Hardware_init() {
    BRD_init();
    BRD_LEDInit();
    s4396122_hal_joystick_init();
    s4396122_hal_ledbar_init();
    s4396122_hal_ir_init();

    GPIO_InitTypeDef GPIO_InitStructure;
    __BRD_D0_GPIO_CLK();
    HAL_NVIC_SetPriority(BRD_D0_EXTI_IRQ, 10, 0);

    NVIC_SetVector(BRD_D0_EXTI_IRQ, (uint32_t) &exti_d0_irqhandler);
    lastSignal = HAL_GetTick();
    frequency = 0;
    NVIC_EnableIRQ(BRD_D0_EXTI_IRQ);

    GPIO_InitStructure.Pin = BRD_D0_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(BRD_D0_GPIO_PORT, &GPIO_InitStructure);
}

int main() {

    Hardware_init();

    unsigned int lastTick = HAL_GetTick();
    int lastState = 0;
    s4396122_hal_ir_datamodulation_cli();

    while (1) {

        s4396122_hal_ir_carrieron();

        int frequency = map(s4396122_hal_joystick_x_read(), 0, 4096, 0, 50);
        int ledBar = 1 << (frequency / 5);
        s4396122_hal_ledbar_write(ledBar);

        if (frequency >= 1) {
            int delayPeriod = 1000/frequency;
            if (HAL_GetTick() > (delayPeriod/2 + lastTick)) {
                lastTick = HAL_GetTick();
                if (lastState) {
                    s4396122_hal_ir_datamodulation_cli();
                    lastState = 0;
                } else {
                    s4396122_hal_ir_datamodulation_set();
                    lastState = 1;
                }
            } else {
                debug_printf("Frequency: %d\n", frequency);
            }
        } else {
            lastTick = HAL_GetTick();
        }

        BRD_LEDToggle();
    }
}
