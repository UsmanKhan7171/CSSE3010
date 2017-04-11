#include "s4396122_hal_time.h"

TIM_HandleTypeDef TIM_Micro; // Local timer for microseconds

void s4396122_hal_time_init() {
    __TIM4_CLK_ENABLE();

    unsigned int PrescalerValue = (unsigned int) ((SystemCoreClock / 2) / 1000000) - 1;

    TIM_Micro.Instance = TIM4;
    TIM_Micro.Init.Period = 1000000;
    TIM_Micro.Init.Prescaler = PrescalerValue;
    TIM_Micro.Init.ClockDivision = 0;
    TIM_Micro.Init.RepetitionCounter = 0;
    TIM_Micro.Init.CounterMode = TIM_COUNTERMODE_UP;

    HAL_TIM_Base_Init(&TIM_Micro);
    HAL_TIM_Base_Start(&TIM_Micro);
}

unsigned int s4396122_hal_time_get_micro() {
    return __HAL_TIM_GET_COUNTER(&TIM_Micro);
}

void s4396122_hal_time_clear_micro() {
    __HAL_TIM_SET_COUNTER(&TIM_Micro, 0);
}
