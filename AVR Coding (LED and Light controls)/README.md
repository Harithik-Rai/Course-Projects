# AVR LED Control & Light Patterns

## Overview
This project demonstrates **embedded systems programming in C** using an **ATmega microcontroller**.  
It focuses on controlling LEDs with **precise timing** via hardware timers and interrupts.  
The program implements different lighting effects such as **SOS signaling**, **PWM-based glow**, and **pulsing brightness**.

The assignment was completed as part of **CSC Spring 2023**, reinforcing low-level C programming and real-time embedded development.

## Features
- **LED Control Function (`led_state`)**: Turn individual LEDs on/off via bit manipulation of I/O registers.  
- **SOS Signal (`SOS`)**: Blink LEDs in the Morse code pattern for "SOS" using timed delays.  
- **Glow (`glow`)**: Control LED brightness with **Pulse Width Modulation (PWM)** logic.  
- **Pulse Glow (`pulse_glow`)**: Create a smooth breathing/pulsing LED effect by dynamically adjusting brightness thresholds.  
- **Light Show (placeholder)**: Bonus section for creating custom multi-LED animations.  
- **Hardware Timer Integration**: Uses **Timer1 (1μs interrupts)** and **Timer3 (10ms interrupts)** for precision control.  

## Tech Stack
- **C (AVR-GCC)**  
- **ATmega microcontroller (AVR architecture)**  
- **AVR Libc (`avr/io.h`, `avr/interrupt.h`)**  
- **Hardware timers & interrupts**  
- **Bitwise operations for port manipulation**
