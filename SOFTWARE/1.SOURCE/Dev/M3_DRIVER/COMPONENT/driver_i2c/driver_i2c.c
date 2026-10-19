#include "driver_i2c.h"
#include "stm32f1xx_hal.h"

void Driver_I2C_Write(void* hi2c, uint8_t dev_addr, uint8_t data)
{
    HAL_I2C_Master_Transmit((I2C_HandleTypeDef*)hi2c, dev_addr, &data, 1, 100);
}
