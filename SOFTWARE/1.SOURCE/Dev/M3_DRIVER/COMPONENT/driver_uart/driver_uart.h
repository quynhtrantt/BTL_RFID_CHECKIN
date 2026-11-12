#ifndef M3_DRIVER_COMPONENT_DRIVER_UART_DRIVER_UART_H_
#define M3_DRIVER_COMPONENT_DRIVER_UART_DRIVER_UART_H_

#include <stdint.h>
#include <stdbool.h>

bool Driver_UART_Init(void);
void Driver_UART_Task(void);
int Driver_UART_ReadByte(uint8_t *value);
uint16_t Driver_UART_TxSpace(void);
bool Driver_UART_Transmit(const uint8_t *data, uint16_t len);

#endif
