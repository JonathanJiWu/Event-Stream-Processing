# Event Stream Processing

## Overview
This project demonstrates event stream processing in a flight simulation context using modern C++20 and the Raylib graphics library. It simulates a log of timestamped events (sensor readings, actuator commands, control inputs), and provides interactive tools to sort, filter, group, and aggregate these events.

## Features
- **SimulationEvent struct**: Represents a discrete event with timestamp, type, source, and value.
- **Sorting**: Sort events by timestamp (ascending/descending).
- **Filtering**: Filter events by type (sensor, control, actuator).
- **Grouping**: Group events by source or type.
- **Aggregation**: Compute total values for a given source.
- **First Event Search**: Find the first event after a given time threshold.
- **Raylib GUI**: Interactive buttons and log display using Raylib.

## Requirements
- **C++20** compiler (Visual Studio 2019+ recommended)
- **Raylib** library (v5.5.0 or compatible)
- NASA-style font file (`nasa.ttf`) in the project directory (or fallback to Raylib default font)

## How to Build
1. Install Raylib and ensure its include/lib paths are set in your project.
2. Open the solution in Visual Studio.
3. Build the project (F7 or __Build Solution__).
4. Run the executable.

## Usage
- On launch, a window displays the event log and interactive buttons.
- Use buttons to sort, filter, group, and compute aggregates.
- The log updates in real time based on your selection.

## Key Code Concepts
- **C++20 STL algorithms**: `std::ranges::sort`, `std::ranges::copy_if`, `std::accumulate`, etc.
- **Lambda expressions**: Used for custom filtering and aggregation.
- **Unordered maps**: For grouping events by source/type.
- **Raylib drawing functions**: For GUI and text rendering.

## File Structure
- `Event Stream Processing.cpp`: Main source file with all logic and GUI.
- `raylib.h`: Raylib header (external dependency).
- `nasa.ttf`: Font file (optional).

## Example Event
Timestamp: 5.0, Type: ACTUATOR_COMMAND, Source: flaps, Value: 15.0

## Troubleshooting
- If you see font errors, ensure `nasa.ttf` is present or Raylib's default font is used.
- For linker errors about runtime libraries, ensure all dependencies use the same runtime (see project properties).

## License
This project is for educational/demo purposes. Raylib is licensed under zlib/libpng.

## References
- [Raylib](https://www.raylib.com/)
- [C++20 STL Algorithms](https://en.cppreference.com/w/cpp/algorithm/ranges)
