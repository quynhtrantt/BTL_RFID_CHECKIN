#include "driver_eeprom24c04.h"
#include "driver_i2c.h"

bool Driver_EEPROM_WritePage(uint8_t dev_addr, uint16_t mem_addr, uint8_t *data, uint16_t len) {
    return Driver_I2C_Mem_Write(dev_addr, mem_addr, data, len);
}

bool Driver_EEPROM_Read(uint8_t dev_addr, uint16_t mem_addr, uint8_t *data, uint16_t len) {
    return Driver_I2C_Mem_Read(dev_addr, mem_addr, data, len);
}

