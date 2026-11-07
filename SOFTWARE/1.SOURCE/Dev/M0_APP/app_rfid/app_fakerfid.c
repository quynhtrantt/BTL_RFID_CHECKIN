#include "app_rfid.h"
#include "app_system.h"
#include "bsp_rfid.h"
#include <stddef.h>

static bool last_button_state;

void App_RFID_InitDB(void)
{
    last_button_state = false;
    App_System_InitDB();
}

bool App_RFID_HasNewCard(RFID_UID_t *out_card)
{
    RFID_UID_t card;
    bool pressed = BSP_RFID_ReadCard(&card);
    bool new_card = pressed && !last_button_state;
    last_button_state = pressed;
    if (new_card && out_card != NULL) {
        *out_card = card;
        return true;
    }
    return false;
}

/* Compatibility API used by the unchanged app_main state machine. */
int App_RFID_FindCard(RFID_UID_t *card)
{
    return App_System_FindCard(card);
}

bool App_RFID_AddCard(RFID_UID_t *card)
{
    return App_System_AddCard(card);
}

void App_RFID_DeleteCard(int index)
{
    App_System_DeleteCard(index);
}
