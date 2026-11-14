#include "printf.h"
#include "app_main.h"
#include "app_system.h"
#include "app_rfid.h"
#include "app_lcd.h"
#include "app_clock.h"
#include "app_keypad.h"
#include "app_button.h"
#include "bsp_rfid.h"
#include <string.h>
#include "stm32f1xx_hal.h"

typedef enum {
    STATE_IDLE,
    STATE_CHECKING,
    STATE_SHOW_USER,
    STATE_RESULT,
    STATE_STORAGE_ERROR,
    STATE_MENU,
    STATE_AUTH,
    STATE_REGISTER_SCAN,
    STATE_REGISTER_CONFIRM,
    STATE_DELETE_SELECT,
    STATE_DELETE_AUTH,
    STATE_NEW_PASSWORD
} AppState_t;

typedef enum {
    ACTION_REGISTER,
    ACTION_DELETE,
    ACTION_PASSWORD
} AdminAction_t;

typedef struct {
    bool bt1;
    bool bt2;
    char key;
    bool new_card;
    RFID_UID_t scanned;
} AppInputs_t;

static AppInputs_t inputs;
static AppState_t state;
static AdminAction_t action;
static uint32_t timer, result_ms, scan_time;
static RFID_UID_t pending_card;
static AppUser_t user;
static int selected = -1;
static char password[APP_PASS_LENGTH + 1];
static uint8_t pass_length;
static bool pass_overflow;

static void enter(AppState_t next) {
    state = next;
    timer = HAL_GetTick();
}

static bool elapsed(uint32_t ms) {
    return (uint32_t)(HAL_GetTick() - timer) >= ms;
}

static void clear_pass(void) {
    memset(password, 0, sizeof(password));
    pass_length = 0;
    pass_overflow = false;
}

static void idle(void) {
    clear_pass();
    memset(&pending_card, 0, sizeof(pending_card));
    memset(&user, 0, sizeof(user));
    selected = -1;
    scan_time = 0;
    action = ACTION_REGISTER;
    if (!App_System_Ready())
    {
        UI_ShowMessage("MCP GPIO PB10/11", App_System_ErrorText());
        enter(STATE_STORAGE_ERROR);
    }
    else
    {
        UI_ShowMessage("Cho quet the", "*:Menu admin");
        enter(STATE_IDLE);
    }
}

static void result(const char *a, const char *b, uint32_t ms) {
    clear_pass();
    UI_ShowMessage(a, b);
    result_ms = ms;
    enter(STATE_RESULT);
}

static void show_user(const AppUser_t *u) {
    char line[17];
    snprintf(line, sizeof(line), "UID:%02X%02X%02X%02X%02X", u->card.uid[0], u->card.uid[1],
             u->card.uid[2], u->card.uid[3], u->card.uid[4]);
    UI_ShowMessage(u->name[0] ? u->name : "The khong ten", line);
}

static void auth(AppState_t next) {
    clear_pass();
    UI_ShowMessage("Nhap pass hien tai", NULL);
    enter(next);
}

static int password_key(char key) {
    if (!key)
        return 0;
    timer = HAL_GetTick();
    if (key == '#')
        return !pass_overflow && pass_length == APP_PASS_LENGTH ? 1 : -1;
    if (key == '*')
        clear_pass();
    else if (key >= '0' && key <= '9')
    {
        if (pass_length < APP_PASS_LENGTH)
            password[pass_length++] = key;
        else
            pass_overflow = true;
    }
    char stars[APP_PASS_LENGTH + 1] = {0};
    memset(stars, '*', pass_length);
    UI_UpdatePassword(pass_overflow ? "Qua dai! * Xoa" : stars);
    return 0;
}

static bool admin_state(void) {
    return state >= STATE_MENU;
}

void App_Init(void) {
    BSP_RFID_Init();
    App_LCD_Init();
    App_RFID_InitDB();
    (void)App_Clock_Init();
    idle();
}

static void read_inputs(void) {
    App_Button_Task();
    inputs.bt1 = App_Button_IsPressed();
    inputs.bt2 = App_Button2_IsPressed();
    inputs.key = App_Keypad_GetKey();
    inputs.new_card = App_RFID_HasNewCard(&inputs.scanned);
}

static bool state_ready(void) {
    if (state != STATE_STORAGE_ERROR && !App_System_Ready())
    {
        idle();
        return false;
    }
    if (admin_state() && elapsed(5000))
    {
        idle();
        return false;
    }
    return true;
}

static void State_StorageError(void) {

    if (inputs.key == '#')
    {
        App_System_InitDB();
        idle();
    }
}

static void State_Idle(void) {
    if (inputs.key == '*')
    {
        if (!App_System_Ready())
        {
            result("Loi EEPROM MCP", App_System_ErrorText(), 5000);
            return;
        }
        UI_ShowMessage("BT1:Dang ky moi", "BT2:Xoa #:Pass");
        enter(STATE_MENU);
    }
    else if (inputs.new_card && App_System_Ready() && inputs.scanned.size == 5)
    {
        pending_card = inputs.scanned;
        UI_ShowMessage("Dang kiem tra...", NULL);
        enter(STATE_CHECKING);
    }
}

static void State_Checking(void) {
    if (elapsed(300))
    {
        selected = App_System_FindCard(&pending_card);
        if (selected < 0)
        {
            char line[17];
            snprintf(line, sizeof(line), "UID:%02X%02X%02X%02X%02X", pending_card.uid[0],
                     pending_card.uid[1], pending_card.uid[2], pending_card.uid[3], pending_card.uid[4]);
            result("The chua dang ky", line, 3000);
            return;
        }
        if (!App_System_GetUser(selected, &user))
        {
            idle();
            return;
        }
        if (!App_Clock_Get(&scan_time))
        {
            result("Loi gio RTC", "Khong luu diem danh", 2000);
            return;
        }
        show_user(&user);
        enter(STATE_SHOW_USER);
    }
}

static void State_ShowUser(void) {
    if (elapsed(5000))
    {
        if (!App_System_LogAttendance(&user, scan_time))
            result("Luu that bai", "Kiem tra EEPROM", 2000);
        else
        {
            char line[17];
            App_Clock_Format(scan_time, line);
            result("Quet thanh cong", line, 5000);
        }
    }
}

static void State_Result(void) {
    if (elapsed(result_ms))
        idle();
}

static void State_Menu(void) {
    if (inputs.bt1 || inputs.bt2 || inputs.key == '#')
    {
        action = inputs.bt1 ? ACTION_REGISTER : inputs.bt2 ? ACTION_DELETE : ACTION_PASSWORD;
        auth(STATE_AUTH);
    }
}

static void State_Auth(void) {
    int done = password_key(inputs.key);
    if (!done)
        return;
    if (done < 0 || !App_System_CheckPassword(password))
    {
        result("Sai mat khau", NULL, 1500);
        return;
    }
    clear_pass();
    if (action == ACTION_REGISTER)
    {
        UI_ShowMessage("Quet the moi", "PB13:Quet DIP");
        enter(STATE_REGISTER_SCAN);
    }
    else if (action == ACTION_DELETE)
    {
        selected = App_System_NextUser(-1);
        if (selected < 0 || !App_System_GetUser(selected, &user))
            result("Khong co the", NULL, 1500);
        else
        {
            show_user(&user);
            enter(STATE_DELETE_SELECT);
        }
    }
    else
    {
        UI_ShowMessage("Pass moi: 6 so", NULL);
        enter(STATE_NEW_PASSWORD);
    }
}

static void State_RegisterScan(void) {
    if (inputs.new_card && inputs.scanned.size == 5)
    {
        if (App_System_FindCard(&inputs.scanned) >= 0)
        {
            result("The da dang ky", NULL, 1500);
            return;
        }
        pending_card = inputs.scanned;
        AppUser_t preview = {0};
        preview.card = pending_card;
        show_user(&preview);
        enter(STATE_REGISTER_CONFIRM);
    }
}

static void State_RegisterConfirm(void) {
    if (inputs.bt1)
    {
        if (App_System_AddCard(&pending_card))
            result("Dang ky thanh", "cong!", 2000);
        else
            result("Khong them duoc", "Bo nho day/loi", 2000);
    }
}

static void State_DeleteSelect(void) {
    if (inputs.key == '*')
        auth(STATE_DELETE_AUTH);
    else if (inputs.bt2)
    {
        selected = App_System_NextUser(selected);
        if (selected < 0 || !App_System_GetUser(selected, &user))
            result("Khong co the", NULL, 1500);
        else
        {
            show_user(&user);
            timer = HAL_GetTick();
        }
    }
}

static void State_DeleteAuth(void) {
    int done = password_key(inputs.key);
    if (!done)
        return;
    if (done < 0 || !App_System_CheckPassword(password))
        result("Sai mat khau", "Khong xoa the", 1500);
    else if (App_System_DeleteUser(selected))
        result("Xoa thanh cong", NULL, 2000);
    else
        result("Xoa that bai", "Kiem tra EEPROM", 2000);
}

static void State_NewPassword(void) {
    int done = password_key(inputs.key);
    if (!done)
        return;
    if (done < 0)
        result("Pass can 6 so", "Chua doi pass", 1500);
    else if (App_System_ChangePassword(password))
        result("Doi pass thanh", "cong!", 2000);
    else
        result("Loi luu pass", "Hay khoi dong lai", 2000);
}

void App_Task(void) {
    read_inputs();
    if (!state_ready())
        return;

    switch (state)
    {
    case STATE_STORAGE_ERROR:
        State_StorageError();
        break;
    case STATE_IDLE:
        State_Idle();
        break;
    case STATE_CHECKING:
        State_Checking();
        break;
    case STATE_SHOW_USER:
        State_ShowUser();
        break;
    case STATE_RESULT:
        State_Result();
        break;
    case STATE_MENU:
        State_Menu();
        break;
    case STATE_AUTH:
        State_Auth();
        break;
    case STATE_REGISTER_SCAN:
        State_RegisterScan();
        break;
    case STATE_REGISTER_CONFIRM:
        State_RegisterConfirm();
        break;
    case STATE_DELETE_SELECT:
        State_DeleteSelect();
        break;
    case STATE_DELETE_AUTH:
        State_DeleteAuth();
        break;
    case STATE_NEW_PASSWORD:
        State_NewPassword();
        break;
    default:
        idle();
        break;
    }
}
