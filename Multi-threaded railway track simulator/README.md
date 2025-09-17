# Multi-threaded Railway Track Simulator

## Overview
This project simulates trains crossing a **single-track railway** using **multi-threading in C (POSIX threads)**. Each train is represented by a thread with a direction (East/West), priority (High/Low), and loading/crossing time.  

The simulator enforces **fair scheduling rules**:
- High-priority trains (uppercase `E` or `W` in input) get precedence over low-priority trains.  
- At most **two consecutive trains** can go in the same direction before switching.  
- Mutual exclusion ensures **only one train** occupies the track at a time.  

This project highlights concepts in **concurrency, synchronization, condition variables, and scheduling algorithms**.

## Features
- **Concurrent Train Threads** – Each train loads, queues, and crosses independently.  
- **Priority Scheduling** – High-priority trains are always favored.  
- **Fair Direction Rules** – After two consecutive trains in one direction, the track switches.  
- **Accurate Timing** – Simulated loading/crossing times with `usleep()` and real-time logging.  
- **Event Logging** – Simulation logs when trains are ready, on track, and off track.  

## Tech Stack
- **C** (POSIX threads)  
- **pthread_mutex_t** for mutual exclusion  
- **pthread_cond_t** for signaling/waiting  
- **Linked lists** for East/West queues  
- **Monotonic clock** for accurate simulation timing  
