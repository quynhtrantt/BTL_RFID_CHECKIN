

#include "bsp_rfid.h"
#include "driver_rfid.h"
#include "app_lcd.h"
#include "app_rfid.h"
#include "app_system.h"
#include "board.h"
#include "stdbool.h"
#include "app_system.h"
#include "app_main.h"

void app_main(void) {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = BUTTON_1_PIN;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BUTTON_1_PORT, &gpio);
    BSP_Keypad_Init();
    App_Init();
    while (1)
    {
        App_Task();
    }
}
