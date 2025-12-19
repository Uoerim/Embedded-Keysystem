/**
 * @file    test_cases_driver_i2c.c
 * @brief   I2C Driver Unit Tests Implementation - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Tests I2C master configuration and error handling.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_driver_i2c.h"
#include "test_config.h"
#include "test_log.h"
#include "../MCAL/I2C.h"

/*===========================================================================*/
/*                           REGISTER DEFINITIONS                            */
/*===========================================================================*/

/* I2C0 Registers */
#define I2C0_MCR_R        (*((volatile uint32_t *)0x40020020u))
#define I2C0_MTPR_R       (*((volatile uint32_t *)0x4002000Cu))
#define I2C0_MCS_R        (*((volatile uint32_t *)0x40020004u))

/* Master Control Register bits */
#define I2C_MCR_MFE       (0x10u)  /* Master Function Enable */

/* Invalid I2C address for testing */
#define TEST_INVALID_I2C_ADDR   (0x7Eu)

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define I2C_SUITE         "I2C"

/*===========================================================================*/
/*                           TEST IMPLEMENTATIONS                            */
/*===========================================================================*/

boolean Test_I2C_Init_ConfiguresMaster(void)
{
    boolean result = TRUE;
    uint32_t mcr;
    uint32_t mtpr;
    
    /* Execute */
    I2C0_Init_100k_16MHz();
    
    /* Verify Master Function Enable is set */
    mcr = I2C0_MCR_R;
    
    if ((mcr & I2C_MCR_MFE) == 0u)
    {
        result = FALSE;
    }
    
    /* Verify Timer Period Register is configured */
    mtpr = I2C0_MTPR_R;
    
    /* For 100kHz at 16MHz: TPR = (16MHz / (2 * 10 * 100kHz)) - 1 = 7 */
    if (mtpr == 0u)
    {
        result = FALSE;
    }
    
    return result;
}

boolean Test_I2C_Write_InvalidAddress_Timeout(void)
{
    boolean result = TRUE;
    Std_ReturnType retVal;
    
    /* Ensure I2C is initialized */
    I2C0_Init_100k_16MHz();
    
    /* Execute - write to non-existent device should fail */
    retVal = I2C0_WriteByte(TEST_INVALID_I2C_ADDR, 0x00u, 100u);
    
    /* Should return E_NOT_OK due to no ACK */
    if (retVal == E_OK)
    {
        /* If something ACKed this address, unexpected */
        result = FALSE;
    }
    
    return result;
}

void Test_I2C_RunAll(void)
{
    TestLog_SuiteStart(I2C_SUITE);
    
    TEST_RUN(I2C_SUITE, "Init_ConfiguresMaster", Test_I2C_Init_ConfiguresMaster);
    TEST_RUN(I2C_SUITE, "Write_InvalidAddress_Timeout", Test_I2C_Write_InvalidAddress_Timeout);
    
    TestLog_SuiteEnd(I2C_SUITE);
}
