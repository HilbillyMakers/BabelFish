add_library(lib_usbIf
        usb_lowlevel.c
        usb_lowlevel.h

        com/endpoint.h
        com/endpoint.c
        com/commandHandlers.h
        com/commandHandlers.c
        com/canHandlers/canHandlers.h
        com/canHandlers/canHandlers.c
        com/i2cHandlers/i2cHandlers.h
        com/i2cHandlers/i2cHandlers.c
        com/spiHandlers/spiHandlers.h
        com/spiHandlers/spiHandlers.c
        com/uartHandlers/uartHandlers.h
        com/uartHandlers/uartHandlers.c

        gpio/endpoint.h
        gpio/endpoint.c
        gpio/digitalHandlers/digitalHandlers.h
        gpio/digitalHandlers/digitalHandlers.c
        gpio/analogHandlers/analogHandlers.h
        gpio/analogHandlers/analogHandlers.c
        gpio/pwmHandlers/pwmHandlers.h
        gpio/pwmHandlers/pwmHandlers.c
        gpio/commandHandlers.h
        gpio/commandHandlers.c

        usb_common.h
        usb_device_pins.h
        usb_device_pins.c
        )

target_link_libraries(lib_usbIf PRIVATE 
    pico_stdlib 
    hardware_resets 
    hardware_irq
    hardware_pwm
)