/**
 * @file    test_cases_driver_i2c.h
 * @brief   I2C Driver Unit Tests - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_DRIVER_I2C_H
#define TEST_CASES_DRIVER_I2C_H

#include "../Common/Std_Types.h"

/**
 * @brief Run all I2C unit tests
 */
void Test_I2C_RunAll(void);

/**
 * @brief Test I2C0 initialization configures master mode
 * @return TRUE if passed
 */
boolean Test_I2C_Init_ConfiguresMaster(void);

/**
 * @brief Test I2C0 write with invalid address times out
 * @return TRUE if passed
 */
boolean Test_I2C_Write_InvalidAddress_Timeout(void);

#endif /* TEST_CASES_DRIVER_I2C_H */
