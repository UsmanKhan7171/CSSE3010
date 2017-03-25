#include "s4396122_hal_irremote.h"

// void s4396122_hal_irremote_init() {
//
//     lastInput = -1;
//     newInput = 0;
//     newMessage = 0;
//     buffer = 0;
//     lastInputTime = HAL_GetTick();
//
//     GPIO_InitTypeDef GPIO_InitStructure;
//     __BRD_D0_GPIO_CLK();
//     HAL_NVIC_SetPriority(BRD_D0_EXTI_IRQ, 10, 0);
//
//     NVIC_SetVector(BRD_D0_EXTI_IRQ, (uint32_t) &s4396122_hal_irremote_interrupt);
//     NVIC_EnableIRQ(BRD_D0_EXTI_IRQ);
//
//     GPIO_InitStructure.Pin = BRD_D0_PIN;
//     GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
//     GPIO_InitStructure.Pull = GPIO_PULLUP;
//     GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
//     HAL_GPIO_Init(BRD_D0_GPIO_PORT, &GPIO_InitStructure);
// }
//
// void s4396122_hal_irremote_interrupt() {
//
//     HAL_GPIO_EXTI_IRQHandler(BRD_D0_PIN);
//
//     int diff = HAL_GetTick() - lastInputTime;
//     lastInputTime = HAL_GetTick();
//     // debug_printf("%d\n", diff);
//     if (approx(diff, 9, 1)) {
//         newMessage = 1;
//         debug_printf("Got\n");
//     } else {
//         newMessage = 0;
//     }
//     // debug_printf("%d\n", diff);
// }

TIM_HandleTypeDef TIM_Init;
void s4396122_hal_irremote_init() {

    IRQueue = s4396122_util_queue_create();
    newIRMessage = 0;

    __TIM3_CLK_ENABLE();
    __BRD_D0_GPIO_CLK();

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Pin = BRD_D0_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(BRD_D0_GPIO_PORT, &GPIO_InitStructure);

    uint16_t PrescalerValue = (uint16_t) ((SystemCoreClock / 2) / 50000) - 1;

    TIM_Init.Instance = TIM3;
    TIM_Init.Init.Period = 2 * 50000 / 10; // 100ms Period
    TIM_Init.Init.Prescaler = PrescalerValue;
    TIM_Init.Init.ClockDivision = 0;
    TIM_Init.Init.RepetitionCounter = 0;
    TIM_Init.Init.CounterMode = TIM_COUNTERMODE_UP;

    TIM_IC_InitTypeDef TIM_ICInitStructure;
    TIM_ICInitStructure.ICPolarity = TIM_ICPOLARITY_RISING;
    TIM_ICInitStructure.ICSelection = TIM_ICSELECTION_DIRECTTI;
    TIM_ICInitStructure.ICPrescaler = TIM_ICPSC_DIV1;
    TIM_ICInitStructure.ICFilter = 0;

    HAL_NVIC_SetPriority(TIM3_IRQn, 10, 0);

    NVIC_SetVector(TIM3_IRQn, (uint32_t) &s4396122_hal_irremote_interrupt);
    NVIC_EnableIRQ(TIM3_IRQn);

    HAL_TIM_IC_Init(&TIM_Init);
    HAL_TIM_IC_ConfigChannel(&TIM_Init, &TIM_ICInitStructure, TIM_CHANNEL_2);

    HAL_TIM_IC_Start_IT(&TIM_Init, TIM_CHANNEL_2);
}

unsigned int lastTick;
void s4396122_hal_irremote_interrupt() {
    __HAL_TIM_CLEAR_IT(&TIM_Init, TIM_IT_TRIGGER);

    // unsigned int *input_malloc = malloc(sizeof(unsigned int));
    // *input_malloc = HAL_TIM_ReadCapturedValue(&TIM_Init, TIM_CHANNEL_2);
    // *input_malloc = HAL_GetTick() - lastTick;
    // lastTick = HAL_GetTick();
    // __HAL_TIM_SET_COUNTER(&TIM_Init, 0);
    // // debug_printf("I:%d\n", *input_malloc);
    //
    // if (HAL_GPIO_ReadPin(BRD_D0_GPIO_PORT, BRD_D0_PIN)) {
    //     s4396122_util_queue_push(IRQueue, input_malloc);
    // }

    int tim = HAL_TIM_ReadCapturedValue(&TIM_Init, TIM_CHANNEL_2);
    unsigned int *diff = malloc(sizeof(unsigned int));
    // *diff = HAL_GetTick() - lastTick;
    *diff = tim;
    // lastTick = tim;
    // debug_printf("%d\n", *diff);
    s4396122_util_queue_push(IRQueue, diff);
    __HAL_TIM_SET_COUNTER(&TIM_Init, TIM_CHANNEL_2);

    // if (approx(input_capture_value, 9000, 200)) {
    //     debug_printf("Yes\n");
    //     newMessage = 1;
    // } else if (newMessage == 1 && approx(input_capture_value, 2250, 200)) {
    //     debug_printf("Repeat\n");
    // } else if (approx(input_capture_value, 562, 100)) {
    //     debug_printf("End\n");
    // } else {
    //     newMessage = 0;
    // }
}

void s4396122_hal_irremote_process() {
    // debug_printf("Processing IR\n");
    for (int i = 0; i < s4396122_util_queue_size(IRQueue); i++) {
        unsigned int *temp = s4396122_util_queue_pop(IRQueue);
        // debug_printf("%d\n", *temp);
        if (*temp > 300) {
            free(temp);
            continue;
        }

        if (approx(*temp, 258, 3)) {
            debug_printf("Got IR\n");
            transmitionState = 1;
            IRaddress = 0;
            IRcommand = 0;
        } else {
            int readBit = 0;
            if (approx(*temp, 62, 2)) {
                readBit = 1;
            } else if (approx(*temp, 116, 2)) {
                readBit = 0;
            } else {
                // debug_printf("IR Error\n");
            }

            if (transmitionState < 9) {
                IRaddress <<= 1;
                IRaddress |= readBit;
                transmitionState++;
            } else if (transmitionState < 17) {
                transmitionState++;
            } else if (transmitionState < 25) {
                IRcommand <<= 1;
                IRcommand |= readBit;
                transmitionState++;
            } else if (transmitionState < 33) {
                transmitionState++;
            } else if (transmitionState == 33) {
                newIRMessage = IRcommand;
            }
        }

        free(temp);
    }
}

int s4396122_hal_irremote_get_input() {
    int oldMessage = newIRMessage;
    newIRMessage = 0;
    return oldMessage;
}

int s4396122_hal_irremote_input_available() {
    return newIRMessage;
}

char s4396122_hal_irremote_get_char() {
    char c;
    switch(s4396122_hal_irremote_get_input()) {
        case 93:
            c = '@';
            break;
        case 29:
            c = 'M';
            break;
        case 221:
            c = 'T';
            break;
        case 253:
            c = '+';
            break;
        case 61:
            c = 'B';
            break;
        case 31:
            c = '<';
            break;
        case 87:
            c = 'P';
            break;
        case 111:
            c = '>';
            break;
        case 151:
            c = '0';
            break;
        case 103:
            c = '-';
            break;
        case 79:
            c = 'C';
            break;
        case 207:
            c = '1';
            break;
        case 231:
            c = '2';
            break;
        case 133:
            c = '3';
            break;
        case 239:
            c = '4';
            break;
        case 199:
            c = '5';
            break;
        case 165:
            c = '6';
            break;
        case 189:
            c = '7';
            break;
        case 181:
            c = '8';
            break;
        case 173:
            c = '9';
            break;
        default:
            debug_printf("Unknown Code\n");
    }
    return c;
}
