#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "delay.h"

/* SysTick runs from system clock.
 * At 16 MHz: 1 ms = 16000 cycles.
 */
#define SYSCLK_HZ           (16000000u)
#define SYSTICK_1MS_RELOAD  ((SYSCLK_HZ / 1000u) - 1u)

static volatile uint32_t g_msTicks = 0u;

void SysTick_Handler(void)
{
    g_msTicks++;
}

void Delay_Init_16MHz(void)
{
    g_msTicks = 0u;

    /* Disable SysTick during setup */
    NVIC_ST_CTRL_R = 0u;

    /* Reload for 1ms tick */
    NVIC_ST_RELOAD_R = SYSTICK_1MS_RELOAD;

    /* Clear current value */
    NVIC_ST_CURRENT_R = 0u;

    /* Enable SysTick with:
     * - core clock (CLKSOURCE=1)
     * - interrupt (INTEN=1)
     * - enable (ENABLE=1)
     */
    NVIC_ST_CTRL_R = (1u << 2) | (1u << 1) | (1u << 0);
}

void Delay_ms(uint32_t ms)
{
    uint32_t start = g_msTicks;

    while ((g_msTicks - start) < ms)
    {
        /* wait */
    }
}
