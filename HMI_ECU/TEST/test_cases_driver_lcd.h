/**
 * @file    test_cases_driver_lcd.h
 * @brief   LCD Driver Unit Tests - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_DRIVER_LCD_H
#define TEST_CASES_DRIVER_LCD_H

#include "../Common/Std_Types.h"

/**
 * @brief Run all LCD unit tests
 */
void Test_LCD_RunAll(void);

/**
 * @brief Test LCD initialization completes without error
 * @return TRUE if passed
 */
boolean Test_LCD_Init_Completes(void);

/**
 * @brief Test LCD clear function
 * @return TRUE if passed
 */
boolean Test_LCD_Clear_Works(void);

/**
 * @brief Test LCD set cursor function
 * @return TRUE if passed
 */
boolean Test_LCD_SetCursor_Works(void);

/**
 * @brief Test LCD string output (smoke test)
 * @return TRUE if passed
 */
boolean Test_LCD_SendString_SmokeTest(void);

#endif /* TEST_CASES_DRIVER_LCD_H */
