/*
 * test_lcd.c
 *
 *  Created on: Oct 28, 2026
 *      Author: Quynh
 */
#include "app_lcd.h"
#include "bsp_lcd.h"
#include "bsp_led.h"
#include <stdbool.h>

void app_main(void)
{
	BSP_LED_Red_Set(1);
	//BSP_LCD_Init();
	//UI_ShowIdle ();
	BSP_LCD_Init();

	BSP_LCD_SetCursor(0, 0);
	BSP_LCD_Print("HELLO WORLD");

	BSP_LCD_SetCursor(0, 1);
	BSP_LCD_Print("LCD TEST");
	while (1)
	{

	}
}
