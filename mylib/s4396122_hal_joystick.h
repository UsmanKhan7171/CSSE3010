/**
 * @file    s4396122_hal_joystick.h
 * @author  Daniel Fitzmaurice = 43961229
 * @date    150317
 * @brief   Library for reading the joystick input
 *          REFERENCE: stage3
 *******************************************************************************
 * s4396122_hal_joystick_init()     Initialise Joystick
 * int joystick_read(ADC Handler)   Internal Generic function to read specified
 *      joystick value
 * s4396122_hal_joystick_x_read()   Reads the X value from the joystick
 * s4396122_hal_joystick_y_read()   Reads the Y value from the joystick
 */

#ifndef HAL_JOYSTICK_H
#define HAL_JOYSTICK_H

#include <board.h>
#include <stm32f4xx_hal_conf.h>

#define s4396122_hal_joystick_x_read() joystick_read(AdcHandle)
#define s4396122_hal_joystick_y_read() joystick_read(AdcHandle)

ADC_HandleTypeDef AdcHandle;

void s4396122_hal_joystick_init();
int joystick_read(ADC_HandleTypeDef adc);

#endif
