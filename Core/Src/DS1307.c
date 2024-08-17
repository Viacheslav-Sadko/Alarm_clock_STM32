#include "DS1307.h"
#include "main.h"


uint8_t Decimal_to_BCD(uint8_t val) {
    return ((val / 10 * 16) + (val % 10));
}

// Функція для перетворення BCD у десяткове число
uint8_t BCD_to_Decimal(uint8_t val) {
    return ((val / 16 * 10) + (val % 16));
}

HAL_StatusTypeDef DS1307_Write_Register(uint8_t reg, uint8_t data) {
    return HAL_I2C_Mem_Write(&hi2c1, DS1307_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}

// Функція зчитування одного байту з DS1307
HAL_StatusTypeDef DS1307_Read_Register(uint8_t reg, uint8_t *data) {
	HAL_Delay(50);
    return HAL_I2C_Mem_Read(&hi2c1, DS1307_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

// Функція для встановлення часу
HAL_StatusTypeDef DS1307_Set_Time(uint8_t hours, uint8_t minutes) {
    HAL_StatusTypeDef status;


    // Записуємо хвилини
    HAL_Delay(20);
    status = DS1307_Write_Register(DS1307_MINUTES_REG, Decimal_to_BCD(minutes));
    if (status != HAL_OK) return status;

    // Записуємо години
    HAL_Delay(20);
    status = DS1307_Write_Register(DS1307_HOURS_REG, Decimal_to_BCD(hours));
    if (status != HAL_OK) return status;

    return HAL_OK;
}

HAL_StatusTypeDef DS1307_Set_Date(uint8_t day, uint8_t month, uint8_t year) {
    HAL_StatusTypeDef status;

    // Записуємо день
    HAL_Delay(50);
    status = DS1307_Write_Register(DS1307_DAY_REG, Decimal_to_BCD(day));
    if (status != HAL_OK) return status;

    // Записуємо місяць
    HAL_Delay(50);
    status = DS1307_Write_Register(DS1307_MONTH_REG, Decimal_to_BCD(month));
    if (status != HAL_OK) return status;

    // Записуємо рік
    HAL_Delay(50);
    status = DS1307_Write_Register(DS1307_YEAR_REG, Decimal_to_BCD(year));
    if (status != HAL_OK) return status;

    return HAL_OK;
}


// Функція для зчитування часу
HAL_StatusTypeDef DS1307_Read_Time(uint8_t *timeData) {
    HAL_StatusTypeDef status;

    // Зчитуємо секунди
    HAL_Delay(50);
    status = DS1307_Read_Register(DS1307_SECONDS_REG, &timeData[0]);
    if (status != HAL_OK) return status;

    // Зчитуємо хвилини
    HAL_Delay(50);
    status = DS1307_Read_Register(DS1307_MINUTES_REG, &timeData[1]);
    if (status != HAL_OK) return status;

    // Зчитуємо години
    HAL_Delay(50);
    status = DS1307_Read_Register(DS1307_HOURS_REG, &timeData[2]);
    if (status != HAL_OK) return status;
    HAL_Delay(50);

    status = DS1307_Read_Register(DS1307_DAY_REG, &timeData[3]);
    if (status != HAL_OK) return status;
    HAL_Delay(50);
    status = DS1307_Read_Register(DS1307_MONTH_REG, &timeData[4]);
    if (status != HAL_OK) return status;
    HAL_Delay(50);
    status = DS1307_Read_Register(DS1307_YEAR_REG, &timeData[5]);
    if (status != HAL_OK) return status;

    return HAL_OK;
}
