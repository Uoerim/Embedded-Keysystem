#include "ADC.h"
#include "TM4C123GH6PM.h"
#include "../MCAL/delay.h"

void ADC_Init(void)
{
    SYSCTL_RCGCGPIO_R |= (1u << 1);  /* Port B */
    SYSCTL_RCGCADC_R  |= (1u << 0);  /* ADC0 */

    (void)SYSCTL_RCGCGPIO_R;
    (void)SYSCTL_RCGCADC_R;

    GPIO_PORTB_AFSEL_R |= (1u << 4);
    GPIO_PORTB_DEN_R   &= ~(1u << 4);
    GPIO_PORTB_AMSEL_R |= (1u << 4);

    ADC0_ACTSS_R &= ~(1u << 3);
    ADC0_EMUX_R  &= ~0xF000u;
    ADC0_SSMUX3_R = 10u;
    ADC0_SSCTL3_R = (1u << 1) | (1u << 2);
    ADC0_ACTSS_R |= (1u << 3);
}

Std_ReturnType ADC_ReadTimeout(uint32 timeout_ms, uint16 *out)
{
    uint32 waited = 0u;

    if (out == (uint16*)0) { return E_NOT_OK; }

    ADC0_PSSI_R = (1u << 3);

    while ((ADC0_RIS_R & (1u << 3)) == 0u)
    {
        if (waited >= timeout_ms) { return E_NOT_OK; }
        Delay_ms(1u);
        waited++;
    }

    *out = (uint16)(ADC0_SSFIFO3_R & 0x0FFFu);
    ADC0_ISC_R = (1u << 3);

    return E_OK;
}
