/**
 * @file    test_cases_driver_keypad.h
 * @brief   Keypad Driver Unit Tests - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_DRIVER_KEYPAD_H
#define TEST_CASES_DRIVER_KEYPAD_H

#include "../Common/Std_Types.h"

/**
 * @brief Run all Keypad unit tests
 */
void Test_Keypad_RunAll(void);

/**
 * @brief Test Keypad initialization configures GPIO
 * @return TRUE if passed
 */
boolean Test_Keypad_Init_ConfiguresGPIO(void);

/**
 * @brief Test Keypad timeout returns E_NOT_OK when no key pressed
 * @return TRUE if passed
 */
boolean Test_Keypad_Timeout_NoKey_ReturnsNotOK(void);

#endif /* TEST_CASES_DRIVER_KEYPAD_H */
