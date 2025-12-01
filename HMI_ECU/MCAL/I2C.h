#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>

void I2C0_Init(void);
void I2C0_WriteByte(uint8_t slaveAddr, uint8_t data);

#endif
