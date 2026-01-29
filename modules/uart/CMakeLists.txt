add_library(lib_uartIf
    uartHandlers.h
    uartHandlers.c
)

target_link_libraries(lib_uartIf PRIVATE 
    pico_stdlib 
    hardware_resets 
    hardware_irq
    hardware_pwm
    lib_pinConfigs
)