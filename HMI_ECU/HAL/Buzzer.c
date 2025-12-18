#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "../MCAL/delay.h"
#include "Buzzer.h"

/* Buzzer on PF2 */
#define SYSCTL_RCGCGPIO_PORTF_MASK  (1u << 5)
#define BUZZER_PIN_MASK            (1u << 2)

#define BUZZER_BEEP_MS             (80u)

void Buzzer_Init(void)
{
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_PORTF_MASK;
    (void)SYSCTL_RCGCGPIO_R;

    GPIO_PORTF_DIR_R |= BUZZER_PIN_MASK;
    GPIO_PORTF_DEN_R |= BUZZER_PIN_MASK;

    GPIO_PORTF_AFSEL_R &= ~BUZZER_PIN_MASK;
    GPIO_PORTF_AMSEL_R &= ~BUZZER_PIN_MASK;

    Buzzer_Off();
}

void Buzzer_On(void)
{
    GPIO_PORTF_DATA_R |= BUZZER_PIN_MASK;
}

void Buzzer_Off(void)
{
    GPIO_PORTF_DATA_R &= ~BUZZER_PIN_MASK;
}

void Buzzer_BeepShort(void)
{
    Buzzer_On();
    Delay_ms(BUZZER_BEEP_MS);
    Buzzer_Off();
}
