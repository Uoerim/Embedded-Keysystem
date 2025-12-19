/**
 * @file    test_cases_driver_rgb.c
 * @brief   RGB LED Driver Unit Tests Implementation - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Tests RGB LED register configuration and color output.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_driver_rgb.h"
#include "test_config.h"
#include "test_log.h"
#include "../HAL/RGB_LED.h"

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define RGB_SUITE         "RGB_LED"

/*===========================================================================*/
/*                           TEST IMPLEMENTATIONS                            */
/*===========================================================================*/

uint8_t Test_RGB_Init_SetsRegisters(void)
{
    uint8_t result = TRUE;
    uint32_t dirBits;
    uint32_t denBits;
    
    /* Execute */
    RGB_LED_Init();
    
    /* Verify DIR register - PC5/6/7 should be outputs */
    dirBits = GPIO_PORTC_DIR_R & RGB_ALL_PINS;
    if (dirBits != RGB_ALL_PINS)
    {
        result = FALSE;
    }
    
    /* Verify DEN register - PC5/6/7 should be enabled */
    denBits = GPIO_PORTC_DEN_R & RGB_ALL_PINS;
    if (denBits != RGB_ALL_PINS)
    {
        result = FALSE;
    }
    
    return result;
}

uint8_t Test_RGB_SetColor_Red(void)
{
    uint8_t result = TRUE;
    uint32_t dataBits;
    
    /* Setup */
    RGB_LED_Init();
    
    /* Execute */
    RGB_LED_SetColor(RGB_RED);
    
    /* Verify PC5=1 (Red), PC6=0, PC7=0 */
    dataBits = GPIO_PORTC_DATA_R & RGB_ALL_PINS;
    
    if ((dataBits & RGB_RED_PIN) == 0u)
    {
        result = FALSE;  /* Red should be ON */
    }
    if ((dataBits & RGB_GREEN_PIN) != 0u)
    {
        result = FALSE;  /* Green should be OFF */
    }
    if ((dataBits & RGB_BLUE_PIN) != 0u)
    {
        result = FALSE;  /* Blue should be OFF */
    }
    
    return result;
}

uint8_t Test_RGB_SetColor_Green(void)
{
    uint8_t result = TRUE;
    uint32_t dataBits;
    
    /* Setup */
    RGB_LED_Init();
    
    /* Execute */
    RGB_LED_SetColor(RGB_GREEN);
    
    /* Verify PC5=0, PC6=1 (Green), PC7=0 */
    dataBits = GPIO_PORTC_DATA_R & RGB_ALL_PINS;
    
    if ((dataBits & RGB_RED_PIN) != 0u)
    {
        result = FALSE;
    }
    if ((dataBits & RGB_GREEN_PIN) == 0u)
    {
        result = FALSE;
    }
    if ((dataBits & RGB_BLUE_PIN) != 0u)
    {
        result = FALSE;
    }
    
    return result;
}

uint8_t Test_RGB_SetColor_Blue(void)
{
    uint8_t result = TRUE;
    uint32_t dataBits;
    
    /* Setup */
    RGB_LED_Init();
    
    /* Execute */
    RGB_LED_SetColor(RGB_BLUE);
    
    /* Verify PC5=0, PC6=0, PC7=1 (Blue) */
    dataBits = GPIO_PORTC_DATA_R & RGB_ALL_PINS;
    
    if ((dataBits & RGB_RED_PIN) != 0u)
    {
        result = FALSE;
    }
    if ((dataBits & RGB_GREEN_PIN) != 0u)
    {
        result = FALSE;
    }
    if ((dataBits & RGB_BLUE_PIN) == 0u)
    {
        result = FALSE;
    }
    
    return result;
}

uint8_t Test_RGB_SetColor_Off(void)
{
    uint8_t result = TRUE;
    uint32_t dataBits;
    
    /* Setup */
    RGB_LED_Init();
    RGB_LED_SetColor(RGB_WHITE);  /* Turn all on first */
    
    /* Execute */
    RGB_LED_SetColor(RGB_OFF);
    
    /* Verify all pins OFF */
    dataBits = GPIO_PORTC_DATA_R & RGB_ALL_PINS;
    
    if (dataBits != 0u)
    {
        result = FALSE;
    }
    
    return result;
}

uint8_t Test_RGB_SetColor_White(void)
{
    uint8_t result = TRUE;
    uint32_t dataBits;
    
    /* Setup */
    RGB_LED_Init();
    
    /* Execute */
    RGB_LED_SetColor(RGB_WHITE);
    
    /* Verify all pins ON */
    dataBits = GPIO_PORTC_DATA_R & RGB_ALL_PINS;
    
    if (dataBits != RGB_ALL_PINS)
    {
        result = FALSE;
    }
    
    return result;
}

void Test_RGB_RunAll(void)
{
    TestLog_SuiteStart(RGB_SUITE);
    
    TEST_RUN(RGB_SUITE, "Init_SetsRegisters", Test_RGB_Init_SetsRegisters);
    TEST_RUN(RGB_SUITE, "SetColor_Red", Test_RGB_SetColor_Red);
    TEST_RUN(RGB_SUITE, "SetColor_Green", Test_RGB_SetColor_Green);
    TEST_RUN(RGB_SUITE, "SetColor_Blue", Test_RGB_SetColor_Blue);
    TEST_RUN(RGB_SUITE, "SetColor_Off", Test_RGB_SetColor_Off);
    TEST_RUN(RGB_SUITE, "SetColor_White", Test_RGB_SetColor_White);
    
    TestLog_SuiteEnd(RGB_SUITE);
}
