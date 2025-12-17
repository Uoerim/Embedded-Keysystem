#ifndef RGB_H
#define RGB_H

#include <stdint.h>

/* RGB color definitions */
#define RGB_RED     0x02
#define RGB_BLUE    0x04
#define RGB_GREEN   0x08
#define RGB_WHITE   0x0E
#define RGB_OFF     0x00

void RGB_Init(void);
void RGB_SetColor(uint8_t color);

#endif
