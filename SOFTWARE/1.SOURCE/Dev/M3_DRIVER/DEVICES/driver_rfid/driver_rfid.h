/*
 * driver_rfid.h
 *
 *  Created on: Oct 19, 2026
 *      Author: Quynh
 */

#ifndef M3_DRIVER_DEVICES_DRIVER_RFID_DRIVER_RFID_H_
#define M3_DRIVER_DEVICES_DRIVER_RFID_DRIVER_RFID_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct { uint8_t uid[5]; uint8_t size; } RFID_UID_t;

void Driver_RFID_Assign(void (*cs_fn)(bool), uint8_t (*spi_fn)(uint8_t));
void Driver_RFID_Init(void);
void Driver_RFID_ArmInterrupt(void);
void Driver_RFID_ClearInterrupt(void);
bool Driver_RFID_ReadCard(RFID_UID_t *uid);

#endif /* M3_DRIVER_DEVICES_DRIVER_RFID_DRIVER_RFID_H_ */
