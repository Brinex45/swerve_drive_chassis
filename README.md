# Swerve Drive Chassis

**Arduino/Teensy-based swerve drive chassis with field-oriented control, IMU heading hold, and PS4 controller input.**

This project implements a full swerve drive system where each wheel module can independently rotate to any angle while simultaneously driving at any speed. The system supports both a **4-wheel** and a **3-wheel** swerve configuration and communicates with ODrive motor controllers over UART for BLDC (drive) motor velocity control.

---

## What is Swerve Drive?

In a swerve drive chassis, each wheel module has two independent motors:

- A **DC motor** (via BTS driver) that rotates the wheel module to any angle (steering)
- A **BLDC motor** (via ODrive) that spins the wheel for propulsion (drive)

This allows the chassis to move in any direction while maintaining any orientation — translation and rotation are fully independent.

---

## Repository Structure

```
swerve_drive_chassis/
├── main/
│   ├── my_swerve_teensy.ino        # 4-wheel swerve — Teensy (basic, no IMU)
│   └── Swerve_off.ino              # 4-wheel swerve — Teensy (with BNO055 IMU + heading hold PID)
├── 3_wheel_swerve_teensy_/
│   └── 3_wheel_swerve_teensy_.ino  # 3-wheel swerve — Teensy (field-oriented + heading hold)
├── 3_wheel_swerve_esp/
│   └── 3_wheel_swerve_esp.ino      # ESP32 — PS4 Bluetooth receiver (sends data over I2C)
└── Test_codes/
    ├── enco_closed.ino             # Encoder closed-loop angle control test
    ├── module_swerve.ino           # Single module swerve test
    ├── pos_swerve.ino              # Position-based swerve test
    ├── cordinate_swerve.ino        # Coordinate-based movement test
    ├── positionControlLogic.ino    # ODrive position control logic test
    ├── 4_odrive.ino                # 4 ODrive UART communication test
    └── Swerve_Teensy.ino           # Early Teensy swerve prototype
```

---

## System Architecture

```
PS4 Controller (Bluetooth)
         │
    ESP32 (PS4Controller lib)
         │  I2C (Wire)
    Teensy 4.1 (Main Controller)
    ┌────────────────────────┐
    │  BNO055 IMU (I2C)      │  ← Heading / orientation
    │  3–4x Encoders         │  ← Module steering angle feedback
    │  3–4x BTS Motor Drivers│  ← DC motors (module rotation / steering)
    │  3–4x ODrive (UART)    │  ← BLDC motors (wheel drive velocity)
    └────────────────────────┘
```

**Communication chain:**
- PS4 controller connects over Bluetooth to the ESP32
- ESP32 reads joystick axes (leftX, leftY, rightX for rotation) and packs them into bytes
- ESP32 acts as I2C slave (address `0x08`); Teensy requests data each loop cycle
- Teensy runs swerve kinematics, PID, and sends commands to all motors

---

## Hardware

| Component | Role |
|---|---|
| Teensy 4.1 | Main controller — swerve kinematics, PID, motor commands |
| ESP32 | PS4 Bluetooth receiver — sends joystick data to Teensy via I2C |
| ODrive (×3 or ×4) | BLDC motor controllers — closed-loop velocity control over UART |
| BTS7960 (×3 or ×4) | DC motor H-bridge drivers — module steering |
| Quadrature Encoders (×3 or ×4) | Steering angle feedback for each module |
| Adafruit BNO055 | 9-DOF IMU — heading hold and field-oriented control |

---

## Features

**Swerve Kinematics**
- Vector decomposition: translational vector (joystick XY) and rotational vector (right stick) are combined per module using trigonometry
- Shortest-path steering optimization — each module always takes the shortest arc to its target angle, reversing drive direction if needed to avoid rotating more than 90°

**Heading Hold PID**
- BNO055 measures current robot heading
- When the driver releases the rotation stick, the current heading is locked as the target
- Angular PID (kp, ki, kd tunable) continuously corrects for drift and disturbances
- Statutory PID mode: when heading error is small, correction is applied as proportional wheel velocities rather than triggering full module rotation

**Field-Oriented Control (3-wheel version)**
- Translation direction is corrected by IMU heading
- Pushing forward on the joystick always moves the robot in the same global direction, regardless of robot body orientation

**Module Angle Reset**
- PS4 touchpad button triggers a reset routine to return all modules to zero angle

---

## Configurations

### 4-Wheel — `main/my_swerve_teensy.ino`
Basic 4-module swerve with no IMU. PS4 left stick controls translation, right stick X controls rotation. Rotational vectors for the four modules point at 45° / 135° / 225° / 315°.

### 4-Wheel with IMU — `main/Swerve_off.ino`
Full heading hold PID using BNO055, vector addition per module, statutory PID for fine correction, ODrive closed-loop velocity mode.

### 3-Wheel — `3_wheel_swerve_teensy_.ino`
3-module omni swerve with modules at 270° / 150° / 30° (120° apart). Full field-oriented control with BNO055, improved angular PID with derivative term, per-module odometry coordinate tracking (in development), touchpad reset support.

### ESP32 PS4 Receiver — `3_wheel_swerve_esp/`
Connects to PS4 over Bluetooth. Reads all axes and buttons with configurable bitmask selection (press vs click mode configurable per-button via `#define`). Sends selected data as I2C slave to Teensy. Handles disconnect gracefully by zeroing all outputs.

---

## Libraries Required

Install in Arduino IDE (Library Manager or manual):

| Library | Purpose |
|---|---|
| `Encoder` | Quadrature encoder reading (Teensy) |
| `ODriveUART` / `ODriveSwerve` | ODrive UART communication |
| `Adafruit BNO055` | IMU driver |
| `Adafruit Unified Sensor` | Dependency for BNO055 |
| `Wire` | I2C — built-in |
| `PS4Controller` | PS4 Bluetooth on ESP32 |

---

## Pin Mapping — Teensy 4.1 (4-wheel)

**BTS Motor Drivers (steering DC motors)**

| Module | R_PWM | L_PWM |
|--------|-------|-------|
| 1 | 9 | 8 |
| 2 | 6 | 7 |
| 3 | 4 | 5 |
| 4 | 2 | 3 |

**Encoders (steering angle feedback)**

| Module | Pin A | Pin B |
|--------|-------|-------|
| 1 | 26 | 27 |
| 2 | 33 | 32 |
| 3 | 38 | 37 |
| 4 | 31 | 30 |

**ODrive UART (BLDC drive motors)**

| Module | Serial Port |
|--------|-------------|
| 1 | Serial1 |
| 2 | Serial5 |
| 3 | Serial3 |
| 4 | Serial8 |

**I2C:** Wire1 (Teensy) ↔ Wire (ESP32, slave address `0x08`)

**IMU:** Wire2 at address `0x28`

---

## ODrive Setup

Each ODrive must be in **closed-loop velocity control** mode. On startup the Teensy waits for and enables this state:

```cpp
while (odrive.getState() != AXIS_STATE_CLOSED_LOOP_CONTROL) {
    odrive.clearErrors();
    odrive.setState(AXIS_STATE_CLOSED_LOOP_CONTROL);
    delay(10);
}
```

Each module's encoder CPR (counts per full revolution) is calibrated individually and stored as a constant (e.g. `cpr1 = 466104`). Recalibrate by slowly rotating a module exactly one full turn and reading the encoder tick count.

---

## Upload Instructions

1. Select the correct board in Arduino IDE:
   - **Teensy 4.1** → all sketches in `main/` and `3_wheel_swerve_teensy_/`
   - **ESP32** → `3_wheel_swerve_esp/`
2. Install all required libraries
3. Upload ESP32 sketch first and pair the PS4 controller
4. Then upload the Teensy sketch
5. Open Serial Monitor at **115200 baud** for debug output

---

## Test Codes

The `Test_codes/` folder contains standalone development sketches:

| File | Purpose |
|---|---|
| `enco_closed.ino` | Closed-loop angle control on a single encoder + BTS driver |
| `module_swerve.ino` | Single swerve module angle + velocity test |
| `pos_swerve.ino` | Position-based swerve angle control |
| `cordinate_swerve.ino` | Coordinate-based movement prototype |
| `positionControlLogic.ino` | ODrive position mode testing |
| `4_odrive.ino` | UART communication with all 4 ODrives |
| `Swerve_Teensy.ino` | Early swerve prototype |

---

## Notes

- `Swerve_off.ino` is a legacy name from development — it is the most complete 4-wheel version with IMU, not a disabled version
- The 3-wheel odometry coordinate tracking code is present but commented out, still under development
- Each encoder's CPR must be individually calibrated — values differ slightly per module due to mechanical variation
- The ESP32 must pair with the PS4 controller before the Teensy powers on, as the I2C slave address is registered during `setup()`

---

## Team

Developed by **Team Automatons** as part of the rover chassis development for International Rover Challenge.
