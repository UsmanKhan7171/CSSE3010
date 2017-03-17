/**
 * @file    s4396122_hal_joystick.c
 * @author  Daniel Fitzmaurice = 43961229
 * @date    150317
 * @brief   Control library for the IR transmitter and receiver
 *          REFERENCE: stage3
 *******************************************************************************
 * s4396122_hal_joystick_init()     Initialise Joystick
 * int joystick_read(ADC Handler)   Internal Generic function to read specified
 *      joystick value
 */

#include "s4396122_hal_joystick.h"

void s4396122_hal_joystick_init() {
    // ADC_HandleTypeDef AdcHandle;
    ADC_ChannelConfTypeDef AdcChanConfig;
    GPIO_InitTypeDef GPIO_InitStructure;

    __BRD_A0_GPIO_CLK();
    GPIO_InitStructure.Pin = BRD_A0_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(BRD_A0_GPIO_PORT, &GPIO_InitStructure);

    __ADC1_CLK_ENABLE();
    AdcHandle.Instance = (ADC_TypeDef*) ADC1_BASE;
    AdcHandle.Init.ClockPrescaler = ADC_CLOCKPRESCALER_PCLK_DIV2;
    AdcHandle.Init.Resolution = ADC_RESOLUTION12b;
    AdcHandle.Init.ScanConvMode = DISABLE;
    AdcHandle.Init.ContinuousConvMode = DISABLE;
    AdcHandle.Init.DiscontinuousConvMode = DISABLE;
    AdcHandle.Init.NbrOfDiscConversion = 0;
    AdcHandle.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
    AdcHandle.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONV_T1_CC1;
    AdcHandle.Init.DataAlign = ADC_DATAALIGN_RIGHT;
    AdcHandle.Init.NbrOfConversion = 1;
    AdcHandle.Init.DMAContinuousRequests = DISABLE;
    AdcHandle.Init.EOCSelection = DISABLE;
    HAL_ADC_Init(&AdcHandle);

    AdcChanConfig.Channel = BRD_A0_ADC_CHAN;
    AdcChanConfig.Rank = 1;
    AdcChanConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    AdcChanConfig.Offset = 0;
    HAL_ADC_ConfigChannel(&AdcHandle, &AdcChanConfig);
}

int joystick_read(ADC_HandleTypeDef adc) {
    HAL_ADC_Start(&adc);
    while (HAL_ADC_PollForConversion(&adc, 10) != HAL_OK);
    return (uint16_t) HAL_ADC_GetValue(&adc);
}
