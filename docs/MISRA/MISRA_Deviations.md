# MISRA-C:2012 Deviations
**Project:** TM4C123GH6PM Door Lock System (HMI ECU + Control ECU)  
**Standard:** MISRA C:2012 (Guidelines for the use of the C language in critical systems)  
**Version:** 1.0  
**Date:** ____/____/2025  
**Owner:** __________________

---

## 1. Purpose
This document records **approved deviations** from MISRA-C:2012 for this project.  
Deviations are allowed only when:
- There is a **hardware / toolchain requirement** (bare-metal MCU, registers, ISR, etc.)
- The risk is **understood and mitigated**
- The code remains **readable, testable, and reviewable**

---

## 2. Scope
Applies to:
- HMI ECU application + drivers (UART, I2C, LCD, Keypad, ADC, SysTick/Delay, Buzzer)
- Control ECU application + drivers (UART, EEPROM, Motor, RGB, Delay)

Out of scope:
- Vendor header `TM4C123GH6PM.h` (third-party / device header)
- Toolchain startup files / linker scripts

---

## 3. Assumptions and Constraints
- **Bare-metal** (no OS)
- System clock fixed at **16 MHz**
- Hardware access is done using **memory-mapped registers** from `TM4C123GH6PM.h`
- Timing is achieved using SysTick tick delay or calibrated loops (depending on module)

---

## 4. Deviation Process
Each deviation must include:
- MISRA rule/directive reference (MISRA-C:2012)
- Reason (why MISRA can’t be followed strictly)
- Risk
- Mitigation (what we do to keep it safe)
- Scope (files/functions affected)
- Approval

---

## 5. Approved Deviations (Summary)
> Note: exact rule IDs may differ slightly depending on your static analysis tool interpretation.  
> These deviations are written to match common MISRA-C:2012 interpretations used in embedded projects.

| ID | MISRA Ref | Title | Reason | Scope |
|---:|-----------|-------|--------|-------|
| D1 | Rule 11.x | Pointer/integer conversion for register access | Hardware register mapping | All MCAL/HAL drivers |
| D2 | Rule 2.2 / 14.2 | Intentional infinite loops (`for(;;)`) | Embedded main loop / polling | APP main loops, keypad wait |
| D3 | Rule 21.3 / 21.4 | Direct use of low-level device header | MCU header provides volatile registers | All modules including TM4C header |
| D4 | Rule 8.7 / 8.9 | File-scope `static` state used for drivers | Driver internal state, module encapsulation | delay.c (ticks), app state, etc. |
| D5 | Rule 10.x | Explicit casts in arithmetic scaling | Avoid implicit promotions & keep fixed mapping | ADC → timeout mapping |
| D6 | Dir 4.12 / Rule 13.2 (tool-dependent) | Busy-wait delays | No RTOS timer; simple deterministic waits | LCD init delays, short beeps |
| D7 | Rule 17.7 | Ignored return values from HW access | Some HW writes are “fire-and-forget” | I2C write, register writes |

---

## D1 — Register access requires pointer/integer conversions
**MISRA Ref:** Rule 11.4 / 11.6 / 11.3 (tool-dependent)  
**Description:** Conversion between integer and pointer types may be flagged when accessing memory mapped registers.  
**Reason:** TM4C uses memory-mapped peripherals; address-based register definitions are required.  
**Risk:** Incorrect address/cast could write unintended memory.  
**Mitigation:**
- Use **vendor-provided** `TM4C123GH6PM.h` register definitions (volatile).
- Do not create new raw addresses in application code.
- Restrict register access to MCAL/HAL layers.
- Code review checklist includes “no new hard-coded addresses”.
**Scope (examples):**
- `MCAL/UART.c`, `MCAL/I2C.c`, `MCAL/ADC.c`, `MCAL/delay.c`, `MCAL/EEPROM.c`
- Any file including `TM4C123GH6PM.h`
**Approval:** ____________ (Name/Date)

---

## D2 — Intentional infinite loops for embedded control flow
**MISRA Ref:** Rule 2.2 (unreachable code), Rule 14.2 (control flow), and related “no-termination” warnings (tool-dependent)  
**Description:** Embedded firmware frequently uses `for(;;)` loops for main polling and waiting for events.  
**Reason:** No OS; firmware is intended to run indefinitely.  
**Risk:** If a loop waits forever, the system might appear frozen.  
**Mitigation:**
- For communication waits, use **timeouts** where possible (UART receive timeout).
- In critical loops, provide error handling (e.g., show “Timeout Err”, retry).
- Watchdog can be added if required (future improvement).
**Scope (examples):**
- `APP/main.c` loops in HMI and Control
- `Keypad_GetKey()` blocking scan loops (or timeout version)
**Approval:** ____________ (Name/Date)

---

## D3 — Dependence on device header / low-level hardware definitions
**MISRA Ref:** Rule 21.3 / 21.4 (standard library & implementation-defined), and “external headers not MISRA-compliant” (tool-dependent)  
**Description:** `TM4C123GH6PM.h` is a vendor header that may not comply with MISRA formatting and macros.  
**Reason:** Required for correct register definitions and bit fields.  
**Risk:** Header may contain non-MISRA constructs (macros, unnamed structs/unions).  
**Mitigation:**
- Treat as **trusted third-party** and do not modify it.
- Keep it isolated: include it only in MCAL/HAL where needed.
- Application layer should not directly touch registers unless justified.
**Scope:** All files that include `TM4C123GH6PM.h`  
**Approval:** ____________ (Name/Date)

---

## D4 — File-scope static state in drivers / application
**MISRA Ref:** Rule 8.7 / 8.9 (object definitions at file scope, visibility)  
**Description:** Some modules keep internal state using `static` file-scope variables.  
**Reason:** Encapsulation (e.g., tick counter `g_msTicks`, cached timeout/password state).  
**Risk:** Hidden state can complicate testing if not controlled.  
**Mitigation:**
- Keep state **private** (`static`) and expose API to reset/load if needed.
- Unit tests may include a driver reset/init function.
- Avoid writing to these variables outside their module.
**Scope (examples):**
- `MCAL/delay.c` tick counter
- `APP/main.c` state variables for password/timeout
**Approval:** ____________ (Name/Date)

---

## D5 — Explicit casts required for scaling arithmetic (ADC mapping, UART)
**MISRA Ref:** Rule 10.1 / 10.3 / 10.4 / 10.8 (essential type model)  
**Description:** Mapping ADC counts (0..4095) to timeout seconds (5..30) requires controlled integer arithmetic.  
**Reason:** Without casts, compilers may apply implicit promotions; MISRA flags mixed essential types.  
**Risk:** Overflow or wrong scaling if done incorrectly.  
**Mitigation:**
- Use `uint32_t` for intermediate multiplication.
- Clamp output to `[TIMEOUT_MIN_SEC..TIMEOUT_MAX_SEC]`.
- Unit test boundary cases: ADC=0, ADC=4095, mid-range.
**Scope (examples):**
- `APP/main.c` pot mapping helper
- `MCAL/ADC.c` read function return types
**Approval:** ____________ (Name/Date)

---

## D6 — Busy-wait delays for hardware timing (LCD/I2C, buzzer)
**MISRA Ref:** Directive 4.12 (resource usage), Rule 13.2 (side effects / timing) and performance warnings (tool-dependent)  
**Description:** Some peripherals require short deterministic waits during init (HD44780 timing, I2C strobes).  
**Reason:** Simple project requirement, no RTOS, and LCD init needs micro/millisecond waits.  
**Risk:** CPU wasted during busy-wait; can affect responsiveness.  
**Mitigation:**
- Restrict busy-waits to init or short feedback durations.
- Prefer SysTick tick-based delays where practical (`Delay_ms()`).
- Document all calibrated delay constants.
**Scope (examples):**
- `HAL/LCD.c` initialization & enable strobes
- `HAL/Buzzer.c` short beep timing
**Approval:** ____________ (Name/Date)

---

## D7 — Ignoring some return values / status results from HW writes
**MISRA Ref:** Rule 17.7 (value returned by a function should be used)  
**Description:** Some low-level HW writes (e.g., register writes, “write byte” operations) may not return status. If they do, code might ignore it.  
**Reason:** Project uses minimal drivers; some functions are “fire-and-forget”.  
**Risk:** Silent failure (e.g., I2C NACK not detected).  
**Mitigation:**
- For critical operations, implement status returns (future improvement: I2C error flags).
- Integration tests validate LCD + I2C and UART link behavior.
**Scope (examples):**
- `MCAL/I2C.c` write byte
- `HAL/LCD.c` I2C write calls
**Approval:** ____________ (Name/Date)

---

## 6. Notes for the Report
- This file is the **deviation register**.
- Your “5 violations with before/after” belongs in a separate file:
  - `docs/MISRA_BeforeAfter.md`

---

## 7. Approvals
| Role | Name | Signature | Date |
|------|------|-----------|------|
| Developer | | | |
| Reviewer | | | |
| Instructor/TA (if required) | | | |
