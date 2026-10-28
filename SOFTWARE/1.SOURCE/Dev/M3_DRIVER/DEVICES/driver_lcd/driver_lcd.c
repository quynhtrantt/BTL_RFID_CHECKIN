#include "driver_lcd.h"
#include "driver_i2c.h"
#include <stdlib.h>
#include "stm32f1xx_hal.h"

static uint8_t lcd_addr;

static void LCD_SendCmd(uint8_t cmd) {
    uint8_t data_u, data_l;
    uint8_t data_t[4];
    data_u = (cmd & 0xf0);
    data_l = ((cmd << 4) & 0xf0);
    data_t[0] = data_u | 0x0C;
    data_t[1] = data_u | 0x08;
    data_t[2] = data_l | 0x0C;
    data_t[3] = data_l | 0x08;
    Driver_I2C_Write(lcd_addr, data_t, 4);
}

static void LCD_SendData(uint8_t data) {
	 uint8_t data_u, data_l;
	 uint8_t data_t[4];

	    data_u = data & 0xF0;
	    data_l = (data << 4) & 0xF0;

	    data_t[0] = data_u | 0x0D;
	    data_t[1] = data_u | 0x09;
	    data_t[2] = data_l | 0x0D;
	    data_t[3] = data_l | 0x09;

	    Driver_I2C_Write(lcd_addr, data_t, 4);
}

void Driver_LCD_Init(uint8_t i2c_addr)
{
    lcd_addr = i2c_addr;

    HAL_Delay(50);

    LCD_SendCmd(0x33);
    HAL_Delay(5);

    LCD_SendCmd(0x32);
    HAL_Delay(5);

    LCD_SendCmd(0x28);
    LCD_SendCmd(0x0C);
    LCD_SendCmd(0x06);

    Driver_LCD_Clear();
}

void Driver_LCD_Clear(void)
{
	LCD_SendCmd(0x01);
	HAL_Delay(2);
}

void Driver_LCD_SetCursor(uint8_t col, uint8_t row) {
    uint8_t row_offsets[] = {0x00, 0x40};
    LCD_SendCmd(0x80 | (col + row_offsets[row]));
}

void Driver_LCD_Print(const char *str)
{
    if (str == NULL)
        return;

    for (int i = 0; i < 16 && str[i] != '\0'; i++)
    {
        LCD_SendData((uint8_t)str[i]);
    }
}
