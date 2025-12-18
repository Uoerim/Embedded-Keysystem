#ifndef DELAY_H_
#define DELAY_H_

#include <stdint.h>

void Delay_Init_16MHz(void);
void Delay_ms(uint32_t ms);
uint32_t Delay_GetTicksMs(void);

#endif /* DELAY_H_ */
