#ifndef HAL_SYSMON_H
#define HAL_SYSMON_H

#include <board.h>
#include <stm32f4xx_hal_conf.h>

#define LA_CHANNEL_0_PORT BRD_D9_GPIO_PORT
#define LA_CHANNEL_0_PIN BRD_D9_PIN
#define LA_CHANNEL_1_PORT BRD_D10_GPIO_PORT
#define LA_CHANNEL_1_PIN BRD_D10_PIN
#define LA_CHANNEL_2_PORT BRD_D12_GPIO_PORT
#define LA_CHANNEL_2_PIN BRD_D12_PIN

extern void s4396122_hal_sysmon_init();

#define s4396122_hal_sysmon_chan0_clr() HAL_GPIO_WritePin(LA_CHANNEL_0_PORT, LA_CHANNEL_0_PIN, 0)
#define s4396122_hal_sysmon_chan0_set() HAL_GPIO_WritePin(LA_CHANNEL_0_PORT, LA_CHANNEL_0_PIN, 1)

#define s4396122_hal_sysmon_chan1_clr() HAL_GPIO_WritePin(LA_CHANNEL_1_PORT, LA_CHANNEL_1_PIN, 0)
#define s4396122_hal_sysmon_chan1_set() HAL_GPIO_WritePin(LA_CHANNEL_1_PORT, LA_CHANNEL_1_PIN, 1)

#define s4396122_hal_sysmon_chan2_clr() HAL_GPIO_WritePin(LA_CHANNEL_2_PORT, LA_CHANNEL_2_PIN, 0)
#define s4396122_hal_sysmon_chan2_set() HAL_GPIO_WritePin(LA_CHANNEL_2_PORT, LA_CHANNEL_2_PIN, 1)

#endif
