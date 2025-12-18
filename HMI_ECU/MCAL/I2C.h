#ifndef I2C_H_
#define I2C_H_

#include <stdint.h>
#include "../Common/Std_Types.h"

void I2C0_Init_100k_16MHz(void);
Std_ReturnType I2C0_WriteByte(uint8_t slave_addr_7bit, uint8_t data, uint32_t timeout_ms);

#endif /* I2C_H_ */
