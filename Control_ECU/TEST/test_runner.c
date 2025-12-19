/**
 * @file    test_runner.c
 * @brief   Test Runner Implementation - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_runner.h"
#include "test_config.h"
#include "test_log.h"

/* Test case headers */
#include "test_cases_driver_rgb.h"
#include "test_cases_driver_motor.h"
#include "test_cases_driver_eeprom.h"
#include "test_cases_driver_uart.h"
#include "test_cases_driver_delay.h"
#include "test_cases_integration.h"
#include "test_cases_system.h"

/*===========================================================================*/
/*                           PRIVATE VARIABLES                               */
/*===========================================================================*/

static uint8_t g_runner_initialized = 0u;

/*===========================================================================*/
/*                           PUBLIC FUNCTIONS                                */
/*===========================================================================*/

Std_ReturnType TestRunner_Init(void)
{
    Std_ReturnType result;
    
    if (g_runner_initialized == 0u)
    {
        result = TestLog_Init();
        
        if (result == E_OK)
        {
            g_runner_initialized = 1u;
            TestLog_Info("Test Runner initialized");
        }
    }
    else
    {
        result = E_OK;
    }
    
    return result;
}

uint16_t TestRunner_RunUnitTests(void)
{
    uint16_t failed = 0u;
    const TestStats_t *stats;
    uint16_t prev_failed;
    
#if (TEST_ENABLE_UNIT_TESTS == 1u)
    
    TestLog_Info("=== UNIT TESTS ===");
    
#if (TEST_UNIT_RGB_LED == 1u)
    stats = TestLog_GetStats();
    prev_failed = stats->failed;
    Test_RGB_RunAll();
    stats = TestLog_GetStats();
    failed += (stats->failed - prev_failed);
#endif

#if (TEST_UNIT_MOTOR == 1u)
    stats = TestLog_GetStats();
    prev_failed = stats->failed;
    Test_Motor_RunAll();
    stats = TestLog_GetStats();
    failed += (stats->failed - prev_failed);
#endif

#if (TEST_UNIT_EEPROM == 1u)
    stats = TestLog_GetStats();
    prev_failed = stats->failed;
    Test_EEPROM_RunAll();
    stats = TestLog_GetStats();
    failed += (stats->failed - prev_failed);
#endif

#if (TEST_UNIT_UART == 1u)
    stats = TestLog_GetStats();
    prev_failed = stats->failed;
    Test_UART_RunAll();
    stats = TestLog_GetStats();
    failed += (stats->failed - prev_failed);
#endif

#if (TEST_UNIT_DELAY == 1u)
    stats = TestLog_GetStats();
    prev_failed = stats->failed;
    Test_Delay_RunAll();
    stats = TestLog_GetStats();
    failed += (stats->failed - prev_failed);
#endif

#else
    TestLog_Info("Unit tests disabled in configuration");
    (void)stats;
    (void)prev_failed;
#endif
    
    return failed;
}

uint16_t TestRunner_RunIntegrationTests(void)
{
    uint16_t failed = 0u;
    const TestStats_t *stats;
    uint16_t prev_failed;
    
#if (TEST_ENABLE_INTEGRATION_TESTS == 1u)
    
    TestLog_Info("=== INTEGRATION TESTS ===");
    
    stats = TestLog_GetStats();
    prev_failed = stats->failed;
    
    Test_Integration_RunAll();
    
    stats = TestLog_GetStats();
    failed = stats->failed - prev_failed;
    
#else
    TestLog_Info("Integration tests disabled");
    (void)stats;
    (void)prev_failed;
#endif
    
    return failed;
}

uint16_t TestRunner_RunSystemTests(void)
{
    uint16_t failed = 0u;
    const TestStats_t *stats;
    uint16_t prev_failed;
    
#if (TEST_ENABLE_SYSTEM_TESTS == 1u)
    
    TestLog_Info("=== SYSTEM TESTS ===");
    
    stats = TestLog_GetStats();
    prev_failed = stats->failed;
    
    Test_System_RunAll();
    
    stats = TestLog_GetStats();
    failed = stats->failed - prev_failed;
    
#else
    TestLog_Info("System tests disabled");
    (void)stats;
    (void)prev_failed;
#endif
    
    return failed;
}

uint16_t TestRunner_RunLevel(TestLevel_t level)
{
    uint16_t failed = 0u;
    
    switch (level)
    {
        case TEST_LEVEL_UNIT:
            failed = TestRunner_RunUnitTests();
            break;
            
        case TEST_LEVEL_INTEGRATION:
            failed = TestRunner_RunIntegrationTests();
            break;
            
        case TEST_LEVEL_SYSTEM:
            failed = TestRunner_RunSystemTests();
            break;
            
        case TEST_LEVEL_ALL:
            failed  = TestRunner_RunUnitTests();
            failed += TestRunner_RunIntegrationTests();
            failed += TestRunner_RunSystemTests();
            break;
            
        default:
            TestLog_Info("Invalid test level");
            break;
    }
    
    return failed;
}

uint16_t TestRunner_RunAll(void)
{
    uint16_t total_failed;
    
    if (g_runner_initialized == 0u)
    {
        (void)TestRunner_Init();
    }
    
    TestLog_ResetStats();
    
    total_failed = TestRunner_RunLevel(TEST_LEVEL_ALL);
    
    TestLog_PrintSummary();
    
    return total_failed;
}

uint8_t TestRunner_IsReady(void)
{
    return g_runner_initialized;
}
