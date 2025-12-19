/**
 * @file    test_runner.h
 * @brief   Test Runner API - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
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

typedef uint8_t (*TestFunc_t)(void);

typedef struct
{
    const char     *suite;
    const char     *name;
    TestFunc_t      func;
} TestCase_t;

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

Std_ReturnType TestRunner_Init(void);
uint16_t TestRunner_RunAll(void);
uint16_t TestRunner_RunLevel(TestLevel_t level);
uint16_t TestRunner_RunUnitTests(void);
uint16_t TestRunner_RunIntegrationTests(void);
uint16_t TestRunner_RunSystemTests(void);
uint8_t TestRunner_IsReady(void);

#endif /* TEST_RUNNER_H */
