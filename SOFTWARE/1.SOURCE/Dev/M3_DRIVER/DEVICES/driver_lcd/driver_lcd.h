#ifndef M3_DRIVER_DEVICES_DRIVER_LCD_DRIVER_LCD_H_
#define M3_DRIVER_DEVICES_DRIVER_LCD_DRIVER_LCD_H_
#include <stdint.h>

void Driver_LCD_Init(uint8_t i2c_addr);
void Driver_LCD_Clear(void);
void Driver_LCD_SetCursor(uint8_t col, uint8_t row);
void Driver_LCD_Print(const char *str);


#endif /* M3_DRIVER_DEVICES_DRIVER_LCD_DRIVER_LCD_H_ */
