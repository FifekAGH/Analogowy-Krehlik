cmake_minimum_required(VERSION 3.24)

project(precise_ammeter LANGUAGES CXX C ASM)

include(FetchContent)
include(cmake/requirements.cmake)

add_subdirectory(bsp/${BOARD_NAME})
add_subdirectory(drivers)
add_subdirectory(src)
