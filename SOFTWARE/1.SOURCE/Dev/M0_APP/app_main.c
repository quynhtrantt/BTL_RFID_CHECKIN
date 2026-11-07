#include "app_main.h"
#include "app_system.h"
#include "app_rfid.h"
#include "app_lcd.h"
#include "app_clock.h"
#include "app_keypad.h"
#include "app_button.h"
#include "bsp_rfid.h"
#include <stdio.h>
#include <string.h>
#include "stm32f1xx_hal.h"

typedef enum {
    STATE_IDLE, STATE_CHECKING, STATE_SHOW_USER, STATE_RESULT,
    STATE_STORAGE_ERROR, STATE_MENU, STATE_AUTH, STATE_REGISTER_SCAN, STATE_REGISTER_CONFIRM,
    STATE_DELETE_SELECT, STATE_DELETE_AUTH, STATE_NEW_PASSWORD
} AppState_t;
typedef enum { ACTION_REGISTER, ACTION_DELETE, ACTION_PASSWORD } AdminAction_t;
static AppState_t state;
static AdminAction_t action;
static uint32_t timer, result_ms, scan_time;
static RFID_UID_t pending_card;
static AppUser_t user;
static int selected=-1;
static char password[APP_PASS_LENGTH+1];
static uint8_t pass_length;
static bool pass_overflow;

static void enter(AppState_t next) { state=next; timer=HAL_GetTick(); }
static bool elapsed(uint32_t ms) { return (uint32_t)(HAL_GetTick()-timer)>=ms; }
static void clear_pass(void) {
    memset(password,0,sizeof(password)); pass_length=0; pass_overflow=false;
}
static void idle(void) {
    clear_pass(); memset(&pending_card,0,sizeof(pending_card)); memset(&user,0,sizeof(user));
    selected=-1; scan_time=0; action=ACTION_REGISTER;
    if(!App_System_Ready()) {
        UI_ShowMessage("MCP GPIO PB10/11",App_System_ErrorText()); enter(STATE_STORAGE_ERROR);
    } else { UI_ShowMessage("Cho quet the","*:Menu admin"); enter(STATE_IDLE); }
}
static void result(const char *a,const char *b,uint32_t ms) {
    clear_pass(); UI_ShowMessage(a,b); result_ms=ms; enter(STATE_RESULT);
}
static void show_user(const AppUser_t *u) {
    char line[17];
    snprintf(line,sizeof(line),"UID:%02X%02X%02X%02X%02X",u->card.uid[0],u->card.uid[1],
        u->card.uid[2],u->card.uid[3],u->card.uid[4]);
    UI_ShowMessage(u->name[0]?u->name:"The khong ten",line);
}
static void auth(AppState_t next) {
    clear_pass(); UI_ShowMessage("Nhap pass hien tai","# OK, * Xoa"); enter(next);
}
/* Return 1 on confirmation, 0 while editing, -1 on an invalid length.
   Extra digits are rejected until the admin clears the entire input. */
static int password_key(char key) {
    if(!key) return 0;
    timer=HAL_GetTick();
    if(key=='#') return !pass_overflow && pass_length==APP_PASS_LENGTH?1:-1;
    if(key=='*') clear_pass();
    else if(key>='0' && key<='9') {
        if(pass_length<APP_PASS_LENGTH) password[pass_length++]=key;
        else pass_overflow=true;
    }
    char stars[APP_PASS_LENGTH+1]={0}; memset(stars,'*',pass_length);
    UI_ShowMessage(state==STATE_NEW_PASSWORD?"Pass moi: 6 so":"Nhap pass hien tai",
        pass_overflow?"Qua dai! * Xoa":stars);
    return 0;
}
static bool admin_state(void) {
    return state>=STATE_MENU;
}
void App_Init(void) {
    BSP_RFID_Init(); App_LCD_Init();
    App_RFID_InitDB();
    (void)App_Clock_Init();
    idle();

}
void App_Task(void) {
    App_Button_Task();
    bool bt1=App_Button_IsPressed(),bt2=App_Button2_IsPressed();
    char key=App_Keypad_GetKey();
    RFID_UID_t scanned;
    /* Poll continuously so presses outside scanning states cannot be replayed. */
    bool new_card=App_RFID_HasNewCard(&scanned);
    if(state!=STATE_STORAGE_ERROR && !App_System_Ready()) { idle(); return; }
    if(admin_state() && elapsed(5000)) { idle(); return; }
    switch(state) {
    case STATE_STORAGE_ERROR:
        /* Show the fault continuously, not a misleading ready-to-scan screen.
           # retries loading/provisioning; it never formats existing data. */
        if(key=='#') { App_System_InitDB(); idle(); }
        break;
    case STATE_IDLE:
        if(key=='*') {
            if(!App_System_Ready()) { result("Loi EEPROM MCP",App_System_ErrorText(),5000); break; }
            UI_ShowMessage("BT1:Dang ky moi","BT2:Xoa #:Pass"); enter(STATE_MENU);
        } else if(new_card && App_System_Ready() && scanned.size==5) {
            pending_card=scanned; UI_ShowMessage("Dang kiem tra...",NULL); enter(STATE_CHECKING);
        }
        break;
    case STATE_CHECKING:
        if(elapsed(300)) {
            selected=App_System_FindCard(&pending_card);
            if(selected<0 || !App_System_GetUser(selected,&user)) { idle(); break; }
            if(!App_Clock_Get(&scan_time)) { result("Loi gio RTC","Khong luu diem danh",2000); break; }
            show_user(&user); enter(STATE_SHOW_USER);
        }
        break;
    case STATE_SHOW_USER:
        if(elapsed(5000)) {
            if(!App_System_LogAttendance(&user,scan_time)) result("Luu that bai","Kiem tra EEPROM",2000);
            else { char line[17]; App_Clock_Format(scan_time,line); result("Quet thanh cong",line,5000); }
        }
        break;
    case STATE_RESULT:
        if(elapsed(result_ms)) idle();
        break;
    case STATE_MENU:
        if(bt1 || bt2 || key=='#') {
            action=bt1?ACTION_REGISTER:bt2?ACTION_DELETE:ACTION_PASSWORD;
            auth(STATE_AUTH);
        }
        break;
    case STATE_AUTH: {
        int done=password_key(key);
        if(!done) break;
        if(done<0 || !App_System_CheckPassword(password)) { result("Sai mat khau",NULL,1500); break; }
        clear_pass();
        if(action==ACTION_REGISTER) {
            UI_ShowMessage("Quet the moi","PB13:Quet DIP"); enter(STATE_REGISTER_SCAN);
        } else if(action==ACTION_DELETE) {
            selected=App_System_NextUser(-1);
            if(selected<0 || !App_System_GetUser(selected,&user)) result("Khong co the",NULL,1500);
            else { show_user(&user); enter(STATE_DELETE_SELECT); }
        } else {
            UI_ShowMessage("Pass moi: 6 so","# OK, * Xoa"); enter(STATE_NEW_PASSWORD);
        }
        break;
    }
    case STATE_REGISTER_SCAN:
        if(new_card && scanned.size==5) {
            if(App_System_FindCard(&scanned)>=0) { result("The da dang ky",NULL,1500); break; }
            pending_card=scanned;
            AppUser_t preview={0}; preview.card=pending_card;
            show_user(&preview); enter(STATE_REGISTER_CONFIRM);
        }
        break;
    case STATE_REGISTER_CONFIRM:
        if(bt1) {
            if(App_System_AddCard(&pending_card)) result("Dang ky thanh","cong!",2000);
            else result("Khong them duoc","Bo nho day/loi",2000);
        }
        break;
    case STATE_DELETE_SELECT:
        if(key=='*') auth(STATE_DELETE_AUTH);
        else if(bt2) {
            selected=App_System_NextUser(selected);
            if(selected<0 || !App_System_GetUser(selected,&user)) result("Khong co the",NULL,1500);
            else { show_user(&user); timer=HAL_GetTick(); }
        }
        break;
    case STATE_DELETE_AUTH: {
        int done=password_key(key);
        if(!done) break;
        if(done<0 || !App_System_CheckPassword(password)) result("Sai mat khau","Khong xoa the",1500);
        else if(App_System_DeleteUser(selected)) result("Xoa thanh cong",NULL,2000);
        else result("Xoa that bai","Kiem tra EEPROM",2000);
        break;
    }
    case STATE_NEW_PASSWORD: {
        int done=password_key(key);
        if(!done) break;
        if(done<0) result("Pass can 6 so","Chua doi pass",1500);
        else if(App_System_ChangePassword(password)) result("Doi pass thanh","cong!",2000);
        else result("Loi luu pass","Hay khoi dong lai",2000);
        break;
    }
    default: idle(); break;
    }
}
