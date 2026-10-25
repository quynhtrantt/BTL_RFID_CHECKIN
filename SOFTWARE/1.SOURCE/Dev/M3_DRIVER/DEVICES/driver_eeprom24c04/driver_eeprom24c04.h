#ifndef M3_DRIVER_DEVICES_DRIVER_EEPROM24C04_DRIVER_EEPROM24C04_H_
#define M3_DRIVER_DEVICES_DRIVER_EEPROM24C04_DRIVER_EEPROM24C04_H_

#include <stdint.h>
#include <stdbool.h>

bool Driver_EEPROM_WritePage(uint8_t dev_addr, uint16_t mem_addr, uint8_t *data, uint16_t len);
bool Driver_EEPROM_Read(uint8_t dev_addr, uint16_t mem_addr, uint8_t *data, uint16_t len);

#endif /* M3_DRIVER_DEVICES_DRIVER_EEPROM24C04_DRIVER_EEPROM24C04_H_ */
