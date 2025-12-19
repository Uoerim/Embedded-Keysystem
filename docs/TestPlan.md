# Test Framework Documentation

**Embedded Door-Lock System — TM4C123GH6PM**

---

## 1. Document Information

| **Attribute**       | **Value**                          |
|---------------------|------------------------------------|
| Document Title      | Test Framework & Test Plan         |
| Project Name        | Embedded Door-Lock System          |
| ECU Configuration   | HMI ECU, Control ECU               |
| Target MCU          | Texas Instruments TM4C123GH6PM     |
| Test Framework      | Custom Embedded Test Framework v1.0|
| Document Status     | **Final**                          |
| Last Updated        | December 2024                      |

---

## 2. Test Architecture Overview

The test framework implements a **3-layer testing strategy**:

```
┌─────────────────────────────────────────────────────────────┐
│                    LAYER 3: SYSTEM TESTS                     │
│              End-to-End Functional Scenarios                 │
│     (First Boot, Door Open/Close, Timeout, Lockout)         │
├─────────────────────────────────────────────────────────────┤
│                 LAYER 2: INTEGRATION TESTS                   │
│              Cross-Module Interactions                       │
│   (Keypad→LCD, ADC→LCD, UART Protocol, Motor+RGB)           │
├─────────────────────────────────────────────────────────────┤
│                   LAYER 1: UNIT TESTS                        │
│              Individual Driver Validation                    │
│    (GPIO, UART, ADC, I2C, EEPROM, LCD, Motor, RGB)          │
└─────────────────────────────────────────────────────────────┘
```

---

## 3. Project Test Structure

### 3.1 HMI ECU Test Files

```text
HMI_ECU/
└── TEST/
    ├── test_config.h              # Test configuration
    ├── test_log.h                 # Logger API
    ├── test_log.c                 # Logger implementation
    ├── test_runner.h              # Test orchestration API
    ├── test_runner.c              # Test orchestration
    ├── test_main.c                # Test entry point
    ├── test_cases_driver_gpio.h   # GPIO unit tests
    ├── test_cases_driver_gpio.c
    ├── test_cases_driver_uart.h   # UART unit tests
    ├── test_cases_driver_uart.c
    ├── test_cases_driver_adc.h    # ADC unit tests
    ├── test_cases_driver_adc.c
    ├── test_cases_driver_i2c.h    # I2C unit tests
    ├── test_cases_driver_i2c.c
    ├── test_cases_driver_delay.h  # Delay unit tests
    ├── test_cases_driver_delay.c
    ├── test_cases_driver_lcd.h    # LCD unit tests
    ├── test_cases_driver_lcd.c
    ├── test_cases_driver_keypad.h # Keypad unit tests
    ├── test_cases_driver_keypad.c
    ├── test_cases_driver_buzzer.h # Buzzer unit tests
    ├── test_cases_driver_buzzer.c
    ├── test_cases_integration.h   # Integration tests
    ├── test_cases_integration.c
    ├── test_cases_system.h        # System tests
    └── test_cases_system.c
```

### 3.2 Control ECU Test Files

```text
Control_ECU/
└── TEST/
    ├── test_config.h              # Test configuration
    ├── test_log.h                 # Logger API
    ├── test_log.c                 # Logger implementation
    ├── test_runner.h              # Test orchestration API
    ├── test_runner.c              # Test orchestration
    ├── test_main.c                # Test entry point
    ├── test_cases_driver_rgb.h    # RGB LED unit tests
    ├── test_cases_driver_rgb.c
    ├── test_cases_driver_motor.h  # Motor unit tests
    ├── test_cases_driver_motor.c
    ├── test_cases_driver_eeprom.h # EEPROM unit tests
    ├── test_cases_driver_eeprom.c
    ├── test_cases_driver_uart.h   # UART unit tests
    ├── test_cases_driver_uart.c
    ├── test_cases_driver_delay.h  # Delay unit tests
    ├── test_cases_driver_delay.c
    ├── test_cases_integration.h   # Integration tests
    ├── test_cases_integration.c
    ├── test_cases_system.h        # System tests
    └── test_cases_system.c
```

---

## 4. IAR Embedded Workbench Setup

### 4.1 Create TEST Configuration

1. **Open Project Options** (Alt+F7)
2. **Right-click project** → "New Configuration"
3. **Name:** `TEST`
4. **Based on:** `Debug`

### 4.2 Configure File Exclusions

**For TEST configuration:**

| Action    | Files                              |
|-----------|------------------------------------|
| Exclude   | `APP/main.c`                       |
| Include   | All files in `TEST/` folder        |

### 4.3 Build & Run

```
Project → Make (F7)
Project → Download and Debug (Ctrl+D)
```

---

## 5. Test Output Format

### 5.1 UART Logger Protocol

All test results are logged via **UART1** at **9600 baud**.

```text
===========================================
  EMBEDDED DOOR-LOCK SYSTEM - TEST FRAMEWORK v1.0.0
  Target: TM4C123GH6PM | ECU: HMI
===========================================

>>> Suite: GPIO
-------------------------------------------
[PASS] GPIO::InitLED_SetsRegisters
[PASS] GPIO::ToggleLED_ChangesState
<<< End: GPIO

>>> Suite: UART
-------------------------------------------
[PASS] UART::Init_SetsControlRegisters
[PASS] UART::Init_SetsBaudRate
[FAIL] UART::ReceiveTimeout_ReturnsNotOK | expected=TRUE got=FALSE
<<< End: UART

===========================================
                  TEST SUMMARY
===========================================
  Total Tests : 15
  Passed      : 14
  Failed      : 1
  Skipped     : 0
  Duration    : 3245 ms
===========================================
  *** TESTS FAILED: 1 ***
===========================================
```

### 5.2 Log Message Formats

| Type | Format |
|------|--------|
| Pass | `[PASS] <Suite>::<TestName>` |
| Fail | `[FAIL] <Suite>::<TestName> \| expected=<x> got=<y>` |
| Skip | `[SKIP] <Suite>::<TestName> \| <reason>` |
| Info | `[INFO] <message>` |

---

## 6. Test Plan

### 6.1 HMI ECU Unit Tests

| Test ID | Suite | Test Name | Steps | Expected Result |
|---------|-------|-----------|-------|-----------------|
| HMI-U-001 | GPIO | InitLED_SetsRegisters | Call GPIOF_InitLED() | DIR & DEN bits set for PF1 |
| HMI-U-002 | GPIO | ToggleLED_ChangesState | Toggle LED twice | Data register changes |
| HMI-U-003 | UART | Init_SetsControlRegisters | Call UART1_Init(9600) | CTL: UARTEN=1, TXE=1, RXE=1 |
| HMI-U-004 | UART | Init_SetsBaudRate | Call UART1_Init(9600) | IBRD ≠ 0 |
| HMI-U-005 | UART | ReceiveTimeout_ReturnsNotOK | Receive with no data | Returns E_NOT_OK |
| HMI-U-006 | ADC | Init_EnablesSS3 | Call ADC_Init() | ACTSS ASEN3=1 |
| HMI-U-007 | ADC | Read_InValidRange | Read ADC value | 0 ≤ value ≤ 4095 |
| HMI-U-008 | I2C | Init_ConfiguresMaster | Call I2C0_Init_100k_16MHz() | MCR MFE=1, MTPR ≠ 0 |
| HMI-U-009 | I2C | Write_InvalidAddress_Timeout | Write to 0x7E | Returns E_NOT_OK |
| HMI-U-010 | Delay | TickCounter_Increments | Wait 50ms | Ticks increased |
| HMI-U-011 | Delay | Ms_Accuracy | Delay 100ms | Elapsed ≈ 100ms (±10%) |
| HMI-U-012 | LCD | Init_Completes | Call LCD_Init() | No hang |
| HMI-U-013 | LCD | SendString_SmokeTest | Write "TEST" | No hang, visible output |
| HMI-U-014 | Keypad | Init_ConfiguresGPIO | Call Keypad_Init() | DEN bits set |
| HMI-U-015 | Keypad | Timeout_NoKey_ReturnsNotOK | Read with no press | Returns E_NOT_OK |
| HMI-U-016 | Buzzer | Init_ConfiguresGPIO | Call Buzzer_Init() | No hang |
| HMI-U-017 | Buzzer | BeepShort_Works | Call Buzzer_BeepShort() | Audible beep |

### 6.2 HMI ECU Integration Tests

| Test ID | Suite | Test Name | Steps | Expected Result |
|---------|-------|-----------|-------|-----------------|
| HMI-I-001 | Integration | Keypad_LCD_AsteriskDisplay | Press key | LCD shows '*' |
| HMI-I-002 | Integration | ADC_LCD_TimeoutDisplay | Read ADC | LCD shows timeout (5-30s) |
| HMI-I-003 | Integration | UART_Protocol_CheckInit | Send 'I' | Receive '0' or '1' |
| HMI-I-004 | Integration | UART_Protocol_SetPassword | Send 'N' + 5 bytes | Receive 'K' |
| HMI-I-005 | Integration | UART_Protocol_VerifyPassword | Send 'V' + 5 bytes | Receive 'Y' or 'N' |
| HMI-I-006 | Integration | UART_Protocol_SetTimeout | Send 'T' + 2 digits | Receive 'K' |
| HMI-I-007 | Integration | UART_Protocol_GetTimeout | Send 'G' | Receive timeout value |

### 6.3 HMI ECU System Tests

| Test ID | Suite | Test Name | Scenario | Expected Result |
|---------|-------|-----------|----------|-----------------|
| HMI-S-001 | System | FirstBoot_NotInitialized | Fresh start | 'I' returns '0' |
| HMI-S-002 | System | FirstBoot_SetupPassword | Set password | 'I' returns '1' |
| HMI-S-003 | System | ValidPassword_DoorOpens | Enter correct pass | Door opens |
| HMI-S-004 | System | SetTimeout_FromPot | Read pot, set timeout | Timeout stored |
| HMI-S-005 | System | WrongPassword_Lockout | 3 wrong attempts | Lockout triggered |

### 6.4 Control ECU Unit Tests

| Test ID | Suite | Test Name | Steps | Expected Result |
|---------|-------|-----------|-------|-----------------|
| CTL-U-001 | RGB_LED | Init_SetsRegisters | Call RGB_LED_Init() | PC5/6/7 DIR=1, DEN=1 |
| CTL-U-002 | RGB_LED | SetColor_Red | Set RGB_RED | PC5=1, PC6=0, PC7=0 |
| CTL-U-003 | RGB_LED | SetColor_Green | Set RGB_GREEN | PC5=0, PC6=1, PC7=0 |
| CTL-U-004 | RGB_LED | SetColor_Blue | Set RGB_BLUE | PC5=0, PC6=0, PC7=1 |
| CTL-U-005 | RGB_LED | SetColor_Off | Set RGB_OFF | All pins = 0 |
| CTL-U-006 | RGB_LED | SetColor_White | Set RGB_WHITE | All pins = 1 |
| CTL-U-007 | Motor | Init_ConfiguresGPIO | Call Motor_Init() | Motor pins output |
| CTL-U-008 | Motor | Open_SetsDirectionPins | Call Motor_Open() | Direction pins set |
| CTL-U-009 | Motor | Close_SetsDirectionPins | Call Motor_Close() | Direction pins set |
| CTL-U-010 | Motor | Stop_ClearsPins | Call Motor_Stop() | All motor pins = 0 |
| CTL-U-011 | EEPROM | Init_Completes | Call EEPROM0_Init() | No hang |
| CTL-U-012 | EEPROM | SaveLoad_Password | Save/Load password | Data matches |
| CTL-U-013 | EEPROM | SaveLoad_Timeout | Save/Load timeout | Timeout matches |
| CTL-U-014 | EEPROM | Clear_Works | Clear EEPROM | Init flag = 0 |
| CTL-U-015 | EEPROM | DataValidation | Boundary values | Valid range maintained |

### 6.5 Control ECU Integration Tests

| Test ID | Suite | Test Name | Steps | Expected Result |
|---------|-------|-----------|-------|-----------------|
| CTL-I-001 | Integration | UART_EEPROM_SetTimeout | Set timeout via UART | EEPROM stores value |
| CTL-I-002 | Integration | UART_EEPROM_SetPassword | Set password via UART | EEPROM stores password |
| CTL-I-003 | Integration | Motor_RGB_OpenSequence | Open door | RGB green, motor runs |
| CTL-I-004 | Integration | Motor_RGB_CloseSequence | Close door | RGB red, motor runs |
| CTL-I-005 | Integration | CommandHandler_CheckInit | Check init status | Correct response |
| CTL-I-006 | Integration | CommandHandler_GetTimeout | Get timeout | Correct value |

### 6.6 Control ECU System Tests

| Test ID | Suite | Test Name | Scenario | Expected Result |
|---------|-------|-----------|----------|-----------------|
| CTL-S-001 | System | FullDoorOpenClose | Complete door cycle | Proper sequence |
| CTL-S-002 | System | PasswordVerification | Verify password logic | Correct/wrong detection |
| CTL-S-003 | System | TimeoutPersistence | Save, reinit, load | Value persists |

---

## 7. Hardware Setup for Testing

### 7.1 Required Connections

| Connection | Description |
|------------|-------------|
| UART1 TX→PC | Connect PB1 to USB-UART adapter for log output |
| HMI↔Control | Connect HMI UART1 to Control UART1 for protocol tests |
| Potentiometer | Connect to PE3 (ADC input) for timeout tests |
| LCD | I2C connection to PB2/PB3 |
| Keypad | Connect 4x4 matrix to PD0-PD3, PE0-PE3 |

### 7.2 Test Environment

```text
┌──────────────┐      UART1       ┌───────────────┐
│   HMI ECU    │◄────────────────►│  Control ECU  │
│  (TM4C123)   │                  │   (TM4C123)   │
└──────────────┘                  └───────────────┘
       │                                  │
       │ UART1                            │ UART1
       ▼                                  ▼
┌──────────────┐                  ┌───────────────┐
│  USB-UART    │                  │   USB-UART    │
│   Adapter    │                  │    Adapter    │
└──────────────┘                  └───────────────┘
       │                                  │
       ▼                                  ▼
   ┌────────────────────────────────────────┐
   │            PC Terminal                  │
   │     (Tera Term / PuTTY @ 9600)         │
   └────────────────────────────────────────┘
```

---

## 8. Running Tests

### 8.1 Quick Start

1. **Build TEST configuration** in IAR
2. **Flash to target MCU**
3. **Open terminal** at 9600 baud
4. **Reset MCU** to start tests
5. **Observe output** for PASS/FAIL results

### 8.2 Selective Test Execution

Modify `test_config.h` to enable/disable test levels:

```c
#define TEST_ENABLE_UNIT_TESTS          (1u)  /* Enable */
#define TEST_ENABLE_INTEGRATION_TESTS   (0u)  /* Disable */
#define TEST_ENABLE_SYSTEM_TESTS        (0u)  /* Disable */
```

### 8.3 Individual Driver Tests

```c
#define TEST_UNIT_GPIO                  (1u)  /* Enable GPIO tests */
#define TEST_UNIT_UART                  (0u)  /* Disable UART tests */
```

---

## 9. Sample Test Output

### 9.1 HMI ECU Test Log

```text
===========================================
  EMBEDDED DOOR-LOCK SYSTEM - TEST FRAMEWORK v1.0.0
  Target: TM4C123GH6PM | ECU: HMI
===========================================

[INFO] Test Runner initialized
[INFO] === UNIT TESTS ===

>>> Suite: GPIO
-------------------------------------------
[PASS] GPIO::InitLED_SetsRegisters
[PASS] GPIO::ToggleLED_ChangesState
<<< End: GPIO

>>> Suite: UART
-------------------------------------------
[PASS] UART::Init_SetsControlRegisters
[PASS] UART::Init_SetsBaudRate
[PASS] UART::ReceiveTimeout_ReturnsNotOK
<<< End: UART

>>> Suite: ADC
-------------------------------------------
[PASS] ADC::Init_EnablesSS3
[PASS] ADC::Read_InValidRange
[PASS] ADC::ReadTimeout_Works
<<< End: ADC

>>> Suite: Delay
-------------------------------------------
[PASS] Delay::TickCounter_Increments
[PASS] Delay::Ms_Accuracy
<<< End: Delay

>>> Suite: LCD
-------------------------------------------
[PASS] LCD::Init_Completes
[PASS] LCD::Clear_Works
[PASS] LCD::SetCursor_Works
[PASS] LCD::SendString_SmokeTest
<<< End: LCD

[INFO] === INTEGRATION TESTS ===

>>> Suite: Integration
-------------------------------------------
[SKIP] Integration::Keypad_LCD_AsteriskDisplay | No key pressed within timeout
[PASS] Integration::ADC_LCD_TimeoutDisplay
[INFO] UART Protocol tests require Control ECU connection
[PASS] Integration::UART_Protocol_CheckInit
[PASS] Integration::UART_Protocol_SetPassword
[PASS] Integration::UART_Protocol_VerifyPassword
[PASS] Integration::UART_Protocol_SetTimeout
[PASS] Integration::UART_Protocol_GetTimeout
<<< End: Integration

[INFO] === SYSTEM TESTS ===

>>> Suite: System
-------------------------------------------
[INFO] Scenario: First Boot - Checking initialized state
[INFO] System already initialized
[PASS] System::FirstBoot_NotInitialized
[INFO] Scenario: Setup new password
[INFO] Password set successfully
[INFO] System now initialized
[PASS] System::FirstBoot_SetupPassword
[INFO] Scenario: Open door with valid password
[INFO] Password verified - sending OPEN command
[INFO] Door OPEN command acknowledged
[PASS] System::ValidPassword_DoorOpens
[SKIP] System::ValidPassword_DoorCloses | Long duration test - run manually
[PASS] System::SetTimeout_FromPot
[SKIP] System::SetTimeout_Persistence | Requires manual reboot
[PASS] System::WrongPassword_Lockout
[SKIP] System::Lockout_Duration | Long duration test - run manually
<<< End: System

===========================================
                  TEST SUMMARY
===========================================
  Total Tests : 28
  Passed      : 24
  Failed      : 0
  Skipped     : 4
  Duration    : 8432 ms
===========================================
  *** ALL TESTS PASSED ***
===========================================
```

---

## 10. Troubleshooting

| Symptom | Possible Cause | Solution |
|---------|----------------|----------|
| No UART output | Wrong baud rate | Set terminal to 9600 |
| Tests hang | Blocking call | Check timeout values |
| All UART protocol tests fail | ECUs not connected | Verify UART wiring |
| ADC tests fail | No potentiometer | Connect pot to PE3 |
| LCD tests fail | I2C not working | Check I2C wiring |

---

## 11. Revision History

| **Version** | **Date**       | **Author**   | **Description**           |
|-------------|----------------|--------------|---------------------------|
| 1.0         | December 2024  | —            | Initial test framework    |

---

<div align="center">

**— End of Document —**

</div>
