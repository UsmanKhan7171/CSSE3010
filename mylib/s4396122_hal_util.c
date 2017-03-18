/**
 * @file    s4396122_hal_util.c
 * @author  Daniel Fitzmaurice = 43961229
 * @date    180317
 * @brief   External library for utility functions and constants
 *******************************************************************************
 * s4396122_hal_util_map(x, oldMin, oldMax, newMin, newMax)     Maps a number
 *      to a new value through linear calculations
 */

#include "s4396122_hal_util.h"

/**
 * Linear Maps an integer to a new value using min and max of old variables
 * (based on the Arduino map function)
 * @param  test   The integer to be mapped
 * @param  oldMin The existing minimum value
 * @param  oldMax The existing maximum value
 * @param  newMin The new minimum value
 * @param  newMax The new maximum value
 * @return        The value that has been mapped
 */
int map(int test, int oldMin, int oldMax, int newMin, int newMax) {
    return ((test - oldMin) * (newMax - newMin) / (oldMax - oldMin)) + newMin;
}
