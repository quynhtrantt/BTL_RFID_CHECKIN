#include "driver_lcd.h"
#include <stddef.h>

static void (*I2C_Write)(uint8_t, uint8_t) = NULL;
static void (*Delay)(uint32_t) = NULL;
static uint8_t lcd_addr = 0;

void Driver_LCD_Assign(uint8_t addr, void (*write_fn)(uint8_t, uint8_t), void (*delay_fn)(uint32_t)) {
    lcd_addr = addr; I2C_Write = write_fn; Delay = delay_fn;
}

static void LCD_Send4Bit(uint8_t data, uint8_t rs) {
    uint8_t hi = (data & 0xF0) | rs | 0x08;
    uint8_t lo = ((data << 4) & 0xF0) | rs | 0x08;
    I2C_Write(lcd_addr, hi | 0x04); I2C_Write(lcd_addr, hi & ~0x04);
    I2C_Write(lcd_addr, lo | 0x04); I2C_Write(lcd_addr, lo & ~0x04);
}

void Driver_LCD_Init(void) {
    LCD_Send4Bit(0x33, 0); LCD_Send4Bit(0x32, 0);
    LCD_Send4Bit(0x28, 0); LCD_Send4Bit(0x0C, 0);
    LCD_Send4Bit(0x06, 0); Driver_LCD_Clear();
}
void Driver_LCD_Clear(void) { LCD_Send4Bit(0x01, 0); Delay(2); }
void Driver_LCD_SetCursor(uint8_t col, uint8_t row) {
    uint8_t offsets[] = {0x00, 0x40};
    LCD_Send4Bit(0x80 | (col + offsets[row & 1]), 0);
}
void Driver_LCD_Print(const char *str) {
    while (*str) LCD_Send4Bit(*str++, 0x01);
}
