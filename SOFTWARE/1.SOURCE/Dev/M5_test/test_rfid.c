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
#include "board.h"

void app_main(void)
{
    // Khởi tạo
    App_LCD_Init();
    BSP_RFID_Init(); // STM32 sẽ tự động gọi hàm Init của file Fake!

    // Giao diện chờ
    UI_ShowIdle();

    RFID_UID_t card;

    while (1)
    {
        // FSM hoàn toàn mù mờ, không hề biết thẻ này là đồ giả
        if (App_RFID_HasNewCard(&card) == true)
        {
            UI_ShowChecking(&card);
            UI_ShowIdle();
        }
    }
}



