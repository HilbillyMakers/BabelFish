add_library(lib_pinConfigs
    device_pins.h
    device_pins.c
)

target_link_libraries(lib_pinConfigs PRIVATE 
    pico_stdlib 
)