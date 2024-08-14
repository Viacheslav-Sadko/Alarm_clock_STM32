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

int main(void)
{
  HAL_Init();

  SystemClock_Config();

  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  LCD_Init();
  LCD_Str("Hello");
  uint8_t status = 255;
  uint8_t last_status = 255;
  char buffer[17];
  while (1)
  {
	  status = DS1307_Read_Register(DS1307_SECONDS_REG, 0);
	  if(last_status != status){
		  last_status = status;
		  sprintf(buffer, "The status is : %i", last_status);
		  HAL_UART_Transmit(&huart1, buffer, strlen(buffer), HAL_MAX_DELAY);
		  //UART_Send_Status(sprintf(buffer, "The status is : %i", last_status));
		  UART_Send_Status("\r");
		  UART_Send_Status("\n");
	  }

	  if (HAL_GPIO_ReadPin(GPIOB, OK_Pin) == 0) {
	    		            HAL_Delay(50);
	    		            if (HAL_GPIO_ReadPin(GPIOB, OK_Pin) == 0) {

	    		                if (currentMode == MODE_NORMAL) {
	    		                    currentMode = MODE_SET_TIME;
	    		                    enterSetTimeMode();
	    		                } else if (currentMode == MODE_SET_TIME) {
	    		                    currentMode = MODE_SET_ALARM;
	    		                    enterSetAlarmMode();
	    		                    finish_menu++;
	    		                } else if (currentMode == MODE_SET_ALARM) {
	    		                    currentMode = MODE_NORMAL;
	    		                    display_time();
	    		                }


	    		                while (HAL_GPIO_ReadPin(GPIOB, OK_Pin) == 0) {
	    		                    HAL_Delay(10);
	    		                }
	    		            }
	    		        }
	    		if (currentMode == MODE_NORMAL) {
	    		        display_time();
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
