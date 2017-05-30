/**
 * @file s4396122_hal_util.h
 * @brief External library for utility functions and constants
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef HAL_UTIL_H
#define HAL_UTIL_H

int map(int test, int oldMin, int oldMax, int newMin, int newMax);
int approx(int test, int expected, int range);

#endif
