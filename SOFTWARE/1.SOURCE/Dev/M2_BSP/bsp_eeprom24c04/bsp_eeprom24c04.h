#ifndef M2_BSP_BSP_EEPROM24C04_BSP_EEPROM24C04_H_
#define M2_BSP_BSP_EEPROM24C04_BSP_EEPROM24C04_H_

#include <stdint.h>
#include <stdbool.h>

void BSP_EEPROM_Init(void);
bool BSP_EEPROM_Write(uint16_t addr, uint8_t *data, uint16_t len);

#endif /* M2_BSP_BSP_EEPROM24C04_BSP_EEPROM24C04_H_ */
