#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "ADC.h"

// Using AIN10 = PB4

void ADC_Init(void)
{
    // Enable clock for GPIOB and ADC0
    SYSCTL_RCGCGPIO_R |= (1U << 1);  // Port B
    SYSCTL_RCGCADC_R  |= (1U << 0);  // ADC0

    // small delay
    for (volatile int i = 0; i < 1000; i++);

    // Configure PB4 as analog input
    GPIO_PORTB_AFSEL_R |= (1U << 4);    // alternate function
    GPIO_PORTB_DEN_R   &= ~(1U << 4);   // disable digital
    GPIO_PORTB_AMSEL_R |= (1U << 4);    // enable analog

    // ADC0 SS3 setup
    ADC0_ACTSS_R &= ~(1U << 3);         // disable SS3
    ADC0_EMUX_R  &= ~0xF000;            // software trigger for SS3
    ADC0_SSMUX3_R = 10;                 // AIN10 (PB4)
    ADC0_SSCTL3_R = (1U << 1) | (1U << 2); // IE0 + END0
    ADC0_ACTSS_R |= (1U << 3);          // enable SS3
}

uint16_t ADC_Read(void)
{
    ADC0_PSSI_R = (1U << 3);                // start SS3
    while ((ADC0_RIS_R & (1U << 3)) == 0);  // wait until done

    uint16_t result = ADC0_SSFIFO3_R & 0x0FFF; // 12-bit result

    ADC0_ISC_R = (1U << 3);                 // clear flag

    return result;   // 0–4095
}

