/**
 * @file s4396122_hal_ir.c
 * @brief Control library for the IR transmitter and receiver
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_hal_ir.h"

TIM_HandleTypeDef TIM_Init;     //!< Local definition of modulation wave timer
TIM_HandleTypeDef IR_Input_Init;//!< Keep as a local definition

/**
 * @brief Handles the interrupt for the ir input. The timings will be stored in 
 * a queue for processing later on
 */
void s4396122_hal_ir_interrupt() {

    __HAL_TIM_CLEAR_IT(&IR_Input_Init, TIM_IT_TRIGGER);

    // Read the last time this interrupt was triggered
    int tim = HAL_TIM_ReadCapturedValue(&IR_Input_Init, TIM_CHANNEL_2);
    unsigned int *diff = malloc(sizeof(unsigned int));
    *diff = tim;
    s4396122_util_queue_push(IRQueue, diff);
    // Reset the timer
    __HAL_TIM_SET_COUNTER(&IR_Input_Init, TIM_CHANNEL_2);
}

/**
 * @brief Initializes the hardware for the ir transmitter. This will also turn 
 * the carrier signal on. Pin D8 is the logic pin and D1 is the carrier signal
 */
void s4396122_hal_ir_init() {

    IRQueue = s4396122_util_queue_create();

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
    __TIM8_CLK_ENABLE();
    __BRD_D1_GPIO_CLK();

    // Setup the carrier pin to work as a pwm pin
    GPIO_InitStructure.Pin = BRD_D1_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Alternate = GPIO_AF3_TIM8;
    HAL_GPIO_Init(BRD_D1_GPIO_PORT, &GPIO_InitStructure);

    unsigned int PrescalerValue = (unsigned int)
            ((SystemCoreClock / 2) / 50000000) - 1;

    // Initialize the timer to a 38KHz square wave
    TIM_Init.Instance = TIM8;
    TIM_Init.Init.Period = 4420;
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

    // Initialize the IR input pin
    __BRD_D0_GPIO_CLK();
    GPIO_InitStructure.Pin = BRD_D0_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(BRD_D0_GPIO_PORT, &GPIO_InitStructure);
    PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 1000000) - 1;
    
    // Initialize the ir input capture timer
    IR_Input_Init.Instance = TIM3;
    IR_Input_Init.Init.Period = 1000000;
    IR_Input_Init.Init.Prescaler = PrescalerValue;
    IR_Input_Init.Init.ClockDivision = 0;
    IR_Input_Init.Init.RepetitionCounter = 0;
    IR_Input_Init.Init.CounterMode = TIM_COUNTERMODE_UP;
    TIM_IC_InitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.ICPolarity = TIM_ICPOLARITY_BOTHEDGE;
    TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI;
    TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.ICFilter = 0;
    HAL_NVIC_SetPriority(TIM3_IRQn, 10, 0);
    NVIC_SetVector(TIM3_IRQn, (uint32_t) &s4396122_hal_ir_interrupt);
    NVIC_EnableIRQ(TIM3_IRQn);
    HAL_TIM_IC_Init(&IR_Input_Init);
    HAL_TIM_IC_ConfigChannel(&IR_Input_Init, &TIM_ICInitStructure,
            TIM_CHANNEL_2);
    HAL_TIM_IC_Start_IT(&IR_Input_Init, TIM_CHANNEL_2);
}

/**
 * @brief Turns on or off the carrier signal
 * @param state (1 = turns on the signal), (0 = turns off the signal)
 */
void irhal_carrier(int state) {

    if (state) {

        __HAL_TIM_SET_COMPARE(&TIM_Init, TIM_CHANNEL_1, 4420);
    } else {

        __HAL_TIM_SET_COMPARE(&TIM_Init, TIM_CHANNEL_1, 0);
    }
}

/**
 * @brief Gets the IRQueue and returns it for referencing to external libraries 
 * and functions
 * @return The IRQueue which contains all the current IR data
 */
Queue* s4396122_hal_ir_get_queue() {

    return IRQueue;
}
