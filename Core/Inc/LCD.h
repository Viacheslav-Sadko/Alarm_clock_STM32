#include <stdint.h>
#include "stm32f4xx_hal.h"

#ifndef INC_LCD_H_
#define INC_LCD_H_

#define LCD_ADDR 0x3F << 1

void LCD_Init();
void LCD_Set_Cursor(uint8_t  x, uint8_t  y);
void LCD_Send_Char(uint8_t  c);
void LCD_Str(uint8_t  str[]);
void LCD_Clr();
void LCD_Send_Comand(uint8_t  Command);
void LCD_Send_Data(uint8_t  Command);

#endif
