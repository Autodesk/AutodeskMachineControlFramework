# Repository Guidelines

## Project Structure & Module Organization
- `Implementation/` holds the core C++ server, API handlers, and unit-test sources.
- `Framework/` contains SDK headers and plugin interfaces; `SDK/` includes SDK build artifacts (including WASM).
- `Drivers/` hosts hardware/transport driver modules; `Examples/` provides sample systems and plugins.
- `Client/` contains the web UI sources; `Artifacts/` holds packaged client/API docs and build scripts.
- `Tests/` contains driver/system tests wired into CMake; `Documentation/` covers API and website docs.
- Build outputs are generated under `build_<platform>/Output` (for example `build_linux64/Output`).

## Build, Test, and Development Commands
- `./build_clean_linux64.sh` builds Linux x64 binaries and dev packages into `build_linux64/Output`.
- `./build_clean_rpi.sh` builds for Raspberry Pi (ARM) using `build_rpi/Output`.
- `./build_clean_win64.bat` builds Windows artifacts into `build_win64/Output`.
- A complete clean build is only necessary for major structural changes or the initial clean build; otherwise run `cmake --build .` from the build directory.
- After a build, run the server from the output folder, e.g. `./build_linux64/Output/amc_server`.

## Coding Style & Naming Conventions
- Follow existing C++ style: tabs for indentation, braces on new lines, and `CamelCase` types with `snake_case` or `m_`-prefixed members.
- Keep header/implementation pairs aligned (for example `amc_api_handler_build.hpp` + `.cpp`).
- Prefer minimal, local changes and keep platform-specific logic behind existing `#ifdef` patterns.

## Testing Guidelines
- Unit tests live in `Implementation/UnitTest` and build into `amc_unittest`.
- Driver/system tests are grouped under `Tests/<DriverName>Test`.
- Run unit tests from the build directory (for example `cd build_linux64 && ./Output/amc_unittest`) to ensure runtime libraries are found.
- After building, run executables from `build_<platform>/Output` (for example `./build_linux64/Output/amc_unittest`).
- Add tests alongside the module you changed and mirror existing test naming.

## Commit & Pull Request Guidelines
- Commit messages are short, sentence-case summaries; some include issue numbers like `(... #49)`.
- Use the git-flow model: open PRs against `develop`.
- Before coding, discuss changes in an issue and reference it in the PR description.
- Include clear change summaries and note any build/test commands run.
- Sign and submit the appropriate contributor agreement PDF in the repo root.
- Follow the project Code of Conduct in `CODE_OF_CONDUCT.md`.

## Security & Configuration Tips
- Build scripts read environment variables such as `AMCF_PACKAGE_REPOSITORY` for package uploads; avoid committing secrets.
- Generated packages are placed in `build_<platform>/DevPackage` and `build_<platform>/Artifacts`.
