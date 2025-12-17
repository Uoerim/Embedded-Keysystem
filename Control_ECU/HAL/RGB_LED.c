#include "RGB_LED.h"
#include "TM4C123GH6PM.h"

/* PF1 = Red, PF2 = Blue, PF3 = Green */
#define RED_PIN    (1U << 1)
#define BLUE_PIN   (1U << 2)
#define GREEN_PIN  (1U << 3)

void RGB_LED_Init(void)
{
    /* Enable Port F */
    SYSCTL_RCGCGPIO_R |= (1U << 5);
    (void)SYSCTL_RCGCGPIO_R;

    /* Unlock PF0 not needed, but safe practice */
    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R  |= (RED_PIN | BLUE_PIN | GREEN_PIN);

    GPIO_PORTF_DIR_R |= (RED_PIN | BLUE_PIN | GREEN_PIN);
    GPIO_PORTF_DEN_R |= (RED_PIN | BLUE_PIN | GREEN_PIN);

    /* All OFF initially */
    GPIO_PORTF_DATA_R &= ~(RED_PIN | BLUE_PIN | GREEN_PIN);
}

void RGB_LED_SetColor(RGB_Color_t color)
{
    /* Turn everything OFF first */
    GPIO_PORTF_DATA_R &= ~(RED_PIN | BLUE_PIN | GREEN_PIN);

    switch (color)
    {
        case RGB_RED:
            GPIO_PORTF_DATA_R |= RED_PIN;
            break;

        case RGB_GREEN:
            GPIO_PORTF_DATA_R |= GREEN_PIN;
            break;

        case RGB_BLUE:
            GPIO_PORTF_DATA_R |= BLUE_PIN;
            break;

        case RGB_YELLOW:
            GPIO_PORTF_DATA_R |= (RED_PIN | GREEN_PIN);
            break;

        case RGB_CYAN:
            GPIO_PORTF_DATA_R |= (GREEN_PIN | BLUE_PIN);
            break;

        case RGB_MAGENTA:
            GPIO_PORTF_DATA_R |= (RED_PIN | BLUE_PIN);
            break;

        case RGB_WHITE:
            GPIO_PORTF_DATA_R |= (RED_PIN | GREEN_PIN | BLUE_PIN);
            break;

        case RGB_OFF:
        default:
            /* already OFF */
            break;
    }
}
