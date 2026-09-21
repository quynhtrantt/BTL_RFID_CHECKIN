#ifndef __LCD_I2C_H
#define __LCD_I2C_H

#include "main.h"
#include "stm32f1xx_hal.h"
#include <stdint.h>

#define LCD_I2C_ADDR (0x27 << 1)   /* đổi thành (0x3F << 1) nếu module của bạn dùng 0x3F */


HAL_StatusTypeDef LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Print(const char *str);
void LCD_PrintLine(uint8_t row, const char *str);
void LCD_Backlight(uint8_t on);

#endif
