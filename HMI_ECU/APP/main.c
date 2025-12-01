#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "../HAL/LCD.h"

int main(void)
{
    LCD_Init();

    LCD_SetCursor(0, 0);
    LCD_SendString("LCD I2C OK");

    LCD_SetCursor(1, 0);
    LCD_SendString("Enter Password");

    while (1)
    {
        // later we will add keypad here
    }
}
