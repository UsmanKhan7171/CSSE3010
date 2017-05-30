/**
 * @file s4396122_hal_irremote.c
 * @brief Library for processing and reporting of the irremote
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_hal_irremote.h"

int transmitionState;   //!< Used to keep track of the position in the ir code
int IRaddress;          //!< Address of the ir remote
int IRcommand;          //!< Last received ir command
int newIRMessage;       //!< If there is a new IR message waiting

/**
 * @brief Initializes the pins and timers needed for the irremote
 */
void s4396122_hal_irremote_init() {

    transmitionState = 0;
    newIRMessage = 0;
}

/**
 * @brief Processes an data stored inside the IRQueue and calculates the ir 
 * address and command
 */
void s4396122_hal_irremote_process(Queue *IRQueue) {

    while (1) {

        unsigned int *state = s4396122_util_queue_pop(IRQueue);
        if (state == NULL) { 

            return;
        }

        if (approx(*state, 9100, 100)) {

            unsigned int *down = s4396122_util_queue_pop(IRQueue);
            if (down == NULL) { 

                return;
            }

            if (approx(*down, 4520, 100)) {

                break;
            } else if (approx(*down, 2250, 100)) {

                newIRMessage = IRcommand;
            }
        }
    }

    transmitionState = 0;
    IRaddress = 0;
    IRcommand = 0;
    while (1) {

        unsigned int *high = s4396122_util_queue_pop(IRQueue);
        unsigned int *low = s4396122_util_queue_pop(IRQueue);
        if (high == NULL || low == NULL) {

            break;
        }

        int bit = -1;
        if (approx(*high, 562, 100) && approx(*low, 562, 100)) {

            bit = 1;
        } else if (approx(*high, 562, 100) && approx(*low, 1687, 100)) {

            bit  = 0;
        } else {

            debug_printf("Invalid Pair: (%d, %d)\n", *high, *low);
            free(high);
            free(low);
            break;
        }

        if (transmitionState < 8) {

            IRaddress <<= 1;
            IRaddress |= bit;
        } else if (transmitionState < 16) {

        } else if (transmitionState < 24) {

            IRcommand <<= 1;
            IRcommand |= bit;
        } else if (transmitionState < 32) {

        }
        transmitionState++;

        if (transmitionState == 32) {

            newIRMessage = IRcommand;
            free(high);
            free(low);
            break;
        }

        free(high);
        free(low);
    }
}

/**
 * @brief Returns the IR Command that was last received and sets the message to 
 * 0, can only be used once per command
 * @return the IR Message that was last unread
 */
int s4396122_hal_irremote_get_input() {

    int oldMessage = newIRMessage;
    newIRMessage = 0;
    return oldMessage;
}

/**
 * @brief Checks if there a IR message command pending to be read
 * @return 0 if there is no message, otherwise there is a message
 */
int s4396122_hal_irremote_input_available() {

    return newIRMessage;
}

/**
 * @brief Reads the IR message command and transforms it to a character 
 * symbolized by the remote
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
