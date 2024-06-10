cmake_minimum_required(VERSION 3.29)

include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(FAMILY rp2040)
set(BOARD pico_sdk)

add_subdirectory(src)

add_executable(keyboard_firmware)

pico_set_linker_script(keyboard_firmware ${CMAKE_CURRENT_LIST_DIR}/memmap_custom.ld)

target_link_libraries(keyboard_firmware PRIVATE Main)

pico_add_extra_outputs(keyboard_firmware)
