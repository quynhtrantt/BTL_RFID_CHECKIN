#include "driver_spi.h"
#include "board.h"

uint8_t Driver_SPI_TxRx(uint8_t data) {
    uint8_t rx_data = 0;
    HAL_SPI_TransmitReceive(&hspi1, &data, &rx_data, 1, 100);
    return rx_data;
}
