#include "bsp_led.h"
#include "board.h"
#include "driver_gpio.h"

void BSP_LED_Red_Set(bool state) {
#if BOARD_RED_LED_AVAILABLE
    Driver_GPIO_Write(LED_RED_PORT, LED_RED_PIN, state);
#else
    (void)state;
#endif
}

void BSP_LED_Green_Set(bool state) {
#if BOARD_GREEN_LED_AVAILABLE
    Driver_GPIO_Write(LED_GREEN_PORT, LED_GREEN_PIN, state);
#else
    (void)state;
#endif
}
