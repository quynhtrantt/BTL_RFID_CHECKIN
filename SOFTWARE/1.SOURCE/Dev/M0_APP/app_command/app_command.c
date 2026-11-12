#include "printf.h"
#include "app_command.h"
#include "app_system.h"
#include "app_clock.h"
#include "bsp_uart.h"
#include <string.h>

#define COMMAND_SIZE 128U
#define RESPONSE_SPACE 128U
#define RX_BUDGET 64U

typedef enum {
    REPLY_NONE,
    REPLY_HELP,
    REPLY_USERS,
    REPLY_LOGS,
    REPLY_TIME,
    REPLY_ERROR
} Reply_t;

static Reply_t reply;
static char command[COMMAND_SIZE], error_text[48], rtc_text[20];
static uint16_t command_length;
static bool line_overflow, invalid_character;
static unsigned count, output_index;
static union {
    AppUser_t users[APP_USER_COUNT];
    AppLog_t logs[APP_LOG_COUNT];
} snapshot;

static void reset_line(void) {
    command_length = 0;
    line_overflow = false;
    invalid_character = false;
}

static void command_error(const char *text) {
    snprintf(error_text, sizeof(error_text), "ERR %s", text);
    reply = REPLY_ERROR;
}

static void format_uid(const uint8_t uid[5], char text[11]) {
    const char hex[] = "0123456789ABCDEF";
    for (unsigned i = 0; i < 5; i++)
    {
        text[i * 2] = hex[uid[i] >> 4];
        text[i * 2 + 1] = hex[uid[i] & 15];
    }
    text[10] = 0;
}

static void execute_command(void) {
    command[command_length] = 0;
    char *text = command;
    while (*text == ' ' || *text == '\t')
        text++;
    size_t length = strlen(text);
    while (length && (text[length - 1] == ' ' || text[length - 1] == '\t'))
        text[--length] = 0;
    for (size_t i = 0; i < length; i++)
        if (text[i] >= 'a' && text[i] <= 'z')
            text[i] -= 'a' - 'A';

    count = 0;
    output_index = 0;
    if (!length)
        return;
    if (!strcmp(text, "HELP"))
        reply = REPLY_HELP;
    else if (!strcmp(text, "TIME"))
    {
        uint32_t stamp;
        if (App_Clock_Get(&stamp) && App_Clock_FormatFull(stamp, rtc_text))
            reply = REPLY_TIME;
        else
            command_error("RTC unavailable");
    }
    else if (!strcmp(text, "USERS") || !strcmp(text, "LOGS"))
    {
        if (!App_System_Ready())
        {
            command_error(App_System_ErrorText());
            return;
        }
        if (!strcmp(text, "USERS"))
        {
            for (int i = 0; i < APP_USER_COUNT; i++)
                if (App_System_GetUser(i, &snapshot.users[count]))
                    count++;
            reply = REPLY_USERS;
        }
        else
        {
            for (unsigned i = 0; i < APP_LOG_COUNT; i++)
                if (App_System_ReadLog(i, &snapshot.logs[count]))
                    count++;
            reply = REPLY_LOGS;
        }
    }
    else
        command_error("unknown command; type HELP");
}

static void send_response(void) {
    if (reply == REPLY_NONE || BSP_UART_TxSpace() < RESPONSE_SPACE)
        return;
    int written = -1;
    bool last = false;
    char uid[11], time[20];
    if (reply == REPLY_HELP)
    {
        static const char *const lines[] = {
            "HELP BEGIN", "USERS - registered UID and name", "LOGS - last 2 successful scans",
            "TIME - current RTC", "HELP - command list", "HELP END"};
        written = BSP_UART_Printf("%s\r\n", lines[output_index]);
        last = output_index + 1U == sizeof(lines) / sizeof(lines[0]);
    }
    else if (reply == REPLY_USERS || reply == REPLY_LOGS)
    {
        const char *label = reply == REPLY_USERS ? "USERS" : "LOGS";
        if (!output_index)
            written = BSP_UART_Printf("%s BEGIN count=%u\r\n", label, count);
        else if (output_index <= count)
        {
            unsigned i = output_index - 1;
            if (reply == REPLY_USERS)
            {
                AppUser_t *user = &snapshot.users[i];
                format_uid(user->card.uid, uid);
                written = BSP_UART_Printf("%u | %s | %s\r\n", output_index, uid,
                                 user->name[0] ? user->name : "The khong ten");
            }
            else
            {
                AppLog_t *log = &snapshot.logs[i];
                format_uid(log->uid, uid);
                if (!App_Clock_FormatFull(log->time, time))
                    strcpy(time, "RTC invalid");
                written = BSP_UART_Printf("%u | %s | %s | %s | seq=%lu\r\n", output_index, uid,
                                 log->name[0] ? log->name : "The khong ten", time,
                                 (unsigned long)log->sequence);
            }
        }
        else
        {
            written = BSP_UART_Printf("%s END\r\n", label);
            last = true;
        }
    }
    else if (reply == REPLY_TIME)
    {
        written = BSP_UART_Printf("TIME %s\r\n", rtc_text);
        last = true;
    }
    else if (reply == REPLY_ERROR)
    {
        written = BSP_UART_Printf("%s\r\n", error_text);
        last = true;
    }
    if (written < 0)
    {
        return;
    }
    output_index++;
    if (last)
        reply = REPLY_NONE;
}

bool App_Command_Init(void) {
    bool ready = BSP_UART_Init();
    reply = REPLY_NONE;
    reset_line();
    BSP_UART_Printf("UART READY 115200 8N1\r\nUSERS | LOGS | TIME | HELP\r\n");
    return ready;
}

void App_Command_Task(void) {
    BSP_UART_Task();
    if (reply != REPLY_NONE)
    {
        send_response();
        return;
    }
    for (unsigned i = 0; i < RX_BUDGET; i++)
    {
        uint8_t value;
        int status = BSP_UART_ReadByte(&value);
        if (!status)
            break;
        if (status < 0)
        {
            reset_line();
            command_error("UART RX lost data; resend command");
            break;
        }
        if (value == '\r' || value == '\n')
        {
            if (line_overflow)
                command_error("line too long");
            else if (invalid_character)
                command_error("invalid character");
            else
                execute_command();
            reset_line();
            if (reply != REPLY_NONE)
                break;
        }
        else if (line_overflow || invalid_character)
            continue;
        else if (value == '\b' || value == 0x7F)
        {
            if (command_length)
                command_length--;
        }
        else if ((value < 0x20 && value != '\t') || value > 0x7E)
            invalid_character = true;
        else if (command_length + 1U < sizeof(command))
            command[command_length++] = (char)value;
        else
            line_overflow = true;
    }
    send_response();
}
