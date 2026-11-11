#ifndef M2_BSP_BSP_UART_BSP_UART_H_
#define M2_BSP_BSP_UART_BSP_UART_H_

#include <stdbool.h>
#include <stdint.h>

bool BSP_UART_Init(void);
void BSP_UART_Task(void);
int BSP_UART_ReadByte(uint8_t *value);
uint16_t BSP_UART_TxSpace(void);
void BSP_UART_SendString(const char *str);

#endif
