#include "driver_uart.h"
#include "board.h"

bool Driver_UART_Transmit(uint8_t *data, uint16_t len) {
    return (HAL_UART_Transmit(&huart1, data, len, 100) == HAL_OK);
}
