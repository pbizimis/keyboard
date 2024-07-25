add_library(KEYS keys.c debounce.c)

target_include_directories(KEYS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


target_link_libraries(KEYS PUBLIC KEYBOARD_CONFIG OLED USB pico_stdlib)
