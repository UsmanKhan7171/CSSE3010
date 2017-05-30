/**
 * @file s4396122_hal_sysmon.h
 * @brief Library for interface between 3 LA channels and the np2
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#ifndef HAL_SYSMON_H
#define HAL_SYSMON_H

#include <board.h>
#include <stm32f4xx_hal_conf.h>

// LA Pin Defines
#define LA_CHANNEL_0_PORT BRD_D9_GPIO_PORT  //!< LA Channel 0 Port definition
#define LA_CHANNEL_0_PIN BRD_D9_PIN         //!< LA Channel 0 Pin definition
#define LA_CHANNEL_1_PORT BRD_D10_GPIO_PORT //!< LA Channel 1 Port definition
#define LA_CHANNEL_1_PIN BRD_D10_PIN        //!< LA Channel 1 Pin definition
#define LA_CHANNEL_2_PORT BRD_D12_GPIO_PORT //!< LA Channel 2 Port definition
#define LA_CHANNEL_2_PIN BRD_D12_PIN        //!< LA Channel 2 Pin definition

// External Functions
extern void s4396122_hal_sysmon_init();
//! Sets channel 0 low
#define s4396122_hal_sysmon_chan0_clr() HAL_GPIO_WritePin(LA_CHANNEL_0_PORT, LA_CHANNEL_0_PIN, 0)
//! Sets channel 0 high
#define s4396122_hal_sysmon_chan0_set() HAL_GPIO_WritePin(LA_CHANNEL_0_PORT, LA_CHANNEL_0_PIN, 1)
//! Sets channel 1 low
#define s4396122_hal_sysmon_chan1_clr() HAL_GPIO_WritePin(LA_CHANNEL_1_PORT, LA_CHANNEL_1_PIN, 0)
//! Sets channel 1 high
#define s4396122_hal_sysmon_chan1_set() HAL_GPIO_WritePin(LA_CHANNEL_1_PORT, LA_CHANNEL_1_PIN, 1)
//! Sets channel 2 low
#define s4396122_hal_sysmon_chan2_clr() HAL_GPIO_WritePin(LA_CHANNEL_2_PORT, LA_CHANNEL_2_PIN, 0)
//! Sets channel 2 high
#define s4396122_hal_sysmon_chan2_set() HAL_GPIO_WritePin(LA_CHANNEL_2_PORT, LA_CHANNEL_2_PIN, 1)

#endif
