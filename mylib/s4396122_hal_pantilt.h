/**
 * @file s4396122_hal_pantilt.h
 * @brief Pan and Tilt control system
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef HAL_PANTILT_H
#define HAL_PANTILT_H

#include <board.h>
#include <stm32f4xx_hal_conf.h>

//! Sets the pan angle
#define s4396122_hal_pantilt_pan_write(angle) pantilt_angle_write(0, angle)
//! Gets the pan angle
#define s4396122_hal_pantilt_pan_read() pantilt_angle_read(0)
//! Sets the tilt angle
#define s4396122_hal_pantilt_tilt_write(angle) pantilt_angle_write(1, angle)
//! Gets the tilt angle
#define s4396122_hal_pantilt_tilt_read() pantilt_angle_read(1)

// Global Variables:
//! Used to store the information for the pan servo
TIM_HandleTypeDef TIM_PanTilt;

void s4396122_hal_pantilt_init();
void s4396122_hal_pantilt_deinit();

#endif
