#ifndef M2_BSP_BSP_LCD_BSP_LCD_H_
#define M2_BSP_BSP_LCD_BSP_LCD_H_
#include <stdint.h>

void BSP_LCD_Init(void);
void BSP_LCD_Clear(void);
void BSP_LCD_SetCursor(uint8_t col, uint8_t row);
void BSP_LCD_Print(const char *str);

#endif
