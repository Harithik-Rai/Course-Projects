# Multi-threaded Railway Track Simulator

## Overview
This project simulates a railway track system using **multi-threading in C**. It models how multiple trains share a single track by using synchronization primitives to prevent collisions and ensure efficient scheduling. The simulator uses **mutexes, condition variables, and interrupt-driven counters** to manage timing and events.

The project overall demonstrates key concepts in **concurrency, synchronization, and embedded-style system programming**, making it a strong example of systems-level coding.

## Features
- **Train Scheduling** – Handles multiple trains with proper timing and order.  
- **Concurrency Control** – Uses mutexes and condition variables to avoid race conditions.  
- **Timer-Driven Simulation** – Interrupts simulate delays and periodic checks.  
- **LED State Control** – Hardware-inspired signals (LEDs) represent train or track states.  
- **Event Logging** – Provides detailed logs of train arrivals, departures, and idle states.  

## Tech Stack
- **C** (POSIX Threads / AVR timers)  
- **Interrupts & Timers** for event-driven control  
- **Embedded System Concepts** (registers, ports, delays)
