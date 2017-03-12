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

    // TODO: Factorize out the following code
    }

/**
 * Main Program Loop
 * @return  The exit code of the program
 */
int main(void) {
    BRD_init();
    Hardware_init();

    debug_printf("Holla\n");

    s4396122_hal_pantilt_pan_write(0);

    int degree = 0;

    while (1) { // Main function loop
        BRD_LEDToggle();

        char c = debug_getc();
        if (c != 0) {
            if (c == 'n') {
                degree -= 10;
            } else if (c == 'p') {
                degree += 10;
            }

            if (degree > 70) {
                degree = 70;
            } else if (degree < -70) {
                degree = -70;
            }

            s4396122_hal_pantilt_pan_write(degree);
        }
    }
}
