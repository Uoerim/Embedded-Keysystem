/**
 * @file    test_cases_driver_motor.c
 * @brief   Motor Driver Unit Tests Implementation - Control ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Tests motor GPIO configuration and direction control.
 * 
 * MISRA-C:2012 Compliant
 */

#include "test_cases_driver_motor.h"
#include "test_config.h"
#include "test_log.h"
#include "../HAL/Motor.h"
#include "../MCAL/Delay.h"

/*===========================================================================*/
/*                           MOTOR PIN DEFINITIONS                           */
/*===========================================================================*/

/* Motor control pins - adjust based on actual hardware */
#define MOTOR_IN1_PIN         (0x01u)  /* PB0 */
#define MOTOR_IN2_PIN         (0x02u)  /* PB1 */
#define MOTOR_ENABLE_PIN      (0x04u)  /* PB2 - if used */
#define MOTOR_ALL_PINS        (MOTOR_IN1_PIN | MOTOR_IN2_PIN)

/*===========================================================================*/
/*                           TEST SUITE NAME                                 */
/*===========================================================================*/

#define MOTOR_SUITE       "Motor"

/*===========================================================================*/
/*                           TEST IMPLEMENTATIONS                            */
/*===========================================================================*/

uint8_t Test_Motor_Init_ConfiguresGPIO(void)
{
    uint8_t result = TRUE;
    uint32_t dirBits;
    uint32_t denBits;
    
    /* Execute */
    Motor_Init();
    
    /* Verify motor pins are configured as outputs */
    dirBits = GPIO_PORTB_DIR_R & MOTOR_ALL_PINS;
    if (dirBits != MOTOR_ALL_PINS)
    {
        /* Motor pins should be outputs */
        result = FALSE;
    }
    
    /* Verify motor pins are digitally enabled */
    denBits = GPIO_PORTB_DEN_R & MOTOR_ALL_PINS;
    if (denBits != MOTOR_ALL_PINS)
    {
        result = FALSE;
    }
    
    return result;
}

uint8_t Test_Motor_Open_SetsDirectionPins(void)
{
    uint8_t result = TRUE;
    uint32_t dataBits;
    
    /* Setup */
    Motor_Init();
    
    /* Execute - open door (unlock direction) */
    Motor_Open();
    
    /* Small delay to allow motor command to take effect */
    Delay_ms(10u);
    
    /* Verify direction pins are set for "open" direction */
    /* Typical: IN1=1, IN2=0 for forward rotation */
    dataBits = GPIO_PORTB_DATA_R & MOTOR_ALL_PINS;
    
    /* At least one direction pin should be set */
    if (dataBits == 0u)
    {
        result = FALSE;
    }
    
    /* Stop motor after test */
    Motor_Stop();
    
    return result;
}

uint8_t Test_Motor_Close_SetsDirectionPins(void)
{
    uint8_t result = TRUE;
    uint32_t dataBits;
    
    /* Setup */
    Motor_Init();
    
    /* Execute - close door (lock direction) */
    Motor_Close();
    
    /* Small delay */
    Delay_ms(10u);
    
    /* Verify direction pins are set for "close" direction */
    /* Typical: IN1=0, IN2=1 for reverse rotation */
    dataBits = GPIO_PORTB_DATA_R & MOTOR_ALL_PINS;
    
    /* At least one direction pin should be set */
    if (dataBits == 0u)
    {
        result = FALSE;
    }
    
    /* Stop motor after test */
    Motor_Stop();
    
    return result;
}

uint8_t Test_Motor_Stop_ClearsPins(void)
{
    uint8_t result = TRUE;
    uint32_t dataBits;
    
    /* Setup - start motor first */
    Motor_Init();
    Motor_Open();
    Delay_ms(10u);
    
    /* Execute - stop motor */
    Motor_Stop();
    
    /* Small delay for stop to take effect */
    Delay_ms(10u);
    
    /* Verify motor pins are cleared (motor stopped) */
    dataBits = GPIO_PORTB_DATA_R & MOTOR_ALL_PINS;
    
    if (dataBits != 0u)
    {
        /* Motor should be stopped (all direction pins low) */
        result = FALSE;
    }
    
    return result;
}

void Test_Motor_RunAll(void)
{
    TestLog_SuiteStart(MOTOR_SUITE);
    
    TEST_RUN(MOTOR_SUITE, "Init_ConfiguresGPIO", Test_Motor_Init_ConfiguresGPIO);
    TEST_RUN(MOTOR_SUITE, "Open_SetsDirectionPins", Test_Motor_Open_SetsDirectionPins);
    TEST_RUN(MOTOR_SUITE, "Close_SetsDirectionPins", Test_Motor_Close_SetsDirectionPins);
    TEST_RUN(MOTOR_SUITE, "Stop_ClearsPins", Test_Motor_Stop_ClearsPins);
    
    TestLog_SuiteEnd(MOTOR_SUITE);
}
