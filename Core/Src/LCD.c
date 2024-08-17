#include "LCD.h"
#include "main.h"

void LCD_Init()
{
    HAL_Delay(15);
    LCD_Send_Comand(0b11);
    HAL_Delay(5);
    LCD_Send_Comand(0b11);
    HAL_Delay(1);
    LCD_Send_Comand(0b11);
    HAL_Delay(1);
    LCD_Send_Comand(0b10);
    HAL_Delay(1);
    LCD_Send_Comand(0b101000);
    HAL_Delay(1);
    LCD_Send_Comand(0b1100);
    HAL_Delay(1);
    LCD_Send_Comand(0b110);
    HAL_Delay(1);
    LCD_Send_Comand(0b1);
    HAL_Delay(1);
}

void LCD_Set_Cursor(uint8_t  x, uint8_t  y)
{
	uint8_t  addr = (0x40 * y + x) | (1 << 7);
	LCD_Send_Comand(addr);
}

void LCD_Send_Char(uint8_t  c)
{
    LCD_Send_Data(c);
}

void LCD_Str(uint8_t  str[])
{
	  while (*str) LCD_Send_Data(*str++);
}

void LCD_Clr()
{
	LCD_Send_Comand(0b1);
    HAL_Delay(10);
}

void LCD_Send_Comand(uint8_t  Command){
	uint8_t  Data[4];
	Data[0] = (Command & 0b11110000) | 0b1100;
	Data[1] =(Command &  0b11110000);
	Data[2] =(Command << 4) | 0b1100;
	Data[3] =(Command << 4) ;

	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, Data, 4, 100);
}

void LCD_Send_Data(uint8_t  Command){
	uint8_t  Data[4];
	Data[0] = (Command & 0b11110000)|0b1101;
	Data[1] =(Command &  0b11110000)|0b1001;
	Data[2] =((Command << 4))| 0b1101;
	Data[3] =((Command << 4))| 0b1001;

	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, Data, 4, 100);
}
