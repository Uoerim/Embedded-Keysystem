#include <stdint.h>
#include "TM4C123GH6PM.h"

#include "../MCAL/UART.h"
#include "../MCAL/EEPROM.h"
#include "../HAL/Motor.h"

/* ================== CONFIG ================== */
#define PASSWORD_LENGTH  5u

/* External RGB LED on Port C (common cathode, ON = HIGH)
   PC5 = Red, PC6 = Green, PC7 = Blue */
#define RGB_RED_PIN    (1u << 5)   /* PC5 */
#define RGB_GREEN_PIN  (1u << 6)   /* PC6 */
#define RGB_BLUE_PIN   (1u << 7)   /* PC7 */

/* ================== STATE ================== */
static char    g_password[PASSWORD_LENGTH] = { '1','2','3','4','5' };
static uint8_t g_timeout_seconds           = 10u;
static uint8_t g_initialized               = 0u;

/* ================== HELPERS ================== */
static void delay(volatile uint32_t t) { while (t--) { } }

typedef enum
{
    RGB_OFF = 0,
    RGB_RED,
    RGB_GREEN,
    RGB_BLUE,
    RGB_YELLOW,   /* R+G */
    RGB_CYAN,     /* G+B */
    RGB_MAGENTA,  /* R+B */
    RGB_WHITE
} RGB_Color_t;

static void RGB_Init(void)
{
    SYSCTL_RCGCGPIO_R |= (1u << 2);   /* Port C */
    (void)SYSCTL_RCGCGPIO_R;

    GPIO_PORTC_DIR_R |= (RGB_RED_PIN | RGB_GREEN_PIN | RGB_BLUE_PIN);
    GPIO_PORTC_DEN_R |= (RGB_RED_PIN | RGB_GREEN_PIN | RGB_BLUE_PIN);

    GPIO_PORTC_DATA_R &= ~(RGB_RED_PIN | RGB_GREEN_PIN | RGB_BLUE_PIN);
}

static void RGB_Set(RGB_Color_t c)
{
    GPIO_PORTC_DATA_R &= ~(RGB_RED_PIN | RGB_GREEN_PIN | RGB_BLUE_PIN);

    switch (c)
    {
        case RGB_RED:     GPIO_PORTC_DATA_R |= RGB_RED_PIN; break;
        case RGB_GREEN:   GPIO_PORTC_DATA_R |= RGB_GREEN_PIN; break;
        case RGB_BLUE:    GPIO_PORTC_DATA_R |= RGB_BLUE_PIN; break;
        case RGB_YELLOW:  GPIO_PORTC_DATA_R |= (RGB_RED_PIN | RGB_GREEN_PIN); break;
        case RGB_CYAN:    GPIO_PORTC_DATA_R |= (RGB_GREEN_PIN | RGB_BLUE_PIN); break;
        case RGB_MAGENTA: GPIO_PORTC_DATA_R |= (RGB_RED_PIN | RGB_BLUE_PIN); break;
        case RGB_WHITE:   GPIO_PORTC_DATA_R |= (RGB_RED_PIN | RGB_GREEN_PIN | RGB_BLUE_PIN); break;
        case RGB_OFF:
        default:
            break;
    }
}

static uint8_t Password_Equals(const char *a, const char *b)
{
    for (uint8_t i = 0u; i < PASSWORD_LENGTH; i++)
    {
        if (a[i] != b[i]) { return 0u; }
    }
    return 1u;
}

/* ================== COMMAND HANDLERS ================== */

static void Handle_I(void)
{
    UART1_SendByte(g_initialized ? '1' : '0');
}

static void Handle_V(void)
{
    char entered[PASSWORD_LENGTH];

    for (uint8_t i = 0u; i < PASSWORD_LENGTH; i++)
    {
        entered[i] = (char)UART1_ReceiveByte();
    }

    if ((g_initialized != 0u) && Password_Equals(entered, g_password))
    {
        UART1_SendByte('Y');
        RGB_Set(RGB_GREEN);
        delay(120000);
        RGB_Set(RGB_BLUE);
    }
    else
    {
        UART1_SendByte('N');
        RGB_Set(RGB_RED);
        delay(120000);
        RGB_Set(RGB_BLUE);
    }
}

static void Handle_N(void)
{
    char newPass[PASSWORD_LENGTH];

    for (uint8_t i = 0u; i < PASSWORD_LENGTH; i++)
    {
        newPass[i] = (char)UART1_ReceiveByte();
    }

    for (uint8_t i = 0u; i < PASSWORD_LENGTH; i++)
    {
        g_password[i] = newPass[i];
    }

    g_initialized = 1u;

    EEPROM_Save(g_password, g_timeout_seconds, g_initialized);

    UART1_SendByte('K');

    RGB_Set(RGB_CYAN);
    delay(150000);
    RGB_Set(RGB_BLUE);
}

/* Robust timeout handler:
   Accepts:
   1) 'T' + '0'..'9' + '0'..'9'  (ASCII digits)
   OR
   2) 'T' + raw byte (5..30)
*/
static void Handle_T(void)
{
    uint8_t b1 = UART1_ReceiveByte();
    uint8_t val;

    if ((b1 >= (uint8_t)'0') && (b1 <= (uint8_t)'9'))
    {
        uint8_t b2 = UART1_ReceiveByte();

        if ((b2 >= (uint8_t)'0') && (b2 <= (uint8_t)'9'))
        {
            val = (uint8_t)((b1 - (uint8_t)'0') * 10u + (b2 - (uint8_t)'0'));
        }
        else
        {
            /* bad second digit: fallback */
            val = 10u;
        }
    }
    else
    {
        /* raw byte format */
        val = b1;
    }

    if (val < 5u)  { val = 5u; }
    if (val > 30u) { val = 30u; }

    g_timeout_seconds = val;

    EEPROM_Save(g_password, g_timeout_seconds, g_initialized);

    UART1_SendByte('K');

    RGB_Set(RGB_YELLOW);
    delay(150000);
    RGB_Set(RGB_BLUE);
}

static void Handle_R(void)
{
    g_initialized = 0u;
    g_timeout_seconds = 10u;

    Motor_Stop();
    EEPROM_Clear();

    UART1_SendByte('K');

    RGB_Set(RGB_MAGENTA);
    delay(250000);
    RGB_Set(RGB_OFF);
    delay(250000);
    RGB_Set(RGB_MAGENTA);
    delay(250000);
    RGB_Set(RGB_BLUE);
}

/* ================== MAIN ================== */

int main(void)
{
    RGB_Init();
    Motor_Init();
    UART1_Init(9600);
    EEPROM0_Init();

    /* Load from EEPROM if exists */
    {
        uint8_t init = 0u;
        uint8_t t = 10u;
        char pass[PASSWORD_LENGTH];

        if (EEPROM_Load(pass, &t, &init))
        {
            for (uint8_t i = 0u; i < PASSWORD_LENGTH; i++)
            {
                g_password[i] = pass[i];
            }
            g_timeout_seconds = t;
            g_initialized = init;
        }
        else
        {
            g_initialized = 0u;
            g_timeout_seconds = 10u;
        }
    }

    /* Boot indicator */
    RGB_Set(RGB_WHITE);
    delay(300000);
    RGB_Set(RGB_BLUE); /* idle */

    while (1)
    {
        uint8_t cmd = UART1_ReceiveByte();

        if      (cmd == 'I') { Handle_I(); }
        else if (cmd == 'V') { Handle_V(); }
        else if (cmd == 'N') { Handle_N(); }
        else if (cmd == 'T') { Handle_T(); }
        else if (cmd == 'R') { Handle_R(); }

        else if (cmd == 'O')
        {
            RGB_Set(RGB_CYAN);
            Motor_Open();
            RGB_Set(RGB_BLUE);
        }
        else if (cmd == 'L')
        {
            RGB_Set(RGB_YELLOW);
            Motor_Close();
            RGB_Set(RGB_BLUE);
        }
        else
        {
            UART1_SendByte('?');  /* helps debugging */
            RGB_Set(RGB_RED);
            delay(80000);
            RGB_Set(RGB_BLUE);
        }
    }
}
