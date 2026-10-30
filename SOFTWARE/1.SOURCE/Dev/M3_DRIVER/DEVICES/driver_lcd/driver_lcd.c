
#include "driver_lcd.h"
#include "driver_pcf8574.h"
#include "stm32f1xx_hal.h"
#include <stddef.h>

#define LCD_RS  0x01U
#define LCD_EN  0x04U
#define LCD_BL  0x08U

static uint8_t lcd_addr;

/* Ghi 1 nibble 4-bit ra PCF8574 */
static void LCD_Write4Bits(uint8_t nibble, uint8_t isData)
{
    uint8_t value = (uint8_t)((nibble & 0x0FU) << 4);

    value |= LCD_BL;

    /* RS = 1: ghi dữ liệu; RS = 0: ghi lệnh */
    if (isData != 0U) {
        value |= LCD_RS;
    }

    /* EN lên mức cao rồi xuống thấp */
    Driver_PCF8574_WritePort(value | LCD_EN);
    Driver_PCF8574_WritePort(value);
}

/* Gửi lệnh LCD */
static void LCD_SendCmd(uint8_t cmd)
{
    LCD_Write4Bits((uint8_t)(cmd >> 4), 0U);
    LCD_Write4Bits((uint8_t)(cmd & 0x0FU), 0U);

    if (cmd == 0x01U || cmd == 0x02U) {
        HAL_Delay(2);
    }
}

/* Gửi dữ liệu/ký tự LCD */
static void LCD_SendData(uint8_t data)
{
    LCD_Write4Bits((uint8_t)(data >> 4), 1U);
    LCD_Write4Bits((uint8_t)(data & 0x0FU), 1U);
}

void Driver_LCD_Init(uint8_t i2c_addr)
{
    lcd_addr = i2c_addr;

    /* Khởi tạo PCF8574 trước khi điều khiển LCD */
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

    /* Chuyển sang chế độ 4-bit */
    LCD_Write4Bits(0x02U, 0U);
    HAL_Delay(1);

    LCD_SendCmd(0x28U);
    LCD_SendCmd(0x08U);
    Driver_LCD_Clear();
    LCD_SendCmd(0x06U);  /* Tăng địa chỉ DDRAM */
    LCD_SendCmd(0x0CU);  /* Bật hiển thị, tắt cursor */
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
