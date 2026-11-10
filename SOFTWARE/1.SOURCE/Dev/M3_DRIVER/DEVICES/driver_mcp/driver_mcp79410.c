#include "driver_mcp79410.h"
#include "driver_mcp_i2c_gpio.h"
#include "stm32f1xx_hal.h"
#include <string.h>
#include <stdio.h>
#define RTC_ADDR 0x6FU
#define EEPROM_ADDR 0x57U

#define EEPROM_WRITE_GUARD_MS 6U
static char error[17] = "OK";

static bool fail(const char *stage, unsigned addr) {
    snprintf(error, sizeof(error), "%s @%02X", stage, addr & 0xFF);
    return false;
}

const char *Driver_MCP_ErrorText(void) {
    return error;
}

static bool wait_eeprom(uint8_t addr) {

    HAL_Delay(EEPROM_WRITE_GUARD_MS);
    uint32_t start = HAL_GetTick();
    while (!Driver_MCP_GPIO_IsReady(EEPROM_ADDR))
    {
        if ((uint32_t)(HAL_GetTick() - start) >= 15U)
            return fail("EE BUSY", addr);
    }
    return true;
}

bool Driver_MCP_PrepareEEPROM(void) {
    uint8_t status;
    if (!Driver_MCP_GPIO_Read(EEPROM_ADDR, 0xFF, &status, 1))
        return fail("EE STATUS RD", 0xFF);

    if (status & 0x0C)
    {
        uint8_t value = status & ~0x0CU;
        if (!Driver_MCP_GPIO_Write(EEPROM_ADDR, 0xFF, &value, 1))
            return fail("EE STATUS WR", 0xFF);
        if (!wait_eeprom(0xFF))
            return false;
        if (!Driver_MCP_GPIO_Read(EEPROM_ADDR, 0xFF, &status, 1))
            return fail("EE STATUS RD", 0xFF);
        if (status & 0x0C)
            return fail("EE PROTECTED", 0xFF);
    }
    strcpy(error, "OK");
    return true;
}

bool Driver_MCP_ReadRTC(uint8_t reg, uint8_t *data, uint16_t len) {
    return data && len && (uint16_t)reg + len <= 0x60 &&
           Driver_MCP_GPIO_Read(RTC_ADDR, reg, data, len);
}

bool Driver_MCP_WriteRTC(uint8_t reg, const uint8_t *data, uint16_t len) {
    return data && len && (uint16_t)reg + len <= 0x60 &&
           Driver_MCP_GPIO_Write(RTC_ADDR, reg, (uint8_t *)data, len);
}

bool Driver_MCP_ReadEEPROM(uint8_t addr, uint8_t *data, uint16_t len) {
    if (!data || !len || (uint16_t)addr + len > 128)
        return fail("EE RANGE", addr);
    if (!Driver_MCP_GPIO_Read(EEPROM_ADDR, addr, data, len))
    {
        snprintf(error, sizeof(error), "RD %s @%02X", Driver_MCP_GPIO_Error(), addr);
        return false;
    }
    strcpy(error, "OK");
    return true;
}

bool Driver_MCP_WriteEEPROM(uint8_t addr, const uint8_t *data, uint16_t len) {
    if (!data || !len || (uint16_t)addr + len > 128)
        return fail("EE RANGE", addr);
    while (len)
    {
        uint16_t n = 8U - (addr & 7U);
        if (n > len)
            n = len;
        if (!Driver_MCP_GPIO_Write(EEPROM_ADDR, addr, (uint8_t *)data, n))
            return fail("EE WR NACK", addr);
        if (!wait_eeprom(addr))
            return false;
        uint8_t verify[8];
        if (!Driver_MCP_ReadEEPROM(addr, verify, n))
            return false;
        for (unsigned i = 0; i < n; i++)
            if (verify[i] != data[i])
                return fail("EE VERIFY", addr + i);
        addr += n;
        data += n;
        len -= n;
    }
    strcpy(error, "OK");
    return true;
}
