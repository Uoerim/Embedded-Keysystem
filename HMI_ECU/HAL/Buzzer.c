#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "Buzzer.h"

// Buzzer on PF2 (via NPN transistor + 1k resistor)

#define BUZZER_PIN   (1U << 2)

static void buzzer_delay(volatile uint32_t t)
{
    while (t--) { }
}

void Buzzer_Init(void)
{
    // Enable clock for Port F
    SYSCTL_RCGCGPIO_R |= (1U << 5);
    (void)SYSCTL_RCGCGPIO_R;

    // PF2 as output
    GPIO_PORTF_DIR_R |= BUZZER_PIN;
    GPIO_PORTF_DEN_R |= BUZZER_PIN;

    Buzzer_Off();
}

void Buzzer_On(void)
{
    GPIO_PORTF_DATA_R |= BUZZER_PIN;
}

void Buzzer_Off(void)
{
    GPIO_PORTF_DATA_R &= ~BUZZER_PIN;
}

void Buzzer_BeepShort(void)
{
    Buzzer_On();
    buzzer_delay(80000);   // adjust beep length
    Buzzer_Off();
}
