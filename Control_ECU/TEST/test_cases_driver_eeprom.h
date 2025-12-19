/**
 * @file    test_cases_driver_eeprom.h
 * @brief   EEPROM Driver Unit Tests - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CASES_DRIVER_EEPROM_H
#define TEST_CASES_DRIVER_EEPROM_H

#include "../Common/Std_Types.h"

void Test_EEPROM_RunAll(void);

uint8_t Test_EEPROM_Init_Completes(void);
uint8_t Test_EEPROM_SaveLoad_Password(void);
uint8_t Test_EEPROM_SaveLoad_Timeout(void);
uint8_t Test_EEPROM_Clear_Works(void);
uint8_t Test_EEPROM_DataValidation(void);

#endif /* TEST_CASES_DRIVER_EEPROM_H */
