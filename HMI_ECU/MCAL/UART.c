#include "TM4C123GH6PM.h"
#include "UART.h"

void UART1_Init(uint32_t baudrate)
{
    SYSCTL_RCGCUART_R |= (1U << 1);   // UART1
    SYSCTL_RCGCGPIO_R |= (1U << 1);   // GPIOB
    (void)SYSCTL_RCGCGPIO_R;

    GPIO_PORTB_AFSEL_R |= (1U << 0) | (1U << 1);
    GPIO_PORTB_DEN_R   |= (1U << 0) | (1U << 1);
    GPIO_PORTB_PCTL_R  &= ~((0xF << 0) | (0xF << 4));
    GPIO_PORTB_PCTL_R  |=  ((0x1 << 0) | (0x1 << 4));   // U1Rx, U1Tx
    GPIO_PORTB_AMSEL_R &= ~((1U << 0) | (1U << 1));

    UART1_CTL_R &= ~(1U << 0);        // disable UART

    uint32_t brd  = 16000000UL / (16UL * baudrate);
    uint32_t frac = ( (16000000UL * 4UL) / baudrate + 1UL ) / 2UL - (brd * 64UL);

    UART1_IBRD_R = (uint16_t)brd;
    UART1_FBRD_R = (uint8_t)frac;

    UART1_LCRH_R = (0x3 << 5);        // 8N1
    UART1_CTL_R |= (1U << 8) | (1U << 9); // TXE, RXE
    UART1_CTL_R |= (1U << 0);             // UARTEN
}

void UART1_SendByte(uint8_t data)
{
    while (UART1_FR_R & (1U << 5)) { } // TXFF
    UART1_DR_R = data;
}

uint8_t UART1_ReceiveByte(void)
{
    while (UART1_FR_R & (1U << 4)) { } // RXFE
    return (uint8_t)(UART1_DR_R & 0xFF);
}

void UART1_SendString(const char *str)
{
    while (*str)
        UART1_SendByte((uint8_t)*str++);
}
