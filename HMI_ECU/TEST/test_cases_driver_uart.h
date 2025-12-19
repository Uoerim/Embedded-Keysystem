/**
 * @file    test_cases_driver_uart.h
 * @brief   UART Driver Unit Tests - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_DRIVER_UART_H
#define TEST_CASES_DRIVER_UART_H

#include "../Common/Std_Types.h"

/**
 * @brief Run all UART unit tests
 */
void Test_UART_RunAll(void);

/**
 * @brief Test UART1 initialization sets control registers
 * @return TRUE if passed
 */
boolean Test_UART_Init_SetsControlRegisters(void);

/**
 * @brief Test UART1 initialization sets baud rate registers
 * @return TRUE if passed
 */
boolean Test_UART_Init_SetsBaudRate(void);

/**
 * @brief Test UART1 receive timeout works correctly
 * @return TRUE if passed
 */
boolean Test_UART_ReceiveTimeout_ReturnsNotOK(void);

#endif /* TEST_CASES_DRIVER_UART_H */
