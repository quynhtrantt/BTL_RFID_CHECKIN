/* Entry point for DIP-switch RFID + MCP79410 attendance demo. */
#include "app_system.h"
#include "app_main.h"
#include "bsp_keypad.h"
#include "bsp_button.h"
void app_main(void) {
    BSP_Button_Init();
    BSP_Keypad_Init();
    App_Init();
    while(1) App_Task();
}
