/**
 * @file    test_cases_driver_delay.h
 * @brief   Delay Driver Unit Tests - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_DRIVER_DELAY_H
#define TEST_CASES_DRIVER_DELAY_H

#include "../Common/Std_Types.h"

void Test_Delay_RunAll(void);

uint8_t Test_Delay_Init_Completes(void);
uint8_t Test_Delay_Ms_Works(void);

#endif /* TEST_CASES_DRIVER_DELAY_H */
