#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "I2C.h"

void I2C0_Init(void)
{
    // Enable clock for I2C0 and GPIOB
    SYSCTL_RCGCI2C_R |= (1U << 0);   // I2C0
    SYSCTL_RCGCGPIO_R |= (1U << 1);  // GPIOB

    // Configure PB2 (SCL) and PB3 (SDA) for I2C0
    GPIO_PORTB_AFSEL_R |= (1U << 2) | (1U << 3);
    GPIO_PORTB_DEN_R   |= (1U << 2) | (1U << 3);
    GPIO_PORTB_ODR_R   |= (1U << 3);         // SDA open-drain

    // PCTL: set PB2/PB3 to I2C0 (function 3)
    GPIO_PORTB_PCTL_R &= ~((0xF << 8) | (0xF << 12));
    GPIO_PORTB_PCTL_R |=  (0x3 << 8) | (0x3 << 12);

    // Initialize I2C0 Master
    I2C0_MCR_R = 0x10;               // Master mode

    // 100 kHz SCL @ 16 MHz
    I2C0_MTPR_R = 7;
}

void I2C0_WriteByte(uint8_t slaveAddr, uint8_t data)
{
    I2C0_MSA_R = (slaveAddr << 1);   // 7-bit address, write
    I2C0_MDR_R = data;

    I2C0_MCS_R = 0x07;               // START + RUN + STOP

    // wait until BUSY bit clears
    while (I2C0_MCS_R & 0x01) { }
}
