/**
 * @file    test_config.h
 * @brief   Test Framework Configuration - HMI ECU
 * @project Embedded Door-Lock System
 * @target  TM4C123GH6PM
 * @version 1.0
 * 
 * @details Global configuration for the test framework including
 *          test selection, logging options, and hardware settings.
 * 
 * MISRA-C:2012 Compliant
 */

#ifndef TEST_CONFIG_H
#define TEST_CONFIG_H

/*===========================================================================*/
/*                           TEST FRAMEWORK VERSION                          */
/*===========================================================================*/

#define TEST_FRAMEWORK_VERSION_MAJOR    (1u)
#define TEST_FRAMEWORK_VERSION_MINOR    (0u)
#define TEST_FRAMEWORK_VERSION_PATCH    (0u)

/*===========================================================================*/
/*                           TEST LEVEL SELECTION                            */
/*===========================================================================*/
/* Enable/disable test levels (1 = enabled, 0 = disabled)                    */

#define TEST_ENABLE_UNIT_TESTS          (1u)
#define TEST_ENABLE_INTEGRATION_TESTS   (1u)
#define TEST_ENABLE_SYSTEM_TESTS        (1u)

/*===========================================================================*/
/*                           UNIT TEST SELECTION                             */
/*===========================================================================*/

#define TEST_UNIT_GPIO                  (1u)
#define TEST_UNIT_UART                  (1u)
#define TEST_UNIT_ADC                   (1u)
#define TEST_UNIT_I2C                   (1u)
#define TEST_UNIT_DELAY                 (1u)
#define TEST_UNIT_LCD                   (1u)
#define TEST_UNIT_KEYPAD                (1u)
#define TEST_UNIT_BUZZER                (1u)

/*===========================================================================*/
/*                       INTEGRATION TEST SELECTION                          */
/*===========================================================================*/

#define TEST_INT_KEYPAD_LCD             (1u)
#define TEST_INT_ADC_LCD                (1u)
#define TEST_INT_UART_PROTOCOL          (1u)

/*===========================================================================*/
/*                         SYSTEM TEST SELECTION                             */
/*===========================================================================*/

#define TEST_SYS_FIRST_BOOT             (1u)
#define TEST_SYS_VALID_PASSWORD         (1u)
#define TEST_SYS_SET_TIMEOUT            (1u)
#define TEST_SYS_LOCKOUT                (1u)

/*===========================================================================*/
/*                           LOGGING CONFIGURATION                           */
/*===========================================================================*/

#define TEST_LOG_UART_BAUDRATE          (9600u)
#define TEST_LOG_BUFFER_SIZE            (128u)
#define TEST_LOG_ENABLE_TIMESTAMPS      (1u)
#define TEST_LOG_ENABLE_COLORS          (0u)  /* ANSI colors - disable for simple terminals */

/*===========================================================================*/
/*                           TIMEOUT CONFIGURATION                           */
/*===========================================================================*/

#define TEST_DEFAULT_TIMEOUT_MS         (1000u)
#define TEST_UART_TIMEOUT_MS            (600u)
#define TEST_ADC_TIMEOUT_MS             (100u)
#define TEST_KEYPAD_TIMEOUT_MS          (5000u)

/*===========================================================================*/
/*                           HARDWARE CONFIGURATION                          */
/*===========================================================================*/

/* TM4C123GH6PM Register Base Addresses */
#define SYSCTL_BASE                     (0x400FE000u)
#define GPIO_PORTA_BASE                 (0x40004000u)
#define GPIO_PORTB_BASE                 (0x40005000u)
#define GPIO_PORTC_BASE                 (0x40006000u)
#define GPIO_PORTD_BASE                 (0x40007000u)
#define GPIO_PORTE_BASE                 (0x40024000u)
#define GPIO_PORTF_BASE                 (0x40025000u)
#define UART0_BASE                      (0x4000C000u)
#define UART1_BASE                      (0x4000D000u)
#define ADC0_BASE                       (0x40038000u)
#define I2C0_BASE                       (0x40020000u)

/* Register Offsets */
#define GPIO_DATA_OFFSET                (0x3FCu)
#define GPIO_DIR_OFFSET                 (0x400u)
#define GPIO_DEN_OFFSET                 (0x51Cu)
#define GPIO_AFSEL_OFFSET               (0x420u)
#define GPIO_PCTL_OFFSET                (0x52Cu)

#define UART_CTL_OFFSET                 (0x030u)
#define UART_IBRD_OFFSET                (0x024u)
#define UART_FBRD_OFFSET                (0x028u)

#define ADC_ACTSS_OFFSET                (0x000u)
#define ADC_SS3_OFFSET                  (0x0A0u)

/*===========================================================================*/
/*                           TEST ASSERTIONS                                 */
/*===========================================================================*/

#define TEST_ASSERT_EQUAL(expected, actual)     \
    do {                                        \
        if ((expected) != (actual)) {           \
            return FALSE;                       \
        }                                       \
    } while (0)

#define TEST_ASSERT_NOT_EQUAL(val1, val2)       \
    do {                                        \
        if ((val1) == (val2)) {                 \
            return FALSE;                       \
        }                                       \
    } while (0)

#define TEST_ASSERT_TRUE(condition)             \
    do {                                        \
        if (!(condition)) {                     \
            return FALSE;                       \
        }                                       \
    } while (0)

#define TEST_ASSERT_IN_RANGE(val, min, max)     \
    do {                                        \
        if (((val) < (min)) || ((val) > (max))) { \
            return FALSE;                       \
        }                                       \
    } while (0)

#endif /* TEST_CONFIG_H */
