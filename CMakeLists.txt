cmake_minimum_required(VERSION 3.13)

set(NAME tempus_pico_puzzle)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(pico_sdk_import.cmake)

project(${NAME} C CXX ASM)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project

# Set Ethernet chip
set(WIZNET_CHIP W5100S)

add_subdirectory(lib/tempus_pico_lib)

# Main firmware files
add_subdirectory(src)

add_compile_options(-Wall
    -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
    -Wno-unused-function # we have some for the docs that aren't called
    -Wno-maybe-uninitialized
    -Wno-psabi #https://github.com/nlohmann/json/issues/1861
)
