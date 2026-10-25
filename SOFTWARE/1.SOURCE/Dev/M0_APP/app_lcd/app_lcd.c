#include "app_lcd.h"
#include "bsp_lcd.h"
#include <stdio.h>

void UI_ShowMessage(const char *line1, const char *line2)
{
    BSP_LCD_Clear();
    if(line1)
    {
    	BSP_LCD_SetCursor(0, 0);
        BSP_LCD_Print(line1);
    }
    if(line2)
    {
    	BSP_LCD_SetCursor(0, 1);
        BSP_LCD_Print(line2);
    }
}


void UI_ShowIdle(void)
{
	UI_ShowMessage("He Thong Cua", "Quet the...");
}

void UI_ShowChecking(RFID_UID_t *card)
{
    char buf[16];
    sprintf(buf, "UID:%02X%02X%02X%02X", card->uid[0], card->uid[1], card->uid[2], card->uid[3]);
    UI_ShowMessage("Dang kiem tra...", buf);
}

void UI_ShowResult(bool isValid) {
    UI_ShowMessage(isValid ? "The Hop Le!" : "The Tu Choi!", NULL);
}

void UI_ShowUnregistered(void)
{
	UI_ShowMessage("The Chua Dang Ky", "Bam NUT de luu");
}

void UI_ShowSpecialMenu(void)
{
	UI_ShowMessage("1.Diem Danh ?", "Bo qua sau 5s");
}

void UI_ShowPasswordPrompt(const char* stars)
{
	UI_ShowMessage("Nhap Master Pass", stars);
}

void UI_ShowManageCard(RFID_UID_t *card)
{
    char buf[16];
    sprintf(buf, "UID:%02X%02X%02X%02X", card->uid[0], card->uid[1], card->uid[2], card->uid[3]);
    UI_ShowMessage(buf, "NUT:D.Danh #:Xoa");
}
