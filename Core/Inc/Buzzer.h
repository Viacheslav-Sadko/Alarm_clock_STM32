#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "main.h"

#ifndef INC_BUZZER_H_
#define INC_BUZZER_H_

void SetBuzzerFrequency(uint32_t frequency);
void PlayMelody(void);

#endif
