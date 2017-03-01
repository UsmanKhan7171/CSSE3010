#include "s4396122_hal_ledbar.h"

void ledbar_seg_set(int segment, unsigned char segment_value) {

}

void s4396122_hal_ledbar_init() {
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Pin = BRD_A3_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(BRD_A3_GPIO_PORT, &GPIO_InitStructure);       // A0 for the 259

    GPIO_InitStructure.Pin = BRD_A4_PIN;
    HAL_GPIO_Init(BRD_A4_GPIO_PORT, &GPIO_InitStructure);       // A1 for the 259

    GPIO_InitStructure.Pin = BRD_A5_PIN;
    HAL_GPIO_Init(BRD_A5_GPIO_PORT, &GPIO_InitStructure);       // A2 for the 259

    GPIO_InitStructure.Pin = BRD_D4_PIN;
    HAL_GPIO_Init(BRD_D4_GPIO_PORT, &GPIO_InitStructure);       // D for the 259
    HAL_GPIO_WritePin(BRD_D4_GPIO_PORT, BRD_D4_PIN, 1);         // Sets this to high

    GPIO_InitStructure.Pin = BRD_D5_PIN;
    HAL_GPIO_Init(BRD_D5_GPIO_PORT, &GPIO_InitStructure);       // LE for the 259

    GPIO_InitStructure.Pin = BRD_D6_PIN;
    HAL_GPIO_Init(BRD_D6_GPIO_PORT, &GPIO_InitStructure);       // LED 8

    GPIO_InitStructure.Pin = BRD_D7_PIN;
    HAL_GPIO_Init(BRD_D7_GPIO_PORT, &GPIO_InitStructure);       // LED 9
}

void s4396122_hal_ledbar_write(unsigned short value) {

}
