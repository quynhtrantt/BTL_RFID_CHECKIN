/*
 * test_rfid.c
 *
 *  Created on: Nov 4, 2026
 *      Author: Quynh
 */
#include "bsp_rfid.h"
#include "driver_rfid.h"
#include "app_lcd.h"
#include "app_rfid.h"
#include "app_system.h"
#include "board.h"
#include "stdbool.h"
#include "app_system.h"
#include "app_main.h"

/*
void app_main(void)
{
    App_LCD_Init();
    BSP_RFID_Init();

    UI_ShowIdle();

    RFID_UID_t card;

    while (1)
    {
        if (App_RFID_HasNewCard(&card) == true)
        {
            UI_ShowChecking(&card);
            bool is_Valid = App_System_VerifyCard(&card);
            UI_ShowResult(is_Valid);

        }
    }
}
*/
void app_main(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = BUTTON_1_PIN;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BUTTON_1_PORT, &gpio);
    BSP_Keypad_Init();
    App_Init();
    while (1) {
        App_Task();
    }
}



