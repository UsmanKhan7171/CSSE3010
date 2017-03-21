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

/**
 * Global variables (used for continuous interrupts)
 */
unsigned int lastSignal;
unsigned int lastOutput;
int IRfrequency;

/**
 * Interrupt handler for pin D0. This handles the signal input from the ir
 * receiver
 */
void exti_d0_irqhandler() {
    // Handle the interrupt
    HAL_GPIO_EXTI_IRQHandler(BRD_D0_PIN);

    // Calculate the difference in time and then work out the frequency from
    // the difference
    unsigned int tickDiff = HAL_GetTick() - lastSignal;
    IRfrequency = 1000 / tickDiff;
    lastSignal = HAL_GetTick();     // Update the signal tracker
}

/**
 * Initializes the hardware for Stage3
 */
void Hardware_init() {
    // Initialise the Board and then call the library init functions
    BRD_init();
    BRD_LEDInit();
    s4396122_hal_joystick_init();
    s4396122_hal_ledbar_init();
    s4396122_hal_ir_init();

    // Initialize the D0 Pin for capturing and recording interrupts
    GPIO_InitTypeDef GPIO_InitStructure;
    __BRD_D0_GPIO_CLK();
    HAL_NVIC_SetPriority(BRD_D0_EXTI_IRQ, 10, 0);

    // Attach a function to the interrupt
    NVIC_SetVector(BRD_D0_EXTI_IRQ, (uint32_t) &exti_d0_irqhandler);
    lastSignal = HAL_GetTick();
    IRfrequency = 0;
    NVIC_EnableIRQ(BRD_D0_EXTI_IRQ);

    // Initialize the GPIO Pin to enable interrupts
    GPIO_InitStructure.Pin = BRD_D0_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(BRD_D0_GPIO_PORT, &GPIO_InitStructure);
}

int main() {

    Hardware_init();    // Initialize the hardware

    // Set all the system runtime defaults to ensure there are no system ghosts
    // from the last boot
    unsigned int lastTick = HAL_GetTick();
    lastOutput = HAL_GetTick();
    int lastState = 0;
    s4396122_hal_ir_datamodulation_cli();

    while (1) {     // System execution loop

        // s4396122_hal_ir_carrieron();    // Ensure the carrier signal is on

        // Capture the joystick input and map it to a frequency between 0 and 50
        int frequency = map(s4396122_hal_joystick_x_read(), 0, 4096, 0, 50);

        // frequency = map(frequency, 0, 32, 0, 50);

        if (HAL_GetTick() > (lastOutput + 500)) {
            lastOutput = HAL_GetTick();
            debug_printf("Frequency: %d\n", IRfrequency);
            int ledBar = 1 << (frequency / 5);  // Get the ledbar representation
            s4396122_hal_ledbar_write(ledBar);
        }

        // Add a handler for if the frequency is below 1. ie is 0
        if (frequency >= 1) {
            int delayPeriod = 1000/frequency;   // Calculate the period
            // Divide the delayPeriod by 2 for an oscilation calculations
            if (HAL_GetTick() > (delayPeriod/2 + lastTick)) {
                // NonBlocking method for transmitting data over the signal
                lastTick = HAL_GetTick();
                // State variable for oscilating over the the data signal
                if (lastState) {
                    s4396122_hal_ir_datamodulation_cli();
                    lastState = 0;
                } else {
                    s4396122_hal_ir_datamodulation_set();
                    lastState = 1;
                }
            } else {
                // If the system doesn't need to send a signal, send the
                // current signal reading
            }
        } else {
            // If the frequency is zero, just don't do anything
            lastTick = HAL_GetTick();
        }

        // Used to see a rough estimation of the system speed and if the system
        // is still alive
        BRD_LEDToggle();
    }
}
