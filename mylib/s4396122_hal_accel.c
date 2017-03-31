#include "s4396122_hal_accel.h"

void s4396122_hal_accel_init() {
    GPIO_InitTypeDef GPIO_InitStructure;

    __BRD_SCL_GPIO_CLK();
    __BRD_SDA_GPIO_CLK();
    __BRD_I2C_CLK();

    GPIO_InitStructure.Pin = BRD_SCL_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    GPIO_InitStructure.Alternate = BRD_SCL_AF;
    HAL_GPIO_Init(BRD_SCL_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.Pin = BRD_SDA_PIN;
    GPIO_InitStructure.Alternate = BRD_SDA_AF;
    HAL_GPIO_Init(BRD_SDA_GPIO_PORT, &GPIO_InitStructure);

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

    while (HAL_I2C_GetState(&I2CHandle) != HAL_I2C_STATE_READY);
}

int s4396122_hal_accel_read_addr(int addr) {
    __HAL_I2C_CLEAR_FLAG(&I2CHandle, I2C_FLAG_AF);

    I2CHandle.Instance->CR1 |= I2C_CR1_START;
    while (__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_SB) == RESET);

    I2CHandle.Instance->DR = __HAL_I2C_7BIT_ADD_WRITE(MMA8452Q_ADDRESS);
    while (__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_ADDR) == RESET);
    __HAL_I2C_CLEAR_ADDRFLAG(&I2CHandle);

    I2CHandle.Instance->DR = addr;
    while ((__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_TXE) == RESET) && (__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_BTF) == RESET));

    I2CHandle.Instance->CR1 |= I2C_CR1_START;
    while (__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_SB) == RESET);

    I2CHandle.Instance->DR = __HAL_I2C_7BIT_ADD_READ(MMA8452Q_ADDRESS);
    while (__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_ADDR) == RESET);
    __HAL_I2C_CLEAR_ADDRFLAG(&I2CHandle);

    while (__HAL_I2C_GET_FLAG(&I2CHandle, I2C_FLAG_RXNE) == RESET);
    uint8_t readValue = I2CHandle.Instance->DR;

    I2CHandle.Instance->CR1 &= ~I2C_CR1_ACK;
    I2CHandle.Instance->CR1 |= I2C_CR1_STOP;

    return readValue;
}
