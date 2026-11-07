#include "app_main.h"
#include "app_rfid.h"
#include "app_lcd.h"
#include "app_keypad.h"
#include "app_button.h"
#include "bsp_rfid.h"
#include "bsp_lcd.h"
#include "stm32f1xx_hal.h"
#include <string.h>

typedef enum {
    STATE_IDLE, STATE_CHECKING, STATE_UNREGISTERED,
    STATE_SPECIAL_MENU, STATE_ENTER_PASS, STATE_MANAGE,
    STATE_REGISTER_PASS
} AppState_t;

static AppState_t current_state = STATE_IDLE;
static uint32_t state_timer = 0;
static RFID_UID_t current_card;
static int current_card_index = -1;

static char input_pass[10] = {0};
static uint8_t pass_idx = 0;
static const char master_pass[] = "123456";

static void State_Switch(AppState_t next_state) {
    current_state = next_state;
    state_timer = HAL_GetTick();
}
static bool Timeout_Check(uint32_t ms) {
    return (HAL_GetTick() - state_timer > ms);
}

static void State_Handle_Idle(void) {
    if (App_RFID_HasNewCard(&current_card)) {
        UI_ShowChecking(&current_card);
        State_Switch(STATE_CHECKING);
    }

    if (App_Button_IsHeld(5000)) {
        UI_ShowSpecialMenu();
        State_Switch(STATE_SPECIAL_MENU);
    }
}

static void State_Handle_Checking(void) {
    if (Timeout_Check(500)) {
        current_card_index = App_RFID_FindCard(&current_card);
        if (current_card_index != -1) {
            UI_ShowResult(true);
            HAL_Delay(2000);
            UI_ShowIdle();
            State_Switch(STATE_IDLE);
        } else {
            UI_ShowUnregistered();
            State_Switch(STATE_UNREGISTERED);
        }
    }
}

static void State_Handle_Unregistered(void) {
    if (App_Button_IsPressed()) {
        memset(input_pass, 0, sizeof(input_pass));
        pass_idx = 0;
        UI_ShowPasswordPrompt("");
        State_Switch(STATE_REGISTER_PASS);
        return;
    }
    if (Timeout_Check(5000)) { UI_ShowIdle(); State_Switch(STATE_IDLE); }
}

static void State_Handle_RegisterPass(void) {
    /* Expire before accepting a late confirmation. */
    if (Timeout_Check(10000)) {
        memset(input_pass, 0, sizeof(input_pass));
        pass_idx = 0;
        UI_ShowIdle();
        State_Switch(STATE_IDLE);
        return;
    }

    char key = App_Keypad_GetKey();
    if (key == '\0') return;
    state_timer = HAL_GetTick();

    if (key == '#') {
        if (strcmp(input_pass, master_pass) == 0) {
            if (App_RFID_AddCard(&current_card)) {
                current_card_index = App_RFID_FindCard(&current_card);
                UI_ShowMessage("Dang ky thanh", "cong!");
            } else {
                UI_ShowMessage("Bo nho day!", NULL);
            }
        } else {
            UI_ShowMessage("Sai Pass!", "Khong luu the");
        }
        memset(input_pass, 0, sizeof(input_pass));
        pass_idx = 0;
        HAL_Delay(2000);
        UI_ShowIdle();
        State_Switch(STATE_IDLE);
        return;
    }

    if (key == '*') {
        memset(input_pass, 0, sizeof(input_pass));
        pass_idx = 0;
    } else if (key >= '0' && key <= '9' &&
               pass_idx < sizeof(input_pass) - 1) {
        input_pass[pass_idx++] = key;
        input_pass[pass_idx] = '\0';
    }

    char stars[sizeof(input_pass)] = {0};
    memset(stars, '*', pass_idx);
    UI_ShowPasswordPrompt(stars);
}

static void State_Handle_SpecialMenu(void) {
    if (App_Button_IsPressed() && Timeout_Check(500)) { //wait 0.5s to enter password
        memset(input_pass, 0, sizeof(input_pass));
        pass_idx = 0;
        UI_ShowPasswordPrompt("");
        State_Switch(STATE_ENTER_PASS);
    } else if (Timeout_Check(5000)) {
        UI_ShowIdle();
        State_Switch(STATE_IDLE);
    }
}

static void State_Handle_EnterPass(void) {
    char key = App_Keypad_GetKey();
    if (key != '\0') {
        state_timer = HAL_GetTick();
        if (key == '#') {
            if (strcmp(input_pass, master_pass) == 0) {
                UI_ShowManageCard(&current_card); //the gan nhat, chua phai cau truc lua the
                State_Switch(STATE_MANAGE);
            } else {
                UI_ShowMessage("Sai Pass!", NULL);
                HAL_Delay(2000);
                UI_ShowIdle();
                State_Switch(STATE_IDLE);
            }
        } else if (pass_idx < 6) {
            input_pass[pass_idx++] = '*';
            UI_ShowPasswordPrompt(input_pass);
            input_pass[pass_idx - 1] = key;
        }
    }
    if (Timeout_Check(10000))
    {
    	UI_ShowIdle();
    	State_Switch(STATE_IDLE);
    }
}

static void State_Handle_Manage(void) {
    if (App_Button_IsPressed()) {
        UI_ShowMessage("Diem danh hop le", NULL);
        HAL_Delay(2000); UI_ShowIdle(); State_Switch(STATE_IDLE);
    }
    if (App_Keypad_GetKey() == '#' && current_card_index != -1)
    {
        App_RFID_DeleteCard(current_card_index);
        UI_ShowMessage("Da xoa the!", NULL);
        HAL_Delay(2000);

        UI_ShowIdle();
        State_Switch(STATE_IDLE);
    }
    if (Timeout_Check(10000))
    {
    	UI_ShowIdle();
    	State_Switch(STATE_IDLE);
    }
}

void App_Init(void) {
    BSP_RFID_Init();
    BSP_LCD_Init();
    App_RFID_InitDB();
    UI_ShowIdle();
}

void App_Task(void) {
    App_Button_Task(); //cap nhat nut nhan

    switch (current_state) {
        case STATE_IDLE:
        	State_Handle_Idle();
        	break;
        case STATE_CHECKING:
        	State_Handle_Checking();
        	break;
        case STATE_UNREGISTERED:
        	State_Handle_Unregistered();
        	break;
        case STATE_REGISTER_PASS:
            State_Handle_RegisterPass();
            break;
        case STATE_SPECIAL_MENU:
        	State_Handle_SpecialMenu();
        	break;
        case STATE_ENTER_PASS:
        	State_Handle_EnterPass();
        	break;
        case STATE_MANAGE:
        	State_Handle_Manage();
        	break;
    }
}
