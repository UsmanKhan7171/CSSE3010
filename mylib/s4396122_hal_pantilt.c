/**
 * @file s4396122_hal_pantilt.c
 * @brief Pan and Tilt control system
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_hal_pantilt.h"

/**
 * @brief Initializes all the hardware pins and the server
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

    // Initialize the tile pin output
    GPIO_InitStructure.Pin = BRD_D3_PIN;
    HAL_GPIO_Init(BRD_D3_GPIO_PORT, &GPIO_InitStructure);

    // Timing prescaler value
    uint16_t PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 50000) - 1;

    // Initialize timer2
    TIM_PanTilt.Instance = TIM2;
    TIM_PanTilt.Init.Period = 50000 * 2/100; // Set a 20ms period
    TIM_PanTilt.Init.Prescaler = PrescalerValue;
    TIM_PanTilt.Init.ClockDivision = 0;
    TIM_PanTilt.Init.RepetitionCounter = 0;
    TIM_PanTilt.Init.CounterMode = TIM_COUNTERMODE_UP;

    // Setup the pwm configuration
    PWMConfig.OCMode = TIM_OCMODE_PWM1;
    PWMConfig.Pulse = 5000 * 1.45/100; // Set pwm to 1.45ms pulse width
    PWMConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    PWMConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    PWMConfig.OCFastMode = TIM_OCFAST_DISABLE;
    PWMConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
    PWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    // Initialize the timer and configure the pwm
    HAL_TIM_PWM_Init(&TIM_PanTilt);
    HAL_TIM_PWM_ConfigChannel(&TIM_PanTilt, &PWMConfig, TIM_CHANNEL_4);
    HAL_TIM_PWM_ConfigChannel(&TIM_PanTilt, &PWMConfig, TIM_CHANNEL_3);

    // Set the pwm pulse width
    HAL_TIM_PWM_Start(&TIM_PanTilt, TIM_CHANNEL_4);
    HAL_TIM_PWM_Start(&TIM_PanTilt, TIM_CHANNEL_3);
}

/**
 * @brief DeInitializes the pantilt peripherals. This includes; stopping the
 * pwm, deinit the pwm, and deinit the GPIO pins
 */
void s4396122_hal_pantilt_deinit() {

    HAL_TIM_PWM_Stop(&TIM_PanTilt, TIM_CHANNEL_4);
    HAL_TIM_PWM_Stop(&TIM_PanTilt, TIM_CHANNEL_3);
    HAL_TIM_PWM_DeInit(&TIM_PanTilt);
    HAL_GPIO_DeInit(BRD_D2_GPIO_PORT, BRD_D2_PIN);
    HAL_GPIO_DeInit(BRD_D3_GPIO_PORT, BRD_D3_PIN);
}

/**
 * @brief Writes the angle to the server for pan or tilt
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
        __HAL_TIM_SET_COMPARE(&TIM_PanTilt, TIM_CHANNEL_4, pulseWidth);
    } else if (type == 1) {

        int pulseWidth = (((angle + 70) * 100) / 170) + 30;
        __HAL_TIM_SET_COMPARE(&TIM_PanTilt, TIM_CHANNEL_3, pulseWidth);
    }

}

/**
 * @brief Reads the angle from the server for pan or tilt
 * @param  type Controls whether the pan or tilt should be affected by the value
 *              (0 = pan, 1 = tilt)
 * @return      The angle read from the server (from either pan or tilt)
 */
int pantilt_angle_read(int type) {

    if (type == 0) {
        
        int pulseWidth = __HAL_TIM_GET_COMPARE(&TIM_PanTilt, TIM_CHANNEL_4);
        // Gets the angle from the pulseWidth (This is just the inverted 
        // function from angle_write)
        return (17 * (pulseWidth - 29) / 10) - 85;
    } else if (type == 1) {

        int pulseWidth = __HAL_TIM_GET_COMPARE(&TIM_PanTilt, TIM_CHANNEL_3);
        return (17 * (pulseWidth - 29) / 10) - 70;
    }
    return 0;
}
