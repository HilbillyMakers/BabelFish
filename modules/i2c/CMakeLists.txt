add_library(lib_i2cIf
    endpoint.h
    endpoint.c
    i2cHandlers.h
    i2cHandlers.c
)

target_link_libraries(lib_i2cIf PRIVATE 
    pico_stdlib 
    hardware_resets 
    hardware_irq
    hardware_i2c
    lib_pinConfigs
)