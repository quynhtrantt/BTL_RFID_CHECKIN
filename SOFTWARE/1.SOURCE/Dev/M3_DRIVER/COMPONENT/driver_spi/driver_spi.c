#include "driver_spi.h"
#include "stm32f1xx_hal.h"

uint8_t Driver_SPI_TxRx(void* hspi, uint8_t data) {
    uint8_t rx = 0;
    HAL_SPI_TransmitReceive((SPI_HandleTypeDef*)hspi, &data, &rx, 1, 100);
    return rx;
}
