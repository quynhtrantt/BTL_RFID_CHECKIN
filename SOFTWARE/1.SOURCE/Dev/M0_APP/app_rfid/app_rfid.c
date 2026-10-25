#include "app_rfid.h"
#include "bsp_rfid.h"
#include "bsp_buzzer.h"
#include <string.h>

extern volatile bool flag_RFID_Scanned;

typedef struct {
    uint8_t uid[5];
    uint8_t valid;
} CardDB_t;

static CardDB_t database[MAX_CARDS];

void App_RFID_InitDB(void) {
    for (int i = 0; i < MAX_CARDS; i++) database[i].valid = 0;
}

bool App_RFID_HasNewCard(RFID_UID_t *out_card)
{
    if (flag_RFID_Scanned)
    {
        bool status = BSP_RFID_ReadCard(out_card);

        BSP_RFID_ClearInterrupt();
        BSP_RFID_ArmInterrupt();
        flag_RFID_Scanned = false;

        if (status)
        {
            BSP_Signal_SuccessBeep();
            return true;
        }
    }
    return false;
}

int App_RFID_FindCard(RFID_UID_t *card) {
    for (int i = 0; i < MAX_CARDS; i++) {
        if (database[i].valid == 1 && memcmp(database[i].uid, card->uid, card->size) == 0) return i;
    }
    return -1;
}

bool App_RFID_AddCard(RFID_UID_t *card) {
    for (int i = 0; i < MAX_CARDS; i++) {
        if (database[i].valid == 0) {
            memcpy(database[i].uid, card->uid, card->size);
            database[i].valid = 1;
            return true;
        }
    }
    return false;
}

void App_RFID_DeleteCard(int index) {
    if (index >= 0 && index < MAX_CARDS) database[index].valid = 0;
}
