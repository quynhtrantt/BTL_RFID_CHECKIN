
#include "driver_lcd.h"
#include "driver_pcf8574.h"
#include "stm32f1xx_hal.h"
#include <stddef.h>

#define LCD_RS 0x01U
#define LCD_EN 0x04U
#define LCD_BL 0x08U

static uint8_t lcd_addr;

static void LCD_Nibble(uint8_t nibble) {
    uint8_t v = (uint8_t)(((nibble & 0x0FU) << 4) | LCD_BL);
    uint8_t buf[2] = {(uint8_t)(v | LCD_EN), v};
    Driver_PCF8574_WriteBuf(buf, 2U);
}

static void LCD_SendByte(uint8_t byte, uint8_t isData) {
    uint8_t rs = (isData != 0U) ? LCD_RS : 0U;
    uint8_t hi = (uint8_t)((byte & 0xF0U) | LCD_BL | rs);
    uint8_t lo = (uint8_t)(((byte << 4) & 0xF0U) | LCD_BL | rs);
    uint8_t buf[4] = {(uint8_t)(hi | LCD_EN), hi, (uint8_t)(lo | LCD_EN), lo};
    Driver_PCF8574_WriteBuf(buf, 4U);
    HAL_Delay(1);
}

static void LCD_SendCmd(uint8_t cmd) {
    LCD_SendByte(cmd, 0U);
    if (cmd == 0x01U || cmd == 0x02U)
    {
        HAL_Delay(5);
    }
}

static void LCD_SendData(uint8_t data) {
    LCD_SendByte(data, 1U);
}

void Driver_LCD_Init(uint8_t i2c_addr) {
    lcd_addr = i2c_addr;

    HAL_Delay(50);
    Driver_PCF8574_Init(lcd_addr);
    HAL_Delay(50);

    LCD_Nibble(0x03U);
    HAL_Delay(10);
    LCD_Nibble(0x03U);
    HAL_Delay(10);
    LCD_Nibble(0x03U);
    HAL_Delay(10);
    LCD_Nibble(0x02U);
    HAL_Delay(10);

    LCD_SendCmd(0x28U);
    LCD_SendCmd(0x08U);
    Driver_LCD_Clear();
    LCD_SendCmd(0x06U);
    LCD_SendCmd(0x0CU);
}

void Driver_LCD_Clear(void) {
    LCD_SendCmd(0x01U);
}

void Driver_LCD_SetCursor(uint8_t col, uint8_t row) {
    if (col > 15U)
        col = 15U;
    if (row > 1U)
        row = 1U;

    uint8_t row_offsets[] = {0x00U, 0x40U};

    LCD_SendCmd((uint8_t)(0x80U | (col + row_offsets[row])));
}

void Driver_LCD_Print(const char *str) {
    if (str == NULL)
    {
        return;
    }

    for (uint8_t i = 0U; i < 16U && str[i] != '\0'; i++)
    {
        LCD_SendData((uint8_t)str[i]);
    }
}
