#include <stdint.h>
#include "TM4C123GH6PM.h"
#include "EEPROM.h"

/*
EEPROM mapping (Block 0):
  offset 0 : MAGIC (0xA5A5C0DE)
  offset 1 : initialized (0/1)
  offset 2 : timeout_sec (5..30)
  offset 3 : pass bytes packed: p0 p1 p2 p3
  offset 4 : pass byte p4 in low byte
*/

#define EE_MAGIC        0xA5A5C0DEu
#define EE_BLOCK        0u

#define OFF_MAGIC       0u
#define OFF_INIT        1u
#define OFF_TIMEOUT     2u
#define OFF_PASS01_23   3u
#define OFF_PASS4       4u

static void eeprom_wait_ready(void)
{
    while (EEPROM_EEDONE_R & (1u << 0)) { } /* WORKING bit */
}

void EEPROM0_Init(void)
{
    SYSCTL_RCGCEEPROM_R |= (1u << 0);   /* enable EEPROM clock */
    (void)SYSCTL_RCGCEEPROM_R;

    eeprom_wait_ready();

    /* Optional: clear errors if any */
    if (EEPROM_EESUPP_R != 0)
    {
        /* If EESUPP indicates problems, still try to continue */
    }
}

static uint32_t ee_read_word(uint32_t offset)
{
    EEPROM_EEBLOCK_R  = EE_BLOCK;
    EEPROM_EEOFFSET_R = offset;
    eeprom_wait_ready();
    return EEPROM_EERDWR_R;
}

static void ee_write_word(uint32_t offset, uint32_t data)
{
    EEPROM_EEBLOCK_R  = EE_BLOCK;
    EEPROM_EEOFFSET_R = offset;
    EEPROM_EERDWR_R   = data;
    eeprom_wait_ready();
}

uint8_t EEPROM_Load(char pass5[5], uint8_t *timeout_sec, uint8_t *initialized)
{
    uint32_t magic = ee_read_word(OFF_MAGIC);
    if (magic != EE_MAGIC)
    {
        return 0u; /* not valid */
    }

    uint32_t initw = ee_read_word(OFF_INIT);
    uint32_t timew = ee_read_word(OFF_TIMEOUT);
    uint32_t p0123 = ee_read_word(OFF_PASS01_23);
    uint32_t p4w   = ee_read_word(OFF_PASS4);

    *initialized = (uint8_t)(initw & 0xFFu);

    uint8_t t = (uint8_t)(timew & 0xFFu);
    if (t < 5u) t = 5u;
    if (t > 30u) t = 30u;
    *timeout_sec = t;

    pass5[0] = (char)( p0123        & 0xFFu);
    pass5[1] = (char)((p0123 >>  8) & 0xFFu);
    pass5[2] = (char)((p0123 >> 16) & 0xFFu);
    pass5[3] = (char)((p0123 >> 24) & 0xFFu);
    pass5[4] = (char)( p4w          & 0xFFu);

    return 1u;
}

void EEPROM_Save(const char pass5[5], uint8_t timeout_sec, uint8_t initialized)
{
    uint32_t p0123 =
        ((uint32_t)(uint8_t)pass5[0]) |
        ((uint32_t)(uint8_t)pass5[1] << 8) |
        ((uint32_t)(uint8_t)pass5[2] << 16) |
        ((uint32_t)(uint8_t)pass5[3] << 24);

    uint32_t p4w = (uint32_t)(uint8_t)pass5[4];

    ee_write_word(OFF_MAGIC, EE_MAGIC);
    ee_write_word(OFF_INIT, (uint32_t)initialized);
    ee_write_word(OFF_TIMEOUT, (uint32_t)timeout_sec);
    ee_write_word(OFF_PASS01_23, p0123);
    ee_write_word(OFF_PASS4, p4w);
}

void EEPROM_Clear(void)
{
    /* simplest: destroy magic */
    ee_write_word(OFF_MAGIC, 0u);
}
