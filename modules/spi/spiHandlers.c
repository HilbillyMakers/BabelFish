#include "hardware/spi.h"
#include "../devicePins/device_pins.h"
#include "pico/stdlib.h"
#include <stdio.h>
#include <string.h>

///TODO: 
/* 
    Prototype SPI pins are: GP10 - GP13 (pinout SPI0)
    Future implementation: user is allowed to customize pin usage,
    need to check for conflicts and coherence
 */  

//default pins for SPI peripheral
#define PROTOTYPE_SPI       spi0
#define SPI_SCK_PIN         10 
#define SPI_TX_PIN          11 
#define SPI_RX_PIN          12
#define SPI_CSN_PIN         13

//comment above and uncomment these when testing CAN communication through SPI
/*
#define PROTOTYPE_SPI       spi0
#define SPI_SCK_PIN         18
#define SPI_TX_PIN          19 
#define SPI_RX_PIN          20
#define SPI_CSN_PIN         21
*/

///TODO: 
/*
    This functionality can be common to multiple protocols
    Should move in a common generic header file
*/
#define SLAVE_SELECTED      0
#define SLAVE_UNSELECTED    1

#define DEFAULT_REPEATED_TX_DATA 0

#define BYTE_1              0
#define BYTE_2              8
#define BYTE_3              16
#define BYTE_4              24

enum
{
    SPI_INIT,
    SPI_DEINIT,
    SPI_TRANSFER
};

uint8_t com_spi_init        (const uint8_t* configuration,  uint8_t* responseBuffer, uint8_t* errorBuffer);
uint8_t com_spi_deinit      (                               uint8_t* responseBuffer, uint8_t* errorBuffer);
uint8_t com_spi_transfer    (const uint8_t* hostMessage,    uint8_t* responseBuffer, uint8_t* errorBuffer);


uint8_t com_spi_commandStringHandler (const uint8_t *commandString, uint8_t commandStringLength, uint8_t *responseBuffer, uint8_t *errorBuffer)
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

uint8_t com_spi_init        (const uint8_t* configuration,  uint8_t* responseBuffer, uint8_t* errorBuffer)
{

    uint32_t baudRate              =    0u;
    uint32_t physicalBaudRate      =    0u;
    
    // Reassemble our uint32_t baudRate from 4 uint8_t variables
    baudRate                       =    configuration[0]          << BYTE_1 |
                                      ((configuration[1]) & 0xff) << BYTE_2 |
                                      ((configuration[2]) & 0xff) << BYTE_3 |
                                      ((configuration[3]) & 0xff) << BYTE_4;

    physicalBaudRate               =    spi_init(PROTOTYPE_SPI, baudRate);
    *((uint32_t *)responseBuffer)  =    physicalBaudRate;

    gpio_set_function   (SPI_RX_PIN,    GPIO_FUNC_SPI);
    gpio_set_function   (SPI_TX_PIN,    GPIO_FUNC_SPI);
    gpio_set_function   (SPI_SCK_PIN,   GPIO_FUNC_SPI);

    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_init           (SPI_CSN_PIN);
    gpio_set_dir        (SPI_CSN_PIN,   GPIO_OUT);
    gpio_put            (SPI_CSN_PIN,   SLAVE_UNSELECTED);

    return sizeof(physicalBaudRate);
}

uint8_t com_spi_deinit      (uint8_t* responseBuffer, uint8_t* errorBuffer)
{
    spi_deinit(PROTOTYPE_SPI);

    char deinit_response[] = "SPI Deinit Successful";
    memcpy(responseBuffer, deinit_response, sizeof(deinit_response));

    return sizeof(deinit_response);
}

uint8_t com_spi_transfer    (const uint8_t* hostMessage,   uint8_t* responseBuffer, uint8_t* errorBuffer)
{
    const uint8_t     txSize    =   hostMessage[0];
    const uint8_t     rxSize    =   hostMessage[1];
    const uint8_t    *txMessage =  &hostMessage[2];
    int8_t      receivedBytes   =   0u;     

    //Chip Select
    gpio_put                (SPI_CSN_PIN,   SLAVE_SELECTED);
    //Send request to slave
    spi_write_blocking      (PROTOTYPE_SPI, txMessage, txSize);
    //Receive data from slave
    receivedBytes   = \
        spi_read_blocking   (PROTOTYPE_SPI, DEFAULT_REPEATED_TX_DATA, responseBuffer, rxSize);
    //Chip Deselect
    gpio_put                (SPI_CSN_PIN,   SLAVE_UNSELECTED);

    return receivedBytes;
}
