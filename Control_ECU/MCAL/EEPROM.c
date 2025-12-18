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

#define EE_MAGIC                (0xA5A5C0DEu)
#define EE_BLOCK                (0u)

/* Offsets (32-bit words) */
#define OFF_MAGIC               (0u)
#define OFF_INIT                (1u)
#define OFF_TIMEOUT             (2u)
#define OFF_PASS01_23           (3u)
#define OFF_PASS4               (4u)

/* Limits */
#define TIMEOUT_MIN_SEC         (5u)
#define TIMEOUT_MAX_SEC         (30u)
#define INIT_VALID_MAX          (1u)

/* EEDONE bits */
#define EEPROM_EEDONE_WORKING_MASK   (1u << 0)  /* WORKING */

/* Minimal �wait until not working� */
static void EEPROM_WaitReady(void)
{
    while ((EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING_MASK) != 0u)
    {
        /* wait */
    }
}

static uint32_t EEPROM_ReadWord(uint32_t offset)
{
    EEPROM_EEBLOCK_R  = EE_BLOCK;
    EEPROM_EEOFFSET_R = offset;
    EEPROM_WaitReady();
    return EEPROM_EERDWR_R;
}

static void EEPROM_WriteWord(uint32_t offset, uint32_t data)
{
    EEPROM_EEBLOCK_R  = EE_BLOCK;
    EEPROM_EEOFFSET_R = offset;
    EEPROM_EERDWR_R   = data;
    EEPROM_WaitReady();
}

static uint8_t ClampTimeout(uint8_t t)
{
    if (t < TIMEOUT_MIN_SEC) { t = TIMEOUT_MIN_SEC; }
    if (t > TIMEOUT_MAX_SEC) { t = TIMEOUT_MAX_SEC; }
    return t;
}

void EEPROM0_Init(void)
{
    /* Enable EEPROM clock */
    SYSCTL_RCGCEEPROM_R |= (1u << 0);
    (void)SYSCTL_RCGCEEPROM_R;

    EEPROM_WaitReady();

    /* If support register indicates a problem, we can't fix much here.
     * You may log or handle it in higher layer if needed.
     */
    (void)EEPROM_EESUPP_R;
}

uint8_t EEPROM_Load(char pass5[5], uint8_t *timeout_sec, uint8_t *initialized)
{
    uint32_t magic;
    uint32_t initw;
    uint32_t timew;
    uint32_t p0123;
    uint32_t p4w;
    uint8_t  init_val;
    uint8_t  t;

    /* Validate pointers */
    if ((pass5 == (char *)0) || (timeout_sec == (uint8_t *)0) || (initialized == (uint8_t *)0))
    {
        return 0u;
    }

    magic = EEPROM_ReadWord(OFF_MAGIC);
    if (magic != EE_MAGIC)
    {
        return 0u; /* not valid */
    }

    initw = EEPROM_ReadWord(OFF_INIT);
    timew = EEPROM_ReadWord(OFF_TIMEOUT);
    p0123 = EEPROM_ReadWord(OFF_PASS01_23);
    p4w   = EEPROM_ReadWord(OFF_PASS4);

    init_val = (uint8_t)(initw & 0xFFu);
    if (init_val > INIT_VALID_MAX)
    {
        init_val = 0u;
    }
    *initialized = init_val;

    t = (uint8_t)(timew & 0xFFu);
    t = ClampTimeout(t);
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
    uint32_t p0123;
    uint32_t p4w;
    uint8_t  t;
    uint8_t  init_val;

    /* Validate pointer */
    if (pass5 == (const char *)0)
    {
        return;
    }

    t = ClampTimeout(timeout_sec);

    init_val = initialized;
    if (init_val > INIT_VALID_MAX)
    {
        init_val = 0u;
    }

    p0123 =
        ((uint32_t)(uint8_t)pass5[0]) |
        ((uint32_t)(uint8_t)pass5[1] << 8) |
        ((uint32_t)(uint8_t)pass5[2] << 16) |
        ((uint32_t)(uint8_t)pass5[3] << 24);

    p4w = (uint32_t)(uint8_t)pass5[4];

    EEPROM_WriteWord(OFF_MAGIC, EE_MAGIC);
    EEPROM_WriteWord(OFF_INIT, (uint32_t)init_val);
    EEPROM_WriteWord(OFF_TIMEOUT, (uint32_t)t);
    EEPROM_WriteWord(OFF_PASS01_23, p0123);
    EEPROM_WriteWord(OFF_PASS4, p4w);
}

void EEPROM_Clear(void)
{
    /* simplest: destroy magic */
    EEPROM_WriteWord(OFF_MAGIC, 0u);
}
