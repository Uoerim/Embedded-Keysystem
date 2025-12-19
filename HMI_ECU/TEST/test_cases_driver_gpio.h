/**
 * @file    test_cases_driver_gpio.h
 * @brief   GPIO Driver Unit Tests - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_DRIVER_GPIO_H
#define TEST_CASES_DRIVER_GPIO_H

#include "../Common/Std_Types.h"

/**
 * @brief Run all GPIO unit tests
 */
void Test_GPIO_RunAll(void);

/**
 * @brief Test GPIO Port F LED initialization
 * @return TRUE if passed
 */
boolean Test_GPIO_InitLED_SetsRegisters(void);

/**
 * @brief Test GPIO Port F LED toggle
 * @return TRUE if passed
 */
boolean Test_GPIO_ToggleLED_ChangesState(void);

#endif /* TEST_CASES_DRIVER_GPIO_H */
