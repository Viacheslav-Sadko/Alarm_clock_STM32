#include "Clock.h"

SystemMode currentMode = MODE_NORMAL;

volatile uint8_t flag = 0;
volatile uint8_t last_minutes = 255;
volatile uint8_t finish_menu = 0;

uint8_t selectedParameter = 0; // Параметр, який зараз редагується
uint8_t hours = 2, minutes = 25, seconds = 0, day = 13, month = 8, year = 24; // Значення часу і дати
uint8_t alarm_hours = 12, alarm_minutes = 0;
// Максимальні значення для кожного параметра
const uint8_t maxValues[] = {23, 59, 59, 31, 12, 99}; // год, хв, сек, день, міс, рік
uint8_t latestgetCurrentValue = 100;
volatile uint8_t HOURS = 0;
volatile uint8_t MINUTES = 0;

uint8_t* getSelectedValuePtr() {
    switch (selectedParameter) {
        case 0: return &hours;
        case 1: return &minutes;
        case 2: return &seconds;
        case 3: return &day;
        case 4: return &month;
        case 5: return &year;
        default: return &hours; // За замовчуванням
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
        case 2: return seconds;
        case 3: return day;
        case 4: return month;
        case 5: return year;
        default: return 0;
    }
}

void displayCurrentSetting() {


    char valueStr[3];


    if(getCurrentValue() != latestgetCurrentValue){
    	latestgetCurrentValue = getCurrentValue();
    	LCD_Clr();
    	LCD_Set_Cursor(0, 0);
    	    LCD_Str("Set ");

    	    switch (selectedParameter) {
    	        case 0: LCD_Str("Hour:"); break;
    	        case 1: LCD_Str("Minute:"); break;
    	        case 2: LCD_Str("Second:"); break;
    	        case 3: LCD_Str("Day:"); break;
    	        case 4: LCD_Str("Month:"); break;
    	        case 5: LCD_Str("Year:"); break;
    	    }
    	    sprintf(valueStr, "%02d", getCurrentValue());
    	    LCD_Set_Cursor(0, 1);
    	LCD_Str(valueStr);
    }


}

void saveSettings() {
    // Наприклад, зберігання налаштувань у DS1307
    DS1307_Set_Time(hours, minutes, seconds);
    DS1307_Set_Date(day, month, year);
}

void enterSetTimeMode() {
    LCD_Clr();
    LCD_Set_Cursor(0, 0);
    LCD_Str("Set Time/Date");

    while (1) {
        // Відображення параметра
        displayCurrentSetting();


        // Обробка натискання кнопок
        if (HAL_GPIO_ReadPin(GPIOB, OK_Pin) == 0) { // Кнопка для зміни значення
            HAL_Delay(200); // Антидребезг
            increaseValue();


        }

        if (HAL_GPIO_ReadPin(GPIOB, SET_Pin) == 0) { // Кнопка для переходу до наступного параметра
            HAL_Delay(200); // Антидребезг
            selectedParameter++;
            if (selectedParameter > 5) {
                selectedParameter = 0;
                saveSettings();
                break; // Вихід з режиму налаштування
            }
        }
    }
}

void enterSetAlarmMode(){
	LCD_Clr();
	LCD_Set_Cursor(0, 0);
	LCD_Str("Set alarm:");

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

        // Розпаковка даних
        seconds = BCD_to_Decimal(timeData[0] & 0x7F); // Очистка CH біт
        minutes = BCD_to_Decimal(timeData[1]);
        hours = BCD_to_Decimal(timeData[2]);
        day = BCD_to_Decimal(timeData[3]);
        month = BCD_to_Decimal(timeData[4]);
        year = BCD_to_Decimal(timeData[5]);

        // Формування рядка часу і дати
        char timeStr[20];
        sprintf(timeStr, "%02d/%02d/%02d %02d:%02d", day, month, year, hours, minutes);

        // Оновлення екрану, якщо потрібно
        if (minutes != last_minutes || finish_menu == 1) {
            finish_menu = 0;
            last_minutes = minutes;
            LCD_Clr();
            HAL_Delay(10); // Затримка після очищення дисплея
            LCD_Set_Cursor(0, 0);
            LCD_Str("Date & Time:");
            LCD_Set_Cursor(0, 1);
            LCD_Str(timeStr);
        }

        // Перевірка для спрацювання будильника
        if (hours == 9 && minutes == 30) {
        	while(HAL_GPIO_ReadPin(GPIOB, OK_Pin) != 0){
        		PlayMelody();
        	}
        }
    }
}
