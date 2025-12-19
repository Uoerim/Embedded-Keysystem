/**
 * @file    test_runner.h
 * @brief   Test Runner API - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Main test orchestration interface. Controls execution
 *          of unit, integration, and system tests.
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_RUNNER_H
#define TEST_RUNNER_H

#include <stdint.h>
#include "../Common/Std_Types.h"

/*===========================================================================*/
/*                              TYPE DEFINITIONS                             */
/*===========================================================================*/

/**
 * @brief Test function pointer type
 */
typedef boolean (*TestFunc_t)(void);

/**
 * @brief Test case descriptor
 */
typedef struct
{
    const char     *suite;
    const char     *name;
    TestFunc_t      func;
} TestCase_t;

/**
 * @brief Test level enumeration
 */
typedef enum
{
    TEST_LEVEL_UNIT = 0,
    TEST_LEVEL_INTEGRATION,
    TEST_LEVEL_SYSTEM,
    TEST_LEVEL_ALL
} TestLevel_t;

/*===========================================================================*/
/*                           FUNCTION PROTOTYPES                             */
/*===========================================================================*/

/**
 * @brief Initialize test runner
 * @return E_OK if successful
 */
Std_ReturnType TestRunner_Init(void);

/**
 * @brief Run all enabled tests
 * @return Number of failed tests
 */
uint16_t TestRunner_RunAll(void);

/**
 * @brief Run tests at specific level
 * @param level Test level to run
 * @return Number of failed tests
 */
uint16_t TestRunner_RunLevel(TestLevel_t level);

/**
 * @brief Run unit tests only
 * @return Number of failed tests
 */
uint16_t TestRunner_RunUnitTests(void);

/**
 * @brief Run integration tests only
 * @return Number of failed tests
 */
uint16_t TestRunner_RunIntegrationTests(void);

/**
 * @brief Run system tests only
 * @return Number of failed tests
 */
uint16_t TestRunner_RunSystemTests(void);

/**
 * @brief Check if test framework is ready
 * @return TRUE if initialized
 */
boolean TestRunner_IsReady(void);

#endif /* TEST_RUNNER_H */
