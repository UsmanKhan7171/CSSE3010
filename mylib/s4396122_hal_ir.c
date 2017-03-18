/**
 * @file    s4396122_hal_ir.c
 * @author  Daniel Fitzmaurice = 43961229
 * @date    150317
 * @brief   Control library for the IR transmitter and receiver
 *          REFERENCE: stage3
 *******************************************************************************
 * s4396122_hal_ir_init()           Initialise the IR receiver/transmitter
 *      hardware
 * irhal_carrier(state)             Internal generic function for turning the
 *      carrier wave on or off
 */

#include "s4396122_hal_ir.h"

/**
 * Initializes the hardware for the ir transmitter. This will also turn the
 * carrier signal on. Pin D8 is the logic pin and D1 is the carrier signal
 */
void s4396122_hal_ir_init() {
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OC_InitTypeDef PWMConfig;

    // Initialize the Data Pin
    __BRD_D8_GPIO_CLK();
    GPIO_InitStructure.Pin = BRD_D8_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(BRD_D8_GPIO_PORT, &GPIO_InitStructure);

    // Initialize the Carrier Signal Pin and PWM Timer
    __TIM3_CLK_ENABLE();
    __BRD_D1_GPIO_CLK();

    // Setup the carrier pin to work as a pwm pin
    GPIO_InitStructure.Pin = BRD_D1_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(BRD_D1_GPIO_PORT, &GPIO_InitStructure);

    unsigned int PrescalerValue = (unsigned int)
            ((SystemCoreClock / 2) / 50000000) - 1;

    // Initialize the timer to a 38KHz square wave
    TIM_Init.Instance = TIM3;
    TIM_Init.Init.Period = 2205;
    TIM_Init.Init.Prescaler = PrescalerValue;
    TIM_Init.Init.ClockDivision = 0;
    TIM_Init.Init.RepetitionCounter = 0;
    TIM_Init.Init.CounterMode = TIM_COUNTERMODE_UP;

    // Set the PWM to oscilation and connect to the timer
    PWMConfig.OCMode = TIM_OCMODE_PWM1;
    PWMConfig.Pulse = TIM_Init.Init.Period / 2;
    PWMConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    PWMConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    PWMConfig.OCFastMode = TIM_OCFAST_DISABLE;
    PWMConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
    PWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    HAL_TIM_PWM_Init(&TIM_Init);
    HAL_TIM_PWM_ConfigChannel(&TIM_Init, &PWMConfig, TIM_CHANNEL_1);

    HAL_TIM_PWM_Start(&TIM_Init, TIM_CHANNEL_1);
}

/**
 * Turns on or off the carrier signal
 * @param state (1 = turns on the signal), (0 = turns off the signal)
 */
void irhal_carrier(int state) {
    if (state) {
        __HAL_TIM_SET_COMPARE(&TIM_Init, TIM_CHANNEL_1, 1102);
    } else {
        __HAL_TIM_SET_COMPARE(&TIM_Init, TIM_CHANNEL_1, 0);
    }
}
