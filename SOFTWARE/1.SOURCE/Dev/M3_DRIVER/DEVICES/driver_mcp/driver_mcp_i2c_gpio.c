#include "driver_mcp_i2c_gpio.h"
#include "board.h"
#include <stddef.h>
#define HALF_BIT_LOOPS 60U
#define CLOCK_WAIT_LOOPS 2000U
static bool initialized;
static bool sda_released;
static const char *error="OK";
const char *Driver_MCP_GPIO_Error(void) { return error; }
static void delay(void) {
    for(volatile uint32_t n=0;n<HALF_BIT_LOOPS;n++) {}
}
static void scl(bool released) {
    HAL_GPIO_WritePin(MCP_I2C_PORT,MCP_SCL_PIN,released?GPIO_PIN_SET:GPIO_PIN_RESET);
}
static void sda(bool released) {
    /* Explicitly tri-state SDA for logic 1, slave ACK and receive data.
       This avoids relying on the simulator's output-high OD interpretation.
       External pull-up to 3.3 V is required. Never drive SDA push-pull HIGH. */
    if(released==sda_released) return;
    GPIO_InitTypeDef gpio={0};
    gpio.Pin=MCP_SDA_PIN;
    gpio.Pull=GPIO_NOPULL;
    gpio.Speed=GPIO_SPEED_FREQ_HIGH;
    if(released) {
        /* Weak pull-up, not push-pull HIGH: the slave can still ACK LOW.
           On STM32F1 this also selects ODR=1 for input pull-up mode. */
        gpio.Pull=GPIO_PULLUP;
        gpio.Mode=GPIO_MODE_INPUT;
        HAL_GPIO_Init(MCP_I2C_PORT,&gpio);
    } else {
        HAL_GPIO_WritePin(MCP_I2C_PORT,MCP_SDA_PIN,GPIO_PIN_RESET);
        gpio.Mode=GPIO_MODE_OUTPUT_OD;
        HAL_GPIO_Init(MCP_I2C_PORT,&gpio);
    }
    sda_released=released;
}
static bool read_sda(void) {
    return HAL_GPIO_ReadPin(MCP_I2C_PORT,MCP_SDA_PIN)==GPIO_PIN_SET;
}
static void init(void) {
    if(initialized) return;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    /* PB10/PB11 are dedicated GPIO. No hardware I2C controller is enabled. */
    HAL_GPIO_WritePin(MCP_I2C_PORT,MCP_SCL_PIN|MCP_SDA_PIN,GPIO_PIN_SET);
    GPIO_InitTypeDef gpio={0};
    gpio.Pin=MCP_SCL_PIN|MCP_SDA_PIN;
    gpio.Mode=GPIO_MODE_OUTPUT_OD;
    gpio.Pull=GPIO_NOPULL;
    gpio.Speed=GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MCP_I2C_PORT,&gpio);
    sda_released=false;
    sda(true);
    initialized=true;
    delay();
}
static bool clock_high(void) {
    scl(true);
    for(uint32_t n=0;n<CLOCK_WAIT_LOOPS;n++) {
        if(HAL_GPIO_ReadPin(MCP_I2C_PORT,MCP_SCL_PIN)==GPIO_PIN_SET) {
            delay(); return true;
        }
    }
    error="SCL0";return false;
}
static bool start(void) {
    sda(true);delay();
    if(!clock_high()) return false;
    if(!read_sda()) { error="SDA0-ST";return false; }
    sda(false);delay();scl(false);delay();return true;
}
static bool stop(void) {
    scl(false);sda(false);delay();
    if(!clock_high()) { sda(true);return false; }
    sda(true);delay();
    if(!read_sda()) { error="SDA0-SP";return false; }
    return true;
}
static bool write_byte(uint8_t value,const char *nack) {
    for(uint8_t bit=0x80;bit;bit>>=1) {
        sda((value&bit)!=0);delay();
        if(!clock_high()) { scl(false);return false; }
        scl(false);delay();
    }
    sda(true);delay();
    if(!clock_high()) { scl(false);return false; }
    bool ack=!read_sda();
    scl(false);delay();
    if(!ack) error=nack;
    return ack;
}
static bool read_byte(uint8_t *value,bool ack) {
    uint8_t byte=0;sda(true);
    for(unsigned i=0;i<8;i++) {
        delay();
        if(!clock_high()) { scl(false);return false; }
        byte=(uint8_t)((byte<<1)|(read_sda()?1:0));
        scl(false);delay();
    }
    sda(!ack);delay();
    if(!clock_high()) { scl(false);sda(true);return false; }
    scl(false);delay();sda(true);delay();*value=byte;return true;
}
static bool finish(bool ok) {
    const char *cause=error;
    bool stopped=stop();
    if(!ok) error=cause;
    else if(stopped) error="OK";
    return ok&&stopped;
}
static bool valid(uint8_t device,uint8_t reg,const void *p,uint16_t len) {
    if(device>0x7F || !p || !len || (uint32_t)reg+len>256U) {
        error="ARG";return false;
    }
    return true;
}
bool Driver_MCP_GPIO_Read(uint8_t device,uint8_t reg,uint8_t *data,uint16_t len) {
    if(!valid(device,reg,data,len)) return false;
    init();error="OK";
    if(!start()) return finish(false);
    if(!write_byte((uint8_t)(device<<1),"AW-NACK")) return finish(false);
    if(!write_byte(reg,"REG-NACK")) return finish(false);
    if(!start()) return finish(false); /* repeated START, no intervening STOP */
    if(!write_byte((uint8_t)((device<<1)|1U),"AR-NACK")) return finish(false);
    for(uint16_t i=0;i<len;i++) if(!read_byte(data+i,i+1U<len)) return finish(false);
    return finish(true);
}
bool Driver_MCP_GPIO_Write(uint8_t device,uint8_t reg,const uint8_t *data,uint16_t len) {
    if(!valid(device,reg,data,len)) return false;
    init();error="OK";
    if(!start()) return finish(false);
    if(!write_byte((uint8_t)(device<<1),"AW-NACK")) return finish(false);
    if(!write_byte(reg,"REG-NACK")) return finish(false);
    for(uint16_t i=0;i<len;i++) if(!write_byte(data[i],"WR-NACK")) return finish(false);
    return finish(true);
}
bool Driver_MCP_GPIO_IsReady(uint8_t device) {
    if(device>0x7F) { error="ARG";return false; }
    init();error="OK";
    if(!start()) return finish(false);
    return finish(write_byte((uint8_t)(device<<1),"AW-NACK"));
}
