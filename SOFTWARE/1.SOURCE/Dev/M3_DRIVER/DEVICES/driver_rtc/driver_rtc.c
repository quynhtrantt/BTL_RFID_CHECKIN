#include "driver_rtc.h"
#include "driver_i2c.h"

static uint8_t rtc_addr;

static uint8_t BCD_To_Dec(uint8_t val) { return (val >> 4) * 10 + (val & 0x0F); }

void Driver_RTC_Init(uint8_t i2c_addr) { rtc_addr = i2c_addr; }

bool Driver_RTC_GetTime(RTC_Time_t *time) {
    uint8_t buffer[7];
    if (Driver_I2C_Mem_Read(rtc_addr, 0x00, buffer, 7)) {
        time->sec   = BCD_To_Dec(buffer[0]);
        time->min   = BCD_To_Dec(buffer[1]);
        time->hour  = BCD_To_Dec(buffer[2]);
        time->date  = BCD_To_Dec(buffer[4]);
        time->month = BCD_To_Dec(buffer[5]);
        time->year  = BCD_To_Dec(buffer[6]);
        return true;
    }
    return false;
}
