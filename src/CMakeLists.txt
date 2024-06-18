add_library(Main main.c)

add_subdirectory(config)
add_subdirectory(usb)
add_subdirectory(rotary)
add_subdirectory(oled)
add_subdirectory(flash)
add_subdirectory(halves)
add_subdirectory(keys)

target_link_libraries(Main PRIVATE USB ROTARY OLED FLASH HALVES KEYS)
target_link_libraries(Main PUBLIC pico_stdlib pico_unique_id pico_cyw43_arch_none hardware_pio)
