add_library(FLASH flash.c)

target_include_directories(FLASH PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(FLASH PUBLIC OLED pico_stdlib hardware_flash hardware_sync)
