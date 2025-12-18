#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "I2C.h"
#include "delay.h"

/* I2C0: PB2=SCL, PB3=SDA */
#define SYSCTL_RCGCI2C_I2C0_MASK        (1u << 0)
#define SYSCTL_RCGCGPIO_GPIOB_MASK      (1u << 1)

#define GPIO_PB2_MASK                   (1u << 2)
#define GPIO_PB3_MASK                   (1u << 3)
#define GPIO_PB23_MASK                  (GPIO_PB2_MASK | GPIO_PB3_MASK)

/* I2C Master Control/Status */
#define I2C_MCS_BUSY_MASK               (1u << 0)
#define I2C_MCS_ERROR_MASK              (1u << 1)
#define I2C_MCS_ARBLST_MASK             (1u << 4)

static Std_ReturnType I2C0_WaitDone(uint32_t timeout_ms)
{
    uint32_t start = Delay_GetTicksMs();

    while ((I2C0_MCS_R & I2C_MCS_BUSY_MASK) != 0u)
    {
        if ((Delay_GetTicksMs() - start) >= timeout_ms)
        {
            return E_NOT_OK;
        }
    }

    /* Check error bits */
    if ((I2C0_MCS_R & (I2C_MCS_ERROR_MASK | I2C_MCS_ARBLST_MASK)) != 0u)
    {
        return E_NOT_OK;
    }

    return E_OK;
}

void I2C0_Init_100k_16MHz(void)
{
    SYSCTL_RCGCI2C_R |= SYSCTL_RCGCI2C_I2C0_MASK;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_GPIOB_MASK;
    (void)SYSCTL_RCGCGPIO_R;

    /* PB2/PB3 alt function */
    GPIO_PORTB_AFSEL_R |= GPIO_PB23_MASK;
    GPIO_PORTB_DEN_R   |= GPIO_PB23_MASK;
    GPIO_PORTB_AMSEL_R &= ~GPIO_PB23_MASK;

    /* SDA open-drain */
    GPIO_PORTB_ODR_R |= GPIO_PB3_MASK;

    /* PCTL: function 3 for I2C */
    GPIO_PORTB_PCTL_R &= ~((0xFu << 8) | (0xFu << 12));
    GPIO_PORTB_PCTL_R |=  ((0x3u << 8) | (0x3u << 12));

    /* Master mode */
    I2C0_MCR_R = 0x10u;

    /* 100 kHz @ 16 MHz -> TPR = 7 (common setting) */
    I2C0_MTPR_R = 7u;
}

Std_ReturnType I2C0_WriteByte(uint8_t slave_addr_7bit, uint8_t data, uint32_t timeout_ms)
{
    /* MSA: [7:1]=addr, bit0=0 for write */
    I2C0_MSA_R = ((uint32_t)slave_addr_7bit << 1);

    I2C0_MDR_R = (uint32_t)data;

    /* START + RUN + STOP */
    I2C0_MCS_R = 0x07u;

    return I2C0_WaitDone(timeout_ms);
}
