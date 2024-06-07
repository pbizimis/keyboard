add_library(OLED oled.c)

target_include_directories(OLED PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(OLED PUBLIC pico_stdlib hardware_spi)
