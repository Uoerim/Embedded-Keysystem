#ifndef ADC_H
#define ADC_H

#include "../Common/Std_Types.h"

void ADC_Init(void);
Std_ReturnType ADC_ReadTimeout(uint32 timeout_ms, uint16 *out);

#endif
