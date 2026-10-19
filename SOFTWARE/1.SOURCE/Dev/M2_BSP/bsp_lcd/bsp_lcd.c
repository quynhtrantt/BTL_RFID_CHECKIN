#ifndef M2_BSP_BSP_LCD_BSP_LCD_C_
#define M2_BSP_BSP_LCD_BSP_LCD_C_

#include "bsp_lcd.h"
#include "stm32f1xx_hal.h"
#include "driver_i2c.h"
#include "driver_gpio.h"
#include "driver_lcd.h"

#define BOARD_LCD_I2C_ADDR (0x27 << 1)
extern I2C_HandleTypeDef hi2c1;

static void BSP_LCD_I2C_Write(uint8_t addr, uint8_t data) {
    Driver_I2C_Write(&hi2c1, addr, data);
}

void BSP_LCD_Setup(void) {
    Driver_LCD_Assign(BOARD_LCD_I2C_ADDR, BSP_LCD_I2C_Write, Driver_Delay);
    Driver_LCD_Init();
}

#endif /* M2_BSP_BSP_LCD_BSP_LCD_C_ */
