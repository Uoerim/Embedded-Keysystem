#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "../HAL/LCD.h"
#include "../HAL/Keypad.h"
#include "../HAL/Buzzer.h"



static void delay_small(volatile uint32_t t)
{
    while (t--) { }
}

int main(void)
{
    LCD_Init();
    Keypad_Init();
    Buzzer_Init();        // <-- from Buzzer.c

    LCD_Clear();
    LCD_SetCursor(0, 0);
    LCD_SendString("Press any key");

    while (1)
    {
        char k = Keypad_GetKey();

        Buzzer_BeepShort();   // <-- from Buzzer.c

        LCD_Clear();
        LCD_SetCursor(0, 0);
        LCD_SendString("You pressed:");
        LCD_SetCursor(1, 0);
        LCD_SendChar(k);

        delay_small(300000);

        LCD_Clear();
        LCD_SetCursor(0, 0);
        LCD_SendString("Press any key");
    }
}
