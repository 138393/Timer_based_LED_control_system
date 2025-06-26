# Timer-Based LED Control System – MSP432 Microcontroller (C)

This project implements a **real-time LED control system** on the **MSP432 microcontroller**, using two buttons and timer-based interrupts to control 2 individual LED behaviors, including both a simple LED and a multi-state RGB LED. The system is written in **C** and designed to demonstrate precise real-time response using **hardware timers**, **GPIO interrupts**, and the **Nested Vector Interrupt Controller (NVIC)**.

---

## Features

- **Two-button interface**:
  - **Button 1 (P1.1)**: Toggles between **single LED mode** and **RGB LED mode**, retaining the last state
  - **Button 2 (P1.4)**: Cycles through three timing modes:
    - **Regular Mode** (fast blinking)
    - **Slow Mode** (slower blinking)
    - **Paused Mode** (remain at the current state)

- RGB LED shows different color patterns in a cycle of 8 combinations
- Uses **hardware Timer A** to generate periodic interrupts for blinking/dimming
- All control is **interrupt-driven** (no delay-based polling)

---

## How It Works

- **GPIO Configuration**: P1.0 used for single LED; P2.0–2.2 used for RGB output.
- **Timer A0** is used in up-counting mode with preloaded CCR0 values to create different blinking rates.
- **PORT1_IRQHandler**:
  - Handles button presses and debounces inputs.
  - Button 1 toggles the LED mode.
  - Button 2 cycles between regular, slow, and pause modes, reconfiguring the timer accordingly.
- **TA0_N_IRQHandler**:
  - Fires at intervals defined by `TA0CCR0` and controls either the P1.0 LED or RGB LED color changes depending on the current mode.

---

## Technologies Used

- **MSP432 LaunchPad**
- **C Language**
- **Timer A**, **NVIC**, **GPIO interrupts**
- **Bitwise operations** for efficient I/O manipulation
  
---
