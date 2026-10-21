#include "driver_gpio.h"
#include "stm32f1xx_hal.h"

void Driver_GPIO_Write(void *port, uint16_t pin, bool state) {
    HAL_GPIO_WritePin((GPIO_TypeDef*)port, pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool Driver_GPIO_Read(void *port, uint16_t pin) {
    return (HAL_GPIO_ReadPin((GPIO_TypeDef*)port, pin) == GPIO_PIN_SET);
}
