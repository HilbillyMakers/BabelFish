#include "spiHandlers.h"
#include "hardware/spi.h"
#include "../devicePins/device_pins.h"
#include "pico/stdlib.h"
#include <stdio.h>

///TODO: 
/* 
    Prototype SPI pins are: GP18 - GP21 (pinout SPI0)
    Future implementation: user is allowed to customize pin usage,
    need to check for conflicts and coherence
 */  

#define PROTOTYPE_SPI       spi0
#define SPI_SCK_PIN         18 
#define SPI_TX_PIN          19 
#define SPI_RX_PIN          20
#define SPI_CSN_PIN         21

#define SLAVE_SELECTED      0
#define SLAVE_NOT_SELECTED  1

enum
{
    SPI_INIT,
    SPI_DEINIT,
    SPI_TRANSFER
};

uint8_t com_spi_init        (uint8_t* const configuration,  uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t com_spi_deinit      (                               uint8_t* const responseBuffer, uint8_t* const errorBuffer);
uint8_t com_spi_transfer    (uint8_t* const hostMessage,    uint8_t* const responseBuffer, uint8_t* const errorBuffer);


uint8_t com_spi_commandStringHandler (uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
{
    uint8_t     command         =   commandString[0];
    uint8_t     responseSize    =   0u;

    switch(command)
    {
        case SPI_INIT:
            responseSize    = com_spi_init      (&commandString[1],  responseBuffer, errorBuffer);
        break;

        case SPI_DEINIT:
            responseSize    = com_spi_deinit    (                    responseBuffer, errorBuffer);
        break;

        case SPI_TRANSFER:
            responseSize    = com_spi_transfer  (&commandString[1],  responseBuffer, errorBuffer);
        break;
    }

    return responseSize;
}

uint8_t com_spi_init        (uint8_t* const configuration, uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{

    uint32_t baudRate               =   0u;
    uint32_t physicalBaudRate       =   0u;
    baudRate                        =   *((uint32_t *)configuration);

    physicalBaudRate                =   spi_init(PROTOTYPE_SPI, baudRate);
    *((uint32_t *)responseBuffer)   =   physicalBaudRate;

    gpio_set_function   (SPI_RX_PIN,    GPIO_FUNC_SPI);
    gpio_set_function   (SPI_TX_PIN,    GPIO_FUNC_SPI);
    gpio_set_function   (SPI_SCK_PIN,   GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init           (SPI_CSN_PIN);
    gpio_set_dir        (SPI_CSN_PIN,   GPIO_OUT);
    gpio_put            (SPI_CSN_PIN,   SLAVE_NOT_SELECTED);

    return sizeof(physicalBaudRate);
}

uint8_t com_spi_deinit      (uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    spi_deinit(PROTOTYPE_SPI);

    return 0;
}

uint8_t com_spi_transfer    (uint8_t* const hostMessage,    uint8_t* const responseBuffer, uint8_t* const errorBuffer)
{
    uint8_t     txSize          =   hostMessage[0];
    uint8_t     rxSize          =   hostMessage[1];
    uint8_t    *txMessage       =  &hostMessage[2];
    int8_t      receivedBytes   =   0u;     
    //chip select
    gpio_put            (SPI_CSN_PIN,   SLAVE_SELECTED);
    //send request to slave
    spi_write_blocking  (PROTOTYPE_SPI, txMessage, txSize);
    //receive data from slave
    receivedBytes   = spi_read_blocking(PROTOTYPE_SPI, 0, responseBuffer, rxSize);
    //chip deselect
    gpio_put            (SPI_CSN_PIN,   SLAVE_NOT_SELECTED);

    return receivedBytes;
}