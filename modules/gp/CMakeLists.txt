add_library(lib_gpIf
    commandHandlers.h
    commandHandlers.c

    digitalHandlers/digitalHandlers.h
    digitalHandlers/digitalHandlers.c

    analogHandlers/analogHandlers.h
    analogHandlers/analogHandlers.c
    analogHandlers/analogHandlers_Types.h
    
    pwmHandlers/pwmHandlers.h
    pwmHandlers/pwmHandlers.c
)

target_link_libraries(lib_gpIf PRIVATE 
    pico_stdlib 
    hardware_resets 
    hardware_irq
    hardware_pwm
    hardware_adc
)