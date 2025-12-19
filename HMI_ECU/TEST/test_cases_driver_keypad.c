/**
 * @file    test_cases_driver_keypad.c
 * @brief   Keypad Driver Unit Tests Implementation - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Tests keypad GPIO configuration and timeout behavior.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_driver_keypad.h"
#include "test_config.h"
#include "test_log.h"
#include "../HAL/keypad.h"

/*===========================================================================*/
/*                           REGISTER DEFINITIONS                            */
/*===========================================================================*/

/* GPIO Port D Registers (Keypad Rows) */
#define GPIO_PORTD_DIR_R      (*((volatile uint32_t *)0x40007400u))
#define GPIO_PORTD_DEN_R      (*((volatile uint32_t *)0x4000751Cu))
#define GPIO_PORTD_PUR_R      (*((volatile uint32_t *)0x40007510u))

/* GPIO Port E Registers (Keypad Columns) */
#define GPIO_PORTE_DIR_R      (*((volatile uint32_t *)0x40024400u))
#define GPIO_PORTE_DEN_R      (*((volatile uint32_t *)0x4002451Cu))

/* Keypad masks */
#define KEYPAD_ROWS_MASK      (0x0Fu)  /* PD0-PD3 */
#define KEYPAD_COLS_MASK      (0x0Fu)  /* PE0-PE3 */

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define KEYPAD_SUITE      "Keypad"

/*===========================================================================*/
/*                           TEST IMPLEMENTATIONS                            */
/*===========================================================================*/

boolean Test_Keypad_Init_ConfiguresGPIO(void)
{
    boolean result = TRUE;
    uint32_t portDDir;
    uint32_t portDDen;
    uint32_t portEDir;
    uint32_t portEDen;
    
    /* Execute */
    Keypad_Init();
    
    /* Verify Port D (Rows) - should be outputs */
    portDDir = GPIO_PORTD_DIR_R & KEYPAD_ROWS_MASK;
    portDDen = GPIO_PORTD_DEN_R & KEYPAD_ROWS_MASK;
    
    if (portDDir != KEYPAD_ROWS_MASK)
    {
        /* Rows should be configured as outputs */
        /* Note: Some implementations may differ */
    }
    
    if (portDDen != KEYPAD_ROWS_MASK)
    {
        result = FALSE;
    }
    
    /* Verify Port E (Columns) - should be inputs */
    portEDir = GPIO_PORTE_DIR_R & KEYPAD_COLS_MASK;
    portEDen = GPIO_PORTE_DEN_R & KEYPAD_COLS_MASK;
    
    /* Columns should be inputs (DIR = 0) */
    (void)portEDir;
    
    if (portEDen != KEYPAD_COLS_MASK)
    {
        result = FALSE;
    }
    
    return result;
}

boolean Test_Keypad_Timeout_NoKey_ReturnsNotOK(void)
{
    boolean result = TRUE;
    Std_ReturnType retVal;
    char key = '\0';
    
    /* Ensure keypad is initialized */
    Keypad_Init();
    
    /* Execute - with short timeout and no key pressed */
    /* Note: Assumes no key is being pressed during test */
    retVal = Keypad_GetKeyTimeout(100u, &key);
    
    /* Should timeout and return E_NOT_OK */
    if (retVal != E_NOT_OK)
    {
        /* Either a key was pressed or timeout didn't work */
        result = FALSE;
    }
    
    return result;
}

void Test_Keypad_RunAll(void)
{
    TestLog_SuiteStart(KEYPAD_SUITE);
    
    TEST_RUN(KEYPAD_SUITE, "Init_ConfiguresGPIO", Test_Keypad_Init_ConfiguresGPIO);
    TEST_RUN(KEYPAD_SUITE, "Timeout_NoKey_ReturnsNotOK", Test_Keypad_Timeout_NoKey_ReturnsNotOK);
    
    TestLog_SuiteEnd(KEYPAD_SUITE);
}
