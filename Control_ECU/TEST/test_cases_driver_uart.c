/**
 * @file    test_cases_driver_uart.c
 * @brief   UART Driver Unit Tests Implementation - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Tests UART register configuration.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_driver_uart.h"
#include "test_config.h"
#include "test_log.h"
#include "../MCAL/UART.h"

/*===========================================================================*/
/*                           REGISTER DEFINITIONS                            */
/*===========================================================================*/

#define UART1_CTL_R       (*((volatile uint32_t *)0x4000D030u))
#define UART1_IBRD_R      (*((volatile uint32_t *)0x4000D024u))
#define UART1_FBRD_R      (*((volatile uint32_t *)0x4000D028u))

#define UART_CTL_UARTEN   (0x0001u)
#define UART_CTL_TXE      (0x0100u)
#define UART_CTL_RXE      (0x0200u)

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define UART_SUITE        "UART"

/*===========================================================================*/
/*                           TEST IMPLEMENTATIONS                            */
/*===========================================================================*/

uint8_t Test_UART_Init_SetsControlRegisters(void)
{
    uint8_t result = TRUE;
    uint32_t ctlReg;
    
    /* Execute */
    UART1_Init(9600u);
    
    /* Verify control register bits */
    ctlReg = UART1_CTL_R;
    
    if ((ctlReg & UART_CTL_UARTEN) == 0u)
    {
        result = FALSE;
    }
    
    if ((ctlReg & UART_CTL_TXE) == 0u)
    {
        result = FALSE;
    }
    
    if ((ctlReg & UART_CTL_RXE) == 0u)
    {
        result = FALSE;
    }
    
    return result;
}

uint8_t Test_UART_Init_SetsBaudRate(void)
{
    uint8_t result = TRUE;
    uint32_t ibrd;
    
    /* Execute */
    UART1_Init(9600u);
    
    /* Verify baud rate register is configured */
    ibrd = UART1_IBRD_R;
    
    if (ibrd == 0u)
    {
        result = FALSE;
    }
    
    return result;
}

uint8_t Test_UART_SendByte_Completes(void)
{
    uint8_t result = TRUE;
    
    /* Ensure UART is initialized */
    UART1_Init(9600u);
    
    /* Execute - send test byte */
    UART1_SendByte((uint8_t)'T');
    
    /* If we got here without hanging, send completed */
    
    return result;
}

void Test_UART_RunAll(void)
{
    TestLog_SuiteStart(UART_SUITE);
    
    TEST_RUN(UART_SUITE, "Init_SetsControlRegisters", Test_UART_Init_SetsControlRegisters);
    TEST_RUN(UART_SUITE, "Init_SetsBaudRate", Test_UART_Init_SetsBaudRate);
    TEST_RUN(UART_SUITE, "SendByte_Completes", Test_UART_SendByte_Completes);
    
    TestLog_SuiteEnd(UART_SUITE);
}
