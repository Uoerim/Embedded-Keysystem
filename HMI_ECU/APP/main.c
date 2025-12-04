#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "../HAL/LCD.h"
#include "../MCAL/ADC.h"

static void delay_small(volatile uint32_t t)
{
    while (t--) {}
}

uint16_t ADC_ReadAverage(uint8_t samples)
{
    uint32_t sum = 0;
    for (uint8_t i = 0; i < samples; i++)
    {
        sum += ADC_Read();
    }
    return (uint16_t)(sum / samples);
}

int main(void)
{
    LCD_Init();
    ADC_Init();

    while (1)
    {
        uint16_t val = ADC_ReadAverage(16);   // smooth reading

        if (val < 20) val = 0;                 // remove low-end noise

        LCD_Clear();
        LCD_SetCursor(0,0);
        LCD_SendString("Pot value:");

        LCD_SetCursor(1,0);

        // print value
        char buf[6];
        sprintf(buf, "%u", val);
        LCD_SendString(buf);

        delay_small(200000);
    }
}
