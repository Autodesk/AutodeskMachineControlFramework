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


## Dependencies / Prerequisites (on Windows)
* Visual Studio C/C++ (the Community Edition is sufficient technically) TODO
* Get and install go https://golang.org/doc/install
* A 64-MinGW compiler for cgo (we recommend TDM-GCC: https://jmeubank.github.io/tdm-gcc/)
* Get and install cmake https://cmake.org/download/
* Get and install nodejs: https://nodejs.org/en/download/ with optional chocolatey install (will install Phython as well)


## Dependencies / Prerequisites (on x64-Linux)
* Get and install go https://golang.org/doc/install
* On x64 Debian, the following packages should be sufficient
"build-essential", "cmake", "nodejs", "git", "npm".


## Dependencies / Prerequisites (on ARM Linux)
TODO

## Build and first run
* Clone the Git repository to your local machine
* run "build_clean.bat"
* (copy Output folders together - should rework that)
* Adjust config.xml, e.g.: (shoud change the defaults) [server hostname="127.0.0.1" port="8666"]  TODO
* run "mcserver.exe"
* connect a web browser to http://[your server name or IP]:[your port number]


## Contributing
The Autodesk Machine Control Framework is an open source project.
Contributions are welcome and we are looking for people that can improve existing functionality or create new integrations. Have a look at the [contributor's guide](CONTRIBUTING.md) for details.

## License and 3rd party acknowledgements
* Autodesk Machine Control Framework has a [BSD-3-Clause license](LICENSE.md)
* Autodesk Machine Control Framework uses these [3rd Party components](3RD_PARTY.md)

