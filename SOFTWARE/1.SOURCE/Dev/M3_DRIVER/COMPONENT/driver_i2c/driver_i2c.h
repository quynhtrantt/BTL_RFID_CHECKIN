#ifndef M3_DRIVER_COMPONENT_DRIVER_I2C_DRIVER_I2C_H_
#define M3_DRIVER_COMPONENT_DRIVER_I2C_DRIVER_I2C_H_

#include <stdint.h>
#include <stdbool.h>

bool Driver_I2C_Write(uint8_t dev_addr, uint8_t *data, uint16_t len);
bool Driver_I2C_Mem_Write(uint8_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t len);
bool Driver_I2C_Mem_Read(uint8_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t len);

#endif /* M3_DRIVER_COMPONENT_DRIVER_I2C_DRIVER_I2C_H_ */
