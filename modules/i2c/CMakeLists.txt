add_library(lib_i2cIf
    i2cHandlers.h
    i2cHandlers.c
)

target_link_libraries(lib_i2cIf PRIVATE 
    pico_stdlib 
    hardware_resets 
    hardware_irq
    hardware_pwm
    lib_pinConfigs
)