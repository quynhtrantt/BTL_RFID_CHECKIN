#ifndef M3_DRIVER_DEVICES_DRIVER_RFID_DRIVER_RFID_H_
#define M3_DRIVER_DEVICES_DRIVER_RFID_DRIVER_RFID_H_

#include <stdint.h>
#include <stdbool.h>

typedef void (*RFID_CS_Write_Fn)(bool state);

void Driver_RFID_AssignCS(RFID_CS_Write_Fn cs_fn);
void Driver_RFID_Init(void);
bool Driver_RFID_ReadUID(uint8_t *uid_buffer);

#endif /* M3_DRIVER_DEVICES_DRIVER_RFID_DRIVER_RFID_H_ */
