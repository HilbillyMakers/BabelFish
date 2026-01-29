add_library(lib_usbIf
    tusb_config.h
    usb_descriptors.c
    usb_descriptors.h
    usb_interface.h
    usb_interface.c

    usb_types.h
    endpoint.h
    endpoint.c
)

# Make sure TinyUSB can find tusb_config.h
target_include_directories(lib_usbIf PUBLIC
        ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(lib_usbIf PRIVATE 
    pico_stdlib 
    hardware_resets 
    hardware_irq
    hardware_pwm
    pico_unique_id 
    tinyusb_device 
    tinyusb_board
    
    lib_pinConfigs
)