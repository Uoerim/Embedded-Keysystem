/*========================================================
 * File: main.c
 * Brief: Control ECU main application (APP layer)
 * Target: TM4C123GH6PM
 * Clock: 16 MHz
 *========================================================*/
#include <stdint.h>
#include "../Common/Std_Types.h"

#include "../MCAL/UART.h"
#include "../MCAL/EEPROM.h"
#include "../MCAL/delay.h"

#include "../HAL/Motor.h"
#include "../HAL/RGB_LED.h"

/* ================== CONFIG ================== */
#define PASSWORD_LENGTH        (5u)

#define UART_BAUDRATE          (9600u)

#define TIMEOUT_DEFAULT_SEC    (10u)
#define TIMEOUT_MIN_SEC        (5u)
#define TIMEOUT_MAX_SEC        (30u)

#define BOOT_LED_MS            (300u)
#define FEEDBACK_MS            (120u)
#define SHORT_BLIP_MS          (80u)
#define BLINK_MS               (250u)

/* ================== STATE ================== */
static char    g_password[PASSWORD_LENGTH] = { '1','2','3','4','5' };
static uint8   g_timeout_seconds           = TIMEOUT_DEFAULT_SEC;
static uint8   g_initialized               = 0u;

/* ================== HELPERS ================== */
static uint8 Password_Equals(const char *a, const char *b)
{
    uint8 i;
    for (i = 0u; i < PASSWORD_LENGTH; i++)
    {
        if (a[i] != b[i]) { return 0u; }
    }
    return 1u;
}

static void Password_Copy(char *dst, const char *src)
{
    uint8 i;
    for (i = 0u; i < PASSWORD_LENGTH; i++)
    {
        dst[i] = src[i];
    }
}

static uint8 ClampTimeout(uint8 t)
{
    if (t < TIMEOUT_MIN_SEC) { t = TIMEOUT_MIN_SEC; }
    if (t > TIMEOUT_MAX_SEC) { t = TIMEOUT_MAX_SEC; }
    return t;
}

/* ================== COMMANDS ==================
   I : init flag (0/1)
   V : verify password -> Y/N
   N : set password (first run / change) -> K
   O : open motor
   L : close motor
   R : reset -> K
   G : get saved timeout -> sends one byte timeout (5..30)
   S : set timeout with password (atomic) -> K/N/E
       HMI sends: 'S' + 5 pass bytes + 2 ascii digits
*/
static void Handle_I(void)
{
    UART1_SendByte((g_initialized != 0u) ? (uint8)'1' : (uint8)'0');
}

static void Handle_V(void)
{
    char entered[PASSWORD_LENGTH];
    uint8 i;

    for (i = 0u; i < PASSWORD_LENGTH; i++)
    {
        entered[i] = (char)UART1_ReceiveByte();
    }

    if ((g_initialized != 0u) && (Password_Equals(entered, g_password) != 0u))
    {
        UART1_SendByte((uint8)'Y');
        RGB_LED_SetColor(RGB_GREEN);
    }
    else
    {
        UART1_SendByte((uint8)'N');
        RGB_LED_SetColor(RGB_RED);
    }
    Delay_ms(FEEDBACK_MS);
    RGB_LED_SetColor(RGB_BLUE);
}

static void Handle_N(void)
{
    char new_pass[PASSWORD_LENGTH];
    uint8 i;

    for (i = 0u; i < PASSWORD_LENGTH; i++)
    {
        new_pass[i] = (char)UART1_ReceiveByte();
    }

    Password_Copy(g_password, new_pass);
    g_initialized = 1u;

    EEPROM_Save(g_password, g_timeout_seconds, g_initialized);

    UART1_SendByte((uint8)'K');

    RGB_LED_SetColor(RGB_CYAN);
    Delay_ms(FEEDBACK_MS);
    RGB_LED_SetColor(RGB_BLUE);
}

static void Handle_G(void)
{
    /* return currently saved timeout (already clamped) */
    UART1_SendByte((uint8)g_timeout_seconds);
}

/* Atomic set timeout with password */
static void Handle_S(void)
{
    char entered[PASSWORD_LENGTH];
    uint8 i;
    uint8 b1;
    uint8 b2;
    uint8 timeout;

    for (i = 0u; i < PASSWORD_LENGTH; i++)
    {
        entered[i] = (char)UART1_ReceiveByte();
    }

    b1 = UART1_ReceiveByte();
    b2 = UART1_ReceiveByte();

    /* must be digits */
    if ((b1 < (uint8)'0') || (b1 > (uint8)'9') || (b2 < (uint8)'0') || (b2 > (uint8)'9'))
    {
        UART1_SendByte((uint8)'E');
        RGB_LED_SetColor(RGB_RED);
        Delay_ms(FEEDBACK_MS);
        RGB_LED_SetColor(RGB_BLUE);
        return;
    }

    timeout = (uint8)(((b1 - (uint8)'0') * 10u) + (b2 - (uint8)'0'));
    timeout = ClampTimeout(timeout);

    if ((g_initialized == 0u) || (Password_Equals(entered, g_password) == 0u))
    {
        UART1_SendByte((uint8)'N');
        RGB_LED_SetColor(RGB_RED);
        Delay_ms(FEEDBACK_MS);
        RGB_LED_SetColor(RGB_BLUE);
        return;
    }

    g_timeout_seconds = timeout;
    EEPROM_Save(g_password, g_timeout_seconds, g_initialized);

    UART1_SendByte((uint8)'K');
    RGB_LED_SetColor(RGB_YELLOW);
    Delay_ms(FEEDBACK_MS);
    RGB_LED_SetColor(RGB_BLUE);
}

static void Handle_R(void)
{
    g_initialized = 0u;
    g_timeout_seconds = TIMEOUT_DEFAULT_SEC;

    Motor_Stop();
    EEPROM_Clear();

    UART1_SendByte((uint8)'K');

    RGB_LED_SetColor(RGB_MAGENTA);
    Delay_ms(BLINK_MS);
    RGB_LED_SetColor(RGB_OFF);
    Delay_ms(BLINK_MS);
    RGB_LED_SetColor(RGB_MAGENTA);
    Delay_ms(BLINK_MS);
    RGB_LED_SetColor(RGB_BLUE);
}

static void Handle_O(void)
{
    RGB_LED_SetColor(RGB_CYAN);
    Motor_Open();
    RGB_LED_SetColor(RGB_BLUE);
}

static void Handle_L(void)
{
    RGB_LED_SetColor(RGB_YELLOW);
    Motor_Close();
    RGB_LED_SetColor(RGB_BLUE);
}

static void Handle_Unknown(void)
{
    UART1_SendByte((uint8)'?');
    RGB_LED_SetColor(RGB_RED);
    Delay_ms(SHORT_BLIP_MS);
    RGB_LED_SetColor(RGB_BLUE);
}

/* ================== MAIN ================== */
int main(void)
{
    uint8 init = 0u;
    uint8 t = TIMEOUT_DEFAULT_SEC;
    char pass[PASSWORD_LENGTH];

    Delay_Init_16MHz();
    RGB_LED_Init();
    Motor_Init();
    UART1_Init(UART_BAUDRATE);
    EEPROM0_Init();

    if (EEPROM_Load(pass, &t, &init) != 0u)
    {
        Password_Copy(g_password, pass);
        g_timeout_seconds = ClampTimeout(t);
        g_initialized = (init != 0u) ? 1u : 0u;
    }

    RGB_LED_SetColor(RGB_WHITE);
    Delay_ms(BOOT_LED_MS);
    RGB_LED_SetColor(RGB_BLUE);

    for (;;)
    {
        uint8 cmd = UART1_ReceiveByte();

        switch (cmd)
        {
            case (uint8)'I': Handle_I(); break;
            case (uint8)'V': Handle_V(); break;
            case (uint8)'N': Handle_N(); break;
            case (uint8)'G': Handle_G(); break;
            case (uint8)'S': Handle_S(); break;
            case (uint8)'R': Handle_R(); break;
            case (uint8)'O': Handle_O(); break;
            case (uint8)'L': Handle_L(); break;
            default:         Handle_Unknown(); break;
        }
    }
}
