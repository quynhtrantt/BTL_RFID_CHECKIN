#include "bsp_rtc.h"
#define RTC_I2C_ADDRESS (0x68 << 1)

void BSP_RTC_Init(void)
{
	Driver_RTC_Init(RTC_I2C_ADDRESS);
}
bool BSP_RTC_GetTime(RTC_Time_t *time)
{
	return Driver_RTC_GetTime(time);
}
