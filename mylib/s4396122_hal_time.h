/**
 * @file   s4396122_hal_time.h
 * @author Daniel Fitzmaurice = 43961229
 * @date   120417
 * @brief  Library for time related functions
 */
#ifndef HAL_TIME_H
#define HAL_TIME_H

#include <board.h>
#include <stm32f4xx_hal_conf.h>

void s4396122_hal_time_init();
unsigned int s4396122_hal_time_get_micro();
void s4396122_hal_time_clear_micro();

#endif
