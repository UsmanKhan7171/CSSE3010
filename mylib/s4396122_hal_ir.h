/**
 * @file    s4396122_hal_ir.h
 * @author  Daniel Fitzmaurice = 43961229
 * @date    150317
 * @brief   Control library for the IR transmitter and receiver
 *          REFERENCE: stage3
 *******************************************************************************
 * s4396122_hal_ir_init()               Initialise the IR receiver/transmitter
 * irhal_carrier(state)                 Internal generic function for turning
 *      the carrier wave on or off
 * s4396122_hal_ir_carrieron()  Turns on the carrier signal
 * s4396122_hal_ir_carrieroff() Turns off the carrier signal
 * s4396122_hal_ir_datamodulation_set() Set the data out to high
 * s4396122_hal_ir_datamodulation_cli() Set the data out to low
 * s4396122_hal_ir_get_queue() Gets the IRQueue and returns it for global access
 */

#ifndef HAL_IR_H
#define HAL_IR_H

#include <board.h>
#include <stm32f4xx_hal_conf.h>
#include "s4396122_util_queue.h"

#define s4396122_hal_ir_carrieron() irhal_carrier(1);
#define s4396122_hal_ir_carrieroff() irhal_carrier(0);
#define s4396122_hal_ir_datamodulation_set() \
        HAL_GPIO_WritePin(BRD_D8_GPIO_PORT, BRD_D8_PIN, 1);
#define s4396122_hal_ir_datamodulation_cli() \
        HAL_GPIO_WritePin(BRD_D8_GPIO_PORT, BRD_D8_PIN, 0);

Queue *IRQueue; // Global variable for storing and accessing the IR data

void s4396122_hal_ir_init();
void irhal_carrier(int state);
Queue* s4396122_hal_ir_get_queue();

#endif
