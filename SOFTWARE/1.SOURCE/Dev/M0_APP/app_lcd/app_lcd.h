#ifndef M0_APP_APP_LCD_APP_LCD_H_
#define M0_APP_APP_LCD_APP_LCD_H_

#include <stdint.h>
#include <stdbool.h>

void App_LCD_Init(void);
void App_LCD_ShowUID(uint8_t *uid, uint8_t size);
void App_LCD_ShowStatus(bool isValid);

#endif /* M0_APP_APP_LCD_APP_LCD_H_ */
