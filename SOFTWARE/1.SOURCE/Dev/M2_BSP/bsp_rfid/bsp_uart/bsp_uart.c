#include "bsp_uart.h"
#include "driver_uart.h"
#include <string.h>

void BSP_UART_SendString(const char *str) {
    Driver_UART_Transmit((uint8_t*)str, strlen(str));
}
