#ifndef M3_DRIVER_COMPONENT_DRIVER_UART_DRIVER_UART_H_
#define M3_DRIVER_COMPONENT_DRIVER_UART_DRIVER_UART_H_

#include <stdint.h>
#include <stdbool.h>

bool Driver_UART_Transmit(uint8_t *data, uint16_t len);

#endif /* M3_DRIVER_COMPONENT_DRIVER_UART_DRIVER_UART_H_ */
