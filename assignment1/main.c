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
#include "s4396122_hal_hamming.h"
#include "s4396122_util_matrix.h"
#include "s4396122_hal_ircoms.h"
#include "s4396122_hal_time.h"

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
// Queues for data to be sent and queue for data that was last sent
Queue *transmitQueue;
Queue *lastTransmit;

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

/**
 * Used for handling encoding and decoding of the newline command feature
 * @param gotH           Progress state storage
 * @param manchesterMode If the user has chosen to use manchester or not
 * @param encoding       If the user wants to encode data
 * @param decoding       If the user wants to decode data
 * @param bothMode       If the user wants to transmit data over ir
 * @param inQueue        The resulting Queue of the data extracted
 */
void get_character_list(int *gotH, int *manchesterMode, int *encoding,
        int *decoding, int *bothMode, Queue *inQueue) {
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
        } else if (*queueData == 'T') {
            *gotH = 1;
            *bothMode = 1;
        } else if (*queueData == 'E' && *gotH) {
            *encoding = 1;
            *gotH = 0;
        } else if (*queueData == 'D' && *gotH) {
            *decoding = 1;
            *gotH = 0;
        } else if (*queueData >= '0' && *queueData <= '9' &&
                (*encoding || *decoding || *bothMode)) {
            int num = *queueData - '0';
            int *qNum = malloc(sizeof(int));
            *qNum = num;
            s4396122_util_queue_push(inQueue, qNum);
        } else if (*queueData >= 'A' && *queueData <= 'F' &&
                (*encoding || *decoding || *bothMode)) {
            int num = *queueData - 'A' + 10;
            int *qNum = malloc(sizeof(int));
            *qNum = num;
            s4396122_util_queue_push(inQueue, qNum);
        }
        free(queueData);
    }
}
/**
 * Encodes and decodes the data parsed in based on the input parameters
 * @param encoding       If the user wants to encode data
 * @param decoding       If the user wants to decode data
 * @param manchesterMode If the user has chosen to use manchester or not
 * @param convertedData  The resulting conversion of the encoded/decoded data
 * @param inQueue        The resulting Queue of the data extracted
 */
void process_character_list(int *encoding, int *decoding, int *manchesterMode,
        int *convertedData, Queue *inQueue) {
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
                // We don't have enough start a decode, buffer the current
                // input
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

/**
 * Encodes the data in the Queue with hamming and then matches the manchester
 * waveform and adds the data to the transmission queue
 * @param inQueue Queue of data to be encoded and sent
 */
void encode_hamming_manchester(Queue *inQueue) {
    Queue *totalQueue = s4396122_util_queue_create();
    while (1) {
        int *val = s4396122_util_queue_pop(inQueue);
        if (val == NULL)  {
            break;
        }
        unsigned int hamCode = s4396122_hal_hamming_encode(*val);
        Queue *manchesterQueue = s4396122_hal_ircoms_encode(hamCode);
        while (1) {
            int *m = s4396122_util_queue_pop(manchesterQueue);
            if (m == NULL) {
                break;
            }
            s4396122_util_queue_push(totalQueue, m);
        }
        s4396122_util_queue_free(manchesterQueue);
    }
    s4396122_util_queue_push(transmitQueue, totalQueue);
}

/**
 * Handles the newline serial character input
 */
void hamming_newline() {
    int encoding = 0;
    int decoding = 0;
    int convertedData = 0;
    int gotH = 0;
    int manchesterMode = 0;
    int bothMode = 0;
    Queue *inQueue = s4396122_util_queue_create();

    get_character_list(&gotH, &manchesterMode, &encoding, &decoding, &bothMode,
            inQueue);
    if (bothMode && encoding) { // The user transmits post hamming data
        Queue *totalQueue = s4396122_util_queue_create();
        while (1) {
            int *left = s4396122_util_queue_pop(inQueue);
            int *right = s4396122_util_queue_pop(inQueue);
            if (left == NULL || right == NULL) break;

            int hamCode = (*left << 4) | *right;
            Queue *manchesterQueue = s4396122_hal_ircoms_encode(hamCode);

            while (1) {
                int *d = s4396122_util_queue_pop(manchesterQueue);
                if (d == NULL) break;
                s4396122_util_queue_push(totalQueue, d);
            }
            s4396122_util_queue_free(manchesterQueue);

            free(left); free(right);
        }
        s4396122_util_queue_push(transmitQueue, totalQueue);
    } else if (bothMode) {
        encode_hamming_manchester(inQueue);
    } else {
        process_character_list(&encoding, &decoding, &manchesterMode,
                &convertedData, inQueue);
        if (manchesterMode && encoding) {
            // Print out the manchester waveform in binary form
            Queue *manchester = s4396122_hal_ircoms_encode(convertedData);
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

    s4396122_util_queue_free(inQueue);
}

/**
 * Handles transmitting manchester waveform from queue
 * @param q                  Queue to send data from
 * @param recordTransmission Whether the transmission is to be added to the
 * lastTransmit Queue
 */
void transmit_queue(Queue *q, int recordTransmission) {
    if (recordTransmission) {
        s4396122_util_queue_free(lastTransmit);
        lastTransmit = s4396122_util_queue_create();
    }
    while (1) {
        int *d = s4396122_util_queue_pop(q);
        if (recordTransmission) {
            s4396122_util_queue_push(lastTransmit, d);
        }
        if (d == NULL) break;
        while (s4396122_hal_time_get_micro() < 500);

        if (*d) {
            s4396122_hal_ir_datamodulation_set();
        } else {
            s4396122_hal_ir_datamodulation_cli();
        }

        if (!recordTransmission) {
            free(d);
        }

        s4396122_hal_time_clear_micro();
    }
}

/**
 * Function to be called by the function queue for going through the function
 * queue and transmitting any queued data
 */
void transmit_data() {
    s4396122_hal_time_clear_micro();
    Queue *q = s4396122_util_queue_pop(transmitQueue);
    if (q == NULL) return;

    transmit_queue(q, 1);

    s4396122_util_queue_free(q);
}

/**
 * Converts the ir input timing queue to a decoded manchester waveform
 * @param  IRQueue IR Timing Queue to be scanned and decoded
 * @return         Queue of decoded manchester waveform result
 */
Queue* handle_manchester_ir_input(Queue *IRQueue) {
    Queue *inputQueue = s4396122_util_queue_create();
    free(s4396122_util_queue_pop(IRQueue));

    int *c = malloc(sizeof(int));
    *c = 1;
    s4396122_util_queue_push(inputQueue, c);

    int currentBit = 1;
    int even = 1;
    while (1) {
        int *val = s4396122_util_queue_pop(IRQueue);
        if (val == NULL) break;

        if (approx(*val, 1000, 100)) {
            currentBit = !currentBit;
            int *d = malloc(sizeof(int));
            *d = currentBit;
            s4396122_util_queue_push(inputQueue, d);
            even = 0;
        } else if (approx(*val, 500, 100)) {
            if (even) {
                int *d = malloc(sizeof(int));
                *d = currentBit;
                s4396122_util_queue_push(inputQueue, d);
            }
            even = !even;
        }

        free(val);
    }
    return inputQueue;
}

/**
 * Checks which mode the ir receiver should be in (manchester or NEC) and
 * handles the code for those according functions
 */
void handle_ir_input() {
    if (NECIRInput) {
        // Handle IRRemote input
        s4396122_hal_irremote_process(s4396122_hal_ir_get_queue());
        handle_irremote_input();
    } else {
        // Handle manchester input
        Queue *manchesterQueue =
            handle_manchester_ir_input(s4396122_hal_ir_get_queue());
        if (s4396122_util_queue_size(manchesterQueue) > 1) {
            // There is input to be handled
            unsigned int finalResult = 0;
            for (int i = 0; i < 2; i++) {
                // Remove the two start bits
                free(s4396122_util_queue_pop(manchesterQueue));
                free(s4396122_util_queue_pop(manchesterQueue));
                // Loop through the Queue and store in an int for easy
                // manipulation later
                unsigned int result = 0;
                for (int j = 0; j < 8; j++) {
                    int *d = s4396122_util_queue_pop(manchesterQueue);
                    result |= (*d << j);
                    free(d);
                }
                unsigned int hamResult = s4396122_hal_hamming_decode(result);
                if (hamResult == 0xFFFF) {
                    // If there is an error in the ham decoding, send the NAK
                    // byte
                    debug_printf("Sending NAK byte\n");
                    Queue *errorQueue = s4396122_util_queue_create();
                    int errorCode1 = 1;
                    int errorCode2 = 5;
                    s4396122_util_queue_push(errorQueue, &errorCode1);
                    s4396122_util_queue_push(errorQueue, &errorCode2);
                    encode_hamming_manchester(errorQueue);
                    s4396122_util_queue_free(errorQueue);
                    free(manchesterQueue);
                    return;
                }
                // Move the final result to the left to allow for the right
                finalResult <<= 4;
                finalResult |= hamResult;
                // Remove the last stop bit
                free(s4396122_util_queue_pop(manchesterQueue));
            }
            if (finalResult == 21) {
                debug_printf("Resending last transmission\n");
                transmit_queue(lastTransmit, 0);
            } else if (finalResult == 6) {
                // Do nothing cause the ack was accepted
            } else {
                // There was no problems so send the ACK byte
                debug_printf("Got: %X\n", finalResult);
                Queue *ackQueue = s4396122_util_queue_create();
                int ackCode1 = 0;
                int ackCode2 = 6;
                s4396122_util_queue_push(ackQueue, &ackCode1);
                s4396122_util_queue_push(ackQueue, &ackCode2);
                encode_hamming_manchester(ackQueue);
                s4396122_util_queue_free(ackQueue);
            }
        }
        s4396122_util_queue_free(manchesterQueue);
    }

    // Clear out the left over IR input for a clean input for the next call
    while (1) {
        int *d = s4396122_util_queue_pop(s4396122_hal_ir_get_queue());
        if (d == NULL) {
            break;
        }
        free(d);
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
    s4396122_hal_time_init();

    // Create the serial queue for storing of unused characters
    serialQueue = s4396122_util_queue_create();

    // Setup the global variables
    xDegree = 0;
    yDegree = 0;
    currentAngle = 0;
    lastFuncAccuracy = HAL_GetTick();
    NECIRInput = 1;
    transmitQueue = s4396122_util_queue_create();
    lastTransmit = s4396122_util_queue_create();

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

/**
 * Main function for the np2
 * @return The return code
 */
int main() {

    Hardware_init();

    FuncQueue *queue = s4396122_util_func_queue_create();

    // Make sure the system is still alive
    s4396122_util_func_queue_add(queue, &BRD_LEDToggle, 250);

    s4396122_util_func_queue_add(queue, &transmit_data, 50);

    s4396122_util_func_queue_add(queue, &handle_serial_input, 20);
    s4396122_util_func_queue_add(queue, &handle_joystick_input, 40);
    s4396122_util_func_queue_add(queue, &update_pan_tilt_motor, 20);
    s4396122_util_func_queue_add(queue, &print_pantilt_readings, 500);

    s4396122_util_func_queue_add(queue, &handle_ir_input, 200);

    // Add a call to ensure that the system is not being overloaded with
    // functions
    s4396122_util_func_queue_add(queue, &check_func_accuracy, 50);

    while (1) { // Main execution loop

        // Every loop, run the tick to call the function queue functions
        s4396122_util_func_queue_tick(queue);

    }
}
