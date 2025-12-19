/**
 * @file    test_cases_driver_adc.c
 * @brief   ADC Driver Unit Tests Implementation - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Tests ADC initialization and value range validation.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_driver_adc.h"
#include "test_config.h"
#include "test_log.h"
#include "../MCAL/ADC.h"

/*===========================================================================*/
/*                           REGISTER DEFINITIONS                            */
/*===========================================================================*/

/* ADC0 Registers */
#define ADC0_ACTSS_R      (*((volatile uint32_t *)0x40038000u))
#define ADC0_EMUX_R       (*((volatile uint32_t *)0x40038014u))
#define ADC0_SSMUX3_R     (*((volatile uint32_t *)0x400380A0u))
#define ADC0_SSCTL3_R     (*((volatile uint32_t *)0x400380A4u))

/* Sample Sequencer 3 enable bit */
#define ADC_ACTSS_ASEN3   (0x08u)

/* ADC value limits */
#define ADC_MIN_VALUE     (0u)
#define ADC_MAX_VALUE     (4095u)

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define ADC_SUITE         "ADC"

/*===========================================================================*/
/*                           TEST IMPLEMENTATIONS                            */
/*===========================================================================*/

boolean Test_ADC_Init_EnablesSS3(void)
{
    boolean result = TRUE;
    uint32_t actss;
    
    /* Execute */
    ADC_Init();
    
    /* Verify SS3 is enabled */
    actss = ADC0_ACTSS_R;
    
    if ((actss & ADC_ACTSS_ASEN3) == 0u)
    {
        result = FALSE;
    }
    
    return result;
}

boolean Test_ADC_Read_InValidRange(void)
{
    boolean result = TRUE;
    uint16 adcValue = 0u;
    Std_ReturnType retVal;
    
    /* Ensure ADC is initialized */
    ADC_Init();
    
    /* Execute - read ADC with timeout */
    retVal = ADC_ReadTimeout(TEST_ADC_TIMEOUT_MS, &adcValue);
    
    if (retVal == E_OK)
    {
        /* Verify value is in 12-bit range */
        if (adcValue > ADC_MAX_VALUE)
        {
            result = FALSE;
        }
    }
    else
    {
        /* Read failed - this is still a valid test result */
        result = FALSE;
    }
    
    return result;
}

boolean Test_ADC_ReadTimeout_Works(void)
{
    boolean result = TRUE;
    uint16 adcValue = 0u;
    Std_ReturnType retVal;
    
    /* Ensure ADC is initialized */
    ADC_Init();
    
    /* Execute - read should complete within timeout */
    retVal = ADC_ReadTimeout(TEST_ADC_TIMEOUT_MS, &adcValue);
    
    /* Should return E_OK if hardware is working */
    if (retVal != E_OK)
    {
        /* May fail on simulator - log as skip condition */
        result = FALSE;
    }
    
    return result;
}

void Test_ADC_RunAll(void)
{
    TestLog_SuiteStart(ADC_SUITE);
    
    TEST_RUN(ADC_SUITE, "Init_EnablesSS3", Test_ADC_Init_EnablesSS3);
    TEST_RUN(ADC_SUITE, "Read_InValidRange", Test_ADC_Read_InValidRange);
    TEST_RUN(ADC_SUITE, "ReadTimeout_Works", Test_ADC_ReadTimeout_Works);
    
    TestLog_SuiteEnd(ADC_SUITE);
}
