cmake_minimum_required(VERSION 3.12)
# Enable debug symbols
set(CMAKE_BUILD_TYPE Debug)
set(DCMAKE_C_FLAGS "-Og -g3")
set(DPICO_DEOPTIMIZED_DEBUG 1)

include(pico_sdk_import.cmake)
include(pico_extras_import_optional.cmake)

set(PROJECT_NAME BabelFish)

project(${PROJECT_NAME} C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(PICO_USE_STACK_GUARDS 1)

if (PICO_SDK_VERSION_STRING VERSION_LESS "2.0.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 2.0.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

pico_sdk_init()

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
)

message("Build type is ${CMAKE_BUILD_TYPE}")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
        if (PICO_DEOPTIMIZED_DEBUG)
        message("Using fully de-optimized debug build (set PICO_DEOPTIMIZED_DEBUG=0 to optimize)")
        else()
        message("Using regular optimized debug build (set PICO_DEOPTIMIZED_DEBUG=1 to de-optimize)")
        endif()
endif()

add_subdirectory(modules)

add_executable(${PROJECT_NAME} main.c)

target_link_libraries(${PROJECT_NAME} 
        pico_stdlib
        pico_time
        lib_usbIf
        lib_canIf
        lib_gpIf
        lib_i2cIf
        lib_spiIf
        lib_uartIf
)