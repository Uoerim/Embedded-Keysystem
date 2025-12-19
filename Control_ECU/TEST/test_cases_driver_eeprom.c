/**
 * @file    test_cases_driver_eeprom.c
 * @brief   EEPROM Driver Unit Tests Implementation - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Tests EEPROM save/load and data persistence.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_driver_eeprom.h"
#include "test_config.h"
#include "test_log.h"
#include "../MCAL/EEPROM.h"

/*===========================================================================*/
/*                           TEST DATA                                       */
/*===========================================================================*/

#define PASSWORD_LENGTH     (5u)
#define TIMEOUT_MIN_SEC     (5u)
#define TIMEOUT_MAX_SEC     (30u)

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define EEPROM_SUITE      "EEPROM"

/*===========================================================================*/
/*                           TEST IMPLEMENTATIONS                            */
/*===========================================================================*/

uint8_t Test_EEPROM_Init_Completes(void)
{
    uint8_t result = TRUE;
    
    /* Execute - EEPROM init should complete without hanging */
    EEPROM0_Init();
    
    /* If we got here, init completed */
    
    return result;
}

uint8_t Test_EEPROM_SaveLoad_Password(void)
{
    uint8_t result = TRUE;
    const char testPassword[PASSWORD_LENGTH] = {'T', 'E', 'S', 'T', '1'};
    char readPassword[PASSWORD_LENGTH] = {0, 0, 0, 0, 0};
    uint8_t timeout = 15u;
    uint8_t initialized = 1u;
    uint8_t readTimeout = 0u;
    uint8_t readInit = 0u;
    uint8_t loadResult;
    uint8_t i;
    
    /* Setup */
    EEPROM0_Init();
    
    /* Execute - save data */
    EEPROM_Save(testPassword, timeout, initialized);
    
    /* Execute - load data */
    loadResult = EEPROM_Load(readPassword, &readTimeout, &readInit);
    
    /* Verify load succeeded */
    if (loadResult != 1u)
    {
        result = FALSE;
    }
    
    /* Verify password matches */
    for (i = 0u; i < PASSWORD_LENGTH; i++)
    {
        if (readPassword[i] != testPassword[i])
        {
            result = FALSE;
            break;
        }
    }
    
    return result;
}

uint8_t Test_EEPROM_SaveLoad_Timeout(void)
{
    uint8_t result = TRUE;
    const char testPassword[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
    char readPassword[PASSWORD_LENGTH];
    uint8_t testTimeout = 25u;
    uint8_t initialized = 1u;
    uint8_t readTimeout = 0u;
    uint8_t readInit = 0u;
    uint8_t loadResult;
    
    /* Setup */
    EEPROM0_Init();
    
    /* Execute - save with specific timeout */
    EEPROM_Save(testPassword, testTimeout, initialized);
    
    /* Execute - load data */
    loadResult = EEPROM_Load(readPassword, &readTimeout, &readInit);
    
    /* Verify load succeeded */
    if (loadResult != 1u)
    {
        result = FALSE;
    }
    
    /* Verify timeout matches */
    if (readTimeout != testTimeout)
    {
        result = FALSE;
    }
    
    /* Verify initialized flag */
    if (readInit != initialized)
    {
        result = FALSE;
    }
    
    return result;
}

uint8_t Test_EEPROM_Clear_Works(void)
{
    uint8_t result = TRUE;
    const char testPassword[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
    char readPassword[PASSWORD_LENGTH];
    uint8_t readTimeout = 0u;
    uint8_t readInit = 0u;
    uint8_t loadResult;
    
    /* Setup - save valid data first */
    EEPROM0_Init();
    EEPROM_Save(testPassword, 15u, 1u);
    
    /* Execute - clear EEPROM */
    EEPROM_Clear();
    
    /* Verify - load should indicate not initialized */
    loadResult = EEPROM_Load(readPassword, &readTimeout, &readInit);
    
    /* After clear, system should report not initialized */
    if (readInit != 0u)
    {
        result = FALSE;
    }
    
    (void)loadResult;
    
    return result;
}

uint8_t Test_EEPROM_DataValidation(void)
{
    uint8_t result = TRUE;
    const char testPassword[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
    char readPassword[PASSWORD_LENGTH];
    uint8_t readTimeout = 0u;
    uint8_t readInit = 0u;
    uint8_t loadResult;
    
    /* Setup */
    EEPROM0_Init();
    
    /* Save with boundary values */
    EEPROM_Save(testPassword, TIMEOUT_MIN_SEC, 1u);
    loadResult = EEPROM_Load(readPassword, &readTimeout, &readInit);
    
    if (loadResult != 1u)
    {
        result = FALSE;
    }
    
    /* Verify minimum timeout */
    if (readTimeout < TIMEOUT_MIN_SEC)
    {
        result = FALSE;
    }
    
    /* Save with maximum timeout */
    EEPROM_Save(testPassword, TIMEOUT_MAX_SEC, 1u);
    loadResult = EEPROM_Load(readPassword, &readTimeout, &readInit);
    
    if (loadResult != 1u)
    {
        result = FALSE;
    }
    
    /* Verify maximum timeout */
    if (readTimeout > TIMEOUT_MAX_SEC)
    {
        result = FALSE;
    }
    
    return result;
}

void Test_EEPROM_RunAll(void)
{
    TestLog_SuiteStart(EEPROM_SUITE);
    
    TEST_RUN(EEPROM_SUITE, "Init_Completes", Test_EEPROM_Init_Completes);
    TEST_RUN(EEPROM_SUITE, "SaveLoad_Password", Test_EEPROM_SaveLoad_Password);
    TEST_RUN(EEPROM_SUITE, "SaveLoad_Timeout", Test_EEPROM_SaveLoad_Timeout);
    TEST_RUN(EEPROM_SUITE, "Clear_Works", Test_EEPROM_Clear_Works);
    TEST_RUN(EEPROM_SUITE, "DataValidation", Test_EEPROM_DataValidation);
    
    TestLog_SuiteEnd(EEPROM_SUITE);
}
