/**
 * @file s4396122_hal_accel.h
 * @brief Library for accessing and controlling the accelerometer
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef HAL_ACCEL_H
#define HAL_ACCEL_H

#include "board.h"

#define MMA8452Q_ADDRESS (0x1D << 1)    //!< Address of the accelerometer

/**
 * @brief structure for containing the result of the pl register of the 
 * accelerometer
 */
struct PortLand {
    int portUp; //!< The value of the portrait bit
    int landUp; //!< The value of the landscape bit
};

void s4396122_hal_accel_init();
struct PortLand s4396122_hal_accel_get_pl();

#endif
