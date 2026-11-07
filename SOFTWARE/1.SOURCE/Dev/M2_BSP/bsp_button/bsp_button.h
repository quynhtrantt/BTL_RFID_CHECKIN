#ifndef BSP_BUTTON_H
#define BSP_BUTTON_H
#include <stdbool.h>
void BSP_Button_Init(void);
bool BSP_Button_IsPressed(void);
bool BSP_Button2_IsPressed(void);
#endif
