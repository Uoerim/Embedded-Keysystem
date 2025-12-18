#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "../Common/Std_Types.h"

void UART1_Init(uint32_t baudrate);
void UART1_SendByte(uint8_t data);
void UART1_SendString(const char *str);
void UART1_FlushRx(void);


uint8_t UART1_ReceiveByte(void); /* blocking */

Std_ReturnType UART1_ReceiveByteTimeout(uint32_t timeout_ms, uint8_t *out);

#endif /* UART_H_ */
