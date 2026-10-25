#ifndef M3_DRIVER_COMPONENT_DRIVER_GPIO_DRIVER_GPIO_H_
#define M3_DRIVER_COMPONENT_DRIVER_GPIO_DRIVER_GPIO_H_

#include <stdint.h>
#include <stdbool.h>

void Driver_GPIO_Write(void *port, uint16_t pin, bool state);
bool Driver_GPIO_Read(void *port, uint16_t pin);

#endif /* M3_DRIVER_COMPONENT_DRIVER_GPIO_DRIVER_GPIO_H_ */
