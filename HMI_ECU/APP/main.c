#include <stdint.h>
#include "TM4C123GH6PM.h"

#include "../HAL/LCD.h"
#include "../HAL/Keypad.h"
#include "../HAL/Buzzer.h"

#include "../MCAL/ADC.h"
#include "../MCAL/SysTick.h"
#include "../MCAL/UART.h"

#define PASSWORD_LENGTH    5
#define MAX_ATTEMPTS       3

static uint8_t g_timeout_seconds = 10u;

/* ================== HELPERS ================== */

static void LCD_PrintNumber(uint16_t num)
{
    char buf[10];
    int i = 0;

    if (num == 0)
    {
        LCD_SendChar('0');
        return;
    }

    while (num > 0)
    {
        buf[i++] = (char)('0' + (num % 10u));
        num      = (uint16_t)(num / 10u);
    }

    while (i > 0)
    {
        LCD_SendChar(buf[--i]);
    }
}

/* Non-blocking scan for timeout screen */
static char Keypad_ScanOnce(void)
{
    static const char keymap[4][4] =
    {
        {'1','2','3','A'},
        {'4','5','6','B'},
        {'7','8','9','C'},
        {'*','0','#','D'}
    };

    for (uint8_t row = 0u; row < 4u; row++)
    {
        GPIO_PORTD_DATA_R |= 0x0F;
        GPIO_PORTD_DATA_R &= ~(1u << row);

        for (volatile int d = 0; d < 300; d++) { }

        uint8_t cols = (uint8_t)(GPIO_PORTE_DATA_R & 0x1E);

        if (cols != 0x1E)
        {
            uint8_t colIndex;

            if ((cols & (1u << 1)) == 0u)      colIndex = 0u;
            else if ((cols & (1u << 2)) == 0u) colIndex = 1u;
            else if ((cols & (1u << 3)) == 0u) colIndex = 2u;
            else                               colIndex = 3u;

            while ((GPIO_PORTE_DATA_R & 0x1E) != 0x1E) { }

            return keymap[row][colIndex];
        }
    }

    return 0;
}

static void HMI_GetPasswordScreen(const char *title, char *buffer)
{
    uint8_t index = 0u;

    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString(title);
    LCD_SetCursor(1u, 0u);

    while (1)
    {
        char k = Keypad_GetKey();
        Buzzer_BeepShort();

        if ((k >= '0') && (k <= '9'))
        {
            if (index < PASSWORD_LENGTH)
            {
                buffer[index++] = k;
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
            if (index == PASSWORD_LENGTH)
            {
                return;
            }
            else
            {
                LCD_Clear();
                LCD_SetCursor(0u, 0u);
                LCD_SendString("Enter 5 digits");
                SysTick_DelayMs(1300);

                LCD_Clear();
                LCD_SetCursor(0u, 0u);
                LCD_SendString(title);
                LCD_SetCursor(1u, 0u);
                index = 0u;
            }
        }
    }
}

static void HMI_LockoutCountdown(uint8_t seconds)
{
    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString("LOCKOUT");

    for (uint8_t s = seconds; s > 0u; s--)
    {
        LCD_SetCursor(1u, 0u);
        LCD_SendString("Wait: ");
        LCD_PrintNumber(s);
        LCD_SendString("s ");

        Buzzer_BeepShort();
        SysTick_DelayMs(1000);
    }

    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString("Returning...");
    SysTick_DelayMs(800);
}

/* ================== CONTROL HANDSHAKE ================== */

static uint8_t HMI_IsInitialized(void)
{
    uint8_t r;

    /* give UART a moment after power-up */
    SysTick_DelayMs(200);

    while (1)
    {
        UART1_SendByte('I');
        r = UART1_ReceiveByte();

        if (r == '0') return 0u;
        if (r == '1') return 1u;

        /* if garbage, wait and retry */
        SysTick_DelayMs(50);
    }
}


/* ================== VERIFY PASSWORD VIA CONTROL ================== */

static uint8_t HMI_VerifyPasswordWithAttempts(const char *title)
{
    char entered[PASSWORD_LENGTH];
    uint8_t attempts = 0u;

    while (attempts < MAX_ATTEMPTS)
    {
        HMI_GetPasswordScreen(title, entered);

        UART1_SendByte('V');
        for (uint8_t i = 0; i < PASSWORD_LENGTH; i++)
        {
            UART1_SendByte((uint8_t)entered[i]);
        }

        uint8_t reply = UART1_ReceiveByte(); /* Y/N */

        if (reply == 'Y')
        {
            return 1u;
        }

        attempts++;

        LCD_Clear();
        LCD_SetCursor(0u, 0u);
        LCD_SendString("Wrong Password");
        Buzzer_BeepShort();
        SysTick_DelayMs(1500);
    }

    HMI_LockoutCountdown(20u);
    return 0u;
}

/* ================== INITIAL PASSWORD SETUP ================== */

static void HMI_InitialPasswordSetup(void)
{
    char first[PASSWORD_LENGTH];
    char confirm[PASSWORD_LENGTH];

    while (1)
    {
        HMI_GetPasswordScreen("Enter Password", first);
        SysTick_DelayMs(400);

        HMI_GetPasswordScreen("Re-enter Pass", confirm);

        uint8_t match = 1u;
        for (uint8_t i = 0; i < PASSWORD_LENGTH; i++)
        {
            if (first[i] != confirm[i]) { match = 0u; break; }
        }

        if (!match)
        {
            LCD_Clear();
            LCD_SetCursor(0u, 0u);
            LCD_SendString("Mismatch!");
            for (int i = 0; i < 3; i++)
            {
                Buzzer_BeepShort();
                SysTick_DelayMs(300);
            }
            SysTick_DelayMs(1200);
            continue;
        }

        UART1_SendByte('N');
        for (uint8_t i = 0; i < PASSWORD_LENGTH; i++)
        {
            UART1_SendByte((uint8_t)first[i]);
        }

        uint8_t reply = UART1_ReceiveByte(); /* K */

        if (reply == 'K')
        {
            LCD_Clear();
            LCD_SetCursor(0u, 0u);
            LCD_SendString("Pass Saved");
            Buzzer_On();
            SysTick_DelayMs(180);
            Buzzer_Off();
            SysTick_DelayMs(1300);
            return;
        }

        LCD_Clear();
        LCD_SetCursor(0u, 0u);
        LCD_SendString("Save Error");
        Buzzer_BeepShort();
        SysTick_DelayMs(1300);
    }
}

/* ================== TIMEOUT SET ================== */

static void HMI_SetTimeoutUsingPot(void)
{
    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString("Adjust Timeout");

    while (1)
    {
        uint16_t adc = ADC_Read();
        uint8_t timeout = (uint8_t)(5u + (adc * 25u) / 4095u); /* 5..30 */

        LCD_SetCursor(1u, 0u);
        LCD_SendString("Value: ");
        LCD_PrintNumber(timeout);
        LCD_SendString("s   ");

        SysTick_DelayMs(100);

        char k = Keypad_ScanOnce();
        if (k == 0) { continue; }

        Buzzer_BeepShort();

        if (k == 'A')
        {
            if (HMI_VerifyPasswordWithAttempts("Enter Password"))
            {
                g_timeout_seconds = timeout;

                UART1_SendByte('T');
                UART1_SendByte((uint8_t)('0' + (timeout / 10u)));
                UART1_SendByte((uint8_t)('0' + (timeout % 10u)));

                uint8_t reply = UART1_ReceiveByte(); /* K */

                LCD_Clear();
                LCD_SetCursor(0u, 0u);
                LCD_SendString((reply == 'K') ? "Timeout Saved" : "Timeout Err");

                Buzzer_On();
                SysTick_DelayMs(150);
                Buzzer_Off();
                SysTick_DelayMs(1400);
            }
            return;
        }
        else if (k == 'B')
        {
            LCD_Clear();
            LCD_SetCursor(0u, 0u);
            LCD_SendString("Canceled");
            SysTick_DelayMs(900);
            return;
        }
    }
}

/* ================== OPEN DOOR ================== */

static void HMI_OpenDoorFlow(void)
{
    if (!HMI_VerifyPasswordWithAttempts("Enter Password"))
    {
        return;
    }

    UART1_SendByte('O');

    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString("Door Unlocking");
    Buzzer_BeepShort();
    SysTick_DelayMs(1500);

    for (uint8_t s = g_timeout_seconds; s > 0u; s--)
    {
        LCD_Clear();
        LCD_SetCursor(0u, 0u);
        LCD_SendString("Door Open");
        LCD_SetCursor(1u, 0u);
        LCD_SendString("Auto-lock in ");
        LCD_PrintNumber(s);
        LCD_SendString("s");

        Buzzer_BeepShort();
        SysTick_DelayMs(1000);
    }

    UART1_SendByte('L');

    LCD_Clear();
    LCD_SetCursor(0u, 0u);
    LCD_SendString("Relocking Door");
    Buzzer_BeepShort();
    SysTick_DelayMs(2000);
}

/* ================== CHANGE PASSWORD ================== */

static void HMI_ChangePassword(void)
{
    char first[PASSWORD_LENGTH];
    char confirm[PASSWORD_LENGTH];

    if (!HMI_VerifyPasswordWithAttempts("Enter Old Pass"))
    {
        return;
    }

    while (1)
    {
        HMI_GetPasswordScreen("Enter New Pass", first);
        SysTick_DelayMs(400);

        HMI_GetPasswordScreen("Re-enter Pass", confirm);

        uint8_t match = 1u;
        for (uint8_t i = 0; i < PASSWORD_LENGTH; i++)
        {
            if (first[i] != confirm[i]) { match = 0u; break; }
        }

        if (!match)
        {
            LCD_Clear();
            LCD_SetCursor(0u, 0u);
            LCD_SendString("Mismatch!");
            Buzzer_BeepShort();
            SysTick_DelayMs(1300);
            continue;
        }

        UART1_SendByte('N');
        for (uint8_t i = 0; i < PASSWORD_LENGTH; i++)
        {
            UART1_SendByte((uint8_t)first[i]);
        }

        uint8_t reply = UART1_ReceiveByte(); /* K */

        LCD_Clear();
        LCD_SetCursor(0u, 0u);
        LCD_SendString((reply == 'K') ? "Pass Changed" : "Change Error");
        Buzzer_BeepShort();
        SysTick_DelayMs(1200);

        return;
    }
}

/* ================== RESET SYSTEM ================== */

static void HMI_ResetSystem(void)
{
    if (!HMI_VerifyPasswordWithAttempts("Enter Password"))
    {
        return;
    }

    UART1_SendByte('R');
    uint8_t reply = UART1_ReceiveByte(); /* K */

    LCD_Clear();
    if (reply == 'K')
    {
        g_timeout_seconds = 10u;

        LCD_SetCursor(0u, 0u);
        LCD_SendString("System Reset");
        Buzzer_On();
        SysTick_DelayMs(180);
        Buzzer_Off();
        SysTick_DelayMs(1200);

        /* After reset, force new password setup */
        HMI_InitialPasswordSetup();
    }
    else
    {
        LCD_SetCursor(0u, 0u);
        LCD_SendString("Reset Error");
        Buzzer_BeepShort();
        SysTick_DelayMs(1300);
    }
}

/* ================== MENU ================== */

typedef enum
{
    MENU_OPEN = 0,
    MENU_CHANGE,
    MENU_TIMEOUT,
    MENU_RESET,
    MENU_COUNT
} MenuId;

static const char *MenuNames[MENU_COUNT] =
{
    "+ Open Door",
    "- Change Pass",
    "* Set Timeout",
    "# Reset System"
};

static void HMI_MainMenu(void)
{
    MenuId selected = MENU_OPEN;

    while (1)
    {
        LCD_Clear();
        LCD_SetCursor(0u, 0u);
        LCD_SendString("Main Menu");
        LCD_SetCursor(1u, 0u);
        LCD_SendString(MenuNames[selected]);

        char k = Keypad_GetKey();
        Buzzer_BeepShort();

        if (k == 'C') { selected = (MenuId)((selected + 1u) % MENU_COUNT); }
        else if (k == 'D') { selected = (MenuId)((selected + MENU_COUNT - 1u) % MENU_COUNT); }
        else if (k == 'A')
        {
            if (selected == MENU_OPEN)      { HMI_OpenDoorFlow(); }
            else if (selected == MENU_CHANGE){ HMI_ChangePassword(); }
            else if (selected == MENU_TIMEOUT){ HMI_SetTimeoutUsingPot(); }
            else if (selected == MENU_RESET) { HMI_ResetSystem(); }
        }
    }
}

/* ================== MAIN ================== */

int main(void)
{
    LCD_Init();
    Keypad_Init();
    Buzzer_Init();
    ADC_Init();
    SysTick_Init();
    UART1_Init(9600);

    /* Force initial setup if Control ECU isn't initialized */
    if (HMI_IsInitialized() == 0u)
    {
        HMI_InitialPasswordSetup();
    }

    HMI_MainMenu();
    while (1) { }
}
