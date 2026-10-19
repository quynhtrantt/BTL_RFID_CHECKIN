#ifndef M3_DRIVER_DEVICES_DRIVER_LCD_DRIVER_LCD_H_
#define M3_DRIVER_DEVICES_DRIVER_LCD_DRIVER_LCD_H_
#include <stdint.h>

void Driver_LCD_Assign(uint8_t addr, void (*write_fn)(uint8_t, uint8_t), void (*delay_fn)(uint32_t));
void Driver_LCD_Init(void);
void Driver_LCD_Clear(void);
void Driver_LCD_SetCursor(uint8_t col, uint8_t row);
void Driver_LCD_Print(const char *str);

#endif /* M3_DRIVER_DEVICES_DRIVER_LCD_DRIVER_LCD_H_ */
