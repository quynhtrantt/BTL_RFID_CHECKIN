/*
 * driver_gpio.c
 *
 *  Created on: Oct 19, 2026
 *      Author: Quynh
 */

#ifndef M3_DRIVER_COMPONENT_DRIVER_GPIO_DRIVER_GPIO_C_
#define M3_DRIVER_COMPONENT_DRIVER_GPIO_DRIVER_GPIO_C_

#include "driver_gpio.h"
#include "stm32f1xx_hal.h"

void Driver_GPIO_WritePin(void* port, uint16_t pin, bool state) {
    HAL_GPIO_WritePin((GPIO_TypeDef*)port, pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}
void Driver_Delay(uint32_t ms) { HAL_Delay(ms); }
uint32_t Driver_GetTick(void)  { return HAL_GetTick(); }

#endif /* M3_DRIVER_COMPONENT_DRIVER_GPIO_DRIVER_GPIO_C_ */
