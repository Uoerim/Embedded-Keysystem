#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "Keypad.h"

// Rows: PD0�PD3 (output)
// Cols: PE1�PE4 (input w/ pull-up)

static const char keymap[4][4] =
{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

static void delay_small(volatile uint32_t t)
{
    while (t--) { }
}

void Keypad_Init(void)
{
    // Enable Port D + Port E
    SYSCTL_RCGCGPIO_R |= (1U << 3) | (1U << 4);
    (void)SYSCTL_RCGCGPIO_R;

    /** ROWS ? PD0�PD3 OUTPUT **/
    GPIO_PORTD_DIR_R |= 0x0F;   // PD0�3 = output
    GPIO_PORTD_DEN_R |= 0x0F;

    /** COLUMNS ? PE1�PE4 INPUT + PULL-UP **/
    GPIO_PORTE_DIR_R &= ~0x1E;  // PE1�4 = input
    GPIO_PORTE_DEN_R |=  0x1E;
    GPIO_PORTE_PUR_R |=  0x1E;  // enable pull-up

    // Set rows HIGH initially
    GPIO_PORTD_DATA_R |= 0x0F;
}

char Keypad_GetKey(void)
{
    while (1)
    {
        for (uint8_t row = 0; row < 4; row++)
        {
            // Set all rows HIGH
            GPIO_PORTD_DATA_R |= 0x0F;

            // Drive current row LOW
            GPIO_PORTD_DATA_R &= ~(1U << row);

            delay_small(1000);

            uint8_t cols = GPIO_PORTE_DATA_R & 0x1E;

            if (cols != 0x1E)   // some column pulled low
            {
                uint8_t colIndex;

                if ((cols & (1U << 1)) == 0) colIndex = 0;   // PE1
                else if ((cols & (1U << 2)) == 0) colIndex = 1; // PE2
                else if ((cols & (1U << 3)) == 0) colIndex = 2; // PE3
                else colIndex = 3;  // PE4

                // Wait for key release (debounce)
                while ((GPIO_PORTE_DATA_R & 0x1E) != 0x1E);
                
                // Debounce delay after release
                delay_small(20000);

                return keymap[row][colIndex];
            }
        }
    }
}
