
#include "driver_lcd.h"
#include "driver_pcf8574.h"
#include "stm32f1xx_hal.h"
#include <stddef.h>

#define LCD_RS  0x01U
#define LCD_EN  0x04U
#define LCD_BL  0x08U

static uint8_t lcd_addr;

static void LCD_Write4Bits(uint8_t nibble, uint8_t isData)
{
    // Cờ RS (1 là ghi chữ, 0 là ghi lệnh)
    uint8_t rs = (isData != 0U) ? LCD_RS : 0U;

    // Đóng gói data: Dữ liệu + RS + Đèn nền
    uint8_t out = (uint8_t)(((nibble & 0x0FU) << 4) | rs | LCD_BL);

    // BẮT BUỘC PHẢI CÓ 3 NHỊP NÀY ĐỂ PROTEUS KỊP ĐỌC
    // Nhịp 1: Gửi data chuẩn bị (EN = 0)
    Driver_PCF8574_WritePort(out);
    HAL_Delay(1);

    // Nhịp 2: Chớp EN lên cao (EN = 1)
    Driver_PCF8574_WritePort(out | LCD_EN);
    HAL_Delay(1);

    // Nhịp 3: Chốt EN xuống thấp (EN = 0)
    Driver_PCF8574_WritePort(out);
    HAL_Delay(1);
}

/*
static void LCD_Write4Bits(uint8_t nibble, uint8_t isData) //ghi data ra pcf
{
    uint8_t value = (uint8_t)((nibble & 0x0FU) << 4);

    value |= LCD_BL;
    if (isData != 0U) { //rs=1 ghi data
        value |= LCD_RS;
    }

    Driver_PCF8574_WritePort(value | LCD_EN);
    Driver_PCF8574_WritePort(value);
}
*/
static void LCD_SendCmd(uint8_t cmd) //send command lcd
{
    LCD_Write4Bits((uint8_t)(cmd >> 4), 0U);
    LCD_Write4Bits((uint8_t)(cmd & 0x0FU), 0U);

    if (cmd == 0x01U || cmd == 0x02U) {
        HAL_Delay(2);
    }
}


static void LCD_SendData(uint8_t data)
{
    LCD_Write4Bits((uint8_t)(data >> 4), 1U);
    LCD_Write4Bits((uint8_t)(data & 0x0FU), 1U);
}

void Driver_LCD_Init(uint8_t i2c_addr)
{
    lcd_addr = i2c_addr;

    if (!Driver_PCF8574_Init(lcd_addr)) {
        return;
    }

    HAL_Delay(50);

    /* Khởi tạo LCD theo datasheet HD44780 */
    LCD_Write4Bits(0x03U, 0U);
    HAL_Delay(5);

    LCD_Write4Bits(0x03U, 0U);
    HAL_Delay(5);

    LCD_Write4Bits(0x03U, 0U);
    HAL_Delay(1);

    LCD_Write4Bits(0x02U, 0U);
    HAL_Delay(1);

    LCD_SendCmd(0x28U);
    LCD_SendCmd(0x08U);
    Driver_LCD_Clear();
    LCD_SendCmd(0x06U);
    LCD_SendCmd(0x0CU);
}

void Driver_LCD_Clear(void)
{
    LCD_SendCmd(0x01U);

}

void Driver_LCD_SetCursor(uint8_t col, uint8_t row)
{
    if (col > 15U) col = 15U;
    if (row > 1U) row = 1U;

    uint8_t row_offsets[] = {0x00U, 0x40U};

    LCD_SendCmd((uint8_t)(0x80U | (col + row_offsets[row])));
}

void Driver_LCD_Print(const char *str)
{
    if (str == NULL) {
        return;
    }

    for (uint8_t i = 0U; i < 16U && str[i] != '\0'; i++) {
        LCD_SendData((uint8_t)str[i]);
    }
}
