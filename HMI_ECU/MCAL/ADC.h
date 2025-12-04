#ifndef ADC_H_
#define ADC_H_

#include <stdint.h>

void ADC_Init(void);        // configure PB4 as ADC input (AIN10)
uint16_t ADC_Read(void);    // returns 0–4095

#endif
