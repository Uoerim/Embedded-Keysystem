/**
 * @file    test_cases_driver_motor.h
 * @brief   Motor Driver Unit Tests - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_DRIVER_MOTOR_H
#define TEST_CASES_DRIVER_MOTOR_H

#include "../Common/Std_Types.h"

void Test_Motor_RunAll(void);

uint8_t Test_Motor_Init_ConfiguresGPIO(void);
uint8_t Test_Motor_Open_SetsDirectionPins(void);
uint8_t Test_Motor_Close_SetsDirectionPins(void);
uint8_t Test_Motor_Stop_ClearsPins(void);

#endif /* TEST_CASES_DRIVER_MOTOR_H */
