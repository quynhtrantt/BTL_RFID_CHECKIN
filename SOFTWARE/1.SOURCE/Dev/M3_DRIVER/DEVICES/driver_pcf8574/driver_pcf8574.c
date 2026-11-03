#include "driver_pcf8574.h"
#include "driver_i2c.h"

static uint8_t pcf_addr;

bool Driver_PCF8574_Init(uint8_t addr)
{
    pcf_addr = addr;
    return Driver_PCF8574_WritePort(0x08U);
}

bool Driver_PCF8574_WritePort(uint8_t data)
{
    return Driver_I2C_Write(pcf_addr, &data, 1U);
}

