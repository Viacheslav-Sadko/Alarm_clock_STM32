#include <stdint.h>
#include "stm32f4xx_hal.h"

#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_

#define EEPROM_ADDRESS 0x50 << 1
#define EEPROM_PAGE_SIZE 8

HAL_StatusTypeDef EEPROM_Write(uint16_t memAddress, uint8_t* data, uint16_t size);
HAL_StatusTypeDef EEPROM_Read(uint16_t memAddress, uint8_t* buffer, uint16_t size);

#endif
