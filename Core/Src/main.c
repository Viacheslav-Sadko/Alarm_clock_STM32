#include "main.h"
#include "System.h"
#include "LCD.h"
#include "EEPROM.h"
#include "DS1307.h"
#include "UART.h"
#include "Buzzer.h"
#include "I2C_Scan.h"
#include "Clock.h"
#include <stdio.h>

I2C_HandleTypeDef hi2c1;
TIM_HandleTypeDef htim2;
UART_HandleTypeDef huart1;
uint8_t flag_set = 0;
int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  LCD_Init();
  LCD_Set_Cursor(2, 0);
  LCD_Str("Alarm project");
  LCD_Set_Cursor(0, 1);
  LCD_Str("Viacheslav Sadko");
  HAL_Delay(2000);
  uint8_t status = 255;
  uint8_t last_status = 255;
  char buffer[31];
  while (1)
  {
      // Відображаємо час в звичайному режимі
      if (currentMode == MODE_NORMAL) {
          display_time();
      }

      // Якщо натиснуто кнопку OK
      if (HAL_GPIO_ReadPin(GPIOB, OK_Pin) == 0) {
          HAL_Delay(50); // Дебаунс
          while (HAL_GPIO_ReadPin(GPIOB, OK_Pin) == 0); // Очікуємо відпускання кнопки

          switch (currentMode) {
              case MODE_NORMAL:
                  // Перехід до головного меню
                  currentMode = MODE_MENU;
                  flag_set = 0; // Початковий вибір — "Set time"
                  LCD_Clr();
                  LCD_Set_Cursor(0, 0);
                  LCD_Str("* Set time");
                  LCD_Set_Cursor(0, 1);
                  LCD_Str("  Set alarm");
                  break;

              case MODE_MENU:
                  // Перемикання між "Set time" і "Set alarm"
                  if (flag_set == 0) {
                      LCD_Clr();

                      LCD_Set_Cursor(0, 0);
                      LCD_Str("  Set time");
                      LCD_Set_Cursor(0, 1);
                      LCD_Str("* Set alarm");
                      flag_set = 1;
                  } else {
                      LCD_Clr();
                      LCD_Set_Cursor(0, 0);
                      LCD_Str("* Set time");
                      LCD_Set_Cursor(0, 1);
                      LCD_Str("  Set alarm");
                      flag_set = 0;
                  }
                  break;

              case MODE_SET_TIME:
                  // Логіка для налаштування часу
                  enterSetTimeMode();
                  currentMode = MODE_NORMAL; // Повертаємось до нормального режиму після налаштування
                  break;

              case MODE_SET_ALARM:
                  // Логіка для налаштування будильника
                  enterSetAlarmMode();
                  currentMode = MODE_NORMAL; // Повертаємось до нормального режиму після налаштування
                  break;
          }
      }

      // Якщо натиснуто кнопку SET у меню
      if (HAL_GPIO_ReadPin(GPIOB, SET_Pin) == 0 && currentMode == MODE_MENU) {
          HAL_Delay(50); // Дебаунс
          while (HAL_GPIO_ReadPin(GPIOB, SET_Pin) == 0); // Очікуємо відпускання кнопки

          if (flag_set == 0) {
        	  enterSetTimeMode();
        	  currentMode = MODE_NORMAL;

          } else {
        	  enterSetAlarmMode();
        	  currentMode = MODE_NORMAL;
          }
      }
  }

}




void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif
