#ifndef HAL_LEDBAR_H
#define HAL_LEDBAR_H

#include "board.h"
#include <stm32f4xx_hal_conf.h>

void s4396122_hal_ledbar_init();
void s4396122_hal_ledbar_write(unsigned short value);

#endif
