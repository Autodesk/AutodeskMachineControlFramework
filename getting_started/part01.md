| [Prev](index.html) | [Getting Started -- Table of Contents](index.html) | [Next](part02.html) |
# Part 1:  How to clone, build and run AMCF

To get AMC built and running follow the steps of readme on the Github repository (see "View on GitHub").

The steps below describe the process to create a plugin on a Windows installation.

## Dependencies / Prerequisites (Windows)
-	Visual Studio C/C++
-	GoLang - [GoLang](https://golang.org/doc/install)
-	64-MinGW Compiler - e.g [TDM-GCC](https://jmeubank.github.io/tdm-gcc/)
-	cmake - [cmake](https://cmake.org/download/)
-	nodejs - [nodejs](https://nodejs.org/en/download/)

## Building & Running AMCF
-	Clone the AMCF GitHub to your local drive
-	Checkout wanted branch, FFF for example
-	Run the build_clean_win64 or build_clean_linux64 script 
-	Navigate to build/Output and run amc_server (you may have to change the file extension to .exe)
-	Open a web browser and go to 127.0.0.1:8869

## Editing with Visual Studio
-	Clone the AMCF GitHub to your local drive
-	Checkout wanted branch, FFF for example
-	Run the build_clean_win64 or build_clean_linux64 script
-	Navigate to AutodeskMachineControlFramework/build
-	Open LibMc.sln in Visual Studio
-	If you want to edit, compile and debug the plugin “Printer Connection”, then right click on the for example 'libmcplugin_printerconnection' project.
-	Set as Startup Project
-	Build the solution
-	Run the Local Windows Debugger
-	Open a web browser and go to 127.0.0.1:8869 to have a look around. If you want define another IP address and/or port, change the corresponding entries in the file “amc_server.xml” located in the Output subfolder.


