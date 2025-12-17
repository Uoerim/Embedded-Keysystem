#ifndef RGB_LED_H_
#define RGB_LED_H_

#include <stdint.h>

typedef enum
{
    RGB_OFF = 0,
    RGB_RED,
    RGB_GREEN,
    RGB_BLUE,
    RGB_YELLOW,   /* Red + Green */
    RGB_CYAN,     /* Green + Blue */
    RGB_MAGENTA,  /* Red + Blue */
    RGB_WHITE
} RGB_Color_t;

void RGB_LED_Init(void);
void RGB_LED_SetColor(RGB_Color_t color);

#endif
