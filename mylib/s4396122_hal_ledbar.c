/**
 * @file s4396122_hal_ledbar.c
 * @brief LED Light Bar peripheral driver
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_hal_ledbar.h"

/**
 * @brief Write an individual bit to the light bar
 * @param segment       The position bit to write to
 * @param segment_value Value to write (should be 1 or 0)
 */
void ledbar_seg_set(int segment, unsigned char segment_value) {

    unsigned char val = (segment_value > 0) ? 1 : 0;
    int seg_position = segment;
    if (segment < 8) {

        HAL_GPIO_WritePin(BRD_D5_GPIO_PORT, BRD_D5_PIN, 1);
        HAL_GPIO_WritePin(BRD_A3_GPIO_PORT, BRD_A3_PIN,
                (seg_position & 0x01) ? 1 : 0);
        HAL_GPIO_WritePin(BRD_A4_GPIO_PORT, BRD_A4_PIN,
                (seg_position & 0x02) ? 1 : 0);
        HAL_GPIO_WritePin(BRD_A5_GPIO_PORT, BRD_A5_PIN,
                (seg_position & 0x04) ? 1 : 0);
        HAL_GPIO_WritePin(BRD_D4_GPIO_PORT, BRD_D4_PIN, val);
        HAL_GPIO_WritePin(BRD_D5_GPIO_PORT, BRD_D5_PIN, 0);
    } else if (segment == 8) {

        HAL_GPIO_WritePin(BRD_D6_GPIO_PORT, BRD_D6_PIN, val);
    } else if (segment == 9) {

        HAL_GPIO_WritePin(BRD_D7_GPIO_PORT, BRD_D7_PIN, val);
    }
}

/**
 * @brief Initialize the IO pins for the LED bar
 */
void s4396122_hal_ledbar_init() {

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.Pin = BRD_A3_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_PULLDOWN;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    HAL_GPIO_Init(BRD_A3_GPIO_PORT, &GPIO_InitStructure);     // A0 for the 259
    __BRD_A3_GPIO_CLK();

    GPIO_InitStructure.Pin = BRD_A4_PIN;
    HAL_GPIO_Init(BRD_A4_GPIO_PORT, &GPIO_InitStructure);     // A1 for the 259
    __BRD_A4_GPIO_CLK();

    GPIO_InitStructure.Pin = BRD_A5_PIN;
    HAL_GPIO_Init(BRD_A5_GPIO_PORT, &GPIO_InitStructure);     // A2 for the 259
    __BRD_A5_GPIO_CLK();

    GPIO_InitStructure.Pin = BRD_D4_PIN;
    HAL_GPIO_Init(BRD_D4_GPIO_PORT, &GPIO_InitStructure);     // D for the 259
    __BRD_D4_GPIO_CLK();

    GPIO_InitStructure.Pin = BRD_D5_PIN;
    HAL_GPIO_Init(BRD_D5_GPIO_PORT, &GPIO_InitStructure);     // LE for the 259
    __BRD_D5_GPIO_CLK();
    // Sets to low so lights show up straight away
    HAL_GPIO_WritePin(BRD_D5_GPIO_PORT, BRD_D5_PIN, 1);

    GPIO_InitStructure.Pin = BRD_D6_PIN;
    HAL_GPIO_Init(BRD_D6_GPIO_PORT, &GPIO_InitStructure);     // LED 8
    __BRD_D6_GPIO_CLK();

    GPIO_InitStructure.Pin = BRD_D7_PIN;
    HAL_GPIO_Init(BRD_D7_GPIO_PORT, &GPIO_InitStructure);     // LED 9
    __BRD_D7_GPIO_CLK();
}

/**
 * @brief Writes the numeric value to the LED bar
 * @param value The value to be written to the light bar
 */
void s4396122_hal_ledbar_write(unsigned short value) {

    for (int i = 0; i < 10; i++) {
        ledbar_seg_set(i, (value >> i) & 1);
    }
}
