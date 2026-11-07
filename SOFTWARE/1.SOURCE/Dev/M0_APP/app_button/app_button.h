#ifndef APP_BUTTON_H
#define APP_BUTTON_H
#include <stdbool.h>
#include <stdint.h>
void App_Button_Task(void);
bool App_Button_IsPressed(void);
bool App_Button2_IsPressed(void);
bool App_Button_IsHeld(uint32_t hold_time_ms);
#endif
