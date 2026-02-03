# Queueing System Simulator

A C++/Qt desktop application for simulating queuing systems with multiple data sources, buffer memory, and processing devices. This simulator models computational systems where data packets from sources are processed through a buffer and service devices, useful for analyzing system performance under different configurations.

## Building

### Prerequisites
- Visual Studio 2022 with C++ support
- Qt 6.10.0
- CMake 3.10+

### Build Commands Example
```bash
# Configure the project
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.10.0\msvc2022_64"

# Build in Release mode
cmake --build build --config Release

# Deploy Qt dependencies
C:\Qt\6.10.0\msvc2022_64\bin\windeployqt.exe build\apps\gui\Release\sim_gui.exe
```

## Running

### GUI Application
Run `build/apps/gui/Release/sim_gui.exe` for the graphical interface where you can configure system parameters, watch real-time simulation with visual timeline, and view detailed analytics.

### CLI Tools
- `sim_cli.exe`: Command-line interface for batch simulations
- `sim_sweep.exe`: Configuration sweeper for finding optimal setups

## Features
- Real-time timeline visualization of packet flow
- Event calendar showing upcoming events and system state
- Performance analytics with comprehensive metrics
- Batch processing for multiple configurations
- Flexible adjustment of sources, intervals, buffer size, and device parameters

## Project Structure
```
libs/sim_core/     # Core simulation engine (C++)
apps/gui/          # Qt-based graphical interface
apps/cli/          # Command-line tools for batch processing
figures/           # Screenshots and diagrams
```

## Quick Start
1. Launch `sim_gui.exe`
2. Set up system: number of sources, buffer capacity, service devices
3. Click "Run" to start simulation
4. Watch the timeline visualization
5. Check analytics for performance metrics

## Screenshots
![Main Interface](report/figures/main_start.png)
Main simulation window showing timeline, event calendar, and live metrics.

![Analytics View](report/figures/analytics.png)
Post-simulation analytics with detailed performance breakdown.
