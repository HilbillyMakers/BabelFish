add_library(lib_canIf
    canHandlers.c
)

target_link_libraries(lib_canIf PRIVATE 
    pico_stdlib 
    hardware_resets 
    hardware_irq
    hardware_spi
    lib_pinConfigs
)