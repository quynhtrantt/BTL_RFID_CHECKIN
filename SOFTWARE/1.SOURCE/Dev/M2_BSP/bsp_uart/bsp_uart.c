#include "printf.h"
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

int BSP_UART_Printf(const char *format, ...)
{
    if (format == NULL)
    {
        return -1;
    }

    char buffer[128];
    va_list arguments;
    va_start(arguments, format);
    int length = vsnprintf_(buffer, sizeof(buffer), format, arguments);
    va_end(arguments);

    if (length < 0 || (size_t)length >= sizeof(buffer))
    {
        return -1;
    }

    if (!Driver_UART_Transmit((const uint8_t *)buffer, (uint16_t)length))
    {
        return -1;
    }

    return length;
}

void _putchar(char character)
{
    uint8_t value = (uint8_t)character;
    Driver_UART_Transmit(&value, 1U);
}

int __io_putchar(int character)
{
    uint8_t value = (uint8_t)character;
    return Driver_UART_Transmit(&value, 1U) ? value : -1;
}
