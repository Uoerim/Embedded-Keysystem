/**
 * @file    test_cases_driver_rgb.h
 * @brief   RGB LED Driver Unit Tests - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_DRIVER_RGB_H
#define TEST_CASES_DRIVER_RGB_H

#include "../Common/Std_Types.h"

void Test_RGB_RunAll(void);

uint8_t Test_RGB_Init_SetsRegisters(void);
uint8_t Test_RGB_SetColor_Red(void);
uint8_t Test_RGB_SetColor_Green(void);
uint8_t Test_RGB_SetColor_Blue(void);
uint8_t Test_RGB_SetColor_Off(void);
uint8_t Test_RGB_SetColor_White(void);

#endif /* TEST_CASES_DRIVER_RGB_H */
