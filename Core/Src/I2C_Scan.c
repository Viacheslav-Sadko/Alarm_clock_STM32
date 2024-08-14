#include "I2C_Scan.h"

void I2C_ScanBus(I2C_HandleTypeDef *hi2c) {
    UART_Send_Status("Scanning I2C bus...\r\n");

    for (uint8_t address = 1; address < 128; address++) {
        // Зсув адреси на один біт вліво для створення 8-бітної адреси
        uint8_t i2cAddress = address << 1;

        // Перевіряємо, чи пристрій відповідає на цю адресу
        HAL_StatusTypeDef result = HAL_I2C_IsDeviceReady(hi2c, i2cAddress, 1, 10);

        if (result == HAL_OK) {
            // Пристрій знайдено
            char msg[64];
            snprintf(msg, sizeof(msg), "Device found at address 0x%02X\r\n", address);
            UART_Send_Status(msg);
        }
    }

    UART_Send_Status("I2C scan completed.\r\n");
}
