#include <stdint.h>
#include "../MCAL/I2C.h"
#include "LCD.h"

#define LCD_ADDR        0x27    // from your scan
#define LCD_BACKLIGHT   0x08    // P3
#define LCD_ENABLE      0x04    // P2
#define LCD_RW          0x02    // P1 (we keep RW=0)
#define LCD_RS          0x01    // P0

static void lcd_delay(volatile uint32_t t)
{
    while (t--) { }
}

static void lcd_write_nibble(uint8_t nibble, uint8_t rs)
{
    uint8_t data = 0;

    // put nibble on D4–D7 (P4–P7)
    data |= (nibble & 0x0F) << 4;

    // set RS
    if (rs)
        data |= LCD_RS;

    // backlight always ON
    data |= LCD_BACKLIGHT;

    // EN high
    I2C0_WriteByte(LCD_ADDR, data | LCD_ENABLE);
    lcd_delay(2000);

    // EN low
    I2C0_WriteByte(LCD_ADDR, data & ~LCD_ENABLE);
    lcd_delay(2000);
}

static void lcd_send_byte(uint8_t value, uint8_t rs)
{
    lcd_write_nibble((value >> 4) & 0x0F, rs); // high nibble
    lcd_write_nibble(value & 0x0F, rs);        // low nibble
}

static void lcd_send_cmd(uint8_t cmd)
{
    lcd_send_byte(cmd, 0);
}

void LCD_SendChar(char c)
{
    lcd_send_byte((uint8_t)c, 1);
}

void LCD_SendString(const char *str)
{
    while (*str)
    {
        LCD_SendChar(*str++);
    }
}

void LCD_Clear(void)
{
    lcd_send_cmd(0x01);  // clear display
    lcd_delay(8000);
}

void LCD_SetCursor(uint8_t row, uint8_t col)
{
    uint8_t addr = (row == 0) ? 0x00 : 0x40;
    addr += col;
    lcd_send_cmd(0x80 | addr);
}

void LCD_Init(void)
{
    I2C0_Init();
    lcd_delay(40000);

    // init sequence for 4-bit mode
    lcd_write_nibble(0x03, 0);
    lcd_delay(40000);
    lcd_write_nibble(0x03, 0);
    lcd_delay(40000);
    lcd_write_nibble(0x03, 0);
    lcd_delay(40000);
    lcd_write_nibble(0x02, 0);   // 4-bit
    lcd_delay(40000);

    // function set: 4-bit, 2 lines, 5x8 dots
    lcd_send_cmd(0x28);

    // display on, cursor off, blink off
    lcd_send_cmd(0x0C);

    // entry mode: increment, no shift
    lcd_send_cmd(0x06);

    LCD_Clear();
}
