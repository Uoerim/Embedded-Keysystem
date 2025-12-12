#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>

void Motor_Init(void);
void Motor_Open(void);    // rotate to "unlock" direction
void Motor_Close(void);   // rotate to "lock" direction
void Motor_Stop(void);

#endif /* MOTOR_H_ */
