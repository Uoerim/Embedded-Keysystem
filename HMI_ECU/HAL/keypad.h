#ifndef KEYPAD_H_
#define KEYPAD_H_

#include <stdint.h>
#include "../Common/Std_Types.h"

/* Blocking read */
char Keypad_GetKey(void);

/* Timeout read (recommended for MISRA friendliness) */
Std_ReturnType Keypad_GetKeyTimeout(uint32_t timeout_ms, char *out);

void Keypad_Init(void);

#endif /* KEYPAD_H_ */
