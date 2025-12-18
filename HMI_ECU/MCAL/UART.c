#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "UART.h"
#include "delay.h"

/* ================== CONFIG ================== */
#define SYSCLK_HZ                   (16000000u)
#define UART_DIV_MUL                (16u)

/* Clocks */
#define SYSCTL_RCGCUART_UART1_MASK  (1u << 1)
#define SYSCTL_RCGCGPIO_GPIOB_MASK  (1u << 1)

/* PB0/PB1 */
#define GPIO_PB0_MASK               (1u << 0)
#define GPIO_PB1_MASK               (1u << 1)
#define GPIO_PB01_MASK              (GPIO_PB0_MASK | GPIO_PB1_MASK)

/* UART flags */
#define UART_FR_TXFF_MASK           (1u << 5)
#define UART_FR_RXFE_MASK           (1u << 4)

/* UART CTL bits */
#define UART_CTL_UARTEN_MASK        (1u << 0)
#define UART_CTL_TXE_MASK           (1u << 8)
#define UART_CTL_RXE_MASK           (1u << 9)

/* UART LCRH */
#define UART_LCRH_WLEN_8            (0x3u << 5)

static void UART1_SetBaudRate(uint32_t baudrate)
{
    uint32_t denom;
    uint32_t ibrd;
    uint32_t rem;
    uint32_t fbrd;

    if (baudrate == 0u)
    {
        baudrate = 9600u;
    }

    denom = (UART_DIV_MUL * baudrate);

    ibrd = (SYSCLK_HZ / denom);
    rem  = (SYSCLK_HZ % denom);

    /* FBRD = round(rem * 64 / denom) */
    fbrd = ((rem * 64u) + (denom / 2u)) / denom;
    if (fbrd > 63u)
    {
        fbrd = 63u;
    }

    UART1_IBRD_R = (uint16_t)ibrd;
    UART1_FBRD_R = (uint8_t)fbrd;
}

void UART1_Init(uint32_t baudrate)
{
    SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_UART1_MASK;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_GPIOB_MASK;
    (void)SYSCTL_RCGCGPIO_R;

    /* PB0/PB1 alt function UART1 */
    GPIO_PORTB_AFSEL_R |= GPIO_PB01_MASK;
    GPIO_PORTB_DEN_R   |= GPIO_PB01_MASK;
    GPIO_PORTB_AMSEL_R &= ~GPIO_PB01_MASK;

    GPIO_PORTB_PCTL_R &= ~((0xFu << 0) | (0xFu << 4));
    GPIO_PORTB_PCTL_R |=  ((0x1u << 0) | (0x1u << 4));

    /* Disable UART during config */
    UART1_CTL_R &= ~UART_CTL_UARTEN_MASK;

    UART1_SetBaudRate(baudrate);

    UART1_LCRH_R = UART_LCRH_WLEN_8;

    UART1_CTL_R |= (UART_CTL_TXE_MASK | UART_CTL_RXE_MASK);
    UART1_CTL_R |= UART_CTL_UARTEN_MASK;
}

void UART1_SendByte(uint8_t data)
{
    while ((UART1_FR_R & UART_FR_TXFF_MASK) != 0u)
    {
        /* wait */
    }
    UART1_DR_R = (uint32_t)data;
}

uint8_t UART1_ReceiveByte(void)
{
    while ((UART1_FR_R & UART_FR_RXFE_MASK) != 0u)
    {
        /* wait */
    }
    return (uint8_t)(UART1_DR_R & 0xFFu);
}

Std_ReturnType UART1_ReceiveByteTimeout(uint32_t timeout_ms, uint8_t *out)
{
    uint32_t start;

    if (out == (uint8_t *)0)
    {
        return E_NOT_OK;
    }

    start = Delay_GetTicksMs();

    while ((UART1_FR_R & UART_FR_RXFE_MASK) != 0u)
    {
        if ((Delay_GetTicksMs() - start) >= timeout_ms)
        {
            return E_NOT_OK;
        }
    }

    *out = (uint8_t)(UART1_DR_R & 0xFFu);
    return E_OK;
}

void UART1_SendString(const char *str)
{
    if (str == (const char *)0)
    {
        return;
    }

    while (*str != '\0')
    {
        UART1_SendByte((uint8_t)(*str));
        str++;
    }
}
void UART1_FlushRx(void)
{
    /* Drain RX FIFO: while RXFE == 0 read DR */
    while ((UART1_FR_R & (1u << 4)) == 0u)
    {
        (void)UART1_DR_R;
    }
}
