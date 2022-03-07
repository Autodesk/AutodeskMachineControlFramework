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

project(LibOIE)

if(NOT APPLE)
  find_library(LIB_OPENSSL NAMES crypto ssl)
  if(LIB_OPENSSL)
    add_definitions(-DBRYNET_USE_OPENSSL)
    SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-as-needed -ldl -lrt -lssl -lcrypto")
  elseif()
    message("not found openssl")
  endif()
endif()


if(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
elseif(UNIX)
    if(NOT CMAKE_BUILD_TYPE)
      set(CMAKE_BUILD_TYPE "Release")
    endif()
    SET(CMAKE_CXX_FLAGS_DEBUG   "-O0 -g -ggdb -Wall -Wextra -D_DEBUG")
    SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -g -ggdb -Wall -Wextra -DNDEBUG")
  if (CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 7.0)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -fPIC")
endif()

file(GLOB LIBOIE_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/Implementation/*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Interfaces/*.cpp
)

add_library(liboie SHARED ${LIBOIE_SRC})
# Do not prefix the binary's name with "lib" on Unix systems:
set_target_properties(liboie PROPERTIES PREFIX "" IMPORT_PREFIX "" )
# The following two properties are crucial to reduce the number of undesirably exported symbols
set_target_properties(liboie PROPERTIES CXX_VISIBILITY_PRESET hidden)
set_target_properties(liboie PROPERTIES VISIBILITY_INLINES_HIDDEN ON)
# This makes sure symbols are exported
target_compile_options(liboie PRIVATE "-D__LIBOIE_EXPORTS")
target_include_directories(liboie PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Implementation)
target_include_directories(liboie PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Interfaces)
target_include_directories(liboie PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Libraries)

IF(WIN32)
	target_link_libraries(liboie Ws2_32.lib)
ENDIF(WIN32)

