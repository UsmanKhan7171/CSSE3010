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
#include "s4396122_hal_irremote.h"
#include "s4396122_hal_ir.h"
#include "s4396122_util_queue.h"
#include "s4396122_util_map.h"
#include "s4396122_hal_accel.h"
#include "s4396122_hal_hamming.h"
#include "s4396122_util_matrix.h"
#include "s4396122_hal_ircoms.h"

// public variables
int xDegree;    // Tracks the x and y degree of the pan and tilt motors
int yDegree;
unsigned int lastFuncAccuracy; // Stores the last time the check_func_accuracy
// function was called
int currentAngle;
// Maps for handling ir remote and serial input
Map *remoteMap;
Map *serialMap;
Queue *serialQueue;
// mode switch for ir input
int NECIRInput;

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
 * Makes sure that the pantilt is within the required bounds and if it is
 * outside the bounds then make sure it stays within the bounds
 */
void handle_pantilt_angle_bounds() {
    if (xDegree < -85) {
        xDegree = -85;
    } else if (xDegree > 85) {
        xDegree = 85;
    }
    if (yDegree < -80) {
        yDegree = -80;
    } else if (yDegree > 80) {
        yDegree = 80;
    }
}

/**
 * Captures the serial input and sets the x and y degree of the pan tilt motors
 * to the correct angle
 * This is a Function Queue function
 */
void handle_serial_input() {
    int c = debug_getc();
    if (c != 0) {
        void (*f)(void) = s4396122_util_map_get(serialMap, c);
        if (f == NULL) {
            // debug_printf("Error Finding Function: '%d'\n", c);
            int *newData = malloc(sizeof(int));
            *newData = c;
            s4396122_util_queue_push(serialQueue, newData);
            return;
        }
        int oldAngle = currentAngle;
        currentAngle = -1;
        f();
        currentAngle = oldAngle;
        handle_pantilt_angle_bounds();
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
        handle_pantilt_angle_bounds();
    }
}

/**
 * Handles the irremote reading and input for the pantilt motors
 */
void handle_irremote_input() {
    if (s4396122_hal_irremote_input_available()) {
        char c = s4396122_hal_irremote_get_char();
        debug_printf("RECEIVED FROM REMOTE: %c\n", c);
        void (*f)(char c) = s4396122_util_map_get(remoteMap, (int) c);
        if (f == NULL) {
            debug_printf("Error Finding Function\n");
            return;
        }
        f(c);
        handle_pantilt_angle_bounds();
    }
}

/**
 * Handles moving the pantilt motors left using ir
 * @param c Character from the remote
 */
void ir_move_left(char c) {
    if (currentAngle == -1) {
        xDegree += 1;
    } else {
        xDegree += currentAngle;
        currentAngle = -1;
    }
}

/**
 * Handles moving the pantilt motors right using ir
 * @param c Character from the remote
 */
void ir_move_right(char c) {
    if (currentAngle == -1) {
        xDegree -= 1;
    } else {
        xDegree -= currentAngle;
        currentAngle = -1;
    }
}

/**
 * Handles moving the pantilt motors up using ir
 * @param c Character from the remote
 */
void ir_move_up(char c) {
    if (currentAngle == -1) {
        yDegree -= 1;
    } else {
        yDegree -= currentAngle;
        currentAngle = -1;
    }
}

/**
 * Handles moving the pantilt motors down using ir
 * @param c Character from the remote
 */
void ir_move_down(char c) {
    if (currentAngle == -1) {
        yDegree += 1;
    } else {
        yDegree += currentAngle;
        currentAngle = -1;
    }
}

/**
 * Handles moving the pantilt motors back to the center using ir
 * @param c Character from the remote
 */
void ir_move_center(char c) {
    xDegree = 0;
    yDegree = 55;
}

/**
 * Handles the number input from the ir remote
 * @param c Character from the remote
 */
void ir_handle_num(char c) {
    if (currentAngle == -1) {
        currentAngle = 0;
    }
    int num = c - '0';
    currentAngle = currentAngle * 10 + num;
}

/**
 * Handles clearing the number input from the ir remote
 * @param c Character from the remote
 */
void ir_handle_clear(char c) {
    currentAngle = -1;
}

/**
 * Handles switching of NECIRInput using the serial input
 */
void handle_mode_switch() {
    NECIRInput = !NECIRInput;
    if (NECIRInput) {
        debug_printf("In NEC IR Remote input\n");
    } else {
        debug_printf("In Manchester input\n");
    }
}

/**
 * Prints out the pan and tilt settings
 */
void print_pantilt_readings() {
    debug_printf("Pan: %d Tilt: %d\n", xDegree, yDegree);
}

void handle_accel_input() {
    int accel_x = s4396122_hal_accel_read_addr(0x05);
    debug_printf("X: %d\n", accel_x);
}

void get_character_list(int *gotH, int *manchesterMode, int *encoding, int *decoding, Queue *inQueue) {
    while (1) {
        int *queueData = s4396122_util_queue_pop(serialQueue);
        if (queueData == NULL) {
            break;
        }
        if (*queueData == 'H') {
            *gotH = 1;
        } else if (*queueData == 'M') {
            *gotH = 1;
            *manchesterMode = 1;
        } else if (*queueData == 'E' && *gotH) {
            *encoding = 1;
            *gotH = 0;
        } else if (*queueData == 'D' && *gotH) {
            *decoding = 1;
            *gotH = 0;
        } else if (*queueData >= '0' && *queueData <= '9' &&
                (*encoding || *decoding)) {
            int num = *queueData - '0';
            int *qNum = malloc(sizeof(int));
            *qNum = num;
            s4396122_util_queue_push(inQueue, qNum);
        } else if (*queueData >= 'A' && *queueData <= 'F' &&
                (*encoding || *decoding)) {
            int num = *queueData - 'A' + 10;
            int *qNum = malloc(sizeof(int));
            *qNum = num;
            s4396122_util_queue_push(inQueue, qNum);
        }
        free(queueData);
    }
}

void process_character_list(int *encoding, int *decoding, int *manchesterMode, int *convertedData,
        Queue *inQueue) {
    unsigned int lastDecodeVal = 0;
    int gotDecodeVal = 0;
    while (1) {
        if (*decoding && *manchesterMode) {
            *convertedData = s4396122_hal_ircoms_decode(inQueue);
        }
        int *val = s4396122_util_queue_pop(inQueue);
        if (val == NULL) {
            break;
        }

        if (*encoding && *manchesterMode == 0) {
            int convertedVal = s4396122_hal_hamming_encode(*val);
            *convertedData <<= 8;
            *convertedData |= convertedVal;
        } else if (*decoding && *manchesterMode == 0) {
            if (gotDecodeVal) {
                // We have two words, start to decode the input
                gotDecodeVal = 0;
                unsigned int totalVal = (lastDecodeVal << 4) | *val;
                int convertedVal = s4396122_hal_hamming_decode(totalVal);
                *convertedData <<= 4;
                *convertedData |= convertedVal;
            } else {
                // We don't have enough start a decode, buffer the current input
                lastDecodeVal = *val;
                gotDecodeVal = 1;
            }
        } else if (*encoding && *manchesterMode) {
            *convertedData <<= 4;
            *convertedData |= *val;
        } else {
            debug_printf("Encoding or Decoding not selected!\n");
            return;
        }

        free(val);
    }
}

void hamming_newline() {
    int encoding = 0;
    int decoding = 0;
    int convertedData = 0;
    int gotH = 0;
    int manchesterMode = 0;
    Queue *inQueue = s4396122_util_queue_create();
    get_character_list(&gotH, &manchesterMode, &encoding, &decoding, inQueue);
    process_character_list(&encoding, &decoding, &manchesterMode, &convertedData, inQueue);

    s4396122_util_queue_free(inQueue);

    if (manchesterMode && encoding) {
        Queue *manchester = s4396122_hal_ircoms_encode(convertedData);
        debug_printf("Size: %d\n", s4396122_util_queue_size(manchester));
        while (1) {
            int *d = s4396122_util_queue_pop(manchester);
            if (d == NULL) {
                break;
            }
            debug_printf("%d", *d);
            free(d);
        }
        s4396122_util_queue_free(manchester);
        debug_printf("\n");
    } else {
        debug_printf("%X\n", convertedData);
    }

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
    s4396122_hal_irremote_init();
    s4396122_hal_accel_init();
    s4396122_hal_hamming_init();
    s4396122_hal_ir_init();

    // Create the serial queue for storing of unused characters
    serialQueue = s4396122_util_queue_create();

    // Setup the global variables
    xDegree = 0;
    yDegree = 0;
    currentAngle = 0;
    lastFuncAccuracy = HAL_GetTick();
    NECIRInput = 1;

    // Creates the ir remote control mapping
    remoteMap = s4396122_util_map_create();
    s4396122_util_map_add(remoteMap, (int) '<', &ir_move_left);
    s4396122_util_map_add(remoteMap, (int) '>', &ir_move_right);
    s4396122_util_map_add(remoteMap, (int) '+', &ir_move_up);
    s4396122_util_map_add(remoteMap, (int) '-', &ir_move_down);
    s4396122_util_map_add(remoteMap, (int) 'P', &ir_move_center);
    for (int i = '0'; i <= '9'; i++) {
        s4396122_util_map_add(remoteMap, i, &ir_handle_num);
    }
    s4396122_util_map_add(remoteMap, (int) 'C', &ir_handle_clear);

    // Creates the serial control mapping
    serialMap = s4396122_util_map_create();
    s4396122_util_map_add(serialMap, (int) 'w', &ir_move_up);
    s4396122_util_map_add(serialMap, (int) 's', &ir_move_down);
    s4396122_util_map_add(serialMap, (int) 'a', &ir_move_left);
    s4396122_util_map_add(serialMap, (int) 'd', &ir_move_right);
    s4396122_util_map_add(serialMap, (int) '`', &handle_mode_switch);
    s4396122_util_map_add(serialMap, 13, &hamming_newline);

}

void handle_ir_input() {
    if (NECIRInput) {
        // Handle IRRemote input
        s4396122_hal_irremote_process(s4396122_hal_ir_get_queue());
        handle_irremote_input();
    } else {
        // Handle manchester input
    }

    while (1) {
        int *d = s4396122_util_queue_pop(s4396122_hal_ir_get_queue());
        if (d == NULL) {
            break;
        }
        free(d);
    }
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
    // TODO: Change the pantilt printout
    s4396122_util_func_queue_add(queue, &print_pantilt_readings, 5000);

    // Add a call to ensure that the system is not being overloaded with
    // functions
    s4396122_util_func_queue_add(queue, &check_func_accuracy, 50);

    // s4396122_util_func_queue_add(queue, &s4396122_hal_irremote_process, 50);
    // s4396122_util_func_queue_add(queue, &handle_irremote_input, 50);
    s4396122_util_func_queue_add(queue, &handle_ir_input, 50);

    // s4396122_util_func_queue_add(queue, &handle_accel_input, 100);

    while (1) { // Main execution loop

        // Every loop, run the tick to call the function queue functions
        s4396122_util_func_queue_tick(queue);

    }
}
