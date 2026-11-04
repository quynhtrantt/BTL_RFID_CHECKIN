/*
 * driver_pcf8574.h
 *
 *  Created on: Oct 30, 2026
 *      Author: Quynh
 */

#ifndef DEVICES_DRIVER_PCF8574_DRIVER_PCF8574_H_
#define DEVICES_DRIVER_PCF8574_DRIVER_PCF8574_H_

#include <stdint.h>
#include <stdbool.h>

bool Driver_PCF8574_Init(uint8_t addr);
bool Driver_PCF8574_WritePort(uint8_t data);

bool Driver_PCF8574_WriteBuf(uint8_t *data, uint16_t len);

#endif /* DEVICES_DRIVER_PCF8574_DRIVER_PCF8574_H_ */
