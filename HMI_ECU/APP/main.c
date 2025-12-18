#include <stdint.h>
#include "../Common/Std_Types.h"

#include "../MCAL/delay.h"
#include "../MCAL/UART.h"
#include "../MCAL/ADC.h"

#include "../HAL/LCD.h"
#include "../HAL/Keypad.h"
#include "../HAL/Buzzer.h"

#define PASSWORD_LENGTH        (5u)
#define MAX_ATTEMPTS           (3u)

#define UART_BAUDRATE          (9600u)

#define LOCKOUT_SEC            (20u)

#define TIMEOUT_MIN_SEC        (5u)
#define TIMEOUT_MAX_SEC        (30u)
#define ADC_MAX_COUNTS         (4095u)

#define MSG_MS_SHORT           (800u)
#define MSG_MS_MED             (1200u)
#define MSG_MS_LONG            (1500u)

static uint8 g_timeout_seconds = 10u;

/* ---------- helpers ---------- */
static void LCD_PrintNumber(uint16 num)
{
    char buf[6];
    uint8 idx = 0u;

    if (num == 0u) { LCD_SendChar('0'); return; }

    while ((num > 0u) && (idx < (uint8)sizeof(buf)))
    {
        uint16 digit = (uint16)(num % 10u);
        buf[idx] = (char)('0' + (char)digit);
        idx++;
        num = (uint16)(num / 10u);
    }

    while (idx > 0u)
    {
        idx--;
        LCD_SendChar(buf[idx]);
    }
}

static void Password_ReadScreen(const char *title, char *buffer5)
{
    uint8 index = 0u;

    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString(title);
    LCD_SetCursor(1u, 0u);

    for (;;)
    {
        char k = Keypad_GetKey();
        Buzzer_BeepShort();

        if ((k >= '0') && (k <= '9'))
        {
            if (index < PASSWORD_LENGTH)
            {
                buffer5[index] = k;
                index++;
                LCD_SendChar('*');
            }
        }
        else if (k == 'B')
        {
            index = 0u;
            LCD_SetCursor(1u, 0u);
            LCD_SendString("     ");
            LCD_SetCursor(1u, 0u);
        }
        else if (k == 'A')
        {
            if (index == PASSWORD_LENGTH) { return; }

            LCD_Clear();
            LCD_SetCursor(0u, 0u);
            LCD_SendString("Enter 5 digits");
            Delay_ms(MSG_MS_MED);

            LCD_Clear();
            LCD_SetCursor(0u, 0u);
            LCD_SendString(title);
            LCD_SetCursor(1u, 0u);
            index = 0u;
        }
        else { }
    }
}

static void Lockout_Countdown(uint8 seconds)
{
    uint8 s;

    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString("LOCKOUT");

    for (s = seconds; s > 0u; s--)
    {
        LCD_SetCursor(1u, 0u);
        LCD_SendString("Wait: ");
        LCD_PrintNumber(s);
        LCD_SendString("s   ");

        Buzzer_BeepShort();
        Delay_ms(1000u);
    }

    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString("Returning...");
    Delay_ms(MSG_MS_SHORT);
}

static uint8 Pot_ReadTimeoutSeconds(void)
{
    uint16 adc = 0u;
    uint32 scaled;
    uint8 timeout;

    if (ADC_ReadTimeout(200u, &adc) != E_OK) { adc = 0u; }

    scaled  = ((uint32)adc * (uint32)(TIMEOUT_MAX_SEC - TIMEOUT_MIN_SEC)) / (uint32)ADC_MAX_COUNTS;
    timeout = (uint8)(TIMEOUT_MIN_SEC + (uint8)scaled);

    if (timeout < TIMEOUT_MIN_SEC) { timeout = TIMEOUT_MIN_SEC; }
    if (timeout > TIMEOUT_MAX_SEC) { timeout = TIMEOUT_MAX_SEC; }

    return timeout;
}

/* ---------- Control link ---------- */
static boolean Control_IsInitialized(void)
{
    uint8 r;

    for (;;)
    {
        UART1_FlushRx();
        UART1_SendByte((uint8)'I');

        if (UART1_ReceiveByteTimeout(300u, &r) == E_OK)
        {
            if (r == (uint8)'0') { return FALSE; }
            if (r == (uint8)'1') { return TRUE; }
        }
        Delay_ms(50u);
    }
}

static void Control_LoadSavedTimeout(void)
{
    uint8 t = 10u;

    UART1_FlushRx();
    UART1_SendByte((uint8)'G');

    if (UART1_ReceiveByteTimeout(300u, &t) == E_OK)
    {
        if (t < TIMEOUT_MIN_SEC) { t = TIMEOUT_MIN_SEC; }
        if (t > TIMEOUT_MAX_SEC) { t = TIMEOUT_MAX_SEC; }
        g_timeout_seconds = t;
    }
}

/* Verify password (used for open/lock/reset/change) */
static boolean VerifyPassword_WithAttempts(const char *title)
{
    char entered[PASSWORD_LENGTH];
    uint8 attempts = 0u;

    while (attempts < MAX_ATTEMPTS)
    {
        uint8 i;
        uint8 reply;

        Password_ReadScreen(title, entered);

        UART1_FlushRx();
        UART1_SendByte((uint8)'V');
        for (i = 0u; i < PASSWORD_LENGTH; i++)
        {
            UART1_SendByte((uint8)entered[i]);
        }

        /* IMPORTANT: use timeout read to avoid hanging */
        if (UART1_ReceiveByteTimeout(400u, &reply) != E_OK)
        {
            LCD_Clear();
            LCD_SetCursor(0u,0u);
            LCD_SendString("No Control ECU");
            Delay_ms(MSG_MS_MED);
            return FALSE;
        }

        if (reply == (uint8)'Y') { return TRUE; }

        attempts++;

        LCD_Clear();
        LCD_SetCursor(0u, 0u);
        LCD_SendString("Wrong Password");
        Buzzer_BeepShort();
        Delay_ms(MSG_MS_MED);
    }

    Lockout_Countdown(LOCKOUT_SEC);
    return FALSE;
}

static void InitialPasswordSetup(void)
{
    char first[PASSWORD_LENGTH];
    char confirm[PASSWORD_LENGTH];

    for (;;)
    {
        uint8 i;
        boolean match = TRUE;
        uint8 reply;

        Password_ReadScreen("Enter Password", first);
        Delay_ms(250u);
        Password_ReadScreen("Re-enter Pass", confirm);

        for (i = 0u; i < PASSWORD_LENGTH; i++)
        {
            if (first[i] != confirm[i]) { match = FALSE; break; }
        }

        if (match == FALSE)
        {
            LCD_Clear();
            LCD_SetCursor(0u, 0u);
            LCD_SendString("Mismatch!");
            for (i = 0u; i < 3u; i++)
            {
                Buzzer_BeepShort();
                Delay_ms(250u);
            }
            Delay_ms(MSG_MS_SHORT);
            continue;
        }

        UART1_FlushRx();
        UART1_SendByte((uint8)'N');
        for (i = 0u; i < PASSWORD_LENGTH; i++)
        {
            UART1_SendByte((uint8)first[i]);
        }

        if (UART1_ReceiveByteTimeout(500u, &reply) != E_OK)
        {
            LCD_Clear();
            LCD_SetCursor(0u,0u);
            LCD_SendString("No Control ECU");
            Delay_ms(MSG_MS_MED);
            continue;
        }

        if (reply == (uint8)'K')
        {
            LCD_Clear();
            LCD_SetCursor(0u, 0u);
            LCD_SendString("Pass Saved");
            Buzzer_On();
            Delay_ms(170u);
            Buzzer_Off();
            Delay_ms(MSG_MS_MED);

            Control_LoadSavedTimeout();
            return;
        }

        LCD_Clear();
        LCD_SetCursor(0u, 0u);
        LCD_SendString("Save Error");
        Buzzer_BeepShort();
        Delay_ms(MSG_MS_MED);
    }
}

/* Set timeout: POT used here ONLY, saved in Control EEPROM using 'S' atomically */
static void SetTimeoutUsingPot(void)
{
    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString("Adjust Timeout");

    for (;;)
    {
        uint8 timeout = Pot_ReadTimeoutSeconds();
        char k;

        LCD_SetCursor(1u, 0u);
        LCD_SendString("Value: ");
        LCD_PrintNumber(timeout);
        LCD_SendString("s   ");

        if (Keypad_GetKeyTimeout(50u, &k) != E_OK) { continue; }
        Buzzer_BeepShort();

        if (k == 'A')
        {
            char pass[PASSWORD_LENGTH];
            uint8 r;
            uint8 i;

            Password_ReadScreen("Enter Password", pass);

            UART1_FlushRx();
            UART1_SendByte((uint8)'S');

            for (i = 0u; i < PASSWORD_LENGTH; i++)
            {
                UART1_SendByte((uint8)pass[i]);
            }

            UART1_SendByte((uint8)('0' + (timeout / 10u)));
            UART1_SendByte((uint8)('0' + (timeout % 10u)));

            if (UART1_ReceiveByteTimeout(600u, &r) != E_OK)
            {
                LCD_Clear();
                LCD_SetCursor(0u, 0u);
                LCD_SendString("Timeout Err");
                Buzzer_BeepShort();
                Delay_ms(MSG_MS_MED);
                return;
            }

            if (r == (uint8)'K')
            {
                g_timeout_seconds = timeout;
                LCD_Clear();
                LCD_SetCursor(0u, 0u);
                LCD_SendString("Timeout Saved");
                Buzzer_On();
                Delay_ms(150u);
                Buzzer_Off();
                Delay_ms(MSG_MS_MED);
                return;
            }
            else if (r == (uint8)'N')
            {
                LCD_Clear();
                LCD_SetCursor(0u, 0u);
                LCD_SendString("Wrong Password");
                Buzzer_BeepShort();
                Delay_ms(MSG_MS_MED);
                return;
            }
            else
            {
                LCD_Clear();
                LCD_SetCursor(0u, 0u);
                LCD_SendString("Timeout Err");
                Buzzer_BeepShort();
                Delay_ms(MSG_MS_MED);
                return;
            }
        }
        else if (k == 'B')
        {
            LCD_Clear();
            LCD_SetCursor(0u, 0u);
            LCD_SendString("Canceled");
            Delay_ms(MSG_MS_SHORT);
            return;
        }
        else { }
    }
}

static void OpenDoorFlow(void)
{
    uint8 s;

    if (VerifyPassword_WithAttempts("Enter Password") == FALSE) { return; }

    UART1_FlushRx();
    UART1_SendByte((uint8)'O');

    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString("Door Unlocking");
    Buzzer_BeepShort();
    Delay_ms(MSG_MS_MED);

    for (s = g_timeout_seconds; s > 0u; s--)
    {
        LCD_Clear();
        LCD_SetCursor(0u, 0u);
        LCD_SendString("Door Open");
        LCD_SetCursor(1u, 0u);
        LCD_SendString("Auto-lock in ");
        LCD_PrintNumber(s);
        LCD_SendString("s");

        Buzzer_BeepShort();
        Delay_ms(1000u);
    }

    UART1_FlushRx();
    UART1_SendByte((uint8)'L');

    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString("Relocking Door");
    Buzzer_BeepShort();
    Delay_ms(MSG_MS_LONG);
}

/* menu */
typedef enum
{
    MENU_OPEN = 0,
    MENU_TIMEOUT,
    MENU_COUNT
} MenuId;

static const char *MenuNames[MENU_COUNT] =
{
    "- Open Door",
    "- Set Timeout"
};

static void MainMenu(void)
{
    MenuId selected = MENU_OPEN;

    for (;;)
    {
        char k;

        LCD_Clear();
        LCD_SetCursor(0u, 0u);
        LCD_SendString("Main Menu");
        LCD_SetCursor(1u, 0u);
        LCD_SendString(MenuNames[selected]);

        k = Keypad_GetKey();
        Buzzer_BeepShort();

        if (k == 'C') { selected = (MenuId)((selected + 1u) % MENU_COUNT); }
        else if (k == 'D') { selected = (MenuId)((selected + MENU_COUNT - 1u) % MENU_COUNT); }
        else if (k == 'A')
        {
            if (selected == MENU_OPEN) { OpenDoorFlow(); }
            else { SetTimeoutUsingPot(); }
        }
        else { }
    }
}

int main(void)
{
    Delay_Init_16MHz();

    LCD_Init();
    Keypad_Init();
    Buzzer_Init();
    ADC_Init();
    UART1_Init(UART_BAUDRATE);

    if (Control_IsInitialized() == FALSE)
    {
        InitialPasswordSetup();
    }
    else
    {
        Control_LoadSavedTimeout();
    }

    MainMenu();
    return 0;
}
