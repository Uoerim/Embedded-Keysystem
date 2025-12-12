#include "TM4C123GH6PM.h"
#include "UART.h"

/* UART1 on PB0 (RX) / PB1 (TX), 8N1 */

void UART1_Init(uint32_t baudrate)
{
    /* Enable clocks for UART1 and GPIOB */
    SYSCTL_RCGCUART_R |= (1U << 1);   // UART1
    SYSCTL_RCGCGPIO_R |= (1U << 1);   // GPIOB
    (void)SYSCTL_RCGCGPIO_R;          // small delay

    /* Configure PB0, PB1 for alternate function UART1 */
    GPIO_PORTB_AFSEL_R |= (1U << 0) | (1U << 1);
    GPIO_PORTB_DEN_R   |= (1U << 0) | (1U << 1);
    GPIO_PORTB_PCTL_R  &= ~((0xF << 0) | (0xF << 4));
    GPIO_PORTB_PCTL_R  |=  ((0x1 << 0) | (0x1 << 4));   // U1Rx, U1Tx
    GPIO_PORTB_AMSEL_R &= ~((1U << 0) | (1U << 1));     // digital only

    /* Disable UART1 while configuring */
    UART1_CTL_R &= ~(1U << 0);   // UARTEN = 0

    /* Baud rate calculation for 16 MHz system clock */
    /*
        BRD = SysClk / (16 * baudrate)
        For 9600: BRD = 16,000,000 / (16 * 9600) ˜ 104.1667
        IBRD = 104
        FBRD = round(0.1667 * 64) = 11
    */
    uint32_t brd = 16000000UL / (16UL * baudrate);
    uint32_t frac = ( (16000000UL * 4UL) / baudrate + 1UL ) / 2UL - (brd * 64UL);

    UART1_IBRD_R = (uint16_t)brd;
    UART1_FBRD_R = (uint8_t)frac;

    /* 8-bit, no parity, one stop bit, FIFOs disabled */
    UART1_LCRH_R = (0x3 << 5);   // WLEN=3 -> 8 bits

    /* Enable RX, TX and UART */
    UART1_CTL_R |= (1U << 8) | (1U << 9); // TXE, RXE
    UART1_CTL_R |= (1U << 0);             // UARTEN
}

void UART1_SendByte(uint8_t data)
{
    /* Wait until TXFF = 0 (FIFO not full / holding register empty) */
    while (UART1_FR_R & (1U << 5)) { }
    UART1_DR_R = data;
}

uint8_t UART1_ReceiveByte(void)
{
    /* Wait until RXFE = 0 (data available) */
    while (UART1_FR_R & (1U << 4)) { }
    return (uint8_t)(UART1_DR_R & 0xFF);
}

void UART1_SendString(const char *str)
{
    while (*str)
    {
        UART1_SendByte((uint8_t)*str++);
    }
}
