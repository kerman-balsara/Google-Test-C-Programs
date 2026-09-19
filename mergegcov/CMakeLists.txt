# Based on cmake --ver, we have 3.28.3
cmake_minimum_required(VERSION 3.25)

project(GoogleTestC VERSION 0.1.0 LANGUAGES C)

set (MY_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})

set (CMAKE_C_FLAGS "-Wall -g")

set(
    SRC_FILES

    ${MY_SOURCE_DIR}/mergegcov.c
)

add_executable(
    mergegcov
    ${SRC_FILES}
)
