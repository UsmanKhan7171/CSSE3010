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

#include "s4396122_hal_pantilt.h"

/**
 * Initializes all the hardware pins and the server
 */
void s4396122_hal_pantilt_init() {

}

/**
 * Writes the angle to the server for pan or tilt
 * @param type  Controls whether the pan or tilt should be affected by the value
 *              (0 = pan, 1 = tilt)
 * @param angle The angle to set the pan/tilt to
 */
void pantilt_angle_write(int type, int angle) {

}

/**
 * Reads the angle from the server for pan or tilt
 * @param  type Controls whether the pan or tilt should be affected by the value
 *              (0 = pan, 1 = tilt)
 * @return      The angle read from the server (from either pan or tilt)
 */
int pantilt_angle_read(int type) {

}
