#ifndef M3_DRIVER_DEVICES_DRIVER_RTC_DRIVER_RTC_H_
#define M3_DRIVER_DEVICES_DRIVER_RTC_DRIVER_RTC_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct {
    uint8_t hour, min, sec;
    uint8_t date, month, year;
} RTC_Time_t;

void Driver_RTC_Init(uint8_t i2c_addr);
bool Driver_RTC_GetTime(RTC_Time_t *time);

#endif /* M3_DRIVER_DEVICES_DRIVER_RTC_DRIVER_RTC_H_ */
