/**
 * @file s4396122_hal_accel.c
 * @brief Library for accessing and controlling the accelerometer
 * @author Daniel Fitzmaurice - 43961229
 * @version 1
 * @date 2017-05-31
 */
#include "s4396122_hal_accel.h"

I2C_HandleTypeDef I2CHandle; //!< Handler for the I2C interface

/**
 * @brief Reads a particular register from the I2C device
 *
 * @param addr Register address to read from
 *
 * @return value read from the register
 */
int readRegister(int addr) {

    __HAL_I2C_CLEAR_FLAG(&I2CHandle, I2C_FLAG_AF);

    // Write the address to the i2c device
    I2CHandle.Instance->CR1 |= I2C_CR1_START;
    while (__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_SB) == RESET);
    I2CHandle.Instance->DR = __HAL_I2C_7BIT_ADD_WRITE(MMA8452Q_ADDRESS);
    while (__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_ADDR) == RESET);
    __HAL_I2C_CLEAR_ADDRFLAG(&I2CHandle);
    I2CHandle.Instance->DR = addr;
    while ((__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_TXE) == RESET) && (__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_BTF) == RESET));

    // Read the result from the i2c device
    I2CHandle.Instance->CR1 |= I2C_CR1_START;
    while (__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_SB) == RESET);
    I2CHandle.Instance->DR = __HAL_I2C_7BIT_ADD_READ(MMA8452Q_ADDRESS);
    while (__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_ADDR) == RESET);
    __HAL_I2C_CLEAR_ADDRFLAG(&I2CHandle);
    while (__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_RXNE) == RESET);

    // Get the value and stop the i2c channel
    uint8_t readVal = I2CHandle.Instance->DR;
    I2CHandle.Instance->CR1 &= ~I2C_CR1_ACK;
    I2CHandle.Instance->CR1 |= I2C_CR1_STOP;
    return readVal;
}

/**
 * @brief Inits the accelerometer and any required hardware
 */
void s4396122_hal_accel_init() {

    // Initialize the i2c pins
    GPIO_InitTypeDef GPIO_InitStructure;
    __BRD_SCL_GPIO_CLK(); // Enable the clocks
    __BRD_SDA_GPIO_CLK();
    __BRD_I2C_CLK();
    GPIO_InitStructure.Pin = BRD_SCL_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Alternate = BRD_SCL_AF;
    HAL_GPIO_Init(BRD_SCL_GPIO_PORT, &GPIO_InitStructure);
    GPIO_InitStructure.Pin = BRD_SDA_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Alternate = BRD_SDA_AF;
    HAL_GPIO_Init(BRD_SDA_GPIO_PORT, &GPIO_InitStructure);

    // Initialize the I2CHandle
    I2CHandle.Instance = BRD_I2C;
    I2CHandle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    I2CHandle.Init.ClockSpeed = 1000000;
    I2CHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2CHandle.Init.DutyCycle = I2C_DUTYCYCLE_2;
    I2CHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2CHandle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    I2CHandle.Init.OwnAddress1 = 0;
    I2CHandle.Init.OwnAddress2 = 0;
    HAL_I2C_Init(&I2CHandle);
    // Wait for the i2c peripheral to be ready
    while (HAL_I2C_GetState(&I2CHandle) != HAL_I2C_STATE_READY);

    // Set the accelerometer into deactive move, able to edit registers
    uint8_t status[2] = {0x2A, 0};
    status[1] = readRegister(status[0]);
    HAL_I2C_Master_Transmit(&I2CHandle, MMA8452Q_ADDRESS, status, 2, 100);

    // Set the pl register to be enabled
    uint8_t plReg[2] = {0x11, 0xC0};
    HAL_I2C_Master_Transmit(&I2CHandle, MMA8452Q_ADDRESS, plReg, 2, 100);

    // Enable the accelerometer
    status[1] |= 1;
    HAL_I2C_Master_Transmit(&I2CHandle, MMA8452Q_ADDRESS, status, 2, 100);
}

/**
 * @brief Returns the result if the system is portrait or landscape
 *
 * @return Struct containing the result if the system is landscape or portrait
 */
struct PortLand s4396122_hal_accel_get_pl() {

    struct PortLand result;
    uint8_t readVal = readRegister(0x10); // Read the register value
    readVal &= 0x06; // Only get the bits we care about
    readVal >>= 1; // Easier to map out the result
    result.portUp = !!(readVal & 0x01);
    result.landUp = !!(readVal & 0x02);
    return result;
}
