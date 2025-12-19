/**
 * @file    test_cases_system.h
 * @brief   System Tests - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details End-to-end system scenario tests.
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_SYSTEM_H
#define TEST_CASES_SYSTEM_H

#include "../Common/Std_Types.h"

/**
 * @brief Run all system tests
 */
void Test_System_RunAll(void);

/*===========================================================================*/
/*                       SCENARIO 1: FIRST BOOT                              */
/*===========================================================================*/

/**
 * @brief Test first boot / not initialized scenario
 * @details Verifies system behavior when no password is set
 * @return TRUE if passed
 */
boolean Test_Sys_FirstBoot_NotInitialized(void);

/**
 * @brief Test initial password setup
 * @details Sets password and verifies initialized flag changes
 * @return TRUE if passed
 */
boolean Test_Sys_FirstBoot_SetupPassword(void);

/*===========================================================================*/
/*                       SCENARIO 2: VALID PASSWORD                          */
/*===========================================================================*/

/**
 * @brief Test open door with valid password
 * @details Enters correct password and verifies door opens
 * @return TRUE if passed
 */
boolean Test_Sys_ValidPassword_DoorOpens(void);

/**
 * @brief Test door auto-close after timeout
 * @details Verifies door closes after configured timeout
 * @return TRUE if passed
 */
boolean Test_Sys_ValidPassword_DoorCloses(void);

/*===========================================================================*/
/*                       SCENARIO 3: SET TIMEOUT                             */
/*===========================================================================*/

/**
 * @brief Test timeout configuration via potentiometer
 * @details Reads pot, sets timeout, verifies persistence
 * @return TRUE if passed
 */
boolean Test_Sys_SetTimeout_FromPot(void);

/**
 * @brief Test timeout persistence across reboot
 * @details Verifies EEPROM-stored timeout survives power cycle
 * @return TRUE if passed
 */
boolean Test_Sys_SetTimeout_Persistence(void);

/*===========================================================================*/
/*                       SCENARIO 4: LOCKOUT                                 */
/*===========================================================================*/

/**
 * @brief Test wrong password lockout
 * @details 3 wrong attempts trigger lockout
 * @return TRUE if passed
 */
boolean Test_Sys_WrongPassword_Lockout(void);

/**
 * @brief Test lockout duration
 * @details Verifies lockout lasts expected duration
 * @return TRUE if passed
 */
boolean Test_Sys_Lockout_Duration(void);

#endif /* TEST_CASES_SYSTEM_H */
