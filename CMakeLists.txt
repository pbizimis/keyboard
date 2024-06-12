cmake_minimum_required(VERSION 3.29)

include(pico_sdk_import.cmake)

project(pico_examples C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

set(FAMILY rp2040)
set(BOARD pico_sdk)

# Hacky solution to set the MAIN_HALF preprocessor definition only for the left half
# I am still looking for the best solution to this problem
# cmake .. -DPICO_BOARD=pico_w -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DHALF=RIGHT

if(NOT DEFINED HALF)
  message(FATAL_ERROR "HALF variable is not set. Use -DHALF=LEFT or -DHALF=RIGHT.")
endif()

if(HALF STREQUAL "LEFT")
  add_compile_definitions(MAIN_HALF)
  add_subdirectory(src)
  add_executable(keyboard_firmware_left)
  pico_set_linker_script(keyboard_firmware_left ${CMAKE_CURRENT_LIST_DIR}/memmap_custom.ld)
  target_link_libraries(keyboard_firmware_left PRIVATE Main)
  pico_add_extra_outputs(keyboard_firmware_left)
elseif(HALF STREQUAL "RIGHT")
  add_subdirectory(src)
  add_executable(keyboard_firmware_right)
  pico_set_linker_script(keyboard_firmware_right ${CMAKE_CURRENT_LIST_DIR}/memmap_custom.ld)
  target_link_libraries(keyboard_firmware_right PRIVATE Main)
  pico_add_extra_outputs(keyboard_firmware_right)
else()
  message(FATAL_ERROR "Invalid value for HALF. Use LEFT or RIGHT.")
endif()
