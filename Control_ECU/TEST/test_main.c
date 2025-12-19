/**
 * @file    test_main.c
 * @brief   Test Main Entry Point - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Main entry point for TEST build configuration.
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

#include "../MCAL/Delay.h"
#include "../MCAL/UART.h"
#include "../HAL/RGB_LED.h"

/*===========================================================================*/
/*                           MAIN FUNCTION                                   */
/*===========================================================================*/

int main(void)
{
    uint16_t failedTests;
    const TestStats_t *stats;
    
    /* Initialize hardware */
    Delay_Init_16MHz();
    
    /* Initialize RGB LED for status indication */
    RGB_LED_Init();
    RGB_LED_SetColor(RGB_BLUE);  /* Blue = tests running */
    
    /* Initialize test framework */
    (void)TestRunner_Init();
    
    /* Run all tests */
    failedTests = TestRunner_RunAll();
    
    /* Get final statistics */
    stats = TestLog_GetStats();
    
    /* Set LED based on results */
    if (failedTests == 0u)
    {
        RGB_LED_SetColor(RGB_GREEN);  /* Green = all passed */
    }
    else
    {
        RGB_LED_SetColor(RGB_RED);    /* Red = failures */
    }
    
    (void)stats;
    
    /* Infinite loop - tests complete */
    for (;;)
    {
        Delay_ms(1000u);
    }
    
    return 0;
}
