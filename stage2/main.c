/**
 * *****************************************************************************
 * @file    stage2/main.c
 * @author  Daniel Fitzmaurice = 43961229
 * @date    080317
 * @brief   Main file for Stage 2
 * *****************************************************************************
 */

#include <board.h>
#include "s4396122_hal_ledbar.h"
#include "s4396122_hal_pantilt.h"

/**
 * Initializes the hardware or calls the functions require to initialize the
 * hardware
 */
void Hardware_init() {
    // Initialize and turn off the debug LED
    BRD_LEDInit();
    BRD_LEDOff();

    s4396122_hal_ledbar_init(); // initializes the ledbar
    s4396122_hal_pantilt_init(); // initializes the pantilt server
}

/**
 * Main Program Loop
 * @return  The exit code of the program
 */
int main(void) {
    Hardware_init();

    while (1) { // Main function loop

    }
}
