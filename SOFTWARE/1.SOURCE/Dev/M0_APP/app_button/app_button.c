#include "app_button.h"
#include "bsp_button.h"
#include "stm32f1xx_hal.h"

static bool stable_state = false;
static uint32_t press_start_time = 0;
static bool is_holding = false;

void App_Button_Task(void) {
    static uint32_t last_time = 0;
    static bool last_state = false;
    bool current_state = BSP_Button_IsPressed();

    if (current_state != last_state) {
        last_time = HAL_GetTick();
        last_state = current_state;
    }

    if ((HAL_GetTick() - last_time) > 50)
    {
        if (stable_state != last_state)
        {
            stable_state = last_state;
            if (stable_state)
            {
                press_start_time = HAL_GetTick();
                is_holding = true;
            }
            else {
                is_holding = false;
            }
        }
    }
}

bool App_Button_IsPressed(void) {
    static bool prev_state = false;
    bool pressed = (stable_state && !prev_state);
    prev_state = stable_state;
    return pressed;
}

bool App_Button_IsHeld(uint32_t hold_time_ms) {
    if (is_holding && (HAL_GetTick() - press_start_time >= hold_time_ms)) {
        is_holding = false; // Chỉ trả về true 1 lần
        return true;
    }
    return false;
}
