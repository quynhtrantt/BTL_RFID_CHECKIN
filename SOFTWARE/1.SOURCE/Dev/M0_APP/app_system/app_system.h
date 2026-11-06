#ifndef M0_APP_APP_SYSTEM_APP_SYSTEM_H_
#define M0_APP_APP_SYSTEM_APP_SYSTEM_H_

#include "driver_rfid.h"

void app_main(void);
void App_System_InitDB(void);
bool App_System_VerifyCard(RFID_UID_t *card);
int App_System_FindCard(RFID_UID_t *card);
bool App_System_AddCard(RFID_UID_t *card);
void App_System_DeleteCard(int index);

#endif
