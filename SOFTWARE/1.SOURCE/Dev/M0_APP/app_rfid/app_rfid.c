#include "app_rfid.h"
#include "app_lcd.h"
#include "bsp_rfid.h"
#include "driver_rfid.h"
#include "driver_gpio.h"

volatile bool flag_RFID_Scanned = false;
static uint32_t last_scan_time = 0;
static bool is_displaying_result = false;

const uint8_t VALID_CARD[4] = {0x11, 0x22, 0x33, 0x44};

void App_RFID_Init(void) {
    BSP_RFID_Setup();
}

void App_RFID_Task(void) {
    if (is_displaying_result && (Driver_GetTick() - last_scan_time > 1500)) {
        is_displaying_result = false;
        App_LCD_Init();
    }

    if (flag_RFID_Scanned == true) {
        flag_RFID_Scanned = false;

        if (Driver_GetTick() - last_scan_time < 1500) {
            Driver_RFID_ClearInterrupt();
            Driver_RFID_ArmInterrupt();
            return;
        }

        RFID_UID_t myCard;
        if (Driver_RFID_ReadCard(&myCard)) {
            last_scan_time = Driver_GetTick();
            is_displaying_result = true;

            bool isOk = true;
            for(int i = 0; i < 4; i++) {
                if(myCard.uid[i] != VALID_CARD[i]) isOk = false;
            }

            App_LCD_ShowUID(myCard.uid, myCard.size);
            App_LCD_ShowStatus(isOk);
        }

        Driver_RFID_ClearInterrupt();
        Driver_RFID_ArmInterrupt();
    }
}

