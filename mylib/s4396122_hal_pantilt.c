/**
 * *****************************************************************************
 * @file    s4396122_hal_pantilt.c
 * @author  Daniel Fitzmaurice = 43961229
 * @date    080317
 * @brief   Pan and Tilt control system
 *          REFERENCE: stage2
 * *****************************************************************************
 * s4396122_hal_pantilt_init() = initialise the servo
 * s4396122_hal_pantilt_pan_write(int) = sets the pan of the servo to the int
 * s4396122_hal_pantilt_pan_read() = returns the pan of the servo
 */

#include "s4396122_hal_pantilt.h"

/**
 * Initializes all the hardware pins and the server
 */
void s4396122_hal_pantilt_init() {
    __TIM2_CLK_ENABLE();        // Enable the pwm timer for the pantilt
    __BRD_D2_GPIO_CLK();        // Enable the pan pin clock

    // Require structures for initialization
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OC_InitTypeDef PWMConfig;

    // Initialize the pan pin output
    GPIO_InitStructure.Pin = BRD_D2_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(BRD_D2_GPIO_PORT, &GPIO_InitStructure); // Initialize

    // Timing prescaler value
    uint16_t PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 50000) - 1;

    // Initialize timer2
    TIM_Init.Instance = TIM2;
    TIM_Init.Init.Period = 50000 * 2/100; // Set a 20ms period
    TIM_Init.Init.Prescaler = PrescalerValue;
    TIM_Init.Init.ClockDivision = 0;
    TIM_Init.Init.RepetitionCounter = 0;
    TIM_Init.Init.CounterMode = TIM_COUNTERMODE_UP;

    // Setup the pwm configuration
    PWMConfig.OCMode = TIM_OCMODE_PWM1;
    PWMConfig.Pulse = 5000 * 1.45/100; // Set pwm to 1.45ms pulse width
    PWMConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    PWMConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    PWMConfig.OCFastMode = TIM_OCFAST_DISABLE;
    PWMConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
    PWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    // Initialize the timer and configure the pwm
    HAL_TIM_PWM_Init(&TIM_Init);
    HAL_TIM_PWM_ConfigChannel(&TIM_Init, &PWMConfig, TIM_CHANNEL_4);

    // Set the pwm pulse width
    HAL_TIM_PWM_Start(&TIM_Init, TIM_CHANNEL_4);
}

/**
 * Writes the angle to the server for pan or tilt
 * @param type  Controls whether the pan or tilt should be affected by the value
 *              (0 = pan, 1 = tilt)
 * @param angle The angle to set the pan/tilt to
 */
void pantilt_angle_write(int type, int angle) {
    if (angle < -85 || angle > 85) {
        // The angle is outside of what is allowed, so return and don't do
        // anything
        return;
    }

    if (type == 0) {
        // Get the pulseWidth from the angle (This formula is taken from the
        // Arduino Map function, but it has been simplified to help reduce
        // processing time)
        int pulseWidth = (((angle + 85) * 100) / 170) + 30;
        __HAL_TIM_SET_COMPARE(&TIM_Init, TIM_CHANNEL_4, pulseWidth);
    }

}

/**
 * Reads the angle from the server for pan or tilt
 * @param  type Controls whether the pan or tilt should be affected by the value
 *              (0 = pan, 1 = tilt)
 * @return      The angle read from the server (from either pan or tilt)
 */
int pantilt_angle_read(int type) {
    if (type == 0) {
        int pulseWidth = __HAL_TIM_GET_COMPARE(&TIM_Init, TIM_CHANNEL_4);
        // Gets the angle from the pulseWidth (This is just the inverted function
        // from angle_write)
        return (17 * (pulseWidth - 29) / 10) - 85;
    }
    return 0;
}
