/*
 * driver_gpio.h
 *
 *  Created on: Oct 19, 2026
 *      Author: Quynh
 */

#ifndef M3_DRIVER_COMPONENT_DRIVER_GPIO_DRIVER_GPIO_H_
#define M3_DRIVER_COMPONENT_DRIVER_GPIO_DRIVER_GPIO_H_

#include <stdint.h>
#include <stdbool.h>

void Driver_GPIO_WritePin(void* port, uint16_t pin, bool state);
void Driver_Delay(uint32_t ms);
uint32_t Driver_GetTick(void);

#endif /* M3_DRIVER_COMPONENT_DRIVER_GPIO_DRIVER_GPIO_H_ */
