#include "main.h"
#include "stdlib.h"
#include "pitches.h"

#define LCD_ADDR 0x3F << 1
#define EEPROM_ADDRESS 0x50 << 1
#define EEPROM_PAGE_SIZE 8
#define DS1307_ADDRESS 0x68 << 1

#define DS1307_SECONDS_REG 0x00
#define DS1307_MINUTES_REG 0x01
#define DS1307_HOURS_REG 0x02
#define DS1307_DAY_REG 0x03
#define DS1307_DATE_REG 0x04
#define DS1307_MONTH_REG 0x05
#define DS1307_YEAR_REG 0x06

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim2;

UART_HandleTypeDef huart1;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM2_Init(void);

void lcd_init();
void set_cursor(uint8_t  x, uint8_t  y);
void sendchar(uint8_t  c);
void lcd_str(uint8_t  str[]);
void lcd_clr();
void send_comand(uint8_t  cmd);
void send_data(uint8_t  cmd);

void UART_Send_Status(const char* message);
void I2C_ScanBus(I2C_HandleTypeDef *hi2c);

HAL_StatusTypeDef EEPROM_Write(uint16_t memAddress, uint8_t* data, uint16_t size);
HAL_StatusTypeDef EEPROM_Read(uint16_t memAddress, uint8_t* buffer, uint16_t size);

uint8_t Decimal_to_BCD(uint8_t val);
uint8_t BCD_to_Decimal(uint8_t val);

HAL_StatusTypeDef DS1307_Write_Register(uint8_t reg, uint8_t data);
HAL_StatusTypeDef DS1307_Read_Register(uint8_t reg, uint8_t *data);
HAL_StatusTypeDef DS1307_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds);
HAL_StatusTypeDef DS1307_Read_Time(uint8_t *timeData);

void SetBuzzerFrequency(uint32_t frequency);
void PlayMelody(void);

void display_time();

volatile uint8_t flag = 0;
volatile uint8_t last_minutes = 255;

void EXTI9_5_IRQHandler(void);

int main(void)
{
	char str[15];
	uint8_t writeData[EEPROM_PAGE_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
	uint8_t readData[EEPROM_PAGE_SIZE] = {0};

	HAL_Init();

	SystemClock_Config();
	MX_GPIO_Init();
	MX_I2C1_Init();
	MX_USART1_UART_Init();
	MX_TIM2_Init();

	lcd_init();
  	//DS1307_Set_Time(17, 52, 0);
  	I2C_ScanBus(&hi2c1);
  	EEPROM_Write(0x0000, writeData, EEPROM_PAGE_SIZE);
  	HAL_Delay(50);
  	EEPROM_Read(0x0000, readData, EEPROM_PAGE_SIZE);
  	sprintf(str, "%0.2x, %0.2x, %0.2x, %0.2x", readData[0], readData[1], readData[2], readData[3]);
  	set_cursor(0, 0);
  	lcd_str("Time:");
  	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);

  	uint8_t after_exti = 0;
  	while (1)
  	{
  		display_time();
  	}


}

void EXTI9_5_IRQHandler(void)
{
    /* Перевірка, чи це дійсно переривання від піну PA1 */
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_9) != RESET)
    {
    	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_9);
    	  			set_cursor(0, 0);
    	  			lcd_str("Setting menu:");
    	  			while(HAL_GPIO_ReadPin(GPIOB, Alarm_button_Pin) == 1){


    	  			}
    }
}



void display_time(){
	uint8_t timeData[3] = {0};
	uint8_t seconds = 0;
	uint8_t minutes = 0;
	uint8_t hours = 0;
	if (DS1307_Read_Time(timeData) == HAL_OK){

	  	        seconds = BCD_to_Decimal(timeData[0] & 0x7F);
	  	        minutes = BCD_to_Decimal(timeData[1]);
	  	        hours = BCD_to_Decimal(timeData[2]);
	  	        char timeStr[9];
	  	        sprintf(timeStr, "%02d:%02d", hours, minutes);

	  	        if (minutes != last_minutes)
	  	        {
	  	            last_minutes = minutes;
	  	            lcd_clr();
	  	            HAL_Delay(10); // Додати затримку після очищення дисплея
	  	            set_cursor(0, 0);
	  	            lcd_str("Time:");
	  	            set_cursor(0, 1);
	  	            lcd_str(timeStr);
	  	        }
	  	        else if (hours == 00 && minutes == 54)
	  	        {
	  	            if (HAL_GPIO_ReadPin(GPIOB, Alarm_button_Pin) == 0)
	  	            {
	  	                PlayMelody();
	  	            }
	  	        }
	 }
}

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

uint8_t Decimal_to_BCD(uint8_t val) {
    return ((val / 10 * 16) + (val % 10));
}

// Функція для перетворення BCD у десяткове число
uint8_t BCD_to_Decimal(uint8_t val) {
    return ((val / 16 * 10) + (val % 16));
}

// Функція запису одного байту до DS1307
HAL_StatusTypeDef DS1307_Write_Register(uint8_t reg, uint8_t data) {
    return HAL_I2C_Mem_Write(&hi2c1, DS1307_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}

// Функція зчитування одного байту з DS1307
HAL_StatusTypeDef DS1307_Read_Register(uint8_t reg, uint8_t *data) {
    return HAL_I2C_Mem_Read(&hi2c1, DS1307_ADDRESS, reg, I2C_MEMADD_SIZE_8BIT, data, 1, HAL_MAX_DELAY);
}

// Функція для встановлення часу
HAL_StatusTypeDef DS1307_Set_Time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    HAL_StatusTypeDef status;

    // Записуємо секунди
    status = DS1307_Write_Register(DS1307_SECONDS_REG, Decimal_to_BCD(seconds) & 0x7F); // Clear CH bit
    if (status != HAL_OK) return status;

    // Записуємо хвилини
    status = DS1307_Write_Register(DS1307_MINUTES_REG, Decimal_to_BCD(minutes));
    if (status != HAL_OK) return status;

    // Записуємо години
    status = DS1307_Write_Register(DS1307_HOURS_REG, Decimal_to_BCD(hours));
    if (status != HAL_OK) return status;

    return HAL_OK;
}

// Функція для зчитування часу
HAL_StatusTypeDef DS1307_Read_Time(uint8_t *timeData) {
    HAL_StatusTypeDef status;

    // Зчитуємо секунди
    status = DS1307_Read_Register(DS1307_SECONDS_REG, &timeData[0]);
    if (status != HAL_OK) return status;

    // Зчитуємо хвилини
    status = DS1307_Read_Register(DS1307_MINUTES_REG, &timeData[1]);
    if (status != HAL_OK) return status;

    // Зчитуємо години
    status = DS1307_Read_Register(DS1307_HOURS_REG, &timeData[2]);
    if (status != HAL_OK) return status;

    return HAL_OK;
}

HAL_StatusTypeDef EEPROM_Write(uint16_t memAddress, uint8_t* data, uint16_t size) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDRESS, memAddress, I2C_MEMADD_SIZE_16BIT, data, size, HAL_MAX_DELAY);

    if (status == HAL_OK) {
        UART_Send_Status("Data written successfully.\r\n");
    } else {
        UART_Send_Status("Failed to write data.\r\n");
    }
    return status;
}

HAL_StatusTypeDef EEPROM_Read(uint16_t memAddress, uint8_t* buffer, uint16_t size) {
    HAL_StatusTypeDef status = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDRESS, memAddress, I2C_MEMADD_SIZE_16BIT, buffer, size, HAL_MAX_DELAY);

    if (status == HAL_OK) {
        UART_Send_Status("Data read successfully.\r\n");
    } else {
        UART_Send_Status("Failed to read data.\r\n");
    }
    return status;
}

void UART_Send_Status(const char* message) {
    HAL_UART_Transmit(&huart1, (uint8_t*)message, strlen(message), HAL_MAX_DELAY);
}

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

void lcd_init()
{
    HAL_Delay(15);
    send_comand(0b11);
    HAL_Delay(5);
    send_comand(0b11);
    HAL_Delay(1);
    send_comand(0b11);
    HAL_Delay(1);
    send_comand(0b10);
    HAL_Delay(1);
    send_comand(0b101000);
    HAL_Delay(1);
    send_comand(0b1100);
    HAL_Delay(1);
    send_comand(0b110);
    HAL_Delay(1);
    send_comand(0b1);
    HAL_Delay(1);
}

void set_cursor(uint8_t  x, uint8_t  y)
{
	uint8_t  addr = (0x40 * y + x) | (1 << 7);
	send_comand(addr);
}

void sendchar(uint8_t  c)
{
    send_data(c);
}

void lcd_str(uint8_t  str[])
{
	  while (*str) send_data(*str++);
}

void lcd_clr()
{
	send_comand(0b1);
    HAL_Delay(1);
}

void send_comand(uint8_t  cmd){
	uint8_t  data[4];
	data[0] = (cmd & 0b11110000) | 0b1100;
	data[1] =(cmd &  0b11110000);
	data[2] =(cmd << 4) | 0b1100;
	data[3] =(cmd << 4) ;

	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data, 4, 100);
}

void send_data(uint8_t  cmd){
	uint8_t  data[4];
	data[0] = (cmd & 0b11110000)|0b1101;
	data[1] =(cmd &  0b11110000)|0b1001;
	data[2] =((cmd << 4))| 0b1101;
	data[3] =((cmd << 4))| 0b1001;

	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data, 4, 100);
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin : Alarm_button_Pin */
  GPIO_InitStruct.Pin = Alarm_button_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Alarm_button_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Alarm_setting_Pin */
  GPIO_InitStruct.Pin = Alarm_setting_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Alarm_setting_GPIO_Port, &GPIO_InitStruct);
  /* Налаштування пріоритету та увімкнення переривань для обох кнопок */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
