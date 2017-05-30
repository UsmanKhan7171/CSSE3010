/**
 * @file s4396122_hal_util.c
 * @brief External library for utility functions and contants
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_hal_util.h"

/**
 * @brief Linear Maps an integer to a new value using min and max of old 
 * variables (based on the Arduino map function)
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

/**
 * @brief Checks to see if the test value is within the expected range
 * @param  test     The value to be tested on
 * @param  expected The expected value
 * @param  range    The range for the value to be tested within
 * @return          Returns true if the test value is within the range of the
 *                          expected value
 */
int approx(int test, int expected, int range) {

    if (test > (expected + range)) {

        return 0;
    } else if (test < (expected - range)) {

        return 0;
    } else {

        return 1;
    }
}
