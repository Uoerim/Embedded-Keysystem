/**
 * @file    test_log.h
 * @brief   Test Logger API - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Provides standardized logging for test results over UART.
 *          Output format follows industry-standard pass/fail notation.
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_LOG_H
#define TEST_LOG_H

#include <stdint.h>
#include "../Common/Std_Types.h"

/*===========================================================================*/
/*                              TYPE DEFINITIONS                             */
/*===========================================================================*/

typedef enum
{
    TEST_RESULT_PASS = 0,
    TEST_RESULT_FAIL = 1,
    TEST_RESULT_SKIP = 2
} TestResult_t;

typedef struct
{
    uint16_t total_tests;
    uint16_t passed;
    uint16_t failed;
    uint16_t skipped;
    uint32_t start_time_ms;
    uint32_t end_time_ms;
} TestStats_t;

/*===========================================================================*/
/*                           FUNCTION PROTOTYPES                             */
/*===========================================================================*/

Std_ReturnType TestLog_Init(void);
void TestLog_Pass(const char *suite, const char *testName);
void TestLog_Fail(const char *suite, const char *testName,
                  const char *expected, const char *actual);
void TestLog_FailNumeric(const char *suite, const char *testName,
                         uint32_t expected, uint32_t actual);
void TestLog_Skip(const char *suite, const char *testName, const char *reason);
void TestLog_SuiteStart(const char *suiteName);
void TestLog_SuiteEnd(const char *suiteName);
void TestLog_Info(const char *message);
void TestLog_Print(const char *str);
void TestLog_Separator(void);
const TestStats_t* TestLog_GetStats(void);
void TestLog_PrintSummary(void);
void TestLog_ResetStats(void);

/*===========================================================================*/
/*                           CONVENIENCE MACROS                              */
/*===========================================================================*/

#define TEST_LOG_PASS(suite, test) \
    TestLog_Pass((suite), (test))

#define TEST_LOG_FAIL(suite, test, exp, act) \
    TestLog_Fail((suite), (test), (exp), (act))

#define TEST_LOG_FAIL_NUM(suite, test, exp, act) \
    TestLog_FailNumeric((suite), (test), (exp), (act))

#define TEST_LOG_SKIP(suite, test, reason) \
    TestLog_Skip((suite), (test), (reason))

#define TEST_RUN(suite, testName, testFunc)         \
    do {                                            \
        if ((testFunc)() == TRUE) {                 \
            TestLog_Pass((suite), (testName));      \
        } else {                                    \
            TestLog_Fail((suite), (testName),       \
                         "TRUE", "FALSE");          \
        }                                           \
    } while (0)

#endif /* TEST_LOG_H */
