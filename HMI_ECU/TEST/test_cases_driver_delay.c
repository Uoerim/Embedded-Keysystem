/**
 * @file    test_cases_driver_delay.c
 * @brief   Delay Driver Unit Tests Implementation - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Tests delay timing accuracy and tick counter.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_driver_delay.h"
#include "test_config.h"
#include "test_log.h"
#include "../MCAL/Delay.h"

/*===========================================================================*/
/*                           TEST CONFIGURATION                              */
/*===========================================================================*/

/* Timing tolerance (±10%) */
#define DELAY_TOLERANCE_PERCENT   (10u)

/* Test delay duration */
#define TEST_DELAY_MS             (100u)

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define DELAY_SUITE       "Delay"

/*===========================================================================*/
/*                           TEST IMPLEMENTATIONS                            */
/*===========================================================================*/

boolean Test_Delay_TickCounter_Increments(void)
{
    boolean result = TRUE;
    uint32_t ticksBefore;
    uint32_t ticksAfter;
    
    /* Ensure timer is initialized */
    Delay_Init_16MHz();
    
    /* Record initial ticks */
    ticksBefore = Delay_GetTicksMs();
    
    /* Wait a bit */
    Delay_ms(50u);
    
    /* Check ticks increased */
    ticksAfter = Delay_GetTicksMs();
    
    if (ticksAfter <= ticksBefore)
    {
        result = FALSE;
    }
    
    return result;
}

boolean Test_Delay_Ms_Accuracy(void)
{
    boolean result = TRUE;
    uint32_t ticksBefore;
    uint32_t ticksAfter;
    uint32_t elapsed;
    uint32_t minExpected;
    uint32_t maxExpected;
    
    /* Ensure timer is initialized */
    Delay_Init_16MHz();
    
    /* Calculate tolerance bounds */
    minExpected = TEST_DELAY_MS - ((TEST_DELAY_MS * DELAY_TOLERANCE_PERCENT) / 100u);
    maxExpected = TEST_DELAY_MS + ((TEST_DELAY_MS * DELAY_TOLERANCE_PERCENT) / 100u);
    
    /* Record start time */
    ticksBefore = Delay_GetTicksMs();
    
    /* Execute delay */
    Delay_ms(TEST_DELAY_MS);
    
    /* Record end time */
    ticksAfter = Delay_GetTicksMs();
    
    /* Calculate elapsed time */
    elapsed = ticksAfter - ticksBefore;
    
    /* Verify within tolerance */
    if ((elapsed < minExpected) || (elapsed > maxExpected))
    {
        result = FALSE;
    }
    
    return result;
}

void Test_Delay_RunAll(void)
{
    TestLog_SuiteStart(DELAY_SUITE);
    
    TEST_RUN(DELAY_SUITE, "TickCounter_Increments", Test_Delay_TickCounter_Increments);
    TEST_RUN(DELAY_SUITE, "Ms_Accuracy", Test_Delay_Ms_Accuracy);
    
    TestLog_SuiteEnd(DELAY_SUITE);
}
