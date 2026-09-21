#include "i2c_forLCD.h"
#include "main.h"
#include <string.h>

extern I2C_HandleTypeDef hi2c1;

#define LCD_BACKLIGHT 0x08
#define LCD_ENABLE    0x04
#define LCD_RS        0x01
#define LCD_RW        0x02

static uint8_t backlight_state = LCD_BACKLIGHT;

static HAL_StatusTypeDef LCD_ExpanderWrite(uint8_t data)
{
    uint8_t v = (uint8_t)(data | backlight_state);
    return HAL_I2C_Master_Transmit(&hi2c1, LCD_I2C_ADDR, &v, 1, 100);
}

static HAL_StatusTypeDef LCD_PulseEnable(uint8_t data)
{
    HAL_StatusTypeDef st;
    st = LCD_ExpanderWrite((uint8_t)(data | LCD_ENABLE));
    if (st != HAL_OK) return st;
    st = LCD_ExpanderWrite((uint8_t)(data & (uint8_t)~LCD_ENABLE));
    return st;
}

static HAL_StatusTypeDef LCD_Write4Bits(uint8_t nibble, uint8_t rs)
{
    uint8_t data = (uint8_t)((nibble & 0xF0) | (rs ? LCD_RS : 0));
    return LCD_PulseEnable(data);
}

static HAL_StatusTypeDef LCD_Send(uint8_t value, uint8_t rs)
{
    HAL_StatusTypeDef st;
    st = LCD_Write4Bits((uint8_t)(value & 0xF0), rs);
    if (st != HAL_OK) return st;
    st = LCD_Write4Bits((uint8_t)((value << 4) & 0xF0), rs);
    return st;
}

HAL_StatusTypeDef LCD_Init(void)
{
    HAL_StatusTypeDef st;

    HAL_Delay(50);
    st = LCD_ExpanderWrite(0x00);
    if (st != HAL_OK) return st;

    /* 4-bit initialization sequence */
    LCD_Write4Bits(0x30, 0);
    HAL_Delay(5);
    LCD_Write4Bits(0x30, 0);
    HAL_Delay(1);
    LCD_Write4Bits(0x30, 0);
    HAL_Delay(1);
    LCD_Write4Bits(0x20, 0);
    HAL_Delay(1);

    LCD_Send(0x28, 0);  /* 4-bit, 2-line, 5x8 */
    LCD_Send(0x08, 0);  /* display off */
    LCD_Clear();
    LCD_Send(0x06, 0);  /* entry mode */
    LCD_Send(0x0C, 0);  /* display on, cursor off */

    return HAL_OK;
}

void LCD_Clear(void)
{
    LCD_Send(0x01, 0);
    HAL_Delay(2);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0) ? 0x00 : 0x40;
    if (col > 15) col = 15;
    LCD_Send((uint8_t)(0x80 | (addr + col)), 0);
}

void LCD_Print(const char *str)
{
    if (!str) return;
    while (*str) {
        LCD_Send((uint8_t)*str++, 1);
    }
}

void LCD_PrintLine(uint8_t row, const char *str)
{
    char buf[17];
    memset(buf, ' ', sizeof(buf));
    buf[16] = '\0';
    if (str) {
        size_t n = strlen(str);
        if (n > 16) n = 16;
        memcpy(buf, str, n);
    }
    LCD_SetCursor(row, 0);
    LCD_Print(buf);
}

void LCD_Backlight(uint8_t on)
{
    backlight_state = on ? LCD_BACKLIGHT : 0x00;
    (void)LCD_ExpanderWrite(0x00);
}
