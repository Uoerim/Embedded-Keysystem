#include <stdint.h>
#include "TM4C123GH6PM.h"

#include "../MCAL/UART.h"
#include "../MCAL/EEPROM.h"
#include "../HAL/Motor.h"

#define PASSWORD_LENGTH 5

static char    g_password[PASSWORD_LENGTH] = { '1','2','3','4','5' };
static uint8_t g_timeout_seconds           = 10u;
static uint8_t g_initialized               = 0u;

static void delay(volatile uint32_t t){ while (t--){} }

/* PF1 red LED status */
static void LED_Init(void)
{
    SYSCTL_RCGCGPIO_R |= (1U << 5);
    (void)SYSCTL_RCGCGPIO_R;

    GPIO_PORTF_LOCK_R = 0x4C4F434B;
    GPIO_PORTF_CR_R  |= (1U << 1);

    GPIO_PORTF_DIR_R |= (1U << 1);
    GPIO_PORTF_DEN_R |= (1U << 1);

    GPIO_PORTF_DATA_R &= ~(1U << 1);
}
static void LED_On(void){ GPIO_PORTF_DATA_R |= (1U<<1); }
static void LED_Off(void){ GPIO_PORTF_DATA_R &= ~(1U<<1); }

static uint8_t Password_Equals(const char *a, const char *b)
{
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++)
        if (a[i] != b[i]) return 0u;
    return 1u;
}

/* -------- Commands -------- */

static void Handle_I(void)
{
    UART1_SendByte(g_initialized ? '1' : '0');
}

static void Handle_V(void)
{
    char entered[PASSWORD_LENGTH];
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++)
        entered[i] = (char)UART1_ReceiveByte();

    if ((g_initialized != 0u) && Password_Equals(entered, g_password))
        UART1_SendByte('Y');
    else
        UART1_SendByte('N');
}

static void Handle_N(void)
{
    char newPass[PASSWORD_LENGTH];
    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++)
        newPass[i] = (char)UART1_ReceiveByte();

    for (uint8_t i = 0; i < PASSWORD_LENGTH; i++)
        g_password[i] = newPass[i];

    g_initialized = 1u;

    /* ? persist */
    EEPROM_Save(g_password, g_timeout_seconds, g_initialized);

    UART1_SendByte('K');
}

static void Handle_T(void)
{
    char d1 = (char)UART1_ReceiveByte();
    char d2 = (char)UART1_ReceiveByte();

    uint8_t val = (uint8_t)((d1 - '0') * 10u + (d2 - '0'));
    if (val < 5u)  val = 5u;
    if (val > 30u) val = 30u;

    g_timeout_seconds = val;

    /* ? persist */
    EEPROM_Save(g_password, g_timeout_seconds, g_initialized);

    UART1_SendByte('K');
}

static void Handle_R(void)
{
    /* Go back to “first run” */
    g_initialized = 0u;
    g_timeout_seconds = 10u;

    Motor_Stop();
    LED_Off();

    /* ? clear persistent */
    EEPROM_Clear();

    UART1_SendByte('K');
}

int main(void)
{
    LED_Init();
    Motor_Init();
    UART1_Init(9600);
    EEPROM0_Init();

    /* ? Load from EEPROM if exists */
    {
        uint8_t init = 0, t = 10;
        char pass[PASSWORD_LENGTH];

        if (EEPROM_Load(pass, &t, &init))
        {
            for (uint8_t i = 0; i < PASSWORD_LENGTH; i++)
                g_password[i] = pass[i];

            g_timeout_seconds = t;
            g_initialized = init;
        }
        else
        {
            /* not valid yet => first run */
            g_initialized = 0u;
            g_timeout_seconds = 10u;
        }
    }

    /* startup blink */
    for (int i = 0; i < 3; i++)
    {
        LED_On(); delay(300000);
        LED_Off(); delay(300000);
    }

    while (1)
    {
        uint8_t cmd = UART1_ReceiveByte();

        if      (cmd == 'I') Handle_I();
        else if (cmd == 'V') Handle_V();
        else if (cmd == 'N') Handle_N();
        else if (cmd == 'T') Handle_T();
        else if (cmd == 'R') Handle_R();

        else if (cmd == 'O')
        {
            LED_On();
            Motor_Open();
            LED_Off();
        }
        else if (cmd == 'L')
        {
            LED_On();
            Motor_Close();
            LED_Off();
        }
        else
        {
            /* unknown */
            LED_On(); delay(150000);
            LED_Off(); delay(150000);
        }
    }
}
