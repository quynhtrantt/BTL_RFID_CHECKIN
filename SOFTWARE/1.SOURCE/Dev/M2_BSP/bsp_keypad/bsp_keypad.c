#include "bsp_keypad.h"
#include "board.h"
#include "driver_gpio.h"

static const char KEY_MAP[4][4] = {
    {'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'}
};
static const uint16_t ROW_PINS[4] = {KEYPAD_R0_PIN, KEYPAD_R1_PIN, KEYPAD_R2_PIN, KEYPAD_R3_PIN};
static const uint16_t COL_PINS[4] = {KEYPAD_C0_PIN, KEYPAD_C1_PIN, KEYPAD_C2_PIN, KEYPAD_C3_PIN};

char BSP_Keypad_Scan(void) {
    for (int r = 0; r < 4; r++) {
        Driver_GPIO_Write(KEYPAD_ROW_PORT, ROW_PINS[r], false);
        for (int c = 0; c < 4; c++) {
            if (Driver_GPIO_Read(KEYPAD_COL_PORT, COL_PINS[c]) == false) {
                Driver_GPIO_Write(KEYPAD_ROW_PORT, ROW_PINS[r], true);
                return KEY_MAP[r][c];
            }
        }
        Driver_GPIO_Write(KEYPAD_ROW_PORT, ROW_PINS[r], true);
    }
    return '\0';
}
