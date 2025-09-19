add_library(lib_usbIf
        usb_lowlevel.c
        usb_lowlevel.h
        usb_common.h
        )

target_link_libraries(lib_usbIf PRIVATE 
    pico_stdlib 
    hardware_resets 
    hardware_irq
)