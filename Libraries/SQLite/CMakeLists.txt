cmake_minimum_required(VERSION 3.8)
project(SQLite3
    VERSION   3.31.1
    LANGUAGES C
    )

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_library(${PROJECT_NAME} STATIC sqlite3.c)
set_target_properties(${PROJECT_NAME} PROPERTIES
    OUTPUT_NAME   sqlite3
    PUBLIC_HEADER sqlite3.h
    DEBUG_POSTFIX d
    )