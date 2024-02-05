
# Introduction to the Autodesk Machine Control Framework

## Background information & Justification

The Autodesk Machine Control Framework is an open software collection that glues together the Upstream CAD/CAM data preparation pipeline with the hardware components that are sold in the market. As a middleware, it tries to 

## Open Source methodology

## License

Copyright (C) 2024 Autodesk Inc. 

All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
	* Redistributions of source code must retain the above copyright
	  notice, this list of conditions and the following disclaimer.
	* Redistributions in binary form must reproduce the above copyright
	  notice, this list of conditions and the following disclaimer in the
	  documentation and/or other materials provided with the distribution.
	* Neither the name of the Autodesk Inc. nor the
	  names of its contributors may be used to endorse or promote products
	  derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AUTODESK INC. BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

## Contributions

Contributions are welcome and we are looking for people that can improve existing language bindings or create new bindings or implementation stubs.

You can also contribute by reporting bugs in the Issue tracker on github, helping review pull requests, participate in discussions about issues and more.

### Filing issues
1. When filing an issue to report errors or problems, make sure to answer these five questions:
	1. Which version of the Autodesk Machine Controller Framework are you using?
	2. Which operating system, programming language(s) and development tools (compiler/interpreter) are you using?
	3. What did you do?
	4. What did you expect to see?
	5. What did you see instead?

2. When contributing to this repository, please first discuss the change you wish to make via issue with the [maintainers](#maintainers) of this repository. This way, we can ensure that there is no overlap between contributions or internal development work.

3. Adhere to the code of conduct..

4. Review, sign and send in either the "Autodesk Machine Control Framework Form Corp Contrib Agmt for Open Source.pdf" if you want to contribute as a corporation or the "Autodesk Machine Control Framework Form Ind Contrib Agmt for Open Source.pdf" (both files are in the main folder of this repository) if you want to contribute as an individual.

### Submitting a pull request
When ready to contribute, fork this repository and submit a pull request that references the issue it resolves. Be sure to include a clear and detailed description of the changes you've made so that we can verify them and eventually merge.

The Autodesk Machine Controller Framework follows the [git-flow](https://www.atlassian.com/git/tutorials/comparing-workflows/gitflow-workflow) branching model. New developments are integrated into the [develop](../../tree/develop)-branch. The maintainers will create releases from the develop-branch when appropriate.

If you've established yourself as an impactful contributor to the project, and are willing take on the extra work, we'd love to have your help maintaining it! Email the current maintainers for details.


# I The AMCF Core System

## 1. Engineering Principles and Design choices

### 1.1. Multi-platform, Multil-anguage, Multi-user Architecture

### 1.2. Modular Architecture and the need for a Client-Server system

### 1.3. Separation of Framework and Machine specialization

### 1.4. The Hourglass pattern and Stable ABIs

### 1.5. Plugin mechanism and choice of programming language

### 1.6. API First Design guidelines and best practices

### 1.7. Serialization principles, XML, JSON and binary storage

### 1.8. Resource handling and file access

### 1.9. Database backend

## 2. Engineering Lifecycle Considerations

### 2.1. Code degradation and successful development over many years

### 2.2. Using of git hashes in the core code

### 2.3. Stable APIs, Backwards compatibility and Data migration

### 2.4. Separation of Data and Logic and forward compatibilty.

### 2.5. Package management and 

### 2.6. CI/CD and proper build pipelines.

### 2.7. Test Framework

## 3. General Backend Architecture

### 3.1. Client Server Implementation

### 3.2. Finite State Machines

### 3.3. Signals and Concurrency

### 3.4. Driver Concept

### 3.5. Build data management

### 3.6. 3MF Toolpath and Metadata

### 3.7. Logging and Journaling

### 3.8. Determinism and Replay

### 3.9. Implementation constraints

### 3.10. Resource sharing and driver locks

### 3.11. Consuming proprietary SDKs

## 4. Machine specialization

### 4.1. The Configuration XML

### 4.2. Language Definitions

### 4.3. Permissions and Role models

### 4.4. Alert System

### 4.5. State machines, Variables and Signal Definitions

### 4.6. Plugin Mechanism

### 4.7. The API Environment

### 4.8. Essential StateEnvironment Methods

### 4.9. A minimal machine flow example

## 5. Advanced API Functionality



# II The AMCF User Interface System

## 1. Generic Concept

### 1.1. Pages, Dialogs and Modules

### 1.2. Overview of Widget system

### 1.3. Custom pages

### 1.4. User Interface Plugin Mechanism

### 1.5. The API Environment

### 1.6. Events and Essential UIEnvironment Methods

## 2. Module Documentation

### 2.1. Content Module

### 2.1.1 Paragraph Widget

### 2.1.2 Image Widget

### 2.1.3 Form Widget

### 2.1.4 ButtonGroup Widget

### 2.1.5 Chart Widget

### 2.1.6 Upload Widget

### 2.1.7 ParameterList Widget

### 2.1.8 BuildList Widget

### 2.1.9 AlertList Widget

### 2.2. LayerView Module

### 3.3. Logs Module

### 3.3. Graphic Module

### 3.4. GLScene Module

### 3.5. Grid Module 

### 3.6. Tabs Module 


# III Standard Driver Documentation

## 1. PLC and Motion Drivers

### 1.1. B&R Automation TCP/IP Driver

### 1.2. Beckhoff ADS Driver

### 1.3. Hilscher cifX Driver 

### 1.4 Siemens S7PLC Driver

### 1.5 Technosoft TML Driver

### 1.6. Aerotech A3200 Driver

### 1.7. Standa XIMC Driver

## 2. Scanner Drivers

### 2.1. SCANLAB RTC6 Driver

### 2.2. Raylase Driver

###  2.3. HA Laser Systems Driver

## 3. Open Communication Drivers

### 3.1. OPCUA Driver

### 3.2 TCP/IP Driver

### 3.2 UART Driver

### 3.3. GRPC Driver

### 3.4. Marlin Driver

## 4. Auxiliary Device Drivers

### 4.1. SCANLAB Open Interface Extension Driver

### 4.2. SCANLAB scanMotionControl Driver

### 4.3. FLIR Spinnaker Driver

### 4.4. Rasterizer Driver

### 4.5. Beckhoff BK9xxx Driver




# IV Development environment

## 1. C++ Compiler, CMake, Git

## 2. Setting up a Developer Environment on Windows

## 3. Setting up a Developer Environment on Linux x64

## 4. Setting up a Developer Environment on Linux ARM

## 5. Code Dependencies


# V Custom Driver Development

# VI Features

# VII Requirement Engineering and Future Roadmap

# VII Tutorials

# VIII AMCF REST API Documentation

