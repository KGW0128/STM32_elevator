/*
 * I2C_LCD.c
 *
 *  Created on: Jan 14, 2025
 *      Author: user
 */

#include "I2C_LCD.h"


extern I2C_HandleTypeDef hi2c1;


void LCD_Command(uint8_t command)
{

  uint8_t high_nibble, low_nibble;
  uint8_t i2c_buffer[4];
  high_nibble = command & 0xf0;
  low_nibble = (command << 4) & 0xf0;
  i2c_buffer[0] = high_nibble | 0x04 | 0x08;
  i2c_buffer[1] = high_nibble | 0x00 | 0x08;

  i2c_buffer[2] = low_nibble  | 0x04 | 0x08;
  i2c_buffer[3] = low_nibble  | 0x00 | 0x08;

  while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS, i2c_buffer, 4, 100) != HAL_OK);

}


//1byte write
void LCD_Data(uint8_t data)
{

  uint8_t high_nibble, low_nibble;
  uint8_t i2c_buffer[4];
  high_nibble = data & 0xf0;
  low_nibble = (data << 4) & 0xf0;
  i2c_buffer[0] = high_nibble | 0x05 | 0x08;
  i2c_buffer[1] = high_nibble | 0x01 | 0x08;

  i2c_buffer[2] = low_nibble  | 0x05 | 0x08;
  i2c_buffer[3] = low_nibble  | 0x01 | 0x08;

  while(HAL_I2C_Master_Transmit(&hi2c1, I2C_LCD_ADDRESS, i2c_buffer, 4, 100) != HAL_OK);

}


void LCD_Init(void)
{
  HAL_Delay(50);
  LCD_Command(0x33);
  HAL_Delay(5);
  LCD_Command(0x32);
  HAL_Delay(5);
  LCD_Command(0x28);
  HAL_Delay(5);
  LCD_Command(DISPLAY_ON);
  HAL_Delay(5);
  LCD_Command(0x06);
  HAL_Delay(5);
  LCD_Command(CLEAR_DISPLAY);
  HAL_Delay(2);
}


void LCD_String(char *str)
{
  while(*str)LCD_Data(*str++);
}


void moveCusor(uint8_t row, uint8_t col)
{
  LCD_Command(0x80 | row<<6 | col);
}



