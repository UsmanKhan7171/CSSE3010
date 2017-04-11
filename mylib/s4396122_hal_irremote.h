/**
 * @file   s4396122_hal_irremote.h
 * @author Daniel Fitzmaurice = 43961229
 * @date   120417
 * @brief  Library for processing and reporting of the irremote
 */
#ifndef HAL_IRREMOTE_H
#define HAL_IRREMOTE_H

#include <board.h>
#include <stm32f4xx_hal_conf.h>
#include "s4396122_hal_util.h"
#include "s4396122_util_queue.h"
#include <stdlib.h>

int transmitionState;
int IRaddress;
int IRcommand;
int newIRMessage;

void s4396122_hal_irremote_init();
void s4396122_hal_irremote_interrupt();
int s4396122_hal_irremote_get_last_input();
int s4396122_hal_irremote_input_available();
void s4396122_hal_irremote_process();
char s4396122_hal_irremote_get_char();

#endif
