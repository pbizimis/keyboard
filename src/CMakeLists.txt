add_library(Main main.c)

add_subdirectory(config)
add_subdirectory(usb)
add_subdirectory(rotary)
add_subdirectory(oled)

target_link_libraries(Main PRIVATE USB ROTARY OLED)
target_link_libraries(Main PUBLIC pico_stdlib pico_unique_id pico_cyw43_arch_none hardware_pio)
