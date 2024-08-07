#[[++

Copyright (C) 2020 Autodesk Inc.

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

]]


cmake_minimum_required(VERSION 3.5)

##########################################################################################
### Change the next line for making a new driver
##########################################################################################

set (DRIVERPROJECT S7Net)

include (../CMakeDriverCommon.txt)

##########################################################################################
### Add custom code below
##########################################################################################

IF(WIN32)
	target_link_libraries(${DRIVERNAME} setupapi.lib)
ENDIF(WIN32)

file(GLOB LIBS7COM_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/LibS7Com/Interfaces/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/LibS7Com/Implementation/*.cpp
)
target_sources(${DRIVERNAME} PRIVATE ${LIBS7COM_SRC})


target_include_directories(${DRIVERNAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/LibS7Com/Implementation)
target_include_directories(${DRIVERNAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/LibS7Com/Interfaces)
target_include_directories(${DRIVERNAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/LibS7Com/CppDynamic)
target_include_directories(${DRIVERNAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/LibS7Net/CppDynamic)

