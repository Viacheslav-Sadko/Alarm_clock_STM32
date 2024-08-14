#include "main.h"

#ifndef INC_CLOCK_H_
#define INC_CLOCK_H_

uint8_t* getSelectedValuePtr();
void increaseValue();
uint8_t getCurrentValue();
void displayCurrentSetting();
void saveSettings();

void display_time();
void enterSetTimeMode();
void enterSetAlarmMode();

typedef enum {
    MODE_NORMAL,
    MODE_SET_TIME,
    MODE_SET_ALARM
} SystemMode;

extern SystemMode currentMode;

extern volatile uint8_t flag;
extern volatile uint8_t last_minutes;
extern volatile uint8_t finish_menu;

#endif /* INC_CLOCK_H_ */
