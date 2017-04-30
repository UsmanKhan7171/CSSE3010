/**
 * *****************************************************************************
 * @file    s4396122_hal_pantilt.c
 * @author  Daniel Fitzmaurice = 43961229
 * @date    080317
 * @brief   Pan and Tilt control system
 *          REFERENCE: stage2
 * *****************************************************************************
 * s4396122_hal_pantilt_init() = initialise the servo
 * s4396122_hal_pantilt_pan_write(int) = sets the pan of the servo to the int
 * s4396122_hal_pantilt_pan_read() = returns the pan of the servo
 */

#ifndef HAL_PANTILT_H
#define HAL_PANTILT_H

#include <board.h>
#include <stm32f4xx_hal_conf.h>

#define s4396122_hal_pantilt_pan_write(angle) pantilt_angle_write(0, angle)
#define s4396122_hal_pantilt_pan_read() pantilt_angle_read(0)
#define s4396122_hal_pantilt_tilt_write(angle) pantilt_angle_write(1, angle)
#define s4396122_hal_pantilt_tilt_read() pantilt_angle_read(1)

// Global Variables:
TIM_HandleTypeDef TIM_PanTilt; // Used to store the information for the pan servo

void s4396122_hal_pantilt_init();
void s4396122_hal_pantilt_deinit();

#endif
