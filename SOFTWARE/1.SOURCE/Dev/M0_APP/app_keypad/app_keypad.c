#include "app_keypad.h"
#include "bsp_keypad.h"
#include "stm32f1xx_hal.h"
char App_Keypad_GetKey(void) {
    static char raw, stable;
    static uint32_t changed;
    char key=BSP_Keypad_Scan();
    uint32_t now=HAL_GetTick();
    if(key!=raw) { raw=key; changed=now; }
    if((uint32_t)(now-changed)>=30 && stable!=raw) {
        stable=raw;
        return stable;
    }
    return '\0';
}
