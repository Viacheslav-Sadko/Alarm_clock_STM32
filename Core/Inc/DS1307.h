#include <stdint.h>
#include "stm32f4xx_hal.h"

#ifndef INC_DS1307_H_
#define INC_DS1307_H_

#define DS1307_ADDRESS 0x68 << 1

#define DS1307_SECONDS_REG 0x00
#define DS1307_MINUTES_REG 0x01
#define DS1307_HOURS_REG 0x02
#define DS1307_DAY_REG 0x03
#define DS1307_DATE_REG 0x04
#define DS1307_MONTH_REG 0x05
#define DS1307_YEAR_REG 0x06

HAL_StatusTypeDef DS1307_Write_Register(uint8_t reg, uint8_t data);
HAL_StatusTypeDef DS1307_Read_Register(uint8_t reg, uint8_t *data);
HAL_StatusTypeDef DS1307_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds);
HAL_StatusTypeDef DS1307_Set_Date(uint8_t day, uint8_t month, uint8_t year);
HAL_StatusTypeDef DS1307_Read_Time(uint8_t *timeData);

uint8_t Decimal_to_BCD(uint8_t val);
uint8_t BCD_to_Decimal(uint8_t val);

#endif
