#include "bsp_button.h"
#include "board.h"
#include "driver_gpio.h"
void BSP_Button_Init(void) {
    Driver_GPIO_InitInputPullUp(BUTTON_1_PORT,BUTTON_1_PIN);
    Driver_GPIO_InitInputPullUp(BUTTON_2_PORT,BUTTON_2_PIN);
}
bool BSP_Button_IsPressed(void) { return !Driver_GPIO_Read(BUTTON_1_PORT,BUTTON_1_PIN); }
bool BSP_Button2_IsPressed(void) { return !Driver_GPIO_Read(BUTTON_2_PORT,BUTTON_2_PIN); }
