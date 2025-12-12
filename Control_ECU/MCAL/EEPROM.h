#ifndef EEPROM_MCAL_H_
#define EEPROM_MCAL_H_

#include <stdint.h>

void EEPROM0_Init(void);

/* returns 1 if valid data exists */
uint8_t EEPROM_Load(char pass5[5], uint8_t *timeout_sec, uint8_t *initialized);

/* write full state */
void EEPROM_Save(const char pass5[5], uint8_t timeout_sec, uint8_t initialized);

/* clear stored state (back to “first run”) */
void EEPROM_Clear(void);

#endif
