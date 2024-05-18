add_library(USB setup.c usb_descriptors.c hid.c)

target_include_directories(USB PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(USB PUBLIC pico_stdlib pico_unique_id tinyusb_device
                                 tinyusb_board)
