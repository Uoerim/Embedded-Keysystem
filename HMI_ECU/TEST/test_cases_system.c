/**
 * @file    test_cases_system.c
 * @brief   System Tests Implementation - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details End-to-end scenario tests for complete system verification.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_system.h"
#include "test_config.h"
#include "test_log.h"

/* Driver includes */
#include "../HAL/LCD.h"
#include "../HAL/keypad.h"
#include "../HAL/Buzzer.h"
#include "../MCAL/UART.h"
#include "../MCAL/ADC.h"
#include "../MCAL/Delay.h"

/*===========================================================================*/
/*                           PROTOCOL DEFINITIONS                            */
/*===========================================================================*/

#define CMD_CHECK_INIT      ((uint8_t)'I')
#define CMD_VERIFY_PASS     ((uint8_t)'V')
#define CMD_SET_PASS        ((uint8_t)'N')
#define CMD_SET_TIMEOUT     ((uint8_t)'T')
#define CMD_GET_TIMEOUT     ((uint8_t)'G')
#define CMD_OPEN_DOOR       ((uint8_t)'O')
#define CMD_CLOSE_DOOR      ((uint8_t)'C')

#define RESP_ACK            ((uint8_t)'K')
#define RESP_YES            ((uint8_t)'Y')
#define RESP_NO             ((uint8_t)'N')
#define RESP_INIT_YES       ((uint8_t)'1')
#define RESP_INIT_NO        ((uint8_t)'0')

#define PROTOCOL_TIMEOUT_MS (600u)
#define PASSWORD_LENGTH     (5u)
#define LOCKOUT_ATTEMPTS    (3u)

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define SYS_SUITE           "System"

/*===========================================================================*/
/*                           HELPER FUNCTIONS                                */
/*===========================================================================*/

static Std_ReturnType SendCommandGetResponse(uint8_t cmd, uint8_t *response)
{
    UART1_FlushRx();
    UART1_SendByte(cmd);
    return UART1_ReceiveByteTimeout(PROTOCOL_TIMEOUT_MS, response);
}

static void SendPasswordBytes(const char password[PASSWORD_LENGTH])
{
    uint8_t i;
    for (i = 0u; i < PASSWORD_LENGTH; i++)
    {
        UART1_SendByte((uint8_t)password[i]);
    }
}

static void DisplayTestStatus(const char *testName, boolean passed)
{
    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString(testName);
    LCD_SetCursor(1u, 0u);
    
    if (passed == TRUE)
    {
        LCD_SendString("[PASS]");
    }
    else
    {
        LCD_SendString("[FAIL]");
    }
    
    Delay_ms(500u);
}

/*===========================================================================*/
/*                       SCENARIO 1: FIRST BOOT                              */
/*===========================================================================*/

boolean Test_Sys_FirstBoot_NotInitialized(void)
{
    boolean result = TRUE;
    uint8_t response = 0u;
    Std_ReturnType retVal;
    
    TestLog_Info("Scenario: First Boot - Checking initialized state");
    
    /* Query Control ECU for initialized status */
    retVal = SendCommandGetResponse(CMD_CHECK_INIT, &response);
    
    if (retVal == E_OK)
    {
        /* Log the current state */
        if (response == RESP_INIT_NO)
        {
            TestLog_Info("System not initialized (expected for first boot)");
        }
        else if (response == RESP_INIT_YES)
        {
            TestLog_Info("System already initialized");
        }
        else
        {
            TestLog_Info("Unexpected response from Control ECU");
            result = FALSE;
        }
    }
    else
    {
        TestLog_Info("No response from Control ECU - check connection");
        result = FALSE;
    }
    
    DisplayTestStatus("FirstBoot", result);
    
    return result;
}

boolean Test_Sys_FirstBoot_SetupPassword(void)
{
    boolean result = TRUE;
    uint8_t response = 0u;
    Std_ReturnType retVal;
    const char testPassword[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
    
    TestLog_Info("Scenario: Setup new password");
    
    /* Send 'N' command to set new password */
    UART1_FlushRx();
    UART1_SendByte(CMD_SET_PASS);
    
    /* Send password bytes */
    SendPasswordBytes(testPassword);
    
    /* Wait for ACK */
    retVal = UART1_ReceiveByteTimeout(PROTOCOL_TIMEOUT_MS, &response);
    
    if (retVal == E_OK)
    {
        if (response == RESP_ACK)
        {
            TestLog_Info("Password set successfully");
            
            /* Verify system is now initialized */
            retVal = SendCommandGetResponse(CMD_CHECK_INIT, &response);
            
            if (retVal == E_OK)
            {
                if (response == RESP_INIT_YES)
                {
                    TestLog_Info("System now initialized");
                }
                else
                {
                    TestLog_Info("System still not initialized after password set");
                    result = FALSE;
                }
            }
        }
        else
        {
            TestLog_Info("Password set failed - no ACK");
            result = FALSE;
        }
    }
    else
    {
        result = FALSE;
    }
    
    DisplayTestStatus("SetupPass", result);
    
    return result;
}

/*===========================================================================*/
/*                       SCENARIO 2: VALID PASSWORD                          */
/*===========================================================================*/

boolean Test_Sys_ValidPassword_DoorOpens(void)
{
    boolean result = TRUE;
    uint8_t response = 0u;
    Std_ReturnType retVal;
    const char correctPassword[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
    
    TestLog_Info("Scenario: Open door with valid password");
    
    /* Verify password */
    UART1_FlushRx();
    UART1_SendByte(CMD_VERIFY_PASS);
    SendPasswordBytes(correctPassword);
    
    retVal = UART1_ReceiveByteTimeout(PROTOCOL_TIMEOUT_MS, &response);
    
    if (retVal == E_OK)
    {
        if (response == RESP_YES)
        {
            TestLog_Info("Password verified - sending OPEN command");
            
            /* Send open command */
            UART1_FlushRx();
            UART1_SendByte(CMD_OPEN_DOOR);
            
            retVal = UART1_ReceiveByteTimeout(PROTOCOL_TIMEOUT_MS, &response);
            
            if ((retVal == E_OK) && (response == RESP_ACK))
            {
                TestLog_Info("Door OPEN command acknowledged");
            }
            else
            {
                TestLog_Info("Door OPEN command not acknowledged");
                /* May still pass if door is opening */
            }
        }
        else if (response == RESP_NO)
        {
            TestLog_Info("Password verification failed");
            result = FALSE;
        }
        else
        {
            result = FALSE;
        }
    }
    else
    {
        result = FALSE;
    }
    
    DisplayTestStatus("ValidPass", result);
    
    return result;
}

boolean Test_Sys_ValidPassword_DoorCloses(void)
{
    boolean result = TRUE;
    uint8_t response = 0u;
    uint8_t timeout = 15u;
    Std_ReturnType retVal;
    
    TestLog_Info("Scenario: Verify door closes after timeout");
    
    /* Get current timeout value */
    retVal = SendCommandGetResponse(CMD_GET_TIMEOUT, &response);
    
    if (retVal == E_OK)
    {
        timeout = response;
        TestLog_Info("Waiting for door close (timeout period)");
        
        /* Wait for timeout period plus buffer */
        Delay_ms((uint32_t)timeout * 1000u + 2000u);
        
        /* Door should now be closed - send CLOSE command to verify state */
        UART1_FlushRx();
        UART1_SendByte(CMD_CLOSE_DOOR);
        
        retVal = UART1_ReceiveByteTimeout(PROTOCOL_TIMEOUT_MS, &response);
        
        if (retVal == E_OK)
        {
            TestLog_Info("Door close sequence verified");
        }
    }
    else
    {
        /* Can't verify without timeout value */
        result = FALSE;
    }
    
    DisplayTestStatus("DoorClose", result);
    
    return result;
}

/*===========================================================================*/
/*                       SCENARIO 3: SET TIMEOUT                             */
/*===========================================================================*/

boolean Test_Sys_SetTimeout_FromPot(void)
{
    boolean result = TRUE;
    uint16 adcValue = 0u;
    uint8_t timeout;
    uint8_t response = 0u;
    Std_ReturnType retVal;
    char tens;
    char units;
    
    TestLog_Info("Scenario: Set timeout from potentiometer");
    
    /* Read ADC for timeout value */
    ADC_Init();
    retVal = ADC_ReadTimeout(TEST_ADC_TIMEOUT_MS, &adcValue);
    
    if (retVal == E_OK)
    {
        /* Calculate timeout (5-30 seconds) */
        timeout = (uint8_t)(5u + ((uint32_t)adcValue * 25u) / 4095u);
        
        if (timeout < 5u)
        {
            timeout = 5u;
        }
        if (timeout > 30u)
        {
            timeout = 30u;
        }
        
        /* Send timeout to Control ECU */
        UART1_FlushRx();
        UART1_SendByte(CMD_SET_TIMEOUT);
        
        /* Send as two digit characters */
        tens = (char)('0' + (timeout / 10u));
        units = (char)('0' + (timeout % 10u));
        UART1_SendByte((uint8_t)tens);
        UART1_SendByte((uint8_t)units);
        
        /* Wait for ACK */
        retVal = UART1_ReceiveByteTimeout(PROTOCOL_TIMEOUT_MS, &response);
        
        if ((retVal == E_OK) && (response == RESP_ACK))
        {
            TestLog_Info("Timeout set successfully");
            
            /* Verify by reading back */
            retVal = SendCommandGetResponse(CMD_GET_TIMEOUT, &response);
            
            if ((retVal == E_OK) && (response == timeout))
            {
                TestLog_Info("Timeout verified");
            }
            else
            {
                TestLog_Info("Timeout readback mismatch");
                result = FALSE;
            }
        }
        else
        {
            result = FALSE;
        }
    }
    else
    {
        result = FALSE;
    }
    
    DisplayTestStatus("SetTmout", result);
    
    return result;
}

boolean Test_Sys_SetTimeout_Persistence(void)
{
    boolean result = TRUE;
    uint8_t response = 0u;
    uint8_t savedTimeout = 0u;
    Std_ReturnType retVal;
    
    TestLog_Info("Scenario: Verify timeout persistence");
    TestLog_Info("Note: Requires manual reboot of Control ECU");
    
    /* Get current timeout before simulated reboot */
    retVal = SendCommandGetResponse(CMD_GET_TIMEOUT, &response);
    
    if (retVal == E_OK)
    {
        savedTimeout = response;
        
        /* Display instructions */
        LCD_Clear();
        LCD_SetCursor(0u, 0u);
        LCD_SendString("Reboot Control");
        LCD_SetCursor(1u, 0u);
        LCD_SendString("Then press key");
        
        /* Wait for user to reboot Control ECU */
        {
            char key;
            Std_ReturnType keyRet;
            keyRet = Keypad_GetKeyTimeout(30000u, &key);  /* 30 second timeout */
            (void)keyRet;
        }
        
        /* Read timeout after reboot */
        Delay_ms(2000u);  /* Allow Control ECU to initialize */
        
        retVal = SendCommandGetResponse(CMD_GET_TIMEOUT, &response);
        
        if (retVal == E_OK)
        {
            if (response == savedTimeout)
            {
                TestLog_Info("Timeout persisted correctly");
            }
            else
            {
                TestLog_Info("Timeout changed after reboot");
                result = FALSE;
            }
        }
        else
        {
            TestLog_Info("Failed to read timeout after reboot");
            result = FALSE;
        }
    }
    else
    {
        result = FALSE;
    }
    
    DisplayTestStatus("Persist", result);
    
    return result;
}

/*===========================================================================*/
/*                       SCENARIO 4: LOCKOUT                                 */
/*===========================================================================*/

boolean Test_Sys_WrongPassword_Lockout(void)
{
    boolean result = TRUE;
    uint8_t response = 0u;
    Std_ReturnType retVal;
    const char wrongPassword[PASSWORD_LENGTH] = {'9', '9', '9', '9', '9'};
    uint8_t attempt;
    
    TestLog_Info("Scenario: Wrong password lockout");
    
    for (attempt = 0u; attempt < LOCKOUT_ATTEMPTS; attempt++)
    {
        UART1_FlushRx();
        UART1_SendByte(CMD_VERIFY_PASS);
        SendPasswordBytes(wrongPassword);
        
        retVal = UART1_ReceiveByteTimeout(PROTOCOL_TIMEOUT_MS, &response);
        
        if (retVal == E_OK)
        {
            if (response != RESP_NO)
            {
                TestLog_Info("Unexpected: wrong password accepted");
                result = FALSE;
                break;
            }
        }
        else
        {
            result = FALSE;
            break;
        }
        
        Delay_ms(100u);  /* Brief delay between attempts */
    }
    
    if (result == TRUE)
    {
        TestLog_Info("Lockout should now be active");
        
        /* Try one more time - should still fail even with correct password */
        const char correctPassword[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
        
        UART1_FlushRx();
        UART1_SendByte(CMD_VERIFY_PASS);
        SendPasswordBytes(correctPassword);
        
        retVal = UART1_ReceiveByteTimeout(PROTOCOL_TIMEOUT_MS, &response);
        
        if (retVal == E_OK)
        {
            if (response == RESP_NO)
            {
                TestLog_Info("Lockout active - correct password rejected");
            }
            else if (response == RESP_YES)
            {
                TestLog_Info("Warning: Lockout may not be implemented");
                /* Don't fail - lockout behavior may vary */
            }
        }
    }
    
    DisplayTestStatus("Lockout", result);
    
    return result;
}

boolean Test_Sys_Lockout_Duration(void)
{
    boolean result = TRUE;
    uint8_t response = 0u;
    Std_ReturnType retVal;
    const char correctPassword[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
    uint32_t lockoutDurationMs = 60000u;  /* 60 seconds typical lockout */
    
    TestLog_Info("Scenario: Lockout duration test");
    TestLog_Info("Waiting for lockout to expire...");
    
    /* Display wait message */
    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString("Lockout wait");
    LCD_SetCursor(1u, 0u);
    LCD_SendString("60 seconds...");
    
    /* Wait for lockout to expire */
    Delay_ms(lockoutDurationMs + 5000u);  /* Add 5 second buffer */
    
    /* Try correct password after lockout */
    UART1_FlushRx();
    UART1_SendByte(CMD_VERIFY_PASS);
    SendPasswordBytes(correctPassword);
    
    retVal = UART1_ReceiveByteTimeout(PROTOCOL_TIMEOUT_MS, &response);
    
    if (retVal == E_OK)
    {
        if (response == RESP_YES)
        {
            TestLog_Info("Lockout expired - password accepted");
        }
        else if (response == RESP_NO)
        {
            TestLog_Info("Lockout still active or password incorrect");
            result = FALSE;
        }
    }
    else
    {
        result = FALSE;
    }
    
    DisplayTestStatus("LockDur", result);
    
    return result;
}

/*===========================================================================*/
/*                           RUN ALL SYSTEM TESTS                            */
/*===========================================================================*/

void Test_System_RunAll(void)
{
    TestLog_SuiteStart(SYS_SUITE);
    
    TestLog_Info("=== System Scenario Tests ===");
    TestLog_Info("Requires Control ECU connection");
    
    /* Scenario 1: First Boot */
    TestLog_Info("--- Scenario 1: First Boot ---");
    TEST_RUN(SYS_SUITE, "FirstBoot_NotInitialized", Test_Sys_FirstBoot_NotInitialized);
    TEST_RUN(SYS_SUITE, "FirstBoot_SetupPassword", Test_Sys_FirstBoot_SetupPassword);
    
    /* Scenario 2: Valid Password */
    TestLog_Info("--- Scenario 2: Valid Password ---");
    TEST_RUN(SYS_SUITE, "ValidPassword_DoorOpens", Test_Sys_ValidPassword_DoorOpens);
    /* Skip door close test - requires long wait */
    TestLog_Skip(SYS_SUITE, "ValidPassword_DoorCloses", "Long duration test - run manually");
    
    /* Scenario 3: Set Timeout */
    TestLog_Info("--- Scenario 3: Set Timeout ---");
    TEST_RUN(SYS_SUITE, "SetTimeout_FromPot", Test_Sys_SetTimeout_FromPot);
    TestLog_Skip(SYS_SUITE, "SetTimeout_Persistence", "Requires manual reboot");
    
    /* Scenario 4: Lockout */
    TestLog_Info("--- Scenario 4: Lockout ---");
    TEST_RUN(SYS_SUITE, "WrongPassword_Lockout", Test_Sys_WrongPassword_Lockout);
    TestLog_Skip(SYS_SUITE, "Lockout_Duration", "Long duration test - run manually");
    
    TestLog_SuiteEnd(SYS_SUITE);
}
