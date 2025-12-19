/**
 * @file    test_cases_driver_gpio.c
 * @brief   GPIO Driver Unit Tests Implementation - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Tests GPIO register configuration after initialization.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_driver_gpio.h"
#include "test_config.h"
#include "test_log.h"
#include "../MCAL/GPIO.h"

/*===========================================================================*/
/*                           REGISTER DEFINITIONS                            */
/*===========================================================================*/

/* GPIO Port F Registers (LED Port) */
#define GPIO_PORTF_DATA_R     (*((volatile uint32_t *)0x400253FCu))
#define GPIO_PORTF_DIR_R      (*((volatile uint32_t *)0x40025400u))
#define GPIO_PORTF_DEN_R      (*((volatile uint32_t *)0x4002551Cu))

/* Expected masks for Port F LED (PF1 = Red LED) */
#define LED_PIN_MASK          (0x02u)  /* PF1 */

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define GPIO_SUITE            "GPIO"

/*===========================================================================*/
/*                           TEST IMPLEMENTATIONS                            */
/*===========================================================================*/

boolean Test_GPIO_InitLED_SetsRegisters(void)
{
    boolean result = TRUE;
    uint32_t dirBits;
    uint32_t denBits;
    
    /* Execute */
    GPIOF_InitLED();
    
    /* Verify DIR register - PF1 should be output */
    dirBits = GPIO_PORTF_DIR_R & LED_PIN_MASK;
    if (dirBits != LED_PIN_MASK)
    {
        result = FALSE;
    }
    
    /* Verify DEN register - PF1 should be enabled */
    denBits = GPIO_PORTF_DEN_R & LED_PIN_MASK;
    if (denBits != LED_PIN_MASK)
    {
        result = FALSE;
    }
    
    return result;
}

boolean Test_GPIO_ToggleLED_ChangesState(void)
{
    boolean result = TRUE;
    uint32_t stateBefore;
    uint32_t stateAfter;
    
    /* Setup - ensure LED is initialized */
    GPIOF_InitLED();
    
    /* Record initial state */
    stateBefore = GPIO_PORTF_DATA_R & LED_PIN_MASK;
    
    /* Execute */
    GPIOF_ToggleRedLED();
    
    /* Verify state changed */
    stateAfter = GPIO_PORTF_DATA_R & LED_PIN_MASK;
    
    if (stateBefore == stateAfter)
    {
        result = FALSE;
    }
    
    /* Toggle back to restore */
    GPIOF_ToggleRedLED();
    
    return result;
}

void Test_GPIO_RunAll(void)
{
    TestLog_SuiteStart(GPIO_SUITE);
    
    TEST_RUN(GPIO_SUITE, "InitLED_SetsRegisters", Test_GPIO_InitLED_SetsRegisters);
    TEST_RUN(GPIO_SUITE, "ToggleLED_ChangesState", Test_GPIO_ToggleLED_ChangesState);
    
    TestLog_SuiteEnd(GPIO_SUITE);
}
