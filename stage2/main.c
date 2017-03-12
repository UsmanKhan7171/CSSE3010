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
    int stageSwitch = 0;
    int metronomeVal = 1;
    int metronomeDelay = 2000;
    unsigned int lastTick = HAL_GetTick();

    while (1) { // Main function loop
        BRD_LEDToggle();

        char c = debug_getc();
        if (c == 'n') {
            if (stageSwitch) {
                degree -= 10;
                if (degree < -70) {
                    degree = -70;
                }
            } else {
                degree = s4396122_hal_pantilt_pan_read();
                stageSwitch = 1;
                s4396122_hal_ledbar_write(0);
            }
        } else if (c == 'p') {
            degree += 10;
            if (degree > 70) {
                degree = 70;
            }
        } else if (c == 'm') {
            stageSwitch = 0;
            lastTick = HAL_GetTick();
            s4396122_hal_ledbar_write(0);
        } else if (c == '+') {
            metronomeDelay += 1000;
            if (metronomeDelay > 20000) {
                metronomeDelay = 20000;
            }
        } else if (c == '-') {
            metronomeDelay -= 1000;
            if (metronomeDelay < 2000) {
                metronomeDelay = 2000;
            }
        }

        if (stageSwitch) {
            // run stage 2.2
            s4396122_hal_pantilt_pan_write(degree);
        } else {
            // run stage 2.3
            if (HAL_GetTick() > (lastTick + metronomeDelay)) {
                s4396122_hal_pantilt_pan_write(40 * metronomeVal);
                lastTick = HAL_GetTick();
                if (metronomeVal == -1) {
                    uint16_t ledPos = 1;
                    for (int i = 0; i < 10; i++) {
                        s4396122_hal_ledbar_write(ledPos);
                        ledPos = ledPos << 1;
                        HAL_Delay(20);
                    }
                } else {
                    uint16_t ledPos = (1 << 9);
                    for (int i = 0; i < 10; i++) {
                        s4396122_hal_ledbar_write(ledPos);
                        ledPos = ledPos >> 1;
                        HAL_Delay(20);
                    }
                }
                metronomeVal *= -1;
            }
        }
    }
}
