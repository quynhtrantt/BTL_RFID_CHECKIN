#include "driver_i2c.h"
#include "board.h"
#include "stdbool.h"
#include <stddef.h>

#define HALF_BIT_LOOPS   60U
#define STRETCH_LOOPS    2000U

static bool ready = false;

static void HalfBit(void)
{
    for (volatile uint32_t i = 0; i < HALF_BIT_LOOPS; i++)
    {
    }
}

static void SCL(bool high)
{
    HAL_GPIO_WritePin(I2C_PORT, SCL_PIN,
                      high ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static void SDA(bool high)
{
    HAL_GPIO_WritePin(I2C_PORT, SDA_PIN,
                      high ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

static bool SDA_Read(void)
{
    return HAL_GPIO_ReadPin(I2C_PORT, SDA_PIN) == GPIO_PIN_SET;
}

/* Khoi tao Software I2C */
static void Bus_Init(void)
{
    GPIO_InitTypeDef pin = {0};

    if (ready)
        return;

    __HAL_RCC_GPIOB_CLK_ENABLE();

    pin.Pin = SCL_PIN | SDA_PIN;
    pin.Mode = GPIO_MODE_OUTPUT_OD;
    pin.Pull = GPIO_NOPULL;
    pin.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init(I2C_PORT, &pin);

    SCL(true);
    SDA(true);

    ready = true;
}

/* Tha SCL len muc cao */
static bool SCL_Release(void)
{
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

/* START condition */
static bool Start(void)
{
    SDA(true);

    if (!SCL_Release())
        return false;

    SDA(false);
    HalfBit();

    SCL(false);
    HalfBit();

    return true;
}

/* STOP condition */
static bool Stop(void)
{
    SDA(false);
    HalfBit();

    if (!SCL_Release())
        return false;

    SDA(true);
    HalfBit();

    return true;
}

/* Ghi 1 byte, kiem tra ACK */
static bool WriteByte(uint8_t value)
{
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

    /* Tha SDA de slave phan hoi ACK */
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

/* Doc 1 byte */
static uint8_t ReadByte(bool ack)
{
    uint8_t value = 0U;

    SDA(true);

    for (uint8_t i = 0; i < 8U; i++)
    {
        HalfBit();

        if (!SCL_Release())
        {
            SCL(false);
            return value;
        }

        value <<= 1;

        if (SDA_Read())
            value |= 1U;

        SCL(false);
    }

    /* ACK keo SDA xuong, NACK tha SDA */
    SDA(!ack);
    HalfBit();

    (void)SCL_Release();
    SCL(false);

    SDA(true);
    HalfBit();

    return value;
}

/* Ghi nhieu byte */
static bool WriteBytes(const uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        if (!WriteByte(data[i]))
            return false;
    }

    return true;
}

/* Ghi du lieu */
bool Driver_I2C_Write(uint8_t dev_addr,
                      uint8_t *data,
                      uint16_t len)
{
    if (data == NULL || len == 0U)
        return false;

    Bus_Init();

    if (!Start())
        return false;

    /* dev_addr la dia chi 7 bit */
    bool ok = WriteByte((uint8_t)(dev_addr << 1));

    if (ok)
        ok = WriteBytes(data, len);

    (void)Stop();

    return ok;
}

/* Ghi thanh ghi 8 bit */
bool Driver_I2C_Mem_Write(uint8_t dev_addr,
                          uint16_t reg_addr,
                          uint8_t *data,
                          uint16_t len)
{
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

    (void)Stop();

    return ok;
}

/* Doc thanh ghi 8 bit */
bool Driver_I2C_Mem_Read(uint8_t dev_addr,
                         uint16_t reg_addr,
                         uint8_t *data,
                         uint16_t len)
{
    if (data == NULL || len == 0U)
        return false;

    Bus_Init();

    if (!Start())
        return false;

    bool ok = WriteByte((uint8_t)(dev_addr << 1));

    if (ok)
        ok = WriteByte((uint8_t)reg_addr);

    if (ok)
    {
        /* Repeated START */
        ok = Start();

        if (ok)
        {
            ok = WriteByte((uint8_t)((dev_addr << 1) | 0x01U));
        }
    }

    if (ok)
    {
        for (uint16_t i = 0; i < len; i++)
        {
            data[i] = ReadByte(i + 1U < len);
        }
    }

    (void)Stop();

    return ok;
}

/* Kiem tra thiet bi co ACK khong */
bool Driver_I2C_IsReady(uint8_t dev_addr)
{
    Bus_Init();

    if (!Start())
        return false;

    bool ok = WriteByte((uint8_t)(dev_addr << 1));

    (void)Stop();

    return ok;
}
