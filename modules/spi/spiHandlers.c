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


uint8_t com_spi_commandStringHandler (const uint8_t *commandString,  uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
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
