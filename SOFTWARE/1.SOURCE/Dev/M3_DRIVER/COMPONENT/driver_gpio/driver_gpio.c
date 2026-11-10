#include "driver_gpio.h"
#include "stm32f1xx_hal.h"

void Driver_GPIO_InitInputPullUp(void *port, uint16_t pin) {
    if (port == GPIOA)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
    }
    else if (port == GPIOB)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
    }
    else if (port == GPIOC)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
    }
    else
    {
        return;
    }

    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = pin;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init((GPIO_TypeDef *)port, &gpio);
}

void Driver_GPIO_Write(void *port, uint16_t pin, bool state) {
    HAL_GPIO_WritePin((GPIO_TypeDef *)port, pin, state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

bool Driver_GPIO_Read(void *port, uint16_t pin) {
    return (HAL_GPIO_ReadPin((GPIO_TypeDef *)port, pin) == GPIO_PIN_SET);
}
