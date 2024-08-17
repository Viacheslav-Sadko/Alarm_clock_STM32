#include "Clock.h"
#include "DS1307.h"
#include "main.h"
#include "EEPROM.h"

Mode currentMode = MODE_NORMAL;

volatile uint8_t flag = 0;
volatile uint8_t last_minutes = 255;
volatile uint8_t finish_menu = 0;

uint8_t selectedParameter = 0; // Параметр, який зараз редагується
uint8_t hours = 2, minutes = 25, seconds = 0, day = 13, month = 8, year = 24; // Значення часу і дати
uint8_t alarm_hours = 0, alarm_minutes = 0;
// Максимальні значення для кожного параметра
const uint8_t maxValues[] = {23, 59, 59, 31, 12, 99}; // год, хв, сек, день, міс, рік
uint8_t latestgetCurrentValue = 100;


uint8_t* getSelectedValuePtr() {
    switch (selectedParameter) {
        case 0: return &hours;
        case 1: return &minutes;
        default: return &hours; // За замовчуванням
    }
}


uint8_t* alarm_getSelectedValuePtr() {
    switch (selectedParameter) {
        case 0: return &alarm_hours;
        case 1: return &alarm_minutes;
        default: return &alarm_hours; // За замовчуванням
    }
}

void alarm_increaseValue() {
    uint8_t *valuePtr = alarm_getSelectedValuePtr();
    (*valuePtr)++;

    if (*valuePtr > maxValues[selectedParameter]) {
        *valuePtr = 0;
    }

}

void increaseValue() {
    uint8_t *valuePtr = getSelectedValuePtr();
    (*valuePtr)++;

    if (*valuePtr > maxValues[selectedParameter]) {
        *valuePtr = 0;
    }

}

uint8_t getCurrentValue() {
    switch (selectedParameter) {
        case 0: return hours;
        case 1: return minutes;
        default: return 0;
    }
}

uint8_t alarm_getCurrentValue() {
    switch (selectedParameter) {
        case 0: return alarm_hours;
        case 1: return alarm_minutes;
        default: return 0;
    }
}


void displayCurrentSetting(uint8_t mode) {

	HAL_Delay(80);
    char valueStr[3];

    switch(mode){
    case 0:
		if(getCurrentValue() != latestgetCurrentValue){
				latestgetCurrentValue = getCurrentValue();
				LCD_Clr();
				LCD_Set_Cursor(0, 0);
					LCD_Str("Set ");

					switch (selectedParameter) {
						case 0: LCD_Str("Hour:"); break;
						case 1: LCD_Str("Minute:"); break;
					}
					sprintf(valueStr, "%02d", getCurrentValue());
					LCD_Set_Cursor(0, 1);
				LCD_Str(valueStr);
			}
    break;
    case 1:
    	if(alarm_getCurrentValue() != latestgetCurrentValue){
    					latestgetCurrentValue = alarm_getCurrentValue();
    					LCD_Clr();
    					LCD_Set_Cursor(0, 0);
    						LCD_Str("Set ");

    						switch (selectedParameter) {
    							case 0: LCD_Str("Hour:"); break;
    							case 1: LCD_Str("Minute:"); break;
    						}
    						sprintf(valueStr, "%02d", alarm_getCurrentValue());
    						LCD_Set_Cursor(0, 1);
    					LCD_Str(valueStr);
    				}
    break;
    }



}

DS1307_Set_Time_alarm(uint8_t hours, uint8_t minutes){
	EEPROM_Write(0, &alarm_hours, 1);
	EEPROM_Write(1, &alarm_minutes, 1);
}

void saveSettings(uint8_t mode) {
	if(mode == 0){
		DS1307_Set_Time(hours, minutes);
	}else if(mode == 1){
		DS1307_Set_Time_alarm(alarm_hours, alarm_minutes);
	}

}

void enterSetTimeMode() {
    LCD_Clr();
    LCD_Set_Cursor(0, 0);
    LCD_Str("Set Time/Date");

    while (1) {
        // Відображення параметра
        displayCurrentSetting(0);
        // Обробка натискання кнопок
        if (HAL_GPIO_ReadPin(GPIOB, OK_Pin) == 0) { // Кнопка для зміни значення
            HAL_Delay(30); // Антидребезг
            increaseValue();
        }

        if (HAL_GPIO_ReadPin(GPIOB, SET_Pin) == 0) { // Кнопка для переходу до наступного параметра
            HAL_Delay(40); // Антидребезг
            selectedParameter++;
            if (selectedParameter > 1) {
                selectedParameter = 0;
                saveSettings(0);
                break; // Вихід з режиму налаштування
            }
        }
    }
}

void enterSetAlarmMode(){
	LCD_Clr();
	LCD_Set_Cursor(0, 0);
	LCD_Str("Set alarm:");
	while (1) {
		displayCurrentSetting(1);
		if (HAL_GPIO_ReadPin(GPIOB, OK_Pin) == 0) {
			HAL_Delay(30);
			alarm_increaseValue();
		}
		if (HAL_GPIO_ReadPin(GPIOB, SET_Pin) == 0) {
			HAL_Delay(40);
			selectedParameter++;
			if (selectedParameter > 1) {
				selectedParameter = 0;
				saveSettings(1);
				break;
			}
		}
	}
}


void display_time() {
    uint8_t timeData[7] = {0}; // Містить дані часу і дати: секунди, хвилини, години, день, місяць, рік
    uint8_t seconds = 0;
    uint8_t minutes = 0;
    uint8_t hours = 0;
    uint8_t day = 0;
    uint8_t month = 0;
    uint8_t year = 0;

    if (DS1307_Read_Time(timeData) == HAL_OK) {
        minutes = BCD_to_Decimal(timeData[1]);
        hours = BCD_to_Decimal(timeData[2]);
        char timeStr[5];
        sprintf(timeStr, "%02d:%02d      %02d:%02d",hours, minutes, alarm_hours, alarm_minutes);

        if (minutes != last_minutes || finish_menu == 1) {
            finish_menu = 0;
            last_minutes = minutes;
            LCD_Clr();
            HAL_Delay(10); // Затримка після очищення дисплея
            LCD_Set_Cursor(0, 0);
            LCD_Str("Time:     Alarm:");
            LCD_Set_Cursor(0, 1);
            LCD_Str(timeStr);
            HAL_UART_Transmit(&huart1, timeStr, sizeof(timeStr), HAL_MAX_DELAY);
        }

        // Перевірка для спрацювання будильника
        if (hours == alarm_hours && minutes == alarm_minutes) {
        	PlayMelody();
        	while(HAL_GPIO_ReadPin(GPIOB, OK_Pin) == 0){
        		DS1307_Read_Time(timeData);
        		HAL_Delay(60000 -seconds);
        	}
        }

    }
}
