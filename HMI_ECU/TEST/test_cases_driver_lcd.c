/**
 * @file    test_cases_driver_lcd.c
 * @brief   LCD Driver Unit Tests Implementation - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Tests LCD functionality via I2C interface.
 *          These are smoke tests - LCD output verified visually.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_driver_lcd.h"
#include "test_config.h"
#include "test_log.h"
#include "../HAL/LCD.h"
#include "../MCAL/Delay.h"

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define LCD_SUITE         "LCD"

/*===========================================================================*/
/*                           TEST IMPLEMENTATIONS                            */
/*===========================================================================*/

boolean Test_LCD_Init_Completes(void)
{
    boolean result = TRUE;
    
    /* Execute - LCD_Init should complete without hanging */
    /* Note: If I2C bus has issues, this may hang */
    LCD_Init();
    
    /* If we got here, init completed */
    /* Can't verify internal state without reading LCD */
    
    return result;
}

boolean Test_LCD_Clear_Works(void)
{
    boolean result = TRUE;
    
    /* Ensure LCD is initialized */
    LCD_Init();
    
    /* Execute - should complete without error */
    LCD_Clear();
    
    /* Brief delay for LCD command processing */
    Delay_ms(5u);
    
    /* Visual verification required */
    
    return result;
}

boolean Test_LCD_SetCursor_Works(void)
{
    boolean result = TRUE;
    
    /* Ensure LCD is initialized */
    LCD_Init();
    LCD_Clear();
    
    /* Execute - set cursor to various positions */
    LCD_SetCursor(0u, 0u);  /* Row 0, Col 0 */
    LCD_SendChar('A');
    
    LCD_SetCursor(1u, 0u);  /* Row 1, Col 0 */
    LCD_SendChar('B');
    
    LCD_SetCursor(0u, 15u); /* Row 0, Col 15 */
    LCD_SendChar('C');
    
    LCD_SetCursor(1u, 15u); /* Row 1, Col 15 */
    LCD_SendChar('D');
    
    /* If we got here without hanging, consider passed */
    /* Visual verification: should show A,B,C,D at corners */
    
    return result;
}

boolean Test_LCD_SendString_SmokeTest(void)
{
    boolean result = TRUE;
    
    /* Ensure LCD is initialized */
    LCD_Init();
    LCD_Clear();
    
    /* Execute - write test pattern */
    LCD_SetCursor(0u, 0u);
    LCD_SendString("TEST FRAMEWORK");
    
    LCD_SetCursor(1u, 0u);
    LCD_SendString("HMI ECU v1.0");
    
    /* Brief delay for visual observation */
    Delay_ms(100u);
    
    /* If we got here, string output worked */
    
    return result;
}

void Test_LCD_RunAll(void)
{
    TestLog_SuiteStart(LCD_SUITE);
    
    TEST_RUN(LCD_SUITE, "Init_Completes", Test_LCD_Init_Completes);
    TEST_RUN(LCD_SUITE, "Clear_Works", Test_LCD_Clear_Works);
    TEST_RUN(LCD_SUITE, "SetCursor_Works", Test_LCD_SetCursor_Works);
    TEST_RUN(LCD_SUITE, "SendString_SmokeTest", Test_LCD_SendString_SmokeTest);
    
    TestLog_SuiteEnd(LCD_SUITE);
}
