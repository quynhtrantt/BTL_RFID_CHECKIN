/*
 * driver_spi.h
 *
 *  Created on: Oct 19, 2026
 *      Author: Quynh
 */

#ifndef M3_DRIVER_COMPONENT_DRIVER_SPI_DRIVER_SPI_H_
#define M3_DRIVER_COMPONENT_DRIVER_SPI_DRIVER_SPI_H_

#include <stdint.h>
uint8_t Driver_SPI_TxRx(void* hspi, uint8_t data);

#endif /* M3_DRIVER_COMPONENT_DRIVER_SPI_DRIVER_SPI_H_ */
