#include "bsp_keypad.h"
#include "board.h"
#include "driver_gpio.h"

static const char KEY_MAP[4][3] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}
};
static const uint16_t ROW_PINS[4] = {KEYPAD_R0_PIN, KEYPAD_R1_PIN, KEYPAD_R2_PIN, KEYPAD_R3_PIN};
static const uint16_t COL_PINS[3] = {KEYPAD_C0_PIN, KEYPAD_C1_PIN, KEYPAD_C2_PIN};

void BSP_Keypad_Init(void) {
    for (int i = 0; i < 4; i++) {
        Driver_GPIO_Write(KEYPAD_ROW_PORT, ROW_PINS[i], true);
    }
}

char BSP_Keypad_Scan(void) {
    for (int r = 0; r < 4; r++) {
        Driver_GPIO_Write(KEYPAD_ROW_PORT, ROW_PINS[r], false);
        for (int c = 0; c < 3; c++) {
            if (Driver_GPIO_Read(KEYPAD_COL_PORT, COL_PINS[c]) == false) {
                Driver_GPIO_Write(KEYPAD_ROW_PORT, ROW_PINS[r], true);
                return KEY_MAP[r][c];
            }
        }
        Driver_GPIO_Write(KEYPAD_ROW_PORT, ROW_PINS[r], true);
    }
    return '\0';
}
