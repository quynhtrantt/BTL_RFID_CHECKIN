#ifndef M0_APP_APP_RFID_APP_RFID_H_
#define M0_APP_APP_RFID_APP_RFID_H_

#include <stdbool.h>
#include "driver_rfid.h"

#define MAX_CARDS 8

void App_RFID_InitDB(void);
bool App_RFID_HasNewCard(RFID_UID_t *out_card);
int App_RFID_FindCard(RFID_UID_t *card);
bool App_RFID_AddCard(RFID_UID_t *card);
void App_RFID_DeleteCard(int index);

#endif /* M0_APP_APP_RFID_APP_RFID_H_ */
