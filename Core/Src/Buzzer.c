#include "Buzzer.h"

void SetBuzzerFrequency(uint32_t frequency) {
    if (frequency == 0) {
        HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_1);
    } else {
        // Обчислення значення ARR для заданої частоти
        uint32_t arrValue = (SystemCoreClock / frequency) - 1;
        __HAL_TIM_SET_AUTORELOAD(&htim2, arrValue);
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
    }
}

void PlayMelody(void) {
	int melody[] = {261, 293, 329, 349, 392, 440, 493, 523, 523, 493, 440, 392, 349, 329, 293, 261};
	int durations[] = {8, 8, 8, 8, 4, 4, 8, 8, 8, 8, 4, 4, 8, 8, 8, 8};


    for (int i = 0; i < sizeof(melody) / sizeof(melody[0]); i++) {

        SetBuzzerFrequency(melody[i]);
        HAL_Delay(durations[i]);  // Затримка для відтворення ноти
        SetBuzzerFrequency(0);  // Зупинка зумера
        HAL_Delay(100);  // Затримка між нотами
    }
}
