/**
 * @file    test_log.h
 * @brief   Test Logger API - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Provides standardized logging for test results over UART.
 *          Output format follows industry-standard pass/fail notation.
 * 
 * @output_format
 *          [PASS] <Suite>::<TestName>
 *          [FAIL] <Suite>::<TestName> | expected=<x> got=<y>
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

/**
 * @brief Test result enumeration
 */
typedef enum
{
    TEST_RESULT_PASS = 0,
    TEST_RESULT_FAIL = 1,
    TEST_RESULT_SKIP = 2
} TestResult_t;

/**
 * @brief Test statistics structure
 */
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

/**
 * @brief Initialize test logger (UART + stats)
 * @return E_OK if successful, E_NOT_OK otherwise
 */
Std_ReturnType TestLog_Init(void);

/**
 * @brief Log a passing test
 * @param suite    Test suite name (e.g., "GPIO", "UART")
 * @param testName Test case name (e.g., "Init_sets_DEN")
 */
void TestLog_Pass(const char *suite, const char *testName);

/**
 * @brief Log a failing test with details
 * @param suite    Test suite name
 * @param testName Test case name
 * @param expected Expected value as string
 * @param actual   Actual value as string
 */
void TestLog_Fail(const char *suite, const char *testName, 
                  const char *expected, const char *actual);

/**
 * @brief Log a failing test with numeric values
 * @param suite    Test suite name
 * @param testName Test case name
 * @param expected Expected numeric value
 * @param actual   Actual numeric value
 */
void TestLog_FailNumeric(const char *suite, const char *testName,
                         uint32_t expected, uint32_t actual);

/**
 * @brief Log a skipped test
 * @param suite    Test suite name
 * @param testName Test case name
 * @param reason   Skip reason
 */
void TestLog_Skip(const char *suite, const char *testName, const char *reason);

/**
 * @brief Log test suite header
 * @param suiteName Name of test suite
 */
void TestLog_SuiteStart(const char *suiteName);

/**
 * @brief Log test suite completion
 * @param suiteName Name of test suite
 */
void TestLog_SuiteEnd(const char *suiteName);

/**
 * @brief Log informational message
 * @param message Info message
 */
void TestLog_Info(const char *message);

/**
 * @brief Print raw string to log
 * @param str String to print
 */
void TestLog_Print(const char *str);

/**
 * @brief Print separator line
 */
void TestLog_Separator(void);

/**
 * @brief Get current test statistics
 * @return Pointer to test statistics structure
 */
const TestStats_t* TestLog_GetStats(void);

/**
 * @brief Print final test summary
 */
void TestLog_PrintSummary(void);

/**
 * @brief Reset test statistics
 */
void TestLog_ResetStats(void);

/*===========================================================================*/
/*                           CONVENIENCE MACROS                              */
/*===========================================================================*/

/**
 * @brief Quick pass logging macro
 */
#define TEST_LOG_PASS(suite, test) \
    TestLog_Pass((suite), (test))

/**
 * @brief Quick fail logging macro with strings
 */
#define TEST_LOG_FAIL(suite, test, exp, act) \
    TestLog_Fail((suite), (test), (exp), (act))

/**
 * @brief Quick fail logging macro with numbers
 */
#define TEST_LOG_FAIL_NUM(suite, test, exp, act) \
    TestLog_FailNumeric((suite), (test), (exp), (act))

/**
 * @brief Quick skip logging macro
 */
#define TEST_LOG_SKIP(suite, test, reason) \
    TestLog_Skip((suite), (test), (reason))

/**
 * @brief Run test and auto-log result
 */
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
