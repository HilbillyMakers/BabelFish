add_library(lib_usbPc
        usb_protocol.c
        usb_protocol.h
        messages.h
        )

target_link_libraries(lib_usbPc PRIVATE 
    pico_stdlib 
    hardware_resets 
    hardware_irq
)