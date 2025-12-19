/**
 * @file    test_cases_integration.h
 * @brief   Integration Tests - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_INTEGRATION_H
#define TEST_CASES_INTEGRATION_H

#include "../Common/Std_Types.h"

void Test_Integration_RunAll(void);

/* UART + EEPROM Integration */
uint8_t Test_Int_UART_EEPROM_SetTimeout(void);
uint8_t Test_Int_UART_EEPROM_SetPassword(void);

/* Motor + RGB Integration */
uint8_t Test_Int_Motor_RGB_OpenSequence(void);
uint8_t Test_Int_Motor_RGB_CloseSequence(void);

/* Full Command Handler */
uint8_t Test_Int_CommandHandler_CheckInit(void);
uint8_t Test_Int_CommandHandler_GetTimeout(void);

#endif /* TEST_CASES_INTEGRATION_H */
