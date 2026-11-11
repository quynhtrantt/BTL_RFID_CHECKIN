
#include "app_system.h"
#include "app_main.h"
#include "bsp_keypad.h"
#include "bsp_button.h"
#include "app_command.h"

void app_main(void) {
    BSP_Button_Init();
    BSP_Keypad_Init();
    App_Init();
    (void)App_Command_Init();
    while (1)
    {
        App_Task();
        App_Command_Task();
    }
}
