#include "printf.h"
#include "app_lcd.h"
#include "bsp_lcd.h"
#include <string.h>

#define LCD_ROWS 2U
#define LCD_COLUMNS 16U

static char displayed[LCD_ROWS][LCD_COLUMNS];
static bool display_valid;

bool App_LCD_Init(void) {
    BSP_LCD_Init();
    display_valid = false;
    return true;
}

static void update_line(uint8_t row, const char *text) {
    char next[LCD_COLUMNS + 1U];
    memset(next, ' ', LCD_COLUMNS);
    next[LCD_COLUMNS] = 0;

    if (text)
    {
        for (unsigned i = 0; i < LCD_COLUMNS && text[i]; i++)
            next[i] = text[i];
    }

    unsigned column = 0;
    while (column < LCD_COLUMNS)
    {
        if (display_valid && displayed[row][column] == next[column])
        {
            column++;
            continue;
        }

        unsigned start = column;
        char changed[LCD_COLUMNS + 1U];
        unsigned length = 0;

        do
        {
            changed[length++] = next[column];
            column++;
        } while (column < LCD_COLUMNS &&
                 (!display_valid || displayed[row][column] != next[column]));

        changed[length] = 0;
        BSP_LCD_SetCursor((uint8_t)start, row);
        BSP_LCD_Print(changed);
        memcpy(displayed[row] + start, next + start, length);
    }
}

void UI_ShowMessage(const char *line1, const char *line2) {
    update_line(0, line1);
    update_line(1, line2);
    display_valid = true;
}

void UI_ShowIdle(void) {
    UI_ShowMessage("Trang thai cho", "Moi quet the");
}

void UI_ShowChecking(RFID_UID_t *card) {
    char buf[16];
    sprintf(buf, "UID:%02X%02X%02X%02X%02X", card->uid[0], card->uid[1], card->uid[2], card->uid[3],
            card->uid[4]);
    UI_ShowMessage("Dang kiem tra...", buf);
}

void UI_ShowResult(bool isValid) {
    UI_ShowMessage(isValid ? "The Hop Le!" : "The Tu Choi!", NULL);
}

void UI_ShowUnregistered(void) {
    UI_ShowMessage("The Chua Dang Ky", "Bam NUT de luu");
}

void UI_ShowSpecialMenu(void) {
    UI_ShowMessage("1.Diem Danh ?", "Bo qua sau 5s");
}

void UI_ShowPasswordPrompt(const char *stars) {
    UI_ShowMessage("Nhap Master Pass", stars);
}

void UI_ShowManageCard(RFID_UID_t *card) {
    char buf[16];
    sprintf(buf, "UID:%02X%02X%02X%02X%02X", card->uid[0], card->uid[1], card->uid[2], card->uid[3],
            card->uid[4]);
    UI_ShowMessage(buf, "NUT:D.Danh #:Xoa");
}
