/**
 * @file    test_log.c
 * @brief   Test Logger Implementation - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Implements UART-based test logging with standardized output.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_log.h"
#include "test_config.h"
#include "../MCAL/UART.h"
#include "../MCAL/Delay.h"

/*===========================================================================*/
/*                           PRIVATE VARIABLES                               */
/*===========================================================================*/

static TestStats_t g_testStats = {0u, 0u, 0u, 0u, 0u, 0u};
static boolean     g_initialized = FALSE;

/*===========================================================================*/
/*                           PRIVATE FUNCTIONS                               */
/*===========================================================================*/

/**
 * @brief Convert uint32 to decimal string
 */
static void UInt32ToString(uint32_t value, char *buffer)
{
    char temp[12];
    uint8_t idx = 0u;
    uint8_t i;
    
    if (value == 0u)
    {
        buffer[0] = '0';
        buffer[1] = '\0';
        return;
    }
    
    while (value > 0u)
    {
        temp[idx] = (char)('0' + (value % 10u));
        value /= 10u;
        idx++;
    }
    
    /* Reverse */
    for (i = 0u; i < idx; i++)
    {
        buffer[i] = temp[idx - 1u - i];
    }
    buffer[idx] = '\0';
}

/**
 * @brief Print newline
 */
static void PrintNewline(void)
{
    UART1_SendByte((uint8_t)'\r');
    UART1_SendByte((uint8_t)'\n');
}

/**
 * @brief Print timestamp if enabled
 */
static void PrintTimestamp(void)
{
#if (TEST_LOG_ENABLE_TIMESTAMPS == 1u)
    char timeStr[12];
    uint32_t ticks = Delay_GetTicksMs();
    
    UART1_SendByte((uint8_t)'[');
    UInt32ToString(ticks, timeStr);
    UART1_SendString(timeStr);
    UART1_SendString("ms] ");
#endif
}

/*===========================================================================*/
/*                           PUBLIC FUNCTIONS                                */
/*===========================================================================*/

Std_ReturnType TestLog_Init(void)
{
    if (g_initialized == FALSE)
    {
        UART1_Init(TEST_LOG_UART_BAUDRATE);
        Delay_Init_16MHz();
        
        g_testStats.total_tests = 0u;
        g_testStats.passed      = 0u;
        g_testStats.failed      = 0u;
        g_testStats.skipped     = 0u;
        g_testStats.start_time_ms = Delay_GetTicksMs();
        g_testStats.end_time_ms   = 0u;
        
        g_initialized = TRUE;
        
        /* Print header */
        PrintNewline();
        TestLog_Separator();
        UART1_SendString("  EMBEDDED DOOR-LOCK SYSTEM - TEST FRAMEWORK v");
        
        {
            char verStr[4];
            UInt32ToString(TEST_FRAMEWORK_VERSION_MAJOR, verStr);
            UART1_SendString(verStr);
            UART1_SendByte((uint8_t)'.');
            UInt32ToString(TEST_FRAMEWORK_VERSION_MINOR, verStr);
            UART1_SendString(verStr);
            UART1_SendByte((uint8_t)'.');
            UInt32ToString(TEST_FRAMEWORK_VERSION_PATCH, verStr);
            UART1_SendString(verStr);
        }
        
        PrintNewline();
        UART1_SendString("  Target: TM4C123GH6PM | ECU: HMI");
        PrintNewline();
        TestLog_Separator();
        PrintNewline();
    }
    
    return E_OK;
}

void TestLog_Pass(const char *suite, const char *testName)
{
    if (suite == NULL)
    {
        return;
    }
    if (testName == NULL)
    {
        return;
    }
    
    PrintTimestamp();
    UART1_SendString("[PASS] ");
    UART1_SendString(suite);
    UART1_SendString("::");
    UART1_SendString(testName);
    PrintNewline();
    
    g_testStats.total_tests++;
    g_testStats.passed++;
}

void TestLog_Fail(const char *suite, const char *testName,
                  const char *expected, const char *actual)
{
    if (suite == NULL)
    {
        return;
    }
    if (testName == NULL)
    {
        return;
    }
    
    PrintTimestamp();
    UART1_SendString("[FAIL] ");
    UART1_SendString(suite);
    UART1_SendString("::");
    UART1_SendString(testName);
    
    if ((expected != NULL) && (actual != NULL))
    {
        UART1_SendString(" | expected=");
        UART1_SendString(expected);
        UART1_SendString(" got=");
        UART1_SendString(actual);
    }
    
    PrintNewline();
    
    g_testStats.total_tests++;
    g_testStats.failed++;
}

void TestLog_FailNumeric(const char *suite, const char *testName,
                         uint32_t expected, uint32_t actual)
{
    char expStr[12];
    char actStr[12];
    
    UInt32ToString(expected, expStr);
    UInt32ToString(actual, actStr);
    
    TestLog_Fail(suite, testName, expStr, actStr);
}

void TestLog_Skip(const char *suite, const char *testName, const char *reason)
{
    if (suite == NULL)
    {
        return;
    }
    if (testName == NULL)
    {
        return;
    }
    
    PrintTimestamp();
    UART1_SendString("[SKIP] ");
    UART1_SendString(suite);
    UART1_SendString("::");
    UART1_SendString(testName);
    
    if (reason != NULL)
    {
        UART1_SendString(" | ");
        UART1_SendString(reason);
    }
    
    PrintNewline();
    
    g_testStats.total_tests++;
    g_testStats.skipped++;
}

void TestLog_SuiteStart(const char *suiteName)
{
    if (suiteName == NULL)
    {
        return;
    }
    
    PrintNewline();
    UART1_SendString(">>> Suite: ");
    UART1_SendString(suiteName);
    PrintNewline();
    UART1_SendString("-------------------------------------------");
    PrintNewline();
}

void TestLog_SuiteEnd(const char *suiteName)
{
    if (suiteName == NULL)
    {
        return;
    }
    
    UART1_SendString("<<< End: ");
    UART1_SendString(suiteName);
    PrintNewline();
    PrintNewline();
}

void TestLog_Info(const char *message)
{
    if (message == NULL)
    {
        return;
    }
    
    PrintTimestamp();
    UART1_SendString("[INFO] ");
    UART1_SendString(message);
    PrintNewline();
}

void TestLog_Print(const char *str)
{
    if (str == NULL)
    {
        return;
    }
    UART1_SendString(str);
}

void TestLog_Separator(void)
{
    UART1_SendString("===========================================");
    PrintNewline();
}

const TestStats_t* TestLog_GetStats(void)
{
    return &g_testStats;
}

void TestLog_PrintSummary(void)
{
    char numStr[12];
    uint32_t duration;
    
    g_testStats.end_time_ms = Delay_GetTicksMs();
    duration = g_testStats.end_time_ms - g_testStats.start_time_ms;
    
    PrintNewline();
    TestLog_Separator();
    UART1_SendString("                  TEST SUMMARY");
    PrintNewline();
    TestLog_Separator();
    
    UART1_SendString("  Total Tests : ");
    UInt32ToString(g_testStats.total_tests, numStr);
    UART1_SendString(numStr);
    PrintNewline();
    
    UART1_SendString("  Passed      : ");
    UInt32ToString(g_testStats.passed, numStr);
    UART1_SendString(numStr);
    PrintNewline();
    
    UART1_SendString("  Failed      : ");
    UInt32ToString(g_testStats.failed, numStr);
    UART1_SendString(numStr);
    PrintNewline();
    
    UART1_SendString("  Skipped     : ");
    UInt32ToString(g_testStats.skipped, numStr);
    UART1_SendString(numStr);
    PrintNewline();
    
    UART1_SendString("  Duration    : ");
    UInt32ToString(duration, numStr);
    UART1_SendString(numStr);
    UART1_SendString(" ms");
    PrintNewline();
    
    TestLog_Separator();
    
    if (g_testStats.failed == 0u)
    {
        UART1_SendString("  *** ALL TESTS PASSED ***");
    }
    else
    {
        UART1_SendString("  *** TESTS FAILED: ");
        UInt32ToString(g_testStats.failed, numStr);
        UART1_SendString(numStr);
        UART1_SendString(" ***");
    }
    
    PrintNewline();
    TestLog_Separator();
    PrintNewline();
}

void TestLog_ResetStats(void)
{
    g_testStats.total_tests   = 0u;
    g_testStats.passed        = 0u;
    g_testStats.failed        = 0u;
    g_testStats.skipped       = 0u;
    g_testStats.start_time_ms = Delay_GetTicksMs();
    g_testStats.end_time_ms   = 0u;
}
