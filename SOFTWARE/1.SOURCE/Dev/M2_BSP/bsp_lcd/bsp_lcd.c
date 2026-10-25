#ifndef M2_BSP_BSP_LCD_BSP_LCD_C_
#define M2_BSP_BSP_LCD_BSP_LCD_C_

#include "bsp_lcd.h"
#include "driver_lcd.h"

#define LCD_I2C_ADDRESS (0x27 << 1)

void BSP_LCD_Init(void) { Driver_LCD_Init(LCD_I2C_ADDRESS); }

#endif /* M2_BSP_BSP_LCD_BSP_LCD_C_ */
