/**
 * @file    test_cases_driver_uart.h
 * @brief   UART Driver Unit Tests - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_DRIVER_UART_H
#define TEST_CASES_DRIVER_UART_H

#include "../Common/Std_Types.h"

void Test_UART_RunAll(void);

uint8_t Test_UART_Init_SetsControlRegisters(void);
uint8_t Test_UART_Init_SetsBaudRate(void);
uint8_t Test_UART_SendByte_Completes(void);

#endif /* TEST_CASES_DRIVER_UART_H */
