/**
 * *****************************************************************************
 * @file    stage2/main.c
 * @author  Daniel Fitzmaurice = 43961229
 * @date    080317
 * @brief   Main file for Stage 2
 * *****************************************************************************
 */

#include <board.h>
#include <stm32f4xx_hal_conf.h>
#include "s4396122_hal_ledbar.h"
#include "s4396122_hal_pantilt.h"

/**
 * Initializes the hardware or calls the functions require to initialize the
 * hardware
 */
void Hardware_init() {
    // Initialize and turn off the debug LED
    BRD_LEDInit();
    BRD_LEDOff();

    s4396122_hal_ledbar_init(); // initializes the ledbar
    s4396122_hal_pantilt_init(); // initializes the pantilt server

    // TODO: Factorize out the following code
    __TIM2_CLK_ENABLE();
    __BRD_D2_GPIO_CLK();

    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_OC_InitTypeDef PWMConfig;
    TIM_HandleTypeDef TIM_Init;

    GPIO_InitStructure.Pin = BRD_D2_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(BRD_D2_GPIO_PORT, &GPIO_InitStructure);

    uint16_t PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 50000) - 1;

    TIM_Init.Instance = TIM2;
    TIM_Init.Init.Period = 50000 * 2/100;
    TIM_Init.Init.Prescaler = PrescalerValue;
    TIM_Init.Init.ClockDivision = 0;
    TIM_Init.Init.RepetitionCounter = 0;
    TIM_Init.Init.CounterMode = TIM_COUNTERMODE_UP;

    PWMConfig.OCMode = TIM_OCMODE_PWM1;
    PWMConfig.Pulse = 5000 * 1.45/100;
    PWMConfig.OCPolarity = TIM_OCPOLARITY_HIGH;
    PWMConfig.OCNPolarity = TIM_OCNPOLARITY_HIGH;
    PWMConfig.OCFastMode = TIM_OCFAST_DISABLE;
    PWMConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
    PWMConfig.OCNIdleState = TIM_OCNIDLESTATE_RESET;

    HAL_TIM_PWM_Init(&TIM_Init);
    HAL_TIM_PWM_ConfigChannel(&TIM_Init, &PWMConfig, TIM_CHANNEL_4);

    HAL_TIM_PWM_Start(&TIM_Init, TIM_CHANNEL_4);
}

/**
 * Main Program Loop
 * @return  The exit code of the program
 */
int main(void) {
    BRD_init();
    Hardware_init();

    while (1) { // Main function loop
        BRD_LEDToggle();
        HAL_Delay(1000);
    }
}
