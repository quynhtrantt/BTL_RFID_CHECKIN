#ifndef M3_DRIVER_DEVICES_DRIVER_RFID_DRIVER_RFID_H_
#define M3_DRIVER_DEVICES_DRIVER_RFID_DRIVER_RFID_H_

#include <stdint.h>
#include <stdbool.h>

#define RFID_UID_MAX_SIZE 5U

typedef struct {
    uint8_t uid[RFID_UID_MAX_SIZE];
    uint8_t size;
} RFID_UID_t;

typedef void (*RFID_CS_Write_Fn)(bool state);

void Driver_RFID_AssignCS(RFID_CS_Write_Fn cs_fn);
void Driver_RFID_Init(void);
void Driver_RFID_ArmInterrupt(void);
void Driver_RFID_ClearInterrupt(void);
bool Driver_RFID_ReadCard(RFID_UID_t *uid);

#endif /* M3_DRIVER_DEVICES_DRIVER_RFID_DRIVER_RFID_H_ */
