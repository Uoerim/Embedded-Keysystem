/**
 * @file    test_cases_system.c
 * @brief   System Tests Implementation - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details End-to-end system tests for Control ECU.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_system.h"
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
#define DOOR_OPEN_TIME_MS   (3000u)  /* 3 seconds for test */

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define SYS_SUITE           "System"

/*===========================================================================*/
/*                           SYSTEM TESTS                                    */
/*===========================================================================*/

uint8_t Test_Sys_FullDoorOpenClose(void)
{
    uint8_t result = TRUE;
    
    TestLog_Info("Testing full door open/close sequence");
    
    /* Initialize all peripherals */
    RGB_LED_Init();
    Motor_Init();
    Delay_Init_16MHz();
    
    /* Phase 1: Door Opening */
    TestLog_Info("Phase 1: Opening door");
    RGB_LED_SetColor(RGB_GREEN);
    Motor_Open();
    Delay_ms(1000u);  /* Motor runs for 1 second */
    Motor_Stop();
    
    /* Phase 2: Door Open (holding) */
    TestLog_Info("Phase 2: Door open - holding");
    RGB_LED_SetColor(RGB_BLUE);  /* Blue = door open */
    Delay_ms(DOOR_OPEN_TIME_MS);
    
    /* Phase 3: Door Closing */
    TestLog_Info("Phase 3: Closing door");
    RGB_LED_SetColor(RGB_RED);
    Motor_Close();
    Delay_ms(1000u);
    Motor_Stop();
    
    /* Phase 4: Door Closed */
    TestLog_Info("Phase 4: Door closed");
    RGB_LED_SetColor(RGB_OFF);
    
    /* Verify final state */
    {
        uint32_t rgbData = GPIO_PORTC_DATA_R & RGB_ALL_PINS;
        uint32_t motorData = GPIO_PORTB_DATA_R & 0x03u;
        
        /* RGB should be off */
        if (rgbData != 0u)
        {
            result = FALSE;
            TestLog_Info("RGB not off after sequence");
        }
        
        /* Motor should be stopped */
        if (motorData != 0u)
        {
            result = FALSE;
            TestLog_Info("Motor not stopped after sequence");
        }
    }
    
    return result;
}

uint8_t Test_Sys_PasswordVerification(void)
{
    uint8_t result = TRUE;
    const char correctPass[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
    const char wrongPass[PASSWORD_LENGTH] = {'9', '9', '9', '9', '9'};
    char storedPass[PASSWORD_LENGTH];
    uint8_t timeout;
    uint8_t init;
    uint8_t i;
    uint8_t match;
    
    TestLog_Info("Testing password verification logic");
    
    /* Initialize */
    EEPROM0_Init();
    
    /* Store correct password */
    EEPROM_Save(correctPass, 15u, 1u);
    
    /* Load stored password */
    (void)EEPROM_Load(storedPass, &timeout, &init);
    
    /* Test 1: Verify correct password matches */
    match = 1u;
    for (i = 0u; i < PASSWORD_LENGTH; i++)
    {
        if (storedPass[i] != correctPass[i])
        {
            match = 0u;
            break;
        }
    }
    
    if (match != 1u)
    {
        result = FALSE;
        TestLog_Info("Correct password did not match");
    }
    
    /* Test 2: Verify wrong password does not match */
    match = 1u;
    for (i = 0u; i < PASSWORD_LENGTH; i++)
    {
        if (storedPass[i] != wrongPass[i])
        {
            match = 0u;
            break;
        }
    }
    
    if (match == 1u)
    {
        result = FALSE;
        TestLog_Info("Wrong password matched - security issue!");
    }
    
    return result;
}

uint8_t Test_Sys_TimeoutPersistence(void)
{
    uint8_t result = TRUE;
    const char password[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
    char readPass[PASSWORD_LENGTH];
    uint8_t testTimeout = 22u;
    uint8_t readTimeout = 0u;
    uint8_t readInit = 0u;
    
    TestLog_Info("Testing timeout persistence");
    
    /* Initialize */
    EEPROM0_Init();
    
    /* Save with specific timeout */
    EEPROM_Save(password, testTimeout, 1u);
    
    /* Simulate "reboot" by re-initializing EEPROM */
    EEPROM0_Init();
    
    /* Load and verify timeout persisted */
    (void)EEPROM_Load(readPass, &readTimeout, &readInit);
    
    if (readTimeout != testTimeout)
    {
        result = FALSE;
        TestLog_Info("Timeout did not persist");
    }
    
    if (readInit != 1u)
    {
        result = FALSE;
        TestLog_Info("Init flag did not persist");
    }
    
    return result;
}

/*===========================================================================*/
/*                           RUN ALL                                         */
/*===========================================================================*/

void Test_System_RunAll(void)
{
    TestLog_SuiteStart(SYS_SUITE);
    
    TestLog_Info("=== Control ECU System Tests ===");
    
    TEST_RUN(SYS_SUITE, "FullDoorOpenClose", Test_Sys_FullDoorOpenClose);
    TEST_RUN(SYS_SUITE, "PasswordVerification", Test_Sys_PasswordVerification);
    TEST_RUN(SYS_SUITE, "TimeoutPersistence", Test_Sys_TimeoutPersistence);
    
    TestLog_SuiteEnd(SYS_SUITE);
}
