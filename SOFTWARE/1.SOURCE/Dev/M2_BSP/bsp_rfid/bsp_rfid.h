#ifndef M2_BSP_BSP_RFID_BSP_RFID_H_
#define M2_BSP_BSP_RFID_BSP_RFID_H_
#include <stdbool.h>
#include "driver_rfid.h"

void BSP_RFID_Init(void);
bool BSP_RFID_ReadCard(RFID_UID_t *out_card);
void BSP_RFID_ClearInterrupt(void);
void BSP_RFID_ArmInterrupt(void);


#endif /* M2_BSP_BSP_RFID_BSP_RFID_H_ */
