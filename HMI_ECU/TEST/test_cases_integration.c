/**
 * @file    test_cases_integration.c
 * @brief   Integration Tests Implementation - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Tests cross-module interactions and UART protocol.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_integration.h"
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

/* UART Protocol Commands */
#define CMD_CHECK_INIT      ((uint8_t)'I')
#define CMD_VERIFY_PASS     ((uint8_t)'V')
#define CMD_SET_PASS        ((uint8_t)'N')
#define CMD_SET_TIMEOUT     ((uint8_t)'T')
#define CMD_GET_TIMEOUT     ((uint8_t)'G')
#define CMD_OPEN_DOOR       ((uint8_t)'O')
#define CMD_CLOSE_DOOR      ((uint8_t)'C')

/* Response codes */
#define RESP_ACK            ((uint8_t)'K')
#define RESP_YES            ((uint8_t)'Y')
#define RESP_NO             ((uint8_t)'N')
#define RESP_INIT_YES       ((uint8_t)'1')
#define RESP_INIT_NO        ((uint8_t)'0')

/* Timeout values */
#define PROTOCOL_TIMEOUT_MS (600u)

/* Password length */
#define PASSWORD_LENGTH     (5u)

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define INT_SUITE           "Integration"

/*===========================================================================*/
/*                           HELPER FUNCTIONS                                */
/*===========================================================================*/

/**
 * @brief Send command and receive response with timeout
 */
static Std_ReturnType SendCommand(uint8_t cmd, uint8_t *response)
{
    UART1_FlushRx();
    UART1_SendByte(cmd);
    return UART1_ReceiveByteTimeout(PROTOCOL_TIMEOUT_MS, response);
}

/**
 * @brief Send password bytes
 */
static void SendPassword(const char password[PASSWORD_LENGTH])
{
    uint8_t i;
    for (i = 0u; i < PASSWORD_LENGTH; i++)
    {
        UART1_SendByte((uint8_t)password[i]);
    }
}

/*===========================================================================*/
/*                       KEYPAD + LCD TESTS                                  */
/*===========================================================================*/

boolean Test_Int_Keypad_LCD_AsteriskDisplay(void)
{
    boolean result = TRUE;
    Std_ReturnType retVal;
    char key = '\0';
    
    /* Initialize peripherals */
    LCD_Init();
    Keypad_Init();
    
    /* Display prompt */
    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString("Press any key:");
    LCD_SetCursor(1u, 0u);
    
    /* Wait for keypress with timeout */
    retVal = Keypad_GetKeyTimeout(TEST_KEYPAD_TIMEOUT_MS, &key);
    
    if (retVal == E_OK)
    {
        /* Display asterisk for password masking */
        LCD_SendChar('*');
        
        /* Verify key is valid (0-9, A-D, *, #) */
        if ((key >= '0') && (key <= '9'))
        {
            /* Numeric key - valid */
        }
        else if ((key >= 'A') && (key <= 'D'))
        {
            /* Function key - valid */
        }
        else if ((key == '*') || (key == '#'))
        {
            /* Special key - valid */
        }
        else
        {
            result = FALSE;
        }
    }
    else
    {
        /* Timeout - mark as skipped (requires manual interaction) */
        TestLog_Skip(INT_SUITE, "Keypad_LCD_AsteriskDisplay", 
                     "No key pressed within timeout");
        return TRUE;  /* Don't fail for manual tests */
    }
    
    return result;
}

/*===========================================================================*/
/*                        ADC + LCD TESTS                                    */
/*===========================================================================*/

boolean Test_Int_ADC_LCD_TimeoutDisplay(void)
{
    boolean result = TRUE;
    uint16 adcValue = 0u;
    uint8_t timeout;
    char displayStr[17];
    Std_ReturnType retVal;
    
    /* Initialize peripherals */
    LCD_Init();
    ADC_Init();
    
    /* Read ADC value */
    retVal = ADC_ReadTimeout(TEST_ADC_TIMEOUT_MS, &adcValue);
    
    if (retVal == E_OK)
    {
        /* Calculate timeout (5-30 seconds range) */
        timeout = (uint8_t)(5u + ((uint32_t)adcValue * 25u) / 4095u);
        
        /* Clamp to valid range */
        if (timeout < 5u)
        {
            timeout = 5u;
        }
        if (timeout > 30u)
        {
            timeout = 30u;
        }
        
        /* Display on LCD */
        LCD_Clear();
        LCD_SetCursor(0u, 0u);
        LCD_SendString("Timeout:");
        
        /* Convert timeout to string */
        displayStr[0] = (char)('0' + (timeout / 10u));
        displayStr[1] = (char)('0' + (timeout % 10u));
        displayStr[2] = 's';
        displayStr[3] = '\0';
        
        LCD_SetCursor(0u, 9u);
        LCD_SendString(displayStr);
        
        /* Verify timeout is in range */
        if ((timeout < 5u) || (timeout > 30u))
        {
            result = FALSE;
        }
    }
    else
    {
        result = FALSE;
    }
    
    return result;
}

/*===========================================================================*/
/*                       UART PROTOCOL TESTS                                 */
/*===========================================================================*/

boolean Test_Int_UART_Protocol_CheckInit(void)
{
    boolean result = TRUE;
    uint8_t response = 0u;
    Std_ReturnType retVal;
    
    /* Send 'I' command */
    retVal = SendCommand(CMD_CHECK_INIT, &response);
    
    if (retVal == E_OK)
    {
        /* Response should be '0' or '1' */
        if ((response != RESP_INIT_YES) && (response != RESP_INIT_NO))
        {
            result = FALSE;
        }
    }
    else
    {
        /* No response from Control ECU */
        result = FALSE;
    }
    
    return result;
}

boolean Test_Int_UART_Protocol_VerifyPassword(void)
{
    boolean result = TRUE;
    uint8_t response = 0u;
    Std_ReturnType retVal;
    const char testPassword[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
    
    /* Send 'V' command */
    UART1_FlushRx();
    UART1_SendByte(CMD_VERIFY_PASS);
    
    /* Send password */
    SendPassword(testPassword);
    
    /* Wait for response */
    retVal = UART1_ReceiveByteTimeout(PROTOCOL_TIMEOUT_MS, &response);
    
    if (retVal == E_OK)
    {
        /* Response should be 'Y' or 'N' */
        if ((response != RESP_YES) && (response != RESP_NO))
        {
            result = FALSE;
        }
    }
    else
    {
        result = FALSE;
    }
    
    return result;
}

boolean Test_Int_UART_Protocol_SetPassword(void)
{
    boolean result = TRUE;
    uint8_t response = 0u;
    Std_ReturnType retVal;
    const char testPassword[PASSWORD_LENGTH] = {'1', '2', '3', '4', '5'};
    
    /* Send 'N' command */
    UART1_FlushRx();
    UART1_SendByte(CMD_SET_PASS);
    
    /* Send password */
    SendPassword(testPassword);
    
    /* Wait for ACK */
    retVal = UART1_ReceiveByteTimeout(PROTOCOL_TIMEOUT_MS, &response);
    
    if (retVal == E_OK)
    {
        if (response != RESP_ACK)
        {
            result = FALSE;
        }
    }
    else
    {
        result = FALSE;
    }
    
    return result;
}

boolean Test_Int_UART_Protocol_SetTimeout(void)
{
    boolean result = TRUE;
    uint8_t response = 0u;
    Std_ReturnType retVal;
    uint8_t testTimeout = 15u;
    
    /* Send 'T' command */
    UART1_FlushRx();
    UART1_SendByte(CMD_SET_TIMEOUT);
    
    /* Send timeout value (as two digit characters) */
    UART1_SendByte((uint8_t)'1');  /* Tens digit */
    UART1_SendByte((uint8_t)'5');  /* Units digit */
    
    /* Wait for ACK */
    retVal = UART1_ReceiveByteTimeout(PROTOCOL_TIMEOUT_MS, &response);
    
    if (retVal == E_OK)
    {
        if (response != RESP_ACK)
        {
            result = FALSE;
        }
    }
    else
    {
        result = FALSE;
    }
    
    (void)testTimeout;  /* Used for documentation */
    
    return result;
}

boolean Test_Int_UART_Protocol_GetTimeout(void)
{
    boolean result = TRUE;
    uint8_t response = 0u;
    Std_ReturnType retVal;
    
    /* Send 'G' command */
    retVal = SendCommand(CMD_GET_TIMEOUT, &response);
    
    if (retVal == E_OK)
    {
        /* Response should be timeout value (5-30) */
        if ((response < 5u) || (response > 30u))
        {
            /* Invalid timeout value */
            result = FALSE;
        }
    }
    else
    {
        result = FALSE;
    }
    
    return result;
}

/*===========================================================================*/
/*                           RUN ALL INTEGRATION TESTS                       */
/*===========================================================================*/

void Test_Integration_RunAll(void)
{
    TestLog_SuiteStart(INT_SUITE);
    
    /* Keypad + LCD tests */
    TEST_RUN(INT_SUITE, "Keypad_LCD_AsteriskDisplay", Test_Int_Keypad_LCD_AsteriskDisplay);
    
    /* ADC + LCD tests */
    TEST_RUN(INT_SUITE, "ADC_LCD_TimeoutDisplay", Test_Int_ADC_LCD_TimeoutDisplay);
    
    /* UART Protocol tests */
    TestLog_Info("UART Protocol tests require Control ECU connection");
    TEST_RUN(INT_SUITE, "UART_Protocol_CheckInit", Test_Int_UART_Protocol_CheckInit);
    TEST_RUN(INT_SUITE, "UART_Protocol_SetPassword", Test_Int_UART_Protocol_SetPassword);
    TEST_RUN(INT_SUITE, "UART_Protocol_VerifyPassword", Test_Int_UART_Protocol_VerifyPassword);
    TEST_RUN(INT_SUITE, "UART_Protocol_SetTimeout", Test_Int_UART_Protocol_SetTimeout);
    TEST_RUN(INT_SUITE, "UART_Protocol_GetTimeout", Test_Int_UART_Protocol_GetTimeout);
    
    TestLog_SuiteEnd(INT_SUITE);
}
