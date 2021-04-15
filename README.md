# Autodesk Machine Control Framework
The Autodesk Machine Control Framework is a solid, open machine controller interface powered by modern web technologies.

## Current features:
* Extensible C++ Plugin interface 

* Multiple (scripting) language-support is planned (e.g. Python, C++, Pascal, LUA, C#, JavaScript)

* Easy to access, change and configure

* Semantically versioned: multiple machine firmware states can be deployed on the same system and changed on the fly.

* Transactional: record full timeline of machine and replay and debug with millisecond resolution.

* Supports Simulation drivers for Unit-Testing

* Made for Git


## Documentation
TODO


## Dependencies / Prerequisites / Compilation (on Windows)
* Visual Studio C/C++ (the Community Edition is sufficient technically) TODO
* Get and install go https://golang.org/doc/install
* A 64-MinGW compiler for cgo (we recommend TDM-GCC: https://jmeubank.github.io/tdm-gcc/)
* Get and install cmake https://cmake.org/download/
* Get and install nodejs: https://nodejs.org/en/download/ with optional chocolatey install (will install Phython as well)
* build_clean_win64.bat should build the binaries and dev packages on Windows. The results are located in "build/Output".


## Dependencies / Prerequisites / Compilation (on x64-Linux)
* Get and install go binaries from https://golang.org/doc/install
* Get and install node binaries from https://nodejs.org/en/download/ 
* On x64 Debian, the following packages should be sufficient: 
    - build-essential
	- cmake
	- git
	- uuid-dev
* build_clean_linux64.sh should build the binaries and dev packages on Windows. The results are located in "build/Output".

## Dependencies / Prerequisites (on ARM Linux)
TODO

## Build and first run
Build as described above.
* run "amc_server.exe"
* connect a web browser to http://[your server name or IP]:[your port number]


## Contributing
The Autodesk Machine Control Framework is an open source project.
Contributions are welcome and we are looking for people that can improve existing functionality or create new integrations. Have a look at the [contributor's guide](CONTRIBUTING.md) for details.

## License and 3rd party acknowledgements
* Autodesk Machine Control Framework has a [BSD-3-Clause license](LICENSE.md)
* Autodesk Machine Control Framework uses these [3rd Party components](3RD_PARTY.md)

