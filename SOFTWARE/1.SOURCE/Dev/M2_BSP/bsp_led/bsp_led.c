#include "bsp_led.h"
#include "board.h"
#include "driver_gpio.h"

void BSP_LED_Red_Set(bool state)
{
	Driver_GPIO_Write(LED_RED_PORT, LED_RED_PIN, state);
}
void BSP_LED_Green_Set(bool state)
{
	Driver_GPIO_Write(LED_GREEN_PORT, LED_GREEN_PIN, state);
}
