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

int map(int test, int oldMin, int oldMax, int newMin, int newMax) {
    return ((test - oldMin) * (newMax - newMin) / (oldMax - oldMin)) + newMin;
}
