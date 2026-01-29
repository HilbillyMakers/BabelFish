add_library(lib_canIf
    canHandlers.h
    canHandlers.c
)

target_link_libraries(lib_canIf PRIVATE 
    pico_stdlib 
    hardware_resets 
    hardware_irq
    hardware_pwm
    lib_pinConfigs
)