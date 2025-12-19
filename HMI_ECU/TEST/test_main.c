/**
 * @file    test_main.c
 * @brief   Test Main Entry Point - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Main entry point for TEST build configuration.
 *          Replace APP/main.c with this file in TEST build.
 * 
 * @build   In IAR:
 *          1. Create new configuration: TEST
 *          2. Exclude APP/main.c from TEST config
 *          3. Include all TEST/*.c files in TEST config
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_runner.h"
#include "test_log.h"
#include "test_config.h"

/* MCAL includes for hardware init */
#include "../MCAL/Delay.h"
#include "../MCAL/UART.h"
#include "../HAL/LCD.h"

/*===========================================================================*/
/*                           MAIN FUNCTION                                   */
/*===========================================================================*/

/**
 * @brief Test main entry point
 * @return Does not return (embedded main loop)
 */
int main(void)
{
    uint16_t failedTests;
    const TestStats_t *stats;
    
    /* Initialize hardware */
    Delay_Init_16MHz();
    
    /* Initialize test framework */
    (void)TestRunner_Init();
    
    /* Display test start on LCD */
    LCD_Init();
    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString("TEST MODE");
    LCD_SetCursor(1u, 0u);
    LCD_SendString("Running...");
    
    /* Run all tests */
    failedTests = TestRunner_RunAll();
    
    /* Get final statistics */
    stats = TestLog_GetStats();
    
    /* Display results on LCD */
    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    
    if (failedTests == 0u)
    {
        LCD_SendString("ALL TESTS PASS");
    }
    else
    {
        LCD_SendString("TESTS FAILED:");
        LCD_SetCursor(1u, 0u);
        
        /* Display fail count */
        {
            char numStr[6];
            uint16_t temp = failedTests;
            uint8_t idx = 0u;
            
            if (temp == 0u)
            {
                numStr[0] = '0';
                idx = 1u;
            }
            else
            {
                while (temp > 0u)
                {
                    numStr[idx] = (char)('0' + (temp % 10u));
                    temp /= 10u;
                    idx++;
                }
            }
            numStr[idx] = '\0';
            
            /* Reverse string */
            {
                uint8_t i;
                uint8_t j = idx - 1u;
                char c;
                
                for (i = 0u; i < (idx / 2u); i++)
                {
                    c = numStr[i];
                    numStr[i] = numStr[j - i];
                    numStr[j - i] = c;
                }
            }
            
            LCD_SendString(numStr);
            LCD_SendString(" failed");
        }
    }
    
    LCD_SetCursor(1u, 0u);
    {
        char passStr[6];
        char totalStr[6];
        uint16_t temp;
        uint8_t idx;
        
        /* Convert passed to string */
        temp = stats->passed;
        idx = 0u;
        
        if (temp == 0u)
        {
            passStr[0] = '0';
            idx = 1u;
        }
        else
        {
            while (temp > 0u)
            {
                passStr[idx] = (char)('0' + (temp % 10u));
                temp /= 10u;
                idx++;
            }
        }
        passStr[idx] = '\0';
        
        /* Reverse */
        {
            uint8_t i;
            uint8_t j = idx - 1u;
            char c;
            
            for (i = 0u; i < (idx / 2u); i++)
            {
                c = passStr[i];
                passStr[i] = passStr[j - i];
                passStr[j - i] = c;
            }
        }
        
        /* Convert total to string */
        temp = stats->total_tests;
        idx = 0u;
        
        if (temp == 0u)
        {
            totalStr[0] = '0';
            idx = 1u;
        }
        else
        {
            while (temp > 0u)
            {
                totalStr[idx] = (char)('0' + (temp % 10u));
                temp /= 10u;
                idx++;
            }
        }
        totalStr[idx] = '\0';
        
        /* Reverse */
        {
            uint8_t i;
            uint8_t j = idx - 1u;
            char c;
            
            for (i = 0u; i < (idx / 2u); i++)
            {
                c = totalStr[i];
                totalStr[i] = totalStr[j - i];
                totalStr[j - i] = c;
            }
        }
        
        LCD_SendString(passStr);
        LCD_SendString("/");
        LCD_SendString(totalStr);
        LCD_SendString(" pass");
    }
    
    /* Infinite loop - tests complete */
    for (;;)
    {
        /* Blink LED to indicate test completion */
        Delay_ms(500u);
    }
    
    /* Never reached */
    return 0;
}
