# Overview

Brief Introduction to Autodesk Machine Control Framework (AMCF)

## Concept

Currently, Autodesk Manufacturing software is not directly controlling the hardware. Users use various proprietary software from the machine builders to manufacture parts. Autodesk Machine Control (AMC) is an open source middleware framework to integrate CAD/CAM software with machine hardware systems into a production-ready, complete and cohesive closed loop system, that can produce high quality products out of the box. AMC is an Android equivalent open source framework for any manufacturing machines/robots and it reduces the costs and time-to-market for any machine builder in a similar way to what Android did for the mobile phone industry during the last decade. Any engineering organization can quickly combine the CAD/CAM stack together with any manufacturing hardware to create beyond-state-of-the-art industrial manufacturing system with a fraction of the traditional effort.
With ever-growing amounts of data flow between CAD/CAM/CAE software and manufacturing hardware systems in use throughout the Design and Make cycle, connecting them all is becoming more important than ever before. AMCF establishes direct connection between Design and Make. It provides machine control capability and allows the user to deliver a truly integrated end-to-end solution. 


## How it works

Middleware bridges the gap between two or more pieces of software that don’t natively communicate. Because middleware has this extremely important purpose, it’s critical that it works well. One of the most important capabilities for tools that perform a middleware function is to serve as a starting point for existing manufacturing hardware systems that need to be connected to the design software.

AMCF enables to bridge the gap between CAD/CAM software and machine hardware, by a modern browser-based Human Machine Interface, configurable Open APIs and unseen features like over-the-air updates and transactional timeline journaling.

Most importantly, all these features are within a single unified framework and can be applied to any industrial machinery or robots. Developers do not have to take the time to develop their own software for controlling machines, learn new tools or switch between environments — everything needed to control the machine is all in a single place with a direct connection to CAD/CAM software stack.


![AMCF Concept](doc/images/AmcfConcept.png "AMC connects CAD/CAM with any machine hardware and creates integrated closed loop systems")

Manufacturing CAD/CAM software is generating design and simulation workflows and outputs build files. These build files are then imported to AMC for to convert toolpath in to control logic and hardware control. AMC runs on the machine and directly controls the hardware by sending the control logic to the controller. Machine controller in turn runs real time machine control while recording all the process data.

AMC allows sending the control commands to the machines. It also allows receiving signals for data recording, process monitoring, and creating Digital Twins. This in turn provides data to CAD/CAM system for inspection by creating a 1st closed loop feedback to design. Another option is to send 2nd closed loop feedback within AMC for quicker in-process control changes and adjustments. The 3rd closed loop feedback can be created within the controller for deployment of the quickest actions.


![AMCF OpenSource](doc/images/AmcfOpenSource.png "AMC has been released as an open source code for unrestricted commercial use")

AMC has been developed as fully open source code with unrestricted commercial use and can be found at the official Autodesk GitHub account: 

[AMCF on GitHub](https://github.com/Autodesk/AutodeskMachineControlFramework) 


![AMCF Block Diagram](doc/images/AmcfBlockDiagram.png "Modern client server architecture of AMC")


At the core of AMC is the modern client server architecture, where different external applications can be connected as clients, for example: web-based HMI client, API access, Fusion 360, Forge data platform. Main server is running on the machine and connects clients to the Core system. The Core system consists of number of state machines that can accommodate and run any plugins and any drivers for any machines: additive machines, subtractive machines, robots, laser cutters, and any other industrial machine





