#include "RGB_LED.h"
#include "TM4C123GH6PM.h"

/* External RGB on Port C:
 * PC5 = Red, PC6 = Green, PC7 = Blue
 * Common cathode, ON = HIGH
 */

#define RGB_RED_PIN     (1u << 5)   /* PC5 */
#define RGB_GREEN_PIN   (1u << 6)   /* PC6 */
#define RGB_BLUE_PIN    (1u << 7)   /* PC7 */
#define RGB_ALL_PINS    (RGB_RED_PIN | RGB_GREEN_PIN | RGB_BLUE_PIN)

#define SYSCTL_RCGCGPIO_PORTC_MASK  (1u << 2)

void RGB_LED_Init(void)
{
    /* Enable Port C clock */
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_PORTC_MASK;
    (void)SYSCTL_RCGCGPIO_R;

    /* PC5, PC6, PC7 as digital outputs */
    GPIO_PORTC_DIR_R |= RGB_ALL_PINS;
    GPIO_PORTC_DEN_R |= RGB_ALL_PINS;

    /* Disable analog + alternate function for these pins (good practice) */
    GPIO_PORTC_AFSEL_R &= ~RGB_ALL_PINS;
    GPIO_PORTC_AMSEL_R &= ~RGB_ALL_PINS;

    /* PCTL for PC5-7 -> GPIO (clear fields) */
    GPIO_PORTC_PCTL_R &= ~((0xFu << 20) | (0xFu << 24) | (0xFu << 28));

    /* All OFF initially */
    GPIO_PORTC_DATA_R &= ~RGB_ALL_PINS;
}

void RGB_LED_SetColor(RGB_Color_t color)
{
    /* Turn OFF first */
    GPIO_PORTC_DATA_R &= ~RGB_ALL_PINS;

    switch (color)
    {
        case RGB_RED:
            GPIO_PORTC_DATA_R |= RGB_RED_PIN;
            break;

        case RGB_GREEN:
            GPIO_PORTC_DATA_R |= RGB_GREEN_PIN;
            break;

        case RGB_BLUE:
            GPIO_PORTC_DATA_R |= RGB_BLUE_PIN;
            break;

        case RGB_YELLOW:
            GPIO_PORTC_DATA_R |= (RGB_RED_PIN | RGB_GREEN_PIN);
            break;

        case RGB_CYAN:
            GPIO_PORTC_DATA_R |= (RGB_GREEN_PIN | RGB_BLUE_PIN);
            break;

        case RGB_MAGENTA:
            GPIO_PORTC_DATA_R |= (RGB_RED_PIN | RGB_BLUE_PIN);
            break;

        case RGB_WHITE:
            GPIO_PORTC_DATA_R |= RGB_ALL_PINS;
            break;

        case RGB_OFF:
        default:
            /* already OFF */
            break;
    }
}
