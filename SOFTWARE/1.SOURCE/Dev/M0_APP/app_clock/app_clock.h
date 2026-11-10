#ifndef APP_CLOCK_H
#define APP_CLOCK_H
#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint16_t year;
    uint8_t month, day, hour, minute, second;
} AppDateTime_t;

bool App_Clock_Init(void);
bool App_Clock_Get(uint32_t *seconds_since_2000);
bool App_Clock_Set(const AppDateTime_t *time);
void App_Clock_Format(uint32_t seconds_since_2000, char line[17]);
#endif
