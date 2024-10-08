#include "main.h"

#ifndef INC_CLOCK_H_
#define INC_CLOCK_H_

uint8_t* getSelectedValuePtr();
void increaseValue();
uint8_t getCurrentValue();
void displayCurrentSetting(uint8_t mode);
void saveSettings(uint8_t mode);

uint8_t* alarm_getSelectedValuePtr();
void alarm_increaseValue();
uint8_t alarm_getCurrentValue();
extern uint8_t alarm_hours, alarm_minutes;

void display_time();
void enterSetTimeMode();
void enterSetAlarmMode();

typedef enum {
    MODE_NORMAL,
    MODE_MENU,
    MODE_SET_TIME,
    MODE_SET_ALARM,
    MODE_ALARM_SETTINGS
} Mode;

extern Mode currentMode;

extern volatile uint8_t flag;
extern volatile uint8_t last_minutes;
extern volatile uint8_t finish_menu;

#endif /* INC_CLOCK_H_ */
