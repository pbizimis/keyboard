add_library(ROTARY rotary.c)

target_include_directories(ROTARY PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(ROTARY PUBLIC pico_stdlib USB)
