#ifndef M2_BSP_BSP_RTC_BSP_RTC_H_
#define M2_BSP_BSP_RTC_BSP_RTC_H_

#include "driver_rtc.h"

void BSP_RTC_Init(void);
bool BSP_RTC_GetTime(RTC_Time_t *time);

#endif /* M2_BSP_BSP_RTC_BSP_RTC_H_ */
