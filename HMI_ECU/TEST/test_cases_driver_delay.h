/**
 * @file    test_cases_driver_delay.h
 * @brief   Delay Driver Unit Tests - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_DRIVER_DELAY_H
#define TEST_CASES_DRIVER_DELAY_H

#include "../Common/Std_Types.h"

/**
 * @brief Run all Delay unit tests
 */
void Test_Delay_RunAll(void);

/**
 * @brief Test tick counter increments
 * @return TRUE if passed
 */
boolean Test_Delay_TickCounter_Increments(void);

/**
 * @brief Test delay_ms produces correct timing
 * @return TRUE if passed
 */
boolean Test_Delay_Ms_Accuracy(void);

#endif /* TEST_CASES_DRIVER_DELAY_H */
