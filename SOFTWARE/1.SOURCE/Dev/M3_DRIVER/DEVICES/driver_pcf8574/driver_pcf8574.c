#include "driver_pcf8574.h"
#include "driver_i2c.h"

static uint8_t pcf_addr;

bool Driver_PCF8574_Init(uint8_t addr) {
    uint8_t buf[2] = {0x08U, 0x08U};
    pcf_addr = addr;
    return Driver_I2C_Write(pcf_addr, buf, 2U);
}

bool Driver_PCF8574_WritePort(uint8_t data) {
    return Driver_I2C_Write(pcf_addr, &data, 1U);
}

bool Driver_PCF8574_WriteBuf(uint8_t *data, uint16_t len) {
    return Driver_I2C_Write(pcf_addr, data, len);
}
