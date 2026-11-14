#include "driver_i2c.h"
#include "board.h"
#include "stdbool.h"
#include <stddef.h>

#define HALF_BIT_US 6U
#define STRETCH_LOOPS 2000U

static bool ready = false;

static void HalfBit(void) {
    uint32_t ticks = (SystemCoreClock / 1000000U) * HALF_BIT_US;
    uint32_t period = SysTick->LOAD + 1U;
    uint32_t previous = SysTick->VAL;
    uint32_t elapsed = 0U;

    while (elapsed < ticks)
    {
        uint32_t current = SysTick->VAL;
        elapsed += previous >= current ? previous - current : previous + period - current;
        previous = current;
    }
}

static void SCL(bool high) {
    HAL_GPIO_WritePin(I2C_PORT, SCL_PIN, high ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void SDA(bool high) {
    HAL_GPIO_WritePin(I2C_PORT, SDA_PIN, high ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static bool SDA_Read(void) {
    return HAL_GPIO_ReadPin(I2C_PORT, SDA_PIN) == GPIO_PIN_SET;
}

static void Bus_Init(void) {
    GPIO_InitTypeDef pin = {0};

    if (ready)
        return;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_WritePin(I2C_PORT, SCL_PIN | SDA_PIN, GPIO_PIN_SET);

    pin.Pin = SCL_PIN | SDA_PIN;
    pin.Mode = GPIO_MODE_OUTPUT_OD;
    pin.Pull = GPIO_NOPULL;
    pin.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(I2C_PORT, &pin);

    SCL(true);
    SDA(true);
    ready = true;
}

static bool SCL_Release(void) {
    SCL(true);

    uint32_t timeout = 0;

    while (HAL_GPIO_ReadPin(I2C_PORT, SCL_PIN) == GPIO_PIN_RESET)
    {
        if (++timeout >= STRETCH_LOOPS)
            return false;
    }

    HalfBit();
    return true;
}

static bool Start(void) {
    SDA(true);

    if (!SCL_Release())
        return false;

    SDA(false);
    HalfBit();

    SCL(false);
    HalfBit();

    return true;
}

static bool Stop(void) {
    SDA(false);
    HalfBit();

    if (!SCL_Release())
        return false;

    SDA(true);
    HalfBit();

    return true;
}

static bool WriteByte(uint8_t value) {
    for (uint8_t mask = 0x80U; mask != 0U; mask >>= 1)
    {
        SDA((value & mask) != 0U);
        HalfBit();

        if (!SCL_Release())
        {
            SCL(false);
            return false;
        }

        SCL(false);
    }

    SDA(true);
    HalfBit();

    if (!SCL_Release())
    {
        SCL(false);
        return false;
    }

    bool ack = !SDA_Read();

    SCL(false);
    HalfBit();

    return ack;
}

static bool ReadByte(bool ack, uint8_t *out) {
    uint8_t value = 0U;

    SDA(true);

    for (uint8_t i = 0; i < 8U; i++)
    {
        HalfBit();

        if (!SCL_Release())
        {
            SCL(false);
            return false;
        }

        value <<= 1;

        if (SDA_Read())
            value |= 1U;

        SCL(false);
    }

    SDA(!ack);
    HalfBit();

    if (!SCL_Release())
    {
        SCL(false);
        SDA(true);
        return false;
    }
    SCL(false);
    SDA(true);
    HalfBit();
    *out = value;
    return true;
}

static bool WriteBytes(const uint8_t *data, uint16_t len) {
    for (uint16_t i = 0; i < len; i++)
    {
        if (!WriteByte(data[i]))
            return false;
    }

    return true;
}

bool Driver_I2C_Write(uint8_t dev_addr, uint8_t *data, uint16_t len) {
    if (data == NULL || len == 0U)
        return false;

    Bus_Init();

    if (!Start())
        return false;

    bool ok = WriteByte((uint8_t)(dev_addr << 1));

    if (ok)
        ok = WriteBytes(data, len);

    bool stopped = Stop();

    return ok && stopped;
}

bool Driver_I2C_Mem_Write(uint8_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t len) {
    if (data == NULL || len == 0U)
        return false;

    Bus_Init();

    if (!Start())
        return false;

    bool ok = WriteByte((uint8_t)(dev_addr << 1));

    if (ok)
        ok = WriteByte((uint8_t)reg_addr);

    if (ok)
        ok = WriteBytes(data, len);

    bool stopped = Stop();

    return ok && stopped;
}

static const char *read_stage = "OK";

const char *Driver_I2C_ReadStage(void) {
    return read_stage;
}

bool Driver_I2C_Mem_Read(uint8_t dev_addr, uint16_t reg_addr, uint8_t *data, uint16_t len) {
    read_stage = "ARG";
    if (!data || !len)
        return false;
    Bus_Init();
    read_stage = "START";
    if (!Start())
        return false;
    bool ok = false;
    read_stage = "AW";
    if (!WriteByte((uint8_t)(dev_addr << 1)))
        goto finish;
    read_stage = "REG";
    if (!WriteByte((uint8_t)reg_addr))
        goto finish;
    read_stage = "RESTART";
    if (!Start())
        goto finish;
    read_stage = "AR";
    if (!WriteByte((uint8_t)((dev_addr << 1) | 1U)))
        goto finish;
    read_stage = "DATA";
    for (uint16_t i = 0; i < len; i++)
    {
        if (!ReadByte(i + 1U < len, &data[i]))
            goto finish;
    }
    ok = true;
finish:
    if (!Stop())
    {
        if (ok)
            read_stage = "STOP";
        return false;
    }
    if (ok)
        read_stage = "OK";
    return ok;
}

bool Driver_I2C_IsReady(uint8_t dev_addr) {
    Bus_Init();

    if (!Start())
        return false;

    bool ok = WriteByte((uint8_t)(dev_addr << 1));

    bool stopped = Stop();

    return ok && stopped;
}
