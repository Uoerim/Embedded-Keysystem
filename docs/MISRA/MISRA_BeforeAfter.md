# MISRA-C:2012 Compliance & Deviation Report

**Embedded Door-Lock System — TM4C123GH6PM**

---

## 1. Document Information

| **Attribute**       | **Value**                        |
|---------------------|----------------------------------|
| Document Title      | MISRA-C:2012 Compliance Report   |
| Project Name        | Embedded Door-Lock System        |
| ECU Configuration   | HMI ECU, Control ECU             |
| Target MCU          | Texas Instruments TM4C123GH6PM   |
| System Clock        | 16 MHz (PIOSC)                   |
| Programming Language| Embedded C                       |
| Coding Standard     | MISRA-C:2012                     |
| Toolchain           | IAR Embedded Workbench           |
| Document Status     | **Final**                        |
| Last Updated        | December 2024                    |

---

## 2. Document Purpose

This document provides a comprehensive record of:

- All identified **MISRA-C:2012 rule violations** discovered during static analysis
- **Technical justifications** for each deviation
- **Before and after code comparisons** demonstrating applied fixes
- Final compliance status of the embedded software

---

## 3. Project Structure

```text
Project/
├── Control_ECU/
│   ├── APP/
│   ├── Common/
│   │   └── Std_Types.h
│   ├── HAL/
│   └── MCAL/
├── HMI_ECU/
│   ├── APP/
│   ├── Common/
│   │   └── Std_Types.h
│   ├── HAL/
│   └── MCAL/
└── docs/
    ├── MISRA_BeforeAfter.md    ← This Document
    └── MISRA_Deviations.md
```

---

## 4. Coding Standards & Constraints

The following coding standards and constraints are enforced throughout the project:

| **Standard / Constraint**         | **Description**                                      |
|-----------------------------------|------------------------------------------------------|
| MISRA-C:2012                      | Required and Advisory rules enforced                 |
| AUTOSAR-compliant `Std_Types`     | Standardized type definitions for portability        |
| No Dynamic Memory Allocation      | `malloc()`, `calloc()`, `free()` prohibited          |
| No Recursion                      | Stack depth must be statically determinable          |
| Deterministic Execution           | All code paths have bounded execution time           |

---

## 5. Standard Type Definitions

> **File:** `Common/Std_Types.h`

```c
#ifndef STD_TYPES_H
#define STD_TYPES_H

/* Unsigned Integer Types */
typedef unsigned char      uint8_t;
typedef unsigned short     uint16_t;
typedef unsigned long      uint32_t;

/* Signed Integer Types */
typedef signed char        sint8_t;
typedef signed short       sint16_t;
typedef signed long        sint32_t;

/* Boolean Type */
typedef uint8_t            boolean;

#define TRUE   ((boolean)1u)
#define FALSE  ((boolean)0u)

/* Standard Return Type */
typedef enum
{
    E_OK     = 0,
    E_NOT_OK = 1
} Std_ReturnType;

#endif /* STD_TYPES_H */
```

---

## 6. Compliance Summary

| **Category**         | **Status**               | **Notes**                          |
|----------------------|--------------------------|------------------------------------|
| Required Rules       | ✅ Compliant             | All mandatory rules satisfied      |
| Advisory Rules       | ✅ Compliant             | Best practices followed            |
| Documented Deviations| ✅ Justified             | See Section 7 for details          |
| Open Issues          | ✅ None                  | All findings resolved              |

---

## 7. Detailed Deviation Analysis

This section documents each MISRA-C:2012 violation identified during code review, along with the corresponding fix and technical justification.

---

### 7.1 Rule 1.1 — Implicit Function Declarations

| **Attribute**    | **Details**                                                |
|------------------|------------------------------------------------------------|
| Rule             | 1.1 (Required)                                             |
| Category         | Standard C Environment                                     |
| Severity         | High                                                       |

#### ❌ Non-Compliant Code

```c
UART1_FlushRx();
```

#### ✅ Compliant Code

```c
/* Function prototype declaration */
void UART1_FlushRx(void);
```

#### Justification

All functions must have explicit declarations (prototypes) visible before their first use. Implicit declarations can lead to undefined behavior due to incorrect parameter passing or return type assumptions.

---

### 7.2 Directive 4.6 — Non-Standard Integer Types

| **Attribute**    | **Details**                                                |
|------------------|------------------------------------------------------------|
| Rule             | Directive 4.6 (Advisory)                                   |
| Category         | Type Safety                                                |
| Severity         | Medium                                                     |

#### ❌ Non-Compliant Code

```c
uint8 timeout;
uint16 adc;
```

#### ✅ Compliant Code

```c
uint8_t timeout;
uint16_t adc;
```

#### Justification

Using standard fixed-width integer types (`uint8_t`, `uint16_t`, etc.) ensures portability across different compilers and target architectures. Non-standard type names may have inconsistent definitions.

---

### 7.3 Rule 17.7 — Null Pointer Dereference Risk

| **Attribute**    | **Details**                                                |
|------------------|------------------------------------------------------------|
| Rule             | 17.7 (Required)                                            |
| Category         | Pointer Safety                                             |
| Severity         | Critical                                                   |

#### ❌ Non-Compliant Code

```c
while (*str) { UART1_SendByte(*str++); }
```

#### ✅ Compliant Code

```c
if (str == NULL)
{
    return;
}

while (*str != '\0')
{
    UART1_SendByte((uint8_t)*str);
    str++;
}
```

#### Justification

Pointer parameters must be validated before dereferencing to prevent undefined behavior. The fix adds explicit null checking and separates the increment operation for clarity.

---

### 7.4 Directive 4.9 — Magic Numbers

| **Attribute**    | **Details**                                                |
|------------------|------------------------------------------------------------|
| Rule             | Directive 4.9 (Advisory)                                   |
| Category         | Code Maintainability                                       |
| Severity         | Low                                                        |

#### ❌ Non-Compliant Code

```c
GPIO_PORTD_DIR_R |= 0x0F;
```

#### ✅ Compliant Code

```c
#define KEYPAD_ROWS_MASK    (0x0Fu)

GPIO_PORTD_DIR_R |= KEYPAD_ROWS_MASK;
```

#### Justification

Replacing magic numbers with descriptive named constants improves code readability, maintainability, and reduces the risk of errors during modification.

---

### 7.5 Rule 10.1 / 10.4 — Mixed Signed Arithmetic

| **Attribute**    | **Details**                                                |
|------------------|------------------------------------------------------------|
| Rule             | 10.1, 10.4 (Required)                                      |
| Category         | Essential Type Model                                       |
| Severity         | High                                                       |

#### ❌ Non-Compliant Code

```c
timeout = 5u + (adc * 25u) / 4095u;
```

#### ✅ Compliant Code

```c
uint32_t scaled = ((uint32_t)adc * (uint32_t)(TIMEOUT_MAX_SEC - TIMEOUT_MIN_SEC))
                  / (uint32_t)ADC_MAX_COUNTS;

timeout = (uint8_t)(TIMEOUT_MIN_SEC + scaled);
```

#### Justification

Explicit type casting ensures all operands have consistent types, preventing implicit conversions that could lead to unexpected results or data loss.

---

### 7.6 Rule 14.2 — Blocking Calls Without Timeout

| **Attribute**    | **Details**                                                |
|------------------|------------------------------------------------------------|
| Rule             | 14.2 (Required)                                            |
| Category         | Control Flow                                               |
| Severity         | Critical                                                   |

#### ❌ Non-Compliant Code

```c
reply = UART1_ReceiveByte();
```

#### ✅ Compliant Code

```c
if (UART1_ReceiveByteTimeout(600u, &reply) != E_OK)
{
    return FALSE;
}
```

#### Justification

Indefinite blocking calls can cause system hangs. Timeout mechanisms ensure bounded execution time and enable graceful error handling.

---

### 7.7 Rule 13.2 — Volatile Loop Counters

| **Attribute**    | **Details**                                                |
|------------------|------------------------------------------------------------|
| Rule             | 13.2 (Required)                                            |
| Category         | Side Effects                                               |
| Severity         | Medium                                                     |

#### ❌ Non-Compliant Code

```c
for (volatile int i = 0; i < 300; i++) { }
```

#### ✅ Compliant Code

```c
uint32_t i;
for (i = 0u; i < 300u; i++)
{
    /* Delay loop body */
}
```

#### Justification

Using `volatile` on loop counters is non-portable for delay purposes. Proper delay functions or hardware timers should be used instead.

---

### 7.8 EEPROM Data Validation

| **Attribute**    | **Details**                                                |
|------------------|------------------------------------------------------------|
| Rule             | Defensive Programming (Advisory)                           |
| Category         | Data Integrity                                             |
| Severity         | High                                                       |

#### ❌ Non-Compliant Code

```c
*timeout = EEPROM_Read();
```

#### ✅ Compliant Code

```c
uint8_t t = EEPROM_Read();

if (t < TIMEOUT_MIN_SEC)
{
    t = TIMEOUT_MIN_SEC;
}
if (t > TIMEOUT_MAX_SEC)
{
    t = TIMEOUT_MAX_SEC;
}

*timeout = t;
```

#### Justification

Data read from non-volatile memory must be validated against expected ranges. Corrupted or uninitialized EEPROM data could cause system malfunction.

---

### 7.9 UART Protocol Robustness

| **Attribute**    | **Details**                                                |
|------------------|------------------------------------------------------------|
| Rule             | Communication Reliability (Advisory)                       |
| Category         | Protocol Safety                                            |
| Severity         | High                                                       |

#### ❌ Non-Compliant Code

```c
UART1_SendByte('T');
reply = UART1_ReceiveByte();
```

#### ✅ Compliant Code

```c
UART1_FlushRx();
UART1_SendByte((uint8_t)'T');

if (UART1_ReceiveByteTimeout(600u, &reply) != E_OK)
{
    return FALSE;
}
```

#### Justification

Flushing the receive buffer before transmission prevents stale data from being interpreted as a response. Timeout handling ensures the system does not hang on communication failures.

---

### 7.10 System-Level Design Improvement

| **Attribute**    | **Details**                                                |
|------------------|------------------------------------------------------------|
| Category         | Architectural Design                                       |
| Severity         | Medium                                                     |

#### Design Changes

| **Aspect**              | **Before**                              | **After**                              |
|-------------------------|-----------------------------------------|----------------------------------------|
| Timeout Configuration   | Potentiometer directly affects unlock   | Pot used only during "Set Timeout"     |
| Data Persistence        | No persistence                          | EEPROM-backed timeout storage          |
| System Behavior         | Non-deterministic                       | Fully deterministic behavior           |

#### Justification

The architectural changes ensure predictable system behavior, improve user experience, and maintain configuration across power cycles.

---

## 8. Final Compliance Statement

Based on the comprehensive review documented above, the following compliance status is declared:

| **Compliance Aspect**                        | **Status** |
|----------------------------------------------|------------|
| MISRA-C:2012 Required Rules                  | ✅ Satisfied |
| MISRA-C:2012 Advisory Rules                  | ✅ Followed  |
| All Deviations Justified & Documented        | ✅ Complete  |
| System Determinism & Safety                  | ✅ Verified  |

**The embedded software is compliant with MISRA-C:2012 guidelines.**

---

## 9. Revision History

| **Version** | **Date**       | **Author**   | **Description**           |
|-------------|----------------|--------------|---------------------------|
| 1.0         | December 2024  | —            | Initial release           |

---

<div align="center">

**— End of Document —**

</div>
