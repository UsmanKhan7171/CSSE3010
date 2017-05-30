/**
 * @file s4396122_os_joystick.c
 * @brief Library for os interface to joystick peripheral
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_os_joystick.h"

// Local Globals
unsigned int lastInterval; //!< Used for Z Input debounce

/**
 * @brief Joystick Z input interrupt function
 */
void exti_a2_interrupt_handler() {

    HAL_GPIO_EXTI_IRQHandler(BRD_A2_PIN);

    // If it has been less than 100 millis than consider it bounce
    if (HAL_GetTick() > (lastInterval + 100)) {

        BaseType_t xHigherPriorityTaskWoken;
        xHigherPriorityTaskWoken = pdFALSE;

        if (s4396122_SemaphoreJoystickZ != NULL) {

            xSemaphoreGiveFromISR(s4396122_SemaphoreJoystickZ,
                &xHigherPriorityTaskWoken);
        }

        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
    lastInterval = HAL_GetTick();
}

/**
 * @brief Initializes the joystick peripherial
 */
void s4396122_os_joystick_init() {

    s4396122_SemaphoreJoystickZ = xSemaphoreCreateBinary();

    GPIO_InitTypeDef GPIO_InitStructure;

    __BRD_A2_GPIO_CLK();
    HAL_NVIC_SetPriority(BRD_A2_EXTI_IRQ, 10, 0);
    NVIC_SetVector(BRD_A2_EXTI_IRQ, (uint32_t) &exti_a2_interrupt_handler);
    NVIC_EnableIRQ(BRD_A2_EXTI_IRQ);

    GPIO_InitStructure.Pin = BRD_A2_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(BRD_A2_GPIO_PORT, &GPIO_InitStructure);

    lastInterval = HAL_GetTick();

}
