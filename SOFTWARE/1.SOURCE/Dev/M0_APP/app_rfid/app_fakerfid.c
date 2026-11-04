/*
 * app_fakerfid.c
 *
 *  Created on: Nov 4, 2026
 *      Author: Quynh
 */
#include "app_rfid.h"
#include "bsp_rfid.h"

void App_RFID_InitDB(void) {}

bool App_RFID_HasNewCard(RFID_UID_t *out_card)
{
    static bool last_button_state = false;

    RFID_UID_t temp_card;
    bool current_button = BSP_RFID_ReadCard(&temp_card);

    if (current_button == true && last_button_state == false)
        {
            last_button_state = true;
            *out_card = temp_card;
            return true;
        }
    if (current_button == false)
        {
            last_button_state = false;
        }
    return false;
    }

