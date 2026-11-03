/*
 * test_pcf.c
 *
 *  Created on: Nov 3, 2026
 *      Author: Quynh
 */

#include "driver_pcf8574.h"
#include "driver_lcd.h"
#include "test_lcd.h"
#include "bsp_lcd.h"
#include "bsp_led.h"
#include "app_lcd.h"
#include <stdbool.h>
#include "stm32f1xx_hal.h"

void app_main(void)
{
	App_LCD_Init();

	//Driver_PCF8574_Init(MODULE_I2C_ADDR);
	//Driver_PCF8574_WritePort(3);
	//BSP_LCD_Init();
	UI_ShowIdle ();

	while (1)
	{

	}
}
