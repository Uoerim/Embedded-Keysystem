/**
 * @file    test_cases_system.h
 * @brief   System Tests - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_SYSTEM_H
#define TEST_CASES_SYSTEM_H

#include "../Common/Std_Types.h"

void Test_System_RunAll(void);

/* Full Door Sequence Tests */
uint8_t Test_Sys_FullDoorOpenClose(void);
uint8_t Test_Sys_PasswordVerification(void);
uint8_t Test_Sys_TimeoutPersistence(void);

#endif /* TEST_CASES_SYSTEM_H */
