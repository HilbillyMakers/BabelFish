add_library(lib_spiIf
    endpoint.h
    endpoint.c
    spiHandlers.h
    spiHandlers.c
)

target_link_libraries(lib_spiIf PRIVATE 
    pico_stdlib 
    hardware_resets 
    hardware_irq
    hardware_spi
    lib_pinConfigs
)