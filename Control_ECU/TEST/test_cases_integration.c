/**
 * @file    test_cases_integration.c
 * @brief   Integration Tests Implementation - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Tests cross-module interactions.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_integration.h"
#include "test_config.h"
#include "test_log.h"

#include "../HAL/RGB_LED.h"
#include "../HAL/Motor.h"
#include "../MCAL/EEPROM.h"
#include "../MCAL/UART.h"
#include "../MCAL/Delay.h"

/*===========================================================================*/
/*                           CONSTANTS                                       */
/*===========================================================================*/

#define PASSWORD_LENGTH     (5u)

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define INT_SUITE           "Integration"

/*===========================================================================*/
/*                       UART + EEPROM TESTS                                 */
/*===========================================================================*/

uint8_t Test_Int_UART_EEPROM_SetTimeout(void)
{
    uint8_t result = TRUE;
    const char password[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
    char readPass[PASSWORD_LENGTH];
    uint8_t testTimeout = 20u;
    uint8_t readTimeout = 0u;
    uint8_t readInit = 0u;
    uint8_t loadResult;
    
    /* Initialize peripherals */
    EEPROM0_Init();
    
    /* Save timeout via EEPROM (simulating UART command result) */
    EEPROM_Save(password, testTimeout, 1u);
    
    /* Verify by loading */
    loadResult = EEPROM_Load(readPass, &readTimeout, &readInit);
    
    if (loadResult != 1u)
    {
        result = FALSE;
    }
    
    if (readTimeout != testTimeout)
    {
        result = FALSE;
    }
    
    return result;
}

uint8_t Test_Int_UART_EEPROM_SetPassword(void)
{
    uint8_t result = TRUE;
    const char password[PASSWORD_LENGTH] = {'A', 'B', 'C', 'D', 'E'};
    char readPass[PASSWORD_LENGTH];
    uint8_t readTimeout = 0u;
    uint8_t readInit = 0u;
    uint8_t loadResult;
    uint8_t i;
    
    /* Initialize */
    EEPROM0_Init();
    
    /* Save password (simulating UART N command) */
    EEPROM_Save(password, 15u, 1u);
    
    /* Load and verify */
    loadResult = EEPROM_Load(readPass, &readTimeout, &readInit);
    
    if (loadResult != 1u)
    {
        result = FALSE;
    }
    
    for (i = 0u; i < PASSWORD_LENGTH; i++)
    {
        if (readPass[i] != password[i])
        {
            result = FALSE;
            break;
        }
    }
    
    return result;
}

/*===========================================================================*/
/*                       MOTOR + RGB TESTS                                   */
/*===========================================================================*/

uint8_t Test_Int_Motor_RGB_OpenSequence(void)
{
    uint8_t result = TRUE;
    uint32_t rgbData;
    
    /* Initialize */
    RGB_LED_Init();
    Motor_Init();
    
    /* Execute open sequence */
    RGB_LED_SetColor(RGB_GREEN);  /* Green = unlocking */
    Motor_Open();
    
    Delay_ms(500u);  /* Allow motor to run */
    
    /* Verify RGB is green */
    rgbData = GPIO_PORTC_DATA_R & RGB_ALL_PINS;
    if ((rgbData & RGB_GREEN_PIN) == 0u)
    {
        result = FALSE;
    }
    
    /* Stop motor */
    Motor_Stop();
    
    return result;
}

uint8_t Test_Int_Motor_RGB_CloseSequence(void)
{
    uint8_t result = TRUE;
    uint32_t rgbData;
    
    /* Initialize */
    RGB_LED_Init();
    Motor_Init();
    
    /* Execute close sequence */
    RGB_LED_SetColor(RGB_RED);  /* Red = locking */
    Motor_Close();
    
    Delay_ms(500u);
    
    /* Verify RGB is red */
    rgbData = GPIO_PORTC_DATA_R & RGB_ALL_PINS;
    if ((rgbData & RGB_RED_PIN) == 0u)
    {
        result = FALSE;
    }
    
    /* Stop motor */
    Motor_Stop();
    RGB_LED_SetColor(RGB_OFF);
    
    return result;
}

/*===========================================================================*/
/*                       COMMAND HANDLER TESTS                               */
/*===========================================================================*/

uint8_t Test_Int_CommandHandler_CheckInit(void)
{
    uint8_t result = TRUE;
    const char password[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
    char readPass[PASSWORD_LENGTH];
    uint8_t readTimeout = 0u;
    uint8_t readInit = 0u;
    uint8_t loadResult;
    
    /* Initialize */
    EEPROM0_Init();
    
    /* Clear EEPROM first */
    EEPROM_Clear();
    
    /* Check - should report not initialized */
    loadResult = EEPROM_Load(readPass, &readTimeout, &readInit);
    
    if (readInit != 0u)
    {
        result = FALSE;
    }
    
    /* Set password - should become initialized */
    EEPROM_Save(password, 15u, 1u);
    
    loadResult = EEPROM_Load(readPass, &readTimeout, &readInit);
    
    if (readInit != 1u)
    {
        result = FALSE;
    }
    
    (void)loadResult;
    
    return result;
}

uint8_t Test_Int_CommandHandler_GetTimeout(void)
{
    uint8_t result = TRUE;
    const char password[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
    char readPass[PASSWORD_LENGTH];
    uint8_t testTimeout = 25u;
    uint8_t readTimeout = 0u;
    uint8_t readInit = 0u;
    
    /* Initialize */
    EEPROM0_Init();
    
    /* Set timeout */
    EEPROM_Save(password, testTimeout, 1u);
    
    /* Get timeout (simulating G command) */
    (void)EEPROM_Load(readPass, &readTimeout, &readInit);
    
    if (readTimeout != testTimeout)
    {
        result = FALSE;
    }
    
    return result;
}

/*===========================================================================*/
/*                           RUN ALL                                         */
/*===========================================================================*/

void Test_Integration_RunAll(void)
{
    TestLog_SuiteStart(INT_SUITE);
    
    /* UART + EEPROM */
    TEST_RUN(INT_SUITE, "UART_EEPROM_SetTimeout", Test_Int_UART_EEPROM_SetTimeout);
    TEST_RUN(INT_SUITE, "UART_EEPROM_SetPassword", Test_Int_UART_EEPROM_SetPassword);
    
    /* Motor + RGB */
    TEST_RUN(INT_SUITE, "Motor_RGB_OpenSequence", Test_Int_Motor_RGB_OpenSequence);
    TEST_RUN(INT_SUITE, "Motor_RGB_CloseSequence", Test_Int_Motor_RGB_CloseSequence);
    
    /* Command Handler */
    TEST_RUN(INT_SUITE, "CommandHandler_CheckInit", Test_Int_CommandHandler_CheckInit);
    TEST_RUN(INT_SUITE, "CommandHandler_GetTimeout", Test_Int_CommandHandler_GetTimeout);
    
    TestLog_SuiteEnd(INT_SUITE);
}
