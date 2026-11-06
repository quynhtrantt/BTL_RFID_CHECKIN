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



