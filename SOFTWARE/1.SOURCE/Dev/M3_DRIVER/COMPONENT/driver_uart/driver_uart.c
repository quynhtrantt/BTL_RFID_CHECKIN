#include "driver_uart.h"
#include "board.h"
#include "fifo.h"

#define UART_RX_SIZE 256U
#define UART_TX_SIZE 1024U
#define UART_TX_CHUNK 64U

static FIFO_t rx_buffer, tx_buffer;
static uint8_t rx_storage[UART_RX_SIZE], tx_storage[UART_TX_SIZE];
static uint8_t rx_byte, tx_chunk[UART_TX_CHUNK];
static uint16_t tx_length;
static bool initialized;
static volatile bool rx_armed, rx_fault, rx_discard;
static volatile bool tx_busy, tx_complete;

static void start_receive(void) {
    rx_armed = HAL_UART_Receive_IT(&huart1, &rx_byte, 1) == HAL_OK;
    if (!rx_armed)
    {
        rx_fault = true;
        rx_discard = true;
    }
}

bool Driver_UART_Init(void) {
    if (initialized)
        return true;
    if (!FIFO_Init(&rx_buffer, rx_storage, sizeof(rx_storage)) ||
        !FIFO_Init(&tx_buffer, tx_storage, sizeof(tx_storage)))
        return false;
    rx_fault = false;
    rx_discard = false;
    tx_busy = false;
    tx_complete = false;
    tx_length = 0;
    initialized = true;
    start_receive();
    return rx_armed;
}

void Driver_UART_Task(void) {
    if (!initialized)
        return;

    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    if (!rx_armed)
        start_receive();
    if (tx_complete)
    {
        tx_complete = false;
        tx_length = 0;
    }
    if (tx_busy)
    {
        __set_PRIMASK(mask);
        return;
    }
    tx_busy = true;
    __set_PRIMASK(mask);

    if (!tx_length)
    {
        while (tx_length < sizeof(tx_chunk) &&
               FIFO_Pop(&tx_buffer, &tx_chunk[tx_length]))
            tx_length++;
    }
    if (!tx_length || HAL_UART_Transmit_IT(&huart1, tx_chunk, tx_length) != HAL_OK)
        tx_busy = false;
}

int Driver_UART_ReadByte(uint8_t *value) {
    if (!initialized || !value)
        return 0;
    uint32_t mask = __get_PRIMASK();
    __disable_irq();
    int result;
    if (rx_fault)
    {
        rx_fault = false;
        FIFO_Clear(&rx_buffer);
        result = -1;
    }
    else
        result = FIFO_Pop(&rx_buffer, value) ? 1 : 0;
    __set_PRIMASK(mask);
    return result;
}

uint16_t Driver_UART_TxSpace(void) {
    return initialized ? FIFO_Space(&tx_buffer) : 0;
}

bool Driver_UART_Transmit(const uint8_t *data, uint16_t len) {
    if (!initialized || (!data && len) || len > FIFO_Space(&tx_buffer))
        return false;
    for (uint16_t i = 0; i < len; i++)
        FIFO_Push(&tx_buffer, data[i]);
    Driver_UART_Task();
    return true;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *uart) {
    if (uart != &huart1 || !initialized)
        return;
    rx_armed = false;
    if (rx_discard)
    {
        if (rx_byte == '\r' || rx_byte == '\n')
            rx_discard = false;
    }
    else if (!FIFO_Push(&rx_buffer, rx_byte))
    {
        rx_fault = true;
        rx_discard = rx_byte != '\r' && rx_byte != '\n';
    }
    start_receive();
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *uart) {
    if (uart == &huart1 && initialized)
    {
        tx_complete = true;
        tx_busy = false;
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *uart) {
    if (uart != &huart1 || !initialized)
        return;
    rx_fault = true;
    rx_discard = true;
    HAL_UART_AbortReceive(uart);
    rx_armed = false;
    start_receive();
}
