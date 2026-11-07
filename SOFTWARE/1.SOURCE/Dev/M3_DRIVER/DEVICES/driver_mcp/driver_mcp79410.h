#ifndef DRIVER_MCP79410_H
#define DRIVER_MCP79410_H
#include <stdbool.h>
#include <stdint.h>
bool Driver_MCP_PrepareEEPROM(void);
const char *Driver_MCP_ErrorText(void);
bool Driver_MCP_ReadRTC(uint8_t reg, uint8_t *data, uint16_t len);
bool Driver_MCP_WriteRTC(uint8_t reg, const uint8_t *data, uint16_t len);
bool Driver_MCP_ReadEEPROM(uint8_t addr, uint8_t *data, uint16_t len);
bool Driver_MCP_WriteEEPROM(uint8_t addr, const uint8_t *data, uint16_t len);
#endif
