#include "app_main.h"
#include "app_lcd.h"
#include "app_rfid.h"

void app_main(void) {
    App_LCD_Init();
    App_RFID_Init();
}

void app_run(void) {
    App_RFID_Task();
}
