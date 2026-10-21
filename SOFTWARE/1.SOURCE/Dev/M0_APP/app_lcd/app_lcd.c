#ifndef M0_APP_APP_LCD_APP_LCD_C_
#define M0_APP_APP_LCD_APP_LCD_C_

#include <bsp_lcd/bsp_lcd.h>
#include "app_lcd.h"
#include "driver_lcd.h"
#include <stdio.h>

void App_LCD_Init(void) {
    BSP_LCD_Setup();
    Driver_LCD_Clear();
    Driver_LCD_SetCursor(0, 0);
    Driver_LCD_Print("Cho quet the");
}

void App_LCD_ShowUID(uint8_t *uid, uint8_t size) {
    char buf[17] = {0};
    sprintf(buf, "ID: %02X%02X%02X%02X     ", uid[0], uid[1], uid[2], uid[3]);
    Driver_LCD_SetCursor(0, 0);
    Driver_LCD_Print(buf);
}

void App_LCD_ShowStatus(bool isValid) {
    Driver_LCD_SetCursor(0, 1);
    if (isValid) Driver_LCD_Print("-> THE HOP LE   ");
    else         Driver_LCD_Print("-> TU CHOI !    ");
}

#endif /* M0_APP_APP_LCD_APP_LCD_C_ */
