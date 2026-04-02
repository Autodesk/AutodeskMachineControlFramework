# Autodesk Machine Control Framework

The Autodesk Machine Control Framework (AMCF) is an open, extensible machine controller platform designed for additive manufacturing and industrial automation. It provides a modern web-based interface, pluggable driver architecture, and comprehensive SDK support for building custom machine control solutions.

## Features

### Core Capabilities
- **State Machine System** - XML-defined state machines with C++ plugin support for custom logic
- **Plugin Architecture** - Extensible C++ plugin interface for integrating custom functionality
- **Driver System** - Modular hardware drivers for motion control, lasers, cameras, and communication protocols
- **REST API** - OpenAPI 3.0 compliant API for integration with external systems
- **Web UI** - Modern Vue.js-based user interface with 3D visualization and real-time monitoring

### Data & Control
- **Transactional Journaling** - Full timeline recording with millisecond resolution for replay and debugging
- **Build Job Management** - Native 3MF file support for additive manufacturing workflows
- **Telemetry** - Data series collection and storage for process monitoring
- **Access Control** - Role-based permission system
- **Alert System** - Configurable alerts and notifications

### Development
- **Simulation Drivers** - Built-in simulation support for unit testing and development
- **Semantic Versioning** - Multiple firmware versions can be deployed and switched on the fly
- **Git-Friendly** - Designed for version control workflows

## Supported Drivers

### Motion Control
- Aerotech A3200
- B&R Automation PLC
- Beckhoff BK9xxx
- Marlin (3D printer firmware)
- TML Motion Library
- XIMC Controllers

### Laser & Scanner
- SCANLAB RTC6 / SMC / OIE
- Raylase SP-ICE 3

### Vision & Cameras
- Basler Pylon
- FLIR Spinnaker
- OpenCV
- Windows Media Foundation

### Communication Protocols
- TCP/IP
- Serial/UART
- OPC UA
- gRPC
- Beckhoff ADS
- Siemens S7
- Hilscher CifX

## Project Structure

```
Implementation/     Core C++ server, API handlers, and unit tests
Framework/          SDK headers and plugin interfaces
SDK/                Client SDK with language bindings and WASM build
Drivers/            Hardware and communication driver modules
Examples/           Sample system implementations
Client/             Vue.js web UI sources
Documentation/      API docs, architecture, and configuration schemas
Tests/              Driver and system integration tests
Libraries/          Third-party libraries (lib3mf, SQLite, curl, OpenCV, etc.)
Artifacts/          Build outputs and packaging scripts
OpenAPI/            OpenAPI/Swagger UI for API documentation
```

## Building

### Prerequisites (All Platforms)
- CMake 3.x or later
- Node.js (for client UI builds)

### Windows (x64)

1. Install Visual Studio with C++ development tools
2. Install CMake: https://cmake.org/download/
3. Install Node.js: https://nodejs.org/
4. Run:
   ```batch
   build_clean_win64.bat
   ```
5. Outputs are in `build_win64/Output/`

### Linux (x64)

1. Install dependencies:
   ```bash
   sudo apt install build-essential cmake git uuid-dev libssl-dev
   ```
2. Install Node.js: https://nodejs.org/
3. Run:
   ```bash
   ./build_clean_linux64.sh
   ```
4. Outputs are in `build_linux64/Output/`

### Raspberry Pi (ARM)

1. Install the same dependencies as Linux x64
2. Run:
   ```bash
   ./build_clean_rpi.sh
   ```
3. Outputs are in `build_rpi/Output/`

### Incremental Builds

For incremental builds after the initial clean build:
```bash
cd build_<platform>
cmake --build .
```

## Running the Server

From the build output folder:

**Windows:**
```batch
cd build_win64\Output
amc_server.exe
```

**Linux/Raspberry Pi:**
```bash
cd build_linux64/Output
./amc_server
```

Connect a web browser to `http://[server-ip]:[port]` to access the web interface.

## Building Packages

### Client Distribution (Vue 2)
```bash
# Windows
Artifacts\build_client_clean.bat

# Linux
./Artifacts/build_client_clean.sh
```
Output: `Artifacts/clientdist/clientpackage_vue2.zip`

### Client Distribution (Vue 3)
```bash
# Windows
Artifacts\build_client_v3_clean.bat

# Linux
./Artifacts/build_client_v3_clean.sh
```
Output: `Artifacts/clientdist_v3/clientpackage_vue3.zip`

### API Documentation
```bash
# Windows
Artifacts\build_apidocs_clean.bat

# Linux
./Artifacts/build_apidocs_clean.sh
```
Output: `Artifacts/apidocsdist/apidocspackage.zip`

## Examples

The `Examples/` directory contains sample system implementations:

- **LPBFSystem** - Laser Powder Bed Fusion system with BuR PLC and ScanLab scanners
- **MinimalSystem** - Minimal example for getting started

## Documentation

- [API Documentation](Documentation/API/) - REST API reference (RST/Sphinx format)
- [Configuration Schema](Documentation/Configuration/) - XML configuration schema
- [OpenAPI Spec](OpenAPI/AMCF_OpenAPI.json) - OpenAPI 3.0 specification

## Contributing

The Autodesk Machine Control Framework is an open source project. Contributions are welcome and we are looking for people that can improve existing functionality or create new integrations. See the [contributor's guide](CONTRIBUTING.md) for details.

## License

- Autodesk Machine Control Framework is licensed under [BSD-3-Clause](LICENSE.md)
- See [3rd Party Acknowledgements](3RD_PARTY.md) for third-party component licenses
