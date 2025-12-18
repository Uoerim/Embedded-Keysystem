#ifndef UART_H_
#define UART_H_

#include <stdint.h>

void UART1_Init(uint32_t baudrate);
void UART1_SendByte(uint8_t data);
uint8_t UART1_ReceiveByte(void);          /* blocking */
void UART1_SendString(const char *str);

#endif /* UART_H_ */
