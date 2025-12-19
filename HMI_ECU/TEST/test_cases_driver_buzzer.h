/**
 * @file    test_cases_driver_buzzer.h
 * @brief   Buzzer Driver Unit Tests - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_DRIVER_BUZZER_H
#define TEST_CASES_DRIVER_BUZZER_H

#include "../Common/Std_Types.h"

/**
 * @brief Run all Buzzer unit tests
 */
void Test_Buzzer_RunAll(void);

/**
 * @brief Test Buzzer initialization configures GPIO
 * @return TRUE if passed
 */
boolean Test_Buzzer_Init_ConfiguresGPIO(void);

/**
 * @brief Test Buzzer On/Off toggle works
 * @return TRUE if passed
 */
boolean Test_Buzzer_OnOff_Works(void);

/**
 * @brief Test Buzzer short beep function
 * @return TRUE if passed
 */
boolean Test_Buzzer_BeepShort_Works(void);

#endif /* TEST_CASES_DRIVER_BUZZER_H */
