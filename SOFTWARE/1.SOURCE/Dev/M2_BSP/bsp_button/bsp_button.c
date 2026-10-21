#include "bsp_button.h"
#include "board.h"
#include "driver_gpio.h"

bool BSP_Button_IsPressed(void) {
    return (Driver_GPIO_Read(BUTTON_1_PORT, BUTTON_1_PIN) == false);
}
