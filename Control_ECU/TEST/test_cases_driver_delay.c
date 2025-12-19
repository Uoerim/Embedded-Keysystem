/**
 * @file    test_cases_driver_delay.c
 * @brief   Delay Driver Unit Tests Implementation - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_driver_delay.h"
#include "test_config.h"
#include "test_log.h"
#include "../MCAL/Delay.h"

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define DELAY_SUITE       "Delay"

/*===========================================================================*/
/*                           TEST IMPLEMENTATIONS                            */
/*===========================================================================*/

uint8_t Test_Delay_Init_Completes(void)
{
    uint8_t result = TRUE;
    
    /* Execute */
    Delay_Init_16MHz();
    
    /* If we got here, init completed */
    
    return result;
}

uint8_t Test_Delay_Ms_Works(void)
{
    uint8_t result = TRUE;
    
    /* Setup */
    Delay_Init_16MHz();
    
    /* Execute - 100ms delay should complete */
    Delay_ms(100u);
    
    /* If we got here, delay completed */
    
    return result;
}

void Test_Delay_RunAll(void)
{
    TestLog_SuiteStart(DELAY_SUITE);
    
    TEST_RUN(DELAY_SUITE, "Init_Completes", Test_Delay_Init_Completes);
    TEST_RUN(DELAY_SUITE, "Ms_Works", Test_Delay_Ms_Works);
    
    TestLog_SuiteEnd(DELAY_SUITE);
}
