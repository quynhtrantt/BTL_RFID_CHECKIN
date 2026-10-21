#include "bsp_eeprom24c04.h"
#include "board.h"
#include "driver_gpio.h"
#include "driver_eeprom24c04.h"

#define EEPROM_ADDR 0xA0

void BSP_EEPROM_Init(void)
{
    Driver_GPIO_Write(EEPROM_WP_PORT, EEPROM_WP_PIN, true);
}

bool BSP_EEPROM_Write(uint16_t addr, uint8_t *data, uint16_t len)
{
    Driver_GPIO_Write(EEPROM_WP_PORT, EEPROM_WP_PIN, false);
    bool ok = Driver_EEPROM_WritePage(EEPROM_ADDR, addr, data, len);
    Driver_GPIO_Write(EEPROM_WP_PORT, EEPROM_WP_PIN, true);
    return ok;
}
