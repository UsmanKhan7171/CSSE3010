/**
 * @file s4396122_hal_ledbar.h
 * @brief LED Light Bar peripheral drive
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef HAL_LEDBAR_H
#define HAL_LEDBAR_H

#include "board.h"
#include <stm32f4xx_hal_conf.h>

void s4396122_hal_ledbar_init();
void s4396122_hal_ledbar_write(unsigned short value);

#endif
