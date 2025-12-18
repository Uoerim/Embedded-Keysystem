#ifndef LCD_H_
#define LCD_H_

#include <stdint.h>
#include "../Common/Std_Types.h"

void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);

void LCD_SendChar(char c);
void LCD_SendString(const char *str);

#endif /* LCD_H_ */
