/*
 * I2C_LCD.h
 *
 *  Created on: Jan 14, 2025
 *      Author: user
 */

#ifndef INC_I2C_LCD_H_
#define INC_I2C_LCD_H_

#include "main.h"

#define I2C_LCD_ADDRESS   (0x27 << 1)
#define BACKLIGHT_ON      0x08
#define DISPLAY_ON        0x0C
#define DISPLAY_OFF       0x08
#define CLEAR_DISPLAY     0x01
#define RETURN_HONE       0x02


void LCD_Command(uint8_t command);
void LCD_Data(uint8_t data);
void LCD_Init(void);
void LCD_String(char *str);
void moveCusor(uint8_t row, uint8_t col);


#endif /* INC_I2C_LCD_H_ */
