add_library(lib_spiIf
    spiHandlers.h
    spiHandlers.c
)

target_link_libraries(lib_spiIf PRIVATE 
    pico_stdlib 
    hardware_resets 
    hardware_irq
    hardware_pwm
    lib_pinConfigs
)