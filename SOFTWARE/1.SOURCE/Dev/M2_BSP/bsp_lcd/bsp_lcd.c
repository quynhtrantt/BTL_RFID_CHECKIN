#include "bsp_lcd.h"
#include "driver_lcd.h"

#define MODULE_I2C_ADDR 0x27

void BSP_LCD_Init(void) {
    Driver_LCD_Init(MODULE_I2C_ADDR);
}

void BSP_LCD_Clear(void) {
    Driver_LCD_Clear();
}

void BSP_LCD_SetCursor(uint8_t col, uint8_t row) {
    Driver_LCD_SetCursor(col, row);
}

void BSP_LCD_Print(const char *str) {
    Driver_LCD_Print(str);
}
