#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "GPIO.h"



void GPIOF_InitLED(void)
{
    // Enable clock for Port F
    SYSCTL_RCGCGPIO_R |= (1U << 5);      // bit 5 ? Port F
    (void)SYSCTL_RCGCGPIO_R;             // dummy read to allow clock to start

    // Unlock PF0 if needed (not used here, but safe)
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R  |= (1U << 1);       // allow changes on PF1

    // Set PF1 as output
    GPIO_PORTF_DIR_R |= (1U << 1);       // PF1 output
    GPIO_PORTF_DEN_R |= (1U << 1);       // PF1 digital enable

    // Turn LED off initially
    GPIO_PORTF_DATA_R &= ~(1U << 1);
}

void GPIOF_ToggleRedLED(void)
{
    GPIO_PORTF_DATA_R ^= (1U << 1);      // toggle PF1
}
