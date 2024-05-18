add_library(Main main.c)

add_subdirectory(usb)

target_link_libraries(Main PRIVATE USB)
target_link_libraries(Main PUBLIC pico_stdlib pico_unique_id pico_cyw43_arch_none hardware_pio)
