#include "EEPROM.h"
#include "main.h"

HAL_StatusTypeDef EEPROM_Write(uint16_t memAddress, uint8_t* data, uint16_t size) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, memAddress, I2C_MEMADD_SIZE_16BIT, data, size, HAL_MAX_DELAY);

    if (status == HAL_OK) {
        UART_Send_Status("Data written successfully.\r\n");
    } else {
        UART_Send_Status("Failed to write data.\r\n");
    }
    return status;
}

HAL_StatusTypeDef EEPROM_Read(uint16_t memAddress, uint8_t* buffer, uint16_t size) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, memAddress, I2C_MEMADD_SIZE_16BIT, buffer, size, HAL_MAX_DELAY);

    if (status == HAL_OK) {
        UART_Send_Status("Data read successfully.\r\n");
    } else {
        UART_Send_Status("Failed to read data.\r\n");
    }
    return status;
}
