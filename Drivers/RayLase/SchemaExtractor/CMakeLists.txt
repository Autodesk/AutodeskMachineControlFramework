cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(SchemaExtractor VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable
add_executable(SchemaExtractor SchemaExtractor.cpp)
target_link_libraries(SchemaExtractor ${CMAKE_CURRENT_SOURCE_DIR}/RAYLASE.SPICE3.Native.ClientLib.lib)