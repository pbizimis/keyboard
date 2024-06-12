add_library(HALVES uart.c)

target_include_directories(HALVES PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(HALVES PUBLIC OLED pico_stdlib hardware_uart hardware_irq)
