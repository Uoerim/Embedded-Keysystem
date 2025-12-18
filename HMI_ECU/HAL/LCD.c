#include <stdint.h>
#include "../MCAL/I2C.h"
#include "../MCAL/delay.h"
#include "LCD.h"

/* PCF8574 I2C backpack */
#define LCD_ADDR_7BIT           (0x27u)

#define LCD_BACKLIGHT_MASK      (0x08u) /* P3 */
#define LCD_EN_MASK             (0x04u) /* P2 */
#define LCD_RW_MASK             (0x02u) /* P1 (kept 0) */
#define LCD_RS_MASK             (0x01u) /* P0 */

#define LCD_I2C_TIMEOUT_MS      (20u)

/* HD44780 commands */
#define LCD_CMD_CLEAR           (0x01u)
#define LCD_CMD_HOME            (0x02u)
#define LCD_CMD_ENTRYMODE       (0x06u)
#define LCD_CMD_DISPLAY_ON      (0x0Cu)
#define LCD_CMD_FUNCTION_4BIT   (0x28u)
#define LCD_CMD_SET_DDRAM       (0x80u)

static Std_ReturnType LCD_WriteExpander(uint8_t data)
{
    return I2C0_WriteByte(LCD_ADDR_7BIT, data, LCD_I2C_TIMEOUT_MS);
}

static void LCD_Strobe(uint8_t data)
{
    (void)LCD_WriteExpander((uint8_t)(data | LCD_EN_MASK));
    Delay_ms(2u);
    (void)LCD_WriteExpander((uint8_t)(data & (uint8_t)(~LCD_EN_MASK)));
    Delay_ms(2u);
}

static void LCD_WriteNibble(uint8_t nibble, boolean rs)
{
    uint8_t data = 0u;

    data |= (uint8_t)((nibble & 0x0Fu) << 4);
    data |= LCD_BACKLIGHT_MASK;

    if (rs != FALSE)
    {
        data |= LCD_RS_MASK;
    }

    /* RW kept low */
    (void)LCD_WriteExpander(data);
    LCD_Strobe(data);
}

static void LCD_SendByte(uint8_t value, boolean rs)
{
    LCD_WriteNibble((uint8_t)((value >> 4) & 0x0Fu), rs);
    LCD_WriteNibble((uint8_t)(value & 0x0Fu), rs);
}

static void LCD_SendCmd(uint8_t cmd)
{
    LCD_SendByte(cmd, FALSE);
    Delay_ms(2u);
}

void LCD_SendChar(char c)
{
    LCD_SendByte((uint8_t)c, TRUE);
    Delay_ms(2u);
}

void LCD_SendString(const char *str)
{
    if (str == (const char *)0)
    {
        return;
    }

    while (*str != '\0')
    {
        LCD_SendChar(*str);
        str++;
    }
}

void LCD_Clear(void)
{
    LCD_SendCmd(LCD_CMD_CLEAR);
    Delay_ms(5u);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t base = (row == 0u) ? 0x00u : 0x40u;
    uint8_t addr = (uint8_t)(base + col);

    LCD_SendCmd((uint8_t)(LCD_CMD_SET_DDRAM | addr));
}

void LCD_Init(void)
{
    I2C0_Init_100k_16MHz();
    Delay_ms(50u); /* power-up */

    /* Backlight ON */
    (void)LCD_WriteExpander(LCD_BACKLIGHT_MASK);
    Delay_ms(10u);

    /* 4-bit init sequence (HD44780) */
    LCD_WriteNibble(0x03u, FALSE);
    Delay_ms(5u);
    LCD_WriteNibble(0x03u, FALSE);
    Delay_ms(5u);
    LCD_WriteNibble(0x03u, FALSE);
    Delay_ms(5u);

    LCD_WriteNibble(0x02u, FALSE); /* 4-bit mode */
    Delay_ms(5u);

    LCD_SendCmd(LCD_CMD_FUNCTION_4BIT);
    LCD_SendCmd(LCD_CMD_DISPLAY_ON);
    LCD_SendCmd(LCD_CMD_ENTRYMODE);
    LCD_Clear();
}
