# Disneyland Line Simulation

## Overview
This project simulates a **lineup of people waiting to enter Disneyland**.  
It models common line behaviors:
- People entering the line normally.  
- People leaving the line one-by-one or in groups when they buy tickets.  
- People “bribing” their way into a premium position in the line.  

The simulation uses a custom line (queue-like structure) built with **Java lists**, along with exception handling for invalid operations. It also includes a `Person` class to represent each individual, and a `Test` class with a demonstration of the simulation.

## Features
- **Normal entry**: Add people to the back of the line.  
- **Ticket handling**: Remove one or multiple people from the front when they purchase tickets.  
- **Premium entry (bribe)**: Insert a person at a specific position in the line.  
- **Line tracking**: Get the number of people currently in line.  
- Exception handling for **empty lines** and **invalid positions**.  

## Tech Stack
- **Java**  
- **Custom exception handling** (`ListEmptyException`, `InvalidPositionException`)  
- **LinkedList-based line structure**
