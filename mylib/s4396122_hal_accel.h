#ifndef HAL_ACCEL_H
#define HAL_ACCEL_H

#include "board.h"

#define MMA8452Q_ADDRESS (0x1D << 1)

struct PortLand {
    int portUp;
    int landUp;
};

void s4396122_hal_accel_init();
struct PortLand s4396122_hal_accel_get_pl();

#endif
