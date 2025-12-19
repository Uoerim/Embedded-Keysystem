/**
 * @file    test_cases_driver_adc.h
 * @brief   ADC Driver Unit Tests - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_DRIVER_ADC_H
#define TEST_CASES_DRIVER_ADC_H

#include "../Common/Std_Types.h"

/**
 * @brief Run all ADC unit tests
 */
void Test_ADC_RunAll(void);

/**
 * @brief Test ADC initialization enables SS3
 * @return TRUE if passed
 */
boolean Test_ADC_Init_EnablesSS3(void);

/**
 * @brief Test ADC read returns value in valid range
 * @return TRUE if passed
 */
boolean Test_ADC_Read_InValidRange(void);

/**
 * @brief Test ADC read timeout works correctly
 * @return TRUE if passed
 */
boolean Test_ADC_ReadTimeout_Works(void);

#endif /* TEST_CASES_DRIVER_ADC_H */
