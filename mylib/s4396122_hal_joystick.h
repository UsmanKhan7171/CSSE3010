/**
 * @file s4396122_hal_joystick.h
 * @brief Library for the reading the joystick input
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef HAL_JOYSTICK_H
#define HAL_JOYSTICK_H

#include <board.h>
#include <stm32f4xx_hal_conf.h>

//! Returns the joystick x value
#define s4396122_hal_joystick_x_read() joystick_read(xAdcHandle)
//! Returns the joystick y value
#define s4396122_hal_joystick_y_read() joystick_read(yAdcHandle)

ADC_HandleTypeDef xAdcHandle;   //!< The adc handler for the x input
ADC_HandleTypeDef yAdcHandle;   //!< the adc handler for the y input

void s4396122_hal_joystick_init();
int joystick_read(ADC_HandleTypeDef adc);

#endif
