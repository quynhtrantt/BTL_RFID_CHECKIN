#ifndef M0_APP_APP_LCD_APP_LCD_H_
#define M0_APP_APP_LCD_APP_LCD_H_

#include "bsp_rfid.h"

void UI_ShowIdle(void);
void UI_ShowChecking(RFID_UID_t *card);
void UI_ShowResult(bool isValid);
void UI_ShowUnregistered(void);
void UI_ShowSpecialMenu(void);
void UI_ShowPasswordPrompt(const char* stars);
void UI_ShowManageCard(RFID_UID_t *card);
void UI_ShowMessage(const char *line1, const char *line2);

#endif /* M0_APP_APP_LCD_APP_LCD_H_ */
