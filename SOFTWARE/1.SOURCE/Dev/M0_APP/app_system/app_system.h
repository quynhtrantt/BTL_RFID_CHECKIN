#ifndef APP_SYSTEM_H
#define APP_SYSTEM_H
#include "driver_rfid.h"
#define APP_USER_COUNT 8
#define APP_LOG_COUNT 2
#define APP_NAME_SIZE 12
#define APP_PASS_LENGTH 6

typedef struct {
    RFID_UID_t card;
    char name[APP_NAME_SIZE];
} AppUser_t;

typedef struct {
    uint8_t uid[5];
    char name[APP_NAME_SIZE];
    uint32_t time, sequence;
} AppLog_t;

void App_System_InitDB(void);
bool App_System_Ready(void);
const char *App_System_ErrorText(void);
bool App_System_VerifyCard(RFID_UID_t *card);
int App_System_FindCard(RFID_UID_t *card);
bool App_System_AddCard(RFID_UID_t *card);
void App_System_DeleteCard(int index);
bool App_System_DeleteUser(int index);
bool App_System_GetUser(int index, AppUser_t *user);
int App_System_NextUser(int previous);
bool App_System_CheckPassword(const char *password);
bool App_System_ChangePassword(const char *password);
bool App_System_LogAttendance(const AppUser_t *user, uint32_t time);
bool App_System_ReadLog(unsigned newest_index, AppLog_t *log);
#endif
