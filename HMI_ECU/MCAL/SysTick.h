#ifndef SYSTICK_H_
#define SYSTICK_H_

#include <stdint.h>

void SysTick_Init(void);
void SysTick_DelayMs(uint32_t ms);

#endif
