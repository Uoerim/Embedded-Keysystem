#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "SysTick.h"

#define SYSTEM_CLOCK_HZ   16000000UL   // internal 16 MHz

void SysTick_Init(void)
{
    // Disable SysTick during setup
    NVIC_ST_CTRL_R = 0x00000000;

    // Max reload for now (we set the real value in Delay)
    NVIC_ST_RELOAD_R = 0x00FFFFFF;

    // Clear current value
    NVIC_ST_CURRENT_R = 0;

    // Enable SysTick with system clock, no interrupt
    NVIC_ST_CTRL_R = NVIC_ST_CTRL_ENABLE | NVIC_ST_CTRL_CLK_SRC;
}

void SysTick_DelayMs(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++)
    {
        // 1 ms at 16 MHz ? 16000 ticks
        NVIC_ST_RELOAD_R = (SYSTEM_CLOCK_HZ / 1000U) - 1U;
        NVIC_ST_CURRENT_R = 0;  // clear current value

        // Wait until COUNT flag is set
        while ((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0)
        {
        }
    }
}
