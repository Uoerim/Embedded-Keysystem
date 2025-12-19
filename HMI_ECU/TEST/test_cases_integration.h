/**
 * @file    test_cases_integration.h
 * @brief   Integration Tests - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Tests interaction between multiple drivers/modules.
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_INTEGRATION_H
#define TEST_CASES_INTEGRATION_H

#include "../Common/Std_Types.h"

/**
 * @brief Run all integration tests
 */
void Test_Integration_RunAll(void);

/*===========================================================================*/
/*                       KEYPAD + LCD INTEGRATION                            */
/*===========================================================================*/

/**
 * @brief Test keypad input displays asterisk on LCD
 * @return TRUE if passed
 * @note Requires manual key press
 */
boolean Test_Int_Keypad_LCD_AsteriskDisplay(void);

/*===========================================================================*/
/*                        ADC + LCD INTEGRATION                              */
/*===========================================================================*/

/**
 * @brief Test ADC timeout value displays on LCD
 * @return TRUE if passed
 */
boolean Test_Int_ADC_LCD_TimeoutDisplay(void);

/*===========================================================================*/
/*                       UART PROTOCOL INTEGRATION                           */
/*===========================================================================*/

/**
 * @brief Test UART 'I' command (check initialized)
 * @return TRUE if passed
 * @note Requires Control ECU to be connected and running
 */
boolean Test_Int_UART_Protocol_CheckInit(void);

/**
 * @brief Test UART 'V' command (verify password)
 * @return TRUE if passed
 * @note Requires Control ECU to be connected and running
 */
boolean Test_Int_UART_Protocol_VerifyPassword(void);

/**
 * @brief Test UART 'N' command (set new password)
 * @return TRUE if passed
 * @note Requires Control ECU to be connected and running
 */
boolean Test_Int_UART_Protocol_SetPassword(void);

/**
 * @brief Test UART 'T' command (set timeout)
 * @return TRUE if passed
 * @note Requires Control ECU to be connected and running
 */
boolean Test_Int_UART_Protocol_SetTimeout(void);

/**
 * @brief Test UART 'G' command (get timeout)
 * @return TRUE if passed
 * @note Requires Control ECU to be connected and running
 */
boolean Test_Int_UART_Protocol_GetTimeout(void);

#endif /* TEST_CASES_INTEGRATION_H */
