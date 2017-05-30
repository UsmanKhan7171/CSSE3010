/**
 * @file s4396122_hal_ir.h
 * @brief Control library for the IR transmitter and receiver
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef HAL_IR_H
#define HAL_IR_H

#include <board.h>
#include <stm32f4xx_hal_conf.h>
#include "s4396122_util_queue.h"

//! Enables the ir carrier signal
#define s4396122_hal_ir_carrieron() irhal_carrier(1);
//! Disables the ir carrier signal
#define s4396122_hal_ir_carrieroff() irhal_carrier(0);
//! Sets the ir output to high
#define s4396122_hal_ir_datamodulation_set() \
        HAL_GPIO_WritePin(BRD_D8_GPIO_PORT, BRD_D8_PIN, 1);
//! Sets the ir output to low
#define s4396122_hal_ir_datamodulation_cli() \
        HAL_GPIO_WritePin(BRD_D8_GPIO_PORT, BRD_D8_PIN, 0);

Queue *IRQueue; //!< Global variable for storing and accessing the IR data

void s4396122_hal_ir_init();
void irhal_carrier(int state);
Queue* s4396122_hal_ir_get_queue();

#endif
