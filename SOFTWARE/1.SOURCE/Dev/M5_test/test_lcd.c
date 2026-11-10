

#include "app_lcd.h"
#include "bsp_lcd.h"
#include "bsp_led.h"
#include "test_lcd.h"
#include <stdbool.h>
#include "stm32f1xx_hal.h"

void app_main(void) {

    for (int i = 0; i < 5; i++)
    {
        BSP_LED_Red_Set(1);
        HAL_Delay(200);
        BSP_LED_Red_Set(0);
        HAL_Delay(200);
    }
    BSP_LCD_Init();

    while (1)
    {
    }
}
