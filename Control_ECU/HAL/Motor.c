#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "Motor.h"

#include "../MCAL/delay.h"

/* Motor on L293D channel 1
 * IN1 -> PB2
 * IN2 -> PB3
 * EN1 tied HIGH externally
 */

#define SYSCTL_RCGCGPIO_PORTB_MASK   (1u << 1)

#define MOTOR_IN1_MASK               (1u << 2)  /* PB2 */
#define MOTOR_IN2_MASK               (1u << 3)  /* PB3 */
#define MOTOR_PINS_MASK              (MOTOR_IN1_MASK | MOTOR_IN2_MASK)

/* Tune this for your lock mechanics */
#define MOTOR_RUN_MS                 (2000u)
#define MOTOR_BRAKE_MS               (20u)       /* short stop between direction changes */

void Motor_Init(void)
{
    /* Enable clock for Port B */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_PORTB_MASK;
    (void)SYSCTL_RCGCGPIO_R;

    /* PB2, PB3 as digital outputs */
    GPIO_PORTB_DIR_R |= MOTOR_PINS_MASK;
    GPIO_PORTB_DEN_R |= MOTOR_PINS_MASK;

    /* Make sure analog/alt functions are disabled on PB2/PB3 */
    GPIO_PORTB_AFSEL_R &= ~MOTOR_PINS_MASK;
    GPIO_PORTB_AMSEL_R &= ~MOTOR_PINS_MASK;

    /* Ensure pins are GPIO in PCTL */
    GPIO_PORTB_PCTL_R &= ~((0xFu << 8) | (0xFu << 12));

    Motor_Stop();
}

void Motor_Stop(void)
{
    GPIO_PORTB_DATA_R &= ~MOTOR_PINS_MASK;
}

void Motor_Open(void)
{
    /* Stop first to avoid shoot-through during direction change */
    Motor_Stop();
    Delay_ms(MOTOR_BRAKE_MS);

    /* Direction: IN1=1, IN2=0 -> forward (unlock) */
    GPIO_PORTB_DATA_R |=  MOTOR_IN1_MASK;
    GPIO_PORTB_DATA_R &= ~MOTOR_IN2_MASK;

    Delay_ms(MOTOR_RUN_MS);
    Motor_Stop();
}

void Motor_Close(void)
{
    /* Stop first to avoid shoot-through during direction change */
    Motor_Stop();
    Delay_ms(MOTOR_BRAKE_MS);

    /* Direction: IN1=0, IN2=1 -> reverse (lock) */
    GPIO_PORTB_DATA_R &= ~MOTOR_IN1_MASK;
    GPIO_PORTB_DATA_R |=  MOTOR_IN2_MASK;

    Delay_ms(MOTOR_RUN_MS);
    Motor_Stop();
}
