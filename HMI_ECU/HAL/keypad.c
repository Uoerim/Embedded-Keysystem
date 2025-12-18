#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "../MCAL/delay.h"
#include "Keypad.h"

/* Rows: PD0..PD3 output
 * Cols: PE1..PE4 input with pull-ups
 */
#define SYSCTL_RCGCGPIO_PORTD_MASK     (1u << 3)
#define SYSCTL_RCGCGPIO_PORTE_MASK     (1u << 4)

#define ROWS_MASK                      (0x0Fu) /* PD0..PD3 */
#define COLS_MASK                      (0x1Eu) /* PE1..PE4 */

#define DEBOUNCE_MS                    (20u)
#define SCAN_SETTLE_MS                 (1u)

static const char s_keymap[4][4] =
{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

static void Keypad_SetAllRowsHigh(void)
{
    GPIO_PORTD_DATA_R |= ROWS_MASK;
}

static void Keypad_DriveRowLow(uint8_t row)
{
    Keypad_SetAllRowsHigh();
    GPIO_PORTD_DATA_R &= (uint8_t)(~(1u << row));
}

static Std_ReturnType Keypad_ScanOnce(char *out)
{
    uint8_t row;

    if (out == (char *)0)
    {
        return E_NOT_OK;
    }

    for (row = 0u; row < 4u; row++)
    {
        uint8_t cols;
        uint8_t col_index;

        Keypad_DriveRowLow(row);
        Delay_ms(SCAN_SETTLE_MS);

        cols = (uint8_t)(GPIO_PORTE_DATA_R & COLS_MASK);

        if (cols != COLS_MASK)
        {
            if ((cols & (1u << 1)) == 0u)      { col_index = 0u; }
            else if ((cols & (1u << 2)) == 0u) { col_index = 1u; }
            else if ((cols & (1u << 3)) == 0u) { col_index = 2u; }
            else                                { col_index = 3u; }

            /* Wait release */
            while (((uint8_t)(GPIO_PORTE_DATA_R & COLS_MASK)) != COLS_MASK)
            {
                /* busy wait: acceptable for keypad (documented deviation) */
            }

            Delay_ms(DEBOUNCE_MS);

            *out = s_keymap[row][col_index];
            return E_OK;
        }
    }

    return E_NOT_OK;
}

void Keypad_Init(void)
{
    SYSCTL_RCGCGPIO_R |= (SYSCTL_RCGCGPIO_PORTD_MASK | SYSCTL_RCGCGPIO_PORTE_MASK);
    (void)SYSCTL_RCGCGPIO_R;

    /* PD0..PD3 output */
    GPIO_PORTD_DIR_R |= ROWS_MASK;
    GPIO_PORTD_DEN_R |= ROWS_MASK;
    GPIO_PORTD_AFSEL_R &= ~ROWS_MASK;
    GPIO_PORTD_AMSEL_R &= ~ROWS_MASK;

    /* PE1..PE4 input + pull-up */
    GPIO_PORTE_DIR_R &= (uint32_t)(~COLS_MASK);
    GPIO_PORTE_DEN_R |= COLS_MASK;
    GPIO_PORTE_PUR_R |= COLS_MASK;
    GPIO_PORTE_AFSEL_R &= (uint32_t)(~COLS_MASK);
    GPIO_PORTE_AMSEL_R &= (uint32_t)(~COLS_MASK);

    Keypad_SetAllRowsHigh();
}

char Keypad_GetKey(void)
{
    char k = '\0';

    while (Keypad_ScanOnce(&k) != E_OK)
    {
        /* wait */
    }

    return k;
}

Std_ReturnType Keypad_GetKeyTimeout(uint32_t timeout_ms, char *out)
{
    uint32_t start;
    char k = '\0';

    if (out == (char *)0)
    {
        return E_NOT_OK;
    }

    start = Delay_GetTicksMs();

    while ((Delay_GetTicksMs() - start) < timeout_ms)
    {
        if (Keypad_ScanOnce(&k) == E_OK)
        {
            *out = k;
            return E_OK;
        }
    }

    return E_NOT_OK;
}
