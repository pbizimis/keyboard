cmake_minimum_required(VERSION 3.12)

include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(FAMILY rp2040)
set(BOARD pico_sdk)

add_executable(keyboard_firmware)

target_sources(keyboard_firmware PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.c
        ${CMAKE_CURRENT_LIST_DIR}/usb_descriptors.c
        )

# Make sure TinyUSB can find tusb_config.h
target_include_directories(keyboard_firmware PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(keyboard_firmware PUBLIC pico_stdlib pico_unique_id tinyusb_device tinyusb_board pico_cyw43_arch_none hardware_pio)

pico_add_extra_outputs(keyboard_firmware)
