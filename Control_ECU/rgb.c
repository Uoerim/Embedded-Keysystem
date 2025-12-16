#include "rgb.h"
#include "tm4c123gh6pm.h"

void RGB_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x20;      /* Enable clock for Port F */
    while((SYSCTL_PRGPIO_R & 0x20) == 0);

    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R |= 0x0E;

    GPIO_PORTF_DIR_R |= 0x0E;       /* PF1, PF2, PF3 output */
    GPIO_PORTF_DEN_R |= 0x0E;
    GPIO_PORTF_DATA_R &= ~0x0E;
}

void RGB_SetColor(uint8_t color)
{
    GPIO_PORTF_DATA_R = (GPIO_PORTF_DATA_R & ~0x0E) | color;
}
