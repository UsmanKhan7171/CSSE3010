/**
 * @file s4396122_hal_sysmon.c
 * @brief Library for interface between 3 LA channels and the np2
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_hal_sysmon.h"

/**
 * @brief Initializes the sysmon pins for interface
 */
void s4396122_hal_sysmon_init() {

    GPIO_InitTypeDef GPIO_InitStructure;

    // Enable the clocks
    __BRD_D9_GPIO_CLK();
    __BRD_D10_GPIO_CLK();
    __BRD_D12_GPIO_CLK();

    GPIO_InitStructure.Pin = LA_CHANNEL_0_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(LA_CHANNEL_0_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = LA_CHANNEL_1_PIN;
    HAL_GPIO_Init(LA_CHANNEL_1_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = LA_CHANNEL_2_PIN;
    HAL_GPIO_Init(LA_CHANNEL_2_PORT, &GPIO_InitStructure);
}
