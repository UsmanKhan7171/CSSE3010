#include "s4396122_hal_irremote.h"

TIM_HandleTypeDef TIM_Init; // Keep as a local definition

/**
 * Initializes the pins and timers needed for the irremote
 */
void s4396122_hal_irremote_init() {
    newIRMessage = 0;
}

/**
 * Processes an data stored inside the IRQueue and calculates the ir address
 * and command
 */
void s4396122_hal_irremote_process(Queue *IRQueue) {
    while (1) {
        unsigned int *temp = s4396122_util_queue_pop(IRQueue);
        if (temp == NULL) {
            break;
        }

        if (*temp > 300) {
            free(temp);
            continue;
        }

        if (approx(*temp, 258, 3)) {
            transmitionState = 1;
            IRaddress = 0;
            IRcommand = 0;
        } else {
            int readBit = 0;
            if (approx(*temp, 62, 2)) {
                readBit = 1;
            } else if (approx(*temp, 116, 2)) {
                readBit = 0;
            } else {
                // debug_printf("IR Error\n");
            }

            if (transmitionState < 9) {
                IRaddress <<= 1;
                IRaddress |= readBit;
                transmitionState++;
            } else if (transmitionState < 17) {
                transmitionState++;
            } else if (transmitionState < 25) {
                IRcommand <<= 1;
                IRcommand |= readBit;
                transmitionState++;
            } else if (transmitionState < 33) {
                transmitionState++;
            } else if (transmitionState == 33) {
                newIRMessage = IRcommand;
            }
        }

        free(temp);
    }
}

/**
 * Returns the IR Command that was last received and sets the message to 0, can
 * only be used once per command
 * @return the IR Message that was last unread
 */
int s4396122_hal_irremote_get_input() {
    int oldMessage = newIRMessage;
    newIRMessage = 0;
    return oldMessage;
}

/**
 * Checks if there a IR message command pending to be read
 * @return 0 if there is no message, otherwise there is a message
 */
int s4396122_hal_irremote_input_available() {
    return newIRMessage;
}

/**
 * Reads the IR message command and transforms it to a character symbolized by
 * the remote
 * @return A character mapped to the irremote
 */
char s4396122_hal_irremote_get_char() {
    switch(s4396122_hal_irremote_get_input()) {
        case 93:
            return '@';
        case 29:
            return 'M';
        case 221:
            return 'T';
        case 253:
            return '+';
        case 61:
            return 'B';
        case 31:
            return '<';
        case 87:
            return 'P';
        case 111:
            return '>';
        case 151:
            return '0';
        case 103:
            return '-';
        case 79:
            return 'C';
        case 207:
            return '1';
        case 231:
            return '2';
        case 133:
            return '3';
        case 239:
            return '4';
        case 199:
            return '5';
        case 165:
            return '6';
        case 189:
            return '7';
        case 181:
            return '8';
        case 173:
            return '9';
        default:
            debug_printf("Unknown Code\n");
    }
}
