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
    // Initialize the hardware
    BRD_init();
    Hardware_init();

    // Main loop state variables
    int degree = 0; // Used to track the degree of the servo with normal mode
    int stageSwitch = 0; // Stores state between normal and metronome
    int metronomeVal = 1; // Controls the direction of the next metronome swing
    int metronomeDelay = 2000; // The current delay before next swing
    unsigned int lastTick = HAL_GetTick(); // Last time the metronome was
    // updated

    while (1) { // Main function loop
        BRD_LEDToggle(); // Make sure the system is alive

        // Get a character and check if it meets the character input
        // requirements
        char c = debug_getc();
        if (c == 'n') {
            // If currently in normal mode, then reduce the degree, if not in
            // normal mode, then switch to normal mode
            if (stageSwitch) {
                // Decrease the degree by 10 and make sure it is still within
                // the required bounds
                degree -= 10;
                if (degree < -70) {
                    degree = -70;
                }
            } else {
                // Set the current degree as the degree of the servo and switch
                // the states
                // degree = s4396122_hal_pantilt_pan_read();
                stageSwitch = 1;
                s4396122_hal_ledbar_write(0); // Clear the ledbar
            }
        } else if (c == 'p') {
            // Increment the degree rotation
            degree += 10;
            if (degree > 70) {
                degree = 70;
            }
        } else if (c == 'm') {
            // Switch to metronome state. Se the tick and ledbar to reduce cross
            // over errors
            stageSwitch = 0;
            lastTick = HAL_GetTick();
            s4396122_hal_ledbar_write(0);
        } else if (c == '+') {
            // Increment the metronome delay but keep it within the required
            // bounds
            metronomeDelay += 1000;
            if (metronomeDelay > 20000) {
                metronomeDelay = 20000;
            }
        } else if (c == '-') {
            // Decrement the metronome delay but keep it within the required
            // bounds
            metronomeDelay -= 1000;
            if (metronomeDelay < 2000) {
                metronomeDelay = 2000;
            }
        }

        int ledbar_pos = 1;

        if (stageSwitch) {
            // run stage 2.2
            // Stage 2 runs at the very end of the if statement (code
            // optimizations)
            int pos = (degree + 70) / 14;
            ledbar_pos = ledbar_pos << pos;
        } else {
            // run stage 2.3
            if (HAL_GetTick() > (lastTick + metronomeDelay)) {
                metronomeVal *= -1;
                lastTick = HAL_GetTick();
            }

            int step = 80 * ((lastTick + metronomeDelay) - HAL_GetTick()) / metronomeDelay;
            step -= 40;
            step *= metronomeVal;
            degree = step;
            int pos = (degree + 40) / 8;
            ledbar_pos = ledbar_pos << pos;
        }

        s4396122_hal_ledbar_write(ledbar_pos);
        s4396122_hal_pantilt_pan_write(degree); // update the degree
    }
}
