#[[++

Copyright (C) 2019 PrimeDevelopers

All rights reserved.

This file has been generated by the Automatic Component Toolkit (ACT) version 1.7.0-develop.

Abstract: This is an autogenerated CMakeLists file for the development of LibS7Net

Interface version: 1.2.0


]]

cmake_minimum_required(VERSION 3.5)

### The implementation of the LibS7Net Component
project(LibS7Net)

set (CMAKE_CXX_STANDARD 11)

# The location of autogenerated interfaces
set(CMAKE_CURRENT_AUTOGENERATED_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Interfaces)
set(CMAKE_CURRENT_OUTPUT_DIR ${PROJECT_BINARY_DIR}/../Resources)


set(LIBS7NET_SRC
	${CMAKE_CURRENT_AUTOGENERATED_DIR}/libs7net_dll.cpp
)

add_library (libs7net SHARED ${LIBS7NET_SRC})
# Do not prefix the binary's name with "lib" on Unix systems:
set_target_properties(libs7net PROPERTIES PREFIX "" IMPORT_PREFIX "" )
# The following two properties are crucial to reduce the number of undesirably exported symbols
set_target_properties(libs7net PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(libs7net PROPERTIES VISIBILITY_INLINES_HIDDEN ON)
# This makes sure symbols are exported
target_compile_options(libs7net PRIVATE "-D__LIBS7NET_EXPORTS")

target_include_directories(libs7net PRIVATE ${CMAKE_CURRENT_AUTOGENERATED_DIR})

set_target_properties(libs7net
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_CURRENT_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_CURRENT_OUTPUT_DIR}"
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_OUTPUT_DIR}"	
	OUTPUT_NAME "libs7net"	
)