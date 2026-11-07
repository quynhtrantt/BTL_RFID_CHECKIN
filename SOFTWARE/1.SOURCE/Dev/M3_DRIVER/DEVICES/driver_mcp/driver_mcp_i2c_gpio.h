#ifndef DRIVER_MCP_I2C_GPIO_H
#define DRIVER_MCP_I2C_GPIO_H
#include <stdint.h>
#include <stdbool.h>
bool Driver_MCP_GPIO_Read(uint8_t device, uint8_t reg, uint8_t *data, uint16_t len);
bool Driver_MCP_GPIO_Write(uint8_t device, uint8_t reg, const uint8_t *data, uint16_t len);
bool Driver_MCP_GPIO_IsReady(uint8_t device);
const char *Driver_MCP_GPIO_Error(void);
#endif
