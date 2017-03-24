/**
 * @file    assignment1/main.c
 * @author  Daniel Fitzmaurice = 43961229
 * @date    220317
 * @brief   Main file for Assignment 1
 */

#include <board.h>
#include <debug_printf.h>
#include <stm32f4xx_hal_conf.h>
#include "s4396122_util_func_queue.h"
#include "s4396122_hal_pantilt.h"
#include "s4396122_hal_joystick.h"
#include "s4396122_hal_util.h"
#include "s4396122_hal_ledbar.h"

// public variables
int xDegree;    // Tracks the x and y degree of the pan and tilt motors
int yDegree;
unsigned int lastFuncAccuracy; // Stores the last time the check_func_accuracy
// function was called

/**
 * Checks to ensure that the function queue is running at optimal speeds and
 * that the system isn't beginning to lag behind
 * Function Queue function
 */
void check_func_accuracy() {
    int diff = HAL_GetTick() - lastFuncAccuracy - 50;
    lastFuncAccuracy = HAL_GetTick();
    if (diff <= -5 || diff >= 5) {
        debug_printf("Lag %d\n", diff);
    }
}

/**
 * Captures the serial input and sets the x and y degree of the pan tilt motors
 * to the correct angle
 * This is a Function Queue function
 */
void handle_serial_input() {
    switch (debug_getc()) {
        case 'w': // Move the tilt up
            yDegree -= 2;
            if (yDegree < -80) {
                yDegree = -80;
            }
            break;
        case 's': // Move the tilt down
            yDegree += 2;
            if (yDegree > 80) {
                yDegree = 80;
            }
            break;
        case 'a': // Pan to the left
            xDegree += 2;
            if (xDegree > 85) {
                xDegree = 85;
            }
            break;
        case 'd': // Pan to the right
            xDegree -= 2;
            if (xDegree < -85) {
                xDegree = -85;
            }
            break;
        case 'e': // Set the pantilt to a natural resting position
            xDegree = 0;
            yDegree = 55;
        case 'r': // Prints out the degree of the pantilt
            debug_printf("(%d, %d)\n", xDegree, yDegree);
            break;
    }
}

/**
 * Updates the pantilt motors to the correct angles using the pantilt library
 */
void update_pan_tilt_motor() {
    s4396122_hal_pantilt_pan_write(xDegree);
    s4396122_hal_pantilt_tilt_write(yDegree);
}

/**
 * Handles the joystick reading and input for the pantilt motors
 */
void handle_joystick_input() {
    // The joystick input and map it to a value between -10 and 10
    int x = map(s4396122_hal_joystick_x_read(), 0, 4096, 10, -10);
    int y = map(s4396122_hal_joystick_y_read(), 0, 4096, -10, 10);
    // If the value of x or y is greater or less then 2, the user has moved the
    // joystick. Otherwise it is just noise
    if (x >= 2 || x <= -2 || y >= 2 || y <= -2) {
        xDegree += x;
        yDegree -= y;

        // Make sure the degrees are still within the required range
        if (xDegree > 85) {
            xDegree = 85;
        } else if (xDegree < -85) {
            xDegree = -85;
        }

        if (yDegree > 80) {
            yDegree = 80;
        } else if (yDegree < -80) {
            yDegree = -80;
        }
    }
}

/**
 * Prints out the pan and tilt settings
 */
void print_pantilt_readings() {
    debug_printf("Pan: %d Tilt: %d\n", xDegree, yDegree);
}

/**
 * Initializes the hardware for Assignment 1
 */
void Hardware_init() {
    // Initializes the Board and then call the library init functions
    BRD_init();
    BRD_LEDInit();
    s4396122_hal_pantilt_init();
    s4396122_hal_joystick_init();
    s4396122_hal_ledbar_init();

    // Setup the global variables
    xDegree = 0;
    yDegree = 0;
    lastFuncAccuracy = HAL_GetTick();
}

/**
 * Main function for the np2
 * @return The return code
 */
int main() {

    Hardware_init();

    FuncQueue *queue = s4396122_util_func_queue_create();

    // Make sure the system is still alive
    s4396122_util_func_queue_add(queue, &BRD_LEDToggle, 250);

    s4396122_util_func_queue_add(queue, &handle_serial_input, 20);
    s4396122_util_func_queue_add(queue, &handle_joystick_input, 40);
    s4396122_util_func_queue_add(queue, &update_pan_tilt_motor, 20);
    s4396122_util_func_queue_add(queue, &print_pantilt_readings, 500);

    // Add a call to ensure that the system is not being overloaded with
    // functions
    s4396122_util_func_queue_add(queue, &check_func_accuracy, 50);

    while (1) { // Main execution loop

        // Every loop, run the tick to call the function queue functions
        s4396122_util_func_queue_tick(queue);

    }
}
