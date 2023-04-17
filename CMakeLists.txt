# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
set(PICO_SDK_PATH "${CMAKE_SOURCE_DIR}/../pico-sdk")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)
#include(pico_extras_import.cmake)

project(picoisauart
    VERSION "0.1"
    LANGUAGES C CXX ASM
)

# set(PICO_DEOPTIMIZED_DEBUG "1")
set(CMAKE_BUILD_TYPE "Release")
set(PICO_COPY_TO_RAM 1)
#set(SKIP_PICO_MALLOC 1)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(picoisauart picoisauart.cpp)
pico_set_program_version(picoisauart ${CMAKE_PROJECT_VERSION})

#add_compile_definitions(
#        PICO_DEFAULT_UART=1
#        #PICO_DEFAULT_UART_TX_PIN=28
#        #PICO_DEFAULT_UART_RX_PIN=-1
#        # USE_IRQ=1
#        USE_ALARM=1
#        PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=5000000
#)

target_sources(picoisauart PRIVATE)

target_link_libraries(
    picoisauart
    pico_stdlib
)

pico_enable_stdio_usb(picoisauart 1)
pico_enable_stdio_uart(picoisauart 0)

pico_add_extra_outputs(picoisauart)
