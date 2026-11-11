#include "bsp_uart.h"
#include "driver_uart.h"
#include <string.h>

bool BSP_UART_Init(void) {
    return Driver_UART_Init();
}

void BSP_UART_Task(void) {
    Driver_UART_Task();
}

int BSP_UART_ReadByte(uint8_t *value) {
    return Driver_UART_ReadByte(value);
}

uint16_t BSP_UART_TxSpace(void) {
    return Driver_UART_TxSpace();
}

void BSP_UART_SendString(const char *str) {
    if (str)
        Driver_UART_Transmit((const uint8_t *)str, (uint16_t)strlen(str));
}
