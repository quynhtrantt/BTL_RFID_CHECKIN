#include "driver_i2c.h"
#include "board.h"

bool Driver_I2C_Write(uint8_t dev_addr, uint8_t *data, uint16_t len)
{
    return (HAL_I2C_Master_Transmit(&hi2c1, dev_addr, data, len, 100) == HAL_OK);
}

bool Driver_I2C_Mem_Write(uint8_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t len)
{
    return (HAL_I2C_Mem_Write(&hi2c1, dev_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data, len, 100) == HAL_OK);
}

bool Driver_I2C_Mem_Read(uint8_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t len)
{
    return (HAL_I2C_Mem_Read(&hi2c1, dev_addr, reg_addr, I2C_MEMADD_SIZE_8BIT, data, len, 100) == HAL_OK);
}
