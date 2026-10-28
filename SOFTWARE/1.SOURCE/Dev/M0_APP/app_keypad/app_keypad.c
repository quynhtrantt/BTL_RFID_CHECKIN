#include "app_keypad.h"
#include "bsp_keypad.h"
#include "stm32f1xx_hal.h"

char App_Keypad_GetKey(void) {
    static uint32_t last_press_time = 0;
    static char last_key = '\0';
    char current_key = BSP_Keypad_Scan();

    if (current_key != '\0' && current_key != last_key) {
        if (HAL_GetTick() - last_press_time > 50) {
            last_key = current_key;
            last_press_time = HAL_GetTick();
            return current_key;
        }
    } else if (current_key == '\0') {
        last_key = '\0';
    }
    return '\0'; //=255
}
