#include "app_clock.h"
#include "driver_mcp79410.h"
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <string.h>

static bool leap(unsigned y) {
    return y % 4 == 0 && (y % 100 != 0 || y % 400 == 0);
}

static unsigned days(unsigned y, unsigned m) {
    static const uint8_t d[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    return d[m - 1] + (m == 2 && leap(y));
}

static bool valid(const AppDateTime_t *t) {
    return t && t->year >= 2000 && t->year <= 2099 && t->month >= 1 && t->month <= 12 &&
           t->day >= 1 && t->day <= days(t->year, t->month) && t->hour < 24 && t->minute < 60 &&
           t->second < 60;
}

static uint8_t bcd(unsigned n) {
    return (n / 10) * 16 + n % 10;
}

static bool decode(uint8_t b, uint8_t *v) {
    if ((b & 15) > 9 || (b >> 4) > 9)
        return false;
    *v = (b >> 4) * 10 + (b & 15);
    return true;
}

static uint32_t encode(const AppDateTime_t *t) {
    uint32_t n = 0;
    for (unsigned y = 2000; y < t->year; y++)
        n += 365 + leap(y);
    for (unsigned m = 1; m < t->month; m++)
        n += days(t->year, m);
    return ((n + t->day - 1) * 24 + t->hour) * 3600 + t->minute * 60 + t->second;
}

bool App_Clock_Get(uint32_t *stamp) {
    uint8_t r[7], sec;
    if (!stamp)
        return false;
    for (unsigned attempt = 0; attempt < 3; attempt++)
    {
        if (!Driver_MCP_ReadRTC(0, r, 7) || !Driver_MCP_ReadRTC(0, &sec, 1))
            return false;
        if (r[0] != sec)
            continue;
        if (!(r[0] & 0x80) || !(r[3] & 0x20) || (r[2] & 0x40))
            return false;
        AppDateTime_t t;
        uint8_t y;
        if (!decode(r[0] & 0x7F, &t.second) || !decode(r[1] & 0x7F, &t.minute) ||
            !decode(r[2] & 0x3F, &t.hour) || !decode(r[4] & 0x3F, &t.day) ||
            !decode(r[5] & 0x1F, &t.month) || !decode(r[6], &y))
            return false;
        t.year = 2000 + y;
        if (!valid(&t))
            return false;
        *stamp = encode(&t);
        return true;
    }
    return false;
}

bool App_Clock_Set(const AppDateTime_t *t) {
    if (!valid(t))
        return false;
    uint8_t stop = 0, weekday = (uint8_t)((encode(t) / 86400 + 6) % 7 + 1);
    uint8_t r[] = {bcd(t->second), bcd(t->minute), bcd(t->hour),       (uint8_t)(weekday | 8),
                   bcd(t->day),    bcd(t->month),  bcd(t->year - 2000)};
    if (!Driver_MCP_WriteRTC(0, &stop, 1))
        return false;

    uint32_t start = HAL_GetTick();
    uint8_t status;
    do
    {
        if (!Driver_MCP_ReadRTC(3, &status, 1))
            return false;
        if ((uint32_t)(HAL_GetTick() - start) > 20)
            return false;
    } while (status & 0x20);
    if (!Driver_MCP_WriteRTC(0, r, 7))
        return false;
    r[0] |= 0x80;
    return Driver_MCP_WriteRTC(0, r, 1);
}

bool App_Clock_Init(void) {
    uint8_t r[7];
    uint32_t stamp;
    if (!Driver_MCP_ReadRTC(0, r, 7))
        return false;
    if (r[0] & 0x80)
    {

        r[3] |= 8;
        if (!Driver_MCP_WriteRTC(3, &r[3], 1))
            return false;
        return App_Clock_Get(&stamp);
    }

    const char *months = "JanFebMarAprMayJunJulAugSepOctNovDec";
    char mon[4] = {0};
    unsigned day, year, h, m, s;
    if (sscanf(__DATE__, "%3s %u %u", mon, &day, &year) != 3 ||
        sscanf(__TIME__, "%u:%u:%u", &h, &m, &s) != 3)
        return false;
    const char *pos = strstr(months, mon);
    if (!pos)
        return false;
    AppDateTime_t t = {(uint16_t)year, (uint8_t)((pos - months) / 3 + 1),
                       (uint8_t)day,   (uint8_t)h,
                       (uint8_t)m,     (uint8_t)s};
    return App_Clock_Set(&t);
}

void App_Clock_Format(uint32_t stamp, char line[17]) {
    unsigned sec = stamp % 60, min = stamp / 60 % 60, h = stamp / 3600 % 24;
    uint32_t n = stamp / 86400;
    unsigned y = 2000, m = 1;
    while (n >= 365U + leap(y))
    {
        n -= 365U + leap(y);
        y++;
    }
    while (n >= days(y, m))
    {
        n -= days(y, m);
        m++;
    }
    if (y > 2099)
    {
        memcpy(line, "RTC ngoai pham vi", 17);
        return;
    }
    unsigned values[] = {(unsigned)n + 1, m, h, min, sec};
    const unsigned pos[] = {0, 3, 6, 9, 12};
    memcpy(line, "00/00 00:00:00", 15);
    for (unsigned i = 0; i < 5; i++)
    {
        line[pos[i]] = '0' + values[i] / 10;
        line[pos[i] + 1] = '0' + values[i] % 10;
    }
}

bool App_Clock_FormatFull(uint32_t stamp, char line[20]) {
    if (!line)
        return false;
    uint32_t n = stamp / 86400U;
    unsigned year = 2000, month = 1;
    while (year <= 2099 && n >= 365U + leap(year))
    {
        n -= 365U + leap(year);
        year++;
    }
    if (year > 2099)
    {
        line[0] = 0;
        return false;
    }
    while (n >= days(year, month))
    {
        n -= days(year, month);
        month++;
    }
    memcpy(line, "00/00/0000 00:00:00", 20);
    unsigned values[] = {(unsigned)n + 1, month, stamp / 3600U % 24U,
                         stamp / 60U % 60U, stamp % 60U};
    const unsigned positions[] = {0, 3, 11, 14, 17};
    for (unsigned i = 0; i < 5; i++)
    {
        line[positions[i]] = '0' + values[i] / 10;
        line[positions[i] + 1] = '0' + values[i] % 10;
    }
    for (int i = 9; i >= 6; i--)
    {
        line[i] = '0' + year % 10;
        year /= 10;
    }
    return true;
}
