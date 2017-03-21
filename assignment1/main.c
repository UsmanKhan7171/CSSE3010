/**
 * @file    assignment1/main.c
 * @author  Daniel Fitzmaurice = 43961229
 * @date    220317
 * @brief   Main file for Assignment 1
 */

#include <board.h>
#include <debug_printf.h>
#include <stm32f4xx_hal_conf.h>

/**
 * Initializes the hardware for Assignment 1
 */
void Hardware_init() {
    // Initializes the Board and then call the library init functions
    BRD_init();
    BRD_LEDInit();
}

/**
 * Main function for the np2
 * @return The return code
 */
int main() {

    Hardware_init();

    while (1) {

    }
}
