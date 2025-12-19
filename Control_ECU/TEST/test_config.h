/**
 * @file    test_config.h
 * @brief   Test Framework Configuration - Control ECU
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

#define TEST_ENABLE_UNIT_TESTS          (1u)
#define TEST_ENABLE_INTEGRATION_TESTS   (1u)
#define TEST_ENABLE_SYSTEM_TESTS        (1u)

/*===========================================================================*/
/*                           UNIT TEST SELECTION                             */
/*===========================================================================*/

#define TEST_UNIT_RGB_LED               (1u)
#define TEST_UNIT_MOTOR                 (1u)
#define TEST_UNIT_EEPROM                (1u)
#define TEST_UNIT_UART                  (1u)
#define TEST_UNIT_DELAY                 (1u)

/*===========================================================================*/
/*                       INTEGRATION TEST SELECTION                          */
/*===========================================================================*/

#define TEST_INT_UART_PROTOCOL          (1u)
#define TEST_INT_EEPROM_PERSIST         (1u)
#define TEST_INT_MOTOR_SEQUENCE         (1u)

/*===========================================================================*/
/*                         SYSTEM TEST SELECTION                             */
/*===========================================================================*/

#define TEST_SYS_COMMAND_HANDLER        (1u)
#define TEST_SYS_PASSWORD_FLOW          (1u)
#define TEST_SYS_DOOR_SEQUENCE          (1u)

/*===========================================================================*/
/*                           LOGGING CONFIGURATION                           */
/*===========================================================================*/

#define TEST_LOG_UART_BAUDRATE          (9600u)
#define TEST_LOG_BUFFER_SIZE            (128u)
#define TEST_LOG_ENABLE_TIMESTAMPS      (1u)

/*===========================================================================*/
/*                           TIMEOUT CONFIGURATION                           */
/*===========================================================================*/

#define TEST_DEFAULT_TIMEOUT_MS         (1000u)
#define TEST_UART_TIMEOUT_MS            (600u)
#define TEST_EEPROM_TIMEOUT_MS          (100u)

/*===========================================================================*/
/*                           HARDWARE CONFIGURATION                          */
/*===========================================================================*/

/* GPIO Port C - RGB LED */
#define GPIO_PORTC_DATA_R     (*((volatile uint32_t *)0x400063FCu))
#define GPIO_PORTC_DIR_R      (*((volatile uint32_t *)0x40006400u))
#define GPIO_PORTC_DEN_R      (*((volatile uint32_t *)0x4000651Cu))

/* RGB LED pins on Port C */
#define RGB_RED_PIN           (0x20u)  /* PC5 */
#define RGB_GREEN_PIN         (0x40u)  /* PC6 */
#define RGB_BLUE_PIN          (0x80u)  /* PC7 */
#define RGB_ALL_PINS          (RGB_RED_PIN | RGB_GREEN_PIN | RGB_BLUE_PIN)

/* Motor pins (assumed Port B) */
#define GPIO_PORTB_DATA_R     (*((volatile uint32_t *)0x400053FCu))
#define GPIO_PORTB_DIR_R      (*((volatile uint32_t *)0x40005400u))
#define GPIO_PORTB_DEN_R      (*((volatile uint32_t *)0x4000551Cu))

/* EEPROM base */
#define EEPROM_BASE           (0x400AF000u)

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
