#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "Motor.h"

/* Motor on L293D channel 1
 * IN1 -> PB2
 * IN2 -> PB3
 * EN1 (pin1) tied to 5V externally
 */

#define MOTOR_IN1   (1U << 2)   // PB2
#define MOTOR_IN2   (1U << 3)   // PB3

static void Motor_Delay(volatile uint32_t t)
{
    while (t--) { }
}

void Motor_Init(void)
{
    /* Enable clock for Port B */
    SYSCTL_RCGCGPIO_R |= (1U << 1);   // Port B
    (void)SYSCTL_RCGCGPIO_R;

    /* PB2, PB3 as digital outputs */
    GPIO_PORTB_DIR_R |= MOTOR_IN1 | MOTOR_IN2;
    GPIO_PORTB_DEN_R |= MOTOR_IN1 | MOTOR_IN2;

    /* Make sure motor is stopped */
    Motor_Stop();
}

void Motor_Stop(void)
{
    GPIO_PORTB_DATA_R &= ~(MOTOR_IN1 | MOTOR_IN2);
}

void Motor_Open(void)
{
    /* Direction: IN1=1, IN2=0 -> forward (unlock) */
    GPIO_PORTB_DATA_R |=  MOTOR_IN1;
    GPIO_PORTB_DATA_R &= ~MOTOR_IN2;

    /* Run motor for ~2 seconds (tune this for your door) */
    Motor_Delay(500000);
    Motor_Stop();
}

void Motor_Close(void)
{
    /* Direction: IN1=0, IN2=1 -> reverse (lock) */
    GPIO_PORTB_DATA_R &= ~MOTOR_IN1;
    GPIO_PORTB_DATA_R |=  MOTOR_IN2;

    /* Run motor for ~2 seconds */
    Motor_Delay(500000);
    Motor_Stop();
}
