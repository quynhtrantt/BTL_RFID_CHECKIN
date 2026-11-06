#include "app_system.h"
#include "app_main.h"
#include "app_rfid.h"
#include "bsp_keypad.h"
#include "board.h"
#include <string.h>

typedef struct {
    RFID_UID_t card;
    bool valid;
} CardDB_t;

static CardDB_t database[MAX_CARDS];

void App_System_InitDB(void)
{
    /* Preserve the three DIP IDs from the previous app_system database. */
    static const uint8_t initial_ids[] = {0x01, 0x05, 0x10};
    memset(database, 0, sizeof(database));
    for (unsigned int i = 0; i < sizeof(initial_ids); ++i) {
        RFID_UID_t card = {{0x01, 0x02, 0x03, 0x04, initial_ids[i]}, 5};
        App_System_AddCard(&card);
    }
}

static bool Card_IsValid(const RFID_UID_t *card)
{
    return card != NULL && card->size > 0 &&
           card->size <= RFID_UID_MAX_SIZE;
}

int App_System_FindCard(RFID_UID_t *card)
{
    if (!Card_IsValid(card)) return -1;
    for (int i = 0; i < MAX_CARDS; ++i) {
        if (database[i].valid && database[i].card.size == card->size &&
            memcmp(database[i].card.uid, card->uid, card->size) == 0)
            return i;
    }
    return -1;
}

bool App_System_AddCard(RFID_UID_t *card)
{
    if (!Card_IsValid(card)) return false;
    if (App_System_FindCard(card) >= 0) return true;
    for (int i = 0; i < MAX_CARDS; ++i) {
        if (!database[i].valid) {
            database[i].card = *card;
            database[i].valid = true;
            return true;
        }
    }
    return false;
}

void App_System_DeleteCard(int index)
{
    if (index >= 0 && index < MAX_CARDS)
        database[index].valid = false;
}

bool App_System_VerifyCard(RFID_UID_t *card)
{
    return App_System_FindCard(card) >= 0;
}

/* Hardware setup stays outside the existing state machine. */
void app_main(void)
{
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitTypeDef gpio = {0};
    gpio.Pin = BUTTON_1_PIN;
    gpio.Mode = GPIO_MODE_INPUT;
    gpio.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(BUTTON_1_PORT, &gpio);
    BSP_Keypad_Init();

    App_Init();
    while (1) {
        App_Task();
    }
}
