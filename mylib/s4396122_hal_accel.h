#ifndef HAL_ACCEL_H
#define HAL_ACCEL_H

#include <board.h>

#define MMA8452Q_ADDRESS 0x1D << 1

static I2C_HandleTypeDef I2CHandle;

void s4396122_hal_accel_init();
int s4396122_hal_accel_read_addr(int addr);

#endif
