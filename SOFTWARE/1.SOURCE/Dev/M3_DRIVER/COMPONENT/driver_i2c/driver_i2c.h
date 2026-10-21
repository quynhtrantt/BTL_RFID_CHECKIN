#ifndef M3_DRIVER_COMPONENT_DRIVER_I2C_DRIVER_I2C_H_
#define M3_DRIVER_COMPONENT_DRIVER_I2C_DRIVER_I2C_H_

#include <stdint.h>
void Driver_I2C_Write(void* hi2c, uint8_t dev_addr, uint8_t data);

#endif /* M3_DRIVER_COMPONENT_DRIVER_I2C_DRIVER_I2C_H_ */
