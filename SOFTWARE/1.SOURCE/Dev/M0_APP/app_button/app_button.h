#ifndef M0_APP_APP_BUTTON_APP_BUTTON_H_
#define M0_APP_APP_BUTTON_APP_BUTTON_H_

#include <stdbool.h>
#include <stdint.h>

void App_Button_Task(void);
bool App_Button_IsPressed(void);
bool App_Button_IsHeld(uint32_t hold_time_ms);


#endif /* M0_APP_APP_BUTTON_APP_BUTTON_H_ */
