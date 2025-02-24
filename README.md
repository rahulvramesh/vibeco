# Vibeco

A modern C++ application built with Qt6 framework, providing a robust and efficient desktop application experience.

## Overview

Vibeco is a desktop application that combines modern C++ with Qt6's powerful GUI capabilities. The project follows best practices in C++ development and provides a structured, maintainable codebase.

## Technology Stack

- **Programming Language:** C++ (Standard: C++20)
- **GUI Framework:** Qt6 (Components: Core, Quick, Widgets, Network)
- **Build System:** CMake (Minimum version 3.30)
- **Development Environment:** CLion/Qt Creator
- **Code Formatting:** Clang Format
- **Version Control:** Git

### External Libraries
- **Audio Processing:** PortAudio (for real-time audio capture and playback)
- **Global Hotkeys:** QHotkey (cross-platform hotkey support)
- **System Integration:** 
  - Native macOS shortcut handling
  - System tray integration
  - Audio device management

## Project Structure

```
vibeco/
├── src/                    # Source code directory
│   ├── core/              # Core application logic
│   ├── gui/               # GUI-related code
│   └── utils/             # Utility functions and helpers
├── external/              # External dependencies
├── third_party/           # Third-party libraries
├── resources/             # Application resources
├── fonts/                 # Font resources
├── tests/                 # Test files
├── docs/                  # Documentation
├── scripts/              # Build and utility scripts
├── cmake/                # CMake modules and configuration
├── macos/                # macOS specific files
└── CMakeLists.txt        # Main CMake configuration file
```

## Building the Project

### Prerequisites

- CMake (version 3.30 or higher)
- Qt6 (installed via Homebrew on macOS)
- C++20 compatible compiler
- PortAudio (for audio processing)
  ```bash
  # On macOS
  brew install portaudio
  ```

### Build Instructions

1. Clone the repository:
   ```bash
   git clone https://github.com/rahulvramesh/vibeco.git
   cd vibeco
   ```

2. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

3. Configure and build:
   ```bash
   cmake ..
   make
   ```

## Development

- The project uses `.clang-format` for consistent code formatting
- Follow the existing code structure and naming conventions
- Write tests for new functionality
- Document your code using appropriate comments

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request
