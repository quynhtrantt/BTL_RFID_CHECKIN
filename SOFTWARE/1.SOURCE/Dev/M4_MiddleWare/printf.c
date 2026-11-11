#include "uart_printf.h"
#include "driver_uart.h"
#include <errno.h>
#include <stdio.h>
#include <stdint.h>

void UART_Printf_Init(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);
}

int _write(int file, char *data, int length) {
    if (file != 1 && file != 2)
    {
        errno = EBADF;
        return -1;
    }
    if (length < 0 || (!data && length))
    {
        errno = EINVAL;
        return -1;
    }
    if (length > UINT16_MAX || !Driver_UART_Transmit((const uint8_t *)data, (uint16_t)length))
    {
        errno = EAGAIN;
        return -1;
    }
    return length;
}

int __io_putchar(int ch) {
    uint8_t value = (uint8_t)ch;
    return Driver_UART_Transmit(&value, 1) ? value : EOF;
}
