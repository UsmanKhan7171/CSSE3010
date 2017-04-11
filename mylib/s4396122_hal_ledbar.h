/**
 * *****************************************************************************
 *  @file
 *  @author Daniel Fitzmaurice = 43961229
 *  @date   010317
 *  @brief  LED Light Bar peripheral driver
 *          REFERENCE: LEDLightBar_datasheet.pdf
 *******************************************************************************
 *  s4396122_hal_ledbar_init() = initialise LED Light Bar
 *  s4396122_hal_ledbar_write(unsigned short) = set LED Light Bar value
 */
#ifndef HAL_LEDBAR_H
#define HAL_LEDBAR_H

#include "board.h"
#include <stm32f4xx_hal_conf.h>

void s4396122_hal_ledbar_init();
void s4396122_hal_ledbar_write(unsigned short value);

#endif
