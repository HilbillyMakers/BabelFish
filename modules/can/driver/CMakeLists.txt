add_library(mcp2515_spi_lib
        MCP2515_can.c
        MCP2515_can.h
        icDefinitions.h
        canDefinitions.h
)

target_link_libraries(mcp2515_spi_lib PUBLIC
        pico_stdlib
        hardware_spi
        hardware_dma
)

